#include <SdFat.h>

#define SD_CS_PIN 5
#define SPI_CLOCK SD_SCK_MHZ(20)

#define SD_CONFIG SdSpiConfig(SD_CS_PIN, DEDICATED_SPI, SPI_CLOCK)

SdFs sd;

void setup()
{
  Serial.begin(9600);
  delay(1000);

  if (not sd.begin(SD_CONFIG))
  {
    sd.initErrorHalt(&Serial);
  }
  
  Serial.println(sd.exists("ddd.wav") ? "found wav" : "did not found wav");
}

void loop()
{
  delay(1000);
  Serial.print(".");
}
