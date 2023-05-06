/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#pragma once

#include "../defines.h"
#include "../math.h"

typedef struct {
	pVec3 pos;
	pVec4 colour;
} rVertex;

typedef struct {
	rVertex* vertices;
	u32      verticesCount;
	u16*     indices;
	u32      indicesCount;
} rMesh;
