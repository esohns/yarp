/***************************************************************************
 *   Copyright (C) 2009 by Erik Sohns   *
 *   erik.sohns@web.de   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef RPG_COMMON_DEFINES_H
#define RPG_COMMON_DEFINES_H

#include <ace/Default_Constants.h>
//#include <limits>

// *PORTABILITY*: pathnames are not portable --> (try to) use %TEMP% on Windows
#if !defined(ACE_WIN32) && !defined(ACE_WIN64)
#define RPG_COMMON_DEF_DUMP_DIR                   "/var/tmp"
#else
#define RPG_COMMON_DEF_DUMP_DIR                   "TEMP" // environment
#endif

// *** trace log ***
#define RPG_COMMON_DEF_LOG_DIRECTORY              RPG_COMMON_DEF_DUMP_DIR
#define RPG_COMMON_LOG_FILENAME_SUFFIX            ".log"
#define RPG_COMMON_LOG_VERBOSE                    false

#define RPG_COMMON_MAX_SPELL_LEVEL                9
#define RPG_COMMON_MAX_CLASS_LEVEL                20

// general stuff
// *WARNING*: make sure group IDs are consistent !!!
#define RPG_COMMON_TIMER_THREAD_GROUP_ID          100
#define RPG_COMMON_TIMER_THREAD_NAME              "RPG timer dispatch"
#define RPG_COMMON_EVENT_DISPATCH_THREAD_GROUP_ID 101
#define RPG_COMMON_EVENT_DISPATCH_THREAD_NAME     "RPG event dispatch"
// *IMPORTANT NOTE*: currently used for (initial) slot pre-allocation only;
// ultimately, the total number of available concurrent slots depends on the
// actual implementation --> check the code, don't rely on ACE_DEFAULT_TIMERS
//#define RPG_COMMON_DEF_NUM_TIMER_SLOTS            std::numeric_limits<long>::max()
#define RPG_COMMON_DEF_NUM_TIMER_SLOTS            ACE_DEFAULT_TIMERS
#define RPG_COMMON_PREALLOCATE_TIMER_SLOTS        true
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#define RPG_COMMON_USE_WFMO_REACTOR               true // ? ACE_WFMO_Reactor : ACE_TP_Reactor
#else
#define RPG_COMMON_USE_DEV_POLL_REACTOR           false // ? ACE_Dev_Poll_Reactor : ACE_TP_Reactor
#endif

// *IMPORTANT NOTE*: used for libc calls (i.e. char buffers, mostly)
#define RPG_COMMON_BUFSIZE                        512 // bytes

// XML-specific
// *NOTE*: refer to the XSD C++/Tree manual for details
#define RPG_COMMON_XML_SCHEMA_CHARSET             "UTF-8"
#define RPG_COMMON_XML_TARGET_NAMESPACE           "urn:rpg"
#define RPG_COMMON_XML_SCHEMA_FILE_EXT            ".xsd"

// BASEDIR-specific
#define RPG_COMMON_DATA_SUB                       "data"
#define RPG_COMMON_CONFIG_SUB                     "config"

#if !defined(ACE_WIN32) && !defined(ACE_WIN64)
#define RPG_COMMON_DEF_USER_LOGIN_BASE            "LOGNAME" // environment
#else
#define RPG_COMMON_DEF_USER_LOGIN_BASE            "USERNAME" // environment
#endif

#endif
