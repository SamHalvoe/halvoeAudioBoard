#include "halvoeSDHandler.hpp"
#include "halvoeLog.hpp"

namespace halvoeAudioBoard
{
  SDHandler::SDHandler()
  {}

  bool SDHandler::setup(uint8_t in_maxRetryCount)
  {
    m_isSetup = SD.begin();

    for (uint8_t retryCount = 0; not m_isSetup && retryCount < in_maxRetryCount; ++retryCount)
    {
      LOG_WARN("SD.begin() failed! Retry in 2 seconds...");
      delay(2000);
      m_isSetup = SD.begin();
    }

    if (m_isSetup)
    {
      LOG_INFO("SD.begin() succeded!");
    }
    else
    {
      LOG_ERROR("SD.begin() failed!");
    }

    return m_isSetup;
  }

  bool SDHandler::isSetup() const
  {
    return m_isSetup;
  }

  std::vector<String> SDHandler::list(const char* in_directoryPath)
  {
    File directory = SD.open(in_directoryPath);
    if (not directory) { return std::vector<String>(); }
    std::vector<String> list;

    for (File file = directory.openNextFile();
         file; file = directory.openNextFile())
    {
      list.emplace_back(file.name());
    }

    return list;
  }
}
