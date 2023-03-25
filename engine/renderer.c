/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#include "logger.h"
#include "renderer.h"
#include <pch/pch.h>

#ifdef pVULKAN
# include <vulkan_ren/core.h>
#endif

typedef struct {
	bool (*pfn)(void);
	i8 type;
} SupportTable;

static const SupportTable sTypePriority[pRENDERER_MAX_TYPES] = {
#ifdef pVULKAN
	{ pVulkanSupport, pRENDERER_TYPE_VULKAN },
#endif
};

static i8 sPickType(void)
{
	for (u32 i = 0; i < pRENDERER_MAX_TYPES; ++i) {
		if (sTypePriority[i].pfn()) {
			return sTypePriority[i].type;
		}
	}
	return pRENDERER_TYPE_INVALID;
}

i32 pRendererPopulate(pRenderer* self)
{
	i8 type = sPickType();
	if (type == pRENDERER_TYPE_INVALID) {
		pLoggerError("Host doesn't support any renderer\n");
		return -1;
	}

	switch (type) {
#ifdef pVULKAN
	case pRENDERER_TYPE_VULKAN:
		self->create     = rVkCreate;
		self->beginFrame = rVkBeginFrame;
		self->endFrame   = rVkEndFrame;
		self->destroy    = rVkDestroy;
		break;
#endif
	default:
		(void) self;
		assert(false && "unreachable");
	}
	return 0;
}
