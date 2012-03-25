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

#include <rpg_common_macros.h>
#include <rpg_common_file_tools.h>

#include <SDL/SDL_mixer.h>

#include <ace/Log_Msg.h>

// init statics
RPG_Sound_CategoryToStringTable_t RPG_Sound_CategoryHelper::myRPG_Sound_CategoryToStringTable;
RPG_Sound_EventToStringTable_t RPG_Sound_EventHelper::myRPG_Sound_EventToStringTable;

std::string            RPG_Sound_Common_Tools::mySoundDirectory;

ACE_Thread_Mutex       RPG_Sound_Common_Tools::myCacheLock;
unsigned long          RPG_Sound_Common_Tools::myOldestCacheEntry = 0;
unsigned long          RPG_Sound_Common_Tools::myCacheSize = 0;
RPG_Sound_SoundCache_t RPG_Sound_Common_Tools::mySoundCache;

bool                   RPG_Sound_Common_Tools::myInitialized = false;

void RPG_Sound_Common_Tools::init(const std::string& directory_in,
                                  const unsigned long& cacheSize_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Sound_Common_Tools::init"));

  // sanity check(s)
  if (!RPG_Common_File_Tools::isDirectory(directory_in))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid argument \"%s\": not a directory, aborting\n"),
               directory_in.c_str()));

    return;
  } // end IF
  mySoundDirectory = directory_in;
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

const int
RPG_Sound_Common_Tools::play(const RPG_Sound_Event& event_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Sound_Common_Tools::play"));

  int result = -1;

  // step1: sound already cached ?
  RPG_Sound_SoundCacheNode_t node;
  node.event = event_in;

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
      sound.event = RPG_SOUND_EVENT_INVALID;
      sound.file.clear();
      sound.interval = 0;
      // retrieve event properties from the dictionary
      sound = RPG_SOUND_DICTIONARY_SINGLETON::instance()->getSound(event_in);
      ACE_ASSERT(sound.event == event_in);
      // load the file
      Mix_Chunk* chunk = NULL;
      std::string path;
      RPG_Sound_Common_Tools::soundToFile(sound, path);
      // sanity check
      if (!RPG_Common_File_Tools::isReadable(path))
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("invalid argument(\"%s\"): not readable, aborting\n"),
                   path.c_str()));

        return result;
      } // end IF
      chunk = Mix_LoadWAV(path.c_str());
      if (!chunk)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to Mix_LoadWAV(\"%s\"): \"%s\", aborting\n"),
                   path.c_str(),
                   SDL_GetError()));

        return result;
      } // end IF
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
      node.chunk = chunk;
      mySoundCache.push_back(node);
      iter = mySoundCache.end(); iter--;
    } // end IF

    ACE_ASSERT((*iter).chunk);
    result = Mix_PlayChannel(-1,            // play on the first free channel
                             (*iter).chunk, // data
                             0);            // don't loop
  } // end lock scope

  return result;
}

const bool
RPG_Sound_Common_Tools::isPlaying(const int& channel_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Sound_Common_Tools::isPlaying"));

  if (channel_in == -1)
    return Mix_PlayingMusic();

  return Mix_Playing(channel_in);
}

void
RPG_Sound_Common_Tools::stop(const int& channel_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Sound_Common_Tools::stop"));

  if (channel_in == -1)
    Mix_HaltMusic();
  else
    Mix_HaltChannel(channel_in);
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
