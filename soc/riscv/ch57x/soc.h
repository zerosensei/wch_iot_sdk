/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SOC_CH57X_SOC_H
#define SOC_CH57X_SOC_H

#include <arch/cpu.h>
#include "dev_cfg.h"
#include "pin_cfg.h"
#include <ch57x_common.h>

/* Exception code Mask */
#define SOC_MCAUSE_EXP_MASK (0x7FFFFFFF)
#define ACTIVE_USEC		    (100)
#define IDLE_USEC		    (1200)


void sys_reboot(uint8_t type);

#endif /* SOC_CH57X_SOC_H */
