# Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
# SPDX-License-Identifier: Apache-2.0

include_guard(GLOBAL)

include(functions)

set_ifndef(AUTOCONF_H ${PROJECT_BINARY_DIR}/autoconf.h)
# Re-configure (Re-execute all CMakeLists.txt code) when autoconf.h changes
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${AUTOCONF_H})

file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/kconfig)

set(DOTCONFIG ${PROJECT_BINARY_DIR}/Kconfig/.config)
set(KCONFIG_SOURCES_TXT ${PROJECT_BINARY_DIR}/Kconfig/source.txt)


#TODO: other config file?
file(GLOB config_file ${APPLICATION_SOURCE_DIR}/*.conf)
list(SORT config_file)
set(append_config_files 
    ${config_file}
)

set(INPUT_CONFIGS ${append_config_files})


set(COMMON_KCONFIG_ENV_SETTINGS
    PYTHON_EXECUTABLE=${PYTHON_EXECUTABLE}
)

message("base: " ${ENV_WCH_BASE})


execute_process(
    COMMAND ${CMAKE_COMMAND} -E env
    ${COMMON_KCONFIG_ENV_SETTINGS}
    ${PYTHON_EXECUTABLE}
    ${ENV_WCH_BASE}/script/kconfig/kconfig.py
    ${ENV_WCH_BASE}/Kconfig
    ${DOTCONFIG}
    ${AUTOCONF_H}
    ${KCONFIG_SOURCES_TXT}
    ${INPUT_CONFIGS}
    WORKING_DIRECTORY ${APPLICATION_SOURCE_DIR}
    RESULT_VARIABLE ret
)

import_kconfig("CONFIG" ${DOTCONFIG})
