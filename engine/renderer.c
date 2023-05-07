/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#include "logger.h"
#include "renderer.h"
#include <dlfcn.h>
#include <pch/pch.h>

typedef struct {
	bool (*pfn)(void);
	i8 type;
} SupportTable;

i32 pRendererLoad(pRenderer* self, const char* path)
{
	self->handle = dlopen(path, RTLD_NOW);
	if (!self->handle) {
		pLoggerError("Could not load renderer \"%s\": %s\n", path, dlerror());
		return -1;
	}

	self->entry = (pRendererEntry*) dlsym(self->handle, "eEntry");
	if (!self->entry) {
		pLoggerError("Could not get renderer entry: %s\n", dlerror());
		pRendererUnload(self);
		return -2;
	}

	self->name = *(char**) dlsym(self->handle, "eName");
	if (!self->name)
		self->name = pRENDERER_NAME_UNKNOWN;

	self->version = *(u32*) dlsym(self->handle, "eVersion");
	if (!self->version)
		self->version = pRENDERER_VERSION_UNKNOWN;

	return 0;
}

void pRendererUnload(pRenderer* self)
{
	if (self->handle) {
		dlclose(self->handle);
		self->handle = NULL;
	}
	self->entry   = NULL;
	self->name    = NULL;
	self->version = 0;
}
