/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef INCLUDE_LOGGING_LOG_H
#define INCLUDE_LOGGING_LOG_H

#include <stddef.h>
#include <stdint.h>
#include <kernel.h>

struct log_source_const_data {
	const char *name;
	uint8_t level;
};

#define LOG_LEVEL_NONE 0U
#define LOG_LEVEL_ERR  1U
#define LOG_LEVEL_WRN  2U
#define LOG_LEVEL_INF  3U
#define LOG_LEVEL_DBG  4U

/**
 * @brief Macro for conditional code generation if provided log level allows.
 *
 * Macro behaves similarly to standard \#if \#else \#endif clause. The
 * difference is that it is evaluated when used and not when header file is
 * included.
 *
 * @param _eval_level Evaluated level. If level evaluates to one of existing log
 *		      log level (1-4) then macro evaluates to _iftrue.
 * @param _iftrue     Code that should be inserted when evaluated to true. Note,
 *		      that parameter must be provided in brackets.
 * @param _iffalse    Code that should be inserted when evaluated to false.
 *		      Note, that parameter must be provided in brackets.
 */
#define Z_LOG_EVAL(_eval_level, _iftrue, _iffalse) \
	Z_LOG_EVAL1(_eval_level, _iftrue, _iffalse)

#define Z_LOG_EVAL1(_eval_level, _iftrue, _iffalse) \
	__COND_CODE(_LOG_ZZZZ##_eval_level, _iftrue, _iffalse)

#define _LOG_ZZZZ1  _LOG_YYYY,
#define _LOG_ZZZZ1U _LOG_YYYY,
#define _LOG_ZZZZ2  _LOG_YYYY,
#define _LOG_ZZZZ2U _LOG_YYYY,
#define _LOG_ZZZZ3  _LOG_YYYY,
#define _LOG_ZZZZ3U _LOG_YYYY,
#define _LOG_ZZZZ4  _LOG_YYYY,
#define _LOG_ZZZZ4U _LOG_YYYY,


/* Macro expects that optionally on second argument local log level is provided.
 * If provided it is returned, otherwise default log level is returned or
 * LOG_LEVEL, if it was locally defined.
 */
#if !defined(CONFIG_LOG)
#define _LOG_LEVEL_RESOLVE(...) LOG_LEVEL_NONE
#else
#define _LOG_LEVEL_RESOLVE(...) \
	Z_LOG_EVAL(LOG_LEVEL, \
		  (GET_ARG_N(2, __VA_ARGS__, LOG_LEVEL)), \
		  (GET_ARG_N(2, __VA_ARGS__, CONFIG_LOG_DEFAULT_LEVEL)))
#endif

/**
 * @brief Register the module with Logger.
 *
 * This macro normally must be used after including <logging/log.h> to
 * complete the initialization of the module.
 *
 * Module registration can be skipped in two cases:
 *
 * Macro accepts one or two parameters:
 * - module name
 * - optional log level. If not provided then default log level is used in
 *  the file.
 *
 * Example usage:
 * - LOG_MODULE_REGISTER(foo, CONFIG_FOO_LOG_LEVEL)
 */
#define LOG_MODULE_REGISTER(...)			\
	static const uint32_t __log_level __unused =			      \
					_LOG_LEVEL_RESOLVE(__VA_ARGS__)


void log_print(const char *fmt, ...);

static inline char log_level_to_char(int level)
{
    switch (level) {
	case LOG_LEVEL_ERR:
		return 'E';
	case LOG_LEVEL_WRN:
		return 'W';
	case LOG_LEVEL_INF:
		return 'I';
	case LOG_LEVEL_DBG:
		return 'D';
	default:
		return '?';
	}
}

#ifdef CONFIG_LOG
#define LOG(_level, fmt, ...) do {  \
	if (_level > __log_level) 	\
		break;	\
    log_print("%c: " fmt "\n",      \
        log_level_to_char(_level),  \
        ##__VA_ARGS__);             \
} while (false)
#else
#define LOG(...)
#endif

#define LOG_ERR(...)        LOG(LOG_LEVEL_ERR, __VA_ARGS__)

#define LOG_WRN(...)        LOG(LOG_LEVEL_WRN, __VA_ARGS__)

#define LOG_INF(...)        LOG(LOG_LEVEL_INF, __VA_ARGS__)

#define LOG_DBG(...)        LOG(LOG_LEVEL_DBG, __VA_ARGS__)

#define LOG_PRINT(...)      LOG(__VA_ARGS__)
#endif /* INCLUDE_LOGGING_LOG_H */