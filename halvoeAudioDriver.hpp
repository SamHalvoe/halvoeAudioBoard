#pragma once

#include <AudioBoard.h>
#include <Driver.h>
#include <DriverCommon.h>
#include <DriverPins.h>

namespace halvoe::audioDriver
{
  // I2C pins & config
  const int8_t SDA_PIN = 21; // I2C Data,  Adafruit ESP32 S3 3, Sparkfun Thing Plus C 23
  const int8_t SCL_PIN = 22; // I2C Clock, Adafruit ESP32 S3 4, Sparkfun Thing Plus C 22
  const uint32_t I2C_SPEED = 100000; // Clock Rate
  const int8_t WM8960_ADDR = 0x1A; // Address of WM8960 I2C port

  // I2S pins (-1 = not used)
  const int8_t MCLK_PIN = -1; // Master Clock
  const int8_t BCLK_PIN = 27; // Bit Clock
  const int8_t WS_PIN = 26; // Word select
  const int8_t DO_PIN = 25; // This is connected to DI on WM8960 (MISO)
  const int8_t DI_PIN = -1; // This is connected to DO on WM8960 (MOSI)

  DriverPins audioPins;
  AudioBoard audioBoard(AudioDriverWM8960, audioPins);

  bool setup()
  {
    // setup I2C and IS2S pins
    audioPins.addI2C(PinFunction::CODEC, SCL_PIN, SDA_PIN, WM8960_ADDR, I2C_SPEED, Wire);
    audioPins.addI2S(PinFunction::CODEC, MCLK_PIN, BCLK_PIN, WS_PIN, DO_PIN, DI_PIN);
    bool isPinSetupOk = audioPins.begin();
    bool isWireOk = Wire.begin();

    audioBoard.setVolume(100);

    return isPinSetupOk && isWireOk;
  }
}
