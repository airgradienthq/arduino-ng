#include <AirGradient.h>
#include "esp32-hal-log.h"

#define TEST_CO2_S8

#if defined(TEST_CO2_S8)
CO2_S8 co2s8(BOARD_ONE_INDOOR_MONITOR_V9_0);
#endif

void setup()
{
  Serial.begin(115200);
  BoardDefPrintBSP(&Serial);
  co2s8.begin(&Serial1);

  // Wait for sensor ready
  // while (1)
  // {
  //   delay(100);
  //   if (co2s8.isReady())
  //   {
  //     break;
  //   }
  // }
}

void loop()
{
#if defined(TEST_CO2_S8)
  // Serial.printf("CO2 value: %d (ppm)\r\n", co2s8.getCO2(1));
  delay(1000);  // wait for 1000ms
#endif
}
