/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#include <assert.h>
#include <pogona/vector.h>
#include <stdlib.h>
#include <time.h>

/* TODO: test return codes of pVector... */

int main(void)
{
	srand(time(NULL));

	pVector(u64) vector;
	pVectorCreate(&vector);

	assert(vector.typeSize == sizeof(u64));
	assert(vector.size == 0);
	/* capacity may differ */

	/* generate 8 random values, and put them into the vector */
	u64 buffer[8] = { 0 }; /* needed to compare the actual values later */
	for (u64 i = 0; i < 8; ++i) {
		buffer[i] = rand();
		pVectorPush(&vector, buffer[i]);
	}

	/* compare the values via vector.data */
	for (u64 i = 0; i < 8; ++i)
		assert(vector.data[i] == buffer[i]);

	/* pop 4 values */
	u64 previousCapacity = vector.capacity;
	for (u64 i = 0; i < 4; ++i)
		pVectorPop(&vector);
	assert(vector.size == 4);
	assert(vector.capacity == previousCapacity);
	for (u64 i = 0; i < 4; ++i)
		assert(vector.data[4 + i] == 0); /* these elements were memset with 0 */

	/* compare the first 4 values that were left. they should be the same */
	for (u64 i = 0; i < 4; ++i)
		assert(vector.data[i] == buffer[i]);

	/* resize to 12 elements (for example) */
	u64 previousSize = vector.size;
	pVectorResize(&vector, 12);
	assert(vector.size == previousSize);
	assert(vector.capacity == 12);

	/* make sure they are 0 initialized */
	for (u64 i = 4; i < 12; ++i)
		assert(vector.data[i] == 0);

	/* shrink the vector via resize */
	pVectorResize(&vector, 2);
	assert(vector.size == 2);
	assert(vector.capacity == 2);

	/* make sure the first 2 elements that are left are the same as they were */
	for (u64 i = 0; i < 2; ++i)
		assert(vector.data[i] == buffer[i]);

	/* preallocate memory and then shrink to fit */
	pVectorResize(&vector, 12);
	pVectorShrinkToFit(&vector);
	assert(vector.size == 2);
	assert(vector.capacity == 2);

	pVectorDestroy(&vector);
	return 0;
}
