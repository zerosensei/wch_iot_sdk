# Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
# SPDX-License-Identifier: Apache-2.0

wch_include_directories(
  ${ENV_WCH_BASE}/kernel/include
  ${ENV_WCH_BASE}/arch/${ARCH}/include
)

add_subdirectory(${ENV_WCH_BASE}/arch/${ARCH})