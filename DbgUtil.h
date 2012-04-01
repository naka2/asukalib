
#pragma once

#include "AsukaLib.h"

VOID DebugWrite(LPSTR format, ...);

#ifdef _DEBUG
#define DEBUG_OUT DebugWrite
#else
#define DEBUG_OUT (void)
#endif

