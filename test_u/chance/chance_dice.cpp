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

// *NOTE*: need this to import correct VERSION !
#ifdef HAVE_CONFIG_H
#include "rpg_config.h"
#endif

#include "rpg_dice_dietype.h"
#include "rpg_dice_roll.h"
#include "rpg_dice.h"

#include "rpg_common_macros.h"

#include "ace/ACE.h"
#include "ace/Get_Opt.h"
#include "ace/High_Res_Timer.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <algorithm>

void
print_usage(const std::string& programName_in)
{
  RPG_TRACE(ACE_TEXT("::print_usage"));

  std::cout << ACE_TEXT("usage: ") << programName_in << ACE_TEXT(" [OPTIONS]") << std::endl << std::endl;
  std::cout << ACE_TEXT("currently available options:") << std::endl;
//   std::cout << ACE_TEXT("-d [VALUE]: type of dice (# sides: 0, 2, 3, 4, 6, 8, 10, 12, 20 or 100); default: 6") << std::endl;
  std::cout << ACE_TEXT("-d [VALUE]: type of dice (# sides: 4, 6, 8, 10, 12, 20 or 100); default: 6") << std::endl;
  std::cout << ACE_TEXT("-m [VALUE]: modifier (e.g. +/-1, as in a roll of e.g. 2d4+1); default: 0") << std::endl;
  std::cout << ACE_TEXT("-n [VALUE]: number of dice (e.g. as in a roll of 3d4); default: 1") << std::endl;
  std::cout << ACE_TEXT("-r [VALUE]: number of such rolls; default: 1") << std::endl;
  std::cout << ACE_TEXT("-s        : include sorted result") << std::endl;
  std::cout << ACE_TEXT("-t        : trace information") << std::endl;
  std::cout << ACE_TEXT("-v        : print version information and exit") << std::endl;
} // end print_usage

const bool
process_arguments(const int argc_in,
                  ACE_TCHAR* argv_in[], // cannot be const...
                  RPG_Dice_Roll& rollSpecs_out,
                  unsigned int& numRolls_out,
                  bool& includeSortedResult_out,
                  bool& traceInformation_out,
                  bool& printVersionAndExit_out)
{
  RPG_TRACE(ACE_TEXT("::process_arguments"));

  // init results
  rollSpecs_out.numDice = 1;
  rollSpecs_out.typeDice = D_6;
  rollSpecs_out.modifier = 0;
  numRolls_out = 1;
  includeSortedResult_out = false;
  traceInformation_out = false;
  printVersionAndExit_out = false;

  ACE_Get_Opt argumentParser(argc_in,
                             argv_in,
                             ACE_TEXT("d:m:n:r:stv"));

  int option = 0;
  while ((option = argumentParser()) != EOF)
  {
    switch (option)
    {
      case 'd':
      {
        // make sure this is a valid number...
        unsigned int temp = ::atol(argumentParser.opt_arg());
        switch (temp)
        {
          case D_0:
//           case D_2:
//           case D_3:
          case D_4:
          case D_6:
          case D_8:
          case D_10:
          case D_12:
          case D_20:
          case D_100:
          {
            rollSpecs_out.typeDice = static_cast<RPG_Dice_DieType> (temp);

            break;
          }
          default:
          {
            ACE_DEBUG((LM_ERROR,
                       ACE_TEXT("invalid input \"%s\", aborting\n"),
                       argumentParser.last_option()));

            return false;
          }
        } // end SWITCH

        break;
      }
      case 'm':
      {
        rollSpecs_out.modifier = ::atol(argumentParser.opt_arg());

        break;
      }
      case 'n':
      {
        rollSpecs_out.numDice = ::atol(argumentParser.opt_arg());

        break;
      }
      case 'r':
      {
        numRolls_out = ::atol(argumentParser.opt_arg());

        break;
      }
      case 's':
      {
        includeSortedResult_out = true;

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

void
do_work(const RPG_Dice_Roll& rollSpecs_in,
        const unsigned int& numRolls_in,
        const bool& includeSortedResult_in)
{
  RPG_TRACE(ACE_TEXT("::do_work"));

  // step1: init randomization
  try
  {
    RPG_Dice::init();
  }
  catch(...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in RPG_Dice::init, returning\n")));

    return;
  }

  // step2a: generate some random numbers...
  RPG_Dice_RollResult_t result;
  try
  {
    RPG_Dice::generateRandomNumbers(rollSpecs_in.typeDice, // see enum
                                    numRolls_in,
                                    result);
  }
  catch(...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in RPG_Dice::generateRandomNumbers, returning\n")));

    return;
  }

//   // debug info
//   // header line
//   std::cout << ACE_TEXT("random numbers: ") << std::endl;
//   std::cout << std::setw(80) << std::setfill(ACE_TEXT_ALWAYS_CHAR('-')) << ACE_TEXT("") << std::setfill(ACE_TEXT_ALWAYS_CHAR(' ')) << std::endl;
//
//   std::ostringstream converter;
//   for (RPG_Dice_RollResultIterator_t iter = result.begin();
//        iter != result.end();
//        iter++)
//   {
//     converter << *iter;
//     std::cout << converter.str() << ACE_TEXT(" ");
//     converter.str(ACE_TEXT_ALWAYS_CHAR("")); // "reset" it...
//   } // end FOR
//   std::cout << std::endl;
//
//   // dump sorted result too ?
//   if (includeSortedResult_in)
//   {
//     // header line
//     std::cout << ACE_TEXT("random numbers (sorted): ") << std::endl;
//     std::cout << std::setw(80) << std::setfill(ACE_TEXT_ALWAYS_CHAR('-')) << ACE_TEXT("") << std::setfill(ACE_TEXT_ALWAYS_CHAR(' ')) << std::endl;
//
//     // sort array
//     std::sort(result.begin(),
//               result.end());
//
// //     converter.str(std::string()); // "reset" it...
//     for (RPG_Dice_RollResultIterator_t iter = result.begin();
//          iter != result.end();
//          iter++)
//     {
//       converter << *iter;
//       std::cout << converter.str() << ACE_TEXT(" ");
//       converter.str(ACE_TEXT_ALWAYS_CHAR("")); // "reset" it...
//     } // end FOR
//     std::cout << std::endl;
//   } // end IF
//
//   std::cout << std::endl;

  // step2b: generate some random dice rolls...
  result.clear();
  try
  {
    RPG_Dice::simulateRoll(rollSpecs_in,
                           numRolls_in,
                           result);
  }
  catch(...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in RPG_Dice::simulateRoll, returning\n")));

    return;
  }

  // debug info
//   // header line
//   std::cout << ACE_TEXT("dice rolls: ") << std::endl;
//   std::cout << std::setw(80) << std::setfill(ACE_TEXT_ALWAYS_CHAR('-')) << ACE_TEXT("") << std::setfill(ACE_TEXT_ALWAYS_CHAR(' ')) << std::endl;

  std::ostringstream converter;
  for (RPG_Dice_RollResultIterator_t iter = result.begin();
       iter != result.end();
       iter++)
  {
    converter << *iter;
    std::cout << converter.str() << ACE_TEXT(" ");
    converter.str(ACE_TEXT_ALWAYS_CHAR("")); // "reset" it...
  } // end FOR
  std::cout << std::endl;

  // dump sorted result too ?
  if (includeSortedResult_in)
  {
    // header line
    std::cout << ACE_TEXT("dice rolls (sorted): ") << std::endl;
    std::cout << std::setw(80) << std::setfill(ACE_TEXT_ALWAYS_CHAR('-')) << ACE_TEXT("") << std::setfill(ACE_TEXT_ALWAYS_CHAR(' ')) << std::endl;

    // sort array
    std::sort(result.begin(),
              result.end());

//     converter.str(std::string()); // "reset" it...
    for (RPG_Dice_RollResultIterator_t iter = result.begin();
         iter != result.end();
         iter++)
    {
      converter << *iter;
      std::cout << converter.str() << ACE_TEXT(" ");
      converter.str(ACE_TEXT_ALWAYS_CHAR("")); // "reset" it...
    } // end FOR
    std::cout << std::endl;
  } // end IF

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
            << RPG_VERSION
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
  std::cout << ACE_TEXT("ACE: ")
            << version_number.str()
            << std::endl;
//   std::cout << "ACE: "
//             << ACE_VERSION
//             << std::endl;
}

int
ACE_TMAIN(int argc,
          ACE_TCHAR* argv[])
{
  RPG_TRACE(ACE_TEXT("::main"));

  // step1: init
  // step1a set defaults
  RPG_Dice_Roll rollSpecs;
  rollSpecs.typeDice       = D_6;
  rollSpecs.numDice        = 1;
  rollSpecs.modifier       = 0;
  unsigned int numRolls    = 1;
  bool includeSortedResult = false;
  bool traceInformation    = false;
  bool printVersionAndExit = false;

  // step1b: parse/process/validate configuration
  if (!(process_arguments(argc,
                          argv,
                          rollSpecs,
                          numRolls,
                          includeSortedResult,
                          traceInformation,
                          printVersionAndExit)))
  {
    // make 'em learn...
    print_usage(std::string(ACE::basename(argv[0])));

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

  ACE_High_Res_Timer timer;
  timer.start();

  // step2: do actual work
  do_work(rollSpecs,
          numRolls,
          includeSortedResult);

  timer.stop();

//   // debug info
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
