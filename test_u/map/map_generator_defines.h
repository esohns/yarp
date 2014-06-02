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

#ifndef MAP_GENERATOR_DEFINES_H
#define MAP_GENERATOR_DEFINES_H

// configuration defaults
#define MAP_GENERATOR_DEF_MIN_ROOMSIZE          0
#define MAP_GENERATOR_DEF_DOORS                 true
#define MAP_GENERATOR_DEF_CORRIDORS             true
#define MAP_GENERATOR_DEF_LEVEL                 false
#define MAP_GENERATOR_DEF_MAX_NUMDOORS_PER_ROOM 3
#define MAP_GENERATOR_DEF_MAXIMIZE_ROOMSIZE     true
#define MAP_GENERATOR_DEF_DUMP                  false
#define MAP_GENERATOR_DEF_NUM_AREAS             5
#define MAP_GENERATOR_DEF_SQUARE_ROOMS          true
#define MAP_GENERATOR_DEF_DIMENSION_X           80
#define MAP_GENERATOR_DEF_DIMENSION_Y           40

// sane configuration limits
#define MAP_GENERATOR_DEF_SIZE_MIN              40
#define MAP_GENERATOR_DEF_SIZE_MAX              512
#define MAP_GENERATOR_DEF_ROOM_SIZE_MAX         30
#define MAP_GENERATOR_DEF_NUM_AREAS_MAX         50

#define MAP_GENERATOR_GUI_GNOME_UI_FILE         "map_generator_gui.glade"
#define MAP_GENERATOR_GUI_GNOME_APPLICATION_ID  "map generator"

#endif
