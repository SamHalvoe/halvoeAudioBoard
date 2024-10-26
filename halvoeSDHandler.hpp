#pragma once

#include <SD.h>
#include <vector>

namespace halvoe
{
  class SDHandler
  {
    public:
      SDHandler();
      bool setup();
      std::vector<String> list(const char* in_directoryPath);
  };
}
