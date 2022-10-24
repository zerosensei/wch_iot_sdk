# SPDX-License-Identifier: Apache-2.0

set_library()

library_sources(
  fatal.c
)

library_sources_ifdef(CONFIG_RISCV_PMP core_pmp.c)

