/**
 * @file BoardSupportPackage_BSP.ino
 * @author Phat N. (phat@pdiytech.com)
 * @brief Example to print BSP (Board Support Package)
 * @version 0.1
 * @date 2023-Dec-20
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <AirGradient.h>

void setup()
{
#if defined(ESP8266)
  /**
   * ESP8266 has support debug log over Serial
   */
  Serial.begin(115200);
  AirGradientBspPrint(&Serial);
#else
  /**
   * ESP32S3 has support debug log over USB-CDC. To show debug log please configure
   * 'Tool' > 'Core debug level' > 'Info' (Or above Debug/Verbose)
   */
  AirGradientBspPrint(NULL);
#endif
}

void loop()
{
  delay(1000);
}
