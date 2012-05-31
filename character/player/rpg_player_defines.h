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

#ifndef RPG_PLAYER_DEFINES_H
#define RPG_PLAYER_DEFINES_H

#define RPG_PLAYER_START_MONEY           0
#define RPG_PLAYER_ATTR_MIN_SUM          80

// XML-specific
#define RPG_PLAYER_SCHEMA_FILE           "rpg_player.xsd"
#define RPG_PLAYER_PROFILE_EXT           ".player"

#define RPG_PLAYER_DEF_ENTITY            "default_player"

#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
#define RPG_PLAYER_DEF_ENTITY_REPOSITORY "/var/tmp"
#else
#define RPG_PLAYER_DEF_ENTITY_REPOSITORY "TEMP" // environment
#endif

#endif
