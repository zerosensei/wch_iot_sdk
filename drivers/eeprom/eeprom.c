/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <kernel.h>
#include <drivers/eeprom.h>

static const struct eeprom_parameters eeprom_param = {
    .write_block_size = 4,
    .erase_value = 0xff,
};

static const struct eeprom_parameters *
eeprom_wch_get_param(const struct device *dev)
{
    ARG_UNUSED(dev);

    return &eeprom_param;
}

static int eeprom_wch_read(const struct device *dev, 
                off_t offset, void *data, size_t len)
{
    int ret = 0;
    int key = irq_lock();
    ARG_UNUSED(dev);

    if (hal_eeprom_read(offset, data, len) != 0) {
        ret = -1;
    }

	irq_unlock(key);

    return ret;
}

static int eeprom_wch_write(const struct device *dev, 
                off_t offset, const void *data, size_t len)
{
    int ret = 0;
    int key = irq_lock();
    ARG_UNUSED(dev);

    if (hal_eeprom_write(offset, data, len) != 0) {
        ret = -1;
    }

	irq_unlock(key);

    return ret;  
}

static int eeprom_wch_erase(const struct device *dev,
                off_t offset, size_t len)
{
    int ret = 0;
    int key = irq_lock();
    ARG_UNUSED(dev);

    if (hal_eeprom_erase(offset, len) != 0) {
        ret = -1;
    }

	irq_unlock(key);

    return ret;
}

static const struct eeprom_driver_api eeprom_wch_driver_api = {
    .erase = eeprom_wch_erase,
    .read = eeprom_wch_read,
    .write = eeprom_wch_write,
    .get_parameters = eeprom_wch_get_param
};

static int wch_eeprom_init(const struct device *dev)
{
    ARG_UNUSED(dev);

    return 0;
}

#define EEPROM_INIT()    \
    static struct device_state eeprom_sta = {0}; \
    DEVICE_DEFINE(eeprom, wch_eeprom_init, NULL,      \
                NULL,       \
                NULL,       \
                &eeprom_wch_driver_api,     \
                &eeprom_sta,     \
                DRIVER,     \
                1) 

EEPROM_INIT();