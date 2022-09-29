# Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
# SPDX-License-Identifier: Apache-2.0

include_guard(GLOBAL)

message("kernel here")

file(TO_CMAKE_PATH "ENV_WCH_BASE" PROJECT_SOURCE_DIR)

SET(WCH_BINARY_DIR ${PROJECT_BINARY_DIR})

if(${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_BINARY_DIR})
    message(FATAL_ERROR "Source directory equals build directory.")
endif()

# TODO: pristine ?

project(wch-project VERSION ${PROJECT_VERSION})

enable_language(C CXX ASM)

set(PROJECT_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(PROJECT_SOURCE_DIR ${ENV_WCH_BASE})

set(KERNEL_NAME     ${CONFIG_KERNEL_BIN_NAME})

set(KERNEL_ELF_NAME   ${KERNEL_NAME}.elf)
set(KERNEL_BIN_NAME   ${KERNEL_NAME}.bin)
set(KERNEL_HEX_NAME   ${KERNEL_NAME}.hex)
set(KERNEL_UF2_NAME   ${KERNEL_NAME}.uf2)
set(KERNEL_MAP_NAME   ${KERNEL_NAME}.map)
set(KERNEL_LST_NAME   ${KERNEL_NAME}.lst)
set(KERNEL_S19_NAME   ${KERNEL_NAME}.s19)
set(KERNEL_EXE_NAME   ${KERNEL_NAME}.exe)
set(KERNEL_STAT_NAME  ${KERNEL_NAME}.stat)
set(KERNEL_STRIP_NAME ${KERNEL_NAME}.strip)
set(KERNEL_META_NAME  ${KERNEL_NAME}.meta)
set(KERNEL_SYMBOLS_NAME    ${KERNEL_NAME}.symbols)

add_library(app STATIC "")

target_link_libraries(app PUBLIC wch_interface)