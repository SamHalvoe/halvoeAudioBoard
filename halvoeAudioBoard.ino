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

  Serial.println("[Serial Ready]");
  Serial.println("---- SETUP BEGIN ----");

  sdHandler.setup();
  audioPipeline.setup();
  serialInterface.setup();

  Serial.println("---- SETUP END ----");
}

void loop()
{
  serialInterface.receiveMessage();
  audioPipeline.run();
}
