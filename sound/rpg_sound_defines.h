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

#ifndef RPG_SOUND_DEFINES_H
#define RPG_SOUND_DEFINES_H

#define RPG_SOUND_AUDIO_DEF_FREQUENCY       44100
#define RPG_SOUND_AUDIO_DEF_FORMAT          AUDIO_S16SYS
#define RPG_SOUND_AUDIO_DEF_CHANNELS        2 // *NOTE*: 1: mono, 2: stereo
#define RPG_SOUND_AUDIO_DEF_CHUNKSIZE       4096
#define RPG_SOUND_AUDIO_DEF_PLAY_CHANNELS   15 // #concurrent sounds

#define RPG_SOUND_AMBIENT_DEF_USE_CD        false
#define RPG_SOUND_AMBIENT_DEF_VOLUME        8 // (0-128)

#define RPG_SOUND_CDTRACK_DEF_PAD           3 // allow an extra 3 seconds between tracks
#define RPG_SOUND_DEF_CACHESIZE             50

#define RPG_SOUND_SDL_AUDIODRIVER_ENV_VAR   "SDL_AUDIODRIVER"
#if defined(ACE_WIN32) || defined(ACE_WIN64)
#define RPG_SOUND_DEF_SDL_AUDIO_DRIVER_NAME "dsound"
//#define RPG_SOUND_DEF_SDL_AUDIO_DRIVER_NAME "waveout"
#elif defined(ACE_LINUX)
//#define RPG_SOUND_DEF_SDL_AUDIO_DRIVER_NAME "dsp"
//#define RPG_SOUND_DEF_SDL_AUDIO_DRIVER_NAME "alsa"
#define RPG_SOUND_DEF_SDL_AUDIO_DRIVER_NAME "pulse"
#else
#define RPG_SOUND_DEF_SDL_AUDIO_DRIVER_NAME ""
#endif

#define RPG_SOUND_DEF_FILE_EXT              ".ogg"

#define RPG_SOUND_SUB_DIRECTORY_STRING      "sound"

#define RPG_SOUND_DICTIONARY_FILE           "rpg_sound.xml"
#define RPG_SOUND_DICTIONARY_INSTANCE       "soundDictionary"

// directory organization
#define RPG_SOUND_DATA_SUB                  "sound"
#define RPG_SOUND_AMBIENT_SUB               "ambient"
#define RPG_SOUND_EFFECT_SUB                "effect"

#endif
