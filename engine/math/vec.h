/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#pragma once

#include "../defines.h"
#include <math.h>

typedef union {
	f32 data[2];
	struct { f32 x, y; };
	struct { f32 r, g; };
	struct { f32 s, t; };
} pVec2;

typedef union {
	f32 data[3];
	struct { f32 x, y, z; };
	struct { f32 r, g, b; };
	struct { f32 s, t, p; };
} pVec3;

typedef union {
	f32 data[4];
	struct { f32 x, y, z, w; };
	struct { f32 r, g, b, a; };
	struct { f32 s, t, p, q; };
} pVec4;

/* S means scalar. Creates a vector, where both .x and .y = v */
pINLINE pVec2 pVec2S(f32 v)
{
	return (pVec2) { .x = v, .y = v };
}
pINLINE pVec3 pVec3S(f32 v)
{
	return (pVec3) { .x = v, .y = v, .z = v };
}
pINLINE pVec4 pVec4S(f32 v)
{
	return (pVec4) { .x = v, .y = v, .z = v, .w = v };
}

/* Addition */
pINLINE pVec2 pVec2Add(pVec2 x, pVec2 y)
{
	return (pVec2) {
		.x = x.x + y.x,
		.y = x.y + y.y,
	};
}
pINLINE pVec3 pVec3Add(pVec3 x, pVec3 y)
{
	return (pVec3) {
		.x = x.x + y.x,
		.y = x.y + y.y,
		.z = x.z + y.z,
	};
}
pINLINE pVec4 pVec4Add(pVec4 x, pVec4 y)
{
	return (pVec4) {
		.x = x.x + y.x,
		.y = x.y + y.y,
		.z = x.z + y.z,
		.w = x.w + y.w,
	};
}

/* Subtraction */
pINLINE pVec2 pVec2Sub(pVec2 x, pVec2 y)
{
	return (pVec2) {
		.x = x.x - y.x,
		.y = x.y - y.y,
	};
}
pINLINE pVec3 pVec3Sub(pVec3 x, pVec3 y)
{
	return (pVec3) {
		.x = x.x - y.x,
		.y = x.y - y.y,
		.z = x.z - y.z,
	};
}
pINLINE pVec4 pVec4Sub(pVec4 x, pVec4 y)
{
	return (pVec4) {
		.x = x.x - y.x,
		.y = x.y - y.y,
		.z = x.z - y.z,
		.w = x.w - y.w,
	};
}

/* Multiplication */
pINLINE pVec2 pVec2Mul(pVec2 x, pVec2 y)
{
	return (pVec2) {
		.x = x.x * y.x,
		.y = x.y * y.y,
	};
}
pINLINE pVec3 pVec3Mul(pVec3 x, pVec3 y)
{
	return (pVec3) {
		.x = x.x * y.x,
		.y = x.y * y.y,
		.z = x.z * y.z,
	};
}
pINLINE pVec4 pVec4Mul(pVec4 x, pVec4 y)
{
	return (pVec4) {
		.x = x.x * y.x,
		.y = x.y * y.y,
		.z = x.z * y.z,
		.w = x.w * y.w,
	};
}

/* Division */
pINLINE pVec2 pVec2Div(pVec2 x, pVec2 y)
{
	return (pVec2) {
		.x = x.x / y.x,
		.y = x.y / y.y,
	};
}
pINLINE pVec3 pVec3Div(pVec3 x, pVec3 y)
{
	return (pVec3) {
		.x = x.x / y.x,
		.y = x.y / y.y,
		.z = x.z / y.z,
	};
}
pINLINE pVec4 pVec4Div(pVec4 x, pVec4 y)
{
	return (pVec4) {
		.x = x.x / y.x,
		.y = x.y / y.y,
		.z = x.z / y.z,
		.w = x.w / y.w,
	};
}

/* Length^2 */
pINLINE f32 pVec2LengthSquared(pVec2 x)
{
	return x.x * x.x + x.y * x.y;
}

pINLINE f32 pVec3LengthSquared(pVec3 x)
{
	return x.x * x.x + x.y * x.y + x.z * x.z;
}

pINLINE f32 pVec4LengthSquared(pVec4 x)
{
	return x.x * x.x + x.y * x.y + x.z * x.z + x.w * x.w;
}

/* Length */
pINLINE f32 pVec2Length(pVec2 x)
{
	return sqrtf(pVec2LengthSquared(x));
}

pINLINE f32 pVec3Length(pVec3 x)
{
	return sqrtf(pVec3LengthSquared(x));
}

pINLINE f32 pVec4Length(pVec4 x)
{
	return sqrtf(pVec4LengthSquared(x));
}

/* Normalize */
pINLINE void pVec2Normalize(pVec2* x)
{
	const f32 length = pVec2Length(*x);
	x->x /= length;
	x->y /= length;
}

pINLINE void pVec3Normalize(pVec3* x)
{
	const f32 length = pVec3Length(*x);
	x->x /= length;
	x->y /= length;
	x->z /= length;
}

pINLINE void pVec4Normalize(pVec4* x)
{
	const f32 length = pVec4Length(*x);
	x->x /= length;
	x->y /= length;
	x->z /= length;
	x->w /= length;
}
