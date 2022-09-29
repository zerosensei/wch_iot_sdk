# SPDX-License-Identifier: Apache-2.0

find_program(CMAKE_C_COMPILER ${CROSS_COMPILE}gcc   PATHS ${TOOLCHAIN_HOME} NO_DEFAULT_PATH)
find_program(CMAKE_GCOV ${CROSS_COMPILE}gcov   PATHS ${TOOLCHAIN_HOME} NO_DEFAULT_PATH)

if(CMAKE_C_COMPILER STREQUAL CMAKE_C_COMPILER-NOTFOUND)
  message(FATAL_ERROR "Unable to find the toolchain. Is the environment misconfigured?
User-configuration:
TOOLCHAIN_NAME: ${TOOLCHAIN_NAME}
Internal variables:
CROSS_COMPILE: ${CROSS_COMPILE}
TOOLCHAIN_HOME: ${TOOLCHAIN_HOME}
")
endif()

execute_process(
  COMMAND ${CMAKE_C_COMPILER} --version
  RESULT_VARIABLE ret
  OUTPUT_QUIET
  ERROR_QUIET
  )

if(ret)
  message(FATAL_ERROR "Executing the below command failed. Are permissions set correctly?
'${CMAKE_C_COMPILER} --version'" )
endif()

if(CONFIG_CPLUSPULS)
  set(cplusplus_compiler ${CROSS_COMPILE}g++)
else()
  if(EXISTS ${CROSS_COMPILE}g++)
    set(cplusplus_compiler ${CROSS_COMPILE}g++)
  else()
    set(cplusplus_compiler ${CMAKE_C_COMPILER})
  endif()
endiF()

find_program(CMAKE_CXX_COMPILER ${cplusplus_compiler} PATHS ${TOOLCHAIN_HOME} NO_DEFAULT_PATH)

if("${ARCH}" STREQUAL "arm")
  include(${ENV_WCH_BASE}/cmake/compiler/gcc/target_arm.cmake)
elseif("${ARCH}" STREQUAL "riscv")
  include(${ENV_WCH_BASE}/cmake/compiler/gcc/target_riscv.cmake)
endif()

