
/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sys/printk.h>
#include <ctype.h>
#include <logging/log.h>

void log_print(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vprintk(fmt, ap);
	va_end(ap);
}