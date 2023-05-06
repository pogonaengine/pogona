/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#include <assert.h>
#include <engine/math/mat.h>

int main(void)
{
	pMat4x4 a = {
		.data = {
			5.f, 7.f, 9.f, 10.f,
			2.f, 3.f, 3.f, 8.f,
			8.f, 10.f, 2.f, 3.f,
			3.f, 3.f, 4.f, 8.f,
		},
	};
	pMat4x4 b = {
		.data = {
			3.f, 10.f, 12.f, 18.f,
			12.f, 1.f, 4.f, 9.f,
			9.f, 10.f, 12.f, 2.f,
			3.f, 12.f, 4.f, 10.f,
		},
	};
	pMat4x4 c = pMat4x4Mul(a, b);

	pMat4x4 d = {
		.data = {
			210.f, 267.f, 236.f, 271.f,
			93.f, 149.f, 104.f, 149.f,
			171.f, 146.f, 172.f, 268.f,
			105.f, 169.f, 128.f, 169.f,
		},
	};
	for (u32 i = 0; i < 4; ++i) {
		for (u32 j = 0; j < 4; ++j) {
			assert(c.data[i*4+j] == d.data[i*4+j]);
		}
	}
	return 0;
}
