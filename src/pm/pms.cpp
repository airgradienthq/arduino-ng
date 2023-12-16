/**
 * @file pms.cpp
 * @author Phat N. (phat@pdiytech.com)
 * @brief 
 * @version 0.1
 * @date 2023-Dec-16
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "pms.h"
#include "../bsp/dprint.h"

#if defined(ESP8266)
#include <SoftwareSerial.h>
#else
#endif

// AirPrintTagDef(PMS2_5);

/**
 * @brief Construct a new pms2 5::pms2 5 object
 * 
 * @param def AirGradient board define @ref AirGradientBoardDef_t
 * @param _debugStream Serial print debug log, NULL if don't use
 */
PMS2_5::PMS2_5(AirGradientBoardDef_t def, Stream *_debugStream) : _boardDef(def),
                                                                  _debugStream(_debugStream)
{
}

/**
 * @brief Construct a new pms2 5::pms2 5 object
 * 
 * @param def AirGradient board define @ref AirGradientBoardDef_t
 */
PMS2_5::PMS2_5(AirGradientBoardDef_t def) : _boardDef(def)
{
}

bool PMS2_5::begin(void)
{
  if (this->_isInit)
  {
    AirPrintf("Initialized, Call end() then try again");
    return true;
  }

  const AirGradientBSP_t *bsp = BoardDefGetBSP(this->_boardDef);
  if (bsp == NULL)
  {
    AirPrintf("Board Type not supported");
    return false;
  }

  if (bsp->PM2_5.supported == false)
  {
    AirPrintf("Board is not support this sensor");
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

bool PMS2_5::begin(Stream *_debugStream)
{
  this->_debugStream = _debugStream;
  return this->begin();
}

void PMS2_5::end(void)
{
  if(this->isInit() == false)
  {
    return;
  }

  // DeInit
  this->_isInit = false;
  AirPrintf("De-Initialized");
}

void PMS2_5::sleep(void)
{
  if(this->isInit() == false)
  {
    return;
  }
  uint8_t command[] = {0x42, 0x4D, 0xE4, 0x00, 0x00, 0x01, 0x73};
  this->_uartStream->write(command, sizeof(command));
}

void PMS2_5::wakeUp(void)
{
  if (this->isInit() == false)
  {
    return;
  }
  uint8_t command[] = {0x42, 0x4D, 0xE4, 0x00, 0x01, 0x01, 0x74};
  this->_uartStream->write(command, sizeof(command));
}

void PMS2_5::activeMode(void)
{
  if (this->isInit() == false)
  {
    return;
  }

  uint8_t command[] = {0x42, 0x4D, 0xE1, 0x00, 0x01, 0x01, 0x71};
  this->_uartStream->write(command, sizeof(command));
  this->_mode = MODE_ACTIVE;
}

void PMS2_5::passiveMode(void)
{
  if (this->isInit() == false)
  {
    return;
  }
  uint8_t command[] = {0x42, 0x4D, 0xE1, 0x00, 0x00, 0x01, 0x70};
  this->_uartStream->write(command, sizeof(command));
  this->_mode = MODE_PASSIVE;
}

bool PMS2_5::isReady(void)
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

bool PMS2_5::read(Data &data)
{
  if (this->isInit() == false)
  {
    return false;
  }

  this->_data = &data;
  this->_readLoop();

  return (this->_status = STATUS_OK);
}

bool PMS2_5::init(const AirGradientBSP_t *bsp)
{
  return this->init(bsp->PM2_5.uart_tx_pin, bsp->PM2_5.uart_rx_pin);
}
bool PMS2_5::init(int txPin, int rxPin)
{
  return this->init(txPin, rxPin, 9600);
}
bool PMS2_5::init(int txPin, int rxPin, uint32_t baud)
{
#if defined(ESP8266)
  SoftwareSerial *uart = new SoftwareSerial(txPin, rxPin);
  uart->begin(baud);
  this->_uartStream = uart;
#else
  Serial1.begin(baud, rxPin = rxPin, txPin = txPin);
  this->_uartStream = &Serial1;
#endif

  this->_isReady = false;
  const char* pm2Str = this->_getPMS2_5_Str();
  if(strcmp(pm2Str, "NULL") == 0)
  {
    AirPrintf("Sensor failed initialized");
    return false;
  }

  this->_lastInitTime = millis();
  AirPrintf("Sensor successfully initialized, Heating up 10s");
  return true;
}

/**
 * @brief Check that sensor is initialized
 *
 * @return true Initialized
 * @return false No-Initialized
 */
bool PMS2_5::isInit(void)
{
  if (this->_isInit)
  {
    return true;
  }
  AirPrintf("Sensor no-initialized");
  return false;
}


/**
 * @brief Read in passive mode
 * 
 */
void PMS2_5::_requestRead(void)
{
  if(this->_mode == Mode::MODE_PASSIVE)
  {
    uint8_t command[] = {0x42, 0x4D, 0xE2, 0x00, 0x00, 0x01, 0x71};
    this->_uartStream->write(command, sizeof(command));
  }
}

bool PMS2_5::_getRaw(Data& data)
{
  this->_requestRead();
  return this->_readUntil(data);
}

int PMS2_5::_getPM2_Raw(void)
{
  Data data;
  if (this->_getRaw(data))
  {
    return data.PM_RAW_2_5;
  }
  return -1;
}

int PMS2_5::_getPM1_Raw(void)
{
  Data data;
  if (this->_getRaw(data))
  {
    return data.PM_RAW_1_0;
  }
  return -1;
}

int PMS2_5::_getPM10_Raw(void)
{
  Data data;
  if (this->_getRaw(data))
  {
    return data.PM_RAW_10_0;
  }
  return -1;
}

bool PMS2_5::_readUntil(Data &data, uint16_t timeout)
{
  this->_data = &data;
  uint32_t start = millis();
  do
  {
    this->_readLoop();
    if (this->_status == STATUS_OK)
    {
      break;
    }
  } while ((uint32_t)(millis() - start) < timeout);

  return (this->_status == STATUS_OK);
}

void PMS2_5::_readLoop(void)
{
  this->_status = STATUS_WAITING;
  if (this->_uartStream->available())
  {
    uint8_t ch = this->_uartStream->read();

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
          this->_status = STATUS_OK;

          // Standard Particles, CF=1.
          _data->PM_SP_UG_1_0 = makeWord(_payload[0], _payload[1]);
          _data->PM_SP_UG_2_5 = makeWord(_payload[2], _payload[3]);
          _data->PM_SP_UG_10_0 = makeWord(_payload[4], _payload[5]);

          // Atmospheric Environment.
          _data->PM_AE_UG_1_0 = makeWord(_payload[6], _payload[7]);
          _data->PM_AE_UG_2_5 = makeWord(_payload[8], _payload[9]);
          _data->PM_AE_UG_10_0 = makeWord(_payload[10], _payload[11]);

          // Total particles count per 100ml air
          _data->PM_RAW_0_3 = makeWord(_payload[12], _payload[13]);
          _data->PM_RAW_0_5 = makeWord(_payload[14], _payload[15]);
          _data->PM_RAW_1_0 = makeWord(_payload[16], _payload[17]);
          _data->PM_RAW_2_5 = makeWord(_payload[18], _payload[19]);
          _data->PM_RAW_5_0 = makeWord(_payload[20], _payload[21]);
          _data->PM_RAW_10_0 = makeWord(_payload[22], _payload[23]);

          // Formaldehyde concentration (PMSxxxxST units only)
          _data->AMB_HCHO = makeWord(_payload[24], _payload[25]) / 1000;

          // Temperature & humidity (PMSxxxxST units only)
          _data->PM_TMP = makeWord(_payload[20], _payload[21]) / 10;
          _data->PM_HUM = makeWord(_payload[22], _payload[23]) / 10;
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

const char *PMS2_5::_getPMS2_5_Str(void)
{
  int result = this->_getPM2_Raw();
  if (result < 0)
  {
    Char_PM2[0] = 'N';
    Char_PM2[1] = 'U';
    Char_PM2[2] = 'L';
    Char_PM2[3] = 'L';
    Char_PM2[4] = 0; /** NULL terminal */
    return Char_PM2;
  }

  sprintf(Char_PM2, "%d", result);
  return Char_PM2;
}
