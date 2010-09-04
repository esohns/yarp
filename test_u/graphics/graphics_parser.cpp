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

// *NOTE*: need this to import correct PACKAGE_STRING/VERSION/... !
#ifdef HAVE_CONFIG_H
#include <test_u-config.h>
#endif

#include <rpg_graphics_dictionary.h>
#include <rpg_graphics_common_tools.h>

#include <rpg_common_tools.h>
#include <rpg_common_file_tools.h>

#include <rpg_dice.h>
#include <rpg_dice_common_tools.h>

#include <ace/ACE.h>
#include <ace/Log_Msg.h>
#include <ace/Get_Opt.h>
#include <ace/High_Res_Timer.h>

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>

#define GRAPHICSPARSER_DEF_GRAPHICS_DICTIONARY ACE_TEXT("rpg_graphics.xml")

void
print_usage(const std::string& programName_in)
{
  ACE_TRACE(ACE_TEXT("::print_usage"));

  std::cout << ACE_TEXT("usage: ") << programName_in << ACE_TEXT(" [OPTIONS]") << std::endl << std::endl;
  std::cout << ACE_TEXT("currently available options:") << std::endl;
  std::cout << ACE_TEXT("-d       : dump dictionary") << std::endl;
  std::cout << ACE_TEXT("-g [FILE]: graphics dictionary (*.xml)") << std::endl;
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
  ACE_TRACE(ACE_TEXT("::process_arguments"));

  // init results
  dumpDictionary_out = false;
  filename_out.clear();
  traceInformation_out = false;
  printVersionAndExit_out = false;
  validateXML_out = true;

  ACE_Get_Opt argumentParser(argc_in,
                             argv_in,
                             ACE_TEXT("dg:tvx"));

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
      case 'g':
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
        const bool& validateXML_in,
        const bool& dumpDictionary_in)
{
  ACE_TRACE(ACE_TEXT("::do_work"));

  // step0: init: random seed, string conversion facilities, ...
  RPG_Dice::init();
  RPG_Dice_Common_Tools::initStringConversionTables();
  RPG_Graphics_Common_Tools::initStringConversionTables();

  // step1a: init graphics dictionary
  try
  {
    RPG_GRAPHICS_DICTIONARY_SINGLETON::instance()->init(dictionary_in,
                                                        validateXML_in);
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in RPG_Graphics_Dictionary::init(), returning\n")));

    return;
  }

  // step2: dump graphics descriptions
  if (dumpDictionary_in)
  {
    RPG_GRAPHICS_DICTIONARY_SINGLETON::instance()->dump();
  } // end IF

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("finished working...\n")));
} // end do_work

void
do_printVersion(const std::string& programName_in)
{
  ACE_TRACE(ACE_TEXT("::do_printVersion"));

//   std::cout << programName_in << ACE_TEXT(" : ") << VERSION << std::endl;
  std::cout << programName_in << ACE_TEXT(" : ") << TEST_U_VERSION << std::endl;

  // create version string...
  // *NOTE*: cannot use ACE_VERSION, as it doesn't contain the (potential) beta version
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
  ACE_TRACE(ACE_TEXT("::main"));

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
  bool dumpDictionary            = false;
  std::string filename           = GRAPHICSPARSER_DEF_GRAPHICS_DICTIONARY;
  bool traceInformation          = false;
  bool printVersionAndExit       = false;
  bool validateXML               = true;

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

  // step2: do actual work
  ACE_High_Res_Timer timer;
  timer.start();
  do_work(filename,
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
  TTF_Quit();
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
