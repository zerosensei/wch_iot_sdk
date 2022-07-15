/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef HAL_COMMON_WCH_ERRORS_H
#define HAL_COMMON_WCH_ERRORS_H

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

#endif /* HAL_COMMON_WCH_ERRORS_H */