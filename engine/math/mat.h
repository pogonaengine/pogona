/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#pragma once

#include "../defines.h"

typedef union {
	f32 data[16];
} pMat4x4;

pINLINE pMat4x4 pMat4x4Identity(void)
{
	return (pMat4x4) {
		.data = {
			1.f, 0.f, 0.f, 0.f,
			0.f, 1.f, 0.f, 0.f,
			0.f, 0.f, 1.f, 0.f,
			0.f, 0.f, 0.f, 1.f,
		},
	};
}

pINLINE pMat4x4 pMat4x4Mul(pMat4x4 x, pMat4x4 y)
{
	const f32* X = x.data;
	const f32* Y = y.data;
	return (pMat4x4) {
		.data = {
			X[0 ]*Y[0] + X[1 ]*Y[4] + X[2 ]*Y[8 ] + X[3 ]*Y[12],
			X[0 ]*Y[1] + X[1 ]*Y[5] + X[2 ]*Y[9 ] + X[3 ]*Y[13],
			X[0 ]*Y[2] + X[1 ]*Y[6] + X[2 ]*Y[10] + X[3 ]*Y[14],
			X[0 ]*Y[3] + X[1 ]*Y[7] + X[2 ]*Y[11] + X[3 ]*Y[15],

			X[4 ]*Y[0] + X[5 ]*Y[4] + X[6 ]*Y[8 ] + X[7 ]*Y[12],
			X[4 ]*Y[1] + X[5 ]*Y[5] + X[6 ]*Y[9 ] + X[7 ]*Y[13],
			X[4 ]*Y[2] + X[5 ]*Y[6] + X[6 ]*Y[10] + X[7 ]*Y[14],
			X[4 ]*Y[3] + X[5 ]*Y[7] + X[6 ]*Y[11] + X[7 ]*Y[15],

			X[8 ]*Y[0] + X[9 ]*Y[4] + X[10]*Y[8 ] + X[11]*Y[12],
			X[8 ]*Y[1] + X[9 ]*Y[5] + X[10]*Y[9 ] + X[11]*Y[13],
			X[8 ]*Y[2] + X[9 ]*Y[6] + X[10]*Y[10] + X[11]*Y[14],
			X[8 ]*Y[3] + X[9 ]*Y[7] + X[10]*Y[11] + X[11]*Y[15],

			X[12]*Y[0] + X[13]*Y[4] + X[14]*Y[8 ] + X[15]*Y[12],
			X[12]*Y[1] + X[13]*Y[5] + X[14]*Y[9 ] + X[15]*Y[13],
			X[12]*Y[2] + X[13]*Y[6] + X[14]*Y[10] + X[15]*Y[14],
			X[12]*Y[3] + X[13]*Y[7] + X[14]*Y[11] + X[15]*Y[15],
		},
	};
}
