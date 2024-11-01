#include "halvoeAudioDriver.hpp"
#include "halvoeLog.hpp"

namespace halvoe
{
  AudioBoardDriver::AudioBoardDriver() : m_audioBoard(AudioDriverWM8960, m_audioPins)
  {}

  bool AudioBoardDriver::setup()
  {
    // define I2C and I2S pins
    m_audioPins.addI2C(PinFunction::CODEC, SCL_PIN, SDA_PIN, WM8960_ADDR, I2C_SPEED, Wire);
    m_audioPins.addI2S(PinFunction::CODEC, MCLK_PIN, BCLK_PIN, WS_PIN, DO_PIN, DI_PIN);

    if (not Wire.begin())
    {
      LOG_ERROR("Could begin Wire!");
      return false;
    }

    return true;
  }

  AudioBoard& AudioBoardDriver::getBoard()
  {
    return m_audioBoard;
  }
}
