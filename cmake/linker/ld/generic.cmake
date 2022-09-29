# SPDX-License-Identifier: Apache-2.0

find_program(CMAKE_LINKER ${CROSS_COMPILE}ld.bfd ${LD_SEARCH_PATH})
if(NOT CMAKE_LINKER)
  find_program(CMAKE_LINKER ${CROSS_COMPILE}ld ${LD_SEARCH_PATH})
endif()

# include(${ENV_WCH_BASE}/cmake/linker/${LINKER}/target_base.cmake)
# include(${ENV_WCH_BASE}/cmake/linker/${LINKER}/target_baremetal.cmake)
# include(${ENV_WCH_BASE}/cmake/linker/${LINKER}/target_cpp.cmake)
# include(${ENV_WCH_BASE}/cmake/linker/${LINKER}/target_relocation.cmake)
