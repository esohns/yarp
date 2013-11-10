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

#define RPG_COMMON_MAX_SPELL_LEVEL             9
#define RPG_COMMON_MAX_CLASS_LEVEL             20

// general stuff
#define RPG_COMMON_MAX_TIMESTAMP_STRING_LENGTH 64
#define RPG_COMMON_DEF_TASK_GROUP_ID           0
// *NOTE*: use ACE_DEFAULT_TIMERS instead ?
#define RPG_COMMON_MAX_TIMER_SLOTS             1024
// *IMPORTANT NOTE*: used for libc calls (i.e. char buffers, mostly)
#define RPG_COMMON_BUFSIZE                     1024

// XML-specific
// *NOTE*: refer to the XSD C++/Tree manual for details
#define RPG_COMMON_XML_SCHEMA_CHARSET          "UTF-8"
#define RPG_COMMON_XML_TARGET_NAMESPACE        "urn:rpg"
#define RPG_COMMON_XML_SCHEMA_FILE_EXT         ".xsd"

// BASEDIR-specific
#define RPG_COMMON_DEF_DATA_SUB                "data"
#define RPG_COMMON_DEF_CONFIG_SUB              "config"

#if !defined(ACE_WIN32) && !defined(ACE_WIN64)
#define RPG_COMMON_DUMP_DIR                    "/var/tmp"
#else
#define RPG_COMMON_DUMP_DIR                    "TEMP" // environment
#endif

#if !defined(ACE_WIN32) && !defined(ACE_WIN64)
#define RPG_COMMON_DEF_USER_LOGIN_BASE        "LOGNAME" // environment
#else
#define RPG_COMMON_DEF_USER_LOGIN_BASE        "USERNAME" // environment
#endif

#endif
