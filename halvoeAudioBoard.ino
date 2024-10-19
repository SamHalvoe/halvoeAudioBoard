#include "halvoeAudioBoard.hpp"
#include "SerialPeriphial.hpp"

using namespace halvoe;

SerialPeriphial serialInterface(Serial1);

void setup()
{
  Serial.begin(115200);
  AudioLogger::instance().begin(Serial, AudioLogger::Warning);
  delay(2000);

  Serial.println("[Serial Ready]");
  Serial.println("---- SETUP BEGIN ----");

  sdHandler::setup();
  audioDriver::setup();
  audioPipeline::setup();
  serialInterface.setup();

  Serial.println("---- SETUP END ----");
}

void loop()
{
  serialInterface.receiveMessage();
  
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
      audioPipeline::end();
      audioPipeline::beginSilence();
      isPlaybackActive = false;
    }
  }
}
