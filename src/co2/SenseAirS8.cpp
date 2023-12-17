/**
 * @file s8.cpp
 * @author Phat N. (phat@pdiytech.com)
 * @brief Arigradient libarary for Senseair S8 NDIR CO2 sensor
 * @version 0.1
 * @date 2023-Dec-16
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "SenseAirS8.h"
#if defined(ESP8266)
#include <SoftwareSerial.h>
#else
#endif

// AirPrintTagDef(SenseAirS8);
#if defined(ESP8266)
/**
 * @brief Construct a new co2 s8::co2 s8 object
 *
 * @param def AirGradient board define @ref AirGradientBoardType_t
 * @param _debugStream Serial print debug log, NULL if don't use
 */
SenseAirS8::SenseAirS8(AirGradientBoardType_t def, Stream *_debugStream) : _debugStream(_debugStream), _boardDef(def)
{
}
#endif

/**
 * @brief Construct a new co2 s8::co2 s8 object
 *
 * @param def AirGradient board define @ref AirGradientBoardType_t
 */
SenseAirS8::SenseAirS8(AirGradientBoardType_t def) : _boardDef(def)
{
}

#if defined(ESP8266)
/**
 * @brief Init sensor
 * @return true = success, otherwise is failure
 */
bool SenseAirS8::begin(void)
{
  if (this->_isInit)
  {
    AgLog("Initialized, Call end() then try again");
    return true;
  }
  
  return this->_begin();
}

/**
 * @brief Init sensor has print debug log, if class create without serial debug
 *        before it's override last define
 *
 * @param _debugStream Serial print debug log, NULL if don't use
 * @return true = success, otherwise is failure
 */
bool SenseAirS8::begin(Stream *_debugStream)
{
  this->_debugStream = _debugStream;
  return this->begin();
}
#else
/**
 * @brief Init sensor 
 * 
 * @param serial Target Serial use for 
 * @return true Success
 * @return false Failure
 */
bool SenseAirS8::begin(HardwareSerial* serial)
{
  if(serial == NULL)
  {
    AgLog("param invalid: null");
    return false;
  }

  this->_serial = serial;
  return this->_begin();
}

#endif

/**
 * @brief De-Initialize sensor and release peripheral resource
 *
 */
void SenseAirS8::end(void)
{
  if (this->_isInit == false)
  {
    AgLog("Senor is not initialized");
    return;
  }

  // Deinit
  AgLog("De-Inititlized");
}

/**
 * @brief Check that device is ready after init
 *
 * @return true
 * @return false
 */
bool SenseAirS8::isReady(void)
{
  if (this->isInit() == false)
  {
    return false;
  }

  if (this->_isReady)
  {
    return true;
  }

  uint32_t ms = (uint32_t)(millis() - this->_lastInitTime);

  // Wait for 10s after init for heating up ready
  if (ms >= 10000)
  {
    this->_isReady = true;
  }
  return this->_isReady;
}

/**
 * @brief Get CO2 sensor raw data
 *
 * @return int -1 if failure
 */
int SenseAirS8::getRaw(void)
{
  if (this->isInit() == false)
  {
    return -1;
  }
  return this->_getRaw();
}

/**
 * @brief Get sensor data
 *
 * @param samples Average number of samples
 * @return int data (ppm)
 */
int SenseAirS8::getCO2(int numberOfSamplesToTake)
{
  if (this->isInit() == false)
  {
    return -1;
  }

  if (numberOfSamplesToTake <= 0)
  {
    AgLog("samples must be larger than '0'");
    return -2;
  }

  int successfulSamplesCounter = 0;
  int co2AsPpmSum = 0;
  for (int sample = 0; sample < numberOfSamplesToTake; sample++)
  {
    int co2AsPpm = this->getRaw();
    if (co2AsPpm > 300 && co2AsPpm < 10000)
    {
      AgLog("CO2 read success %d", co2AsPpm);
      successfulSamplesCounter++;
      co2AsPpmSum += co2AsPpm;
    }
    else
    {
      AgLog("CO2 read failed with %d", co2AsPpm);
    }

    // without delay we get a few 10ms spacing, add some more
    delay(250);
  }

  return co2AsPpmSum / successfulSamplesCounter;
}

/**
 * @brief Init sensor with BSP
 *
 * @param bsp AirGradient BSP @ref AirGradientBsp_t
 * @return true
 * @return false
 */
bool SenseAirS8::init(const AirGradientBsp_t *bsp)
{
  return this->init(bsp->SenseAirS8.uart_tx_pin, bsp->SenseAirS8.uart_rx_pin);
}

/**
 * @brief Init sensor
 *
 * @param txPin UART TX pin
 * @param rxPin UART RX pin
 * @return true = success, otherwise failure
 */
bool SenseAirS8::init(int txPin, int rxPin)
{
  return this->init(txPin, rxPin, 9600);
}

bool SenseAirS8::_begin(void)
{
  const AirGradientBsp_t *bsp = AirGradientBspGet(this->_boardDef);
  if (bsp == NULL)
  {
    AgLog("Board Type not supported");
    return false;
  }

  if (bsp->SenseAirS8.supported == false)
  {
    AgLog("Board is not support this sensor");
    return false;
  }

  // Init sensor
  if (this->init(bsp) == false)
  {
    return false;
  }

  this->_isInit = true;
  return true;
}

/**
 * @brief Init sensor
 *
 * @param txPin UART TX pin
 * @param rxPin UART RX pin
 * @param baud UART speed
 * @return true = success, otherwise failure
 */
bool SenseAirS8::init(int txPin, int rxPin, uint32_t baud)
{
#if defined(ESP8266)
  SoftwareSerial *uart = new SoftwareSerial(txPin, rxPin);
  uart->begin(baud);
  this->_uartStream = uart;
#else
  this->_serial->begin(baud, SERIAL_8N1, rxPin, txPin);
  // Serial1.begin(baud, SERIAL_8N1, rxPin, txPin);
  this->_uartStream = this->_serial;
#endif

  this->_isReady = false;

  int result = this->_getRaw();
  if (result < 0)
  {
    AgLog("Sensor failed to initialize");
    return false;
  }

  AgLog("Sensor successfully initialized. Heating up for 10s");
  this->_lastInitTime = millis();
  return true;
}

/**
 * @brief Check that sensor is initialized
 *
 * @return true Initialized
 * @return false No-Initialized
 */
bool SenseAirS8::isInit(void)
{
  if (this->_isInit)
  {
    return true;
  }
  AgLog("Sensor no-initialized");
  return false;
}

/**
 * @brief Get raw data
 *
 * @return int Sensor data
 */
int SenseAirS8::_getRaw(void)
{
  // Clear rx buffer
  this->_uartStream->flush();

  const byte CO2Command[] = {0xFE, 0x04, 0x00, 0x03, 0x00, 0x01, 0xD5, 0xC5};
  byte CO2Response[] = {0, 0, 0, 0, 0, 0, 0};
  int datapos = -1;
  const int commandSize = 8;
  const int responseSize = 7;

  int numberOfBytesWritten = this->_uartStream->write(CO2Command, commandSize);
  if (numberOfBytesWritten != commandSize)
  {
    AgLog("Fail to write request");
    return -2;
  }

  // attempt to read response
  int timeoutCounter = 0;
  while (this->_uartStream->available() < responseSize)
  {
    timeoutCounter++;
    if (timeoutCounter > 10)
    {
      AgLog("Timeout when reading response");
      return -3;
    }
    delay(50);
  }

  // we have 7 bytes ready to be read
  for (int i = 0; i < responseSize; i++)
  {
    CO2Response[i] = this->_uartStream->read();
    if ((CO2Response[i] == 0xFE) && (datapos == -1))
    {
      datapos = i;
    }
    // AgLog("0x%02x", (uint8_t)CO2Response[i]);
  }

  return CO2Response[datapos + 3] * 256 + CO2Response[datapos + 4];
}