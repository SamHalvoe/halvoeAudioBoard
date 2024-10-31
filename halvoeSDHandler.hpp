#pragma once

#include <SD.h>
#include <vector>

namespace halvoeAudioBoard
{
  class SDHandler
  {
    private:
      bool m_isSetup = false;

    public:
      SDHandler();
      bool setup();
      bool isSetup() const;
      std::vector<String> list(const char* in_directoryPath);
  };
}
