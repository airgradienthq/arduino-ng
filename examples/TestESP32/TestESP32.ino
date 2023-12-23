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
#define TEST_BOARD_OUTDOOR_MONITOR_V1_3       0
#define TEST_BOARD_ONE_INDOOR_MONITOR_V9_0    1
/**
 * @brief Define test sensor
 */
#define TEST_SENSOR_SenseAirS8                1
#define TEST_SENSOR_PMS5003                   1
#define TEST_SENSOR_SHT4x                     1
#define TEST_SENSOR_SGP4x                     1
#define TEST_LED                              1
#define TEST_SWITCH                           0
#define TEST_OLED                             1

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

#if TEST_OLED

#if TEST_BOARD_OUTDOOR_MONITOR_V1_3
// AirGradientOled oled(BOARD_DIY_BASIC_KIT, Serial);
#elif TEST_BOARD_ONE_INDOOR_MONITOR_V9_0
AirGradientOled oled(BOARD_ONE_INDOOR_MONITOR_V9_0);
#else
#error "No test board defined"
#endif

#endif

const AirGradientBsp_t* bsp;

void watchDogInit(void);
void watchDogFeed(void);

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

  // Calib sensor
  // Disable ABC calib
  // bool result = co2s8.setCalibPeriodABC(0);
  // log_i("Disable auto calib: %s", result?"Success":"Failure");

  // bool result = co2s8.manualCalib();
  // log_i("Maunal calib: %s", result?"Success":"Failure");
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

#if TEST_SENSOR_SHT4x || TEST_SENSOR_SGP4x || TEST_OLED
  const AirGradientBsp_t* bsp = AirGradientBspGet(BOARD_ONE_INDOOR_MONITOR_V9_0);
  Wire.begin(AirGradientBspGet_I2C_SDA(bsp), AirGradientBspGet_I2C_SCL(bsp));
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

#if TEST_OLED
  oled.begin(Wire);

  oled.setTextSize(1);
  oled.setCursor(0, 0);
  oled.setTextColor(1);
  oled.setText("Hello");
  oled.display();
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
      log_i("CO2: %d (PPM)", co2s8.getCO2());
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

  watchDogFeed();
}

void watchDogInit(void)
{
  bsp = AirGradientBspGet(BOARD_ONE_INDOOR_MONITOR_V9_0);
  AirGradientBspWdgInit(bsp);
  log_i("WatchDog init");
}

void watchDogFeed(void)
{
  static uint32_t wdgTime = 0;
  static bool wdgBegin = false;
  uint32_t ms;
  
  ms = (uint32_t)(millis() - wdgTime);
  if (wdgBegin)
  {
    if (ms >= 30)
    {
      wdgBegin = false;
      wdgTime = millis();
      AirGradientBspWdgFeedEnd(bsp);
      log_i("Watchdog Feed end");
    }
  }
  else
  {
    if (ms >= 2500)
    {
      wdgTime = millis();
      AirGradientBspWdgFeedBegin(bsp);
      wdgBegin = true;
      log_i("Watchdog Feed begin");
    }
  }
}
