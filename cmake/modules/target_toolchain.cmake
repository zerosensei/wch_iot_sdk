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





message("target COMPILER:" ${COMPILER})
include(${ENV_WCH_BASE}/cmake/compiler/${COMPILER}/target.cmake OPTIONAL)
message("target LINKER:" ${LINKER})
include(${ENV_WCH_BASE}/cmake/linker/${LINKER}/target.cmake OPTIONAL)
message("target BINTOOLS:" ${BINTOOLS})
include(${ENV_WCH_BASE}/cmake/bintools/${BINTOOLS}/target.cmake OPTIONAL)




