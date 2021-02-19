// Leka - LekaOS
// Copyright 2021 APF France handicap
// SPDX-License-Identifier: Apache-2.0

#include "LKCoreDSI.h"

#include "rtos/rtos.h"

#include "corevideo_config.h"

namespace leka {

using namespace std::literals::chrono_literals;

// TODO: move to header after class or use static inline (needs C++17) as described here:
// https://stackoverflow.com/a/50298253/2205264
DSI_HandleTypeDef LKCoreDSI::_hdsi;

LKCoreDSI::LKCoreDSI()
{
	// Base address of DSI Host/Wrapper registers to be set before calling De-Init
	_hdsi.Instance = DSI;

	// Set number of Lanes
	_hdsi.Init.NumberOfLanes = DSI_TWO_DATA_LANES;

	_hdsi.Init.TXEscapeCkdiv = dsi::txEscapeClockDiv;

	_config.VirtualChannelID = 0;
	_config.ColorCoding		 = DSI_RGB888;
	_config.VSPolarity		 = DSI_VSYNC_ACTIVE_HIGH;
	_config.HSPolarity		 = DSI_HSYNC_ACTIVE_HIGH;
	_config.DEPolarity		 = DSI_DATA_ENABLE_ACTIVE_HIGH;
	_config.Mode			 = DSI_VID_MODE_BURST;	 // Mode Video burst ie : one LgP per line
	_config.NullPacketSize	 = 0xFFF;
	_config.NumberOfChunks	 = 0;
	_config.PacketSize		 = lcd::property.HACT;	 // Value depending on display orientation choice portrait/landscape
	_config.HorizontalSyncActive = (lcd::property.HSA * dsi::laneByteClock_kHz) / dsi::lcdClock;
	_config.HorizontalBackPorch	 = (lcd::property.HBP * dsi::laneByteClock_kHz) / dsi::lcdClock;
	_config.HorizontalLine =
		((lcd::property.HACT + lcd::property.HSA + lcd::property.HBP + lcd::property.HFP) * dsi::laneByteClock_kHz) /
		dsi::lcdClock;	 // Value depending on display orientation choice portrait/landscape
	_config.VerticalSyncActive = lcd::property.VSA;
	_config.VerticalBackPorch  = lcd::property.VBP;
	_config.VerticalFrontPorch = lcd::property.VFP;
	_config.VerticalActive = lcd::property.VACT;   // Value depending on display orientation choice portrait/landscape

	// Enable or disable sending LP command while streaming is active in video mode
	_config.LPCommandEnable = DSI_LP_COMMAND_ENABLE;   // Enable sending commands in mode LP (Low Power)

	// Largest packet size possible to transmit in LP mode in VSA, VBP, VFP regions
	// Only useful when sending LP packets is allowed while streaming is active in video mode
	_config.LPLargestPacketSize = 16;

	// Largest packet size possible to transmit in LP mode in HFP region during VACT period
	// Only useful when sending LP packets is allowed while streaming is active in video mode
	_config.LPVACTLargestPacketSize = 0;

	// Specify for each region of the video frame, if the transmission of command in LP mode is allowed in this region
	// while streaming is active in video mode
	_config.LPHorizontalFrontPorchEnable = DSI_LP_HFP_ENABLE;	  // Allow sending LP commands during HFP period
	_config.LPHorizontalBackPorchEnable	 = DSI_LP_HBP_ENABLE;	  // Allow sending LP commands during HBP period
	_config.LPVerticalActiveEnable		 = DSI_LP_VACT_ENABLE;	  // Allow sending LP commands during VACT period
	_config.LPVerticalFrontPorchEnable	 = DSI_LP_VFP_ENABLE;	  // Allow sending LP commands during VFP period
	_config.LPVerticalBackPorchEnable	 = DSI_LP_VBP_ENABLE;	  // Allow sending LP commands during VBP period
	_config.LPVerticalSyncActiveEnable	 = DSI_LP_VSYNC_ENABLE;	  // Allow sending LP commands during VSync = VSA period
}

void LKCoreDSI::initialize()
{
	DSI_PLLInitTypeDef dsiPllInit;

	dsiPllInit.PLLNDIV = 100;
	dsiPllInit.PLLIDF  = DSI_PLL_IN_DIV5;
	dsiPllInit.PLLODF  = DSI_PLL_OUT_DIV1;

	HAL_DSI_DeInit(&(_hdsi));

	// Initialize DSI
	// DO NOT MOVE to the constructor as LCD initialization
	// must be performed in a very specific order
	HAL_DSI_Init(&(_hdsi), &(dsiPllInit));

	// Configure DSI Video mode timings
	HAL_DSI_ConfigVideoMode(&(_hdsi), &(_config));
}

void LKCoreDSI::start()
{
	HAL_DSI_Start(&_hdsi);
}

void LKCoreDSI::reset(void)
{
	// Reset DSI configuration
	// DO NOT CHANGE this function

	GPIO_InitTypeDef gpio_init_structure;

	__HAL_RCC_GPIOJ_CLK_ENABLE();

	// Configure the GPIO on PJ15 (MIPI_DSI_RESET)
	gpio_init_structure.Pin	  = GPIO_PIN_15;
	gpio_init_structure.Mode  = GPIO_MODE_OUTPUT_PP;
	gpio_init_structure.Pull  = GPIO_PULLUP;
	gpio_init_structure.Speed = GPIO_SPEED_HIGH;

	HAL_GPIO_Init(GPIOJ, &gpio_init_structure);

	// Activate MIPI_DSI_RESET active low
	HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_15, GPIO_PIN_RESET);

	ThisThread::sleep_for(20ms);

	// Desactivate MIPI_DSI_RESET
	HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_15, GPIO_PIN_SET);

	// Wait for 10ms after releasing MIPI_DSI_RESET before sending commands
	ThisThread::sleep_for(10ms);
}

DSI_VidCfgTypeDef LKCoreDSI::getConfig()
{
	return _config;
}

void LKCoreDSI::DSI_IO_WriteCmd(uint32_t NbrParams, uint8_t *pParams)
{
	if (NbrParams <= 1) {
		HAL_DSI_ShortWrite(&_hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, pParams[0], pParams[1]);
	} else {
		HAL_DSI_LongWrite(&_hdsi, 0, DSI_DCS_LONG_PKT_WRITE, NbrParams, pParams[NbrParams], pParams);
	}
}

}	// namespace leka

// Implementation mandatory st_otm8009a driver
void DSI_IO_WriteCmd(uint32_t NbrParams, uint8_t *pParams)
{
	leka::LKCoreDSI::DSI_IO_WriteCmd(NbrParams, pParams);
}
