// Leka - LekaOS
// Copyright 2020 APF France handicap
// SPDX-License-Identifier: Apache-2.0

#ifndef _LEKA_OS_SPIKE_MOTORS_UTILS_H_
#define _LEKA_OS_SPIKE_MOTORS_UTILS_H_

#include "LKCoreMotor.h"

using namespace leka;

DigitalOut motor_right_dir_1(PE_4);
DigitalOut motor_right_dir_2(PE_5);
PwmOut motor_right_speed(PA_2);

DigitalOut motor_left_dir_1(PC_13);
DigitalOut motor_left_dir_2(PI_8);
PwmOut motor_left_speed(PA_1);

LKCoreMotor motor_right(PinName::MOTOR_RIGHT_DIRECTION_1, PinName::MOTOR_RIGHT_DIRECTION_2, PinName::MOTOR_RIGHT_PWM);
LKCoreMotor motor_left(PinName::MOTOR_LEFT_DIRECTION_1, PinName::MOTOR_LEFT_DIRECTION_2, PinName::MOTOR_LEFT_PWM);

void spinLeft()
{
	motor_right.spin(Rotation::clockwise, 0.5f);
	motor_left.spin(Rotation::clockwise, 0.5f);
}

void spinRight()
{
	motor_right.spin(Rotation::counterClockwise, 0.5f);
	motor_left.spin(Rotation::counterClockwise, 0.5f);
}

void stop()
{
	motor_right.stop();
	motor_left.stop();
}

void motor_thread()
{
	while (true) {
		spinLeft();
		rtos::ThisThread::sleep_for(5s);

		spinRight();
		rtos::ThisThread::sleep_for(5s);

		stop();
		rtos::ThisThread::sleep_for(5s);
	}
}

#endif	 // _LEKA_OS_SPIKE_MOTORS_UTILS_H_
