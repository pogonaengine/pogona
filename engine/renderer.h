/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#pragma once

#include "defines.h"
#include "engine.h"

/* It is stored in the renderer's library as 'gEntry' */
typedef struct {
	/* FIXME: these are stubs for now */
	i32 (*create)(pEngine* engine);
	i32 (*destroy)(void);
} pRendererEntry;

typedef struct {
	pRendererEntry* entry;
	void* libraryHandle;
} pRendererState;

#ifndef PRENDERER
extern pRendererState gRenderer;
#else
extern pRendererEntry gEntry;
#endif

i32 pLoadRenderer(const char* path);
void pUnloadRenderer(void);
