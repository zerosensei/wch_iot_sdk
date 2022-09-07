/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <kernel.h>
#include <drivers/flash.h>

static const struct flash_parameters flash_param = {
    .write_block_size = 4,
    .erase_value = 0xff,
};

static const struct flash_parameters *
flash_wch_get_param(const struct device *dev)
{
    ARG_UNUSED(dev);

    return &flash_param;
}

static int flash_wch_read(const struct device *dev, 
                off_t offset, void *data, size_t len)
{
    int key = irq_lock();

    ARG_UNUSED(dev);

    hal_flash_rom_read(offset, data, len);
    
    irq_unlock(key);

    return 0;
}

static int flash_wch_write(const struct device *dev, 
                off_t offset, const void *data, size_t len)
{
    int ret = 0;
    int key = irq_lock();

    ARG_UNUSED(dev);

    if (hal_flash_rom_write(offset, data, len) != 0) {
        ret = -1;
    }

    irq_unlock(key);

    return ret;
}

static int flash_wch_erase(const struct device *dev,
                off_t offset, size_t len)
{
    int ret = 0;
    int key = irq_lock();

    ARG_UNUSED(dev);

    if (hal_flash_rom_erase(offset, len) != 0) {
        ret = -1;
    }

    irq_unlock(key);

    return ret;
}

static const struct flash_driver_api flash_wch_drivers_api = {
    .erase = flash_wch_erase,
    .read = flash_wch_read,
    .write = flash_wch_write,
    .get_parameters = flash_wch_get_param,
};

static int wch_flash_init(const struct device *dev)
{
    ARG_UNUSED(dev);

    return 0;
}

#define FLASH_INIT()    \
    static struct device_state flash_sta = {0}; \
    DEVICE_DEFINE(flash, wch_flash_init, NULL,      \
                NULL,       \
                NULL,       \
                &flash_wch_drivers_api,     \
                &flash_sta,     \
                DRIVER,     \
                1) 

FLASH_INIT();