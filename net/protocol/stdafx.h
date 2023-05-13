// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//
#if defined _MSC_VER
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

// *NOTE*: (f)lex defines its own integer types...
#define _STDINT

// Windows Header Files
#include <windows.h>
#endif

// System Library Header Files
#include <ace/OS.h>
#include <ace/ACE.h>
#include <ace/Log_Msg.h>
#include <ace/Assert.h>
//#include <ace/Synch.h>
#include <ace/Task.h>
#include <ace/Reactor.h>
#include <ace/Svc_Handler.h>
#include <ace/SOCK_Stream.h>
#include <ace/Stream.h>

// C RunTime Header Files
#include <string>
#include <map>
#include <vector>
#include <set>
#include <deque>
#include <list>
#include <sstream>
#include <functional>
#include <iterator>

// Local Header Files
#include "rpg_common_macros.h"
