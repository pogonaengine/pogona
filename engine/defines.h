/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define pARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))
#define pCLAMP(x, min, max) ((x <= min) ? min : (x >= max) ? max : x)
#define pMIN(x, y) (x < y ? x : y)
#define pMAX(x, y) (x > y ? x : y)

#define pINLINE static inline

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
