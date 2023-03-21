/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#include "logger.h"
#include "renderer.h"
#include <dlfcn.h>
#include <pch/pch.h>

pRendererState gRenderer = { 0 };

i32 pLoadRenderer(const char* path)
{
	i32 error = 0;

	void* handle = dlopen(path, RTLD_NOW);
	if (!handle) {
		pLoggerError("Could not open renderer '%s': %s\n", path, dlerror());
		error = -1;
		goto exit;
	}
	gRenderer.libraryHandle = handle;

	pRendererEntry* rendererEntry = (pRendererEntry*) dlsym(handle, "gEntry");
	if (!rendererEntry) {
		pLoggerError("Could not get renderer '%s' entry: %s", path, dlerror());
		error = -2;
		goto exit;
	}
	gRenderer.entry = rendererEntry;

exit:
	return error;
}

void pUnloadRenderer(void)
{
	memset(gRenderer.entry, 0, sizeof(gRenderer));
	if (gRenderer.libraryHandle)
		dlclose(gRenderer.libraryHandle);
}
