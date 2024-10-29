#include "halvoeLog.hpp"
#include "halvoeSDHandler.hpp"

namespace halvoe
{
  SDHandler::SDHandler()
  {}

  bool SDHandler::setup()
  {
    if (not SD.begin())
    {
      LOG_ERROR("SD.begin() failed!");
      return false;
    }
    
    LOG_ATTACH_FS_MANUAL(SD, "audioBoard.log", FILE_APPEND);
    LOG_INFO("[File logging ready]");
    return true;
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
