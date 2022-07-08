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

#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>

#include "ace/ACE.h"
#include "ace/Get_Opt.h"
#include "ace/High_Res_Timer.h"
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "ace/Init_ACE.h"
#endif // ACE_WIN32 || ACE_WIN64

#include "common_log_tools.h"

#ifdef HAVE_CONFIG_H
#include "rpg_config.h"
#endif

#include "rpg_dice.h"
#include "rpg_dice_common.h"
#include "rpg_dice_common_tools.h"

#include "rpg_common_macros.h"

void
print_usage(const std::string& programName_in)
{
  RPG_TRACE(ACE_TEXT("::print_usage"));

  std::cout << ACE_TEXT("usage: ") << programName_in << ACE_TEXT(" [OPTIONS]") << std::endl << std::endl;
  std::cout << ACE_TEXT("currently available options:") << std::endl;
  std::cout << ACE_TEXT("-p              : print all results") << std::endl;
  std::cout << ACE_TEXT("-r [VALUE-VALUE]: range (e.g. 5-12)") << std::endl;
  std::cout << ACE_TEXT("-t              : trace information") << std::endl;
  std::cout << ACE_TEXT("-v              : print version information and exit") << std::endl;
} // end print_usage

const bool
process_arguments(const int argc_in,
                  ACE_TCHAR* argv_in[], // cannot be const...
                  bool& printAllResults_out,
                  RPG_Dice_ValueRange& valueRange_out,
                  bool& traceInformation_out,
                  bool& printVersionAndExit_out)
{
  RPG_TRACE(ACE_TEXT("::process_arguments"));

  // init results
  printAllResults_out = false;
  valueRange_out.begin = 0;
  valueRange_out.end = 0;
  traceInformation_out = false;
  printVersionAndExit_out = false;

  ACE_Get_Opt argumentParser(argc_in,
                             argv_in,
                             ACE_TEXT_ALWAYS_CHAR("pr:tv"));

  int option = 0;
  while ((option = argumentParser()) != EOF)
  {
    switch (option)
    {
      case 'p':
      {
        printAllResults_out = true;

        break;
      }
      case 'r':
      {
        std::string range = argumentParser.opt_arg ();
        size_t separator = range.find_first_of (ACE_TEXT_ALWAYS_CHAR ("-"), 0);
        if (separator == std::string::npos)
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("invalid range: \"%s\", aborting\n"),
                      ACE_TEXT (range.c_str ())));
          return false;
        } // end IF

        valueRange_out.begin = ::atol (range.substr (0, separator).c_str ());
        valueRange_out.end = ::atol (range.substr (separator + 1, range.length ()).c_str ());
        if (valueRange_out.begin > valueRange_out.end)
        {
          std::swap (valueRange_out.begin, valueRange_out.end);
        } // end IF

//         // debug info
//         ACE_DEBUG((LM_DEBUG,
//                    ACE_TEXT("range begin: %d, end: %d\n"),
//                    valueRange_out.begin,
//                    valueRange_out.end));

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
      // error handling
      case '?':
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("unrecognized option \"%s\", aborting\n"),
                    ACE_TEXT (argumentParser.last_option ())));
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
do_work (const RPG_Dice_ValueRange& valueRange_in,
         RPG_Dice_Rolls_t& rolls_out)
{
  RPG_TRACE (ACE_TEXT ("::do_work"));

  // init result
  rolls_out.clear ();

  // step0: init framework
  RPG_Dice_Common_Tools::initializeStringConversionTables ();

  // step1a: make sure begin <= end
  ACE_ASSERT (valueRange_in.begin <= valueRange_in.end);

  // step1b: basic case ?
  RPG_Dice_Roll result;
  result.numDice = 0;
  result.typeDice = D_0;
  result.modifier = 0;
  if (valueRange_in.begin == valueRange_in.end)
  {
    result.modifier = valueRange_in.begin;
    rolls_out.push_back(result);

    return;
  } // end IF

  // basic algorithm
  RPG_Dice_DieType current_dieType = D_0;
  RPG_Dice_ValueRange current_range;
  do
  {
    current_dieType++;
    if (current_dieType == D_0)
      break;

//     // debug info
//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("current die type: \"%s\"\n"),
//                RPG_Chance_Dice_Common_Tools::diceTypeToString(current_dieType).c_str()));

    result.numDice = 0;
    result.typeDice = current_dieType;
    result.modifier = 0;
    current_range.begin = 0;
    current_range.end = 0;
    for (unsigned int i = 1;
         i <= 100; // *TODO*: incomplete algorithm
         i++)
    {
      result.numDice = i;
      current_range.begin += 1;
      current_range.end += current_dieType;

      if ((valueRange_in.begin - current_range.begin) ==
          (valueRange_in.end - current_range.end))
      {
        // found a match
        result.modifier = (valueRange_in.begin - current_range.begin);
        rolls_out.push_back(result);

        break;
      } // end IF

      if ((current_range.begin > valueRange_in.begin) &&
          (current_range.end > valueRange_in.end))
        break;
    } // end FOR
  } while (true);

  if (!rolls_out.empty())
    return;

  // fallback: try a (rather simplistic) alternate greedy algorithm
  // debug info
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("trying fallback algorithm...\n")));

  // step1c: find SMALLEST type of die LARGER than range.end to start with
  current_dieType = D_100;
  if (valueRange_in.end < D_100)
  {
    while (current_dieType > valueRange_in.end)
    {
      current_dieType--;
    };
    current_dieType++;
  } // end IF

  // find best results for all valid types of dice, starting at this root
  do
  {
//     // debug info
//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("current die type: \"%s\"\n"),
//                RPG_Chance_Dice_Common_Tools::diceTypeToString(current_dieType).c_str()));

    result.numDice = 0;
    result.typeDice = current_dieType;
    result.modifier = 0;
    current_range = valueRange_in;
    do
    {
      if ((current_dieType == D_0) ||
          ((current_range.begin == 0) && (current_range.end == 0))) // handle special (corner) case gracefully...
      {
        // we're (already) at D_0 --> compute (static) modifier
        break;
      } // end IF

      result.numDice++;
      current_range.begin -= 1;
      current_range.end -= current_dieType;
    } while ((current_range.end >= current_dieType) &&
             (current_range.begin != current_range.end)); // <-- this means we're finished !

    // compute the modifier (*IMPORTANT NOTE*: this MAY be negative !)...
    if (current_range.begin == current_range.end) // perfect match !
    {
      result.modifier = current_range.begin;
    } // end IF
    else
    {
      // cannot match the requested range --> approximate
      result.modifier = (std::max<int>(::abs(current_range.begin),
                                       ::abs(current_range.end)) -
                         std::min<int>(::abs(current_range.begin),
                                       ::abs(current_range.end))) / 2;

      if (current_dieType == D_0)
      {
        // add the smaller value
        result.modifier += valueRange_in.begin;
      } // end IF

//       // debug info
//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("approximated modifier: %d\n"),
//                  result.modifier));
    } // end ELSE

    // remember result
    rolls_out.push_back(result);

    // finished ?
    if (current_dieType == D_0)
    {
      // finished !
      break;
    } // end IF
    else
    {
      current_dieType--;
    };
  } while (true);

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("finished working...\n")));
} // end do_work

void
do_printVersion(const std::string& programName_in)
{
  RPG_TRACE(ACE_TEXT("::do_printVersion"));

  std::cout << programName_in
#ifdef HAVE_CONFIG_H
            << ACE_TEXT(" : ")
            //<< YARP_PACKAGE_VERSION
#endif
            << std::endl;

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
    version_number << ".";

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
ACE_TMAIN (int argc_in,
           ACE_TCHAR* argv_in[])
{
  RPG_TRACE (ACE_TEXT ("::main"));

  // step1: initialize libraries
  // *PORTABILITY*: on Windows, init ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (ACE::init () == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE::init(): \"%m\", aborting\n")));
    return EXIT_FAILURE;
  } // end IF
#endif

  // step2: initialize configuration
  
  // step2a: set defaults
  RPG_Dice_ValueRange value_range;
  value_range.begin = 0;
  value_range.end = 0;
  bool print_all_results      = false;
  bool trace_information      = false;
  bool print_version_and_exit = false;

  // step2b: parse/process/validate configuration
  if (!process_arguments (argc_in,
                          argv_in,
                          print_all_results,
                          value_range,
                          trace_information,
                          print_version_and_exit))
  {
    // make 'em learn...
    print_usage (ACE::basename (argv_in[0]));

    // *PORTABILITY*: on Windows, fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini () == -1)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", aborting\n")));
      return EXIT_FAILURE;
    } // end IF
#endif

    return EXIT_FAILURE;
  } // end IF

//   // step2c: validate arguments
//   if (valueRange.begin == valueRange.end == 0)
//   {
//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("invalid range: \"%s\", aborting\n"),
//                RPG_Chance_Dice_Common_Tools::rangeToString(valueRange).c_str()));
//
//     // make 'em learn...
//     print_usage();
//
//     return EXIT_FAILURE;
//   } // end IF

  // step3: initialize logging and/or tracing
  std::string log_file;
  if (!Common_Log_Tools::initializeLogging (ACE::basename (argv_in[0]), // program name
                                            log_file,                   // logfile
                                            false,                      // log to syslog ?
                                            false,                      // trace messages ?
                                            trace_information,          // debug messages ?
                                            NULL))                      // logger
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Tools::initializeLogging(), aborting\n")));

    // *PORTABILITY*: on Windows, need to fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini () == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif

    return EXIT_FAILURE;
  } // end IF

  // step4: handle specific program modes
  if (print_version_and_exit)
  {
    do_printVersion (ACE::basename (argv_in[0]));

    // *PORTABILITY*: on Windows, need to fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini () == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif

    return EXIT_SUCCESS;
  } // end IF

  ACE_High_Res_Timer timer;
  timer.start ();

  // step5: do work
  RPG_Dice_Rolls_t combinations;
  do_work (value_range,
           combinations);

  timer.stop ();

  // step6: print results
  // header line
  std::cout << ACE_TEXT ("results ")
            << RPG_Dice_Common_Tools::toString (value_range)
            << ACE_TEXT (": ")
            << std::endl;
  std::cout << std::setw (80)
            << std::setfill (ACE_TEXT_ALWAYS_CHAR ('-'))
            << ACE_TEXT ("")
            << std::setfill (ACE_TEXT_ALWAYS_CHAR (' '))
            << std::endl;

  bool perfect_match = true;
  int index = 1;
  RPG_Dice_ValueRange range;
  for (RPG_Dice_RollsIterator_t iterator = combinations.begin ();
       iterator != combinations.end ();
       iterator++, index++)
  {
    perfect_match = true;
    RPG_Dice::rollToRange (*iterator, range);
    if ((range.begin != value_range.begin) || (range.end != value_range.end))
    {
      // not a perfect match...
      perfect_match = false;
      if (!print_all_results)
        continue;
    } // end IF

    std::cout << ACE_TEXT ("[")
              << index
              << (perfect_match ? ACE_TEXT ("*]") : ACE_TEXT ("]"))
              << RPG_Dice_Common_Tools::toString (*iterator)
              << ACE_TEXT (" : ")
              << RPG_Dice_Common_Tools::toString (range)
              << std::endl;
  } // end FOR

//   std::string working_time_string;
//   ACE_Time_Value working_time;
//   timer.elapsed_time(working_time);
//   RPS_FLB_Common_Tools::Period2String(working_time,
//                                       working_time_string);
  //
//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("total working time (h:m:s.us): \"%s\"...\n"),
//              working_time_string.c_str()));

  // step7: finalize libraries
  // *PORTABILITY*: on Windows, fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (ACE::fini () == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE::fini(): \"%m\", aborting\n")));
    return EXIT_FAILURE;
  } // end IF
#endif

  return EXIT_SUCCESS;
} // end main
