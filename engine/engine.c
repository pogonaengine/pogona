/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#include "engine.h"
#include "logger.h"
#include <config.h>
#include <dirent.h>
#include <errno.h>
#include <pch/pch.h>

#define FILE_NAME_SIZE 255

#if defined(POS_UNIX) && !defined(POS_DARWIN)
# define RENDERER_ENDING "_ren.so"
#elif defined(POS_DARWIN)
# define RENDERER_ENDING "_ren.dylib"
#elif defined(POS_WINDOWS)
# define RENDERER_ENDING "_ren.dll"
#endif

/* TODO: MOVE OUT */
static bool sStringEndsWith(const char* string, const char* ending)
{
	if (!string || !ending)
		return false;
	u64 stringLength = strlen(string);
	u64 endingLength = strlen(ending);
	if (stringLength < endingLength)
		return false;
	return strncmp(string + stringLength - endingLength, ending, endingLength) == 0;
}

static i32 sFindRenderers(const char* path, u8 size, char rendererPaths[size][FILE_NAME_SIZE])
{
	DIR* dir = NULL;
	struct dirent* entry = NULL;

	errno = 0;
	dir = opendir(path);
	if (!dir) {
		pLoggerError("Could not find any renderers in '%s': %s\n", path, strerror(errno));
		return -1;
	}

	u8 i = 0;
	while (true) {
		if (i >= size) {
			pLoggerWarning("Renderer count exceeds the supported amount (%u)\n", size);
			break;
		}

		errno = 0;
		entry = readdir(dir);
		if (!entry && errno != 0) { /* an error */
			pLoggerError("Could not iterate '%s': %s", path, strerror(errno));
			closedir(dir);
			return -2;
		}
		if (!entry) { /* EOF */
			closedir(dir);
			break;
		}

		if (sStringEndsWith(entry->d_name, RENDERER_ENDING)) {
			strncpy(rendererPaths[i], entry->d_name, FILE_NAME_SIZE);
			i++;
		}
	}
	return i;
}

extern PAPI i32 pEngineEntry(int argc, char** argv)
{
	/* TODO: command line option to specify path to the renderer */
	(void) argc;
	(void) argv;

	i32 error = 0;

	/* 8 renderers is more than enough for the future */
	char rendererPaths[8][FILE_NAME_SIZE] = { 0 };
	i32 renderersCount = sFindRenderers(".", sizeof(*rendererPaths), rendererPaths);
	if (renderersCount < 0) {
		pLoggerError("Couldn't find any renderers\n");
		error = renderersCount;
		goto exit;
	}

	pLoggerInfo("Renderers found: %d\n", renderersCount);
	for (u8 i = 0; i < renderersCount; ++i) {
		pLoggerInfo("%s\n", rendererPaths[i]);
	}
exit:
	return error;
}
