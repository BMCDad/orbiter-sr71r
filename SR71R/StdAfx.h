// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once


#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#define STRICT
#define ORBITER_MODULE


// Windows Header Files:
#include <windows.h>

// Including the Orbiter header files here greatly improves compilation speed.
#include "OrbiterAPI.h"
#include "Orbitersdk.h"


// TODO: reference additional headers your program requires here