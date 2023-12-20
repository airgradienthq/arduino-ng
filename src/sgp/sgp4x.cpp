/**
 * @file sgp4x.cpp
 * @author Phat N. (phat@pdiytech.com)
 * @brief AirGradient library for SGP4x from Sensirion, this lib is covert libaray
 *        publish by Sensirion https://github.com/Sensirion/arduino-i2c-sgp40
 * @version 0.1
 * @date 2023-Dec-20
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "sgp4x.h"

#if defined(ESP8266)
AirGradientSgp4x::AirGradientSgp4x(AirGradientBoardType_t type, Stream& stream):
  _boardType(type),
  _debugStream(&stream)
{

}
#else
#endif

AirGradientSgp4x::AirGradientSgp4x(AirGradientBoardType_t type):
  _boardType(type)
{

}

bool AirGradientSgp4x::begin(TwoWire& wire)
{
  if(this->_isInit)
  {
    return true;
  }
  if(this->boardSupported() == false)
  {
    return false;
  }

  this->_sensor.begin(wire);

  uint16_t testResult;
  if (this->_sensor.executeSelfTest(testResult) != 0)
  {
    return false;
  }
  if(testResult != 0xD400)
  {
    AgLog("Self-test failed");
    return false;
  }

  this->_isInit = true;
  AgLog("Init");
  return true;
}

void AirGradientSgp4x::end(void)
{
  if(this->_isInit == false)
  {
    return;
  }

  this->_isInit = false;
  
  AgLog("De-Init");
}

bool AirGradientSgp4x::boardSupported(void)
{
  if(this->bsp == nullptr)
  {
    this->bsp = AirGradientBspGet(this->_boardType);
  }

  if ((this->bsp == nullptr) || (this->bsp->I2C.supported == false))
  {
    AgLog("Board not supported");
    return false;
  }
  return true;
}

int AirGradientSgp4x::sdaPin(void)
{
  if(this->bsp)
  {
    return this->bsp->I2C.sda_pin;
  }
  AgLog("sdaPin(): board not supported I2C");
  return -1;
}

int AirGradientSgp4x::sclPin(void)
{
  if(this->bsp)
  {
    return this->bsp->I2C.scl_pin;
  }
  AgLog("sdlPin(): board not supported I2C");
  return -1;
}

bool AirGradientSgp4x::getRawSignal(uint16_t& raw, uint16_t defaultRh, uint16_t defaultT)
{
  if (this->checkInit() == false)
  {
    return false;
  }

  if (this->_sensor.measureRawSignal(defaultRh, defaultT, raw) == 0)
  {
    return true;
  }

  return false;
}

/**
 * @brief This command turns the hotplate off and stops the measurement. 
 *        Subsequently, the sensor enters the idle mode.
 * 
 * @return true Success
 * @return false Failure
 */
bool AirGradientSgp4x::turnHeaterOff(void)
{
  if(this->checkInit() == false)
  {
    return false;
  }

  if (this->_sensor.turnHeaterOff() == 0)
  {
    return true;
  }
  return false;
}

bool AirGradientSgp4x::getSerialNumber(uint16_t serialNumbers[], uint8_t serialNumberSize)
{
  if(this->checkInit() == false)
  {
    return false;
  }

  if (this->_sensor.getSerialNumber(serialNumbers, serialNumberSize) == 0)
  {
    return true;
  }
  return false;
}

bool AirGradientSgp4x::checkInit(void)
{
  if (this->_isInit)
  {
    return true;
  }
  AgLog("Sensor no-initialized");
  return false;
}
