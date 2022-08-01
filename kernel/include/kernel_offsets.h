/*
 * Copyright (c) 2013-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <device.h>

#ifndef ZEPHYR_KERNEL_INCLUDE_KERNEL_OFFSETS_H_
#define ZEPHYR_KERNEL_INCLUDE_KERNEL_OFFSETS_H_



/* All of this is build time magic, but LCOV gets confused. Disable coverage
 * for this whole file.
 *
 * LCOV_EXCL_START
 */

/*
 * The final link step uses the symbol _OffsetAbsSyms to force the linkage of
 * offsets.o into the ELF image.
 */

GEN_ABS_SYM_BEGIN(_OffsetAbsSyms)



/* LCOV_EXCL_STOP */
#endif /* ZEPHYR_KERNEL_INCLUDE_KERNEL_OFFSETS_H_ */
