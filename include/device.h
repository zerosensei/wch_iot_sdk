/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef INCLUDE_DEVICE_H
#define INCLUDE_DEVICE_H

#include <sys/util.h>
#include <toolchain.h>

struct device_state {
    int init_result;
    bool is_initialized;
};

struct device {
    const void *cfg;
    const void *api;
    struct device_state *sta;
    void *data;
};

static inline int device_is_ready(const struct device *dev)
{
    if(dev ==  NULL) {
        return false;
    }

    return dev->sta->is_initialized && (dev->sta->init_result == 0);
}



#endif /* INCLUDE_DEVICE_H */