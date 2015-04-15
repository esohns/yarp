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

#include "rpg_common_file_tools.h"

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "Userenv.h"
#include "Shlobj.h"
#endif

#include "ace/ACE.h"
#include "ace/OS.h"
#include "ace/FILE_IO.h"
#include "ace/FILE_Connector.h"
#include "ace/Dirent_Selector.h"
#include "ace/OS_NS_sys_sendfile.h"

#include "common_tools.h"
#include "common_file_tools.h"
#include "common_defines.h"

#include "rpg_config.h"

#include "rpg_common_macros.h"
#include "rpg_common_defines.h"
#include "rpg_common_tools.h"

std::string
RPG_Common_File_Tools::getConfigurationDataDirectory(const std::string& baseDir_in,
                                                     const bool& isConfig_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Common_File_Tools::getConfigurationDataDirectory"));

  std::string result = baseDir_in;

  if (baseDir_in.empty())
  {
#if defined(ACE_WIN32) || defined(ACE_WIN64)
    TCHAR buffer[PATH_MAX];
    ACE_OS::memset(buffer, 0, sizeof(buffer));

    HRESULT win_result =
      SHGetFolderPath(NULL,                                         // hwndOwner
                      CSIDL_PROGRAM_FILES | CSIDL_FLAG_DONT_VERIFY, // nFolder
                      NULL,                                         // hToken
                      SHGFP_TYPE_CURRENT,                           // dwFlags
                      buffer);                                      // pszPath
    if (FAILED(win_result))
    {
      ACE_OS::memset(buffer, 0, sizeof(buffer));
      if (FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,                // dwFlags
                        NULL,                                      // lpSource
                        win_result,                                // dwMessageId
                        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // dwLanguageId
                        buffer,                                    // lpBuffer
                        PATH_MAX,                                  // nSize
                        NULL) == 0)                                // Arguments
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to FormatMessage(%d): \"%m\", continuing\n"),
                   win_result));
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to SHGetFolderPath(CSIDL_PROGRAM_FILES): \"%s\", falling back\n"),
                 buffer));

      // fallback
      return Common_File_Tools::getWorkingDirectory();
    } // end IF

    result = ACE_TEXT_ALWAYS_CHAR(buffer);
    result += ACE_DIRECTORY_SEPARATOR_CHAR_A;
    result += ACE_TEXT_ALWAYS_CHAR(RPG_PACKAGE);
#else
    return Common_File_Tools::getWorkingDirectory();
#endif
  } // end IF

  result += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  result += (isConfig_in ? ACE_TEXT_ALWAYS_CHAR(RPG_COMMON_CONFIG_SUB)
                         : ACE_TEXT_ALWAYS_CHAR(RPG_COMMON_DATA_SUB));

  // sanity check(s)
  if (!Common_File_Tools::isDirectory(result))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("not a directory: \"%s\", falling back\n"),
               ACE_TEXT(result.c_str())));

    // fallback
    return Common_File_Tools::getWorkingDirectory();
  } // end IF

  return result;
}

std::string
RPG_Common_File_Tools::getUserConfigurationDirectory()
{
  RPG_TRACE(ACE_TEXT("RPG_Common_File_Tools::getUserConfigurationDirectory"));

  std::string result;

#if !defined(ACE_WIN32) && !defined(ACE_WIN64)
  std::string user_name;
  std::string real_name;
  Common_Tools::getCurrentUserName(user_name, real_name);
  if (user_name.empty())
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to Common_Tools::getCurrentUserName(), falling back\n")));

    // fallback
    result =
        ACE_TEXT_ALWAYS_CHAR(ACE_OS::getenv(ACE_TEXT(COMMON_DEF_DUMP_DIR)));
    return result;
  } // end IF

  result = Common_File_Tools::getUserHomeDirectory(user_name);
  if (result.empty())
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to Common_File_Tools::getUserHomeDirectory(\"%s\"), falling back\n"),
               ACE_TEXT(user_name.c_str())));

    // fallback
    result =
        ACE_TEXT_ALWAYS_CHAR(ACE_OS::getenv(ACE_TEXT(COMMON_DEF_DUMP_DIR)));
    return result;
  } // end IF

  result += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  result += ACE_TEXT_ALWAYS_CHAR(".");
#else
  TCHAR buffer[PATH_MAX];
  ACE_OS::memset(buffer, 0, sizeof(buffer));

  HRESULT win_result = SHGetFolderPath(NULL,                                   // hwndOwner
                                       CSIDL_APPDATA | CSIDL_FLAG_DONT_VERIFY, // nFolder
                                       NULL,                                   // hToken
                                       SHGFP_TYPE_CURRENT,                     // dwFlags
                                       buffer);                                // pszPath
  if (FAILED(win_result))
  {
    ACE_OS::memset(buffer, 0, sizeof(buffer));
    if (FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,                // dwFlags
                      NULL,                                      // lpSource
                      win_result,                                // dwMessageId
                      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // dwLanguageId
                      buffer,                                    // lpBuffer
                      PATH_MAX,                                  // nSize
                      NULL) == 0)                                // Arguments
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to FormatMessage(%d): \"%m\", continuing\n"),
                 win_result));
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SHGetFolderPath(CSIDL_APPDATA): \"%s\", falling back\n"),
               buffer));

    // fallback
    result =
        ACE_TEXT_ALWAYS_CHAR(ACE_OS::getenv(ACE_TEXT(COMMON_DEF_DUMP_DIR)));
    return result;
  } // end IF

  result = ACE_TEXT_ALWAYS_CHAR(buffer);
  result += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  result += ACE_TEXT_ALWAYS_CHAR(RPG_PACKAGE);

  if (!Common_File_Tools::isDirectory(result))
  {
    if (!Common_File_Tools::createDirectory(result))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to RPG_Common_File_Tools::createDirectory(\"%s\"), falling back\n"),
                 ACE_TEXT(result.c_str())));

      // fallback
      result =
          ACE_TEXT_ALWAYS_CHAR(ACE_OS::getenv(ACE_TEXT(COMMON_DEF_DUMP_DIR)));
    } // end IF
    else
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("created game directory \"%s\"\n"),
                 ACE_TEXT(result.c_str())));
  } // end IF

  return result;
}

std::string
RPG_Common_File_Tools::getLogFilename(const std::string& programName_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Common_File_Tools::getLogFilename"));

  // sanity check(s)
  ACE_ASSERT(!programName_in.empty());

  // construct correct logfilename...
  std::string result = RPG_Common_File_Tools::getLogDirectory();
  result += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  result += programName_in;
  result += COMMON_LOG_FILENAME_SUFFIX;

  // sanity check(s): log file exists ?
  // Yes ? --> try to delete it then !
  if (Common_File_Tools::isReadable(result))
  {
    if (!Common_File_Tools::deleteFile(result))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to RPG_Common_File_Tools::deleteFile(\"%s\"), aborting\n"),
                 ACE_TEXT(result.c_str())));

      // clean up
      result.clear();

      return result;
    } // end IF

    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("deleted file: \"%s\"...\n"),
               ACE_TEXT(result.c_str())));
  } // end IF

  return result;
}

std::string
RPG_Common_File_Tools::getLogDirectory()
{
  RPG_TRACE(ACE_TEXT("RPG_Common_File_Tools::getLogDirectory"));

  // init return value(s)
  std::string result;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  result = ACE_OS::getenv(ACE_TEXT_ALWAYS_CHAR(COMMON_DEF_LOG_DIRECTORY));
#else
  result = ACE_TEXT_ALWAYS_CHAR(COMMON_DEF_LOG_DIRECTORY);
#endif

  // sanity check(s): directory exists ?
  // No ? --> try to create it then !
  if (!Common_File_Tools::isDirectory(result))
  {
    if (!Common_File_Tools::createDirectory(result))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to RPG_Common_File_Tools::createDirectory(\"%s\"), aborting\n"),
                 ACE_TEXT(result.c_str())));

      // clean up
      result.clear();

      return result;
    } // end IF

    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("created directory: \"%s\"...\n"),
               ACE_TEXT(result.c_str())));
  } // end IF

  return result;
}
