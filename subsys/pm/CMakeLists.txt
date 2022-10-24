# Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
# SPDX-License-Identifier: Apache-2.0

if(CONFIG_PM)
    add_sources(device.c pm.c policy.c)
endif()