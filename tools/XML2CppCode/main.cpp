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

#ifdef HAVE_CONFIG_H
//#include "XML2CppCode-config.h"
#endif

#include "xml2cppcode.h"
#include "XML_parser.h"

#include "xercesc/util/PlatformUtils.hpp"
#include "xercesc/util/XMLString.hpp"

#include "ace/ACE.h"
#include "ace/OS.h"
#include "ace/Log_Msg.h"
#include "ace/Get_Opt.h"
#include "ace/High_Res_Timer.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>

XERCES_CPP_NAMESPACE_USE

void
print_usage(const std::string& programName_in)
{
  ACE_TRACE(ACE_TEXT("::print_usage"));

  std::cout << ACE_TEXT("usage: ") << programName_in << ACE_TEXT(" [OPTIONS]") << std::endl << std::endl;
  std::cout << ACE_TEXT("currently available options:") << std::endl;
  std::cout << ACE_TEXT("-d [STRING] : emit class qualifier (i.e. DLL import/export symbols)") << std::endl;
  std::cout << ACE_TEXT("-e          : emit string conversion utilities") << std::endl;
  std::cout << ACE_TEXT("-f [FILE]   : filename (*.xsd)") << std::endl;
  std::cout << ACE_TEXT("-i          : generate separate include header (for use with -s) file") << std::endl;
  std::cout << ACE_TEXT("-o [PATH]   : target directory") << std::endl;
  std::cout << ACE_TEXT("-p [FILE]   : preamble (*.txt)") << std::endl;
  std::cout << ACE_TEXT("-s          : separate (set of) files per definition") << std::endl;
  std::cout << ACE_TEXT("-t          : trace information") << std::endl;
  std::cout << ACE_TEXT("-u          : emit \"tagged\" unions") << std::endl;
  std::cout << ACE_TEXT("-v          : print version information and exit") << std::endl;
  std::cout << ACE_TEXT("-x [STRING] : set (type) prefix") << std::endl;
  std::cout << ACE_TEXT("-y [STRING] : remove (type) postfix") << std::endl;
} // end print_usage

bool
process_arguments(const int& argc_in,
                  ACE_TCHAR** argv_in, // cannot be const...
                  std::string& emitClassQualifier_out,
                  bool& emitStringConversionUtilities_out,
                  bool& emitTaggedUnions_out,
                  std::string& filename_out,
                  bool& generateIncludeHeader_out,
                  std::string& directory_out,
                  std::string& preamble_out,
                  bool& filePerDefinition_out,
                  bool& traceInformation_out,
                  bool& printVersionAndExit_out,
                  std::string& typePrefix_out,
                  std::string& typePostfix_out)
{
  ACE_TRACE(ACE_TEXT("::process_arguments"));

  // init results
  emitClassQualifier_out.clear();
  emitStringConversionUtilities_out = false;
  emitTaggedUnions_out = false;
  filename_out.clear();
  generateIncludeHeader_out = false;
  directory_out.clear();
  preamble_out.clear();
  filePerDefinition_out = false;
  traceInformation_out = false;
  printVersionAndExit_out = false;
  typePrefix_out.clear();
  typePostfix_out = ACE_TEXT_ALWAYS_CHAR(XML2CPPCODE_DEFAULTTYPEPOSTFIX);

  ACE_Get_Opt argumentParser(argc_in,
                             argv_in,
                             ACE_TEXT("d:ef:io:p:stuvx:y:"));

  int option = 0;
  while ((option = argumentParser()) != EOF)
  {
    switch (option)
    {
		  case 'd':
      {
				emitClassQualifier_out = ACE_TEXT_ALWAYS_CHAR(argumentParser.opt_arg());

        break;
      }
      case 'e':
      {
        emitStringConversionUtilities_out = true;

        break;
      }
      case 'f':
      {
				filename_out = ACE_TEXT_ALWAYS_CHAR(argumentParser.opt_arg());

        break;
      }
      case 'i':
      {
        generateIncludeHeader_out = true;

        break;
      }
      case 'o':
      {
				directory_out = ACE_TEXT_ALWAYS_CHAR(argumentParser.opt_arg());

        break;
      }
      case 'p':
      {
				preamble_out = ACE_TEXT_ALWAYS_CHAR(argumentParser.opt_arg());

        break;
      }
      case 's':
      {
        filePerDefinition_out = true;

        break;
      }
      case 't':
      {
        traceInformation_out = true;

        break;
      }
      case 'u':
      {
        emitTaggedUnions_out = true;

        break;
      }
      case 'v':
      {
        printVersionAndExit_out = true;

        break;
      }
      case 'x':
      {
        typePrefix_out = ACE_TEXT_ALWAYS_CHAR(argumentParser.opt_arg());

        break;
      }
      case 'y':
      {
				typePostfix_out = ACE_TEXT_ALWAYS_CHAR(argumentParser.opt_arg());

        break;
      }
      // error handling
      case '?':
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("unrecognized option \"%s\", aborting\n"),
									 ACE_TEXT(argumentParser.last_option())));

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
do_work(const std::string& emitClassQualifier_in,
        const bool& emitStringConversionUtilities_in,
        const bool& emitTaggedUnions_in,
        const std::string& filename_in,
        const bool& generateIncludeHeader_in,
        const std::string& directory_in,
        const std::string& preamble_in,
        const bool& filePerDefinition_in,
        const std::string& typePrefix_in,
        const std::string& typePostfix_in)
{
  ACE_TRACE(ACE_TEXT("::do_work"));

  // step0: init Xerces framework
  try
  {
    XMLPlatformUtils::Initialize();
  }
  catch (const XMLException& exception_in)
  {
    char* message = XMLString::transcode(exception_in.getMessage());
    ACE_ASSERT(message);

    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in XMLPlatformUtils::Initialize: \"%s\", returning\n"),
               message));

    // clean up
    XMLString::release(&message);

    return;
  }

  // step1: parse XML file
  XML_PARSER_SINGLETON::instance()->parseFile(emitClassQualifier_in,
                                              emitStringConversionUtilities_in,
                                              emitTaggedUnions_in,
                                              filename_in,
                                              generateIncludeHeader_in,
                                              directory_in,
                                              preamble_in,
                                              filePerDefinition_in,
                                              typePrefix_in,
                                              typePostfix_in,
                                              true); // (try to) validate documents

  // And call the termination method
  XMLPlatformUtils::Terminate();

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("finished working...\n")));
} // end do_work

void
do_printVersion(const std::string& programName_in)
{
  ACE_TRACE(ACE_TEXT("::do_printVersion"));

//#if defined(_MSC_VER)
//  std::cout << programName_in << ACE_TEXT(" : ") << XML2CPPCODE_VERSION << std::endl;
//#else
////  // *TODO*: leave as-is for now (see Yarp/configure.ac)
//  //std::cout << programName_in << ACE_TEXT(" : ") << VERSION << std::endl;
//  std::cout << programName_in << ACE_TEXT(" : ") << XML2CPPCODE_VERSION << std::endl;
//#endif

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

bool
validate_arguments(const std::string& emitClassQualifier_in,
									 const bool& emitStringConversionUtilities_in,
									 const bool& emitTaggedUnions_in,
                   const std::string& filename_in,
                   const bool& generateIncludeHeader_in,
                   const std::string& directory_in,
                   const std::string& preamble_in,
                   const bool& filePerDefinition_in)
{
  ACE_TRACE(ACE_TEXT("::validate_arguments"));

  ACE_stat stat;
  if (filename_in.empty() ||
      (ACE_OS::stat(filename_in.c_str(), &stat) == -1)) // exists ?
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid (XMLSchema) filename \"%s\", aborting\n"),
							 ACE_TEXT(filename_in.c_str())));

    return false;
  } // end IF

  if (directory_in.empty() ||
      (ACE_OS::stat(directory_in.c_str(), &stat) == -1)) // exists ?
  {
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("invalid target directory \"%s\", aborting\n"),
							 ACE_TEXT(directory_in.c_str())));

    return false;
  } // end IF

  if (!preamble_in.empty() &&
      (ACE_OS::stat(preamble_in.c_str(), &stat) == -1)) // exists ?
  {
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("invalid preamble \"%s\", aborting\n"),
							 ACE_TEXT(preamble_in.c_str())));

    return false;
  } // end IF

  if (generateIncludeHeader_in && !filePerDefinition_in)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("\"-i\" only makes sense with \"-s\", aborting\n")));

    return false;
  } // end IF

  return true;
}

int
ACE_TMAIN(int argc_in,
          ACE_TCHAR** argv_in)
{
  ACE_TRACE(ACE_TEXT("::main"));

  // step1: init
  // step1a set defaults
  std::string emitClassQualifier;
  bool emitStringConversionUtilities = false;
  bool emitTaggedUnions = false;
  std::string filename, directory, preamble, typePrefix, typePostfix;
  char buf[PATH_MAX];
  ACE_OS::memset(buf, 0, sizeof(buf));
  directory = ACE_OS::getcwd(buf, sizeof(buf));
  bool generateIncludeHeader = false;
  bool filePerDefinition   = false; // by default, we put everything into one class/file...
  bool traceInformation    = false;
  bool printVersionAndExit = false;

  // step1b: parse/process/validate configuration
  if (!process_arguments(argc_in,
				                 argv_in,
                         emitClassQualifier,
                         emitStringConversionUtilities,
                         emitTaggedUnions,
                         filename,
                         generateIncludeHeader,
                         directory,
                         preamble,
                         filePerDefinition,
                         traceInformation,
                         printVersionAndExit,
                         typePrefix,
                         typePostfix))
  {
    // make 'em learn...
    print_usage(ACE::basename(argv_in[0]));

    return EXIT_FAILURE;
  } // end IF

  // step1b: validate arguments
  if (!validate_arguments(emitClassQualifier,
                          emitStringConversionUtilities,
                          emitTaggedUnions,
                          filename,
                          generateIncludeHeader,
                          directory,
                          preamble,
                          filePerDefinition))
  {
    // make 'em learn...
    print_usage(ACE::basename(argv_in[0]));

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
    do_printVersion(ACE::basename(argv_in[0]));

    return EXIT_SUCCESS;
  } // end IF

  ACE_High_Res_Timer timer;
  timer.start();

  // step2: do actual work
  do_work(emitClassQualifier,
          emitStringConversionUtilities,
          emitTaggedUnions,
          filename,
          generateIncludeHeader,
          directory,
          preamble,
          filePerDefinition,
          typePrefix,
          typePostfix);

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
//              ACE_TEXT(working_time_string.c_str())));

  return EXIT_SUCCESS;
} // end main
