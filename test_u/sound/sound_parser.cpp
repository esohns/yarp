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

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#define _SDL_main_h
#define SDL_main_h_
#include "SDL.h"

#include "ace/ACE.h"
#include "ace/Get_Opt.h"
#include "ace/High_Res_Timer.h"
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "ace/Init_ACE.h"
#endif // ACE_WIN32 || ACE_WIN64
#include "ace/Log_Msg.h"

#include "common_file_tools.h"

#include "common_log_tools.h"

#include "common_timer_tools.h"

#if defined (HAVE_CONFIG_H)
#include "rpg_config.h"
#endif // HAVE_CONFIG_H

#include "rpg_dice.h"
#include "rpg_dice_common_tools.h"

#include "rpg_common_defines.h"
#include "rpg_common_file_tools.h"
#include "rpg_common_macros.h"
#include "rpg_common_XML_tools.h"

#include "rpg_sound_common.h"
#include "rpg_sound_common_tools.h"
#include "rpg_sound_defines.h"
#include "rpg_sound_dictionary.h"

#include "rpg_engine_defines.h"

#define SOUNDPARSER_DEF_PLAY_RANDOM_SOUNDS false
#define SDL_TIMEREVENT                     SDL_USEREVENT

//static SDL_CD* cdrom = NULL;

// ******* WORKAROUND *************
#if defined (ACE_WIN32) || defined (ACE_WIN64)
FILE _iob[] = { *stdin, *stdout, *stderr };
extern "C" FILE* __cdecl __iob_func (void)
{
  return _iob;
}
#endif // ACE_WIN32 || ACE_WIN64
// ********************************

Uint32
timer_SDL_cb (Uint32 interval_in,
              void* argument_in)
{
  RPG_TRACE (ACE_TEXT ("::timer_SDL_cb"));

  // create an SDL timer event
  union SDL_Event sdl_event;
  sdl_event.type = SDL_TIMEREVENT;
  sdl_event.user.data1 = argument_in;

  // push it onto the event queue
  if (SDL_PushEvent (&sdl_event))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_PushEvent(): \"%s\", continuing\n"),
                ACE_TEXT (SDL_GetError ())));

  // one-shot timer --> cancel
  return 0;
}

// wait for an input event; stop after timeout_in second(s) (0: wait forever)
void
do_SDL_waitForInput (unsigned int timeout_in)
{
  RPG_TRACE (ACE_TEXT ("::do_SDL_waitForInput"));

  SDL_TimerID timer = 0;
  if (timeout_in)
    timer = SDL_AddTimer ((timeout_in * 1000), // interval (ms)
                          timer_SDL_cb,       // timeout callback
                          NULL);              // callback argument

  // loop until something interesting happens
  union SDL_Event sdl_event;
  do
  {
    if (SDL_WaitEvent (&sdl_event) != 1)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to SDL_WaitEvent(): \"%s\", returning\n"),
                  ACE_TEXT (SDL_GetError ())));

      break;
    } // end IF
    if (sdl_event.type == SDL_KEYDOWN         ||
        sdl_event.type == SDL_MOUSEBUTTONDOWN ||
        sdl_event.type == SDL_TIMEREVENT)
      break;
  } while (true);

  if (timeout_in)
    if (!SDL_RemoveTimer (timer))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_RemoveTimer(): \"%s\", continuing\n"),
                ACE_TEXT (SDL_GetError ())));
  } // end IF
}

void
do_printUsage (const std::string& programName_in)
{
  RPG_TRACE (ACE_TEXT ("::do_printUsage"));

  // enable verbatim boolean output
  std::cout.setf (ios::boolalpha);

  std::string configuration_path =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (yarp_PACKAGE_NAME),
                                                          ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_TEST_U_SUBDIRECTORY),
                                                          ACE_TEXT_ALWAYS_CHAR (""),
                                                          true);
  std::string data_path =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (yarp_PACKAGE_NAME),
                                                          ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_TEST_U_SUBDIRECTORY),
                                                          ACE_TEXT_ALWAYS_CHAR (""),
                                                          false);

  std::cout << ACE_TEXT_ALWAYS_CHAR ("usage: ") 
            << programName_in
            << ACE_TEXT_ALWAYS_CHAR (" [OPTIONS]")
            << std::endl
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("currently available options:")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-d       : dump dictionary") << std::endl;
  std::string path = data_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  path += ACE_TEXT_ALWAYS_CHAR ("sound");
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path += ACE_TEXT_ALWAYS_CHAR (RPG_COMMON_DATA_SUB);
#else
  path += ACE_TEXT_ALWAYS_CHAR (RPG_SOUND_DATA_SUB);
#endif // DEBUG_DEBUGGER
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-f [DIR] : data directory")
            << ACE_TEXT_ALWAYS_CHAR (" [\"")
            << path.c_str ()
            << ACE_TEXT_ALWAYS_CHAR ("\"]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-r       : play random sounds")
            << ACE_TEXT_ALWAYS_CHAR (" [")
            << SOUNDPARSER_DEF_PLAY_RANDOM_SOUNDS
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  path = configuration_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  path += ACE_TEXT_ALWAYS_CHAR ("sound");
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif // DEBUG_DEBUGGER
  path += ACE_TEXT_ALWAYS_CHAR (RPG_SOUND_DICTIONARY_FILE);
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-s [FILE]: sound dictionary (*.xml)")
            << ACE_TEXT_ALWAYS_CHAR (" [\"")
            << path
            << ACE_TEXT_ALWAYS_CHAR ("\"]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-t       : trace information")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-v       : print version information and exit")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-x       : do NOT validate XML")
            << std::endl;
} // end print_usage

bool
do_processArguments (int argc_in,
                     ACE_TCHAR* argv_in[], // cannot be const...
                     bool& dumpDictionary_out,
                     std::string& directory_out,
                     bool& playRandomSounds_out,
                     std::string& filename_out,
                     bool& traceInformation_out,
                     bool& printVersionAndExit_out,
                     bool& validateXML_out)
{
  RPG_TRACE (ACE_TEXT ("::do_processArguments"));

  // initialize results
  std::string configuration_path =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (yarp_PACKAGE_NAME),
                                                          ACE_TEXT_ALWAYS_CHAR (""),
                                                          ACE_TEXT_ALWAYS_CHAR (RPG_SOUND_SUB_DIRECTORY_STRING),
                                                          true);
  std::string data_path =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (yarp_PACKAGE_NAME),
                                                          ACE_TEXT_ALWAYS_CHAR (""),
                                                          ACE_TEXT_ALWAYS_CHAR (RPG_SOUND_SUB_DIRECTORY_STRING),
                                                          false);

  dumpDictionary_out = false;

  directory_out = data_path;

  playRandomSounds_out = SOUNDPARSER_DEF_PLAY_RANDOM_SOUNDS;

  filename_out = configuration_path;
  filename_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  filename_out += ACE_TEXT_ALWAYS_CHAR (RPG_SOUND_DICTIONARY_FILE);

  traceInformation_out = false;
  printVersionAndExit_out = false;
  validateXML_out = true;

  ACE_Get_Opt argumentParser (argc_in,
                              argv_in,
                              ACE_TEXT ("df:rs:tvx"));

  int option = 0;
  while ((option = argumentParser ()) != EOF)
  {
    switch (option)
    {
      case 'd':
      {
        dumpDictionary_out = true;

        break;
      }
      case 'f':
      {
        directory_out = ACE_TEXT_ALWAYS_CHAR (argumentParser.opt_arg ());

        break;
      }
      case 'r':
      {
        playRandomSounds_out = true;

        break;
      }
      case 's':
      {
        filename_out = ACE_TEXT_ALWAYS_CHAR (argumentParser.opt_arg ());

        break;
      }
      case 't':
      {
        traceInformation_out = true;

        break;
      }
      case 'v':
      {
        printVersionAndExit_out = true;

        break;
      }
      case 'x':
      {
        validateXML_out = false;

        break;
      }
      // error handling
      case '?':
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("unrecognized option \"%s\", aborting\n"),
                    argumentParser.last_option ()));
        return false;
      }
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("unrecognized option \"%c\", aborting\n"),
                    option));
        return false;
      }
    } // end SWITCH
  } // end WHILE

  return true;
}

void
do_work (bool dumpDictionary_in,
         const std::string& path_in,
         bool playRandomSounds_in,
         const std::string& schemaRepository_in,
         const std::string& dictionary_in,
         bool validateXML_in)
{
  RPG_TRACE (ACE_TEXT ("::do_work"));

  // step0: initialize: random seed, string conversion facilities, ...
  RPG_Dice::initialize ();
  RPG_Dice_Common_Tools::initializeStringConversionTables ();

  // step1: initialize: sound directory, cache, ...
  struct RPG_Sound_SDLConfiguration sound_configuration;
  sound_configuration.frequency = RPG_SOUND_AUDIO_DEF_FREQUENCY;
  sound_configuration.format = RPG_SOUND_AUDIO_DEF_FORMAT;
  sound_configuration.channels = RPG_SOUND_AUDIO_DEF_CHANNELS;
  sound_configuration.chunksize = RPG_SOUND_AUDIO_DEF_CHUNKSIZE;
  if (!RPG_Sound_Common_Tools::initialize (sound_configuration,
                                           path_in,
                                           RPG_SOUND_AMBIENT_DEF_USE_CD,
                                           RPG_SOUND_DEF_CACHESIZE,
                                           false))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to RPG_Sound_Common_Tools::initialize(), returning\n")));
    return;
  } // end IF

  // step2: initialize sound dictionary
  std::string schema_repository_string = schemaRepository_in;
  schema_repository_string += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  schema_repository_string += ACE_TEXT_ALWAYS_CHAR (RPG_ENGINE_SUB_DIRECTORY_STRING);
  schema_repository_string += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  schema_repository_string += ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_CONFIGURATION_SUBDIRECTORY);
  std::vector<std::string> schema_directories_a;
  schema_directories_a.push_back (schema_repository_string);
  if (!RPG_Common_XML_Tools::initialize (schema_directories_a))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to RPG_Common_XML_Tools::initialize(), returning\n")));
    return;
  } // end IF
  if (!RPG_SOUND_DICTIONARY_SINGLETON::instance ()->initialize (dictionary_in,
                                                                validateXML_in))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to RPG_Sound_Dictionary::initialize(), returning\n")));
    return;
  }

  // step3: dump sound descriptions
  if (dumpDictionary_in)
    RPG_SOUND_DICTIONARY_SINGLETON::instance ()->dump ();

  if (!playRandomSounds_in)
  {
    // clean up
    RPG_Common_XML_Tools::finalize ();

    return;
  } // end IF

  // step4: play (random) sounds...
  RPG_Sound_Event sound_event = RPG_SOUND_EVENT_INVALID;
  RPG_Dice_RollResult_t result;
  int current_channel = -1;
  ACE_Time_Value duration = ACE_Time_Value::zero;
  do
  {
    result.clear ();
    RPG_Dice::generateRandomNumbers (RPG_SOUND_EVENT_MAX,
                                     1,
                                     result);
    sound_event = static_cast<RPG_Sound_Event> ((result.front () - 1));
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("playing event sound \"%s\"...\n"),
                ACE_TEXT (RPG_Sound_EventHelper::RPG_Sound_EventToString (sound_event).c_str ())));

    current_channel = RPG_Sound_Common_Tools::play (sound_event, duration);
    if (current_channel == -1)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to play event sound \"%s\", aborting\n"),
                  ACE_TEXT (RPG_Sound_EventHelper::RPG_Sound_EventToString (sound_event).c_str ())));

      break;
    } // end IF

    // stop after some time (5 seconds)
    ACE_OS::sleep (ACE_Time_Value (5, 0));
    if (RPG_Sound_Common_Tools::isPlaying (current_channel))
      RPG_Sound_Common_Tools::stop (current_channel);
  } while (true);

  // *NOTE*: it seems you cannot SDL_WaitEvent() if there is no window... :-(
//   do_SDL_waitForInput(10);

  // clean up
  RPG_Common_XML_Tools::finalize ();

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("finished working...\n")));
} // end do_work

void
do_printVersion (const std::string& programName_in)
{
  RPG_TRACE (ACE_TEXT ("::do_printVersion"));

  std::cout << programName_in
#if defined (HAVE_CONFIG_H)
            << ACE_TEXT_ALWAYS_CHAR (" : ")
            << yarp_PACKAGE_VERSION
#endif // HAVE_CONFIG_H
            << std::endl;

  // create version string...
  // *NOTE*: cannot use ACE_VERSION, as it doesn't contain the (potential) beta version
  // number... We need this, as the library soname is compared to this string.
  std::ostringstream version_number;
  if (version_number << ACE::major_version ())
  {
    version_number << ACE_TEXT_ALWAYS_CHAR (".");
  } // end IF
  else
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to convert: \"%m\", returning\n")));
    return;
  } // end ELSE
  if (version_number << ACE::minor_version ())
  {
    version_number << ACE_TEXT_ALWAYS_CHAR (".");

    if (version_number << ACE::beta_version ())
    {

    } // end IF
    else
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to convert: \"%m\", returning\n")));
      return;
    } // end ELSE
  } // end IF
  else
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to convert: \"%m\", returning\n")));
    return;
  } // end ELSE
  std::cout << ACE_TEXT_ALWAYS_CHAR ("ACE: ")
            << version_number.str ()
            << std::endl;
  //   std::cout << "ACE: "
//             << ACE_VERSION
//             << std::endl;
}

int
ACE_TMAIN (int argc_in,
           ACE_TCHAR* argv_in[])
{
  RPG_TRACE (ACE_TEXT ("::main"));

  // step0: initialize ACE
  // *PORTABILITY*: on Windows, init ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (ACE::init () == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE::init(): \"%m\", aborting\n")));
    return EXIT_FAILURE;
  } // end IF
#endif // ACE_WIN32 || ACE_WIN64

  // step1: initialize
  // step1a set defaults
  Common_File_Tools::initialize (ACE_TEXT_ALWAYS_CHAR (argv_in[0]));
  std::string configuration_path =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (yarp_PACKAGE_NAME),
                                                          ACE_TEXT_ALWAYS_CHAR (""),
                                                          ACE_TEXT_ALWAYS_CHAR (RPG_SOUND_SUB_DIRECTORY_STRING),
                                                          true);
  std::string data_path =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (yarp_PACKAGE_NAME),
                                                          ACE_TEXT_ALWAYS_CHAR (""),
                                                          ACE_TEXT_ALWAYS_CHAR (RPG_SOUND_SUB_DIRECTORY_STRING),
                                                          false);

  bool dump_dictionary = false;

  std::string sound_directory = data_path;

  bool play_random_sounds = SOUNDPARSER_DEF_PLAY_RANDOM_SOUNDS;

  std::string schema_repository = Common_File_Tools::getWorkingDirectory ();

  std::string filename = configuration_path;
  filename += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  filename += ACE_TEXT_ALWAYS_CHAR (RPG_SOUND_DICTIONARY_FILE);

  bool trace_information = false;
  bool print_version_and_exit = false;
  bool validate_XML = true;

//  RPG_Sound_SDLConfig_t audio_config;
//  audio_config.frequency = RPG_SOUND_DEF_AUDIO_FREQUENCY;
//  audio_config.format    = RPG_SOUND_DEF_AUDIO_FORMAT;
//  audio_config.channels  = RPG_SOUND_DEF_AUDIO_CHANNELS;
//  audio_config.chunksize = RPG_SOUND_DEF_AUDIO_CHUNKSIZE;

  // step1b: parse/process/validate configuration
  if (!do_processArguments (argc_in,
                            argv_in,
                            dump_dictionary,
                            sound_directory,
                            play_random_sounds,
                            filename,
                            trace_information,
                            print_version_and_exit,
                            validate_XML))
  {
    // make 'em learn...
    do_printUsage (ACE_TEXT_ALWAYS_CHAR (ACE::basename (argv_in[0])));

    // *PORTABILITY*: on Windows, fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini () == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif // ACE_WIN32 || ACE_WIN64

    return EXIT_FAILURE;
  } // end IF

  // step1b: validate arguments
  if ((play_random_sounds &&
       !Common_File_Tools::isDirectory (sound_directory)) ||
      !Common_File_Tools::isReadable (filename))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid argument, aborting\n")));

    // make 'em learn...
    do_printUsage (ACE_TEXT_ALWAYS_CHAR (ACE::basename (argv_in[0])));

    // *PORTABILITY*: on Windows, fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini () == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif // ACE_WIN32 || ACE_WIN64

    return EXIT_FAILURE;
  } // end IF

  // step1c: initialize logging and/or tracing
  std::string log_file;
  if (!Common_Log_Tools::initializeLogging (ACE_TEXT_ALWAYS_CHAR (ACE::basename (argv_in[0])), // program name
                                            log_file,                  // logfile
                                            false,                     // log to syslog ?
                                            false,                     // trace messages ?
                                            trace_information,         // debug messages ?
                                            NULL))                     // logger
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Log_Tools::initializeLogging(), aborting\n")));

    // *PORTABILITY*: on Windows, fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini () == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", aborting\n")));
#endif // ACE_WIN32 || ACE_WIN64

    return EXIT_FAILURE;
  } // end IF

  // step1d: handle specific program modes
  if (print_version_and_exit)
  {
    do_printVersion (ACE_TEXT_ALWAYS_CHAR (ACE::basename (argv_in[0])));

    return EXIT_SUCCESS;
  } // end IF

  // step2: initialize SDL
  Uint32 SDL_init_flags = SDL_INIT_TIMER |
                          SDL_INIT_AUDIO |
                          SDL_INIT_NOPARACHUTE; // "...Prevents SDL from catching fatal signals..."
#if defined (SDL_USE)
  SDL_init_flags |= SDL_INIT_CDROM;
#endif // SDL_USE
  if (SDL_Init (SDL_init_flags) == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_Init(0x%x): \"%s\", aborting\n"),
                SDL_init_flags,
                ACE_TEXT (SDL_GetError ())));

    // *PORTABILITY*: on Windows, we must fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini () == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif // ACE_WIN32 || ACE_WIN64

    return EXIT_FAILURE;
  } // end IF
  // ***** keyboard setup *****
  // enable Unicode translation

#if defined (SDL_USE)
  SDL_EnableUNICODE (1);
  // enable key repeat
  SDL_EnableKeyRepeat (SDL_DEFAULT_REPEAT_DELAY,
                       SDL_DEFAULT_REPEAT_INTERVAL);
#endif // SDL_USE
  //   // ignore keyboard events
//   SDL_EventState(SDL_KEYDOWN, SDL_IGNORE);
//   SDL_EventState(SDL_KEYUP, SDL_IGNORE);

//   // SDL event filter (filter mouse motion events and the like)
//   SDL_SetEventFilter(event_filter_SDL_cb);

  // step3: do actual work
  ACE_High_Res_Timer timer;
  timer.start ();
  do_work (dump_dictionary,
           sound_directory,
           play_random_sounds,
           schema_repository,
           filename,
           validate_XML);
  timer.stop ();
  // debug info
  ACE_Time_Value working_time;
  timer.elapsed_time (working_time);
  std::string working_time_string =
    Common_Timer_Tools::periodToString (working_time);
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("total working time (h:m:s.us): \"%s\"...\n"),
              ACE_TEXT (working_time_string.c_str ())));

  // step4a: finalize SDL
  SDL_Quit ();

  // *PORTABILITY*: on Windows, fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (ACE::fini () == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE::fini(): \"%m\", aborting\n")));
    return EXIT_FAILURE;
  } // end IF
#endif // ACE_WIN32 || ACE_WIN64

  return EXIT_SUCCESS;
} // end main
