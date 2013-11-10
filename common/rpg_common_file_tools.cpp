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

#include "rpg_common_macros.h"
#include "rpg_common_defines.h"

#include <ace/ACE.h>
#include <ace/OS.h>
#include <ace/FILE_IO.h>
#include <ace/FILE_Connector.h>
#include <ace/Dirent_Selector.h>

bool
RPG_Common_File_Tools::isReadable(const std::string& filename_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Common_File_Tools::isReadable"));

  ACE_stat stat;
  if (ACE_OS::stat(filename_in.c_str(),
                   &stat) == -1)
  {
//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("failed to ACE_OS::stat(\"%s\"): \"%m\", aborting\n"),
//                filename_in.c_str()));

    return false;
  } // end IF

  return true;
}

bool
RPG_Common_File_Tools::isEmpty(const std::string& filename_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Common_File_Tools::isEmpty"));

  ACE_stat stat;
  if (ACE_OS::stat(filename_in.c_str(),
                   &stat) == -1)
  {
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("failed to ACE_OS::stat(\"%s\"): \"%m\", aborting\n"),
               filename_in.c_str()));

    return false;
  } // end IF

  return (stat.st_size == 0);
}

bool
RPG_Common_File_Tools::isDirectory(const std::string& directory_in)
{
//   RPG_TRACE(ACE_TEXT("RPG_Common_File_Tools::isDirectory"));

  ACE_stat stat;
  if (ACE_OS::stat(directory_in.c_str(),
                   &stat) == -1)
  {
    switch (errno)
    {
      case ENOENT:
      {
        ACE_DEBUG((LM_DEBUG,
                   ACE_TEXT("\"%s\": \"%m\", aborting\n"),
                   directory_in.c_str()));

        // URI doesn't even exist --> NOT a directory !
        return false;
      }
      default:
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to ACE_OS::stat(\"%s\"): \"%m\", aborting\n"),
                   directory_in.c_str()));

        return false;
      }
    } // end SWITCH
  } // end IF

  return ((stat.st_mode & S_IFMT) == S_IFDIR);
}

bool
RPG_Common_File_Tools::createDirectory(const std::string& directory_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Common_File_Tools::createDirectory"));

  if (ACE_OS::mkdir(directory_in.c_str(),
                    ACE_DEFAULT_DIR_PERMS) == -1)
  {
    switch (errno)
    {
      case ENOENT:
      {
        // OK: some base sub-directory doesn't seem to exist...
        // --> try to recurse
        std::string baseDir = ACE::dirname(directory_in.c_str());

        // sanity check: don't recurse for "." !
        if (baseDir != ACE_TEXT("."))
        {
          if (createDirectory(baseDir))
          {
            // now try again...
            return createDirectory(directory_in);
          } // end IF
        } // end IF

        return false;
      }
      case EEXIST:
      {
        // entry already exists...
        ACE_DEBUG((LM_DEBUG,
                   ACE_TEXT("\"%s\" already exists, leaving\n"),
                   directory_in.c_str()));

        return true;
      }
      default:
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to ACE_OS::mkdir(\"%s\"): \"%m\", aborting\n"),
                   directory_in.c_str()));

        return false;
      }
    } // end SWITCH
  } // end IF

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("created: \"%s\"...\n"),
             directory_in.c_str()));

  return true;
}

bool
RPG_Common_File_Tools::deleteFile(const std::string& filename_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Common_File_Tools::deleteFile"));

  // connect to the file...
  ACE_FILE_Addr address;
  if (address.set(ACE_TEXT_CHAR_TO_TCHAR(filename_in.c_str())) == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_FILE_Addr::set() file \"%s\": \"%m\", aborting\n"),
               filename_in.c_str()));

    return false;
  } // end IF

  // don't want to block on file opening (see ACE doc)...
  ACE_FILE_Connector connector;
  ACE_FILE_IO file;
  if (connector.connect(file,
                        address,
                        const_cast<ACE_Time_Value*>(&ACE_Time_Value::zero),
                        ACE_Addr::sap_any,
                        0,
                        (O_WRONLY | O_BINARY | O_EXCL),
                        ACE_DEFAULT_FILE_PERMS) == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_FILE_Connector::connect() to file \"%s\": \"%m\", aborting\n"),
               filename_in.c_str()));

    return false;
  } // end IF

  // delete file
  if (file.remove() == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_FILE_IO::remove() file \"%s\": \"%m\", aborting\n"),
               filename_in.c_str()));

    return false;
  } // end IF

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("deleted \"%s\"...\n"),
             filename_in.c_str()));

  return true;
}

bool
RPG_Common_File_Tools::loadFile(const std::string& filename_in,
                                unsigned char*& file_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Common_File_Tools::deleteFile"));

  // init return value(s)
  file_out = NULL;

  FILE* fp = NULL;
  long fsize = 0;

  fp = ACE_OS::fopen(filename_in.c_str(),
                     ACE_TEXT_ALWAYS_CHAR("rb"));
  if (!fp)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to open file(\"%s\"): %m, aborting\n"),
               filename_in.c_str()));

    return false;
  } // end IF

  // obtain file size
  if (ACE_OS::fseek(fp,
                    0,
                    SEEK_END))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to seek file(\"%s\"): %m, aborting\n"),
               filename_in.c_str()));

    return false;
  } // end IF
  fsize = ACE_OS::ftell(fp);
  if (fsize == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to tell file(\"%s\"): %m, aborting\n"),
               filename_in.c_str()));

    return false;
  } // end IF
  ACE_OS::rewind(fp);

  // allocate array
  file_out = new(std::nothrow) unsigned char[fsize];
  if (!file_out)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to allocate memory(%d): %m, aborting\n"),
               fsize));

    return false;
  } // end IF

  // read data
  if (ACE_OS::fread(static_cast<void*>(file_out), // target buffer
                    fsize,                        // read everything
                    1,                            // ... all at once
                    fp) != 1)                     // handle
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to read file(\"%s\"): %m, aborting\n"),
               filename_in.c_str()));

    // clean up
    delete[] file_out;
    file_out = NULL;

    return false;
  } // end IF

  if (ACE_OS::fclose(fp))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to close file(\"%s\"): %m, aborting\n"),
               filename_in.c_str()));

    // clean up
    delete[] file_out;
    file_out = NULL;

    return false;
  } // end IF

  return true;
}

std::string
RPG_Common_File_Tools::realPath(const std::string& path_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Common_File_Tools::realPath"));

  // init result(s)
  std::string result;

  char path[PATH_MAX];
  if (ACE_OS::realpath(path_in.c_str(),
                       path) == NULL)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_OS::realpath(\"%s\"): %m, aborting\n"),
               path_in.c_str()));

    return result;
  } // end IF
  result = path;

  return result;
}

std::string
RPG_Common_File_Tools::getWorkingDirectory()
{
  RPG_TRACE(ACE_TEXT("RPG_Common_File_Tools::getWorkingDirectory"));

  std::string result;

  // retrieve working directory
  char cwd[MAXPATHLEN];
  ACE_OS::memset(cwd,
                 0,
                 sizeof(cwd));
  if (ACE_OS::getcwd(cwd, sizeof(cwd)) == NULL)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_OS::getcwd(): \"%m\", aborting\n")));

    return result;
  } // end IF
  result = cwd;

  return result;
}

std::string
RPG_Common_File_Tools::getConfigDataDirectory(const std::string& baseDir_in,
                                              const bool& isConfig_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Common_File_Tools::getConfigDataDirectory"));

  std::string result = baseDir_in;

  if (baseDir_in.empty())
    result = RPG_Common_File_Tools::getWorkingDirectory();

#if defined(_DEBUG) && !defined(DEBUG_RELEASE)
  // *IMPORTANT NOTE*: works only if the working directory has been set to the project
  // root
  result = RPG_Common_File_Tools::getWorkingDirectory();
#endif

  if (!RPG_Common_File_Tools::isDirectory(result))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("not a directory: \"%s\", aborting\n"),
               result.c_str()));

    // clean up
    result.clear();

    return result;
  } // end IF

#if !defined(_DEBUG) && !defined(DEBUG_RELEASE)
  result += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  result += (isConfig_in ? ACE_TEXT_ALWAYS_CHAR(RPG_COMMON_DEF_CONFIG_SUB)
                         : ACE_TEXT_ALWAYS_CHAR(RPG_COMMON_DEF_DATA_SUB));
#endif

  return result;
}

std::string
RPG_Common_File_Tools::getUserHomeDirectory(const std::string& user_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Common_File_Tools::getUserHomeDirectory"));

  std::string result;

  std::string    user = user_in;
  int            success = -1;
  struct passwd  pwd;
  struct passwd* pwd_result = NULL;
  char           buf[RPG_COMMON_BUFSIZE];
//  size_t         bufsize = 0;
  //    bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
  //    if (bufsize == -1)          /* Value was indeterminate */
  //        bufsize = 16384;        /* Should be more than enough */

  if (user.empty())
  {
//    uid_t user_id = ACE_OS::geteuid();
//    success = ACE_OS::getpwuid_r(user_id,            // user id
//                                 &pwd,               // passwd entry
//                                 buf,                // buffer
//                                 RPG_COMMON_BUFSIZE, // buffer size
//                                 &pwd_result);       // result (handle)
//    if (pwd_result == NULL)
//    {
//      if (success == 0)
//        ACE_DEBUG((LM_ERROR,
//                   ACE_TEXT("user \"%u\" not found, falling back\n"),
//                   user_id));
//      else
//        ACE_DEBUG((LM_ERROR,
//                   ACE_TEXT("failed to ACE_OS::getpwuid_r(%u): \"%m\", falling back\n"),
//                   user_id));

//      // fallback
//      result = ACE_OS::getenv(ACE_TEXT(RPG_PLAYER_DEF_ENTITY_REPOSITORY_BASE));
//    } // end IF
//    else result = ACE_TEXT_ALWAYS_CHAR(pwd.pw_dir);

//    return result;
    user = ACE_TEXT_ALWAYS_CHAR(ACE_OS::getenv(ACE_TEXT(RPG_COMMON_DEF_USER_LOGIN_BASE)));
  } // end IF

  success = ACE_OS::getpwnam_r(user.c_str(),       // user name
                               &pwd,               // passwd entry
                               buf,                // buffer
                               RPG_COMMON_BUFSIZE, // buffer size
                               &pwd_result);       // result (handle)
  if (pwd_result == NULL)
  {
    if (success == 0)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("user \"%s\" not found, falling back\n"),
                 user.c_str()));
    else
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_OS::getpwnam_r(\"%s\"): \"%m\", falling back\n"),
                 user.c_str()));

    // fallback
    result = ACE_TEXT_ALWAYS_CHAR(ACE_OS::getenv(ACE_TEXT(RPG_COMMON_DUMP_DIR)));
  } // end IF
  else result = ACE_TEXT_ALWAYS_CHAR(pwd.pw_dir);

  return result;
}

std::string
RPG_Common_File_Tools::getUserGameDirectory(const std::string& user_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Common_File_Tools::getUserGameDirectory"));

  std::string result;

  std::string    user = user_in;
  if (user.empty())
  {
//    uid_t user_id = ACE_OS::geteuid();
//    success = ACE_OS::getpwuid_r(user_id,            // user id
//                                 &pwd,               // passwd entry
//                                 buf,                // buffer
//                                 RPG_COMMON_BUFSIZE, // buffer size
//                                 &pwd_result);       // result (handle)
//    if (pwd_result == NULL)
//    {
//      if (success == 0)
//        ACE_DEBUG((LM_ERROR,
//                   ACE_TEXT("user \"%u\" not found, falling back\n"),
//                   user_id));
//      else
//        ACE_DEBUG((LM_ERROR,
//                   ACE_TEXT("failed to ACE_OS::getpwuid_r(%u): \"%m\", falling back\n"),
//                   user_id));

//      // fallback
//      result = ACE_OS::getenv(ACE_TEXT(RPG_PLAYER_DEF_ENTITY_REPOSITORY_BASE));
//    } // end IF
//    else result = ACE_TEXT_ALWAYS_CHAR(pwd.pw_dir);

//    return result;
    user = ACE_TEXT_ALWAYS_CHAR(ACE_OS::getenv(ACE_TEXT(RPG_COMMON_DEF_USER_LOGIN_BASE)));
  } // end IF

  result = RPG_Common_File_Tools::getUserHomeDirectory(user);
  if (result.empty())
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Common_File_Tools::getUserHomeDirectory(\"%s\"), falling back\n"),
               user.c_str()));

    // fallback
    result = ACE_TEXT_ALWAYS_CHAR(ACE_OS::getenv(ACE_TEXT(RPG_COMMON_DUMP_DIR)));

    return result;
  } // end IF

#ifdef META_PACKAGE
  result += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  result += ACE_TEXT_ALWAYS_CHAR(".");
  result += ACE_TEXT_ALWAYS_CHAR(META_PACKAGE);
  if (!RPG_Common_File_Tools::createDirectory(result))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Common_File_Tools::createDirectory(\"%s\"), falling back\n"),
               result.c_str()));

    // fallback
    result = RPG_Common_File_Tools::getUserHomeDirectory(user);
  } // end IF
  else
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("created game directory \"%s\"\n"),
               result.c_str()));
#endif

  return result;
}
