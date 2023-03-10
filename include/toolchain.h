/*
 * Copyright (c) 2022 Nanjing Qinheng Microelectronics Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef INCLUDE_TOOLCHAIN_H
#define INCLUDE_TOOLCHAIN_H

/**
 * @def HAS_BUILTIN(x)
 * @brief Check if the compiler supports the built-in function \a x.
 *
 * This macro is for use with conditional compilation to enable code using a
 * builtin function that may not be available in every compiler.
 */
#ifdef __has_builtin
#define HAS_BUILTIN(x) __has_builtin(x)
#else
/*
 * The compiler doesn't provide the __has_builtin() macro, so instead we depend
 * on the toolchain-specific headers to define HAS_BUILTIN_x for the builtins
 * supported.
 */
#define HAS_BUILTIN(x) HAS_BUILTIN_##x
#endif

#if defined(__GNUC__) || (defined(_LINKER) && defined(__GCC_LINKER_CMD__))
#include <toolchain/gcc.h>
#else
#error "Invalid/unknown toolchain configuration"
#endif

/**
 * @def GCC_VERSION
 * @brief GCC version in xxyyzz for xx.yy.zz. Zero if not GCC compatible.
 */
#ifndef GCC_VERSION
#define GCC_VERSION 0
#endif

/**
 * @def CLANG_VERSION
 * @brief Clang version in xxyyzz for xx.yy.zz. Zero if not Clang compatible.
 */
#ifndef CLANG_VERSION
#define CLANG_VERSION 0
#endif

/**
 * @def TOOLCHAIN_HAS_PRAGMA_DIAG
 * @brief Indicate if toolchain supports \#pragma diagnostics.
 */
#ifndef TOOLCHAIN_HAS_PRAGMA_DIAG
#define TOOLCHAIN_HAS_PRAGMA_DIAG 0
#endif

/**
 * @def TOOLCHAIN_HAS_C_GENERIC
 * @brief Indicate if toolchain supports C Generic.
 */
#if __STDC_VERSION__ >= 201112L
/* _Generic is introduced in C11, so it is supported. */
# ifdef TOOLCHAIN_HAS_C_GENERIC
#  undef TOOLCHAIN_HAS_C_GENERIC
# endif
# define TOOLCHAIN_HAS_C_GENERIC 1
#else
# ifndef TOOLCHAIN_HAS_C_GENERIC
#  define TOOLCHAIN_HAS_C_GENERIC 0
# endif
#endif

/**
 * @def TOOLCHAIN_HAS_C_AUTO_TYPE
 * @brief Indicate if toolchain supports C __auto_type.
 */
#ifndef TOOLCHAIN_HAS_C_AUTO_TYPE
#define TOOLCHAIN_HAS_C_AUTO_TYPE 0
#endif

/*
 * Ensure that __BYTE_ORDER__ and related preprocessor definitions are defined,
 * as these are often used without checking for definition and doing so can
 * cause unexpected behaviours.
 */
#ifndef _LINKER
#if !defined(__BYTE_ORDER__) || !defined(__ORDER_BIG_ENDIAN__) || \
    !defined(__ORDER_LITTLE_ENDIAN__)

#error "__BYTE_ORDER__ is not defined"

#endif
#endif /* !_LINKER */

#endif /* INCLUDE_TOOLCHAIN_H */
