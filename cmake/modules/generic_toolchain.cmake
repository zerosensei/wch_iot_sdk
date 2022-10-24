# SPDX-License-Identifier: Apache-2.0

include_guard(GLOBAL)

include(functions)

# Prevent CMake from testing the toolchain
set(CMAKE_C_COMPILER_FORCED   1)
set(CMAKE_CXX_COMPILER_FORCED 1)

if(NOT TOOLCHAIN_ROOT)
  if(DEFINED ENV{WCH_TOOLCHAIN_PATH})
    # Support for out-of-tree toolchain
    set(TOOLCHAIN_ROOT $ENV{TOOLCHAIN_ROOT})
  else()
    # Default toolchain cmake file
    set(TOOLCHAIN_ROOT ${ENV_WCH_BASE})
  endif()
endif()

# zephyr_file(APPLICATION_ROOT TOOLCHAIN_ROOT)

# Don't inherit compiler flags from the environment
foreach(var AFLAGS CFLAGS CXXFLAGS CPPFLAGS LDFLAGS)
  if(DEFINED ENV{${var}})
    message(WARNING "The environment variable '${var}' was set to $ENV{${var}},
but Zephyr ignores flags from the environment. Use 'cmake -DEXTRA_${var}=$ENV{${var}}' instead.")
    unset(ENV{${var}})
  endif()
endforeach()



# set(TOOLCHAIN_ROOT ${TOOLCHAIN_ROOT} CACHE STRING "Zephyr toolchain root" FORCE)
# assert(TOOLCHAIN_ROOT "Zephyr toolchain root path invalid: please set the TOOLCHAIN_ROOT-variable")


# Set cached ZEPHYR_TOOLCHAIN_VARIANT.
# set(ZEPHYR_TOOLCHAIN_VARIANT ${ZEPHYR_TOOLCHAIN_VARIANT} CACHE STRING "Zephyr toolchain variant")

# message("ZEPHYR_TOOLCHAIN_VARIANT:" ${ZEPHYR_TOOLCHAIN_VARIANT})

# TODO: toolchain select?
if (${ARCH} STREQUAL "riscv")
    set(TOOLCHAIN_NAME  "gcc_wch")
endif()

if (${ARCH} STREQUAL "arm")
    set(TOOLCHAIN_NAME  "armclang")
endif()


message("TOOLCHAIN NAME")

# Configure the toolchain based on what SDK/toolchain is in use.
include(${ENV_WCH_BASE}/cmake/toolchain/${TOOLCHAIN_NAME}/generic.cmake)

# set_ifndef(TOOLCHAIN_KCONFIG_DIR ${TOOLCHAIN_ROOT}/cmake/toolchain/${ZEPHYR_TOOLCHAIN_VARIANT})

# Configure the toolchain based on what toolchain technology is used
# (gcc, host-gcc etc.)

message("COMPILER:" ${COMPILER})

include(${ENV_WCH_BASE}/cmake/compiler/${COMPILER}/generic.cmake OPTIONAL)
include(${ENV_WCH_BASE}/cmake/linker/${LINKER}/generic.cmake OPTIONAL)
include(${ENV_WCH_BASE}/cmake/bintools/${BINTOOLS}/generic.cmake OPTIONAL)
