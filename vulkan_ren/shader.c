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
	char pathBuffer[512] = { 0 };
	const char* envShaderDir = getenv("POGONA_SHADER_DIR");
	if (envShaderDir)
		snprintf(pathBuffer, sizeof(pathBuffer), "%s/%s", envShaderDir, path);
	else
		snprintf(pathBuffer, sizeof(pathBuffer), "shaders/%s", path);

	FILE* file;
	uint64_t fileSize;
	uint8_t* data;

	file = fopen(pathBuffer, "rb");
	if (!file) {
		pLoggerError("Couldn't open shader: %s\n", pathBuffer);
		return -1;
	}

	fseek(file, 0, SEEK_END);
	fileSize = ftell(file);
	fseek(file, 0, SEEK_SET);

	data = malloc(fileSize);
	fread(data, fileSize, 1, file);
	if (ferror(file) != 0 && feof(file) != 0) {
		pLoggerError("Couldn't read shader: %s\n", path);
		fclose(file);
		return -1;
	}

	if (fileSize % sizeof(u32) != 0) {
		pLoggerError("Invalid shader data\n");
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
