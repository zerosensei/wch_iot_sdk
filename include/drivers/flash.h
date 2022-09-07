/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef INCLUDE_DRIVERS_FLASH_H
#define INCLUDE_DRIVERS_FLASH_H

#include <stddef.h>
#include <sys/types.h>
#include <device.h>

#ifdef __cplusplus
extern "C" {
#endif
/**
 * Flash memory parameters. Contents of this structure suppose to be
 * filled in during flash device initialization and stay constant
 * through a runtime.
 */
struct flash_parameters {
	const size_t write_block_size;
	uint8_t erase_value; /* Byte value of erased flash */
};

typedef int (*flash_read_api)(const struct device *dev, off_t offset,
                void *data, size_t len);

/**
 * @brief Flash write implementation handler type
 *
 * @note Any necessary write protection management must be performed by
 * the driver, with the driver responsible for ensuring the "write-protect"
 * after the operation completes (successfully or not) matches the write-protect
 * state when the operation was started.
 */
typedef int (*flash_write_api)(const struct device *dev, off_t offset,
                const void *data, size_t len); 

/**
 * @brief Flash erase implementation handler type
 *
 * @note Any necessary erase protection management must be performed by
 * the driver, with the driver responsible for ensuring the "erase-protect"
 * after the operation completes (successfully or not) matches the erase-protect
 * state when the operation was started.
 */
typedef int (*flash_erase_api)(const struct device *dev, off_t offset,
                size_t size);


typedef const struct flash_parameters* (*flash_get_parameters_api)(const struct device *dev);

struct flash_driver_api {
	flash_read_api read;
	flash_write_api write;
	flash_erase_api erase;
	flash_get_parameters_api get_parameters;
};

/**
 * @brief Read data from flash.
 * 
 * @param dev Flash dev.
 * @param offset Offset (byte aligned) to read.
 * @param data Buffer to store read data.
 * @param len Number of bytes to read.
 * 
 * @return 0 on success, -1 on failure.
 */
static inline int flash_read(const struct device *dev, off_t offset, 
                void *data, size_t len)
{
	const struct flash_driver_api *api =
		(const struct flash_driver_api *)dev->api;

	return api->read(dev, offset, data, len);
}

/**
 * @brief Write buffer into flash memory.
 *
 * Write size and offset must be multiples of the minimum write block size
 * supported by the driver.
 *
 * Any necessary write protection management is performed by the driver
 * write implementation itself.
 *
 * @param  dev flash device
 * @param  offset starting offset for the write
 * @param  data data to write
 * @param  len Number of bytes to write
 *
 *  @return 0 on success, -1 on failure.
 */
static inline int flash_write(const struct device *dev, off_t offset,
				     const void *data, size_t len)
{
	const struct flash_driver_api *api =
		(const struct flash_driver_api *)dev->api;
	int rc;

	rc = api->write(dev, offset, data, len);

	return rc;
}

/**
 * @brief  Erase part or all of a flash memory
 *
 * Any necessary erase protection management is performed by the driver
 * erase implementation itself.
 *
 *  @param  dev flash device
 *  @param  offset erase area starting offset
 *  @param  size size of area to be erased
 *
 *  @return 0 on success, -1 on failure.
 */
static inline int flash_erase(const struct device *dev, off_t offset,
				     size_t size)
{
	const struct flash_driver_api *api =
		(const struct flash_driver_api *)dev->api;
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
 * @param  dev flash device
 *
 * @return  write block size in bytes.
 */
static inline size_t flash_get_write_block_size(const struct device *dev)
{
	const struct flash_driver_api *api =
		(const struct flash_driver_api *)dev->api;

	return api->get_parameters(dev)->write_block_size;
}

/**
 * @brief  Get pointer to flash_parameters structure
 *
 * Returned pointer points to a structure that should be considered
 * constant through a runtime, regardless if it is defined in RAM or
 * Flash.
 * Developer is free to cache the structure pointer or copy its contents.
 *
 * @return pointer to flash_parameters structure characteristic for
 *         the device.
 */
static inline const struct flash_parameters *flash_get_parameters(const struct device *dev)
{
	const struct flash_driver_api *api =
		(const struct flash_driver_api *)dev->api;

	return api->get_parameters(dev);
}


#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_DRIVERS_FLASH_H */