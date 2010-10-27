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

// general stuff
#define RPG_COMMON_MAX_TIMESTAMP_STRING_LENGTH 64

// XML-specific
// *NOTE*: refer to the XSD C++/Tree manual for details
#define RPG_COMMON_XML_SCHEMA_CHARSET          ACE_TEXT("UTF-8")
#define RPG_COMMON_XML_TARGET_NAMESPACE        ACE_TEXT("urn:rpg")

// DATADIR-specific
#define RPG_COMMON_DEF_DATA_SUB                ACE_TEXT("data")
#define RPG_COMMON_DEF_CONFIG_SUB              ACE_TEXT("config")

#define RPG_COMMON_DUMP_DIR                    ACE_TEXT("/var/tmp")

#endif
