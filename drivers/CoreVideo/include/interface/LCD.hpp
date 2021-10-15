// Leka - LekaOS
// Copyright 2021 APF France handicap
// SPDX-License-Identifier: Apache-2.0

#ifndef _LEKA_OS_DRIVER_CORE_VIDEO_INTERFACE_LCD_BASE_H_
#define _LEKA_OS_DRIVER_CORE_VIDEO_INTERFACE_LCD_BASE_H_

namespace leka::interface {

class LCD
{
  public:
	~LCD() = default;

	virtual void initialize() = 0;

	virtual void turnOn()  = 0;
	virtual void turnOff() = 0;

	virtual void setBrightness(float value) = 0;
};

}	// namespace leka::interface

#endif	 // _LEKA_OS_DRIVER_CORE_VIDEO_INTERFACE_LCD_BASE_H_