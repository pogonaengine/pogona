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
#include <pch.h>
#include <pogona/types.h>

typedef int32_t (*EngineInitFunc)(int argc, char** argv);
typedef void    (*EngineShutdownFunc)();

int main(int argc, char** argv)
{
	int32_t result = 0;
	void* engineHandle = NULL;
	EngineInitFunc engineInitFunc = NULL;
	EngineShutdownFunc engineShutdownFunc = NULL;

	engineHandle = dlopen(LIBRARY_NAME, RTLD_NOW);
	if (!engineHandle) {
		fprintf(stderr, "%s\n", dlerror());
		return 1;
	}

	engineInitFunc = dlsym(engineHandle, "pEngineInit");
	if (!engineInitFunc) {	
		fprintf(stderr, "%s\n", dlerror());
		result = 1;
		goto exit;
	}

	engineShutdownFunc = dlsym(engineHandle, "pEngineShutdown");
	if (!engineShutdownFunc) {	
		fprintf(stderr, "%s\n", dlerror());
		result = 1;
		goto exit;
	}

	result = engineInitFunc(argc, argv);

exit:
	engineShutdownFunc();
	dlclose(engineHandle);
	return result;
}
