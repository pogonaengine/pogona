/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#include <../config.h>

#ifndef pOS_UNIX
# error go port me!
#endif

#if defined(pOS_UNIX) && !defined(pOS_DARWIN)
# define LIBRARY_NAME "libpogona.so"
#elif defined(pOS_DARWIN)
# define LIBRARY_NAME "libpogona.dylib"
#elif defined(pOS_WINDOWS)
# define LIBRARY_NAME "libpogona.dll"
#endif

#include <dlfcn.h>
#include <engine/defines.h>
#include <engine/engine.h>
#include <pch/pch.h>

int main(int argc, char** argv)
{
	i32 result = 0;
	void* engineHandle = NULL;
	PFN_pEngineEntry pEngineEntry = NULL;

	engineHandle = dlopen(LIBRARY_NAME, RTLD_NOW);
	if (!engineHandle) {
		fprintf(stderr, "%s\n", dlerror());
		return 1;
	}

	pEngineEntry = (PFN_pEngineEntry) dlsym(engineHandle, "pEngineEntry");
	if (!pEngineEntry) {
		fprintf(stderr, "%s\n", dlerror());
		result = 1;
		goto exit;
	}

	result = pEngineEntry(argc, argv);
exit:
	dlclose(engineHandle);
	return result;
}
