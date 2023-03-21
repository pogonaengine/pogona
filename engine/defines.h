/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef PEXPORT
# ifdef _MSC_VER
#  define PAPI __declspec(dllexport)
# else
#  define PAPI __attribute__((visibility("default")))
# endif /* _MSC_VER */
#else
# ifdef _MSC_VER
#  define PAPI __declspec(dllimport)
# else
#  define PAPI
# endif /* _MSC_VER */
#endif /* PEXPORT */

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;

typedef float    f32;
typedef double   f64;
