# SPDX-License-Identifier: Apache-2.0

set_ifndef(WCH_TOOLCHAIN_PATH   "$ENV{WCH_TOOLCHAIN_PATH}")
set(WCH_TOOLCHAIN_PATH  ${WCH_TOOLCHAIN_PATH} CACHE PATH "")
assert(WCH_TOOLCHAIN_PATH "WCH_TOOLCHAIN_PATH no set")

set(COMPILER gcc)
set(LINKER ld)
set(BINTOOLS gnu)

set(CROSS_COMLILE_TARGET riscv-none-embed)

set(TOOLCHAIN_HOME ${WCH_TOOLCHAIN_PATH})

set(CROSS_COMPILE ${TOOLCHAIN_HOME}/bin/${CROSS_COMLILE_TARGET}-)

message(STATUS "Found toolchain: wch gcc (${WCH_TOOLCHAIN_PATH})")