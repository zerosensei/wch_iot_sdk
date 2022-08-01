/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef INCLUDE_LOGGING_LOG_H
#define INCLUDE_LOGGING_LOG_H

#include <stddef.h>
#include <stdint.h>

#define LOG_LEVEL_NONE 0U
#define LOG_LEVEL_ERR  1U
#define LOG_LEVEL_WRN  2U
#define LOG_LEVEL_INF  3U
#define LOG_LEVEL_DBG  4U

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

#define LOG(_level, fmt, ...) do {  \
    log_print("%c: " fmt "\n",      \
        log_level_to_char(_level),  \
        ##__VA_ARGS__);             \
} while (false)

#define LOG_ERR(...)        LOG(LOG_LEVEL_ERR, __VA_ARGS__)

#define LOG_WRN(...)        LOG(LOG_LEVEL_ERR, __VA_ARGS__)

#define LOG_INF(...)        LOG(LOG_LEVEL_ERR, __VA_ARGS__)

#define LOG_DBG(...)        LOG(LOG_LEVEL_ERR, __VA_ARGS__)

#define LOG_PRINT(...)      LOG(__VA_ARGS__)
#endif /* INCLUDE_LOGGING_LOG_H */