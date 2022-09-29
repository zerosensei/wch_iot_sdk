# SPDX-License-Identifier: Apache-2.0


set(CMAKE_EXECUTABLE_SUFFIX .elf)

add_library(wch_interface INTERFACE)

library_set_named(WCH)

include_directories(include)


include_directories(${SOC_DIR}/${ARCH}/${SOC_SERIES})


message("top")
