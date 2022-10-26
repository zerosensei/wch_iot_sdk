# Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
# SPDX-License-Identifier: Apache-2.0

include_guard(GLOBAL)

cmake_path(GET SOC_DIR PARENT_PATH soc_arch_dir)
cmake_path(GET soc_arch_dir FILENAME ARCH)

if(EXISTS ${ENV_WCH_BASE}/arch/${ARCH}/CMakeLists.txt)
    set(ARCH_DIR ${ENV_WCH_BASE}/arch)
endif()

if(NOT ARCH_DIR)
    message(FATAL_ERROR "Could not find arch path for ${ARCH}")
endif()