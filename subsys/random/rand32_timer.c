/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <random/rand32.h>
#include <drivers/timer/system_timer.h>
#include <kernel.h>
#include <sys/atomic.h>
#include <string.h>

#if defined(__GNUC__)

/*
 * Symbols used to ensure a rapid series of calls to random number generator
 * return different values.
 */
static atomic_val_t _rand32_counter;

#define _RAND32_INC 1000000003U

/**
 * @brief Get a 32 bit random number
 *
 * The non-random number generator returns values that are based off the
 * target's clock counter, which means that successive calls will return
 * different values.
 *
 * @return a 32-bit number
 */
uint32_t sys_rand32_get(void)
{
	return k_cycle_get_32() + atomic_add(&_rand32_counter, _RAND32_INC);
}

/**
 * @brief Fill destination buffer with random numbers
 *
 * The non-random number generator returns values that are based off the
 * target's clock counter, which means that successive calls will return
 * different values.
 *
 * @param dst destination buffer to fill
 * @param outlen size of destination buffer to fill
 */
void sys_rand_get(void *dst, size_t outlen)
{
	uint8_t *udst = dst;
	uint32_t blocksize;
	uint32_t ret;

	while (outlen) {
		ret = sys_rand32_get();
		blocksize = MIN(outlen, sizeof(ret));
		(void)memcpy((void *)udst, &ret, blocksize);
		udst += blocksize;
		outlen -= blocksize;
	}
}

#endif /* __GNUC__ */
