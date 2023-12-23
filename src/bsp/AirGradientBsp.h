/**
 * @file board_defs.h
 * @author Phat N. (phat@pdiytech.com)
 * @brief AirGradient board define
 * @version 0.1
 * @date 2023-Dec-16
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef _AIR_GRADIENT_BOARD_DEF_H_
#define _AIR_GRADIENT_BOARD_DEF_H_

#include <Arduino.h>

#if defined(ESP8266)
#define AgLog(c, ...)                                                   \
  if (this->_debugStream != nullptr)                                        \
  {                                                                         \
    this->_debugStream->printf("[%s] " c "\r\n", this->TAG, ##__VA_ARGS__); \
  }
#else
#include <esp32-hal-log.h>
#define AgLog(c, ...) \
  log_i(c, ##__VA_ARGS__)
#endif

typedef enum
{
  BOARD_DIY_BASIC_KIT           = 0x00,
  BOARD_DIY_PRO_INDOOR_V4_2     = 0x01,
  BOARD_ONE_INDOOR_MONITOR_V9_0 = 0x02,
  BOARD_OUTDOOR_MONITOR_V1_3    = 0x03,
  BOARD_DEF_MAX
}AirGradientBoardType_t;

struct AirGradientBsp_t
{
  /** Board Support CO2 SenseS8 */
  struct 
  {
    const int uart_tx_pin;  /** UART tx pin */
    const int uart_rx_pin;  /** UART rx pin */
    const bool    supported;    /** Is BSP supported for this sensor */
  } SenseAirS8;

  /** Board Support Plantower PMS5003 */
  struct 
  {
    const int uart_tx_pin;  /** UART tx pin */
    const int uart_rx_pin;  /** UART rx pin */
    const bool    supported;    /** Is BSP supported for this sensor */
  }PMS5003;

  /** I2C Bus */
  struct 
  {
    const int sda_pin;      /** I2C SDA pin */
    const int scl_pin;      /** I2C SCL pin */
    const bool supported;       /** Is BSP supported I2C communication */
  } I2C;

  /** Switch */
  struct 
  {
    const int pin;          /** Switch PIN */
    const int activeLevel;  /** Switch pressed level */
    const bool supported;   
  } SW;

  /** LED */
  struct 
  {
    const int pin;            /** Pin control */
    const int rgbNum;         /** Number of RGB LED */
    const int onState;        /** Single LED turn on state */
    const bool supported;     /** SUpported LED */
    const bool rgbSupported;  /** LED is RGB */
  }LED;

  /** OLED */
  struct 
  {
    const uint8_t width;    /** Display Width */
    const uint8_t height;   /** Display height */
    const uint8_t addr;     /** OLED I2C address */
    const bool supported;
  } OLED;

  /** Watchdog */
  struct
  {
    const uint8_t resetPin;
    const bool supported;
  } WDG;
  const char* name;
};

const AirGradientBsp_t* AirGradientBspGet(AirGradientBoardType_t def);
void AirGradientBspPrint(Stream* _debug);

bool AirGradientBspGet_I2C_Supported(const AirGradientBsp_t* bsp);
int  AirGradientBspGet_I2C_SDA(const AirGradientBsp_t* bsp);
int  AirGradientBspGet_I2C_SCL(const AirGradientBsp_t* bsp);

bool AirGradientBspGet_SW_Supported(const AirGradientBsp_t* bsp);
int  AirGradientBspGet_SW_Pin(const AirGradientBsp_t* bsp);
int  AirGradientBspGet_SW_ActiveLevel(const AirGradientBsp_t* bsp);

void AirGradientBspWdgInit(const AirGradientBsp_t* bsp);
void AirGradientBspWdgFeedBegin(const AirGradientBsp_t* bsp);
void AirGradientBspWdgFeedEnd(const AirGradientBsp_t* bsp);

#endif /** _AIR_GRADIENT_BOARD_DEF_H_ */
