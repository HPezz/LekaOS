// Leka - LekaOS
// Copyright 2021 APF France handicap
// SPDX-License-Identifier: Apache-2.0

#include "MathUtils.h"
#include <span>

using namespace leka::utils;

auto math::computeLinearCoefficients(Point p1, Point p2) -> LinearCoefficients
{
	if (p2.x == p1.x) {
		return {0, 0};
	}

	auto coeffs		   = LinearCoefficients {0, 0};
	coeffs.slope	   = (p2.y - p1.y) / (p2.x - p1.x);
	coeffs.y_intercept = p1.y - coeffs.slope * p1.x;

	return coeffs;
}

auto math::checksum8(std::span<uint8_t> data) -> uint8_t
{
	uint8_t checksum = 0;

	for (auto v: data) {
		checksum = (v + checksum) % 256;
	}

	return checksum;
}
