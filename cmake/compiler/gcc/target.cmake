# SPDX-License-Identifier: Apache-2.0

set_ifndef(C++ g++)

find_program(CMAKE_C_COMPILER ${CROSS_COMPILE}${CC} PATHS ${TOOLCHAIN_HOME} NO_DEFAULT_PATH)

if(${CMAKE_C_COMPILER} STREQUAL CMAKE_C_COMPILER-NOTFOUND)
  message(FATAL_ERROR "C compiler ${CROSS_COMPILE}${CC} not found - Please check your toolchain installation")
endif()

if(CONFIG_CPLUSPLUS)
  set(cplusplus_compiler ${CROSS_COMPILE}${C++})
else()
  if(EXISTS ${CROSS_COMPILE}${C++})
    set(cplusplus_compiler ${CROSS_COMPILE}${C++})
  else()
    # When the toolchain doesn't support C++, and we aren't building
    # with C++ support just set it to something so CMake doesn't
    # crash, it won't actually be called
    set(cplusplus_compiler ${CMAKE_C_COMPILER})
  endif()
endif()
find_program(CMAKE_CXX_COMPILER ${cplusplus_compiler} PATHS ${TOOLCHAIN_HOME} NO_DEFAULT_PATH)

set(NOSTDINC "")

# Note that NOSYSDEF_CFLAG may be an empty string, and
# set_ifndef() does not work with empty string.
if(NOT DEFINED NOSYSDEF_CFLAG)
  set(NOSYSDEF_CFLAG -undef)
endif()

foreach(file_name include/stddef.h include-fixed/limits.h)
  execute_process(
    COMMAND ${CMAKE_C_COMPILER} --print-file-name=${file_name}
    OUTPUT_VARIABLE _OUTPUT
    )
  get_filename_component(_OUTPUT "${_OUTPUT}" DIRECTORY)
  string(REGEX REPLACE "\n" "" _OUTPUT "${_OUTPUT}")

  list(APPEND NOSTDINC ${_OUTPUT})
endforeach()

if("${ARCH}" STREQUAL "arm")
  include(${ENV_WCH_BASE}/cmake/compiler/gcc/target_arm.cmake)
elseif("${ARCH}" STREQUAL "riscv")
  include(${CMAKE_CURRENT_LIST_DIR}/target_riscv.cmake)
endif()


# This libgcc code is partially duplicated in compiler/*/target.cmake
execute_process(
  COMMAND ${CMAKE_C_COMPILER} ${TOOLCHAIN_C_FLAGS} --print-libgcc-file-name
  OUTPUT_VARIABLE LIBGCC_FILE_NAME
  OUTPUT_STRIP_TRAILING_WHITESPACE
  )

if(NOT EXISTS ${LIBGCC_FILE_NAME})
  message(FATAL_ERROR "No such file or directory: LIBGCC_FILE_NAME: ${LIBGCC_FILE_NAME}")
endif()

get_filename_component(LIBGCC_DIR ${LIBGCC_FILE_NAME} DIRECTORY)

if(NOT EXISTS ${LIBGCC_DIR})
  message(FATAL_ERROR "No such file or directory: LIBGCC_DIR: ${LIBGCC_DIR}")
endif()

LIST(APPEND LIB_INCLUDE_DIR "-L\"${LIBGCC_DIR}\"")
LIST(APPEND TOOLCHAIN_LIBS gcc)

# For CMake to be able to test if a compiler flag is supported by the
# toolchain we need to give CMake the necessary flags to compile and
# link a dummy C file.
#
# CMake checks compiler flags with check_c_compiler_flag()
foreach(isystem_include_dir ${NOSTDINC})
  list(APPEND isystem_include_flags -isystem "\"${isystem_include_dir}\"")
endforeach()

# Appending onto any existing values lets users specify
# toolchain-specific flags at generation time.
list(APPEND CMAKE_REQUIRED_FLAGS
  -nostartfiles
  ${isystem_include_flags}
  -Wl,--unresolved-symbols=ignore-in-object-files
  -Wl,--entry=0 # Set an entry point to avoid a warning
  )
string(REPLACE ";" " " CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS}")
