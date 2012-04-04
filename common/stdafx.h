// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

// Windows Header Files
#include <windows.h>

// System Library Header Files
#include <ace/OS.h>
#include <ace/ACE.h>
#include <ace/Log_Msg.h>
#include <ace/Assert.h>
#include <ace/Synch.h>
#include <ace/Singleton.h>
#include <ace/CDR_Stream.h>

// C RunTime Header Files
#include <string>
#include <map>
#include <vector>
#include <set>
#include <deque>
#include <locale>
#include <sstream>
#include <stack>
#include <functional>

// Local Library Header Files
#include <xsd/cxx/pre.hxx>
#include <xsd/cxx/config.hxx>

// Local Header Files
#include <rpg_dice_incl.h>
#include "rpg_common_macros.h"
#include "rpg_common_incl.h"
#include "rpg_common_environment_incl.h"
