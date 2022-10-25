# Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
# SPDX-License-Identifier: Apache-2.0

add_include_directories(${ENV_WCH_BASE}/hal/${SOC_SERIES}/inc)

add_sources(
    dev_cfg.c
    pin_cfg.c
    power.c
    soc_irq.c
    soc.c  
    startup_CH573.S
)


# TODO: linker