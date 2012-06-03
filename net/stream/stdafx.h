// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//
#if defined _MSC_VER
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

// Windows Header Files
#include <windows.h>
#endif

// System Library Header Files
#include <ace/OS.h>
#include <ace/ACE.h>
#include <ace/Synch.h>
#include <ace/Condition_T.h>
#include <ace/Lock_Adapter_T.h>
#include <ace/Log_Msg.h>
#include <ace/Stream.h>
#include <ace/Module.h>
#include <ace/Singleton.h>
#include <ace/Reactor.h>
#include <ace/Malloc_Allocator.h>

// C RunTime Header Files
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <functional>
#include <iterator>
#include <algorithm>

// Local Header Files
#include <rpg_common_macros.h>
