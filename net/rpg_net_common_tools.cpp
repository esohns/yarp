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

#include "rpg_net_common_tools.h"

#include "rpg_net_defines.h"

#include <rpg_common_file_tools.h>

#include <ace/OS.h>
#include <ace/FILE_IO.h>
#include <ace/FILE_Connector.h>
#include <ace/Dirent_Selector.h>

#include <sstream>

// init statics
unsigned long RPG_Net_Common_Tools::myMaxNumberOfLogFiles = RPG_NET_DEF_LOG_MAXNUMFILES;

const bool
RPG_Net_Common_Tools::getNextLogFilename(const bool& isServerProcess_in,
                                         const std::string& directory_in,
                                         std::string& FQLogFilename_out)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Common_Tools::getNextLogFilename"));

  // sanity check(s)
  ACE_ASSERT(RPG_Net_Common_Tools::myMaxNumberOfLogFiles >= 1);

  // init return value(s)
  FQLogFilename_out.resize(0);

  // sanity check(s): log directory exists ?
  // No ? --> try to create it then !
  if (!RPG_Common_File_Tools::isDirectory(directory_in))
  {
    if (!RPG_Common_File_Tools::createDirectory(directory_in))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to RPG_Common_File_Tools::createDirectory(\"%s\"), aborting\n"),
                 directory_in.c_str()));

      return false;
    } // end IF

//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("created directory: \"%s\"...\n"),
//                directory_in.c_str()));
  } // end IF

  // construct correct logfilename...
  FQLogFilename_out = directory_in;
  FQLogFilename_out += ACE_DIRECTORY_SEPARATOR_STR;
  std::string logFileName = (isServerProcess_in ? RPG_NET_DEF_LOG_SERVER_FILENAME_PREFIX
                                                : RPG_NET_DEF_LOG_CLIENT_FILENAME_PREFIX);
  logFileName += RPG_NET_DEF_LOG_FILENAME_SUFFIX;
  FQLogFilename_out += logFileName;

  // retrieve all existing logs and sort them alphabetically...
  ACE_Dirent_Selector entries;
  if (entries.open(directory_in.c_str(),
                   (isServerProcess_in ? &RPG_Net_Common_Tools::server_selector
                                       : &RPG_Net_Common_Tools::client_selector),
                   &RPG_Net_Common_Tools::comparator) == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Dirent_Selector::open(\"%s\"): \"%s\", aborting\n"),
               directory_in.c_str(),
               ACE_OS::strerror(errno)));

    return false;
  } // end IF

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("found %d logfiles...\n"),
//              entries.length()));

  // OK: iterate over the entries and perform some magic...
  // *IMPORTANT NOTE*: entries have been sorted alphabetically:
  //                   1 current 2 4 3 --> current 1 2 3 4
  // *TODO*: some malicious user could inject "fake" logfiles which can
  //         "confuse" this algorithm...
  // skip handling of "<PREFIX><SUFFIX>" (if found)...
  // *IMPORTANT NOTE*: <PREFIX><SUFFIX> will become <PREFIX>_1<SUFFIX>...
  bool found_current = false;
  // sscanf settings
  int number = 0;
  int return_val = -1;
  std::string format_string("%d");
  format_string += RPG_NET_DEF_LOG_FILENAME_SUFFIX;
  std::stringstream converter;
  for (int i = entries.length() - 1, index = RPG_Net_Common_Tools::myMaxNumberOfLogFiles - 1;
       i >= 0;
       i--)
  {
    // perform "special treatment" if "<PREFIX><SUFFIX>" found...
    // *TODO*: do this in C++...
    if (ACE_OS::strcmp(entries[i]->d_name,
                       logFileName.c_str()) == 0)
    {
      found_current = true;

      // skip this one for now
      continue;
    } // end IF

    // scan number...
    try
    {
      // *TODO*: do this in C++...
      return_val = ::sscanf(entries[i]->d_name +
                            // skip some characters...
                            (ACE_OS::strlen((isServerProcess_in ? RPG_NET_DEF_LOG_SERVER_FILENAME_PREFIX
                                                                : RPG_NET_DEF_LOG_CLIENT_FILENAME_PREFIX)) + 1),
                            format_string.c_str(),
                            &number);
      if (return_val != 1)
      {
        if (return_val != 0)
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("::sscanf() failed for \"%s\": \"%s\", continuing\n"),
                     entries[i]->d_name,
                     ACE_OS::strerror(errno)));
        } // end IF

        continue;
      } // end IF
    }
    catch (...)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("caught exception in ::sscanf() for \"%s\": \"%s\", continuing\n"),
                 entries[i]->d_name,
                 ACE_OS::strerror(errno)));

      continue;
    }

    // adjust the index, if the number is smaller than max
    if (number < index)
      index = number + 1;

    // if the number is bigger than the max AND we have more than enough logs --> delete it !
    if ((ACE_static_cast(unsigned long, number) >= (RPG_Net_Common_Tools::myMaxNumberOfLogFiles - 1)) &&
        (ACE_static_cast(unsigned long, entries.length()) >= RPG_Net_Common_Tools::myMaxNumberOfLogFiles))
    {
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("removing surplus logfile \"%s\"...\n"),
                 entries[i]->d_name));

      // clean up
      std::string FQfilename = directory_in;
      FQfilename += ACE_DIRECTORY_SEPARATOR_STR;
      FQfilename += entries[i]->d_name;
      RPG_Common_File_Tools::deleteFile(FQfilename);

      continue;
    } // end IF

    // logrotate file...
    std::string oldFQfilename = directory_in;
    oldFQfilename += ACE_DIRECTORY_SEPARATOR_STR;
    oldFQfilename += entries[i]->d_name;

    std::string newFQfilename = directory_in;
    newFQfilename += ACE_DIRECTORY_SEPARATOR_STR;
    newFQfilename += (isServerProcess_in ? RPG_NET_DEF_LOG_SERVER_FILENAME_PREFIX
                                         : RPG_NET_DEF_LOG_CLIENT_FILENAME_PREFIX);
    newFQfilename += "_";
    converter.str(ACE_TEXT_ALWAYS_CHAR(""));
    converter << index;
    newFQfilename += converter.str();
    newFQfilename += RPG_NET_DEF_LOG_FILENAME_SUFFIX;
    // *IMPORTANT NOTE*: last parameter affects Win32 behaviour only,
    // see "ace/OS_NS_stdio.inl" !
    if (ACE_OS::rename(oldFQfilename.c_str(),
                       newFQfilename.c_str(),
                       -1))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_OS::rename() \"%s\" to \"%s\": \"%s\", aborting\n"),
                 oldFQfilename.c_str(),
                 newFQfilename.c_str(),
                 ACE_OS::strerror(errno)));

      return false;
    } // end IF

    // debug info
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("renamed file \"%s\" to \"%s\"...\n"),
               oldFQfilename.c_str(),
               newFQfilename.c_str()));

    index--;
  } // end FOR

  if (found_current)
  {
    std::string newFQfilename = directory_in;
    newFQfilename += ACE_DIRECTORY_SEPARATOR_STR;
    newFQfilename += (isServerProcess_in ? RPG_NET_DEF_LOG_SERVER_FILENAME_PREFIX
                                         : RPG_NET_DEF_LOG_CLIENT_FILENAME_PREFIX);
    newFQfilename += "_1";
    newFQfilename += RPG_NET_DEF_LOG_FILENAME_SUFFIX;

    // *TODO*: last parameter affects Win32 behaviour only, see "ace/OS_NS_stdio.inl" !
    if (ACE_OS::rename(FQLogFilename_out.c_str(),
                       newFQfilename.c_str(),
                       -1))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_OS::rename() \"%s\" to \"%s\": \"%s\", aborting\n"),
                 FQLogFilename_out.c_str(),
                 newFQfilename.c_str(),
                 ACE_OS::strerror(errno)));

      return false;
    } // end IF

    // debug info
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("renamed file \"%s\" to \"%s\"...\n"),
               FQLogFilename_out.c_str(),
               newFQfilename.c_str()));
  } // end IF

  return true;
}

const std::string
RPG_Net_Common_Tools::messageType2String(const RPG_Net_Remote_Comm::MessageType& messageType_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Common_Tools::messageType2String"));

  std::string result;

  switch (messageType_in)
  {
    case RPG_Net_Remote_Comm::RPG_NET_PING:
    {
      result = ACE_TEXT("RPG_NET_PING");

      break;
    }
    case RPG_Net_Remote_Comm::RPG_NET_PONG:
    {
      result = ACE_TEXT("RPG_NET_PONG");

      break;
    }
    default:
    {
      // debug info
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid message header: %d, aborting\n"),
                 messageType_in));

      break;
    }
  } // end SWITCH

  return result;
}

int
RPG_Net_Common_Tools::client_selector(const dirent* dirEntry_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Common_Tools::client_selector"));

  // *IMPORTANT NOTE*: we need to select those files which follow our naming
  // schema for logfiles: "<PREFIX>[_<NUMBER>]<SUFFIX>"

  // sanity check --> prefix ok ?
  if (ACE_OS::strncmp(dirEntry_in->d_name,
                      RPG_NET_DEF_LOG_CLIENT_FILENAME_PREFIX,
                      ACE_OS::strlen(RPG_NET_DEF_LOG_CLIENT_FILENAME_PREFIX)) != 0)
  {
//     // debug info
//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("ignoring \"%s\"...\n"),
//                dirEntry_in->d_name));

    return 0;
  } // end IF

  return 1;
}

int
RPG_Net_Common_Tools::server_selector(const dirent* dirEntry_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Common_Tools::server_selector"));

  // *IMPORTANT NOTE*: we need to select those files which follow our naming
  // schema for logfiles: "<PREFIX>[_<NUMBER>]<SUFFIX>"

  // sanity check --> prefix ok ?
  if (ACE_OS::strncmp(dirEntry_in->d_name,
                      RPG_NET_DEF_LOG_SERVER_FILENAME_PREFIX,
                      ACE_OS::strlen(RPG_NET_DEF_LOG_SERVER_FILENAME_PREFIX)) != 0)
  {
//     // debug info
//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("ignoring \"%s\"...\n"),
//                dirEntry_in->d_name));

    return 0;
  } // end IF

  return 1;
}

int
RPG_Net_Common_Tools::comparator(const dirent** d1,
                                 const dirent** d2)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Common_Tools::comparator"));

  return ACE_OS::strcmp((*d1)->d_name,
                        (*d2)->d_name);
}
