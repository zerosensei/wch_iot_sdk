# Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
# SPDX-License-Identifier: Apache-2.0

include_guard(GLOBAL)

list(APPEND cmake_modules python)

list(APPEND cmake_modules version)

list(APPEND cmake_modules functions)

list(APPEND cmake_modules soc)

list(APPEND cmake_modules arch)

list(APPEND cmake_modules kconfig)

list(APPEND cmake_modules toolchain)

foreach(module IN LISTS cmake_modules)
  # Ensures any module of type `${module}` are properly expanded to list before
  # passed on the `include(${module})`.
  # This is done twice to support cases where the content of `${module}` itself
  # contains a variable, like `${BOARD_DIR}`.
  string(CONFIGURE "${module}" module)
  string(CONFIGURE "${module}" module)
  include(${module})

endforeach()


include(kernel)