# SPDX-License-Identifier: Apache-2.0

# See root CMakeLists.txt for description and expectations of these macros

macro(toolchain_ld_base)

  if(NOT PROPERTY_LINKER_SCRIPT_DEFINES)
    set_property(GLOBAL PROPERTY PROPERTY_LINKER_SCRIPT_DEFINES -D__GCC_LINKER_CMD__)
  endif()

  # TOOLCHAIN_LD_FLAGS comes from compiler/gcc/target.cmake
  # LINKERFLAGPREFIX comes from linker/ld/target.cmake
  wch_ld_options(
    -no-pie
    ${TOOLCHAIN_LD_FLAGS}
  )

  wch_ld_options(
    ${LINKERFLAGPREFIX},--gc-sections
    ${LINKERFLAGPREFIX},--build-id=none
  )

endmacro()
