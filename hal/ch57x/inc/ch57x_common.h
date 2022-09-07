/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef HAL_CH57X_INC_CH57X_COMMON_H
#define HAL_CH57X_INC_CH57X_COMMON_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <auto_config.h>
#include "CH573SFR.h"
#include "ISP573.h"

#define __WCH_INT_FAST          __attribute__((interrupt("WCH-Interrupt-fast")))
#define __HIGHCODE              __attribute__((section(".highcode")))


/**
 * @brief Macro for checking if the specified identifier is defined and it has
 *        a non-zero value.
 *
 */
#define WCH_IS_ENABLED(cfg_macro)   (cfg_macro)

/**
 * @brief Macro for creating unsigned integer with bit position @p x set.
 *
 * @param[in] x Bit position to be set.
 *
 * @return Unsigned integer with requested bit position set.
 */
#define WCH_BIT(x) (1UL << (x))

/**
 * @brief Macro for returning bit mask or 0 if @p x is 0.
 *
 * @param[in] x Bit mask size. Bit mask has bits 0 through x-1 (inclusive) set.
 *
 * @return Bit mask.
 */
#define WCH_MASK(x) (WCH_BIT(x) - 1UL)

/**
 * @brief Macro for concatenating two tokens in macro expansion.
 *
 * @note This macro is expanded in two steps so that tokens given as macros
 *       themselves are fully expanded before they are merged.
 *
 * @param[in] p1 First token.
 * @param[in] p2 Second token.
 *
 * @return The two tokens merged into one, unless they cannot together form
 *         a valid token (in such case, the preprocessor issues a warning and
 *         does not perform the concatenation).
 *
 * @sa WCH_CONCAT_3
 */
#define WCH_CONCAT_2(p1, p2)       _WCH_CONCAT_2(p1, p2)

/** @brief Internal macro used by @ref _WCH_CONCAT_2 to perform the expansion in two steps. */
#define _WCH_CONCAT_2(p1, p2)      p1 ## p2

/**
 * @brief Macro for performing rounded integer division (as opposed to
 *        truncating the result).
 *
 * @param[in] a Numerator.
 * @param[in] b Denominator.
 *
 * @return Rounded (integer) result of dividing @c a by @c b.
 */
#define WCH_ROUNDED_DIV(a, b)  (((a) + ((b) / 2)) / (b))

/**
 * @brief Macro for performing integer division, making sure the result is rounded up.
 *
 * @details A typical use case for this macro is to compute the number of objects
 *          with size @c b required to hold @c a number of bytes.
 *
 * @param[in] a Numerator.
 * @param[in] b Denominator.
 *
 * @return Integer result of dividing @c a by @c b, rounded up.
 */
#define WCH_CEIL_DIV(a, b)  ((((a) - 1) / (b)) + 1)

/**
 * @brief Macro for getting the number of elements in an array.
 *
 * @param[in] array Name of the array.
 *
 * @return Array element count.
 */
#define WCH_ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))


/** @brief Enumerated type for error codes. */
typedef enum {
    WCH_SUCCESS                    = 0,  ///< Operation performed successfully.
    WCH_ERROR_INTERNAL             = 1,  ///< Internal error.
    WCH_ERROR_NO_MEM               = 2,  ///< No memory for operation.
    WCH_ERROR_NOT_SUPPORTED        = 3,  ///< Not supported.
    WCH_ERROR_INVALID_PARAM        = 4,  ///< Invalid parameter.
    WCH_ERROR_INVALID_STATE        = 5,  ///< Invalid state, operation disallowed in this state.
    WCH_ERROR_INVALID_LENGTH       = 6,  ///< Invalid length.
    WCH_ERROR_TIMEOUT              = 7,  ///< Operation timed out.
    WCH_ERROR_FORBIDDEN            = 8,  ///< Operation is forbidden.
    WCH_ERROR_NULL                 = 9,  ///< Null pointer.
    WCH_ERROR_INVALID_ADDR         = 10, ///< Bad memory address.
    WCH_ERROR_BUSY                 = 11, ///< Busy.
    WCH_ERROR_ALREADY_INITIALIZED  = 12, ///< Module already initialized.
} wch_err_t;

//TODO:
//#ifdef CONFIG_UART
#include "ch57x_hal_pfic.h"
#include "ch57x_hal_uart.h"
#include "ch57x_hal_sys.h"
#include "ch57x_hal_gpio.h"
#include "ch57x_hal_systick.h"
#include "ch57x_hal_rtc.h"
#include "ch57x_hal_clk.h"
#include "ch57x_hal_power.h"
#include "ch57x_hal_flash.h"

#endif /* HAL_CH57X_INC_CH57X_COMMON_H */