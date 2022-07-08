// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//
#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers

// *NOTE*: workaround quirky MSVC...
#define NOMINMAX

// Windows Header Files
#include "windows.h"
#endif // _MSC_VER

// C RunTime Header Files
#include <string>
#include <map>
#include <vector>

// System Library Header Files
#include "ace/Assert.h"
#include "ace/Log_Msg.h"

// Local Library Header Files

// Local Header Files
#include "rpg_common_macros.h"

#include "rpg_graphics_common.h"
