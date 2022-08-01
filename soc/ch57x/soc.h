/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SOC_CH57X_SOC_H
#define SOC_CH57X_SOC_H

#include <arch/cpu.h>
#include "pin_cfg.h"
#include "dev_cfg.h"
#include <ch57x_common.h>

/* Exception code Mask */
#define SOC_MCAUSE_EXP_MASK 0x7FFFFFFF

void sys_reboot(uint8_t type);
void wch_sys_init(void);

#endif /* SOC_CH57X_SOC_H */
