#include "halvoeLog.hpp"
#include "halvoeVersion.hpp"
#include "halvoeSDHandler.hpp"
#include "halvoeAudioPipeline.hpp"
#include "SerialPeriphial.hpp"

using namespace halvoe;
using namespace halvoeAudioBoard;

SDHandler sdHandler;
LogFileManager logFileManager;
AudioPipeline audioPipeline;
SerialPeriphial serialInterface(Serial1, audioPipeline);

void setup()
{
  Serial.begin(115200);
  delay(2000);

  LOG_INFO("[Serial is ready]");

  sdHandler.setup();
  logFileManager.setup();
  AudioDriverLogger.begin(logFileManager.getLogStreamAudioLibrary(), AudioDriverLogLevel::Warning);
  AudioToolsLogger.begin(logFileManager.getLogStreamAudioLibrary(), AudioToolsLogLevel::Warning);
  LOG_INFO(getVersionString());
  audioPipeline.setup();
  serialInterface.setup();

  LOG_INFO("Leave setup...");
}

void loop()
{
  serialInterface.receiveMessage();
  audioPipeline.run();
  logFileManager.flush();
  logFileManager.flushAudioLibrary();
}
