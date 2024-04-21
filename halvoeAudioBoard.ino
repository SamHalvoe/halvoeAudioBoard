#include <SdFat.h>
#include <AudioTools.h>
#include <AudioLibs/I2SCodecStream.h>
#include <AudioCodecs/CodecWAV.h>

#define SD_CS_PIN 5
#define SPI_CLOCK SD_SCK_MHZ(20)
#define SD_CONFIG SdSpiConfig(SD_CS_PIN, DEDICATED_SPI, SPI_CLOCK)

// I2C
#define SDA_PIN              21 // I2C Data,  Adafruit ESP32 S3 3, Sparkfun Thing Plus C 23
#define SCL_PIN              22 // I2C Clock, Adafruit ESP32 S3 4, Sparkfun Thing Plus C 22
#define I2C_SPEED        100000 // Clock Rate
#define WM8960_ADDR        0x1A // Address of WM8960 I2C port

// I2S, your configuration for the WM8960 board
#define MCLK_PIN             -1 // Master Clock
#define BCLK_PIN             27 // Bit Clock
#define WS_PIN               26 // Word select
#define DO_PIN               25 // This is connected to DI on WM8960 (MISO)
#define DI_PIN               -1 // This is connected to DO on WM8960 (MOSI)

Pipeline audioPipeline;
AudioInfo audioInfo(44100, 2, 16); // sampling rate, # channels, bit depth
DriverPins audioPins;
AudioBoard audioBoard(AudioDriverWM8960, audioPins);
I2SCodecStream i2sOutStream(audioBoard);

FadeStream fadeStream;
VolumeStream volumeStream;

const char* soundFilename = "ddd4416.wav";
SdFat sdCard;
File soundFile;

WAVDecoder wavDecoder;
EncodedAudioStream encodedAudioStream(&wavDecoder);
StreamCopy audioCopier(audioPipeline, soundFile, 4096);

bool isPlaybackActive = false;

void setup()
{
  Serial.begin(115200);
  AudioLogger::instance().begin(Serial, AudioLogger::Warning);
  delay(2000);

  Serial.println("[Serial Ready]");
  Serial.println("---- SETUP BEGIN ----");

  if (not sdCard.begin(SD_CONFIG))
  {
    sdCard.initErrorHalt(&Serial);
  }

  if (not soundFile.open(soundFilename, FILE_READ))
  {
    Serial.print("Could not open file: \"");
    Serial.print(soundFilename);
    Serial.println("\"");
  }

  // setup i2c and i2s pins
  audioPins.addI2C(PinFunction::CODEC, SCL_PIN, SDA_PIN, WM8960_ADDR, I2C_SPEED, Wire);
  audioPins.addI2S(PinFunction::CODEC, MCLK_PIN, BCLK_PIN, WS_PIN, DO_PIN, DI_PIN);
  audioPins.begin();
  
  // setup i2s
  auto i2sConfig = i2sOutStream.defaultConfig();
  i2sConfig.copyFrom(audioInfo);
  i2sConfig.buffer_size = 2048;
  i2sOutStream.begin(i2sConfig);
  
  // set(up) volume (control)
  i2sOutStream.setVolume(1.0);
  volumeStream.setVolume(1.0);
  volumeStream.begin(audioInfo);

  // setup fade stream
  fadeStream.setFadeInActive(true);
  fadeStream.begin(audioInfo);
  
  // setup encoded stream
  encodedAudioStream.begin(audioInfo);

  // setup audio pipeline
  audioPipeline.add(encodedAudioStream);
  audioPipeline.add(volumeStream);
  audioPipeline.add(fadeStream);
  audioPipeline.setOutput(i2sOutStream);

  // setup audio copier
  audioCopier.begin();

  isPlaybackActive = true;

  Serial.println("---- SETUP END ----");
}

void loop()
{
  if (isPlaybackActive)
  {
    size_t copiedBytesCount = audioCopier.copy();

    if (copiedBytesCount == 0) // 0 bytes copied mean end of source
    {
      audioCopier.end();
      i2sOutStream.setMute(true);
      isPlaybackActive = false;
      Serial.println("playback finished");
    }
  }
}
