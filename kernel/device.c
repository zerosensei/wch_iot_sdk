/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#include <kernel.h>

extern const struct init_entry __init_start[];
extern const struct init_entry __init_KERNEL_start[];
extern const struct init_entry __init_DRIVER_start[];
extern const struct init_entry __init_APPLICATION_start[];
extern const struct init_entry __init_end[];

void z_sys_init_run_level(int32_t level)
{
	static const struct init_entry *levels[] = {
		__init_KERNEL_start,
		__init_DRIVER_start,
		__init_APPLICATION_start,
		/* End marker */
		__init_end,
	};
	const struct init_entry *entry;

	for (entry = levels[level]; entry < levels[level+1]; entry++) {
		const struct device *dev = entry->dev;
		int rc = entry->init(dev);

		if (dev != NULL) {
			/* Mark device initialized.  If initialization
			 * failed, record the error condition.
			 */
			if (rc != 0) {
				if (rc < 0) {
					rc = -rc;
				}
				if (rc > UINT8_MAX) {
					rc = UINT8_MAX;
				}
				dev->sta->init_result = rc;
			}
			dev->sta->is_initialized = true;
		}
	}
}