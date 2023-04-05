/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#include "core.h"
#include "shader.h"
#include "vulkan/vulkan_core.h"
#include <pch/pch.h>

i32 rVkReadShader(VkShaderModule* module, const char* path)
{
	FILE* file;
	uint64_t fileSize;
	uint8_t* data;
	
	file = fopen(path, "rb");
	if (!file) {
		pLoggerError("could not open shader: %s\n", path);
		return -1;
	}

	fseek(file, 0, SEEK_END);
	fileSize = ftell(file);
	fseek(file, 0, SEEK_SET);

	data = malloc(fileSize);
	fread(data, fileSize, 1, file);
	if (ferror(file) != 0 && feof(file) != 0) {
		pLoggerError("could not read shader: %s\n", path);
		fclose(file);
		return -1;
	}

	if (fileSize % sizeof(u32) != 0) {
		pLoggerError("invalid shader data\n");
		return -1;
	}

	VkShaderModuleCreateInfo shaderModuleCreateInfo = {
		.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.pCode    = (const u32*) data,
		.codeSize = fileSize,
	};
	rCHECK(vkCreateShaderModule(gCore.device, &shaderModuleCreateInfo, NULL, module));

	free(data);
	fclose(file);
	return 0;
}
