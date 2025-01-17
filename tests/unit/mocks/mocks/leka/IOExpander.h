// Leka - LekaOS
// Copyright 2021 APF France handicap
// SPDX-License-Identifier: Apache-2.0

#ifndef _LEKA_OS_MOCK_DRIVERS_IO_EXPANDER_H_
#define _LEKA_OS_MOCK_DRIVERS_IO_EXPANDER_H_

#include "gmock/gmock.h"
#include "interface/drivers/IOExpander.h"

namespace leka::mock {

template <typename pin_underlying_type_t>
class IOExpander : public interface::IOExpander<pin_underlying_type_t>
{
  public:
	MOCK_METHOD(void, setPinAsInput, (pin_underlying_type_t), (override));
	MOCK_METHOD(int, readInputPin, (pin_underlying_type_t), (override));

	MOCK_METHOD(void, setModeForPin, (pin_underlying_type_t, PinMode), ());
	MOCK_METHOD(PinMode, getModeForPin, (pin_underlying_type_t), (override));
};

}	// namespace leka::mock

#endif	 // _LEKA_OS_MOCK_DRIVERS_IO_EXPANDER_H_
