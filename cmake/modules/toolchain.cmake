# SPDX-License-Identifier: Apache-2.0

include_guard(GLOBAL)

set(CMAKE_C_COMPILER_FORCED 1)
set(CMAKE_CXX_COMPILER_FORCED 1)

set(CMAKE_SYSTEM_NAME Generic)

set(CMAKE_SYSTEM_PROCESSOR ${ARCH})

set(CMAKE_SYSTEM_VERSION ${PROJECT_VERSION})

set(BUILD_SHARED_LIBS OFF)

add_custom_target(asm)
add_custom_target(compiler)
add_custom_target(compiler-cpp)
add_custom_target(linker)

# TODO: toolchain select?
if (${ARCH} STREQUAL "riscv")
    set(TOOLCHAIN_NAME  "gcc_wch")
endif()

if (${ARCH} STREQUAL "arm")
    set(TOOLCHAIN_NAME  "armclang")
endif()

include(${ENV_WCH_BASE}/cmake/toolchain/${TOOLCHAIN_NAME}/generic.cmake)

assert(TOOLCHAIN_HOME "TOOLCHAIN_HOME not set")

include(${ENV_WCH_BASE}/cmake/compiler/${COMPLIER}/generic.cmake OPTIONAL)
include(${ENV_WCH_BASE}/cmake/linker/${LINKER}/generic.cmake OPTIONAL)
include(${ENV_WCH_BASE}/cmake/bintools/${BINTOOLS}/generic.cmake OPTIONAL)




