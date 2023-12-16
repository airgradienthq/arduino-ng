/**
 * @file board_defs.cpp
 * @author Phat N. (phat@pdiytech.com)
 * @brief AirGradient board define
 * @version 0.1
 * @date 2023-Dec-16
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "board_defs.h"

const AirGradientBSP_t bsps[BOARD_DEF_MAX] = 
{
  /** BOARD_DIY_BASIC_KIT_V4_2 */
  [0x00] = 
  {
    .CO2_S8 = 
    {
      .uart_tx_pin = 1,
      .uart_rx_pin = 0,
#if defined(ESP8266)
      .supported = true,
#else
      .supported = false,
#endif
    },
    .PM2_5 = 
    {
      .uart_tx_pin = 1,
      .uart_rx_pin = 0,
#if defined(ESP8266)
      .supported = true,
#else
      .supported = false,
#endif
    },
  },

  /** BOARD_INDOOR_MONITOR_V1_3 */
  [0x01] = 
  {
    .CO2_S8 = 
    {
      .uart_tx_pin = 1,
      .uart_rx_pin = 0,
#if defined(ESP8266)
      .supported = true,
#else
      .supported = false,
#endif
    },
    .PM2_5 = 
    {
      .uart_tx_pin = 1,
      .uart_rx_pin = 0,
#if defined(ESP8266)
      .supported = true,
#else
      .supported = false,
#endif
    },
  },
  /** BOARD_ONE_INDOOR_MONITOR_V9_0 */
  [0x02 ] = 
  {
    .CO2_S8 = 
    {
      .uart_tx_pin = 1,
      .uart_rx_pin = 0,
#if defined(ESP8266)
      .supported = false,
#else
      .supported = true,
#endif
    },
    .PM2_5 = 
    {
      .uart_tx_pin = 1,
      .uart_rx_pin = 0,
#if defined(ESP8266)
      .supported = false,
#else
      .supported = true,
#endif
    },
  },
  /** BOARD_OUTDOOR_MONITOR_V1_3 */
  [0x03] = 
  {
    .CO2_S8 = 
    {
      .uart_tx_pin = 1,
      .uart_rx_pin = 0,
#if defined(ESP8266)
      .supported = false,
#else
      .supported = true,
#endif
    },
    .PM2_5 = 
    {
      .uart_tx_pin = 1,
      .uart_rx_pin = 0,
#if defined(ESP8266)
      .supported = false,
#else
      .supported = true,
#endif
    },
  }
};

/**
 * @brief Get Board Support Package
 * 
 * @param def Board define @ref AirGradientBoardDef_t
 * @return const AirGradientBSP_t* 
 */
const AirGradientBSP_t *BoardDefGetBSP(AirGradientBoardDef_t def)
{
  if (def >= BOARD_DEF_MAX)
  {
    return NULL;
  }
  return &bsps[def];
}
