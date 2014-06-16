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

#if defined(ACE_WIN32) || defined(ACE_WIN64)
#include "Userenv.h"
#include "Shlobj.h"
#endif

#include <ace/ACE.h>
#include <ace/OS.h>
#include <ace/FILE_IO.h>
#include <ace/FILE_Connector.h>
#include <ace/Dirent_Selector.h>
#include <ace/OS_NS_sys_sendfile.h>

#include "rpg_common_macros.h"
#include "rpg_common_defines.h"
#include "rpg_common_tools.h"

// *NOTE*: need this to import correct VERSION !
#ifdef HAVE_CONFIG_H
#include "rpg_config.h"
#endif

bool
RPG_Common_File_Tools::isReadable(const std::string& filename_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Common_File_Tools::isReadable"));

  ACE_stat stat;
  ACE_OS::memset(&stat, 0, sizeof(stat));
  if (ACE_OS::stat(filename_in.c_str(),
                   &stat) == -1)
  {
//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("failed to ACE_OS::stat(\"%s\"): \"%m\", aborting\n"),
//                ACE_TEXT(filename_in.c_str())));

    return false;
  } // end IF

  return true;
}

bool
RPG_Common_File_Tools::isEmpty(const std::string& filename_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Common_File_Tools::isEmpty"));

  ACE_stat stat;
  ACE_OS::memset(&stat, 0, sizeof(stat));
  if (ACE_OS::stat(filename_in.c_str(),
                   &stat) == -1)
  {
//    ACE_DEBUG((LM_DEBUG,
//               ACE_TEXT("failed to ACE_OS::stat(\"%s\"): \"%m\", aborting\n"),
//               ACE_TEXT(filename_in.c_str())));

    return false;
  } // end IF

  return (stat.st_size == 0);
}

bool
RPG_Common_File_Tools::isDirectory(const std::string& directory_in)
{
//   RPG_TRACE(ACE_TEXT("RPG_Common_File_Tools::isDirectory"));

  ACE_stat stat;
  ACE_OS::memset(&stat, 0, sizeof(stat));
  if (ACE_OS::stat(directory_in.c_str(),
                   &stat) == -1)
  {
    switch (errno)
    {
      case ENOENT:
      {
        ACE_DEBUG((LM_DEBUG,
                   ACE_TEXT("\"%s\": \"%m\", aborting\n"),
                   ACE_TEXT(directory_in.c_str())));

        // URI doesn't even exist --> NOT a directory !
        return false;
      }
      default:
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to ACE_OS::stat(\"%s\"): \"%m\", aborting\n"),
                   ACE_TEXT(directory_in.c_str())));

        return false;
      }
    } // end SWITCH
  } // end IF

  return ((stat.st_mode & S_IFMT) == S_IFDIR);
}

//int
//RPG_Common_File_Tools::selector(const dirent* dirEntry_in)
//{
//  RPG_TRACE(ACE_TEXT("RPG_Common_File_Tools::selector"));

//  // *IMPORTANT NOTE*: select all files

//  // sanity check --> ignore dot/double-dot
//  if (ACE_OS::strncmp(dirEntry_in->d_name,
//                      ACE_TEXT_ALWAYS_CHAR(RPG_NET_SERVER_LOG_FILENAME_PREFIX),
//                      ACE_OS::strlen(ACE_TEXT_ALWAYS_CHAR(RPG_NET_SERVER_LOG_FILENAME_PREFIX))) != 0)
//  {
////     ACE_DEBUG((LM_DEBUG,
////                ACE_TEXT("ignoring \"%s\"...\n"),
////                ACE_TEXT(dirEntry_in->d_name)));

//    return 0;
//  } // end IF

//  return 1;
//}

//int
//RPG_Common_File_Tools::comparator(const dirent** d1,
//                                  const dirent** d2)
//{
//  RPG_TRACE(ACE_TEXT("RPG_Common_File_Tools::comparator"));

//  return ACE_OS::strcmp((*d1)->d_name,
//                        (*d2)->d_name);
//}

bool
RPG_Common_File_Tools::isEmptyDirectory(const std::string& directory_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Common_File_Tools::isEmptyDirectory"));

  // init return value
  bool result = false;

  ACE_Dirent_Selector entries;
  if (entries.open(ACE_TEXT(directory_in.c_str()),
//                   &RPG_Common_File_Tools::dirent_selector,
//                   &RPG_Common_File_Tools::dirent_comparator) == -1)
                   NULL,
                   NULL) == -1)
  {
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("failed to ACE_Dirent_Selector::open(\"%s\"): \"%m\", aborting\n"),
               ACE_TEXT(directory_in.c_str())));

    return result;
  } // end IF
  result = (entries.length() == 0);

  // clean up
  if (entries.close() == -1)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Dirent_Selector::close(\"%s\"): \"%m\", continuing\n"),
               ACE_TEXT(directory_in.c_str())));

  return result;
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
        const ACE_TCHAR* dirname = ACE::dirname(directory_in.c_str(),
                                                ACE_DIRECTORY_SEPARATOR_CHAR);
        if (!dirname)
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("failed to ACE::dirname(\"%s\"): \"%m\", aborting\n"),
                     ACE_TEXT(directory_in.c_str())));

          return false;
        } // end IF
        std::string base_directory = ACE_TEXT_ALWAYS_CHAR(dirname);
        // sanity check: don't recurse for "." !
        if (base_directory != ACE_TEXT_ALWAYS_CHAR("."))
        {
          if (createDirectory(base_directory))
            return createDirectory(directory_in);
          else
            ACE_DEBUG((LM_ERROR,
                       ACE_TEXT("failed to RPG_Common_File_Tools::createDirectory(\"%s\"): \"%m\", aborting\n"),
                       ACE_TEXT(base_directory.c_str())));
        } // end IF

        return false;
      }
      case EEXIST:
      {
        // entry already exists...
        ACE_DEBUG((LM_DEBUG,
                   ACE_TEXT("\"%s\" already exists, leaving\n"),
                   ACE_TEXT(directory_in.c_str())));

        return true;
      }
      default:
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to ACE_OS::mkdir(\"%s\"): \"%m\", aborting\n"),
                   ACE_TEXT(directory_in.c_str())));

        return false;
      }
    } // end SWITCH
  } // end IF

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("created: \"%s\"...\n"),
             ACE_TEXT(directory_in.c_str())));

  return true;
}

bool
RPG_Common_File_Tools::copyFile(const std::string& filename_in,
                                const std::string& directory_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Common_File_Tools::copyFile"));

  // connect to the file...
  ACE_FILE_Addr source_address, target_address;
  if (source_address.set(ACE_TEXT_CHAR_TO_TCHAR(filename_in.c_str())) == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_FILE_Addr::set() file \"%s\": \"%m\", aborting\n"),
               ACE_TEXT(filename_in.c_str())));

    return false;
  } // end IF
  const ACE_TCHAR* basename = ACE::basename(filename_in.c_str(),
                                            ACE_DIRECTORY_SEPARATOR_CHAR);
  if (!basename)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE::basename(\"%s\"): \"%m\", aborting\n"),
               ACE_TEXT(filename_in.c_str())));

    return false;
  } // end IF
  std::string target_filename = directory_in;
  target_filename += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  target_filename += ACE_TEXT_ALWAYS_CHAR(basename);
  if (target_address.set(ACE_TEXT_CHAR_TO_TCHAR(target_filename.c_str())) == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_FILE_Addr::set() file \"%s\": \"%m\", aborting\n"),
               ACE_TEXT(target_filename.c_str())));

    return false;
  } // end IF

  // don't block on file opening (see ACE doc)...
  ACE_FILE_Connector connector, receptor;
  ACE_FILE_IO source_file, target_file;
  if (connector.connect(source_file,                                        // file
                        source_address,                                     // remote SAP
                        const_cast<ACE_Time_Value*>(&ACE_Time_Value::zero), // timeout
                        ACE_Addr::sap_any,                                  // local SAP
                        0,                                                  // reuse address ?
                        (O_RDONLY | O_BINARY | O_EXCL),                     // flags
                        ACE_DEFAULT_FILE_PERMS) == -1)                      // permissions
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_FILE_Connector::connect() to file \"%s\": \"%m\", aborting\n"),
               ACE_TEXT(filename_in.c_str())));

    return false;
  } // end IF
  if (receptor.connect(target_file,                                        // file
                       target_address,                                     // remote SAP
                       const_cast<ACE_Time_Value*>(&ACE_Time_Value::zero), // timeout
                       ACE_Addr::sap_any,                                  // local SAP
                       0,                                                  // reuse address ?
                       (O_WRONLY | O_CREAT | O_BINARY | O_TRUNC),          // flags
                       ACE_DEFAULT_FILE_PERMS) == -1)                      // permissions
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_FILE_Connector::connect() to file \"%s\": \"%m\", aborting\n"),
               ACE_TEXT(target_filename.c_str())));

    // clean up
    if (source_file.close() == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_FILE::close(\"%s\"): \"%m\", continuing\n"),
                 ACE_TEXT(filename_in.c_str())));

    return false;
  } // end IF

  // copy file
//  iovec io_vec;
//  ACE_OS::memset(&io_vec, 0, sizeof(io_vec));
//  if (source_file.recvv(&io_vec) == -1)
//  {
//    ACE_DEBUG((LM_ERROR,
//               ACE_TEXT("failed to ACE_FILE_IO::recvv() file \"%s\": \"%m\", aborting\n"),
//               ACE_TEXT(filename_in.c_str())));

//    // clean up
//    if (source_file.close() == -1)
//      ACE_DEBUG((LM_ERROR,
//                 ACE_TEXT("failed to ACE_FILE::close(\"%s\"): \"%m\", continuing\n"),
//                 ACE_TEXT(filename_in.c_str())));
//    if (target_file.remove() == -1)
//      ACE_DEBUG((LM_ERROR,
//                 ACE_TEXT("failed to ACE_FILE::remove(\"%s\"): \"%m\", continuing\n"),
//                 ACE_TEXT(target_filename.c_str())));

//    return false;
//  } // end IF
//  ACE_ASSERT(io_vec.iov_base);
//  if (target_file.sendv_n(&io_vec, 1) == -1)
//  {
//    ACE_DEBUG((LM_ERROR,
//               ACE_TEXT("failed to ACE_FILE_IO::sendv_n() file \"%s\": \"%m\", aborting\n"),
//               ACE_TEXT(target_filename.c_str())));

//    // clean up
//    delete[] io_vec.iov_base;
//    if (source_file.close() == -1)
//      ACE_DEBUG((LM_ERROR,
//                 ACE_TEXT("failed to ACE_FILE::close(\"%s\"): \"%m\", continuing\n"),
//                 ACE_TEXT(filename_in.c_str())));
//    if (target_file.remove() == -1)
//      ACE_DEBUG((LM_ERROR,
//                 ACE_TEXT("failed to ACE_FILE::remove(\"%s\"): \"%m\", continuing\n"),
//                 ACE_TEXT(target_filename.c_str())));

//    return false;
//  } // end IF
  ACE_FILE_Info file_info;
  if (source_file.get_info(file_info) == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_FILE_IO::get_info(\"%s\"): \"%m\", aborting\n"),
               ACE_TEXT(filename_in.c_str())));

    // clean up
    if (source_file.close() == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_FILE::close(\"%s\"): \"%m\", continuing\n"),
                 ACE_TEXT(filename_in.c_str())));
    if (target_file.remove() == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_FILE::remove(\"%s\"): \"%m\", continuing\n"),
                 ACE_TEXT(target_filename.c_str())));

    return false;
  } // end IF
  if (ACE_OS::sendfile(source_file.get_handle(),
                       target_file.get_handle(),
                       NULL,
                       file_info.size_) != file_info.size_)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_OS::sendfile(\"%s\",\"%s\"): \"%m\", aborting\n"),
               ACE_TEXT(filename_in.c_str()),
               ACE_TEXT(target_filename.c_str())));

    // clean up
    if (source_file.close() == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_FILE::close(\"%s\"): \"%m\", continuing\n"),
                 ACE_TEXT(filename_in.c_str())));
    if (target_file.remove() == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_FILE::remove(\"%s\"): \"%m\", continuing\n"),
                 ACE_TEXT(target_filename.c_str())));

    return false;
  } // end IF

  // clean up
//  delete[] io_vec.iov_base;
  if (source_file.close() == -1)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_FILE::close(\"%s\"): \"%m\", continuing\n"),
               ACE_TEXT(filename_in.c_str())));
  if (target_file.close() == -1)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_FILE::close(\"%s\"): \"%m\", continuing\n"),
               ACE_TEXT(target_filename.c_str())));

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("copied \"%s\" --> \"%s\"...\n"),
             ACE_TEXT(filename_in.c_str()),
             ACE_TEXT(target_filename.c_str())));

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
               ACE_TEXT(filename_in.c_str())));

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
                        (O_WRONLY | O_BINARY),
                        ACE_DEFAULT_FILE_PERMS) == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_FILE_Connector::connect() to file \"%s\": \"%m\", aborting\n"),
               ACE_TEXT(filename_in.c_str())));

    return false;
  } // end IF

  // delete file
  if (file.remove() == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_FILE_IO::remove() file \"%s\": \"%m\", aborting\n"),
               ACE_TEXT(filename_in.c_str())));

    return false;
  } // end IF

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("deleted \"%s\"...\n"),
             ACE_TEXT(filename_in.c_str())));

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
  fp = ACE_OS::fopen(ACE_TEXT(filename_in.c_str()),
                     ACE_TEXT_ALWAYS_CHAR("rb"));
  if (!fp)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_OS::fopen(\"%s\"): %m, aborting\n"),
               ACE_TEXT(filename_in.c_str())));

    return false;
  } // end IF

  // obtain file size
  if (ACE_OS::fseek(fp, 0, SEEK_END))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_OS::fseek(\"%s\"): %m, aborting\n"),
               ACE_TEXT(filename_in.c_str())));

    // clean up
    if (ACE_OS::fclose(fp) == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_OS::fclose(\"%s\"): %m, continuing\n"),
                 ACE_TEXT(filename_in.c_str())));

    return false;
  } // end IF
  long fsize = 0;
  fsize = ACE_OS::ftell(fp);
  if (fsize == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_OS::ftell(\"%s\"): %m, aborting\n"),
               ACE_TEXT(filename_in.c_str())));

    // clean up
    if (ACE_OS::fclose(fp) == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_OS::fclose(\"%s\"): %m, continuing\n"),
                 ACE_TEXT(filename_in.c_str())));

    return false;
  } // end IF
  ACE_OS::rewind(fp);

  // *PORTABILITY* allocate array
  file_out = new(std::nothrow) unsigned char[fsize];
  if (!file_out)
  {
    ACE_DEBUG((LM_CRITICAL,
               ACE_TEXT("failed to allocate memory(%d): %m, aborting\n"),
               fsize));

    // clean up
    if (ACE_OS::fclose(fp) == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_OS::fclose(\"%s\"): %m, continuing\n"),
                 ACE_TEXT(filename_in.c_str())));

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
               ACE_TEXT(filename_in.c_str())));

    // clean up
    if (ACE_OS::fclose(fp) == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_OS::fclose(\"%s\"): %m, continuing\n"),
                 ACE_TEXT(filename_in.c_str())));
    delete[] file_out;
    file_out = NULL;

    return false;
  } // end IF

  // clean up
  if (ACE_OS::fclose(fp) == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_OS::fclose(\"%s\"): %m, aborting\n"),
               ACE_TEXT(filename_in.c_str())));

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
               ACE_TEXT(path_in.c_str())));

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
  char cwd[PATH_MAX];
  ACE_OS::memset(cwd, 0, sizeof(cwd));
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
RPG_Common_File_Tools::getConfigurationDataDirectory(const std::string& baseDir_in,
                                                     const bool& isConfig_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Common_File_Tools::getConfigurationDataDirectory"));

  std::string result = baseDir_in;

  if (baseDir_in.empty())
    return RPG_Common_File_Tools::getWorkingDirectory();

  if (!RPG_Common_File_Tools::isDirectory(result))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("not a directory: \"%s\", aborting\n"),
               ACE_TEXT(result.c_str())));

    // clean up
    result.clear();

    return result;
  } // end IF

  result += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  result += (isConfig_in ? ACE_TEXT_ALWAYS_CHAR(RPG_COMMON_CONFIG_SUB)
                         : ACE_TEXT_ALWAYS_CHAR(RPG_COMMON_DATA_SUB));

  return result;
}

std::string
RPG_Common_File_Tools::getUserHomeDirectory(const std::string& user_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Common_File_Tools::getUserHomeDirectory"));

  // init result value(s)
  std::string result;

  std::string user_name = user_in;
  if (user_name.empty())
  {
    std::string real_name;
    RPG_Common_Tools::getCurrentUserName(user_name, real_name);
    if (user_name.empty())
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to RPG_Common_Tools::getCurrentUserName(), falling back\n")));

      // fallback
      result =
          ACE_TEXT_ALWAYS_CHAR(ACE_OS::getenv(ACE_TEXT(RPG_COMMON_DEF_DUMP_DIR)));
      return result;
    } // end IF
  } // end IF

#if !defined(ACE_WIN32) && !defined(ACE_WIN64)
  int            success = -1;
  struct passwd  pwd;
  struct passwd* pwd_result = NULL;
  char           buffer[RPG_COMMON_BUFSIZE];
  ACE_OS::memset(buffer, 0, sizeof(RPG_COMMON_BUFSIZE));
  success = ACE_OS::getpwnam_r(user_name.c_str(),  // user name
                               &pwd,               // passwd entry
                               buffer,             // buffer
                               RPG_COMMON_BUFSIZE, // buffer size
                               &pwd_result);       // result (handle)
  if (pwd_result == NULL)
  {
    if (success == 0)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("user \"%s\" not found, falling back\n"),
                 ACE_TEXT(user_name.c_str())));
    else
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_OS::getpwnam_r(\"%s\"): \"%m\", falling back\n"),
                 ACE_TEXT(user_name.c_str())));

    // fallback
    result =
        ACE_TEXT_ALWAYS_CHAR(ACE_OS::getenv(ACE_TEXT(RPG_COMMON_DEF_DUMP_DIR)));
  } // end IF
  else result = ACE_TEXT_ALWAYS_CHAR(pwd.pw_dir);
#else
  HANDLE token = 0;
  if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token) == 0)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to OpenProcessToken(), falling back\n")));

    // fallback
    result =
        ACE_TEXT_ALWAYS_CHAR(ACE_OS::getenv(ACE_TEXT(RPG_COMMON_DEF_DUMP_DIR)));
    return result;
  } // end IF

  TCHAR buffer[PATH_MAX];
  ACE_OS::memset(buffer, 0, sizeof(buffer));
  DWORD buffer_size = PATH_MAX;
  if (!GetUserProfileDirectory(token, buffer, &buffer_size))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to GetUserProfileDirectory(): \"%s\", falling back\n")));

    // clean up
    if (!CloseHandle(token))
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to CloseHandle(), continuing\n")));

    // fallback
    result =
        ACE_TEXT_ALWAYS_CHAR(ACE_OS::getenv(ACE_TEXT(RPG_COMMON_DEF_DUMP_DIR)));
    return result;
  } // end IF

  // clean up
  if (!CloseHandle(token))
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to CloseHandle(), continuing\n")));

  result = ACE_TEXT_ALWAYS_CHAR(buffer);
#endif

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
  RPG_Common_Tools::getCurrentUserName(user_name, real_name);
  if (user_name.empty())
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Common_Tools::getCurrentUserName(), falling back\n")));

    // fallback
    result =
        ACE_TEXT_ALWAYS_CHAR(ACE_OS::getenv(ACE_TEXT(RPG_COMMON_DEF_DUMP_DIR)));
    return result;
  } // end IF

  result = RPG_Common_File_Tools::getUserHomeDirectory(user_name);
  if (result.empty())
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Common_File_Tools::getUserHomeDirectory(\"%s\"), falling back\n"),
               ACE_TEXT(user_name.c_str())));

    // fallback
    result =
        ACE_TEXT_ALWAYS_CHAR(ACE_OS::getenv(ACE_TEXT(RPG_COMMON_DEF_DUMP_DIR)));
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
        ACE_TEXT_ALWAYS_CHAR(ACE_OS::getenv(ACE_TEXT(RPG_COMMON_DEF_DUMP_DIR)));
    return result;
	} // end IF

  result = ACE_TEXT_ALWAYS_CHAR(buffer);
  result += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  result += ACE_TEXT_ALWAYS_CHAR(RPG_PACKAGE);

  if (!RPG_Common_File_Tools::isDirectory(result))
  {
    if (!RPG_Common_File_Tools::createDirectory(result))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to RPG_Common_File_Tools::createDirectory(\"%s\"), falling back\n"),
                 ACE_TEXT(result.c_str())));

      // fallback
      result =
          ACE_TEXT_ALWAYS_CHAR(ACE_OS::getenv(ACE_TEXT(RPG_COMMON_DEF_DUMP_DIR)));
    } // end IF
    else
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("created game directory \"%s\"\n"),
                 ACE_TEXT(result.c_str())));
  } // end IF

  return result;
}

std::string
RPG_Common_File_Tools::getDumpDirectory()
{
  RPG_TRACE(ACE_TEXT("RPG_Common_File_Tools::getDumpDirectory"));

  // init return value(s)
  std::string result;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  result = ACE_OS::getenv(ACE_TEXT_ALWAYS_CHAR(RPG_COMMON_DEF_DUMP_DIR));
#else
  result = ACE_TEXT_ALWAYS_CHAR(RPG_COMMON_DEF_DUMP_DIR);
#endif

  // sanity check(s): directory exists ?
  // No ? --> try to create it then !
  if (!RPG_Common_File_Tools::isDirectory(result))
  {
    if (!RPG_Common_File_Tools::createDirectory(result))
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
  result += RPG_COMMON_LOG_FILENAME_SUFFIX;

  // sanity check(s): log file exists ?
  // Yes ? --> try to delete it then !
  if (RPG_Common_File_Tools::isReadable(result))
  {
    if (!RPG_Common_File_Tools::deleteFile(result))
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
  result = ACE_OS::getenv(ACE_TEXT_ALWAYS_CHAR(RPG_COMMON_DEF_LOG_DIRECTORY));
#else
  result = ACE_TEXT_ALWAYS_CHAR(RPG_COMMON_DEF_LOG_DIRECTORY);
#endif

  // sanity check(s): directory exists ?
  // No ? --> try to create it then !
  if (!RPG_Common_File_Tools::isDirectory(result))
  {
    if (!RPG_Common_File_Tools::createDirectory(result))
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
