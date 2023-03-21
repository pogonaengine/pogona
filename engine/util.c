/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#include "util.h"
#include <pch/pch.h>

bool pStringEndsWith(const char* string, const char* ending)
{
	if (!string || !ending)
		return false;
	u64 stringLength = strlen(string);
	u64 endingLength = strlen(ending);
	if (stringLength < endingLength)
		return false;
	return strncmp(string + stringLength - endingLength, ending, endingLength) == 0;
}
