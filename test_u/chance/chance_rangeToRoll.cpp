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
#include <rpg_config.h>
#endif

#include <rpg_dice.h>
#include <rpg_dice_common.h>
#include <rpg_dice_common_tools.h>

#include <rpg_common_macros.h>

#include <ace/ACE.h>
#include <ace/Get_Opt.h>
#include <ace/High_Res_Timer.h>

#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>

void print_usage(const std::string& programName_in)
{
  RPG_TRACE(ACE_TEXT("::print_usage"));

  std::cout << ACE_TEXT("usage: ") << programName_in << ACE_TEXT(" [OPTIONS]") << std::endl << std::endl;
  std::cout << ACE_TEXT("currently available options:") << std::endl;
  std::cout << ACE_TEXT("-p              : print all results") << std::endl;
  std::cout << ACE_TEXT("-r [VALUE-VALUE]: range (e.g. 5-12)") << std::endl;
  std::cout << ACE_TEXT("-t              : trace information") << std::endl;
  std::cout << ACE_TEXT("-v              : print version information and exit") << std::endl;
} // end print_usage

const bool process_arguments(const int argc_in,
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
        std::string range = argumentParser.opt_arg();
        unsigned int separator = range.find_first_of(ACE_TEXT_ALWAYS_CHAR("-"), 0);
        if (separator == std::string::npos)
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("invalid range: \"%s\", aborting\n"),
                     range.c_str()));

          return false;
        } // end IF

        valueRange_out.begin = ::atol(range.substr(0, separator).c_str());
        valueRange_out.end = ::atol(range.substr(separator + 1, range.length()).c_str());
        if (valueRange_out.begin > valueRange_out.end)
        {
          std::swap(valueRange_out.begin, valueRange_out.end);
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

void do_work(const RPG_Dice_ValueRange& valueRange_in,
             RPG_Dice_Rolls_t& rolls_out)
{
  RPG_TRACE(ACE_TEXT("::do_work"));

  // init result
  rolls_out.clear();

  // step0: init framework
  RPG_Dice_Common_Tools::initStringConversionTables();

  // we use a (rather simplistic) greedy algorithm to compute this
  // step1a: make sure begin <= end
  ACE_ASSERT(valueRange_in.begin <= valueRange_in.end);
  // step1b: find SMALLEST type of die LARGER than range.end to start with
  RPG_Dice_DieType current_dieType = D_100;
  if (valueRange_in.end < D_100)
  {
    while (current_dieType > valueRange_in.end)
    {
      current_dieType--;
    };
    current_dieType++;
  } // end IF

  // step2: find best results for all valid types of dice, starting at this root
  do
  {
//     // debug info
//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("current die type: \"%s\"\n"),
//                RPG_Chance_Dice_Common_Tools::diceTypeToString(current_dieType).c_str()));

    RPG_Dice_Roll result;
    result.numDice = 0;
    result.typeDice = current_dieType;
    result.modifier = 0;
    RPG_Dice_ValueRange range = valueRange_in;
    do
    {
      if ((current_dieType == D_0) ||
          ((range.begin == 0) && (range.end == 0))) // handle special (corner) case gracefully...
      {
        // we're (already) at D_0 --> compute (static) modifier
        break;
      } // end IF

      result.numDice++;
      range.begin -= 1;
      range.end -= current_dieType;
    } while ((range.end >= current_dieType) &&
             (range.begin != range.end)); // <-- this means we're finished !

    // compute the modifier (*IMPORTANT NOTE*: this MAY be negative !)...
    if (range.begin == range.end) // perfect match !
    {
      result.modifier = range.begin;
    } // end IF
    else
    {
      // cannot match the requested range --> approximate
      result.modifier = (std::max(::abs(range.begin),
                                  ::abs(range.end)) -
                         std::min(::abs(range.begin),
                                  ::abs(range.end))) / 2;

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
      << ACE_TEXT(" : ")
      << RPG_VERSION
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
               ACE_TEXT("failed to convert: \"%s\", returning\n"),
               ACE_OS::strerror(errno)));

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
                 ACE_TEXT("failed to convert: \"%s\", returning\n"),
                 ACE_OS::strerror(errno)));

      return;
    } // end ELSE
  } // end IF
  else
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to convert: \"%s\", returning\n"),
               ACE_OS::strerror(errno)));

    return;
  } // end ELSE
  std::cout << ACE_TEXT("ACE: ") << version_number.str() << std::endl;
//   std::cout << "ACE: "
//             << ACE_VERSION
//             << std::endl;
}

int ACE_TMAIN(int argc,
              ACE_TCHAR* argv[])
{
  RPG_TRACE(ACE_TEXT("::main"));

  // step1: init
  // step1a set defaults
  RPG_Dice_ValueRange valueRange;
  valueRange.begin = 0;
  valueRange.end = 0;
  bool printAllResults     = false;
  bool traceInformation    = false;
  bool printVersionAndExit = false;

  // step1b: parse/process/validate configuration
  if (!(process_arguments(argc,
                          argv,
                          printAllResults,
                          valueRange,
                          traceInformation,
                          printVersionAndExit)))
  {
    // make 'em learn...
    print_usage(std::string(ACE::basename(argv[0])));

    return EXIT_FAILURE;
  } // end IF

//   // step1b: validate arguments
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

  ACE_High_Res_Timer timer;
  timer.start();

  // step2: do actual work
  RPG_Dice_Rolls_t combinations;
  do_work(valueRange,
          combinations);

  timer.stop();

  // print results
  // header line
  std::cout << ACE_TEXT("results ") << RPG_Dice_Common_Tools::rangeToString(valueRange) << ACE_TEXT(": ") << std::endl;
  std::cout << std::setw(80) << std::setfill(ACE_TEXT_ALWAYS_CHAR('-')) << ACE_TEXT("") << std::setfill(ACE_TEXT_ALWAYS_CHAR(' ')) << std::endl;

  bool perfect_match = true;
  int index = 1;
  RPG_Dice_ValueRange range;
  for (RPG_Dice_RollsIterator_t iterator = combinations.begin();
       iterator != combinations.end();
       iterator++, index++)
  {
    perfect_match = true;
    RPG_Dice::rollToRange(*iterator, range);
    if ((range.begin != valueRange.begin) || (range.end != valueRange.end))
    {
      // not a perfect match...
      perfect_match = false;
      if (!printAllResults)
      {
        continue;
      } // end IF
    } // end IF

    std::cout << ACE_TEXT("[") << index << (perfect_match ? ACE_TEXT("*]") : ACE_TEXT("]")) << RPG_Dice_Common_Tools::rollToString(*iterator) << ACE_TEXT(" : ") << RPG_Dice_Common_Tools::rangeToString(range) << std::endl;
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

  return EXIT_SUCCESS;
} // end main
