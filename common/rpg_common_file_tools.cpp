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

#include "rpg_common_file_tools.h"

#include <ace/OS.h>
#include <ace/ACE.h>
#include <ace/FILE_IO.h>
#include <ace/FILE_Connector.h>
#include <ace/Dirent_Selector.h>

const bool
RPG_Common_File_Tools::isReadable(const std::string& filename_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Common_File_Tools::isReadable"));

  ACE_stat stat;
  if (ACE_OS::stat(filename_in.c_str(),
                   &stat) == -1)
  {
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("failed to ACE_OS::stat(\"%s\"): \"%s\", aborting\n"),
               filename_in.c_str(),
               ACE_OS::strerror(errno)));

    return false;
  } // end IF

  return true;
}

const bool
RPG_Common_File_Tools::isEmpty(const std::string& filename_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Common_File_Tools::isEmpty"));

  ACE_stat stat;
  if (ACE_OS::stat(filename_in.c_str(),
                   &stat) == -1)
  {
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("failed to ACE_OS::stat(\"%s\"): \"%s\", aborting\n"),
               filename_in.c_str(),
               ACE_OS::strerror(errno)));

    return false;
  } // end IF

  return (stat.st_size == 0);
}

const bool
RPG_Common_File_Tools::isDirectory(const std::string& directory_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Common_File_Tools::isDirectory"));

  ACE_stat stat;
  if (ACE_OS::stat(directory_in.c_str(),
                   &stat) == -1)
  {
    switch (errno)
    {
      case ENOENT:
      {
        // debug info
        ACE_DEBUG((LM_DEBUG,
                   ACE_TEXT("\"%s\": \"%s\", aborting\n"),
                   directory_in.c_str(),
                   ACE_OS::strerror(errno)));

        return false;
      }
      default:
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to ACE_OS::stat(\"%s\"): \"%s\", aborting\n"),
                   directory_in.c_str(),
                   ACE_OS::strerror(errno)));

        return false;
      }
    } // end SWITCH
  } // end IF

  return ((stat.st_mode & S_IFMT) == S_IFDIR);
}

const bool
RPG_Common_File_Tools::createDirectory(const std::string& directory_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Common_File_Tools::createDirectory"));

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
                   ACE_TEXT("failed to ACE_OS::mkdir(\"%s\"): %s, aborting\n"),
                   directory_in.c_str(),
                   ACE_OS::strerror(errno)));

        return false;
      }
    } // end SWITCH
  } // end IF

  // debug info
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("created: \"%s\"...\n"),
             directory_in.c_str()));

  return true;
}

const bool
RPG_Common_File_Tools::deleteFile(const std::string& filename_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Common_File_Tools::deleteFile"));

  // connect to the file...
  ACE_FILE_Addr address;
  if (address.set(ACE_TEXT_CHAR_TO_TCHAR(filename_in.c_str())) == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_FILE_Addr::set() file \"%s\": \"%s\", aborting\n"),
               filename_in.c_str(),
               ACE_OS::strerror(errno)));

    return false;
  } // end IF

  // don't want to block on file opening (see ACE doc)...
  ACE_FILE_Connector connector;
  ACE_FILE_IO file;
  if (connector.connect(file,
                        address,
                        ACE_const_cast(ACE_Time_Value*,
                                       &ACE_Time_Value::zero),
                        ACE_Addr::sap_any,
                        0,
                        (O_WRONLY | O_BINARY | O_EXCL),
                        ACE_DEFAULT_FILE_PERMS) == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_FILE_Connector::connect() to file \"%s\": \"%s\", aborting\n"),
               filename_in.c_str(),
               ACE_OS::strerror(errno)));

    return false;
  } // end IF

  // delete file
  if (file.remove() == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_FILE_IO::remove() file \"%s\": \"%s\", aborting\n"),
               filename_in.c_str(),
               ACE_OS::strerror(errno)));

    return false;
  } // end IF

  // debug info
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("deleted \"%s\"...\n"),
             filename_in.c_str()));

  return true;
}
