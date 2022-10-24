# Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
# SPDX-License-Identifier: Apache-2.0

include_guard(GLOBAL)

define_property(GLOBAL PROPERTY WCH_LIBS
    BRIEF_DOCS "Global list of all Zephyr CMake libs that should be linked in"
    FULL_DOCS  "Global list of all Zephyr CMake libs that should be linked in.
zephyr_library() appends libs to this list.")
set_property(GLOBAL PROPERTY WCH_LIBS "")

define_property(GLOBAL PROPERTY INTERFACE_LIBS
    BRIEF_DOCS "Global list of all Zephyr interface libs that should be linked in."
    FULL_DOCS  "Global list of all Zephyr interface libs that should be linked in.
zephyr_interface_library_named() appends libs to this list.")
set_property(GLOBAL PROPERTY INTERFACE_LIBS "")

define_property(GLOBAL PROPERTY GENERATED_APP_SOURCE_FILES
  BRIEF_DOCS "Source files that are generated after Zephyr has been linked once."
  FULL_DOCS "\
Source files that are generated after Zephyr has been linked once.\
May include dev_handles.c etc."
  )
set_property(GLOBAL PROPERTY GENERATED_APP_SOURCE_FILES "")


file(TO_CMAKE_PATH "ENV_WCH_BASE" PROJECT_SOURCE_DIR)

SET(WCH_BINARY_DIR ${PROJECT_BINARY_DIR})

if(${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_BINARY_DIR})
    message(FATAL_ERROR "Source directory equals build directory.")
endif()

# TODO: pristine ?

project(wch-project VERSION ${PROJECT_VERSION})

enable_language(C CXX ASM)


# include(${ENV_WCH_BASE}/cmake/compiler_flags_template.cmake)


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

# "app" is a CMake library containing all the application code and is
# modified by the entry point ${APPLICATION_SOURCE_DIR}/CMakeLists.txt
# that was specified when cmake was called.
library_set_named(app)
set_property(TARGET app PROPERTY ARCHIVE_OUTPUT_DIRECTORY app)

add_subdirectory(${ENV_WCH_BASE}/ ${__build_dir})

# Link 'app' with the interface libraries.
get_property(INTERFACE_LIBS_PROPERTY GLOBAL PROPERTY INTERFACE_LIBS)
foreach(lib ${INTERFACE_LIBS_PROPERTY})
    string(TOUPPER ${lib} lib_upper_case)
    target_link_libraries_ifdef(
        CONFIF_APP_LINK_WITH_${lib_upper_case}
        app
        PUBLIC
        ${lib}
    )
endforeach()
