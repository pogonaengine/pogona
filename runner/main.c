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
#include <pogona/defines.h>

typedef int32_t (*EngineEntryFunc)(int argc, char** argv);

int main(int argc, char** argv)
{
	int32_t result = 0;
	void* engineHandle = NULL;
	EngineEntryFunc engineEntryFunc = NULL;

	engineHandle = dlopen(LIBRARY_NAME, RTLD_NOW);
	if (!engineHandle) {
		fprintf(stderr, "%s\n", dlerror());
		return 1;
	}

	engineEntryFunc = dlsym(engineHandle, "pEngineEntry");
	if (!engineEntryFunc) {
		fprintf(stderr, "%s\n", dlerror());
		result = 1;
		goto exit;
	}

	result = engineEntryFunc(argc, argv);
exit:
	dlclose(engineHandle);
	return result;
}
