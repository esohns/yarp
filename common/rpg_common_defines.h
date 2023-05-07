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

#include "ace/config-lite.h"

#define RPG_COMMON_MAX_SPELL_LEVEL      9
#define RPG_COMMON_MAX_CLASS_LEVEL      20

// XML-specific
// *NOTE*: refer to the XSD C++/Tree manual for details
#define RPG_COMMON_XML_SCHEMA_CHARSET   "UTF-8"
#define RPG_COMMON_XML_TARGET_NAMESPACE "urn:rpg"
#define RPG_COMMON_XML_SCHEMA_FILE_EXT  ".xsd"

#define RPG_COMMON_SUB_DIRECTORY_STRING "common"

#if defined (ACE_WIN32) && defined (ACE_WIN64)
#define RPG_COMMON_DEF_USER_LOGIN_BASE  "USERNAME" // environment
#else
#define RPG_COMMON_DEF_USER_LOGIN_BASE  "LOGNAME" // environment
#endif // ACE_WIN32 || ACE_WIN64

#endif // RPG_COMMON_DEFINES_H
