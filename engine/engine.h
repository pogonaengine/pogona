/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#pragma once

#include "defines.h"

typedef i32 (*PFN_pEngineEntry)(int argc, char** argv);

/* this structure is primarily used by the renderer */
typedef struct {
	void (*loggerLog)(i8 level, const char* fmt, ...);
} pEngine;
