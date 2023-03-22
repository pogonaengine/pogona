/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#include <engine/logger.h>
#include <engine/renderer.h>
#include <pch/vulkan.h>

pEngine* gEngine = NULL;

i32 rCreate(pEngine* engine)
{
	gEngine = engine;

	pLoggerInfo("rCreate()\n");
	return 0;
}

i32 rDestroy(void)
{
	pLoggerInfo("rDestroy()\n");
	return 0;
}

pRendererEntry gEntry = {
	.create  = rCreate,
	.destroy = rDestroy,
};
