# Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
# SPDX-License-Identifier: Apache-2.0

wch_include_directories(${ENV_WCH_BASE}/hal/${SOC_SERIES}/inc)

wch_sources(
    dev_cfg.c
    pin_cfg.c
    soc_irq.c
    soc.c  
    startup_CH573.S
)

if(CONFIG_PM)
    wch_sources(power.c)
endif()

wch_ld_options(
    --specs=nosys.specs
    -nostartfiles
    --specs=nano.specs
  )