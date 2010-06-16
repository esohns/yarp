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

#ifndef RPG_CLIENT_DEFINES_H
#define RPG_CLIENT_DEFINES_H

#define RPG_CLIENT_DEF_ACE_USES_TP                false
#define RPG_CLIENT_DEF_ACE_NUM_TP_THREADS         5

#define RPG_CLIENT_DEF_GNOME_APPLICATION_ID       ACE_TEXT_ALWAYS_CHAR("rpg_client")
#define RPG_CLIENT_DEF_GNOME_UI_FILE              ACE_TEXT("rpg_client.glade")

#define RPG_CLIENT_DEF_INI_FILE                   ACE_TEXT("rpg_client.ini")
#define RPG_CLIENT_CNF_CLIENT_SECTION_HEADER      ACE_TEXT("client")
#define RPG_CLIENT_CNF_CONNECTION_SECTION_HEADER  ACE_TEXT("connection")

#define RPG_CLIENT_DEF_GRAPHICS_DICTIONARY        ACE_TEXT("rpg_graphics.xml")
#define RPG_CLIENT_DEF_GRAPHICS_DIRECTORY         ACE_TEXT(".")
#define RPG_CLIENT_DEF_GRAPHICS_CACHESIZE         50
#define RPG_CLIENT_DEF_GRAPHICS_WINDOWSTYLE_TYPE  TYPE_INTERFACE

#define RPG_CLIENT_DEF_SOUND_DICTIONARY           ACE_TEXT("rpg_sound.xml")
#define RPG_CLIENT_DEF_SOUND_DIRECTORY            ACE_TEXT(".")
#define RPG_CLIENT_DEF_SOUND_CACHESIZE            50

#define RPG_CLIENT_DEF_AUDIO_FREQUENCY            44100
#define RPG_CLIENT_DEF_AUDIO_FORMAT               AUDIO_S16SYS
#define RPG_CLIENT_DEF_AUDIO_CHANNELS             2
#define RPG_CLIENT_DEF_AUDIO_SAMPLES              4096

#define RPG_CLIENT_DEF_VIDEO_W                    1024
#define RPG_CLIENT_DEF_VIDEO_H                    786
#define RPG_CLIENT_DEF_VIDEO_BPP                  32
#define RPG_CLIENT_DEF_VIDEO_FULLSCREEN           false
#define RPG_CLIENT_DEF_VIDEO_DOUBLEBUFFER         false

#define RPG_CLIENT_DEF_MAP_MIN_ROOM_AREA          0 // 0: don't care
#define RPG_CLIENT_DEF_MAP_CORRIDORS              true
#define RPG_CLIENT_DEF_MAP_MAX_NUM_DOORS_PER_AREA 3
#define RPG_CLIENT_DEF_MAP_MAXIMIZE_ROOMS         true
#define RPG_CLIENT_DEF_MAP_NUM_AREAS              5
#define RPG_CLIENT_DEF_MAP_SQUARE_ROOMS           true
#define RPG_CLIENT_DEF_MAP_SIZE_X                 80
#define RPG_CLIENT_DEF_MAP_SIZE_Y                 40

#define SDL_TIMEREVENT   SDL_USEREVENT
#define SDL_MOUSEMOVEOUT (SDL_USEREVENT+1)

#endif
