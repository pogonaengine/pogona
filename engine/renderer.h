/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#pragma once

#include "defines.h"
#include "window/window.h"
#include <config.h>

enum {
	pRENDERER_TYPE_INVALID = -1,
#ifdef pVULKAN
	pRENDERER_TYPE_VULKAN,
#endif

	pRENDERER_MAX_TYPES,
};

typedef struct {
	i8 type;

	i32 (*create)(pWindow* window);
	i32 (*beginFrame)(void);
	i32 (*endFrame)(void);
	void (*destroy)(void);
} pRenderer;

i32 pRendererPopulate(pRenderer* self);
