# Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
# SPDX-License-Identifier: Apache-2.0

wch_link_libraries(
    -L${ENV_WCH_BASE}/lib/${SOC_SERIES}
)

wch_link_libraries(ISP583)

wch_link_libraries_ifdef(CONFIG_BT CH58xBLE)