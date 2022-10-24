# Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
# SPDX-License-Identifier: Apache-2.0

add_subdirectory(eeprom)
add_subdirectory(flash)

add_subdirectory_ifdef(CONFIG_ADC adc)
add_subdirectory_ifdef(CONFIG_DEBUG debug)
add_subdirectory_ifdef(CONFIG_GPIO gpio)
add_subdirectory_ifdef(CONFIG_PINCTRL pinctrl)
add_subdirectory_ifdef(CONFIG_UART uart)

