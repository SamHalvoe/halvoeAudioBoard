#include "halvoeSDHandler.hpp"
#include "halvoeLog.hpp"

namespace halvoeAudioBoard
{
  SDHandler::SDHandler()
  {}

  bool SDHandler::setup()
  {
    m_isSetup = SD.begin();
    if (not m_isSetup) { LOG_ERROR("SD.begin() failed!"); }
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
