/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#include "core.h"
#include "defines.h"
#include <engine/logger.h>
#include <engine/renderer.h>
#include <pch/vulkan.h>

pEngine* gEngine = NULL;

i32 rCreate(pEngine* engine)
{
	i32 error = 0;
	gEngine = engine;

	RVK_CHECK(volkInitialize());

	error = rVkInstanceCreate();
	if (error < 0) {
		pLoggerError("Could not create Vulkan instance: %d\n", error);
		goto exit;
	}
	pLoggerInfo("Supported Vulkan version: %u.%u.%u\n",
			VK_API_VERSION_MAJOR(error),
			VK_API_VERSION_MINOR(error),
			VK_API_VERSION_PATCH(error));

	error = rVkPickPhysicalDevice();
	if (error < 0) {
		pLoggerError("Could not pick physical device or queue family\n");
		goto exit;
	}

exit:
	return error;
}

i32 rDestroy(void)
{
	rVkInstanceDestroy();
	return 0;
}

pRendererEntry gEntry = {
	.create  = rCreate,
	.destroy = rDestroy,
};
