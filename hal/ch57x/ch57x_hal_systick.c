/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ch57x_common.h>
/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ch57x_common.h>

void hal_systick_definit(void)
{
    hal_systick_clk(SYSTICK_CLK_DIV_1);
    hal_systick_int_enable();
    hal_systick_reload_enable();
    hal_systick_enable();
}
