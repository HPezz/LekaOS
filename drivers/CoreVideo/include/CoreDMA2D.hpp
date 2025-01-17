// Leka - LekaOS
// Copyright 2021 APF France handicap
// SPDX-License-Identifier: Apache-2.0

#ifndef _LEKA_OS_LIB_DMA2D_H_
#define _LEKA_OS_LIB_DMA2D_H_

#include "interface/DMA2D.hpp"

namespace leka {

class CoreDMA2D : public interface::DMA2DBase
{
  public:
	explicit CoreDMA2D(LKCoreSTM32HalBase &hal);

	void initialize() final;

	void transferData(uintptr_t input_data_address, uintptr_t output_data_address, uint32_t width,
					  uint32_t height) final;
	void transferImage(uint32_t width, uint32_t height, uint32_t width_offset) final;
	void transferDrawing(uintptr_t first_pixel_address, uint32_t width, uint32_t height, uint32_t color) final;

	auto getHandle() -> DMA2D_HandleTypeDef final;

  private:
	DMA2D_HandleTypeDef _hdma2d {};
	LKCoreSTM32HalBase &_hal;
};

}	// namespace leka

#endif	 // _LEKA_OS_LIB_DMA2D_H_
