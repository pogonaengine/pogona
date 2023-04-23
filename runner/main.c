/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#include <../config.h>

/* FIXME: this check should be here
#if !defined(pOS_UNIX)
# error go port me!
#endif
*/

#if defined(pOS_UNIX) && !defined(pOS_DARWIN)
# define LIBRARY_NAME "libpogona.so"
#elif defined(pOS_DARWIN)
# define LIBRARY_NAME "libpogona.dylib"
#elif defined(pOS_WINDOWS)
# define LIBRARY_NAME "libpogona.dll"
#endif

#ifdef pOS_UNIX
# include <dlfcn.h>
#endif
#ifdef pOS_WINDOWS
# include <windows.h>
#endif
#include <engine/defines.h>
#include <engine/engine.h>
#include <pch/pch.h>

#if defined(pOS_UNIX)
# define dlopen  dlopen
# define dlsym   dlsym
# define dlerror dlerror
# define dlclose dlclose
#elif defined(pOS_WINDOWS)
# define dlopen(x, y) LoadLibrary(x)
# define dlsym(x, y)  GetProcAddress(x, y)
# define dlclose(x)   FreeLibrary(x)
#endif

int main(int argc, char** argv)
{
	i32 result = 0;
	void* engineHandle = NULL;
	PFN_pEngineEntry pEngineEntry = NULL;

	engineHandle = dlopen(LIBRARY_NAME, RTLD_NOW);
	if (!engineHandle) {
#ifdef pOS_UNIX
		fprintf(stderr, "%s\n", dlerror());
#endif
    result = 1;
    goto exit;
	}

	pEngineEntry = (PFN_pEngineEntry) dlsym(engineHandle, "pEngineEntry");
	if (!pEngineEntry) {
#ifdef pOS_UNIX
		fprintf(stderr, "%s\n", dlerror());
#endif
		result = 1;
		goto exit;
	}

	result = pEngineEntry(argc, argv);
exit:
	dlclose(engineHandle);
	return result;
}
