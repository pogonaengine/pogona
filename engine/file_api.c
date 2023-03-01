/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#include "pch/pch.h"
#include <dirent.h>
#include <errno.h>
#include <pogona/logger.h>

bool pIsFilePresentAt(const char* path, const char* name)
{
	DIR* dir = NULL;
	struct dirent* entry = NULL;
	bool exists = false;

	errno = 0;
	dir = opendir(path);
	if (!dir) {
		pLoggerWarning("could not open dir `%`: %s\n", path, strerror(errno));
		goto exit;
	}

	while (true) {
		errno = 0;
		entry = readdir(dir);
		if (!entry && errno != 0) /* error */ {
			pLoggerWarning("could not read dir `%s`: %s", path, strerror(errno));
			break;
		}
		if (!entry) { /* EOF */
			break;
		}

		if (!strcmp(name, entry->d_name)) {
			exists = true;
			goto exit;
		}
	}

exit:
	closedir(dir);
	return exists;
}
