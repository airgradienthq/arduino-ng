#include <AirGradient.h>
#include <Wire.h>

/**
 * AirGradient use ESP32C3 has default Serial0 use for PMS5003, to print log should
 * use esp-hal-log instead.
 */
#include <esp32-hal-log.h>

/**
 * @brief Define test board
 */
#define TEST_BOARD_OUTDOOR_MONITOR_V1_3       1
#define TEST_BOARD_ONE_INDOOR_MONITOR_V9_0    0
/**
 * @brief Define test sensor
 */
#define TEST_SENSOR_SenseAirS8                0
#define TEST_SENSOR_PMS5003                   0
#define TEST_SENSOR_SHT4x                     0
#define TEST_SENSOR_SGP4x                     0
#define TEST_LED                              1
#define TEST_SWITCH                           1

#if TEST_SENSOR_SenseAirS8

#if TEST_BOARD_OUTDOOR_MONITOR_V1_3
SenseAirS8 co2s8(BOARD_OUTDOOR_MONITOR_V1_3);
#elif TEST_BOARD_ONE_INDOOR_MONITOR_V9_0
SenseAirS8 co2s8(BOARD_ONE_INDOOR_MONITOR_V9_0);
#else
#error "No test board defined"
#endif /** TEST_BOARD_DIY_BASIC_KIT */

#endif /** TEST_SENSOR_SenseAirS8 */

#if TEST_SENSOR_PMS5003
#if TEST_BOARD_OUTDOOR_MONITOR_V1_3
PMS5003 pms(BOARD_OUTDOOR_MONITOR_V1_3, Serial);
#elif TEST_BOARD_ONE_INDOOR_MONITOR_V9_0
PMS5003 pms(BOARD_ONE_INDOOR_MONITOR_V9_0, Serial);
#else
#error "No test board defined"
#endif /** TEST_BOARD_DIY_BASIC_KIT */
#endif /** TEST_SENSOR_PMS5003 */

#if TEST_SENSOR_SHT4x
#if TEST_BOARD_OUTDOOR_MONITOR_V1_3
AirGradientSht sht(BOARD_OUTDOOR_MONITOR_V1_3);
#elif TEST_BOARD_ONE_INDOOR_MONITOR_V9_0
AirGradientSht sht(BOARD_ONE_INDOOR_MONITOR_V9_0);
#else
#error "No test board defined"
#endif
#endif /** TEST_SENSOR_SHT4x */

#if TEST_SENSOR_SGP4x
#if TEST_BOARD_OUTDOOR_MONITOR_V1_3
AirGradientSgp4x sgp(BOARD_OUTDOOR_MONITOR_V1_3);
#elif TEST_BOARD_ONE_INDOOR_MONITOR_V9_0
AirGradientSgp4x sgp(BOARD_ONE_INDOOR_MONITOR_V9_0);
#else
#error "No test board defined"
#endif
#endif

#if TEST_LED
#if TEST_BOARD_OUTDOOR_MONITOR_V1_3
AirGradientLED led(BOARD_OUTDOOR_MONITOR_V1_3);
#elif TEST_BOARD_ONE_INDOOR_MONITOR_V9_0
AirGradientLED led(BOARD_ONE_INDOOR_MONITOR_V9_0);
#else
#error "No test board defined"
#endif
#endif

#if TEST_SWITCH
#if TEST_BOARD_OUTDOOR_MONITOR_V1_3
AirGradientSwitch sw(BOARD_OUTDOOR_MONITOR_V1_3);
#elif TEST_BOARD_ONE_INDOOR_MONITOR_V9_0
AirGradientSwitch sw(BOARD_ONE_INDOOR_MONITOR_V9_0);
#else
#error "No test board defined"
#endif
#endif

void setup()
{
  /** Print All AirGradient board define */
  AirGradientBspPrint(NULL);

#if TEST_SENSOR_SenseAirS8
  /** Cause Serial is use default for PMS, CO2S8 should be use Serial 1 
   * Serial 1 will be init by SenseAirS8 don't need to init any more on user code
  */
  if (co2s8.begin(Serial1) == true)
  {
    log_i("CO2S8 sensor init success");
  }
  else
  {
    log_i("CO2S8 sensor init failure");
  }
#endif

#if TEST_SENSOR_PMS5003
  if (pms.begin() == true)
  {
    log_i("PMS5003 sensor init success");
  }
  else
  {
    log_i("PMS5003 sensor init failure");
  }
#endif

#if TEST_SENSOR_SHT4x || TEST_SENSOR_SGP4x
#if TEST_SENSOR_SHT4x
  if(sht.boardSupported())
  {
    Wire.begin(sht.sdaPin(), sht.sclPin());
  }
#else
  if(sgp.boardSupported())
  {
    Wire.begin(sgp.sdaPin(), sgp.sclPin());
  }
#endif
#endif

#if TEST_SENSOR_SHT4x

  if(sht.begin(Wire))
  {
    log_i("SHT init success");
  }
  else 
  {
    log_i("SHT init failed");
  }
#endif

#if TEST_SENSOR_SGP4x
  if(sgp.begin(Wire))
  {
    log_i("SGP init success");
  }
  else
  {
    log_e("SGP init failure");
  }
#endif

#if TEST_LED
  led.begin();
#endif

#if TEST_SWITCH
  sw.begin();
#endif
}

void loop()
{
  uint32_t ms;
#if TEST_SENSOR_SenseAirS8
  static uint32_t  lastTime = 0;

  /** Wait for sensor ready */
  if(co2s8.isReady())
  {
    // Get sensor data each 1sec
    ms = (uint32_t)(millis() - lastTime);
    if(ms >= 1000)
    {
      lastTime = millis();
      log_i("CO2: %d (PPM)", co2s8.getCO2(1));
    }
  }
  else 
  {
    log_i("Wait for sensor ready");
    delay(1000);
  }
#endif

#if TEST_SENSOR_PMS5003
  static uint32_t pmsTime = 0;
  ms = (uint32_t)(millis() - pmsTime);

  if(ms >= 1000)
  {
    pmsTime = millis();

    PMS5003::DATA data;
    if(pms.readUntil(data, 3000))
    {
      log_i("Passive mode PM 1.0  (ug/m3): %d", data.PM_AE_UG_1_0);
      log_i("Passive mode PM 2.5  (ug/m3): %d", data.PM_AE_UG_2_5);
      log_i("Passive mode PM 10.5 (ug/m3): %d", data.PM_AE_UG_10_0);
    }
    else 
    {
      log_i("PMS sensor read failure");
    }
  }
#endif

#if TEST_SENSOR_SHT4x
  /**
   * @brief Get SHT sensor data each 1sec
   * 
   */
  static uint32_t shtTime = 0;
  ms = (uint32_t)(millis() - shtTime);
  if(ms >= 1000)
  {
    shtTime = millis();
    float temperature, humidity;
    if(sht.measureHighPrecision(temperature, humidity))
    {
      log_i("SHT temperature: %f, humidity: %f", temperature, humidity);
    }
    else
    {
      log_e("SHT sensor read failure");
    }
  }
#endif

#if TEST_SENSOR_SGP4x
  static uint32_t sgpTime;
  ms = (uint32_t)(millis() - sgpTime);
  if(ms >= 1000)
  {
    sgpTime = millis();
    uint16_t rawVOC;
    if(sgp.getRawSignal(rawVOC))
    {
      log_i("Get SGP Raw VOC: %d", rawVOC);
    }
    else 
    {
      log_e("Get SGP Raw VOC failure");
    }
  }
#endif

#if TEST_LED
  static uint32_t ledTime;
#if TEST_BOARD_OUTDOOR_MONITOR_V1_3
  // ms = (uint32_t)(millis() - ledTime);
  // if(ms >= 500)
  // {
  //   ledTime = millis();
  //   led.ledToggle();
  // }
#elif TEST_BOARD_ONE_INDOOR_MONITOR_V9_0
  
  static int ledIndex;
  static int ledIndexOld;
  ms = (uint32_t)(millis() - ledTime);
  if(ms >= 50)
  {
    ledTime = millis();
    if(ledIndex == ledIndexOld)
    {
      led.ledOff();
    }
    else
    {
      // Turn last LED off
      led.ledSetColor(0, 0, 0, ledIndexOld);
    }

    // Turn new led ON
    led.ledSetColor(255, 0, 0, ledIndex);
    ledIndexOld = ledIndex;
    ledIndex++;
    if (ledIndex >= led.getNumberOfLed())
    {
      ledIndex = 0;
    }
  }
#else
#endif
  
#endif

#if TEST_SWITCH
  if(sw.getState() == AirGradientSwitch::Presssed)
  {
#if TEST_LED
    led.ledOn();
#endif
  }
  else 
  {
#if TEST_LED
    led.ledOff();
#endif
  }
  log_i("Switch state: %d", sw.getState());
  vTaskDelay(50);
#endif
}
