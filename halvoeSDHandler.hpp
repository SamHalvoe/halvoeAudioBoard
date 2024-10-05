#pragma once

#include <SD.h>
#include <vector>

namespace halvoe::sdHandler
{
  bool setup()
  {
    if (not SD.begin())
    {
      Serial.println("SD.begin() failed!");
      return false;
    }

    return true;
  }

  std::vector<String> list(const char* in_directoryPath)
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
