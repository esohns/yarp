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

#include "ace/config-lite.h"

#if defined (SDL_USE) || defined (SDL2_USE)
#define RPG_SOUND_AUDIO_DEF_FREQUENCY       44100
#define RPG_SOUND_AUDIO_DEF_FORMAT          AUDIO_S16SYS
#elif defined (SDL3_USE)
#define RPG_SOUND_AUDIO_DEF_FREQUENCY       48000
#define RPG_SOUND_AUDIO_DEF_FORMAT          SDL_AUDIO_F32
#endif // SDL_USE || SDL2_USE || SDL3_USE
#define RPG_SOUND_AUDIO_DEF_CHANNELS        2 // *NOTE*: 1: mono, 2: stereo
#define RPG_SOUND_AUDIO_DEF_CHUNKSIZE       4096
#define RPG_SOUND_AUDIO_DEF_PLAY_CHANNELS   16 // #concurrent sounds

#define RPG_SOUND_AMBIENT_DEF_USE_CD        false
#define RPG_SOUND_AMBIENT_DEF_VOLUME        16 // (0-128)

#define RPG_SOUND_CDTRACK_DEF_PAD           3 // allow an extra 3 seconds between tracks
#define RPG_SOUND_DEF_CACHESIZE             50

#if defined (SDL_USE) || defined (SDL2_USE)
#define RPG_SOUND_SDL_AUDIODRIVER_ENV_VAR   "SDL_AUDIODRIVER"
#elif defined (SDL3_USE)
#define RPG_SOUND_SDL_AUDIODRIVER_ENV_VAR   "SDL_AUDIO_DRIVER"
#endif // SDL_USE || SDL2_USE || SDL3_USE
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#if defined (SDL_USE)
#define RPG_SOUND_DEF_SDL_AUDIO_DRIVER_NAME "dsound"
//#define RPG_SOUND_DEF_SDL_AUDIO_DRIVER_NAME "waveout"
#elif defined (SDL2_USE) || defined (SDL3_USE)
#define RPG_SOUND_DEF_SDL_AUDIO_DRIVER_NAME "directsound"
// #define RPG_SOUND_DEF_SDL_AUDIO_DRIVER_NAME "wasapi"
#endif // SDL_USE || SDL2_USE || SDL3_USE

#define RPG_SOUND_DEF_SDL_AUDIO_DEVICE_NAME ""
#elif defined (ACE_LINUX)
//#define RPG_SOUND_DEF_SDL_AUDIO_DRIVER_NAME "pipewire"
//#define RPG_SOUND_DEF_SDL_AUDIO_DRIVER_NAME "pulse"
#define RPG_SOUND_DEF_SDL_AUDIO_DRIVER_NAME "alsa"
//#define RPG_SOUND_DEF_SDL_AUDIO_DRIVER_NAME "dsp"

#define RPG_SOUND_DEF_SDL_AUDIO_DEVICE_NAME "ALSA default output device"
#else
#define RPG_SOUND_DEF_SDL_AUDIO_DRIVER_NAME ""

#define RPG_SOUND_DEF_SDL_AUDIO_DEVICE_NAME ""
#endif // ACE_WIN32 || ACE_WIN64 || ACE_LINUX

#define RPG_SOUND_DEF_FILE_EXT              ".ogg"

#define RPG_SOUND_SUB_DIRECTORY_STRING      "sound"

#define RPG_SOUND_DICTIONARY_FILE           "rpg_sound.xml"
#define RPG_SOUND_DICTIONARY_INSTANCE       "soundDictionary"

// directory organization
#define RPG_SOUND_DATA_SUB                  "sound"
#define RPG_SOUND_AMBIENT_SUB               "ambient"
#define RPG_SOUND_EFFECT_SUB                "effect"

#endif
