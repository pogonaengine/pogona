/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#include "core.h"
#include "shader.h"
#include "vulkan/vulkan_core.h"
#include <pch/pch.h>

i32 rVkReadShader(rVkShader* self, const char* path)
{
	FILE* file = fopen(path, "rb");
	if (!file) {
		pLoggerError("could not open shader: %s\n", path);
		return -1;
	}

	fseek(file, 0, SEEK_END);
	self->size = ftell(file);
	fseek(file, 0, SEEK_SET);

	self->data = malloc(self->size);
	fread(self->data, self->size, 1, file);
	if (ferror(file) != 0 && feof(file) != 0) {
		pLoggerError("could not read shader: %s\n", path);
		fclose(file);
		return -1;
	}

	fclose(file);
	return 0;
}

void rVkFreeShader(rVkShader* self)
{
	self->size = 0;
	free(self->data);
}

i32 rVkCreateShaderModule(VkShaderModule* self, rVkShader* shader)
{
	if (shader->size % sizeof(u32) != 0) {
		pLoggerError("invalid shader data\n");
		return -1;
	}

	VkShaderModuleCreateInfo shaderModuleCreateInfo = {
		.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.pCode    = (const u32*) shader->data,
		.codeSize = shader->size,
	};
	rCHECK(vkCreateShaderModule(gCore.device, &shaderModuleCreateInfo, NULL, self));
	return 0;
}
