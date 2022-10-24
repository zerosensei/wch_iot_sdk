# Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
# SPDX-License-Identifier: Apache-2.0

set_library()


library_include_directories(inc)


library_sources(
    ch57x_hal_adc.c
    ch57x_hal_clk.c
    ch57x_hal_flash.c
    ch57x_hal_gpio.c
    ch57x_hal_power.c
    ch57x_hal_rtc.c
    ch57x_hal_sys.c
    ch57x_hal_systick.c
    ch57x_hal_uart.c

)