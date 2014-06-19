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

// System Library Header Files
#include "glade/glade.h"
#include "gtk/gtk.h"
#include "SDL.h"
#include "png.h"
#include "ace/OS.h"
#include "ace/Log_Msg.h"

// C RunTime Header Files
#include <string>
#include <iostream>

// Local Header Files
#include "rpg_common_macros.h"
