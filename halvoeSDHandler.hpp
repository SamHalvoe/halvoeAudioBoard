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
      bool setup(uint8_t in_maxRetryCount = 3);
      bool isSetup() const;
      std::vector<String> list(const char* in_directoryPath);
  };
}
