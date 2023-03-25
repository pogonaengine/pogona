/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#pragma once

#include "defines.h"

typedef struct pVector pVector;

#define pVector(x)																																																		 \
	struct {																																																						 \
		x*  data;																																																					 \
		u64 size;																																																					 \
		u64 typeSize;																																																			 \
		u64 capacity;																																																			 \
	}

PAPI i32 _pVectorCreate(pVector* self, u64 typeSize);
PAPI i32 _pVectorPush(pVector* self, void* data);
PAPI i32 _pVectorPop(pVector* self);
PAPI i32 _pVectorResize(pVector* self, u64 size);
PAPI i32 _pVectorShrinkToFit(pVector* self);
PAPI void _pVectorDestroy(pVector* self);

#define pVectorCreate(self) (_pVectorCreate((pVector*) self, sizeof(**self.data)))
#define pVectorPushPtr(self, data) (_pVectorPush((pVector*) self, (void*) data))
#define pVectorPop(self) (_pVectorPop((pVector*) self))
#define pVectorResize(self, size) (_pVectorResize((pVector*) self, size))
#define pVectorShrinkToFit(self) (_pVectorShrinkToFit((pVector*) self))
#define pVectorDestroy(self) (_pVectorDestroy((pVector*) self))

#define pVectorPush(self, data)																																												 \
	{																																																										 \
		__auto_type temp = (data);																																												 \
		pVectorPushPtr((pVector*) self, (void*) &temp);																																		 \
	}
