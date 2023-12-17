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

// FIXME Just for test
// #define ESP8266

#include "Arduino.h"

typedef enum
{
  BOARD_DIY_BASIC_KIT           = 0x00,
  BOARD_DIY_PRO_INDOOR_V4_2     = 0x01,
  BOARD_ONE_INDOOR_MONITOR_V9_0 = 0x02,
  BOARD_OUTDOOR_MONITOR_V1_3    = 0x03,
  BOARD_DEF_MAX
}AirGradientBoardDef_t;

struct AirGradientBSP_t
{
  /** Board Support CO2 SenseS8 */
  struct 
  {
    const uint8_t uart_tx_pin;  /** UART tx pin */
    const uint8_t uart_rx_pin;  /** UART rx pin */
    const bool    supported;    /** Is BSP supported for this sensor */
  } CO2_S8;

  /** Board Support Plantower PM2.5 */
  struct 
  {
    const uint8_t uart_tx_pin;  /** UART tx pin */
    const uint8_t uart_rx_pin;  /** UART rx pin */
    const bool    supported;    /** Is BSP supported for this sensor */
  }PM2_5;
  const char* name;
};

const AirGradientBSP_t* BoardDefGetBSP(AirGradientBoardDef_t def);
void BoardDefPrintBSP(Stream* _debug);

#endif /** _AIR_GRADIENT_BOARD_DEF_H_ */
