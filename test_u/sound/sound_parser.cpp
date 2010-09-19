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

// *NOTE*: need this to import correct VERSION !
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <rpg_sound_dictionary.h>
#include <rpg_sound_common_tools.h>

#include <rpg_dice.h>
#include <rpg_dice_common_tools.h>

#include <rpg_common_macros.h>
#include <rpg_common_tools.h>
#include <rpg_common_file_tools.h>

#include <SDL/SDL.h>

#include <ace/ACE.h>
#include <ace/Log_Msg.h>
#include <ace/Get_Opt.h>
#include <ace/High_Res_Timer.h>

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>

#define SOUNDPARSER_DEF_SOUND_DICTIONARY ACE_TEXT("rpg_sound.xml")
#define SOUNDPARSER_DEF_SOUND_DIRECTORY  ACE_TEXT("./../../sound/data")
#define SOUNDPARSER_DEF_SOUND_CACHESIZE  50

#define SOUNDPARSER_DEF_AUDIO_FREQUENCY  44100
#define SOUNDPARSER_DEF_AUDIO_FORMAT     AUDIO_S16SYS
#define SOUNDPARSER_DEF_AUDIO_CHANNELS   2
#define SOUNDPARSER_DEF_AUDIO_SAMPLES    4096

#define SDL_TIMEREVENT                   SDL_USEREVENT

// *NOTE* types as used by SDL
struct SDL_audio_config_t
{
  int    frequency;
  Uint16 format;
//   Uint8  channels;
  int    channels;
  Uint16 samples;
};

static SDL_CD* cdrom = NULL;

const bool
do_initAudio(const SDL_audio_config_t& config_in)
{
  RPG_TRACE(ACE_TEXT("::do_initAudio"));

  // init SDL Audio

//   SDL_AudioSpec wanted;
//   wanted.freq = config_in.frequency;
//   wanted.format = config_in.format;
//   wanted.channels = config_in.channels;
//   wanted.samples = config_in.samples;
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
               SDL_GetError()));

    return false;
  } // end IF
//   Mix_AllocateChannels(4);
  SDL_audio_config_t obtained;
  obtained.frequency = 0;
  obtained.format = 0;
  obtained.channels = 0;
  obtained.samples = 0;
  if (Mix_QuerySpec(&obtained.frequency,
                    &obtained.format,
                    &obtained.channels) == 0)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to Mix_QuerySpec(): \"%s\", aborting\n"),
               SDL_GetError()));

    return false;
  } // end IF
  char driver[MAXPATHLEN];
  if (!SDL_AudioDriverName(driver, sizeof(driver)))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_AudioDriverName(): \"%s\", aborting\n"),
               SDL_GetError()));

    return false;
  } // end IF

  // initialize audioCD playing
  if (SDL_CDNumDrives() <= 0)
  {
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("failed to SDL_CDNumDrives(): \"%s\", continuing\n"),
               SDL_GetError()));
  } // end IF
  else
  {
    cdrom = SDL_CDOpen(0); // open default drive
    if (!cdrom)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to SDL_CDOpen(0): \"%s\", aborting\n"),
                 SDL_GetError()));

      return false;
    } // end IF
  } // end ELSE

  ACE_DEBUG((LM_INFO,
             ACE_TEXT("*** audio capabilities (driver: \"%s\") ***\nfrequency: %d\nformat: %u\nchannels: %u\nCD [id, status]: \"%s\" [%d, %d]\n"),
             driver,
             obtained.frequency,
             ACE_static_cast(unsigned long, obtained.format),
             ACE_static_cast(unsigned long, obtained.channels),
             SDL_CDName(0),
             (cdrom ? cdrom->id : -1),
             (cdrom ? cdrom->status : -1)));

  return true;
}

Uint32
timer_SDL_cb(Uint32 interval_in,
             void* argument_in)
{
  RPG_TRACE(ACE_TEXT("::timer_SDL_cb"));

  // create an SDL timer event
  SDL_Event event;
  event.type = SDL_TIMEREVENT;
  event.user.data1 = argument_in;

  // push it onto the event queue
  if (SDL_PushEvent(&event))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_PushEvent(): \"%s\", continuing\n"),
               SDL_GetError()));
  } // end IF

  // one-shot timer --> cancel
  return 0;
}

// wait for an input event; stop after timeout_in second(s) (0: wait forever)
void
do_SDL_waitForInput(const unsigned long& timeout_in)
{
  RPG_TRACE(ACE_TEXT("::do_SDL_waitForInput"));

  SDL_TimerID timer = NULL;
  if (timeout_in)
    timer = SDL_AddTimer((timeout_in * 1000), // interval (ms)
                          timer_SDL_cb,       // timeout callback
                          NULL);              // callback argument

  // loop until something interesting happens
  SDL_Event event;
  do
  {
    if (SDL_WaitEvent(&event) != 1)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to SDL_WaitEvent(): \"%s\", aborting\n"),
                 SDL_GetError()));

      // what else can we do ?
      break;
    } // end IF
    if (event.type == SDL_KEYDOWN ||
        event.type == SDL_MOUSEBUTTONDOWN ||
        event.type == SDL_TIMEREVENT)
      break;
  } while (true);

  if (timeout_in)
    if (!SDL_RemoveTimer(timer))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_RemoveTimer(): \"%s\", continuing\n"),
               SDL_GetError()));
  } // end IF
}

void
print_usage(const std::string& programName_in)
{
  RPG_TRACE(ACE_TEXT("::print_usage"));

  std::cout << ACE_TEXT("usage: ") << programName_in << ACE_TEXT(" [OPTIONS]") << std::endl << std::endl;
  std::cout << ACE_TEXT("currently available options:") << std::endl;
  std::cout << ACE_TEXT("-d       : dump dictionary") << std::endl;
  std::cout << ACE_TEXT("-s [FILE]: sound dictionary (*.xml)") << std::endl;
  std::cout << ACE_TEXT("-t       : trace information") << std::endl;
  std::cout << ACE_TEXT("-v       : print version information and exit") << std::endl;
  std::cout << ACE_TEXT("-x       : do NOT validate XML") << std::endl;
} // end print_usage

const bool
process_arguments(const int argc_in,
                  ACE_TCHAR* argv_in[], // cannot be const...
                  bool& dumpDictionary_out,
                  std::string& filename_out,
                  bool& traceInformation_out,
                  bool& printVersionAndExit_out,
                  bool& validateXML_out)
{
  RPG_TRACE(ACE_TEXT("::process_arguments"));

  // init results
  dumpDictionary_out = false;
  filename_out.clear();
  traceInformation_out = false;
  printVersionAndExit_out = false;
  validateXML_out = true;

  ACE_Get_Opt argumentParser(argc_in,
                             argv_in,
                             ACE_TEXT("ds:tvx"));

  int option = 0;
  while ((option = argumentParser()) != EOF)
  {
    switch (option)
    {
      case 'd':
      {
        dumpDictionary_out = true;

        break;
      }
      case 's':
      {
        filename_out = argumentParser.opt_arg();

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
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("unrecognized option \"%s\", aborting\n"),
                   argumentParser.last_option()));

        return false;
      }
      default:
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("unrecognized option \"%c\", aborting\n"),
                   option));

        return false;
      }
    } // end SWITCH
  } // end WHILE

  return true;
}

void
do_work(const std::string& dictionary_in,
        const std::string& path_in,
        const unsigned long& cacheSize_in,
        const bool& validateXML_in,
        const bool& dumpDictionary_in)
{
  RPG_TRACE(ACE_TEXT("::do_work"));

  // step0: init: random seed, string conversion facilities, ...
  RPG_Dice::init();
  RPG_Dice_Common_Tools::initStringConversionTables();
  RPG_Sound_Common_Tools::initStringConversionTables();

  // step1: init: sound directory, cache, ...
  RPG_Sound_Common_Tools::init(path_in,
                               cacheSize_in);

  // step2: init sound dictionary
  try
  {
    RPG_SOUND_DICTIONARY_SINGLETON::instance()->init(dictionary_in,
                                                     validateXML_in);
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in RPG_Sound_Dictionary::init(), returning\n")));

    return;
  }

  // step3: dump sound descriptions
  if (dumpDictionary_in)
  {
    RPG_SOUND_DICTIONARY_SINGLETON::instance()->dump();
  } // end IF

  // step4: play a (random) sound
  RPG_Sound_Event event = RPG_SOUND_EVENT_INVALID;
  RPG_Dice_RollResult_t result;
  RPG_Dice::generateRandomNumbers(RPG_SOUND_EVENT_MAX,
                                  1,
                                  result);
  event = ACE_static_cast(RPG_Sound_Event, (result.front() - 1));
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("playing event sound \"%s\"...\n"),
             RPG_Sound_EventHelper::RPG_Sound_EventToString(event).c_str()));
  RPG_Sound_Common_Tools::playSound(event);

  // step5: wait a little while (max: 5 seconds)
  ACE_OS::sleep(ACE_Time_Value(5, 0));
  // *NOTE*: it seems you cannot SDL_WaitEvent() if there is no window... :-(
//   do_SDL_waitForInput(10);

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("finished working...\n")));
} // end do_work

void
do_printVersion(const std::string& programName_in)
{
  RPG_TRACE(ACE_TEXT("::do_printVersion"));

  std::cout << programName_in << ACE_TEXT(" : ") << VERSION << std::endl;

  // create version string...
  // *IMPORTANT NOTE*: cannot use ACE_VERSION, as it doesn't contain the (potential) beta version
  // number... We need this, as the library soname is compared to this string.
  std::ostringstream version_number;
  if (version_number << ACE::major_version())
  {
    version_number << ACE_TEXT(".");
  } // end IF
  else
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to convert: \"%m\", returning\n")));

    return;
  } // end ELSE
  if (version_number << ACE::minor_version())
  {
    version_number << ACE_TEXT(".");

    if (version_number << ACE::beta_version())
    {

    } // end IF
    else
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to convert: \"%m\", returning\n")));

      return;
    } // end ELSE
  } // end IF
  else
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to convert: \"%m\", returning\n")));

    return;
  } // end ELSE
  std::cout << ACE_TEXT("ACE: ") << version_number.str() << std::endl;
//   std::cout << "ACE: "
//             << ACE_VERSION
//             << std::endl;
}

int
ACE_TMAIN(int argc,
          ACE_TCHAR* argv[])
{
  RPG_TRACE(ACE_TEXT("::main"));

  // step0: init ACE
  // *PORTABILITY*: on Windows, we need to init ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (ACE::init() == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE::init(): \"%m\", aborting\n")));

    return EXIT_FAILURE;
  } // end IF
#endif

  // step1: init
  // step1a set defaults
  bool dumpDictionary        = false;
  std::string filename       = SOUNDPARSER_DEF_SOUND_DICTIONARY;
  bool traceInformation      = false;
  bool printVersionAndExit   = false;
  bool validateXML           = true;

  std::string soundDirectory = SOUNDPARSER_DEF_SOUND_DIRECTORY;
  unsigned long cacheSize    = SOUNDPARSER_DEF_SOUND_CACHESIZE;
  SDL_audio_config_t audio_config;
  audio_config.frequency     = SOUNDPARSER_DEF_AUDIO_FREQUENCY;
  audio_config.format        = SOUNDPARSER_DEF_AUDIO_FORMAT;
  audio_config.channels      = SOUNDPARSER_DEF_AUDIO_CHANNELS;
  audio_config.samples       = SOUNDPARSER_DEF_AUDIO_SAMPLES;

  // step1b: parse/process/validate configuration
  if (!(process_arguments(argc,
                          argv,
                          dumpDictionary,
                          filename,
                          traceInformation,
                          printVersionAndExit,
                          validateXML)))
  {
    // make 'em learn...
    print_usage(std::string(ACE::basename(argv[0])));

    // *PORTABILITY*: on Windows, we must fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini() == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE::fini(): \"%m\", continuing\n")));
#endif

    return EXIT_FAILURE;
  } // end IF

  // step1b: validate arguments
  if (filename.empty())
  {
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("invalid (XML) filename \"%s\", aborting\n"),
               filename.c_str()));

    // make 'em learn...
    print_usage(std::string(ACE::basename(argv[0])));

    // *PORTABILITY*: on Windows, we must fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini() == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE::fini(): \"%m\", continuing\n")));
#endif

    return EXIT_FAILURE;
  } // end IF
  else if (!RPG_Common_File_Tools::isDirectory(soundDirectory))
  {
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("invalid sounds directory \"%s\", aborting\n"),
               soundDirectory.c_str()));

    // *PORTABILITY*: on Windows, we must fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini() == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE::fini(): \"%m\", continuing\n")));
#endif

    return EXIT_FAILURE;
  } // end IF

  // step1c: set correct trace level
  //ACE_Trace::start_tracing();
  if (!traceInformation)
  {
    u_long process_priority_mask = (LM_SHUTDOWN |
                                    //LM_INFO |  // <-- DISABLE trace messages !
                                    //LM_DEBUG |
                                    LM_INFO |
                                    LM_NOTICE |
                                    LM_WARNING |
                                    LM_STARTUP |
                                    LM_ERROR |
                                    LM_CRITICAL |
                                    LM_ALERT |
                                    LM_EMERGENCY);

    // set new mask...
    ACE_LOG_MSG->priority_mask(process_priority_mask,
                               ACE_Log_Msg::PROCESS);

    //ACE_LOG_MSG->stop_tracing();

    // don't go VERBOSE...
    //ACE_LOG_MSG->clr_flags(ACE_Log_Msg::VERBOSE_LITE);
  } // end IF

  // step1d: handle specific program modes
  if (printVersionAndExit)
  {
    do_printVersion(std::string(ACE::basename(argv[0])));

    return EXIT_SUCCESS;
  } // end IF

  // step2a: init SDL
  if (SDL_Init(SDL_INIT_TIMER |
               SDL_INIT_AUDIO |
               SDL_INIT_CDROM |
               SDL_INIT_NOPARACHUTE) == -1) // "...Prevents SDL from catching fatal signals..."
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_Init(): \"%s\", aborting\n"),
               SDL_GetError()));

    // *PORTABILITY*: on Windows, we must fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini() == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE::fini(): \"%m\", continuing\n")));
#endif

    return EXIT_FAILURE;
  } // end IF
  // ***** keyboard setup *****
  // enable Unicode translation
  SDL_EnableUNICODE(1);
  // enable key repeat
  SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY,
                      SDL_DEFAULT_REPEAT_INTERVAL);
//   // ignore keyboard events
//   SDL_EventState(SDL_KEYDOWN, SDL_IGNORE);
//   SDL_EventState(SDL_KEYUP, SDL_IGNORE);

//   // SDL event filter (filter mouse motion events and the like)
//   SDL_SetEventFilter(event_filter_SDL_cb);

  // step2b: init Audio
  if (!do_initAudio(audio_config))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to initialize audio, aborting\n")));

    SDL_Quit();
    // *PORTABILITY*: on Windows, we must fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini() == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE::fini(): \"%m\", continuing\n")));
#endif

    return EXIT_FAILURE;
  } // end IF

  // step3: do actual work
  ACE_High_Res_Timer timer;
  timer.start();
  do_work(filename,
          soundDirectory,
          cacheSize,
          validateXML,
          dumpDictionary);
  timer.stop();
  // debug info
  std::string working_time_string;
  ACE_Time_Value working_time;
  timer.elapsed_time(working_time);
  RPG_Common_Tools::period2String(working_time,
                                  working_time_string);
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("total working time (h:m:s.us): \"%s\"...\n"),
             working_time_string.c_str()));

  // step4a: fini SDL
  SDL_Quit();

  // *PORTABILITY*: on Windows, we must fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (ACE::fini() == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE::fini(): \"%m\", aborting\n")));

    return EXIT_FAILURE;
  } // end IF
#endif

  return EXIT_SUCCESS;
} // end main
