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
#endif // ACE_WIN32 || ACE_WIN64

#include "ace/ACE.h"
#include "ace/OS.h"
#include "ace/FILE_IO.h"
#include "ace/FILE_Connector.h"
#include "ace/Dirent_Selector.h"
#include "ace/OS_NS_sys_sendfile.h"

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
#include "common_os_tools.h"
#endif // ACE_WIN32 || ACE_WIN64
#include "common_file_tools.h"
#include "common_defines.h"
#include "common_macros.h"

#include "common_error_tools.h"

#include "common_string_tools.h"

#ifdef HAVE_CONFIG_H
#include "rpg_config.h"
#endif // HAVE_CONFIG_H

#include "rpg_common_macros.h"
#include "rpg_common_defines.h"
#include "rpg_common_tools.h"

std::string
RPG_Common_File_Tools::getSourceDirectory (const std::string& packageName_in,
                                           const std::string& moduleName_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Common_File_Tools::getSourceDirectory"));

  // initialize return value(s)
  std::string return_value;

  // sanity check(s)
  ACE_ASSERT (!packageName_in.empty ());

#if defined (BASEDIR)
  return_value = ACE_TEXT_ALWAYS_CHAR (BASEDIR);
#else
  ACE_TCHAR buffer_a[PATH_MAX];
  ACE_OS::memset (buffer_a, 0, sizeof (ACE_TCHAR[PATH_MAX]));

  ACE_TCHAR* string_p =
    ACE_OS::getenv (ACE_TEXT (COMMON_ENVIRONMENT_DIRECTORY_ROOT_PROJECTS));
  if (!string_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::getenv(%s): \"%m\", aborting\n"),
                ACE_TEXT (COMMON_ENVIRONMENT_DIRECTORY_ROOT_PROJECTS)));
    return return_value;
  } // end IF
  return_value = ACE_TEXT_ALWAYS_CHAR (string_p);
#endif // BASEDIR
  return_value += ACE_DIRECTORY_SEPARATOR_STR;
  //return_value += ACE_TEXT_ALWAYS_CHAR (ACE_DLL_PREFIX);
  return_value += packageName_in;
  if (!moduleName_in.empty ())
  {
    return_value += ACE_DIRECTORY_SEPARATOR_STR;
    return_value += moduleName_in;
  } // end IF

  // sanity check(s)
  if (unlikely (!Common_File_Tools::isDirectory (return_value)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("not a directory (was: \"%s\"), aborting\n"),
                ACE_TEXT (return_value.c_str ())));
    return_value.clear ();
  } // end IF

  return return_value;
}

std::string
RPG_Common_File_Tools::getConfigurationDataDirectory (const std::string& packageName_in,
                                                      const std::string& moduleName_in,
                                                      const std::string& moduleName2_in,
                                                      bool isConfiguration_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Common_File_Tools::getConfigurationDataDirectory"));

  // initialize return value(s)
  std::string return_value;

  // sanity check(s)
  ACE_ASSERT (!packageName_in.empty ());

  bool is_test_b =
    (!ACE_OS::strncmp (moduleName_in.c_str (), ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_TEST_I_SUBDIRECTORY),
                       ACE_OS::strlen (ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_TEST_I_SUBDIRECTORY))) ||
     !ACE_OS::strncmp (moduleName_in.c_str (), ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_TEST_U_SUBDIRECTORY),
                       ACE_OS::strlen (ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_TEST_U_SUBDIRECTORY))));

  if (Common_Error_Tools::inDebugSession ())
  {
    if (is_test_b)
    {
      // sanity check(s)
      ACE_ASSERT (!Common_File_Tools::executable.empty ());

      return_value =
        RPG_Common_File_Tools::getSourceDirectory (packageName_in,
                                                   ACE_TEXT_ALWAYS_CHAR (""));
      return_value += ACE_DIRECTORY_SEPARATOR_STR;
      return_value += moduleName_in;
      return_value += ACE_DIRECTORY_SEPARATOR_STR;
      return_value +=
        (moduleName2_in.empty () ? Common_String_Tools::tolower (Common_File_Tools::basename (Common_File_Tools::executable, true))
                                 : Common_String_Tools::tolower (moduleName2_in));
      return_value += ACE_DIRECTORY_SEPARATOR_STR;
      return_value +=
          (isConfiguration_in ? ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_CONFIGURATION_SUBDIRECTORY)
                              : ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_DATA_SUBDIRECTORY));
    } // end IF
    else
    {
      return_value = RPG_Common_File_Tools::getSourceDirectory (packageName_in,
                                                                moduleName2_in);
      return_value += ACE_DIRECTORY_SEPARATOR_STR;
      return_value +=
          (isConfiguration_in ? ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_CONFIGURATION_SUBDIRECTORY)
                              : ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_DATA_SUBDIRECTORY));
    } // end ELSE
    // sanity check(s)
    //ACE_ASSERT (Common_File_Tools::isDirectory (return_value));

    return return_value;
  } // end IF

  // not running in a debug session

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (is_test_b)
  {
    return_value = Common_File_Tools::getWorkingDirectory ();
    return_value += ACE_DIRECTORY_SEPARATOR_STR;
    //return_value +=
    //  ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_PARENT_SUBDIRECTORY);
    //return_value += ACE_DIRECTORY_SEPARATOR_STR;
    return_value +=
        (isConfiguration_in ? ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_CONFIGURATION_SUBDIRECTORY)
                            : ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_DATA_SUBDIRECTORY));
    //return_value += ACE_DIRECTORY_SEPARATOR_STR;
    //return_value +=
    //  Common_String_Tools::tolower (Common_File_Tools::basename (Common_File_Tools::executable, true));
  } // end IF
  else
  {
    return_value = Common_File_Tools::getWorkingDirectory ();
    return_value += ACE_DIRECTORY_SEPARATOR_STR;
    return_value += ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_DATA_SUBDIRECTORY);
    //return_value += ACE_DIRECTORY_SEPARATOR_STR;
    //return_value +=
    //  Common_String_Tools::tolower (Common_File_Tools::basename (Common_File_Tools::executable, true));
  } // end ELSE
#else
  return_value =
    Common_File_Tools::getSystemConfigurationDataDirectory (packageName_in,
                                                            moduleName_in,
                                                            isConfiguration_in);
#endif // ACE_WIN32 || ACE_WIN64

  // sanity check(s)
//  ACE_ASSERT (Common_File_Tools::isDirectory (return_value));

  return return_value;
}

std::string
RPG_Common_File_Tools::getUserConfigurationDirectory ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Common_File_Tools::getUserConfigurationDirectory"));

  std::string result;

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  TCHAR buffer[PATH_MAX];
  ACE_OS::memset (buffer, 0, sizeof (buffer));

  HRESULT win_result =
    SHGetFolderPath (NULL,                                   // hwndOwner
                     CSIDL_APPDATA | CSIDL_FLAG_DONT_VERIFY, // nFolder
                     NULL,                                   // hToken
                     SHGFP_TYPE_CURRENT,                     // dwFlags
                     buffer);                                // pszPath
  if (FAILED (win_result))
  {
    ACE_OS::memset (buffer, 0, sizeof (buffer));
    if (FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM,                 // dwFlags
                       NULL,                                       // lpSource
                       win_result,                                 // dwMessageId
                       MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT), // dwLanguageId
                       buffer,                                     // lpBuffer
                       PATH_MAX,                                   // nSize
                       NULL) == 0)                                 // Arguments
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to FormatMessage(%d): \"%m\", continuing\n"),
                  win_result));
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SHGetFolderPath(CSIDL_APPDATA): \"%s\", falling back\n"),
                buffer));

    // fallback
    result =
        ACE_TEXT_ALWAYS_CHAR (ACE_OS::getenv (ACE_TEXT (COMMON_LOCATION_TEMPORARY_STORAGE_VARIABLE)));
    return result;
  } // end IF

  result = ACE_TEXT_ALWAYS_CHAR (buffer);
  result += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#else
  std::string user_name = Common_OS_Tools::getUserName ();
  ACE_ASSERT (!user_name.empty ());
  result = Common_File_Tools::getHomeDirectory (user_name);
  ACE_ASSERT (!result.empty ());
  result += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  result += ACE_TEXT_ALWAYS_CHAR (".");
#endif // ACE_WIN32 || ACE_WIN64
  result += ACE_TEXT_ALWAYS_CHAR (yarp_PACKAGE_NAME);

  if (!Common_File_Tools::isDirectory (result))
  {
    if (!Common_File_Tools::createDirectory (result))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Common_File_Tools::createDirectory(\"%s\"), falling back\n"),
                  ACE_TEXT (result.c_str ())));

      // fallback
      result =
          ACE_TEXT_ALWAYS_CHAR (ACE_OS::getenv (ACE_TEXT (COMMON_LOCATION_TEMPORARY_STORAGE_VARIABLE)));
    } // end IF
    else
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("created game directory \"%s\"\n"),
                  ACE_TEXT (result.c_str ())));
  } // end IF

  return result;
}

std::string
RPG_Common_File_Tools::getLogFilename (const std::string& programName_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Common_File_Tools::getLogFilename"));

  // sanity check(s)
  ACE_ASSERT (!programName_in.empty ());

  // construct correct logfilename...
  std::string result = RPG_Common_File_Tools::getLogDirectory ();
  result += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  result += programName_in;
  result += COMMON_LOG_FILENAME_SUFFIX;

  // sanity check(s): log file exists ?
  // Yes ? --> try to delete it then !
  if (Common_File_Tools::isReadable (result))
  {
    if (!Common_File_Tools::deleteFile (result))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to RPG_Common_File_Tools::deleteFile(\"%s\"), aborting\n"),
                  ACE_TEXT (result.c_str ())));

      // clean up
      result.clear ();

      return result;
    } // end IF

    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("deleted file: \"%s\"...\n"),
                ACE_TEXT (result.c_str ())));
  } // end IF

  return result;
}

std::string
RPG_Common_File_Tools::getLogDirectory ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Common_File_Tools::getLogDirectory"));

  // init return value(s)
  std::string result;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  result = ACE_OS::getenv (ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_TEMPORARY_STORAGE_VARIABLE));
#else
  result = ACE_TEXT_ALWAYS_CHAR (COMMON_LOG_DEFAULT_DIRECTORY);
#endif // ACE_WIN32 || ACE_WIN64

  // sanity check(s): directory exists ?
  // No ? --> try to create it then !
  if (!Common_File_Tools::isDirectory (result))
  {
    if (!Common_File_Tools::createDirectory (result))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to RPG_Common_File_Tools::createDirectory(\"%s\"), aborting\n"),
                  ACE_TEXT (result.c_str ())));

      // clean up
      result.clear ();

      return result;
    } // end IF
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("created directory: \"%s\"...\n"),
                ACE_TEXT (result.c_str ())));
  } // end IF

  return result;
}
