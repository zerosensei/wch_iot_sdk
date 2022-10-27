# SPDX-License-Identifier: Apache-2.0

include_guard(GLOBAL)

include(functions)

# Prevent CMake from testing the toolchain
set(CMAKE_C_COMPILER_FORCED   1)
set(CMAKE_CXX_COMPILER_FORCED 1)

set(CMAKE_SYSTEM_NAME Generic)

set(CMAKE_SYSTEM_PROCESSOR ${ARCH})

set(CMAKE_SYSTEM_VERSION ${PROJECT_VERSION})

set(BUILD_SHARED_LIBS OFF)

add_custom_target(asm)
add_custom_target(compiler)
add_custom_target(compiler-cpp)
add_custom_target(linker)
add_custom_target(bintools)

unset(CMAKE_C_COMPILER)
unset(CMAKE_C_COMPILER CACHE)

if(NOT TOOLCHAIN_ROOT)
  if(DEFINED ENV{WCH_TOOLCHAIN_PATH})
    # Support for out-of-tree toolchain
    set(TOOLCHAIN_ROOT $ENV{TOOLCHAIN_ROOT})
  else()
    # Default toolchain cmake file
    set(TOOLCHAIN_ROOT ${ENV_WCH_BASE})
  endif()
endif()


# Don't inherit compiler flags from the environment
foreach(var AFLAGS CFLAGS CXXFLAGS CPPFLAGS LDFLAGS)
  if(DEFINED ENV{${var}})
    message(WARNING "The environment variable '${var}' was set to $ENV{${var}},
but Zephyr ignores flags from the environment. Use 'cmake -DEXTRA_${var}=$ENV{${var}}' instead.")
    unset(ENV{${var}})
  endif()
endforeach()

# TODO: toolchain select?
if (${ARCH} STREQUAL "riscv")
    set(TOOLCHAIN_NAME  "gcc_wch")
endif()

if (${ARCH} STREQUAL "arm")
    set(TOOLCHAIN_NAME  "armclang")
endif()

# Configure the toolchain based on what SDK/toolchain is in use.
include(${ENV_WCH_BASE}/cmake/toolchain/${TOOLCHAIN_NAME}/generic.cmake)

# Configure the toolchain based on what toolchain technology is used
# (gcc, host-gcc etc.)
include(${ENV_WCH_BASE}/cmake/compiler/${COMPILER}/generic.cmake OPTIONAL)
include(${ENV_WCH_BASE}/cmake/linker/${LINKER}/generic.cmake OPTIONAL)
include(${ENV_WCH_BASE}/cmake/bintools/${BINTOOLS}/generic.cmake OPTIONAL)

include(${ENV_WCH_BASE}/cmake/compiler/${COMPILER}/target.cmake OPTIONAL)
include(${ENV_WCH_BASE}/cmake/linker/${LINKER}/target.cmake OPTIONAL)
include(${ENV_WCH_BASE}/cmake/bintools/bintools_template.cmake)
include(${ENV_WCH_BASE}/cmake/bintools/${BINTOOLS}/target.cmake OPTIONAL)