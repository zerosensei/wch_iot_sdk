# Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
# SPDX-License-Identifier: Apache-2.0

include_guard(GLOBAL)

include(functions)

set_ifndef(AUTOCONF_H ${PROJECT_BINARY_DIR}/kconfig/autoconf.h)
# Re-configure (Re-execute all CMakeLists.txt code) when autoconf.h changes
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${AUTOCONF_H})

file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/kconfig)

set(DOTCONFIG ${PROJECT_BINARY_DIR}/Kconfig/.config)
set(KCONFIG_SOURCES_TXT ${PROJECT_BINARY_DIR}/Kconfig/source.txt)




set_ifndef(SOC_DEFCONFIG    
    ${ENV_WCH_BASE}/soc/${ARCH}/${SOC_SERIES}/${SOC_SERIES}_defconfig)

#TODO: other config file?
file(GLOB config_file ${APPLICATION_SOURCE_DIR}/*.conf)
list(SORT config_file)
set(append_config_files 
    ${SOC_DEFCONFIG}
    ${config_file}
)

set(INPUT_CONFIGS ${append_config_files})


set(COMMON_KCONFIG_ENV_SETTINGS
    PYTHON_EXECUTABLE=${PYTHON_EXECUTABLE}
    ARCH=${ARCH}
    SOC_SERIES=${SOC_SERIES}
)

set(EXTRA_KCONFIG_TARGET_COMMAND_FOR_menuconfig
  ${ENV_WCH_BASE}/script/kconfig/menuconfig.py
  )

set(EXTRA_KCONFIG_TARGET_COMMAND_FOR_guiconfig
  ${ENV_WCH_BASE}/script/kconfig/guiconfig.py
  )

set_ifndef(KCONFIG_TARGETS menuconfig guiconfig)

foreach(kconfig_target ${KCONFIG_TARGETS})
    add_custom_target(
        ${kconfig_target}
        ${CMAKE_COMMAND} -E env
        ${COMMON_KCONFIG_ENV_SETTINGS}
        ${PYTHON_EXECUTABLE}
        ${EXTRA_KCONFIG_TARGET_COMMAND_FOR_${kconfig_target}}
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/kconfig
        USES_TERMINAL
        COMMAND_EXPAND_LISTS
    )
    
endforeach()

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
