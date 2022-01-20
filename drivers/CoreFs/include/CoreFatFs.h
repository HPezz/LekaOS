// Leka - LekaOS
// Copyright 2021 APF France handicap
// SPDX-License-Identifier: Apache-2.0

#ifndef _LEKA_OS_LIB_FATFS_H_
#define _LEKA_OS_LIB_FATFS_H_

#include "CoreFatFsBase.h"

namespace leka {

class CoreFatFs : public CoreFatFsBase
{
  public:
	CoreFatFs() = default;	 // SDBlockDevice must be initialized and mounted before using CoreFatFs

	auto open(const char *path) -> FRESULT final;
	auto close() -> FRESULT final;
	auto read(void *buffer, uint32_t number_of_bytes_to_read, uint32_t *number_of_bytes_read) -> FRESULT final;
	auto write(const void *buffer, uint32_t number_of_bytes_to_write, uint32_t *number_of_bytes_written)
		-> FRESULT final;
	auto seek(uint32_t byte_offset_from_top_of_file) -> FRESULT final;
	auto getSize() -> uint32_t final;

	auto getPointer() -> FIL * final;

  private:
	FIL _file;	 // TODO (@yann) - _file is not initialized - make it a pointer?
};

}	// namespace leka

#endif	 // _LEKA_OS_LIB_FATFS_H_
