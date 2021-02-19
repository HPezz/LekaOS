// Leka - LekaOS
// Copyright 2020 APF France handicap
// SPDX-License-Identifier: Apache-2.0

#ifndef _LEKA_OS_LIB_LKUTILS_H_
#define _LEKA_OS_LIB_LKUTILS_H_

namespace leka {

namespace utils {

	template <typename OutputType, typename IntputType>
	OutputType map(IntputType x, IntputType in_min, IntputType in_max, OutputType out_min, OutputType out_max)
	{
		return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
	}

}	// namespace utils

}	// namespace leka

#endif	 // _LEKA_OS_LIB_LKUTILS_H_