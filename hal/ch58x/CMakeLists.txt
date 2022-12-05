# Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
# SPDX-License-Identifier: Apache-2.0

set_library()

library_include_directories(inc)

library_sources(
    ch58x_hal_adc.c
    ch58x_hal_clk.c
    ch58x_hal_flash.c
    ch58x_hal_gpio.c
    ch58x_hal_power.c
    ch58x_hal_rtc.c
    ch58x_hal_sys.c
    # ch58x_hal_systick.c
    ch58x_hal_uart.c

)