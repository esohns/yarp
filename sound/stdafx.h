// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//
#if defined _MSC_VER
#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers

#include "targetver.h"

// Windows Header Files
#include "windows.h"
#endif // _MSC_VER

// C RunTime Header Files
#include <string>

// Local Library Header Files
#define _SDL_main_h
#define SDL_main_h_
#include "SDL.h"
#include "SDL_mixer.h"

// System Library Header Files
#include "ace/OS.h"
#include "ace/ACE.h"
#include "ace/Log_Msg.h"
#include "ace/Assert.h"

#include "xsd/cxx/pre.hxx"
#include "xsd/cxx/config.hxx"
#include "xsd/cxx/xml/error-handler.hxx"

// Local Header Files
#include "rpg_common_macros.h"

#include "rpg_sound_common.h"
