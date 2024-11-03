#pragma once

#define DEBUGLOG_DEFAULT_LOG_LEVEL_INFO
#define DEBUGLOG_DEFAULT_FILE_LEVEL_INFO
#define DEBUGLOG_ENABLE_FILE_LOGGER

#include <SD.h> // this header is needed, because for file logging FILE_WRITE has to be defined and FS.h (included by SD.h) defines FILE_WRITE properly
#include <DebugLog.h>
#include <StreamUtils.hpp>
#include <elapsedMillis.h>

namespace halvoeAudioBoard
{
  class LogFileManager
  {
    private:
      const char* m_logFilePath;
      const unsigned long m_logFileFlushInterval; // in seconds
      elapsedSeconds m_timeSinceFlush;
      bool m_isSetup = false;

      const char* m_logFilePathAudioLibrary;
      const unsigned long m_logFileFlushIntervalAudioLibrary; // in seconds
      elapsedSeconds m_timeSinceFlushAudioLibrary;
      File m_logFileAudioLibrary;
      StreamUtils::LoggingStream m_logStreamAudioLibrary;
      bool m_isSetupAudioLibrary = false;

    public:
      LogFileManager();
      bool setup();
      bool isSetup() const;
      bool isSetupAudioLibrary() const;
      StreamUtils::LoggingStream& getLogStreamAudioLibrary();
      void flush();
      void flushAudioLibrary();
  };
}
