// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//
#if defined _MSC_VER
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

// *NOTE*: workaround quirky MSVC...
#define NOMINMAX

// Windows Header Files
#include <windows.h>
#endif

// C RunTime Header Files
#include <string>
#include <iostream>

// Library Header Files
#include "ace/Log_Msg.h"
#include "ace/OS.h"

// Local Header Files
//#include "rpg_common_macros.h"
