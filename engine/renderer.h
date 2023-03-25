/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#pragma once

#include "defines.h"
#include "engine.h"
#include "window/window.h"

/* It is stored in the renderer's library as 'gEntry' */
typedef struct {
	i32 (*create)(pEngine* engine, pWindow* window);
	i32 (*beginFrame)(void);
	i32 (*endFrame)(void);
	i32 (*destroy)(void);
} pRendererEntry;

typedef struct {
	pRendererEntry* entry;
	void* libraryHandle;
} pRendererState;

#ifndef pRENDERER
extern pRendererState gRenderer;
#else
extern pRendererEntry gEntry;
#endif

i32 pLoadRenderer(const char* path);
void pUnloadRenderer(void);
