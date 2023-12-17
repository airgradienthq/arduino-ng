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
#if defined(ESP32)
#include "esp32-hal-log.h"
#endif

const AirGradientBSP_t bsps[BOARD_DEF_MAX] = 
{
  /** BOARD_DIY_BASIC_KIT */
  [0x00] = 
  {
    // [x] Suppported
    .CO2_S8 = 
    {
      .uart_tx_pin = 2,
      .uart_rx_pin = 0,
#if defined(ESP8266)
      .supported = true,
#else
      .supported = false,
#endif
    },
    // [ ] Suppported
    .PM2_5 = 
    {
      .uart_tx_pin = 14,
      .uart_rx_pin = 12,
#if defined(ESP8266)
      .supported = false,
#else
      .supported = false,
#endif
    },
		.name = "BOARD_DIY_BASIC_KIT",
  },
  /** BOARD_DIY_PRO_INDOOR_V4_2 */
  [0x01] = 
  {
    // [x] Suppported
    .CO2_S8 = 
    {
      .uart_tx_pin = 2,
      .uart_rx_pin = 0,
#if defined(ESP8266)
      .supported = true,
#else
      .supported = false,
#endif
    },
    // [ ] Suppported
    .PM2_5 = 
    {
      .uart_tx_pin = 1,
      .uart_rx_pin = 0,
#if defined(ESP8266)
      .supported = false,
#else
      .supported = false,
#endif
    },
		.name = "BOARD_DIY_PRO_INDOOR_V4_2",
  },
  /** BOARD_ONE_INDOOR_MONITOR_V9_0 */
  [0x02] = 
  {
    // [x] Suppported
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
    // [ ] Suppported
    .PM2_5 = 
    {
      .uart_tx_pin = 1,
      .uart_rx_pin = 0,
#if defined(ESP8266)
      .supported = false,
#else
      .supported = false,
#endif
    },
		.name = "BOARD_ONE_INDOOR_MONITOR_V9_0",
  },
  /** BOARD_OUTDOOR_MONITOR_V1_3 */
  [0x03] = 
  {
    // [x] Suppported
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
    // [ ] Suppported
    .PM2_5 = 
    {
      .uart_tx_pin = 1,
      .uart_rx_pin = 0,
#if defined(ESP8266)
      .supported = false,
#else
      .supported = false,
#endif
    },
		.name = "BOARD_OUTDOOR_MONITOR_V1_3",
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

#if defined(ESP8266)
#define bspPrintf(c, ...)                                       \
	if (_debug != nullptr)                                        \
	{                                                             \
		_debug->printf("[%s] " c "\r\n", this->TAG, ##__VA_ARGS__); \
	}
#else
#define bspPrintf(c, ...)\
  log_i(c, ##__VA_ARGS__)
#endif

/**
 * @brief Print list of support Board and sensor
 *
 * @param _debug Serial debug
 */
void BoardDefPrintBSP(Stream *_debug)
{
	if (_debug == NULL)
	{
		return;
	}

	for (int i = 0; i < BOARD_DEF_MAX; i++)
	{
		bspPrintf("Board name: %s", bsps[i].name);
		bspPrintf("\tSensor CO2 S8:");
		bspPrintf("\t\tSupported: %d", bsps[i].CO2_S8.supported);
		if (bsps[i].CO2_S8.supported)
		{
			bspPrintf("\t\tUART Tx: %d", bsps[i].CO2_S8.uart_tx_pin);
			bspPrintf("\t\tUART Rx: %d", bsps[i].CO2_S8.uart_rx_pin);
		}
		bspPrintf("\tSensor PMS2.5:");
		bspPrintf("\t\tSupported: %d", bsps[i].PM2_5.supported);
		if (bsps[i].PM2_5.supported)
		{
			bspPrintf("\t\tUART Tx: %d", bsps[i].PM2_5.uart_tx_pin);
			bspPrintf("\t\tUART Rx: %d", bsps[i].PM2_5.uart_rx_pin);
		}
	}
}
