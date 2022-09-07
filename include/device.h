/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef INCLUDE_DEVICE_H
#define INCLUDE_DEVICE_H

#include <sys/util.h>
#include <toolchain.h>

#define DEVICE_NAME_GET(name)   _CONCAT(__device_, name)
#define DEVICE_GET(dev)         (&DEVICE_NAME_GET(dev))

struct device_state {
    int init_result;
    bool is_initialized;
};

struct device {
    const void *cfg;
    const void *api;
    struct device_state *sta;
    void *data;
    
#ifdef CONFIG_PM
	/** Reference to the device PM resources. */
	struct pm_device * const pm;
#endif
};

static inline int device_is_ready(const struct device *dev)
{
    if(dev == NULL) {
        return false;
    }

    return dev->sta->is_initialized && (dev->sta->init_result == 0);
}

#define DEVICE_DEFINE(dev_name, init_fn, pm_device, data_ptr, cfg_ptr, api_ptr, sta_ptr,  \
             level, prio)  \
        const Z_DECL_ALIGN(struct device)			\
		DEVICE_NAME_GET(dev_name) __used			\
	    = { \
		.cfg = (cfg_ptr),					\
		.api = (api_ptr),					\
		.sta = (sta_ptr),					\
		.data = (data_ptr),					\
		COND_CODE_1(CONFIG_PM_DEVICE, (.pm = pm_device,), ())	\
	};		        \
	Z_INIT_ENTRY_DEFINE(DEVICE_NAME_GET(dev_name), init_fn,		\
		(&DEVICE_NAME_GET(dev_name)), level, prio)

#include "device_extern.h"

#endif /* INCLUDE_DEVICE_H */