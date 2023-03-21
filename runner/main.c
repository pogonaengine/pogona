/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#include <../config.h>

#ifndef POS_UNIX
# error go port me!
#endif

#if defined(POS_UNIX) && !defined(POS_DARWIN)
# define LIBRARY_NAME "libpogona.so"
#elif defined(POS_DARWIN)
# define LIBRARY_NAME "libpogona.dylib"
#elif defined(POS_WINDOWS)
# define LIBRARY_NAME "libpogona.dll"
#endif

#include <dlfcn.h>
#include <engine/defines.h>
#include <engine/engine.h>
#include <pch/pch.h>

int main(int argc, char** argv)
{
	int32_t result = 0;
	void* engineHandle = NULL;
	PFN_pEngineEntry pEngineEntry = NULL;

	engineHandle = dlopen(LIBRARY_NAME, RTLD_NOW);
	if (!engineHandle) {
		fprintf(stderr, "%s\n", dlerror());
		return 1;
	}

	pEngineEntry = dlsym(engineHandle, "pEngineEntry");
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
