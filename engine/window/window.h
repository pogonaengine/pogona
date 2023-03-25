/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#pragma once

#include "../defines.h"

#define pWINDOW_TITLE_SIZE 64

enum {
	pWINDOW_TYPE_INVALID = -1,
	pWINDOW_TYPE_WAYLAND = 0,

	pWINDOW_MAX_TYPES,
};

typedef struct {
	char title[pWINDOW_TITLE_SIZE];
	u32 width, height;
	i8 type;
	void* api; /* managed by the window itself */
} pWindow;

i32 pWindowCreate(pWindow* self, const char* title, u32 width, u32 height);
/* returns a pointer to an api-specific structure.
 * you should cast it, depending on the `self->type`.
 */
const void* pWindowGetApi(const pWindow* self);
void pWindowDestroy(pWindow* self);
