/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

/* A renderer is a shared library, loaded by the engine. In fact, the engine is a shared library too.
 * A renderer should have these symbols exported from its library:
 * - (pRendererEntry) eEntry
 * - (char*) eName (optional)
 * - (u32) eVersion (optional)
 */

#pragma once

#include "defines.h"
#include "mesh.h"
#include <config.h>

#define pRENDERER_NAME_UNKNOWN    "UNKNOWN"
#define pRENDERER_VERSION_UNKNOWN UINT32_MAX

typedef struct pWindow pWindow;

typedef struct {
	i32 (*create)(pWindow* window);
	i32 (*beginFrame)(void);
	i32 (*drawMesh)(const rMesh* mesh);
	i32 (*endFrame)(void);
	void (*destroy)(void);
} pRendererEntry;

typedef struct {
	void* handle;
	pRendererEntry* entry;

	char* name;
	u32 version; /* TODO: describe format */
} pRenderer;

i32 pRendererLoad(pRenderer* self, const char* path);
void pRendererUnload(pRenderer* self);
