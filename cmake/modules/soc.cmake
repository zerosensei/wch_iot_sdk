# Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
# SPDX-License-Identifier: Apache-2.0

include_guard(GLOBAL)

find_path(SOC_DIR
        NAMES Kconfig.${SOC}
        PATHS ${ENV_WCH_BASE}/soc/*/*
        NO_DEFAULT_PATH)

message("SOC DIR: " ${SOC_DIR})

if(${SOC_DIR} STREQUAL SOC_DIR-NOTFOUND)
    message(FATAL_ERROR "Could not find SOC path for: ${SOC}")
endif()