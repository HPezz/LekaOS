# Leka - LekaOS
# Copyright 2020 APF France handicap
# SPDX-License-Identifier: Apache-2.0

add_subdirectory(${LIBS_DIR}/ContainerKit)
add_subdirectory(${LIBS_DIR}/FileManager)
add_subdirectory(${LIBS_DIR}/FileSystemKit)
add_subdirectory(${LIBS_DIR}/FirmwareKit)
add_subdirectory(${LIBS_DIR}/RobotKit)

add_subdirectory(${LIBS_DIR}/IOKit)

add_subdirectory(${LIBS_DIR}/HardwareTest/LekaSD)
add_subdirectory(${LIBS_DIR}/HardwareTest/LekaMotors)

add_subdirectory(${LIBS_DIR}/InvestigationDay/BLE)
add_subdirectory(${LIBS_DIR}/InvestigationDay/LekaRFID)
add_subdirectory(${LIBS_DIR}/InvestigationDay/LekaWifi)
add_subdirectory(${LIBS_DIR}/InvestigationDay/LekaTouch)
add_subdirectory(${LIBS_DIR}/InvestigationDay/LekaScreen)
add_subdirectory(${LIBS_DIR}/InvestigationDay/LekaBluetooth)

add_subdirectory(${LIBS_DIR}/LKAnimationKit)

add_subdirectory(${LIBS_DIR}/LKCalculatorKit)

add_subdirectory(${LIBS_DIR}/PrettyPrinter)
