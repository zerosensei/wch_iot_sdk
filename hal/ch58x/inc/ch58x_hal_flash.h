/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef HAL_CH58X_INC_CH58X_HAL_FLASH_H
#define HAL_CH58X_INC_CH58X_HAL_FLASH_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ch58x_common.h"

/**
 * @brief Read Flash-ROM
 * 
 * @param addr  Read start address.
 * @param buf   Pointer to data to put.
 * @param len   Size of data to be read, in bytes.
 */
void hal_flash_rom_read(uint32_t addr, void *buf, uint32_t len);

/**
 * @brief Erase Flash-ROM block.
 * 
 * @param addr  Address of the data to be erased.
 * @param len   Size of data to be erased, in bytes.
 * 
 * @return  0 on success, otherwise on failure. 
 */
static inline uint32_t hal_flash_rom_erase(uint32_t addr, uint32_t len)
{
    return FLASH_ROM_ERASE(addr, len);
}

/**
 * @brief Write FlashROM data block, minimal block is dword.
 * 
 * @param addr  Address of the data to be written.
 * @param buf   Pointer to the source buffer, Must be aligned to 4 bytes.
 * @param len   Size of data to be written, in bytes.
 * 
 * @return  0 on success, otherwise on failure.
 */
static inline uint32_t hal_flash_rom_write(uint32_t addr, const void *buf, uint32_t len)
{
    return FLASH_ROM_WRITE(addr, (void *)buf, len);
}

/**
 * @brief Verify FlashROM data block, minimal block is dword.
 * 
 * @param addr  Address of the data to verify.
 * @param buf   Pointer to the source buffer, Must be aligned to 4 bytes.
 * @param len   Size of data to verify, in bytes.
 * 
 * @return  0 on success, otherwise on failure. 
 */
static inline uint32_t hal_flash_rom_verify(uint32_t addr, void *buf, uint32_t len)
{
    return FLASH_ROM_VERIFY(addr, buf, len);
}

/**
 * @brief Read Data-Flash data block.
 * 
 * @param addr  Address of the data to be read.
 * @param buf   Pointer to the buffer where data should be stored, Must be aligned to 4 bytes.
 * @param len   Size of data to be read, in bytes.
 * 
 * @return  0 on success, otherwise on failure. 
 */
static inline uint32_t hal_eeprom_read(uint32_t addr, const void *buf, uint32_t len)
{
    return EEPROM_READ(addr, (void *)buf, len);
}

/**
 * @brief Erase EEPROM block.
 * 
 * @param addr  Address of the data to be erased.
 * @param len   Size of data to be erased, in bytes.
 * 
 * @return  0 on success, otherwise on failure. 
 */
static inline uint32_t hal_eeprom_erase(uint32_t addr, uint32_t len)
{
    return EEPROM_ERASE(addr, len);
}

/**
 * @brief Write FlashROM data block, minimal block is dword.
 * 
 * @param addr  Address of the data to be written.
 * @param buf   Pointer to the source buffer, Must be aligned to 4 bytes.
 * @param len   Size of data to be written, in bytes.
 * 
 * @return  0 on success, otherwise on failure.  
 */
static inline uint32_t hal_eeprom_write(uint32_t addr, const void *buf, uint32_t len)
{
    return EEPROM_WRITE(addr, (void *)buf, len);
}


#ifdef __cplusplus
}
#endif

#endif /* HAL_CH58X_INC_CH58X_HAL_FLASH_H */