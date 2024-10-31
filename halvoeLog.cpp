#include "halvoeLog.hpp"

namespace halvoeAudioBoard
{
  LogFileManager::LogFileManager() :
    m_logFilePath("/audioBoard.log"),
    m_logFileFlushInterval(60), // seconds
    m_isSetup(false),
    m_logFilePathAudioLibrary("/audioLibrary.log"),
    m_logFileFlushIntervalAudioLibrary(60), // seconds
    m_logStreamAudioLibrary(m_logFileAudioLibrary, Serial),
    m_isSetupAudioLibrary(false)
  {}
  
  bool LogFileManager::setup()
  {
    LOG_ATTACH_FS_MANUAL(SD, m_logFilePath, FILE_APPEND);
    m_isSetup = LOG_FILE_IS_OPEN();

    if (m_isSetup)
    {
      LOG_INFO("[Log file is ready]");
    }
    else
    {
      LOG_ERROR("Failed to setup log file!");
    }

    m_logFileAudioLibrary = SD.open(m_logFilePathAudioLibrary, FILE_APPEND, true);
    
    if (m_logFileAudioLibrary)
    {
      m_logStreamAudioLibrary.println("[Log file for audio library is ready]");
      LOG_INFO("[Log file for audio library is ready]");
      m_isSetupAudioLibrary = true;
    }
    else
    {
      LOG_ERROR("Failed to setup log file for audio library!");
    }

    return m_isSetup && m_isSetupAudioLibrary;
  }

  bool LogFileManager::isSetup() const
  {
    return m_isSetup;
  }

  bool LogFileManager::isSetupAudioLibrary() const
  {
    return m_isSetupAudioLibrary;
  }

  StreamUtils::LoggingStream& LogFileManager::getLogStreamAudioLibrary()
  {
    return m_logStreamAudioLibrary;
  }

  void LogFileManager::flush()
  {
    if (m_timeSinceFlush >= m_logFileFlushInterval)
    {
      LOG_FILE_FLUSH();
      LOG_INFO("LOG_FILE_FLUSHed");
      m_timeSinceFlush = 0;
    }
  }

  void LogFileManager::flushAudioLibrary()
  {
    if (m_timeSinceFlushAudioLibrary >= m_logFileFlushIntervalAudioLibrary)
    {
      m_logStreamAudioLibrary.flush();
      LOG_INFO("Audio library log file flushed");
      m_timeSinceFlushAudioLibrary = 0;
    }
  }
}
