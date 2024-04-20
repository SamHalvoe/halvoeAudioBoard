#include <SparkFun_WM8960_Arduino_Library.h> 
#include <AudioTools.h>
#include <AudioLibs/AudioSourceSDFAT.h>
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

WM8960 codec;

AudioInfo audio_info(44100, 2, 16);                // sampling rate, # channels, bit depth
I2SStream i2s_out_stream;

const char* startFilePath="/";
const char* ext="wav";
AudioSourceSDFAT source(startFilePath, ext);
WAVDecoder decoder;
AudioPlayer player(source, i2s_out_stream, decoder);

/*SineWaveGenerator<int16_t> sineWave(32000);
GeneratedSoundStream<int16_t> sound(sineWave);
StreamCopy copier(i2s_out_stream, sound);*/

void codec_setup()
{
  // General setup needed
  codec.enableVREF();
  codec.enableVMID();

  // Connect from DAC outputs to output mixer
  codec.enableLD2LO();
  codec.enableRD2RO();

  // Set gainstage between booster mixer and output mixer
  // For this loopback example, we are going to keep these as low as they go
  codec.setLB2LOVOL(WM8960_OUTPUT_MIXER_GAIN_NEG_21DB); 
  codec.setRB2ROVOL(WM8960_OUTPUT_MIXER_GAIN_NEG_21DB);

  // Enable output mixers
  codec.enableLOMIX();
  codec.enableROMIX();

  // CLOCK STUFF, These settings will get you 44.1KHz sample rate, and class-d 
  // freq at 705.6kHz
  codec.enablePLL(); // Needed for class-d amp clock
  codec.setPLLPRESCALE(WM8960_PLLPRESCALE_DIV_2);
  codec.setSMD(WM8960_PLL_MODE_FRACTIONAL);
  codec.setCLKSEL(WM8960_CLKSEL_PLL);
  codec.setSYSCLKDIV(WM8960_SYSCLK_DIV_BY_2);
  codec.setBCLKDIV(4);
  codec.setDCLKDIV(WM8960_DCLKDIV_16);
  codec.setPLLN(7);
  codec.setPLLK(0x86, 0xC2, 0x26); // PLLK=86C226h
  //codec.setADCDIV(0); // Default is 000 (what we need for 44.1KHz)
  //codec.setDACDIV(0); // Default is 000 (what we need for 44.1KHz)
  codec.setWL(WM8960_WL_16BIT);

  codec.enablePeripheralMode();
  //codec.enableMasterMode();
  //codec.setALRCGPIO(); // Note, should not be changed while ADC is enabled.

  // Enable DACs
  codec.enableDacLeft();
  codec.enableDacRight();

  //codec.enableLoopBack(); // Loopback sends ADC data directly into DAC
  codec.disableLoopBack();

  // Default is "soft mute" on, so we must disable mute to make channels active
  codec.disableDacMute(); 

  codec.enableHeadphones();
  codec.enableOUT3MIX(); // Provides VMID as buffer for headphone ground

  Serial.println("Volume set to -20.00 dB");
  codec.setHeadphoneVolumeDB(-20.00);

  Serial.println("Codec Setup complete.");
}

void setup()
{
  Serial.begin(9600);
  AudioLogger::instance().begin(Serial, AudioLogger::Warning);
  delay(2000);

  Serial.println("[Serial Ready]");
  Serial.println("---- SETUP BEGIN ----");

  Wire.begin();

  if (codec.begin())
  {
    Serial.println("The codec did respond.");

    codec_setup();
  }
  else
  {
    Serial.println("The codec did not respond!");
  }
  
  // setup i2s
  auto i2s_config = i2s_out_stream.defaultConfig();
  i2s_config.copyFrom(audio_info);
  i2s_config.pin_bck = BCLK_PIN;
  i2s_config.pin_ws = WS_PIN;
  i2s_config.pin_data = DO_PIN;
  i2s_config.pin_data_rx = DI_PIN;

  i2s_out_stream.begin(i2s_config);

  // setup player
  player.begin();
  //sineWave.begin(audio_info, N_F5);

  Serial.println("---- SETUP END ----");
}

void loop()
{
  player.copy();
  //copier.copy();
}
