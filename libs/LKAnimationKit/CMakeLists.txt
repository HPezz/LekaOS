# Leka - LekaOS
# Copyright 2020 APF France handicap
# SPDX-License-Identifier: Apache-2.0

add_library(LKAnimationKit STATIC)

target_include_directories(LKAnimationKit
	PUBLIC
		include
		include/internal
)

target_sources(LKAnimationKit
	PRIVATE
		source/BouncingSquare.cpp
		source/LKAnimationKit.cpp
)

target_link_libraries(LKAnimationKit
	LKCoreLL
	LKCoreSTM32Hal
	CoreVideo
)

if (${CMAKE_PROJECT_NAME} STREQUAL "LekaOSUnitTests")

	leka_unit_tests_sources(
		tests/LKAnimationKit_test.cpp
	)

endif()
