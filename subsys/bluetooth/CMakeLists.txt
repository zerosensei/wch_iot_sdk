# Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
# SPDX-License-Identifier: Apache-2.0

set_library()

library_sources(
    bt_init.c
)

add_subdirectory(hal)
add_subdirectory(host)
add_subdirectory(services)