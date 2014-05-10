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

#ifndef RPG_MAP_DEFINES_H
#define RPG_MAP_DEFINES_H

// min distance between any 2 doors of a single room
#define RPG_MAP_DOOR_SEPARATION    1

#define RPG_MAP_CIRCLE_MAX_RADIUS  15

// output more debugging information
#define RPG_MAP_DEF_TRACE_SCANNING false
#define RPG_MAP_DEF_TRACE_PARSING  false

#define RPG_MAP_SCANNER_BUFSIZE    16384 // == to YY_BUF_SIZE

#define RPG_MAP_FILE_EXT           ".txt"

#define RPG_MAP_MAP_SUB            "map"
// default (maps) data directory tree organization
#define RPG_MAP_MAPS_SUB           "maps"

#define RPG_MAP_DEF_MAP_FILE       "dungeon_one"

#define RPG_MAP_ROOM_SIZE_MAX      30
#define RPG_MAP_NUM_AREAS_MAX      30
#define RPG_MAP_SIZE_MIN           40
#define RPG_MAP_SIZE_MAX           512

#endif
