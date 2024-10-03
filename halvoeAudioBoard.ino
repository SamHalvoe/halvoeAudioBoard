#include <AudioBoard.h>
#include <Driver.h>
#include <DriverCommon.h>
#include <DriverPins.h>

#include <AudioTools.h>
#include <AudioLibs/I2SCodecStream.h>
#include <AudioCodecs/CodecWAV.h>

#include <SdFat.h>
#include <elapsedMillis.h>

#include <BasicSerializer.hpp>

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

AudioInfo audioInfo(44100, 2, 16); // sampling rate, # channels, bit depth
Pipeline audioPipeline;
DriverPins audioPins;
AudioBoard audioBoard(AudioDriverWM8960, audioPins);
I2SCodecStream i2sOutStream(audioBoard);
VolumeStream volumeStream;

const char* soundFilename = "ddd4416.wav";
SdFat sdCard;
File soundFile;

WAVDecoder wavDecoder;
EncodedAudioStream encodedAudioStream(&wavDecoder);
StreamCopy audioCopier(audioPipeline, soundFile, 4096);

bool isPlaybackActive = false;

std::array<uint8_t, 128> deserializerBuffer;

void setup()
{
  Serial.begin(115200);
  AudioLogger::instance().begin(Serial, AudioLogger::Warning);
  delay(2000);

  Serial.println("[Serial Ready]");
  Serial.println("---- SETUP BEGIN ----");

  Serial1.setPins(33, 32);
  Serial1.begin(19200);

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
  Wire.begin();

  audioBoard.setVolume(100);

  // setup encoded stream
  encodedAudioStream.begin(audioInfo);

  // setup volume stream
  auto volumeConfig = volumeStream.defaultConfig();
  volumeConfig.copyFrom(audioInfo);
  volumeConfig.allow_boost = true;
  volumeStream.begin(volumeConfig);
  volumeStream.setVolume(0.0);

  // setup i2s
  auto i2sConfig = i2sOutStream.defaultConfig();
  i2sConfig.copyFrom(audioInfo);
  i2sConfig.buffer_size = 2048;
  i2sOutStream.begin(i2sConfig);
  i2sOutStream.setVolume(1.0);

  // setup audio pipeline
  audioPipeline.add(encodedAudioStream);
  audioPipeline.add(volumeStream);
  audioPipeline.setOutput(i2sOutStream);

  // setup audio copier
  audioCopier.begin();

  isPlaybackActive = true;

  Serial.println("---- SETUP END ----");
}

float volumeStep = 0.05;
float volume = volumeStep;

void loop()
{
  if (Serial1.available() >= 4)
  {
    Serial.println("available() >= 4");
    halvoe::Deserializer<128> deserializer(deserializerBuffer);

    if (Serial1.readBytes(deserializerBuffer.data(), sizeof(uint16_t)) == sizeof(uint16_t))
    {
      Serial.println("readBytes() == sizeof(uint16_t)");
      uint16_t length = deserializer.read<uint16_t>();
      Serial.println(length);
      Serial1.readBytes(deserializerBuffer.data() + sizeof(uint16_t), length);
      uint16_t command = deserializer.read<uint16_t>();
      Serial.println(command);
      
      if (command == 42)
      {
        Serial.println("42 begin");
        volumeStream.setVolume(0.0);
        volume = 0.0;
        isPlaybackActive = not isPlaybackActive;

        if (isPlaybackActive)
        {
          soundFile.seek(44); // (re)start playing of soundFile from position 44 to skip the wav header
        }

        Serial.println("42 end");
      }
    }
  }

  if (not isPlaybackActive)
  {
    encodedAudioStream.writeSilence(32);
  }
  
  size_t copiedBytesCount = audioCopier.copy();

  if (isPlaybackActive)
  {
    if (volume < 1.0)
    {
      volumeStream.setVolume(volume);
      volume = volume + volumeStep;
    }

    if (copiedBytesCount == 0) // 0 bytes copied mean end of source
    {
      volumeStream.setVolume(0.0);
      volume = 0.0;
      soundFile.seek(44); // (re)start playing of soundFile from position 44 to skip the wav header
    }
  }
}
