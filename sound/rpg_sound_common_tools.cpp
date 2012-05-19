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
#include "stdafx.h"

#include "rpg_sound_common_tools.h"

#include "rpg_sound_defines.h"
#include "rpg_sound_dictionary.h"
#include "rpg_sound_event_manager.h"

#include <rpg_common_macros.h>
#include <rpg_common_file_tools.h>

#include <rpg_dice_common.h>
#include <rpg_dice.h>

#include <SDL/SDL_mixer.h>

#include <ace/Log_Msg.h>

// init statics
RPG_Sound_CategoryToStringTable_t RPG_Sound_CategoryHelper::myRPG_Sound_CategoryToStringTable;
RPG_Sound_EventToStringTable_t RPG_Sound_EventHelper::myRPG_Sound_EventToStringTable;

bool                   RPG_Sound_Common_Tools::myIsMuted;
std::string            RPG_Sound_Common_Tools::mySoundDirectory;
RPG_Sound_SDLConfig_t  RPG_Sound_Common_Tools::myConfig;

ACE_Thread_Mutex       RPG_Sound_Common_Tools::myCacheLock;
unsigned int           RPG_Sound_Common_Tools::myOldestCacheEntry = 0;
unsigned int           RPG_Sound_Common_Tools::myCacheSize = 0;
RPG_Sound_SoundCache_t RPG_Sound_Common_Tools::mySoundCache;

bool                   RPG_Sound_Common_Tools::myInitialized = false;

bool
RPG_Sound_Common_Tools::init(const RPG_Sound_SDLConfig_t& config_in,
                             const std::string& directory_in,
                             const bool& useCD_in,
                             const unsigned int& cacheSize_in,
                             const bool& mute_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Sound_Common_Tools::init"));

  // init string conversion facilities
  RPG_Sound_Common_Tools::initStringConversionTables();

  // init SDL audio handling
//   SDL_AudioSpec wanted;
//   wanted.freq = audioConfig_in.frequency;
//   wanted.format = audioConfig_in.format;
//   wanted.channels = audioConfig_in.channels;
//   wanted.samples = audioConfig_in.samples;
// //   wanted.callback = fill_audio;
// //   wanted.userdata = NULL;
//
//   // Open the audio device, forcing the desired format
//   if (SDL_OpenAudio(&wanted, NULL) < 0)
//   {
//     ACE_DEBUG((LM_ERROR,
//                ACE_TEXT("failed to SDL_OpenAudio(): \"%s\", aborting\n"),
//                SDL_GetError()));
//
//     return;
//   } // end IF
  if (Mix_OpenAudio(config_in.frequency,
                    config_in.format,
                    config_in.channels,
                    config_in.samples) < 0)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to Mix_OpenAudio(): \"%s\", aborting\n"),
               Mix_GetError()));

    return false;
  } // end IF
  if (Mix_AllocateChannels(config_in.mix_channels) != config_in.mix_channels)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to Mix_AllocateChannels(%d): \"%s\", aborting\n"),
               config_in.mix_channels,
               Mix_GetError()));

    return false;
  } // end IF

  RPG_Sound_Common_Tools::myConfig.frequency = 0;
  RPG_Sound_Common_Tools::myConfig.format = 0;
  RPG_Sound_Common_Tools::myConfig.channels = 0;
  RPG_Sound_Common_Tools::myConfig.samples = 0;
  RPG_Sound_Common_Tools::myConfig.mix_channels = Mix_AllocateChannels(-1);
  std::string format_string;
  if (Mix_QuerySpec(&RPG_Sound_Common_Tools::myConfig.frequency,
                    &RPG_Sound_Common_Tools::myConfig.format,
                    &RPG_Sound_Common_Tools::myConfig.channels) == 0)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to Mix_QuerySpec(): \"%s\", aborting\n"),
               Mix_GetError()));

    return false;
  } // end IF
  switch (RPG_Sound_Common_Tools::myConfig.format)
  {
    case AUDIO_U8:
      format_string = ACE_TEXT_ALWAYS_CHAR("AUDIO_U8"); break;
    case AUDIO_S8:
      format_string = ACE_TEXT_ALWAYS_CHAR("AUDIO_S8"); break;
    case AUDIO_U16LSB:
      format_string = ACE_TEXT_ALWAYS_CHAR("AUDIO_U16LSB"); break;
    case AUDIO_S16LSB:
      format_string = ACE_TEXT_ALWAYS_CHAR("AUDIO_S16LSB"); break;
    case AUDIO_U16MSB:
      format_string = ACE_TEXT_ALWAYS_CHAR("AUDIO_U16MSB"); break;
    case AUDIO_S16MSB:
      format_string = ACE_TEXT_ALWAYS_CHAR("AUDIO_S16MSB"); break;
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid audio format (was: %u), aborting\n"),
                 RPG_Sound_Common_Tools::myConfig.format));

      return false;
    }
  } // end SWITCH
  char driver[MAXPATHLEN];
  if (!SDL_AudioDriverName(driver,
                           sizeof(driver)))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_AudioDriverName(): \"%s\", aborting\n"),
               SDL_GetError()));

    return false;
  } // end IF

  RPG_SOUND_EVENT_MANAGER_SINGLETON::instance()->init(directory_in,
                                                      useCD_in);

  ACE_DEBUG((LM_INFO,
             ACE_TEXT("*** audio capabilities (driver: \"%s\") ***\nfrequency: %d\nformat: %s\nchannels: %d\nmix channels: %d\nCD: %s\n"),
             ACE_TEXT(driver),
             RPG_Sound_Common_Tools::myConfig.frequency,
             ACE_TEXT(format_string.c_str()),
             RPG_Sound_Common_Tools::myConfig.channels,
             RPG_Sound_Common_Tools::myConfig.mix_channels,
             (useCD_in ? ACE_TEXT(SDL_CDName(0)) : ACE_TEXT("N/A"))));

  myIsMuted = mute_in;
  if (!directory_in.empty())
  {
    // sanity check(s)
    if (!RPG_Common_File_Tools::isDirectory(directory_in))
    {
      // re-try with resolved path
      std::string resolved_path = RPG_Common_File_Tools::realPath(directory_in);
      if (!RPG_Common_File_Tools::isDirectory(resolved_path))
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("invalid argument \"%s\": not a directory, aborting\n"),
                   directory_in.c_str()));

        return false;
      } // end IF
    } // end IF
    mySoundDirectory = directory_in;
  } // end IF

  myCacheSize = cacheSize_in;

  if (myInitialized)
  {
    // clean up
    fini();

    myInitialized = false;
  } // end IF
  else
  {

  } // end ELSE

  myInitialized = true;

  return true;
}

void
RPG_Sound_Common_Tools::fini()
{
  RPG_TRACE(ACE_TEXT("RPG_Sound_Common_Tools::fini"));

  // synch cache access
  {
    ACE_Guard<ACE_Thread_Mutex> aGuard(myCacheLock);

    // clear the sound cache
    for (RPG_Sound_SoundCacheIterator_t iter = mySoundCache.begin();
         iter != mySoundCache.end();
         iter++)
    {
      Mix_FreeChunk((*iter).chunk);
    } // end FOR
    mySoundCache.clear();
    myOldestCacheEntry = 0;
  } // end lock scope

  RPG_SOUND_EVENT_MANAGER_SINGLETON::instance()->fini();

  myInitialized = false;
}

void
RPG_Sound_Common_Tools::soundToFile(const RPG_Sound_t& sound_in,
                                    std::string& file_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Sound_Common_Tools::soundToFile"));

  // init return value(s)
  file_out = mySoundDirectory;
  file_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;

  switch (sound_in.category)
  {
    case CATEGORY_EFFECT_ONESHOT:
    case CATEGORY_EFFECT_INTERVAL:
    {
      // assemble path
      file_out += ACE_TEXT_ALWAYS_CHAR(RPG_SOUND_DEF_EFFECT_SUB);
      file_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
      file_out += sound_in.file;

      break;
    }
    case CATEGORY_MUSIC_ONESHOT:
    case CATEGORY_MUSIC_AMBIENT:
    {
      // assemble path
      file_out += ACE_TEXT_ALWAYS_CHAR(RPG_SOUND_DEF_AMBIENT_SUB);
      file_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
      file_out += sound_in.file;

      break;
    }
    case CATEGORY_MUSIC_CDTRACK:
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid category (was: \"%s\"), aborting\n"),
                 RPG_Sound_CategoryHelper::RPG_Sound_CategoryToString(sound_in.category).c_str()));

      file_out.clear();

      return;
    }
  } // end SWITCH
}

int
RPG_Sound_Common_Tools::play(const RPG_Sound_Event& event_in,
                             ACE_Time_Value& length_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Sound_Common_Tools::play"));

  // init result(s)
  int result = -1;
  length_out = ACE_Time_Value::zero;

  // step1: sound already cached ?
  RPG_Sound_SoundCacheNode_t node;
  node.chunk = NULL;
  node.sound_event = event_in;

  // synch cache access
  {
    ACE_Guard<ACE_Thread_Mutex> aGuard(myCacheLock);

    RPG_Sound_SoundCacheIterator_t iter = mySoundCache.begin();
    for (;
         iter != mySoundCache.end();
         iter++)
      if ((*iter) == node)
        break;
    if (iter == mySoundCache.end())
    {
      RPG_Sound_t sound;
      sound.category = RPG_SOUND_CATEGORY_INVALID;
      sound.sound_event = RPG_SOUND_EVENT_INVALID;
      sound.file.clear();
      sound.interval = 0;
      // retrieve event properties from the dictionary
      sound = RPG_SOUND_DICTIONARY_SINGLETON::instance()->get(event_in);
      ACE_ASSERT(sound.sound_event == event_in);
      // load the file
      RPG_Sound_Common_Tools::soundToFile(sound, node.sound_file);
      // sanity check
      if (!RPG_Common_File_Tools::isReadable(node.sound_file))
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("invalid argument(\"%s\"): not readable, aborting\n"),
                   ACE_TEXT(node.sound_file.c_str())));

        return result;
      } // end IF
      node.chunk = Mix_LoadWAV(node.sound_file.c_str());
      if (!node.chunk)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to Mix_LoadWAV(\"%s\"): \"%s\", aborting\n"),
                   ACE_TEXT(node.sound_file.c_str()),
                   SDL_GetError()));

        return result;
      } // end IF

      // set volume (if any)
      if (sound.volume)
        Mix_VolumeChunk(node.chunk, sound.volume);

      // add the chunk to our cache
      if (mySoundCache.size() == myCacheSize)
      {
        iter = mySoundCache.begin();
        ACE_ASSERT(mySoundCache.size() >= myOldestCacheEntry);
        std::advance(iter, myOldestCacheEntry);
        // *TODO*: what if it's still being used ?...
        Mix_FreeChunk((*iter).chunk);
        mySoundCache.erase(iter);
        myOldestCacheEntry++;
        if (myOldestCacheEntry == myCacheSize)
          myOldestCacheEntry = 0;
      } // end IF
      mySoundCache.push_back(node);
      iter = mySoundCache.end(); iter--;
    } // end IF
    ACE_ASSERT((*iter).chunk);

    // compute length
    ACE_UINT64 milliseconds = (*iter).chunk->alen / ((RPG_Sound_Common_Tools::myConfig.format & 0xFF) / 8);
    milliseconds /= RPG_Sound_Common_Tools::myConfig.channels; // <-- frames
    milliseconds *= 1000;
    milliseconds /= RPG_Sound_Common_Tools::myConfig.frequency;
    length_out.msec(static_cast<int>(milliseconds));
    length_out.msec(milliseconds);

    if (!myIsMuted)
    {
      result = Mix_PlayChannel(-1,            // play on the first free channel
                               (*iter).chunk, // data
                               0);            // don't loop
      if (result == -1)
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to Mix_PlayChannel(\"%s\"): \"%s\", aborting\n"),
                   RPG_Sound_EventHelper::RPG_Sound_EventToString(event_in).c_str(),
                   Mix_GetError()));
    } // end IF
    else
      result = 0;
  } // end lock scope

  return result;
}

int
RPG_Sound_Common_Tools::play(const std::string& file_in,
                             const unsigned char& volume_in,
                             ACE_Time_Value& length_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Sound_Common_Tools::play"));

  // init result(s)
  int result = -1;
  length_out = ACE_Time_Value::zero;

  // step1: sound already cached ?
  RPG_Sound_SoundCacheNode_t node;
  node.chunk = NULL;
  node.sound_event = EVENT_AMBIENT;
  node.sound_file = file_in;

  // synch cache access
  {
    ACE_Guard<ACE_Thread_Mutex> aGuard(myCacheLock);

    RPG_Sound_SoundCacheIterator_t iter = mySoundCache.begin();
    for (;
         iter != mySoundCache.end();
         iter++)
      if ((*iter) == node)
        break;
    if (iter == mySoundCache.end())
    {
      // sanity check
      if (!RPG_Common_File_Tools::isReadable(node.sound_file))
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("invalid argument(\"%s\"): not readable, aborting\n"),
                   ACE_TEXT(node.sound_file.c_str())));

        return result;
      } // end IF
      node.chunk = Mix_LoadWAV(node.sound_file.c_str());
      if (!node.chunk)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to Mix_LoadWAV(\"%s\"): \"%s\", aborting\n"),
                   ACE_TEXT(node.sound_file.c_str()),
                   SDL_GetError()));

        return result;
      } // end IF

      // set volume (if any)
      ACE_ASSERT(volume_in <= MIX_MAX_VOLUME);
      Mix_VolumeChunk(node.chunk, volume_in);

      // add the chunk to our cache
      if (mySoundCache.size() == myCacheSize)
      {
        iter = mySoundCache.begin();
        ACE_ASSERT(mySoundCache.size() >= myOldestCacheEntry);
        std::advance(iter, myOldestCacheEntry);
        // *TODO*: what if it's still being used ?...
        Mix_FreeChunk((*iter).chunk);
        mySoundCache.erase(iter);
        myOldestCacheEntry++;
        if (myOldestCacheEntry == myCacheSize)
          myOldestCacheEntry = 0;
      } // end IF
      mySoundCache.push_back(node);
      iter = mySoundCache.end(); iter--;
    } // end IF
    ACE_ASSERT((*iter).chunk);
  
    // compute length
    ACE_UINT64 milliseconds = (*iter).chunk->alen / ((RPG_Sound_Common_Tools::myConfig.format & 0xFF) / 8);
    milliseconds /= RPG_Sound_Common_Tools::myConfig.channels; // <-- frames
    milliseconds *= 1000;
    milliseconds /= RPG_Sound_Common_Tools::myConfig.frequency;
    length_out.msec(static_cast<int>(milliseconds));

    if (!myIsMuted)
    {
      result = Mix_PlayChannel(-1,            // play on the first free channel
                               (*iter).chunk, // data
                               0);            // don't loop
      if (result == -1)
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to Mix_PlayChannel(\"%s\"): \"%s\", aborting\n"),
                   ACE_TEXT(file_in.c_str()),
                   ACE_TEXT(Mix_GetError())));
    } // end IF
    else
      result = 0;

  } // end lock scope

  return result;
}

bool
RPG_Sound_Common_Tools::isPlaying(const int& channel_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Sound_Common_Tools::isPlaying"));

  return Mix_Playing(channel_in);
}

void
RPG_Sound_Common_Tools::stop(const int& channel_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Sound_Common_Tools::stop"));

  Mix_HaltChannel(-1);
}

int
RPG_Sound_Common_Tools::playRandomTrack(SDL_CD* cdrom_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Sound_Common_Tools::playRandomTrack"));

  // sanity check(s)
  ACE_ASSERT(cdrom_in);
  if (cdrom_in->numtracks <= 0)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("no available tracks on CD (drive: \"%s\"), aborting\n"),
               ACE_TEXT(SDL_CDName(cdrom_in->id))));

    return - 1;
  } // end IF
  unsigned int num_audio_tracks = 0;
  for (int i = 0;
       i < cdrom_in->numtracks;
       i++)
    if (cdrom_in->track[i].type == SDL_AUDIO_TRACK)
      num_audio_tracks++;
  if (num_audio_tracks == 0)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("no audio tracks on CD (drive: \"%s\"), aborting\n"),
               ACE_TEXT(SDL_CDName(cdrom_in->id))));

    return -1;
  } // end IF

  // choose random track
  RPG_Dice_RollResult_t roll_result;
  do
  {
    roll_result.clear();
    RPG_Dice::generateRandomNumbers(cdrom_in->numtracks,
                                    1,
                                    roll_result);

    if (cdrom_in->track[roll_result.front() - 1].type == SDL_AUDIO_TRACK)
      break; // not an audio track, try again
  } while (true);

  // play track ?
  if (!myIsMuted)
  {
    if (SDL_CDPlay(cdrom_in,
                   cdrom_in->track[roll_result.front() - 1].offset,
                   cdrom_in->track[roll_result.front() - 1].length))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to play CD track %u (drive: \"%s\"): \"%s\", aborting\n"),
                 roll_result.front(),
                 ACE_TEXT(SDL_CDName(cdrom_in->id)),
                 ACE_TEXT(SDL_GetError())));

      return -1;
    } // end IF
  } // end IF

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("playing CD track %u (drive: \"%s\")...\n"),
             roll_result.front(),
             ACE_TEXT(SDL_CDName(cdrom_in->id))));

  return (roll_result.front() - 1);
}

void
RPG_Sound_Common_Tools::initStringConversionTables()
{
  RPG_TRACE(ACE_TEXT("RPG_Sound_Common_Tools::initStringConversionTables"));

  RPG_Sound_CategoryHelper::init();
  RPG_Sound_EventHelper::init();

  // debug info
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("RPG_Sound_Common_Tools: initialized string conversion tables...\n")));
}
