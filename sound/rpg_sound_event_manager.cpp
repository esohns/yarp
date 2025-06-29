/***************************************************************************
 *   Copyright (C) 2010 by Erik Sohns   *
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

#include "rpg_sound_event_manager.h"

#include "ace/Dirent_Selector.h"
#include "ace/Log_Msg.h"

#include "common_file_tools.h"

#include "common_timer_handler.h"
#include "common_timer_manager_common.h"

#include "rpg_common.h"
#include "rpg_common_macros.h"

#include "rpg_dice.h"

#include "rpg_sound_common_tools.h"

int
RPG_Sound_Event_Manager::dirent_selector (const dirent* entry_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Sound_Event_Manager::dirent_selector"));

  // *NOTE*: select samples
  std::string filename (entry_in->d_name);
  std::string extension (ACE_TEXT_ALWAYS_CHAR (RPG_SOUND_DEF_FILE_EXT));
  if (filename.rfind (extension,
                      std::string::npos) != (filename.size () - extension.size ()))
  {
//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("ignoring \"%s\"...\n"),
//                entry_in->d_name));

    return 0;
  } // end IF

  return 1;
}

int
RPG_Sound_Event_Manager::dirent_comparator (const dirent** entry1_in,
                                            const dirent** entry2_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Sound_Event_Manager::dirent_comparator"));

  return ACE_OS::strcmp ((*entry1_in)->d_name,
                         (*entry2_in)->d_name);
}

RPG_Sound_Event_Manager::RPG_Sound_Event_Manager ()
 : inherited (this,  // handler
              false) // one-shot ?
 , myTimerID (-1)
 , myRepository ()
#if defined (SDL_USE)
 , myUseCDROM (RPG_SOUND_AMBIENT_DEF_USE_CD)
 , myCDROM (NULL)
#endif // SDL_USE
 , myTrackOrChannel (-1)
 , myInitialized (false)
{
  RPG_TRACE (ACE_TEXT ("RPG_Sound_Event_Manager::RPG_Sound_Event_Manager"));

}

RPG_Sound_Event_Manager::~RPG_Sound_Event_Manager ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Sound_Event_Manager::~RPG_Sound_Event_Manager"));

  stop ();
  if (myInitialized)
    finalize ();
}

void
RPG_Sound_Event_Manager::initialize (const std::string& repository_in,
                                     bool useCD_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Sound_Event_Manager::initialize"));

  ACE_GUARD (ACE_Thread_Mutex, aGuard, myLock);

  // sanity check(s)
  std::string path = repository_in;
  if (!path.empty ())
  {
    path += ACE_DIRECTORY_SEPARATOR_STR_A;
    path += ACE_TEXT_ALWAYS_CHAR (RPG_SOUND_AMBIENT_SUB);
  } // end IF

  // sanity check(s)
  if (!Common_File_Tools::isDirectory (path))
  {
    // re-try with resolved path
    std::string resolved_path = Common_File_Tools::realPath (path);
    if (!Common_File_Tools::isDirectory (resolved_path))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid argument \"%s\": not a directory, aborting\n"),
                  ACE_TEXT (path.c_str ())));
      return;
    } // end IF
    path = resolved_path;
  } // end IF
  myRepository = path;

#if defined (SDL_USE)
  myUseCDROM = useCD_in;
#endif // SDL_USE
}

void
RPG_Sound_Event_Manager::finalize ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Sound_Event_Manager::finalize"));

  ACE_GUARD (ACE_Thread_Mutex, aGuard, myLock);

  if (myInitialized)
  { 
#if defined (SDL_USE)
    ACE_ASSERT (myCDROM);
    if (SDL_CDStop (myCDROM))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to SDL_CDStop(\"%s\"): \"%s\", continuing\n"),
                  ACE_TEXT (SDL_CDName (myCDROM->id)),
                  ACE_TEXT (SDL_GetError ())));
    SDL_CDClose (myCDROM);
    myCDROM = NULL;
#endif // SDL_USE

    myInitialized = false;
  } // end IF
}

void
RPG_Sound_Event_Manager::start ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Sound_Event_Manager::start"));

  // sanity check: playing ?
  if (myTimerID != -1)
  {
    cancel (true); // locked access ?
    stop ();
  } // end IF
  ACE_ASSERT (myTimerID == -1);

  // *NOTE*: this is a fire-and-forget API (assumes resp. for timer_handler)...
  myTimerID =
    COMMON_TIMERMANAGER_SINGLETON::instance ()->schedule (this,                  // event handler handle
                                                          NULL,                  // ACT
                                                          COMMON_TIME_POLICY (), // expire immediately
                                                          ACE_Time_Value::zero); // one-shot
  if (myTimerID == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to schedule timer, aborting\n")));
    return;
  } // end IF
}

void
RPG_Sound_Event_Manager::stop ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Sound_Event_Manager::stop"));

  ACE_GUARD (ACE_Thread_Mutex, aGuard, myLock);

  cancel (false);

  if (myTrackOrChannel == -1)
    return; // nothing to do...
  else
    RPG_Sound_Common_Tools::stop (myTrackOrChannel);
  myTrackOrChannel = -1;

#if defined (SDL_USE)
  if (myUseCDROM)
  { ACE_ASSERT(myCDROM);
    if (SDL_CDStatus (myCDROM) != CD_STOPPED)
      if (SDL_CDStop (myCDROM))
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to SDL_CDStop(\"%s\"): \"%s\", continuing\n"),
                    ACE_TEXT (SDL_CDName (myCDROM->id)),
                    ACE_TEXT (SDL_GetError ())));
  } // end IF
#endif // SDL_USE
}

bool
RPG_Sound_Event_Manager::isPlaying () const
{
  RPG_TRACE (ACE_TEXT ("RPG_Sound_Event_Manager::isPlaying"));

  ACE_GUARD_RETURN (ACE_Thread_Mutex, aGuard, myLock, false); // *TODO*: remove false negative

#if defined(SDL_USE)
  if (myUseCDROM)
  { ACE_ASSERT (myCDROM);
    return (SDL_CDStatus (myCDROM) == CD_PLAYING);
  } // end IF
#endif // SDL_USE

  if (myTrackOrChannel == -1)
    return false;
  return RPG_Sound_Common_Tools::isPlaying (myTrackOrChannel);
}

void
RPG_Sound_Event_Manager::handle (const void* act_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Sound_Event_Manager::handle"));

  ACE_UNUSED_ARG (act_in);

  ACE_GUARD (ACE_Thread_Mutex, aGuard, myLock);

  // sanity check: initialized ?
#if defined (SDL_USE)
  if (myUseCDROM &&
      !myInitialized)
  {
    initializeCD ();
    ACE_ASSERT(myInitialized);
  } // end IF
#endif // SDL_USE

  // play (another) track/sound...
  ACE_Time_Value length = ACE_Time_Value::max_time;
#if defined (SDL_USE)
  if (myUseCDROM)
  {
    ACE_ASSERT(myCDROM);
    myTrackOrChannel = RPG_Sound_Common_Tools::playRandomTrack (myCDROM);
    if (myTrackOrChannel == -1)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to RPG_Sound_Common_Tools::playRandomTrack, returning")));
      return;
    } // end IF

    // compute length
    int minutes, seconds, frames;
    FRAMES_TO_MSF (myCDROM->track[myTrackOrChannel].length,
                   &minutes, &seconds, &frames);
    // *NOTE*: allow an extra little bit...
    length.set ((minutes * 60) + seconds + RPG_SOUND_CDTRACK_DEF_PAD, 0);
  } // end IF
  else
#endif // SDL_USE
  {
    // retrieve all sample files...
    ACE_Dirent_Selector entries;
    if (entries.open (ACE_TEXT (myRepository.c_str ()),
                      &RPG_Sound_Event_Manager::dirent_selector,
                      &RPG_Sound_Event_Manager::dirent_comparator) == -1)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Dirent_Selector::open(\"%s\"): \"%m\", returning"),
                  ACE_TEXT (myRepository.c_str())));
      return;
    } // end IF
    RPG_Sound_SampleRepository_t sample_repository;
    std::string path;
    for (unsigned int i = 0;
         i < static_cast<unsigned int> (entries.length ());
         i++)
    {
      path = myRepository;
      path += ACE_DIRECTORY_SEPARATOR_STR_A;
      path += ACE_TEXT_ALWAYS_CHAR (entries[i]->d_name);
      sample_repository.push_back (path);
    } // end FOR
    entries.close ();
    if (sample_repository.empty ())
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("no ambient sounds, returning")));
      return;
    } // end IF

    // choose a random file...
    RPG_Sound_SampleRepositoryConstIterator_t iterator =
        sample_repository.begin ();
    RPG_Dice_RollResult_t roll_result;
    RPG_Dice::generateRandomNumbers (static_cast<unsigned int> (sample_repository.size ()),
                                     1,
                                     roll_result);
    std::advance (iterator, roll_result.front () - 1);

    myTrackOrChannel =
      RPG_Sound_Common_Tools::play (*iterator,
                                    RPG_SOUND_AMBIENT_DEF_VOLUME,
                                    length);
    if (myTrackOrChannel == -1)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to RPG_Sound_Common_Tools::play(\"%s\"), returning"),
                  ACE_TEXT ((*iterator).c_str ())));
      return;
    } // end IF
  } // end ELSE
  ACE_ASSERT (length != ACE_Time_Value::max_time);

  // *NOTE*: this is a fire-and-forget API (assumes resp. for timer_handler)...
  myTimerID =
    COMMON_TIMERMANAGER_SINGLETON::instance ()->schedule (this,                           // event handler handle
                                                          NULL,                           // ACT
                                                          COMMON_TIME_POLICY () + length, // wakeup time
                                                          ACE_Time_Value::zero);          // interval: one-shot
  if (myTimerID == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to schedule timer, aborting\n")));
    return;
  } // end IF
}

#if defined (SDL_USE)
void
RPG_Sound_Event_Manager::initializeCD (int drive_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Sound_Event_Manager::initializeCD"));

  if (myInitialized)
    return;

  if (myCDROM)
  {
    if (SDL_CDStop (myCDROM))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to SDL_CDStop(%@): \"%s\", returning\n"),
                  myCDROM,
                  ACE_TEXT (SDL_GetError ())));
      return;
    } // end IF
    SDL_CDClose (myCDROM);
    myCDROM = NULL;
  } // end IF

  if (SDL_CDNumDrives () <= 0)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_CDNumDrives(): \"%s\" (no drive(s) ?), returning\n"),
                ACE_TEXT (SDL_GetError ())));
    return;
  } // end IF

  myCDROM = SDL_CDOpen (drive_in);
  if (!myCDROM)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_CDOpen(\"%s\"): \"%s\", returning\n"),
                ACE_TEXT (SDL_CDName (drive_in)),
                ACE_TEXT (SDL_GetError ())));
    return;
  } // end IF

  CDstatus cd_status = CD_ERROR;
  bool cd_done = false;
  do
  {
    cd_status = SDL_CDStatus (myCDROM);
    switch (cd_status)
    {
      case CD_TRAYEMPTY:
      {
        ACE_DEBUG ((LM_DEBUG,
                    ACE_TEXT ("tray is empty, insert an AudioCD to continue...\n")));

        SDL_CDEject (myCDROM);
        SDL_Delay (10000); // wait 10s

        break;
      }
      case CD_STOPPED:
      {
        // check that there are Audio Tracks on the CD
        unsigned int num_audio_tracks = 0;
        for (int i = 0;
             i < myCDROM->numtracks;
             i++)
          if (myCDROM->track[i].type == SDL_AUDIO_TRACK)
            num_audio_tracks++;

        if (num_audio_tracks == 0)
        {
          ACE_DEBUG ((LM_DEBUG,
                      ACE_TEXT ("no audio tracks, insert an AudioCD to continue...\n")));

          SDL_CDEject (myCDROM);
          SDL_Delay (10000); // wait 10s
        } // end IF
        else
          cd_done = true;

        break;
      }
      case CD_PLAYING:
      case CD_PAUSED:
      {
        if (SDL_CDStop (myCDROM))
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to SDL_CDStop(\"%s\"): \"%s\", returning\n"),
                      ACE_TEXT (SDL_CDName (myCDROM->id)),
                      ACE_TEXT (SDL_GetError ())));
          return;
        } // end IF

        break;
      }
      case CD_ERROR:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("error, insert an AudioCD to continue...\n")));

        SDL_CDEject (myCDROM);
        SDL_Delay (10000); // wait 10s

        break;
      }
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid CD status (was: %d), returning\n"),
                    cd_status));
        return;
      }
    } // end SWITCH

    if (cd_done)
      break;
  } while (true);

  myInitialized = true;
}
#endif // SDL_USE

void
RPG_Sound_Event_Manager::cancel (bool lockedAccess_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Sound_Event_Manager::cancel"));

  if (lockedAccess_in)
    myLock.acquire ();

  // sanity check
  if (myTimerID == -1)
  {
    if (lockedAccess_in)
      myLock.release ();

    return; // nothing to do...
  } // end IF

  // cancel corresponding activation timer
  const void* act_p = NULL;
  if (COMMON_TIMERMANAGER_SINGLETON::instance ()->cancel (myTimerID,
                                                          &act_p) <= 0)
  {
    //ACE_DEBUG((LM_ERROR,
    //           ACE_TEXT("failed to cancel timer (ID: %d), continuing\n"),
    //           myTimerID));
  } // end IF
  else
  {
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("cancelled timer (ID: %d)\n"),
                myTimerID));
  } // end ELSE
  myTimerID = -1;

  if (lockedAccess_in)
    myLock.release ();
}
