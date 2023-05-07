/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#include "core.h"
#include <engine/renderer.h>

pRendererEntry eEntry = {
	.create = rVkCreate,
	.beginFrame = rVkBeginFrame,
	.endFrame = rVkEndFrame,
	.destroy = rVkDestroy,
};
char* eName    = "Vulkan";
u32   eVersion = VK_API_VERSION_1_3;
