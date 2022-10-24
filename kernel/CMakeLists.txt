# Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
# SPDX-License-Identifier: Apache-2.0

set_library()

library_include_directories(include)

library_sources(device.c
        fatal.c
        init.c
        timeout.c)

message("kernel add source")