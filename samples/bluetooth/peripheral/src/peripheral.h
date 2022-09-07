/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SAMPLES_BLUETOOTH_PERIPHERAL_SRC_PERIPHERAL_H
#define SAMPLES_BLUETOOTH_PERIPHERAL_SRC_PERIPHERAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <kernel.h>
#include <bluetooth/bluetooth.h>

/* Process events */
#define PRRIP_START_DEVICE              BIT(0)
#define PERIP_POLL_NOTIFY_EVENT         BIT(1)

void perip_process_init(void);

#ifdef __cplusplus
}
#endif

#endif /* SAMPLES_BLUETOOTH_PERIPHERAL_SRC_PERIPHERAL_H */

