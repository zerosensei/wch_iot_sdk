/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef INCLUDE_DRIVERS_EEPROM_H
#define INCLUDE_DRIVERS_EEPROM_H

#include <stddef.h>
#include <sys/types.h>
#include <device.h>

#ifdef __cplusplus
extern "C" {
#endif

struct eeprom_parameters {
	const size_t write_block_size;
	uint8_t erase_value;
};

typedef int (*eeprom_api_read)(const struct device *dev, off_t offset,
			       void *data, size_t len);

/**
 * @brief EEPROM write implementation handler type
 */
typedef int (*eeprom_api_write)(const struct device *dev, off_t offset,
				const void *data, size_t len);

/**
 * @brief EEPROM erase implementation handler type
 *
 */
typedef int (*eeprom_erase_api)(const struct device *dev, off_t offset,
                size_t size);


typedef const struct eeprom_parameters* (*eeprom_get_parameters_api)(const struct device *dev);

struct eeprom_driver_api {
	eeprom_api_read read;
	eeprom_api_write write;
	eeprom_erase_api erase;
	eeprom_get_parameters_api get_parameters;
};

/**
 *  @brief Read data from EEPROM
 *
 *  @param dev EEPROM device
 *  @param offset Address offset to read from.
 *  @param data Buffer to store read data.
 *  @param len Number of bytes to read.
 *
 *  @return 0 on success, -1 on failure.
 */
static inline int eeprom_read(const struct device *dev, off_t offset,
				     void *data, size_t len)
{
	const struct eeprom_driver_api *api =
		(const struct eeprom_driver_api *)dev->api;

	return api->read(dev, offset, data, len);
}

/**
 *  @brief Write data to EEPROM
 *
 *  @param dev EEPROM device
 *  @param offset Address offset to write data to.
 *  @param data Buffer with data to write.
 *  @param len Number of bytes to write.
 *
 *  @return 0 on success, -1 on failure.
 */
static inline int eeprom_write(const struct device *dev, off_t offset,
				      const void *data, size_t len)
{
	const struct eeprom_driver_api *api =
		(const struct eeprom_driver_api *)dev->api;

	return api->write(dev, offset, data, len);
}

/**
 * @brief  Erase part or all of a EEPROM
 *
 *  @param  dev EEPROM device
 *  @param  offset erase area starting offset
 *  @param  size size of area to be erased
 *
 *  @return 0 on success, -1 on failure.
 */
static inline int eeprom_erase(const struct device *dev, off_t offset,
				     size_t size)
{
	const struct eeprom_driver_api *api =
		(const struct eeprom_driver_api *)dev->api;
	int rc;

	rc = api->erase(dev, offset, size);

	return rc;
}

/**
 * @brief  Get the minimum write block size supported by the driver
 *
 * The write block size supported by the driver might differ from the write
 * block size of memory used because the driver might implements write-modify
 * algorithm.
 *
 * @param  dev eeprom device
 *
 * @return  write block size in bytes.
 */
static inline size_t eeprom_get_write_block_size(const struct device *dev)
{
	const struct eeprom_driver_api *api =
		(const struct eeprom_driver_api *)dev->api;

	return api->get_parameters(dev)->write_block_size;
}

/**
 * @brief  Get pointer to eeprom_parameters structure
 *
 * @return pointer to eeprom_parameters structure characteristic for
 *         the device.
 */
static inline const struct eeprom_parameters *eeprom_get_parameters(const struct device *dev)
{
	const struct eeprom_driver_api *api =
		(const struct eeprom_driver_api *)dev->api;

	return api->get_parameters(dev);
}

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_DRIVERS_EEPROM_H */