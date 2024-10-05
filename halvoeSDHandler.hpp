#pragma once

#include <SD.h>

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
}
