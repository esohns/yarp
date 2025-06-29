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

#define _SDL_main_h
#define SDL_main_h_
#include "SDL.h"
#include "SDL_mixer.h"

#include "ace/Log_Msg.h"

#include "common_file_tools.h"

// #include "rpg_dice.h"
// #include "rpg_dice_common.h"

#include "rpg_common_macros.h"

#include "rpg_sound_defines.h"
#include "rpg_sound_dictionary.h"
#include "rpg_sound_event_manager.h"

// initialize statics
RPG_Sound_CategoryToStringTable_t RPG_Sound_CategoryHelper::myRPG_Sound_CategoryToStringTable;
RPG_Sound_EventToStringTable_t RPG_Sound_EventHelper::myRPG_Sound_EventToStringTable;

bool                         RPG_Sound_Common_Tools::myIsMuted;
std::string                  RPG_Sound_Common_Tools::mySoundDirectory;
struct RPG_Sound_SDLConfiguration RPG_Sound_Common_Tools::myConfig;

ACE_Thread_Mutex             RPG_Sound_Common_Tools::myCacheLock;
unsigned int                 RPG_Sound_Common_Tools::myOldestCacheEntry = 0;
unsigned int                 RPG_Sound_Common_Tools::myCacheSize = 0;
RPG_Sound_SoundCache_t       RPG_Sound_Common_Tools::mySoundCache;

bool                         RPG_Sound_Common_Tools::myInitialized = false;

bool
RPG_Sound_Common_Tools::preInitialize ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Sound_Common_Tools::preInitialize"));

#if defined (_DEBUG)
#if defined (SDL2_USE) || defined (SDL3_USE)
  for (int i = 0;
       i < SDL_GetNumAudioDrivers ();
       ++i)
  {
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("%d: found audio driver \"%s\"...\n"),
                i, ACE_TEXT (SDL_GetAudioDriver (i))));
  } // end FOR
#endif // SDL2_USE || SDL3_USE
#endif // _DEBUG

  SDL_bool result =
#if defined (SDL_USE) || defined (SDL2_USE)
    SDL_SetHint (ACE_TEXT_ALWAYS_CHAR (SDL_HINT_AUDIODRIVER),
#elif defined (SDL3_USE)
    SDL_SetHint (ACE_TEXT_ALWAYS_CHAR (SDL_HINT_AUDIO_DRIVER),
#endif // SDL_USE || SDL2_USE || SDL3_USE
                 ACE_TEXT_ALWAYS_CHAR (RPG_SOUND_DEF_SDL_AUDIO_DRIVER_NAME));
  if (result == SDL_FALSE)
#if defined (SDL_USE) || defined (SDL2_USE)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_SetHint(\"%s\",\"%s\"): \"%s\", aborting\n"),
                ACE_TEXT (SDL_HINT_AUDIODRIVER),
                ACE_TEXT (RPG_SOUND_DEF_SDL_AUDIO_DRIVER_NAME),
                ACE_TEXT (SDL_GetError ())));
#elif defined (SDL3_USE)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_SetHint(\"%s\",\"%s\"): \"%s\", aborting\n"),
                ACE_TEXT (SDL_HINT_AUDIO_DRIVER),
                ACE_TEXT (RPG_SOUND_DEF_SDL_AUDIO_DRIVER_NAME),
                ACE_TEXT (SDL_GetError ())));
#endif // SDL_USE || SDL2_USE || SDL3_USE
  else
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("set audio driver to \"%s\"...\n"),
                ACE_TEXT (RPG_SOUND_DEF_SDL_AUDIO_DRIVER_NAME)));

  return result == SDL_TRUE;
}

bool
RPG_Sound_Common_Tools::initialize (const struct RPG_Sound_SDLConfiguration& config_in,
                                    const std::string& directory_in,
                                    bool useCD_in,
                                    unsigned int cacheSize_in,
                                    bool mute_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Sound_Common_Tools::initialize"));

  // initialize string conversion facilities
  RPG_Sound_Common_Tools::initializeStringConversionTables ();

  myIsMuted = mute_in;
  if (!directory_in.empty ())
  {
    // sanity check(s)
    if (!Common_File_Tools::isDirectory (directory_in))
    {
      // re-try with resolved path
      std::string resolved_path = Common_File_Tools::realPath (directory_in);
      if (!Common_File_Tools::isDirectory (resolved_path))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid argument \"%s\": not a directory, aborting\n"),
                    ACE_TEXT (directory_in.c_str ())));
        return false;
      } // end IF
    } // end IF
    mySoundDirectory = directory_in;
  } // end IF
	myConfig = config_in;
  myCacheSize = cacheSize_in;

  RPG_SOUND_EVENT_MANAGER_SINGLETON::instance ()->initialize (directory_in,
                                                              (mute_in ? false
                                                                       : useCD_in));

  // initialize SDL audio handling
  if (!mute_in)
  {
    //   SDL_AudioSpec wanted;
    //   wanted.freq = audioConfig_in.frequency;
    //   wanted.format = audioConfig_in.format;
    //   wanted.channels = audioConfig_in.channels;
    //   wanted.samples = audioConfig_in.chunksize;
    // //   wanted.callback = fill_audio;
    // //   wanted.userdata = NULL;
    //
    //   // Open the audio device, forcing the desired format
    //   if (SDL_OpenAudio(&wanted, NULL) < 0)
    //   {
    //     ACE_DEBUG((LM_ERROR,
    //                ACE_TEXT("failed to SDL_OpenAudio(): \"%s\", aborting\n"),
    //                ACE_TEXT(SDL_GetError())));
    //     return;
    //   } // end IF

#if defined (SDL_USE) || defined (SDL2_USE)
    if (Mix_OpenAudio (config_in.frequency,
                       config_in.format,
                       config_in.channels,
                       config_in.chunksize) < 0)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Mix_OpenAudio(): \"%s\", aborting\n"),
                  ACE_TEXT (Mix_GetError ())));
#elif defined (SDL3_USE)
    int flags_i = MIX_INIT_FLAC   |
                  MIX_INIT_MOD    |
                  MIX_INIT_MP3    |
                  MIX_INIT_OGG    |
                  MIX_INIT_MID    |
                  MIX_INIT_OPUS   |
                  MIX_INIT_WAVPACK;
    int result = Mix_Init (flags_i);
    if (result != flags_i)
      ACE_DEBUG ((LM_WARNING,
                  ACE_TEXT ("failed to Mix_Init(0x%x); got: 0x%x, continuing\n"),
                  flags_i, result));

    SDL_AudioDeviceID device_id_i = SDL_AUDIO_DEVICE_DEFAULT_OUTPUT;
    int num_audio_output_devices_i = 0;
    SDL_AudioDeviceID* audio_device_ids_a =
      SDL_GetAudioOutputDevices (&num_audio_output_devices_i);
    if (!audio_device_ids_a)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to SDL_GetAudioOutputDevices(): \"%s\", aborting\n"),
                  ACE_TEXT (SDL_GetError ())));
      return false;
    } // end IF
    for (int i = 0; i < num_audio_output_devices_i; ++i)
    {
      char* device_name_p = SDL_GetAudioDeviceName (audio_device_ids_a[i]);
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("found audio device %d: \"%s\"...\n"),
                  i, ACE_TEXT (device_name_p)));
      if (!ACE_OS::strcmp (device_name_p,
                           ACE_TEXT_ALWAYS_CHAR (RPG_SOUND_DEF_SDL_AUDIO_DEVICE_NAME)))
        device_id_i = audio_device_ids_a[i];
      SDL_free (device_name_p);
    } // end FOR
    SDL_free (audio_device_ids_a);

    SDL_AudioSpec audio_spec_s { config_in.format,
                                 config_in.channels,
                                 config_in.frequency };
    if (Mix_OpenAudio (device_id_i,
                       &audio_spec_s) < 0)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Mix_OpenAudio(%u): \"%s\", aborting\n"),
                  device_id_i,
                  ACE_TEXT (Mix_GetError ())));
#endif // SDL_USE || SDL2_USE || SDL3_USE
      return false;
    } // end IF

    if (Mix_AllocateChannels (RPG_SOUND_AUDIO_DEF_PLAY_CHANNELS) != RPG_SOUND_AUDIO_DEF_PLAY_CHANNELS)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Mix_AllocateChannels(%d): \"%s\", aborting\n"),
                  RPG_SOUND_AUDIO_DEF_PLAY_CHANNELS,
                  ACE_TEXT (Mix_GetError ())));
      return false;
    } // end IF
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("allocated %d audio channel(s)...\n"),
                RPG_SOUND_AUDIO_DEF_PLAY_CHANNELS));

    RPG_Sound_Common_Tools::myConfig.frequency = 0;
    RPG_Sound_Common_Tools::myConfig.format = 0;
    RPG_Sound_Common_Tools::myConfig.channels = 0;
    RPG_Sound_Common_Tools::myConfig.chunksize = 0;
    std::string format_string;
    if (Mix_QuerySpec (&RPG_Sound_Common_Tools::myConfig.frequency,
                       &RPG_Sound_Common_Tools::myConfig.format,
                       &RPG_Sound_Common_Tools::myConfig.channels) == 0)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Mix_QuerySpec(): \"%s\", aborting\n"),
                  ACE_TEXT (Mix_GetError ())));
      return false;
    } // end IF
    switch (RPG_Sound_Common_Tools::myConfig.format)
    {
#if defined (SDL_USE) || defined (SDL2_USE)
      case AUDIO_U8:
        format_string = ACE_TEXT_ALWAYS_CHAR ("AUDIO_U8"); break;
      case AUDIO_S8:
        format_string = ACE_TEXT_ALWAYS_CHAR ("AUDIO_S8"); break;
      case AUDIO_U16LSB:
        format_string = ACE_TEXT_ALWAYS_CHAR ("AUDIO_U16LSB"); break;
      case AUDIO_S16LSB:
        format_string = ACE_TEXT_ALWAYS_CHAR ("AUDIO_S16LSB"); break;
      case AUDIO_U16MSB:
        format_string = ACE_TEXT_ALWAYS_CHAR ("AUDIO_U16MSB"); break;
      case AUDIO_S16MSB:
        format_string = ACE_TEXT_ALWAYS_CHAR ("AUDIO_S16MSB"); break;
#elif defined (SDL3_USE)
      case SDL_AUDIO_U8:
        format_string = ACE_TEXT_ALWAYS_CHAR ("SDL_AUDIO_U8"); break;
      case SDL_AUDIO_S8:
        format_string = ACE_TEXT_ALWAYS_CHAR ("SDL_AUDIO_S8"); break;
      case SDL_AUDIO_S16LE:
        format_string = ACE_TEXT_ALWAYS_CHAR ("SDL_AUDIO_S16LE"); break;
      case SDL_AUDIO_S16BE:
        format_string = ACE_TEXT_ALWAYS_CHAR ("SDL_AUDIO_S16BE"); break;
      case SDL_AUDIO_S32LE:
        format_string = ACE_TEXT_ALWAYS_CHAR ("SDL_AUDIO_S32LE"); break;
      case SDL_AUDIO_S32BE:
        format_string = ACE_TEXT_ALWAYS_CHAR ("SDL_AUDIO_S32BE"); break;
      case SDL_AUDIO_F32LE:
        format_string = ACE_TEXT_ALWAYS_CHAR ("SDL_AUDIO_F32LE"); break;
      case SDL_AUDIO_F32BE:
        format_string = ACE_TEXT_ALWAYS_CHAR ("SDL_AUDIO_F32BE"); break;
#endif // SDL_USE || SDL2_USE || SDL3_USE
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid audio format (was: %u), aborting\n"),
                    RPG_Sound_Common_Tools::myConfig.format));
        return false;
      }
    } // end SWITCH

#if defined (SDL_USE)
    char driver[BUFSIZ];
    if (!SDL_AudioDriverName (driver,
                              sizeof (char[BUFSIZ])))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to SDL_AudioDriverName(): \"%s\", aborting\n"),
                  ACE_TEXT (SDL_GetError ())));
      return false;
    } // end IF

    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT("*** audio capabilities (driver: \"%s\") ***\nfrequency:\t%d\nformat:\t\t%s\nchannels:\t%d\nCD:\t\t%s\n"),
                ACE_TEXT(driver),
                RPG_Sound_Common_Tools::myConfig.frequency,
                ACE_TEXT(format_string.c_str()),
                RPG_Sound_Common_Tools::myConfig.channels,
                (useCD_in ? ACE_TEXT (SDL_CDName(0)) : ACE_TEXT ("N/A"))));
#elif defined (SDL2_USE) || defined (SDL3_USE)
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("*** audio capabilities ***\nfrequency:\t%d\nformat:\t\t%s\nchannels:\t%d\n"),
                RPG_Sound_Common_Tools::myConfig.frequency,
                ACE_TEXT (format_string.c_str ()),
                RPG_Sound_Common_Tools::myConfig.channels));
#endif // SDL_USE || SDL2_USE || SDL3_USE

    int total = Mix_GetNumChunkDecoders ();
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("*** audio decoders (effects) ***\n")));
    for (int i = 0; i < total; i++)
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("chunk decoder: [%s]\n"),
                  ACE_TEXT (Mix_GetChunkDecoder (i))));
    total = Mix_GetNumMusicDecoders ();
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("*** audio decoders (music) ***\n")));
    for (int i = 0; i < total; i++)
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("music decoder: [%s]\n"),
                  ACE_TEXT (Mix_GetMusicDecoder (i))));
  } // end IF

  myInitialized = true;

  return true;
}

void
RPG_Sound_Common_Tools::finalize ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Sound_Common_Tools::finalize"));

  // synch cache access
  { ACE_GUARD (ACE_Thread_Mutex, aGuard, myCacheLock);
    // clear the sound cache
    for (RPG_Sound_SoundCacheIterator_t iter = mySoundCache.begin ();
         iter != mySoundCache.end ();
         iter++)
      Mix_FreeChunk ((*iter).chunk);
    mySoundCache.clear ();
    myOldestCacheEntry = 0;
  } // end lock scope

  RPG_SOUND_EVENT_MANAGER_SINGLETON::instance ()->finalize ();

  Mix_CloseAudio ();
  Mix_Quit ();

  myInitialized = false;
}

void
RPG_Sound_Common_Tools::soundToFile (const RPG_Sound_t& sound_in,
                                     std::string& file_out)
{
  RPG_TRACE (ACE_TEXT ("RPG_Sound_Common_Tools::soundToFile"));

  // initialize return value(s)
  file_out = mySoundDirectory;
  file_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;

  switch (sound_in.category)
  {
    case CATEGORY_EFFECT_ONESHOT:
    case CATEGORY_EFFECT_INTERVAL:
    {
      // assemble path
      file_out += ACE_TEXT_ALWAYS_CHAR (RPG_SOUND_EFFECT_SUB);
      file_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
      file_out += sound_in.file;

      break;
    }
    case CATEGORY_MUSIC_ONESHOT:
    case CATEGORY_MUSIC_AMBIENT:
    {
      // assemble path
      file_out += ACE_TEXT_ALWAYS_CHAR (RPG_SOUND_AMBIENT_SUB);
      file_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
      file_out += sound_in.file;

      break;
    }
    case CATEGORY_MUSIC_CDTRACK:
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid category (was: \"%s\"), aborting\n"),
                  ACE_TEXT (RPG_Sound_CategoryHelper::RPG_Sound_CategoryToString (sound_in.category).c_str ())));
      file_out.clear ();
      return;
    }
  } // end SWITCH
}

int
RPG_Sound_Common_Tools::play (enum RPG_Sound_Event event_in,
                              ACE_Time_Value& length_out)
{
  RPG_TRACE (ACE_TEXT ("RPG_Sound_Common_Tools::play"));

  // initialize result(s)
  int result = -1;
  length_out = ACE_Time_Value::zero;

  // step1: sound already cached ?
  struct RPG_Sound_SoundCacheNode node;
  node.chunk = NULL;
  node.sound_event = event_in;

  // synch cache access
  { ACE_GUARD_RETURN (ACE_Thread_Mutex, aGuard, myCacheLock, -1);
    RPG_Sound_SoundCacheIterator_t iterator = mySoundCache.begin ();
    for (;
         iterator != mySoundCache.end ();
         iterator++)
      if ((*iterator) == node)
        break;
    if (iterator == mySoundCache.end ())
    {
      // retrieve event properties from the dictionary
      RPG_Sound_t sound =
        RPG_SOUND_DICTIONARY_SINGLETON::instance ()->get (event_in);
      ACE_ASSERT (sound.sound_event == event_in);
      // load the file
      RPG_Sound_Common_Tools::soundToFile (sound,
                                           node.sound_file);
      // sanity check
      if (!Common_File_Tools::isReadable (node.sound_file))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid argument(\"%s\"): not readable, aborting\n"),
                    ACE_TEXT (node.sound_file.c_str ())));
        return result;
      } // end IF
#if defined (SDL_USE) || defined (SDL2_USE)
      SDL_RWops* rw_ops = SDL_RWFromFile (node.sound_file.c_str (),
                                          ACE_TEXT_ALWAYS_CHAR ("rb"));
#elif defined (SDL3_USE)
      SDL_IOStream* rw_ops = SDL_IOFromFile (node.sound_file.c_str (),
                                             ACE_TEXT_ALWAYS_CHAR ("rb"));
#endif // SDL_USE || SDL2_USE || SDL3_USE
      if (!rw_ops)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to SDL_RWFromFile(\"%s\"): \"%s\", aborting\n"),
                    ACE_TEXT (node.sound_file.c_str ()),
                    ACE_TEXT (SDL_GetError ())));
        return result;
      } // end IF
#if defined (SDL_USE) || defined (SDL2_USE)
      node.chunk = Mix_LoadWAV_RW (rw_ops,
                                   1); // free rw_ops
#elif defined (SDL3_USE)
      node.chunk = Mix_LoadWAV_IO (rw_ops,
                                   SDL_TRUE); // close stream ?
#endif // SDL_USE || SDL2_USE || SDL3_USE
      if (!node.chunk)
      {
        // *NOTE*: this fails if the sound is muted...
        if (!myIsMuted)
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to Mix_LoadWAV_RW(\"%s\"): \"%s\", aborting\n"),
                      ACE_TEXT (node.sound_file.c_str ()),
                      ACE_TEXT (Mix_GetError ())));
        return (myIsMuted ? 0 : result);
      } // end IF

      // set volume (if any)
      ACE_ASSERT (sound.volume <= MIX_MAX_VOLUME);
      if (sound.volume)
        Mix_VolumeChunk (node.chunk,
                         static_cast<int> (sound.volume));

      // add the chunk to our cache
      if (mySoundCache.size () == myCacheSize)
      {
        iterator = mySoundCache.begin ();
        ACE_ASSERT (mySoundCache.size () >= myOldestCacheEntry);
        std::advance (iterator, myOldestCacheEntry);
        // *TODO*: what if it's still being used ?...
        Mix_FreeChunk ((*iterator).chunk);
        mySoundCache.erase (iterator);
        myOldestCacheEntry++;
        if (myOldestCacheEntry == myCacheSize)
          myOldestCacheEntry = 0;
      } // end IF
      mySoundCache.push_back (node);
      iterator = mySoundCache.end (); iterator--;
    } // end IF
    ACE_ASSERT ((*iterator).chunk);

    // compute length
    ACE_UINT64 milliseconds =
        ((*iterator).chunk->alen /
         ((RPG_Sound_Common_Tools::myConfig.format & 0xFF) / 8));
    milliseconds /= RPG_Sound_Common_Tools::myConfig.channels; // <-- frames
    milliseconds *= 1000;
    milliseconds /= RPG_Sound_Common_Tools::myConfig.frequency;
    length_out.msec (static_cast<int> (milliseconds));

    result = (myIsMuted ? 0
                        : Mix_PlayChannel (-1,                // play on the first free channel
                                           (*iterator).chunk, // data
                                           0));               // don't loop
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Mix_PlayChannel(\"%s\"): \"%s\", aborting\n"),
                  ACE_TEXT (RPG_Sound_EventHelper::RPG_Sound_EventToString (event_in).c_str ()),
                  ACE_TEXT (Mix_GetError ())));
  } // end lock scope

  return result;
}

int
RPG_Sound_Common_Tools::play (const std::string& file_in,
                              ACE_UINT8 volume_in,
                              ACE_Time_Value& length_out)
{
  RPG_TRACE (ACE_TEXT ("RPG_Sound_Common_Tools::play"));

  // initialize result(s)
  int result = -1;
  length_out = ACE_Time_Value::zero;

  // step1: sound already cached ?
  struct RPG_Sound_SoundCacheNode node;
  node.chunk = NULL;
  node.sound_event = EVENT_AMBIENT;
  node.sound_file = file_in;

  // synch cache access
  { ACE_GUARD_RETURN (ACE_Thread_Mutex, aGuard, myCacheLock, -1);
    RPG_Sound_SoundCacheIterator_t iterator = mySoundCache.begin ();
    for (;
         iterator != mySoundCache.end ();
         iterator++)
      if ((*iterator) == node)
        break;
    if (iterator == mySoundCache.end ())
    {
      // sanity check
      if (!Common_File_Tools::isReadable (node.sound_file))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid argument(\"%s\"): not readable, aborting\n"),
                    ACE_TEXT (node.sound_file.c_str ())));
        return result;
      } // end IF
#if defined (SDL_USE) || defined (SDL2_USE)
      SDL_RWops* rw_ops = SDL_RWFromFile (node.sound_file.c_str (),
                                          ACE_TEXT_ALWAYS_CHAR ("rb"));
#elif defined (SDL3_USE)
      SDL_IOStream* rw_ops = SDL_IOFromFile (node.sound_file.c_str (),
                                             ACE_TEXT_ALWAYS_CHAR ("rb"));
#endif // SDL_USE || SDL2_USE || SDL3_USE
      if (!rw_ops)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to SDL_RWFromFile(\"%s\"): \"%s\", aborting\n"),
                    ACE_TEXT (node.sound_file.c_str ()),
                    ACE_TEXT (SDL_GetError ())));
        return result;
      } // end IF
#if defined (SDL_USE) || defined (SDL2_USE)
      node.chunk = Mix_LoadWAV_RW (rw_ops,
                                   1); // free rw_ops
#elif defined (SDL3_USE)
      node.chunk = Mix_LoadWAV_IO (rw_ops,
                                   1); // free rw_ops
#endif // SDL_USE || SDL2_USE || SDL3_USE
      if (!node.chunk)
      {
        // *NOTE*: this fails if the sound is muted...
        if (!myIsMuted)
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to Mix_LoadWAV_RW(\"%s\"): \"%s\", aborting\n"),
                      ACE_TEXT (node.sound_file.c_str ()),
                      ACE_TEXT (SDL_GetError ())));
        return (myIsMuted ? 0 : result);
      } // end IF

      // set volume (if any)
      ACE_ASSERT (volume_in <= MIX_MAX_VOLUME);
      if (volume_in)
        Mix_VolumeChunk (node.chunk,
                         static_cast<int> (volume_in));

      // add the chunk to our cache
      if (mySoundCache.size () == myCacheSize)
      {
        iterator = mySoundCache.begin ();
        ACE_ASSERT (mySoundCache.size () >= myOldestCacheEntry);
        std::advance (iterator, myOldestCacheEntry);
        // *TODO*: what if it's still being used ?...
        Mix_FreeChunk ((*iterator).chunk);
        mySoundCache.erase (iterator);
        myOldestCacheEntry++;
        if (myOldestCacheEntry == myCacheSize)
          myOldestCacheEntry = 0;
      } // end IF
      mySoundCache.push_back (node);
      iterator = mySoundCache.end (); iterator--;
    } // end IF
    ACE_ASSERT ((*iterator).chunk);

    // compute length
    ACE_UINT64 milliseconds =
        ((*iterator).chunk->alen /
         ((RPG_Sound_Common_Tools::myConfig.format & 0xFF) / 8));
    milliseconds /= RPG_Sound_Common_Tools::myConfig.channels; // <-- frames
    milliseconds *= 1000;
    milliseconds /= RPG_Sound_Common_Tools::myConfig.frequency;
    length_out.msec (static_cast<int> (milliseconds));

    result = (myIsMuted ? 0
                        : Mix_PlayChannel (-1,                // play on the first free channel
                                           (*iterator).chunk, // data
                                           0));               // don't loop
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Mix_PlayChannel(\"%s\"): \"%s\", aborting\n"),
                  ACE_TEXT (file_in.c_str ()),
                  ACE_TEXT (Mix_GetError ())));
  } // end lock scope

  return result;
}

bool
RPG_Sound_Common_Tools::isPlaying (int channel_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Sound_Common_Tools::isPlaying"));

  return static_cast<bool> (Mix_Playing (channel_in));
}

void
RPG_Sound_Common_Tools::stop (int channel_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Sound_Common_Tools::stop"));

  int result = Mix_HaltChannel (channel_in);
  if (result == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Mix_HaltChannel(%d): \"%s\", aborting\n"),
                channel_in,
                ACE_TEXT (Mix_GetError ())));
}

#if defined (SDL_USE)
int
RPG_Sound_Common_Tools::playRandomTrack (SDL_CD* cdrom_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Sound_Common_Tools::playRandomTrack"));

  // sanity check(s)
  ACE_ASSERT (cdrom_in);
  if (cdrom_in->numtracks <= 0)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("no available tracks on CD (drive: \"%s\"), aborting\n"),
                ACE_TEXT (SDL_CDName (cdrom_in->id))));
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
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("no audio tracks on CD (drive: \"%s\"), aborting\n"),
                ACE_TEXT (SDL_CDName (cdrom_in->id))));
    return -1;
  } // end IF

  // choose random track
  RPG_Dice_RollResult_t roll_result;
  do
  {
    roll_result.clear ();
    RPG_Dice::generateRandomNumbers (cdrom_in->numtracks,
                                     1,
                                     roll_result);

    if (cdrom_in->track[roll_result.front () - 1].type == SDL_AUDIO_TRACK)
      break; // not an audio track, try again
  } while (true);

  // play track ?
  int result =
      (myIsMuted ? 0
                 : SDL_CDPlay (cdrom_in,
                               cdrom_in->track[roll_result.front() - 1].offset,
                               cdrom_in->track[roll_result.front() - 1].length));
  if (result == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to play CD track %u (drive: \"%s\"): \"%s\", aborting\n"),
                roll_result.front (),
                ACE_TEXT (SDL_CDName (cdrom_in->id)),
                ACE_TEXT (SDL_GetError ())));
  else
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("playing CD track %u (drive: \"%s\")...\n"),
                roll_result.front (),
                ACE_TEXT (SDL_CDName (cdrom_in->id))));

  return result;
}
#endif // SDL_USE

void
RPG_Sound_Common_Tools::initializeStringConversionTables ()
{
  RPG_TRACE(ACE_TEXT("RPG_Sound_Common_Tools::initializeStringConversionTables"));

  RPG_Sound_CategoryHelper::init ();
  RPG_Sound_EventHelper::init ();

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("RPG_Sound_Common_Tools: initialized string conversion tables...\n")));
}
