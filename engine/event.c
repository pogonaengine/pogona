/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#include "event.h"
#include <engine/vector.h>
#include <pch/pch.h>

/* all registered events */
static pVector(PFN_onEvent)* sState = NULL;

i32 pEventSystemCreate(void)
{
	sState = calloc(UINT16_MAX, sizeof(*sState));
	assert(sState && "out of memory");
	return 0;
}

i32 pEventRegister(pEvent event, PFN_onEvent pfn)
{
	i32 error = 0;

	if (!sState[event].data) {
		error = pVectorCreate(&sState[event]);
		if (error < 0)
			goto exit;
	}

	pVectorPush(&sState[event], pfn);

exit:
	return error;
}

bool pEventSend(pEvent event, pEventData data)
{
	if (!sState[event].data)
		return false;

	for (u64 i = 0; i < sState[event].size; ++i) {
		if (sState[event].data[i](data))
			return true;
	}

	return false;
}

void pEventSystemDestroy(void)
{
	for (u32 i = 0; i < UINT16_MAX; ++i) {
		if (sState[i].data)
			pVectorDestroy(&sState[i]);
	}
	free(sState);
}
