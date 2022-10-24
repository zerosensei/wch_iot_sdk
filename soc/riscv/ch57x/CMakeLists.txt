# Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
# SPDX-License-Identifier: Apache-2.0

set_library()


# library_include_directories(${ENV_WCH_BASE}/hal/${SOC_SERIES}/inc)

library_sources(
    dev_cfg.c
    pin_cfg.c
    power.c
    soc_irq.c
    soc.c
)

library_sources(startup_CH573.S)

# TODO: linker