/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#pragma once

#include "defines.h"

typedef i32 (*PFN_pEngineEntry)(int argc, char** argv);

typedef struct pVector pVector;

/* this structure is primarily used by the renderer */
typedef struct {
	void (*loggerLog)(i8 level, const char* fmt, ...);
	i32 (*vectorCreate)(pVector* self, u64 typeSize);
	i32 (*vectorPush)(pVector* self, void* data);
	i32 (*vectorPop)(pVector* self);
	i32 (*vectorResize)(pVector* self, u64 size);
	i32 (*vectorShrinkToFit)(pVector* self);
	void (*vectorDestroy)(pVector* self);
} pEngine;
