# Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
# SPDX-License-Identifier: Apache-2.0

include_guard(GLOBAL)

find_path(SOC_DIR
        NAMES Kconfig.soc
        PATHS ${ENV_WCH_BASE}/soc/*/${SOC_SERIES}
        NO_DEFAULT_PATH)

if(${SOC_DIR} STREQUAL SOC_DIR-NOTFOUND)
    message(FATAL_ERROR "Could not find SOC path for: ${SOC_SERIES}")
endif()