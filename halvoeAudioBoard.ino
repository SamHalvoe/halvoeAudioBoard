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

AudioInfo audio_info(44100, 2, 16);                    // sampling rate, # channels, bit depth
DriverPins audio_pins;                                 // board pins
AudioBoard audio_board(AudioDriverWM8960, audio_pins); // audio board
I2SCodecStream i2s_out_stream(audio_board);            // i2s coded
const char* soundFilename = "ddd4416.wav";
SdFat sdCard;
File soundFile;
WAVDecoder decoder;
EncodedAudioStream encoded_out_stream(&i2s_out_stream, &decoder);
StreamCopy audio_copier(encoded_out_stream, soundFile);

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
  audio_pins.addI2C(PinFunction::CODEC, SCL_PIN, SDA_PIN, WM8960_ADDR, I2C_SPEED, Wire);
  audio_pins.addI2S(PinFunction::CODEC, MCLK_PIN, BCLK_PIN, WS_PIN, DO_PIN, DI_PIN);
  audio_pins.begin();
  
  // setup i2s
  auto i2s_config = i2s_out_stream.defaultConfig();
  i2s_config.copyFrom(audio_info);
  i2s_config.buffer_size = 2048;
  i2s_out_stream.begin(i2s_config);

  // set volume
  i2s_out_stream.setVolume(0.75);

  // setup encoded stream
  encoded_out_stream.begin(audio_info);

  // setup audio copier
  audio_copier.resize(4096);
  audio_copier.begin();
  isPlaybackActive = true;

  Serial.println("---- SETUP END ----");
}

void loop()
{
  if (isPlaybackActive)
  {
    size_t copiedBytesCount = audio_copier.copy();

    if (copiedBytesCount == 0) // 0 bytes copied mean end of source
    {
      audio_copier.end();
      soundFile.close();
      i2s_out_stream.setMute(true);
      isPlaybackActive = false;
      Serial.println("playback finished");
    }
  }
}
