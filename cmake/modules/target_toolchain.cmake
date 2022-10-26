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

# The 'generic' compiler and the 'target' compiler might be different,
# so we unset the 'generic' one and thereby force the 'target' to
# re-set it.
unset(CMAKE_C_COMPILER)
unset(CMAKE_C_COMPILER CACHE)

add_custom_target(bintools)

include(${ENV_WCH_BASE}/cmake/compiler/${COMPILER}/target.cmake OPTIONAL)
include(${ENV_WCH_BASE}/cmake/linker/${LINKER}/target.cmake OPTIONAL)
include(${ENV_WCH_BASE}/cmake/bintools/bintools_template.cmake)
include(${ENV_WCH_BASE}/cmake/bintools/${BINTOOLS}/target.cmake OPTIONAL)




