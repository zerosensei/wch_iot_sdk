# SPDX-License-Identifier: Apache-2.0

find_program(CMAKE_LINKER ${CROSS_COMPILE}ld.bfd ${LD_SEARCH_PATH})
if(NOT CMAKE_LINKER)
  find_program(CMAKE_LINKER ${CROSS_COMPILE}ld ${LD_SEARCH_PATH})
endif()

set_ifndef(LINKERFLAGPREFIX -Wl)

# Run $LINKER_SCRIPT file through the C preprocessor, producing ${linker_script_gen}
# NOTE: ${linker_script_gen} will be produced at build-time; not at configure-time
macro(configure_linker_script linker_script_gen linker_pass_define)
  set(extra_dependencies ${ARGN})
  set(template_script_defines ${linker_pass_define})
  list(TRANSFORM template_script_defines PREPEND "-D")

  # Only Ninja and Makefile generators support DEPFILE.
  if((CMAKE_GENERATOR STREQUAL "Ninja")
      OR (CMAKE_GENERATOR MATCHES "Makefiles")
  )
    set(linker_script_dep DEPFILE ${PROJECT_BINARY_DIR}/${linker_script_gen}.dep)
  else()
    # TODO: How would the linker script dependencies work for non-linker
    # script generators.
    message(STATUS "Warning; this generator is not well supported. The
  Linker script may not be regenerated when it should.")
    set(linker_script_dep "")
  endif()

  # zephyr_get_include_directories_for_lang(C current_includes)
  get_property(current_defines GLOBAL PROPERTY PROPERTY_LINKER_SCRIPT_DEFINES)

  if("${SPARSE}" STREQUAL "y")
    set(ld_command ${REAL_CC})
  else()
    set(ld_command ${CMAKE_C_COMPILER})
  endif()

  add_custom_command(
    OUTPUT ${linker_script_gen}
    DEPENDS
    ${LINKER_SCRIPT}
    ${AUTOCONF_H}
    ${extra_dependencies}
    # NB: 'linker_script_dep' will use a keyword that ends 'DEPENDS'
    ${linker_script_dep}
    COMMAND ${ld_command}
    -x assembler-with-cpp
    ${NOSYSDEF_CFLAG}
    -MD -MF ${linker_script_gen}.dep -MT ${linker_script_gen}
    -D_LINKER
    -D_ASMLANGUAGE
    -imacros ${AUTOCONF_H}
    ${current_includes}
    ${current_defines}
    ${template_script_defines}
    -E ${LINKER_SCRIPT}
    -P # Prevent generation of debug `#line' directives.
    -o ${linker_script_gen}
    VERBATIM
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    COMMAND_EXPAND_LISTS
  )
endmacro()



# Link a target to given libraries with toolchain-specific argument order
#
# Usage:
#   toolchain_ld_link_elf(
#     TARGET_ELF             <target_elf>
#     OUTPUT_MAP             <output_map_file_of_target>
#     LIBRARIES_PRE_SCRIPT   [libraries_pre_script]
#     LINKER_SCRIPT          <linker_script>
#     LIBRARIES_POST_SCRIPT  [libraries_post_script]
#     DEPENDENCIES           [dependencies]
#   )
function(toolchain_ld_link_elf)
  cmake_parse_arguments(
    TOOLCHAIN_LD_LINK_ELF                                     # prefix of output variables
    ""                                                        # list of names of the boolean arguments
    "TARGET_ELF;OUTPUT_MAP;LINKER_SCRIPT"                     # list of names of scalar arguments
    "LIBRARIES_PRE_SCRIPT;LIBRARIES_POST_SCRIPT;DEPENDENCIES" # list of names of list arguments
    ${ARGN}                                                   # input args to parse
  )

  if(${CMAKE_LINKER} STREQUAL "${CROSS_COMPILE}ld.bfd")
    # ld.bfd was found so let's explicitly use that for linking, see #32237
    set(use_linker "-fuse-ld=bfd")
  endif()

  target_link_libraries(
    ${TOOLCHAIN_LD_LINK_ELF_TARGET_ELF}
    ${TOOLCHAIN_LD_LINK_ELF_LIBRARIES_PRE_SCRIPT}
    ${use_linker}
    ${TOPT}
    ${TOOLCHAIN_LD_LINK_ELF_LINKER_SCRIPT}
    ${TOOLCHAIN_LD_LINK_ELF_LIBRARIES_POST_SCRIPT}

    ${LINKERFLAGPREFIX},-Map=${TOOLCHAIN_LD_LINK_ELF_OUTPUT_MAP}
    ${LINKERFLAGPREFIX},--whole-archive
    ${WCH_LIBS_PROPERTY}
    ${LINKERFLAGPREFIX},--no-whole-archive
    kernel
    # $<TARGET_OBJECTS:${OFFSETS_LIB}>
    ${LIB_INCLUDE_DIR}
    -L${PROJECT_BINARY_DIR}
    ${TOOLCHAIN_LIBS}

    ${TOOLCHAIN_LD_LINK_ELF_DEPENDENCIES}
  )
endfunction(toolchain_ld_link_elf)

include(${ENV_WCH_BASE}/cmake/linker/${LINKER}/target_base.cmake)
# include(${ENV_WCH_BASE}/cmake/linker/${LINKER}/target_baremetal.cmake)
# include(${ENV_WCH_BASE}/cmake/linker/${LINKER}/target_cpp.cmake)
# include(${ENV_WCH_BASE}/cmake/linker/${LINKER}/target_relocation.cmake)
