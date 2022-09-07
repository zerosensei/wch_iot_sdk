/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ch57x_common.h>

void hal_flash_rom_read(uint32_t addr, void *buf, uint32_t len)
{
    uint32_t i, length = (len + 3) >> 2;
    uint32_t *pCode = (uint32_t *)addr;
    uint32_t *pBuf = (uint32_t *)buf;

    for(i = 0; i < length; i++) {
        *pBuf++ = *pCode++;
    }
}