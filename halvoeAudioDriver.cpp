#include "halvoeAudioDriver.hpp"

namespace halvoe
{
  AudioBoardDriver::AudioBoardDriver() : m_audioBoard(AudioDriverWM8960, m_audioPins)
  {}

  bool AudioBoardDriver::setup()
  {
    // setup I2C and IS2S pins
    m_audioPins.addI2C(PinFunction::CODEC, SCL_PIN, SDA_PIN, WM8960_ADDR, I2C_SPEED, Wire);
    m_audioPins.addI2S(PinFunction::CODEC, MCLK_PIN, BCLK_PIN, WS_PIN, DO_PIN, DI_PIN);
    bool isPinSetupOk = m_audioPins.begin();
    bool isWireOk = Wire.begin();

    m_audioBoard.setVolume(100);

    return isPinSetupOk && isWireOk;
  }

  AudioBoard& AudioBoardDriver::getBoard()
  {
    return m_audioBoard;
  }
}
