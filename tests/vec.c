/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#include <assert.h>
#include <engine/math/vec.h>

int main(void)
{
	{
		pVec2 a = { .x = 10.f, .y = 5.f };
		pVec2 b = { .x = 5.f,  .y = 10.f };
		pVec2 c;

		c = (pVec2) {};
		c = pVec2Add(a, b);
		assert(c.x == 15.f);
		assert(c.y == 15.f);

		c = (pVec2) {};
		c = pVec2Sub(a, b);
		assert(c.x == 5.f);
		assert(c.y == -5.f);

		c = (pVec2) {};
		c = pVec2Mul(a, b);
		assert(c.x == 50.f);
		assert(c.y == 50.f);

		c = (pVec2) {};
		c = pVec2Div(a, b);
		assert(c.x == 2.f);
		assert(c.y == 0.5f);
	}

	{
		pVec3 a = { .x = 10.f, .y = 5.f,  .z = 20.f };
		pVec3 b = { .x = 5.f,  .y = 10.f, .z = 4.f };
		pVec3 c;

		c = (pVec3) {};
		c = pVec3Add(a, b);
		assert(c.x == 15.f);
		assert(c.y == 15.f);
		assert(c.z == 24.f);

		c = (pVec3) {};
		c = pVec3Sub(a, b);
		assert(c.x == 5.f);
		assert(c.y == -5.f);
		assert(c.z == 16.f);

		c = (pVec3) {};
		c = pVec3Mul(a, b);
		assert(c.x == 50.f);
		assert(c.y == 50.f);
		assert(c.z == 80.f);

		c = (pVec3) {};
		c = pVec3Div(a, b);
		assert(c.x == 2.f);
		assert(c.y == 0.5f);
		assert(c.z == 5.f);
	}

	{
		pVec4 a = { .x = 10.f, .y = 5.f,  .z = 20.f, .w = 16.f };
		pVec4 b = { .x = 5.f,  .y = 10.f, .z = 4.f,  .w = 4.f };
		pVec4 c;

		c = (pVec4) {};
		c = pVec4Add(a, b);
		assert(c.x == 15.f);
		assert(c.y == 15.f);
		assert(c.z == 24.f);
		assert(c.w == 20.f);

		c = (pVec4) {};
		c = pVec4Sub(a, b);
		assert(c.x == 5.f);
		assert(c.y == -5.f);
		assert(c.z == 16.f);
		assert(c.w == 12.f);

		c = (pVec4) {};
		c = pVec4Mul(a, b);
		assert(c.x == 50.f);
		assert(c.y == 50.f);
		assert(c.z == 80.f);
		assert(c.w == 64.f);

		c = (pVec4) {};
		c = pVec4Div(a, b);
		assert(c.x == 2.f);
		assert(c.y == 0.5f);
		assert(c.z == 5.f);
		assert(c.w == 4.f);
	}
	return 0;
}
