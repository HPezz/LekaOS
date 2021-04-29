// Leka - LekaOS
// Copyright 2020 APF France handicap
// SPDX-License-Identifier: Apache-2.0

#include "PinNames.h"

#include "drivers/I2C.h"
#include "rtos/ThisThread.h"

#include "CoreHTS.h"
#include "CoreI2C.h"

using namespace leka;
using namespace std::chrono;

auto main() -> int
{
	mbed::I2C i2c(PinName::SENSOR_IMU_TH_I2C_SDA, PinName::SENSOR_IMU_TH_I2C_SCL);
	CoreI2C corei2c(i2c);
	CoreHTS corehts(corei2c);

	rtos::ThisThread::sleep_for(2s);

	corehts.init();

	auto coefficients_temperature = corehts.getTemperatureCalibrationCoefficients();
	auto coefficients_humidity	  = corehts.getHumidityCalibrationCoefficients();

	printf("Temperature calibration values: slope: %f, y-intercept: %f\n", coefficients_temperature.slope,
		   coefficients_temperature.y_intercept);

	printf("Humidity calibration values: slope: %f, y-intercept: %f\n", coefficients_humidity.slope,
		   coefficients_humidity.y_intercept);

	while (true) {
		auto temperature = corehts.getTemperatureCelsius();
		auto humidity	 = corehts.getRelativeHumidity();

		printf("Temperature: %.1fC, Humidity: %.1f%% (relative humidy)\n", temperature, humidity);

		rtos::ThisThread::sleep_for(1s);
	}
}