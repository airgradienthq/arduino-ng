/**
 * @file sht.cpp
 * @author Phat N. (phat@pdiytech.com)
 * @brief AirGradient sensor library
 * @version 0.1
 * @date 2023-Dec-17
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "sht4x.h"
#include "../library/SensirionSHT4x/src/SensirionI2CSht4x.h"

#define shtSensor()     ((SensirionI2CSht4x*)(this->_sensor))

#if defined(ESP8266)
AirGradientSht::AirGradientSht(AirGradientBoardType_t type, Stream &stream):
  _debugStream(&stream),
  _boardType(type)
{
}

bool AirGradientSht::begin(TwoWire& wire, Stream& debugStream)
{
  this->_debugStream = &debugStream;
  return this->begin(wire);
}
#else

#endif

AirGradientSht::AirGradientSht(AirGradientBoardType_t type):
  _boardType(type)
{
}


bool AirGradientSht::begin(TwoWire& wire)
{
  if (this->_isInit)
  {
    return true;
  }

  if (this->boardSupported() == false)
  {
    return false;
  }
  this->_sensor = new SensirionI2CSht4x();
  shtSensor()->begin(wire, SHT40_I2C_ADDR_44);
  if(shtSensor()->softReset() != 0)
  {
    AgLog("Reset sensor fail, look like sensor is not on I2C bus");
    return false;
  }

  delay(10);

  this->_isInit = true;
  AgLog("Init");
  return true;
}

void AirGradientSht::end(void)
{
  if(this->_isInit == false)
  {
    return;
  }

  this->_isInit = false;
  delete shtSensor();
}

bool AirGradientSht::boardSupported(void)
{
  if (this->_bsp == NULL)
  {
    this->_bsp = AirGradientBspGet(this->_boardType);
  }

  if ((this->_bsp == NULL) || (this->_bsp->I2C.supported == false))
  {
    AgLog("Board not supported");
    return false;
  }
  return true;
}

int AirGradientSht::sdaPin(void)
{
  if(this->_bsp)
  {
    return this->_bsp->I2C.sda_pin;
  }
  AgLog("sdaPin(): board not supported I2C");
  return -1;
}

int AirGradientSht::sclPin(void)
{
  if(this->_bsp)
  {
    return this->_bsp->I2C.scl_pin;
  }
  AgLog("sdlPin(): board not supported I2C");
  return -1;
}

bool AirGradientSht::checkInit(void)
{
  if (this->_isInit)
  {
    return true;
  }
  AgLog("Sensor no-initialized");
  return false;
}

bool AirGradientSht::measureHighPrecision(float &temperature, float &humidity)
{
  if (this->checkInit() == false)
  {
    return false;
  }

  if (shtSensor()->measureHighPrecision(temperature, humidity) == 0)
  {
    return true;
  }
  return false;
}

bool AirGradientSht::measureMediumPrecision(float &temperature, float &humidity)
{
  if (this->checkInit() == false)
  {
    return false;
  }

  if (shtSensor()->measureMediumPrecision(temperature, humidity) == 0)
  {
    return true;
  }
  return false;
}

bool AirGradientSht::measureLowestPrecision(float &temperature, float &humidity)
{
  if (this->checkInit() == false)
  {
    return false;
  }

  if (shtSensor()->measureLowestPrecision(temperature, humidity) == 0)
  {
    return true;
  }
  return false;
}

bool AirGradientSht::activateHighestHeaterPowerShort(float &temperature, float &humidity)
{
  if (this->checkInit() == false)
  {
    return false;
  }

  if (shtSensor()->activateHighestHeaterPowerShort(temperature, humidity) == 0)
  {
    return true;
  }
  return false;
}

bool AirGradientSht::activateMediumHeaterPowerLong(float &temperature, float &humidity)
{
  if (this->checkInit() == false)
  {
    return false;
  }

  if (shtSensor()->activateMediumHeaterPowerLong(temperature, humidity) == 0)
  {
    return true;
  }
  return false;
}

bool AirGradientSht::activateLowestHeaterPowerLong(float &temperature, float &humidity)
{
  if (this->checkInit() == false)
  {
    return false;
  }

  if (shtSensor()->activateLowestHeaterPowerLong(temperature, humidity) == 0)
  {
    return true;
  }
  return false;
}

bool AirGradientSht::getSerialNumber(uint32_t &serialNumber)
{
  if (this->checkInit() == false)
  {
    return false;
  }

  if (shtSensor()->serialNumber(serialNumber) == 0)
  {
    return true;
  }
  return false;
}

bool AirGradientSht::softReset(void)
{
  if (this->checkInit() == false)
  {
    return false;
  }

  if (shtSensor()->softReset() == 0)
  {
    return true;
  }
  return false;
}
