/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#ifndef __linux__
# error go port me!
#endif

#include <dlfcn.h>
#include <pch.h>
#include <pogona/types.h>

typedef int32_t (*EngineInitFunc)(int argc, char** argv);
typedef void (*EngineShutdownFunc)();

int main(int argc, char** argv)
{
	int32_t result = 0;
	void* engineHandle = NULL;
	EngineInitFunc engineInitFunc = NULL;
	EngineShutdownFunc engineShutdownFunc = NULL;

	engineHandle = dlopen("libpogona.so", RTLD_NOW);
	if (!engineHandle) {
		fprintf(stderr, "could not open `libpogona.so`: %s\n", dlerror());
		return 1;
	}

	engineInitFunc = dlsym(engineHandle, "pEngineInit");
	if (!engineInitFunc) {	
		fprintf(stderr, "could not find `pEngineInit` in `libpogona.so`: %s\n", dlerror());
		result = 1;
		goto exit;
	}

	engineShutdownFunc = dlsym(engineHandle, "pEngineShutdown");
	if (!engineShutdownFunc) {	
		fprintf(stderr, "could not find `pEngineShutdown` in `libpogona.so`: %s\n", dlerror());
		result = 1;
		goto exit;
	}

	result = engineInitFunc(argc, argv);

exit:
	engineShutdownFunc();
	dlclose(engineHandle);
	return result;
}