#pragma once

#include <AudioBoard.h>

namespace halvoe
{
  class AudioBoardDriver
  {
    public:
      // I2C pins & config
      const uint32_t I2C_SPEED = 100000; // Clock Rate
      const int8_t SDA_PIN = 21; // I2C Data
      const int8_t SCL_PIN = 22; // I2C Clock
      const int8_t WM8960_ADDR = 0x1A; // Address of WM8960 I2C port

      // I2S pins (-1 = not used)
      const int8_t MCLK_PIN = -1; // Master Clock
      const int8_t BCLK_PIN = 27; // Bit Clock
      const int8_t WS_PIN = 26; // Word select
      const int8_t DO_PIN = 25; // This is connected to DI on WM8960 (MISO)
      const int8_t DI_PIN = -1; // This is connected to DO on WM8960 (MOSI)

    private:
      DriverPins m_audioPins;
      AudioBoard m_audioBoard;

    public:
      AudioBoardDriver();
      bool setup();
      AudioBoard& getBoard();
  };
}
