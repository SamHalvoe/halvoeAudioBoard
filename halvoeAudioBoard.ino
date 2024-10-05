#include "halvoeAudioPipeline.hpp"
#include "halvoeSDHandler.hpp"

#include <BasicSerializer.hpp>

using namespace halvoe;

const char* soundFilename = "ddd4416.wav";
File soundFile;
bool isPlaybackActive = false;
std::array<uint8_t, 128> deserializerBuffer;

bool startPlayback()
{
  String path("/");
  path.concat(soundFilename);
  soundFile = SD.open(path);

  if (not soundFile)
  {
    Serial.print("Could not open file: \"");
    Serial.print(path);
    Serial.println("\"");

    return false;
  }

  audioPipeline::begin(soundFile);
  isPlaybackActive = true;
  
  return true;
}

void setup()
{
  Serial.begin(115200);
  AudioLogger::instance().begin(Serial, AudioLogger::Warning);
  delay(2000);

  Serial.println("[Serial Ready]");
  Serial.println("---- SETUP BEGIN ----");

  Serial1.setPins(33, 32);
  Serial1.begin(19200);

  sdHandler::setup();
  audioDriver::setup();
  audioPipeline::setup();
  startPlayback();

  Serial.println("---- SETUP END ----");
}

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
        audioPipeline::volumeStream.setVolume(0.0);
        audioPipeline::volume = 0.0;
        isPlaybackActive = not isPlaybackActive;

        if (isPlaybackActive)
        {
          soundFile.seek(0); // restart playing of soundFile from position 0
        }

        Serial.println("42 end");
      }
    }
  }

  if (not isPlaybackActive)
  {
    audioPipeline::volumeStream.writeSilence(32);
  }
  
  size_t copiedBytesCount = audioPipeline::audioCopier.copy();

  if (isPlaybackActive)
  {
    if (audioPipeline::volume < 1.0)
    {
      audioPipeline::volumeStream.setVolume(audioPipeline::volume);
      audioPipeline::volume = audioPipeline::volume + audioPipeline::volumeStep;
    }

    if (copiedBytesCount == 0) // 0 bytes copied mean end of source
    {
      audioPipeline::volumeStream.setVolume(0.0);
      audioPipeline::volume = 0.0;
      soundFile.seek(0); // restart playing of soundFile from position 0
    }
  }
}
