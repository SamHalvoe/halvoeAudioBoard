#include <elapsedMillis.h>

#include <SdFat.h>
#include <SparkFun_WM8960_Arduino_Library.h> 
#include <AudioTools.h>
#include <AudioCodecs/CodecWAV.h>

#define SD_CS_PIN 5
#define SPI_CLOCK SD_SCK_MHZ(20)
#define SD_CONFIG SdSpiConfig(SD_CS_PIN, DEDICATED_SPI, SPI_CLOCK)

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

AudioInfo audioInfo(44100, 2, 16); // sampling rate, # channels, bit depth
Pipeline audioPipeline;
VolumeStream volumeStream;
I2SStream i2sOutStream;

const char* soundFilename = "ddd4416.wav";
SdFat sdCard;
File soundFile;

WAVDecoder wavDecoder;
EncodedAudioStream encodedAudioStream(&wavDecoder);
StreamCopy audioCopier(audioPipeline, soundFile, 4096);

bool isPlaybackActive = false;

WM8960 codecBoard;
const float dacVolumeDBMin = -30.0;
const float dacVolumeDBMax = 30.0;
float dacVolumeDB = dacVolumeDBMin;

bool setupCodecBoard()
{
  if (codecBoard.begin())
  {
    Serial.println("The codec did respond.");
  }
  else
  {
    Serial.println("The codec did not respond!");

    return false;
  }

  // General setup needed
  codecBoard.enableVREF();
  codecBoard.enableVMID();

  // Connect from DAC outputs to output mixer
  codecBoard.enableLD2LO();
  codecBoard.enableRD2RO();
  
  // Set gainstage between booster mixer and output mixer
  codecBoard.setLB2LOVOL(WM8960_OUTPUT_MIXER_GAIN_0DB);
  codecBoard.setRB2ROVOL(WM8960_OUTPUT_MIXER_GAIN_0DB);

  // Enable output mixers
  codecBoard.enableLOMIX();
  codecBoard.enableROMIX();

  // CLOCK STUFF, These settings will get you 44.1KHz sample rate, and class-d freq at 705.6kHz
  codecBoard.enablePLL(); // Needed for class-d amp clock
  codecBoard.setPLLPRESCALE(WM8960_PLLPRESCALE_DIV_2);
  codecBoard.setSMD(WM8960_PLL_MODE_FRACTIONAL);
  codecBoard.setCLKSEL(WM8960_CLKSEL_PLL);
  codecBoard.setSYSCLKDIV(WM8960_SYSCLK_DIV_BY_2);
  codecBoard.setBCLKDIV(4);
  codecBoard.setDCLKDIV(WM8960_DCLKDIV_16);
  codecBoard.setPLLN(7);
  codecBoard.setPLLK(0x86, 0xC2, 0x26); // PLLK=86C226h
  //codecBoard.setADCDIV(0); // Default is 000 (what we need for 44.1KHz)
  //codecBoard.setDACDIV(0); // Default is 000 (what we need for 44.1KHz)
  codecBoard.setWL(WM8960_WL_16BIT);

  codecBoard.enablePeripheralMode();

  // Enable DACs
  codecBoard.enableDacLeft();
  codecBoard.enableDacRight();

  codecBoard.disableLoopBack();

  // Default is "soft mute" on, so we must disable mute to make channels active
  codecBoard.disableDacMute();

  codecBoard.enableHeadphones();
  codecBoard.enableOUT3MIX(); // Provides VMID as buffer for headphone ground
  codecBoard.setHeadphoneVolumeDB(-73.0);

  codecBoard.enableSpeakers();
  codecBoard.setSpeakerVolumeDB(6.0);
  codecBoard.setSpeakerDcGain(1);
  
  codecBoard.setDacLeftDigitalVolumeDB(dacVolumeDB);
  codecBoard.setDacRightDigitalVolumeDB(dacVolumeDB);

  return true;
}

void setup()
{
  Serial.begin(115200);
  AudioLogger::instance().begin(Serial, AudioLogger::Warning);
  delay(2000);

  Serial.println("[Serial Ready]");
  Serial.println("---- SETUP BEGIN ----");

  if (not sdCard.begin(SD_CONFIG))
  {
    sdCard.initErrorHalt(&Serial);
  }

  if (not soundFile.open(soundFilename, FILE_READ))
  {
    Serial.print("Could not open file: \"");
    Serial.print(soundFilename);
    Serial.println("\"");
  }

  Wire.begin();
  setupCodecBoard();
  
  // setup i2s
  auto i2sConfig = i2sOutStream.defaultConfig();
  i2sConfig.copyFrom(audioInfo);
  i2sConfig.pin_mck = MCLK_PIN;
  i2sConfig.pin_bck = BCLK_PIN;
  i2sConfig.pin_ws = WS_PIN;
  i2sConfig.pin_data = DO_PIN;
  i2sConfig.pin_data_rx = DI_PIN;
  i2sConfig.buffer_size = 2048;
  i2sOutStream.begin(i2sConfig);
  
  // setup encoded stream
  encodedAudioStream.begin(audioInfo);

  // setup audio pipeline
  audioPipeline.add(encodedAudioStream);
  audioPipeline.setOutput(i2sOutStream);

  // setup audio copier
  audioCopier.begin();

  isPlaybackActive = true;

  Serial.println("---- SETUP END ----");
}

const uint8_t volumeUpdateInterval = 20;
elapsedMillis timeSinceVolumeUpdate = volumeUpdateInterval;

void loop()
{
  if (isPlaybackActive)
  {
    size_t copiedBytesCount = audioCopier.copy();

    if (dacVolumeDB < dacVolumeDBMax)
    {
      dacVolumeDB = dacVolumeDB + 1.0;

      if (timeSinceVolumeUpdate > volumeUpdateInterval)
      {
        codecBoard.setDacLeftDigitalVolumeDB(dacVolumeDB);
        codecBoard.setDacRightDigitalVolumeDB(dacVolumeDB);
        Serial.print("dacVolumeDB: ");
        Serial.println(dacVolumeDB);
        Serial.println("volume updated");
        timeSinceVolumeUpdate = 0;
      }
    }

    if (copiedBytesCount == 0) // 0 bytes copied mean end of source
    {
      dacVolumeDB = dacVolumeDBMin;
      codecBoard.setDacLeftDigitalVolumeDB(dacVolumeDB);
      codecBoard.setDacRightDigitalVolumeDB(dacVolumeDB);
      soundFile.seek(44); // (re)start playing of soundFile from position 44 to skip the wav header

      Serial.println("playback finished");
    }
  }
}
