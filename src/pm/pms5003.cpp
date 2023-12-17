/**
 * @file pms5003.cpp
 * @author Phat N. (phat@pdiytech.com)
 * @brief AirGradient Plantower PMS5003 sensor
 * @version 0.1
 * @date 2023-Dec-17
 * 
 * @copyright Copyright (c) 2023
 * 
 */


#include "Arduino.h"
#include "pms5003.h"

#if defined(ESP8266)
#include <SoftwareSerial.h>

/**
 * @brief Construct a new PMS5003::PMS5003 object
 * 
 * @param def Board type @ref AirGradientBoardType_t
 */
PMS5003::PMS5003(AirGradientBoardType_t def)
{
  this->_boardDef = def;
  this->_debugStream = NULL;
}
#else
#endif

/**
 * @brief Construct a new PMS5003::PMS5003 object
 * 
 * @param def Board type @ref AirGradientBoardType_t
 * @param stream For ESP32 this is Serial, for ESP8266 this is debug print output
 */
PMS5003::PMS5003(AirGradientBoardType_t def, Stream &stream)
{
#if defined(ESP8266)
  this->_debugStream = &stream;
#else
  this->_stream = &stream;
#endif
  this->_boardDef = def;
}

/**
 * @brief Init sensor
 * 
 * @return true Success
 * @return false Failure
 */
bool PMS5003::begin(void)
{
  if (this->_isInit)
  {
    return true;
  }

  this->bsp = AirGradientBspGet(this->_boardDef);
  if (bsp == NULL)
  {
    AgLog("Board [%d] not supported", this->_boardDef);
    return false;
  }

  if (bsp->PMS5003.supported == false)
  {
    AgLog("Board [%d] PMS50035003 not supported", this->_boardDef);
    return false;
  }

#if defined(ESP8266)
  bsp->PMS5003.uart_tx_pin;
  SoftwareSerial *uart = new SoftwareSerial(bsp->PMS5003.uart_tx_pin, bsp->PMS5003.uart_rx_pin);
  uart->begin(PMS5003::BAUD_RATE);
  this->_stream = uart;
#else
#endif

  int timeout = 1000; /** Timeout 1 sec */
  DATA data;
  if (this->_readUntil(data, timeout) == false)
  {
    AgLog("Read sensor timeout");
    return false;
  }

  this->_isInit = true;
  return true;
}

/**
 * @brief Put sensor sleep for low power consumption.
 * 
 */
void PMS5003::sleep()
{
  if(this->checkInit() == false)
  {
    return;
  }

  if(this->_isSleep)
  {
    AgLog("Senor sleeping");
    return;
  }

  uint8_t command[] = {0x42, 0x4D, 0xE4, 0x00, 0x00, 0x01, 0x73};
  _stream->write(command, sizeof(command));
  this->_isSleep = true;
  AgLog("Sleep");
}

/**
 * @brief Wakeup sensor, after wakeup should be wait for fan stable for get sensor data
 * 
 */
void PMS5003::wakeUp()
{
  if(this->checkInit() == false)
  {
    return;
  }

  if(this->_isSleep == false)
  {
    AgLog("Sensor is not sleeped");
    return;
  }

  uint8_t command[] = {0x42, 0x4D, 0xE4, 0x00, 0x01, 0x01, 0x74};
  _stream->write(command, sizeof(command));
  this->_isSleep = false;
  AgLog("Wakeup");
}

/**
 * @brief Default mode after power up. In this mode sensor would send serial 
 * data to the host automatically.
 * 
 */
void PMS5003::activeMode()
{
  if(this->checkInit() == false)
  {
    return;
  }

  uint8_t command[] = {0x42, 0x4D, 0xE1, 0x00, 0x01, 0x01, 0x71};
  _stream->write(command, sizeof(command));
  _mode = MODE_ACTIVE;
}

/**
 * @brief In this mode sensor would send serial data to the host only for request.
 * 
 */
void PMS5003::passiveMode()
{
  if(this->checkInit() == false)
  {
    return;
  }

  uint8_t command[] = {0x42, 0x4D, 0xE1, 0x00, 0x00, 0x01, 0x70};
  _stream->write(command, sizeof(command));
  _mode = MODE_PASSIVE;
}

/**
 * @brief Request read in Passive Mode.
 * 
 */
void PMS5003::requestRead()
{
  if(this->checkInit() == false)
  {
    return;
  }

  if (_mode == MODE_PASSIVE)
  {
    uint8_t command[] = {0x42, 0x4D, 0xE2, 0x00, 0x00, 0x01, 0x71};
    _stream->write(command, sizeof(command));
  }
}

/**
 * @brief Read sensor data in active mode. Non-blocking function for parse response.
 * 
 * @param data [out] Point to structure data
 * @return true Read success
 * @return false Read failure
 */
bool PMS5003::read(DATA &data)
{
  if(this->checkInit() == false)
  {
    return false;
  }

  _data = &data;
  loop();

  return _status == STATUS_OK;
}

/**
 * @brief Blocking function for parse response.
 * 
 * @param data 
 * @param timeout Read timeout, default 1sec
 * @return true Read sucess
 * @return false Read failure
 */
bool PMS5003::readUntil(DATA &data, uint16_t timeout)
{
  if(this->checkInit() == false)
  {
    return false;
  }
  _data = &data;
  uint32_t start = millis();

  // Clear rx buffer
  this->_stream->flush();
  do
  {
    loop();
    if (_status == STATUS_OK)
      break;
  } while (millis() - start < timeout);

  return _status == STATUS_OK;
}

/**
 * @brief Parse data package
 * 
 */
void PMS5003::loop()
{
  _status = STATUS_WAITING;
  if (_stream->available())
  {
    uint8_t ch = _stream->read();

    switch (_index)
    {
    case 0:
      if (ch != 0x42)
      {
        return;
      }
      _calculatedChecksum = ch;
      break;

    case 1:
      if (ch != 0x4D)
      {
        _index = 0;
        return;
      }
      _calculatedChecksum += ch;
      break;

    case 2:
      _calculatedChecksum += ch;
      _frameLen = ch << 8;
      break;

    case 3:
      _frameLen |= ch;
      // Unsupported sensor, different frame length, transmission error e.t.c.
      if (_frameLen != 2 * 9 + 2 && _frameLen != 2 * 13 + 2)
      {
        _index = 0;
        return;
      }
      _calculatedChecksum += ch;
      break;

    default:
      if (_index == _frameLen + 2)
      {
        _checksum = ch << 8;
      }
      else if (_index == _frameLen + 2 + 1)
      {
        _checksum |= ch;

        if (_calculatedChecksum == _checksum)
        {
          _status = STATUS_OK;

          // Standard Particles, CF=1.
          _data->PM_SP_UG_1_0 = makeWord(_payload[0], _payload[1]);
          _data->PM_SP_UG_2_5 = makeWord(_payload[2], _payload[3]);
          _data->PM_SP_UG_10_0 = makeWord(_payload[4], _payload[5]);

          // Atmospheric Environment.
          _data->PM_AE_UG_1_0 = makeWord(_payload[6], _payload[7]);
          _data->PM_AE_UG_2_5 = makeWord(_payload[8], _payload[9]);
          _data->PM_AE_UG_10_0 = makeWord(_payload[10], _payload[11]);
        }

        _index = 0;
        return;
      }
      else
      {
        _calculatedChecksum += ch;
        uint8_t payloadIndex = _index - 4;

        // Payload is common to all sensors (first 2x6 bytes).
        if (payloadIndex < sizeof(_payload))
        {
          _payload[payloadIndex] = ch;
        }
      }

      break;
    }

    _index++;
  }
}

/**
 * @brief Block read sensor data
 * 
 * @param data [out] Point to data
 * @param timeout Read timeout
 * @return true Read success
 * @return false Read failure
 */
bool PMS5003::_readUntil(DATA &data, uint16_t timeout)
{
  _data = &data;
  uint32_t start = millis();
  do
  {
    loop();
    if (_status == STATUS_OK)
      break;
  } while (millis() - start < timeout);

  return _status == STATUS_OK;
}

/**
 * @brief Check device initialized or not
 * 
 * @return true Initialized
 * @return false No-initialized
 */
bool PMS5003::checkInit(void)
{
  if(this->_isInit == false)
  {
    AgLog("No initialized");
    return false;
  }
  return true;
}