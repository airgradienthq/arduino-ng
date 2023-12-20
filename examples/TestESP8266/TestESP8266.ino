#include <AirGradient.h>
#include <Wire.h>

/**
 * @brief Define test board
 */
#define TEST_BOARD_DIY_BASIC_KIT        0
#define TEST_BOARD_DIY_PRO_INDOOR_V4_2  1

/**
 * @brief Define test sensor
 */
#define TEST_SENSOR_SenseAirS8          0
#define TEST_SENSOR_PMS5003             0
#define TEST_SENSOR_SHT4x               0
#define TEST_SENSOR_SGP4x               0
#define TEST_SWITCH                     1

#if TEST_SENSOR_SenseAirS8

#if TEST_BOARD_DIY_BASIC_KIT
SenseAirS8 co2s8(BOARD_DIY_BASIC_KIT);
#elif TEST_BOARD_DIY_PRO_INDOOR_V4_2
SenseAirS8 co2s8(BOARD_DIY_PRO_INDOOR_V4_2);
#else
#error "No test board defined"
#endif /** TEST_BOARD_DIY_BASIC_KIT */

#endif /** TEST_SENSOR_SenseAirS8 */

#if TEST_SENSOR_PMS5003
#if TEST_BOARD_DIY_BASIC_KIT
PMS5003 pms(BOARD_DIY_BASIC_KIT);
#elif TEST_BOARD_DIY_PRO_INDOOR_V4_2
PMS5003 pms(BOARD_DIY_PRO_INDOOR_V4_2);
#else
#error "No test board defined"
#endif /** TEST_BOARD_DIY_BASIC_KIT */
#endif /** TEST_SENSOR_PMS5003 */

#if TEST_SENSOR_SHT4x
#if TEST_BOARD_DIY_BASIC_KIT
AirGradientSht sht(BOARD_DIY_BASIC_KIT, Serial);
#elif TEST_BOARD_DIY_PRO_INDOOR_V4_2
AirGradientSht sht(BOARD_DIY_PRO_INDOOR_V4_2, Serial);
#else
#error "No test board defined"
#endif
#endif /** TEST_SENSOR_SHT4x */

#if TEST_SENSOR_SGP4x
#if TEST_BOARD_DIY_BASIC_KIT
AirGradientSgp4x sgp(BOARD_DIY_BASIC_KIT, Serial);
#elif TEST_BOARD_DIY_PRO_INDOOR_V4_2
AirGradientSgp4x sgp(BOARD_DIY_PRO_INDOOR_V4_2, Serial);
#else
#error "No test board defined"
#endif
#endif /** TEST_SENSOR_SGP4x */

#if TEST_SWITCH
#if TEST_BOARD_DIY_BASIC_KIT
#elif TEST_BOARD_DIY_PRO_INDOOR_V4_2
AirGradientSwitch sw(BOARD_DIY_PRO_INDOOR_V4_2, Serial);
#else
#error "No test board defined"
#endif
#endif

void setup()
{
  Serial.begin(115200);

  /** Print All AirGradient board define */
  AirGradientBspPrint(&Serial);

#if TEST_SENSOR_SenseAirS8
  if (co2s8.begin(&Serial) == true)
  {
    Serial.println("CO2S8 sensor init success");
  }
  else
  {
    Serial.println("CO2S8 sensor init failure");
  }
#endif

#if TEST_SENSOR_PMS5003
  if (pms.begin(&Serial) == true)
  {
    Serial.println("PMS5003 sensor init success");
  }
  else
  {
    Serial.println("PMS5003 sensor init failure");
  }
#endif

#if TEST_SENSOR_SHT4x || TEST_SENSOR_SGP4x
  Wire.begin();
#endif

#if TEST_SENSOR_SHT4x
  if(sht.begin(Wire ,Serial))
  {
    Serial.println("SHT init success");
  }
  else 
  {
    Serial.println("SHT init failed");
  }
#endif

#if TEST_SENSOR_SGP4x
  if (sgp.begin(Wire))
  {
    Serial.println("SGP init succses");
  }
  else
  {
    Serial.println("SGP init failure");
  }
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
      Serial.printf("CO2: %d (PMM)\r\n", co2s8.getCO2(1));
    }
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
      Serial.printf("Passive mode PM 1.0  (ug/m3): %d\r\n", data.PM_AE_UG_1_0);
      Serial.printf("Passive mode PM 2.5  (ug/m3): %d\r\n", data.PM_AE_UG_2_5);
      Serial.printf("Passive mode PM 10.5 (ug/m3): %d\r\n", data.PM_AE_UG_10_0);
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
      Serial.printf("SHT temperature: %f, humidity: %f\r\n", temperature, humidity);
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
      Serial.printf("Get SGP Raw VOC: %d\r\n", rawVOC);
    }
    else 
    {
      Serial.println("Get SGP Raw VOC failure");
    }
  }
#endif

#if TEST_SWITCH
  static uint8_t swOld;
  uint8_t state = (uint8_t)sw.getState();
  if(swOld != state)
  {
    swOld = state;
    Serial.printf("Switch changed: %s\r\n", state == (uint8_t)AirGradientSwitch::Presssed ? "Pressed" : "Released");
  }
#endif
}
