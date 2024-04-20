#include <AudioTools.h>
#include <AudioLibs/AudioSourceSDFAT.h>
#include <AudioLibs/I2SCodecStream.h>
#include <AudioCodecs/CodecWAV.h>

// I2C
#define SDA_PIN              21 // I2C Data,  Adafruit ESP32 S3 3, Sparkfun Thing Plus C 23
#define SCL_PIN              22 // I2C Clock, Adafruit ESP32 S3 4, Sparkfun Thing Plus C 22
#define I2C_SPEED        100000 // Clock Rate
#define WM8960_ADDR        0x1A // Address of WM8960 I2C port

// I2S, your configuration for the WM8960 board
#define MCLK_PIN             -1 // Master Clock
#define BCLK_PIN             27 // Bit Clock
#define WS_PIN               26 // Word select
#define DO_PIN               25 // This is connected to DI on WM8960 (MISO)
#define DI_PIN               -1 // This is connected to DO on WM8960 (MOSI)

AudioInfo                     audio_info(44100, 2, 16);                // sampling rate, # channels, bit depth
DriverPins                    my_pins;                                 // board pins
AudioBoard                    audio_board(AudioDriverWM8960, my_pins); // audio board
I2SCodecStream                i2s_out_stream(audio_board);             // i2s coded

const char* startFilePath="/";
const char* ext="wav";
AudioSourceSDFAT source(startFilePath, ext);
WAVDecoder decoder;
AudioPlayer player(source, i2s_out_stream, decoder);

/*SineWaveGenerator<int16_t> sineWave(32000);
GeneratedSoundStream<int16_t> sound(sineWave);
StreamCopy copier(i2s_out_stream, sound);*/

void setup()
{
  Serial.begin(9600);
  AudioLogger::instance().begin(Serial, AudioLogger::Warning);
  delay(2000);

  Serial.println("[Serial Ready]");
  Serial.println("---- SETUP BEGIN ----");

  // setup i2c and i2s pins
  my_pins.addI2C(PinFunction::CODEC, SCL_PIN, SDA_PIN, WM8960_ADDR, I2C_SPEED, Wire);
  my_pins.addI2S(PinFunction::CODEC, MCLK_PIN, BCLK_PIN, WS_PIN, DO_PIN, DI_PIN);
  my_pins.begin();

  //audio_board.begin();
  
  // setup i2s
  auto i2s_config = i2s_out_stream.defaultConfig();
  i2s_config.copyFrom(audio_info);
  i2s_out_stream.begin(i2s_config);

  // set volume
  i2s_out_stream.setVolume(0.625);

  // setup player
  player.begin();
  //sineWave.begin(audio_info, N_B4);

  Serial.println("---- SETUP END ----");
}

void loop()
{
  player.copy();
  //copier.copy();
}
