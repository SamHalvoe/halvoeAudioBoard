#include "halvoeLog.hpp"
#include "halvoeSDHandler.hpp"
#include "halvoeAudioPipeline.hpp"
#include "SerialPeriphial.hpp"

using namespace halvoe;

SDHandler sdHandler;
AudioPipeline audioPipeline;
SerialPeriphial serialInterface(Serial1, audioPipeline);

void setup()
{
  Serial.begin(115200);
  AudioLogger::instance().begin(Serial, AudioLogger::Warning);
  delay(2000);

  LOG_INFO("[Serial Ready]");
  LOG_INFO("---- SETUP BEGIN ----");

  sdHandler.setup();
  audioPipeline.setup();
  serialInterface.setup();

  LOG_INFO("---- SETUP END ----");
}

void loop()
{
  serialInterface.receiveMessage();
  audioPipeline.run();
}
