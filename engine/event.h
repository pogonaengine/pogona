/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023, Nikita Romanyuk
 */

#pragma once

#include "defines.h"

/* UINT16_MAX should be enough for everyone. At least I hope so.
 * The first 8 bits are reserved for the engine.
 * So you can use codes from (2^8)..(2^16 - 1), or 256..65535.
 */
typedef u16 pEvent;

typedef void* pEventData;

typedef bool (*PFN_onEvent)(pEventData data);

i32 pEventSystemCreate(void);
i32 pEventRegister(pEvent event, PFN_onEvent pfn);
bool pEventSend(pEvent event, pEventData data);
void pEventSystemDestroy(void);

enum {
	pEVENT_FRAME,
	pEVENT_RESIZED,
	pEVENT_RESERVED_MAX = 0x03FF,
};
