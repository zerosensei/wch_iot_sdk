/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sys/printk.h>
#include <sys/__assert.h>

__weak void assert_post_action(const char *file, unsigned int line)
{
	ARG_UNUSED(file);
	ARG_UNUSED(line);
}

void assert_printf(const char *fmt, ...)
{
    va_list ap;

	va_start(ap, fmt);

	vprintk(fmt, ap);

	va_end(ap);
}