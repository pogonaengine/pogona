/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#pragma once

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
