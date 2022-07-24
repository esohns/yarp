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

#define RPG_PLAYER_START_MONEY              0 // GP
#define RPG_PLAYER_ATTRIBUTE_LEVELUP_POINTS 2
#define RPG_PLAYER_ATTRIBUTE_MAXIMUM        20
#define RPG_PLAYER_ATTRIBUTE_MINIMUM_SUM    80
#define RPG_PLAYER_MULTIRACE_PROBABILITY    0.3F // 30%
#define RPG_PLAYER_MULTICLASS_PROBABILITY   0.2F // 20%

// XML-specific
#define RPG_PLAYER_SCHEMA_FILE              "rpg_player.xsd"
#define RPG_PLAYER_PROFILE_EXT              ".profile"

#define RPG_PLAYER_DEF_NAME                 "default player"

#define RPG_PLAYER_PROFILES_SUB             "profiles"

#endif
