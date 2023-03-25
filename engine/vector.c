/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#include "vector.h"
#include <pch/pch.h>

#define START_CAPACITY 16
#define RESIZE_BY      8

typedef struct pVector {
	void* data;
	u64   size;
	u64   typeSize;
	u64   capacity;
} pVector;

i32 _pVectorCreate(pVector* self, u64 typeSize)
{
	self->size     = 0;
	self->typeSize = typeSize;
	self->capacity = START_CAPACITY;
	self->data     = calloc(START_CAPACITY, typeSize);
	return self->data != NULL; /* -ENOMEM then */
}

i32 _pVectorPush(pVector* self, void* data)
{
	i32 error = 0;

	if (self->size == self->capacity) {
		error = pVectorResize(self, self->capacity + RESIZE_BY);
		if (error < 0)
			goto exit;
	}

	memcpy(self->data + (self->size * self->typeSize), data, self->typeSize);
	++self->size;

exit:
	return error;
}

i32 _pVectorPop(pVector* self)
{
	i32 error = 0;

	/* nothing to pop */
	if (self->size < 1) {
		error = -1;
		goto exit;
	}

	memset(self->data + ((self->size - 1) * self->typeSize), 0, self->typeSize);
	--self->size;

exit:
	return error;
}

i32 _pVectorResize(pVector* self, u64 size)
{
	/* don't reallocate anything if the size is the same */
	if (size == self->size)
		return 0;

	void* ptr = realloc(self->data, size * self->typeSize);
	if (!ptr)
		return -1;

	if (size < self->size)
		self->size = size;
	self->capacity = size; /* HMM: maybe preallocate some space (RESIZE_BY)? */
	self->data     = ptr;
	return 0;
}

i32 _pVectorShrinkToFit(pVector* self)
{
	if (self->size == self->capacity)
		return 0;

	void* ptr = realloc(self->data, self->size * self->typeSize);
	if (!ptr)
		return -1;

	self->capacity = self->size; /* HMM: maybe preallocate some space (RESIZE_BY)? */
	self->data     = ptr;
	return 0;
}

void _pVectorDestroy(pVector* self)
{
	free(self->data);
}
