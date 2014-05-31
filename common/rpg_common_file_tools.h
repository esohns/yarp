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

#ifndef RPG_COMMON_FILE_TOOLS_H
#define RPG_COMMON_FILE_TOOLS_H

#include "rpg_common_exports.h"

#include <ace/Global_Macros.h>

#include <string>

class RPG_Common_Export RPG_Common_File_Tools
{
 public:
  static bool isReadable(const std::string&); // FQ filename
  static bool isEmpty(const std::string&); // FQ filename
  static bool isDirectory(const std::string&); // directory
  static bool isEmptyDirectory(const std::string&); // directory
  static bool createDirectory(const std::string&); // directory
  static bool copyFile(const std::string&,  // FQ filename
                       const std::string&); // directory
  static bool deleteFile(const std::string&); // FQ filename
  // *NOTE*: users need to free (delete[]) the returned buffer
  static bool loadFile(const std::string&, // FQ filename
                       unsigned char*&);   // return value: memory buffer (array)

  static std::string realPath(const std::string&); // path

  static std::string getWorkingDirectory();
  // *PORTABILITY*: - on UNIX, this should be passed as a BASEDIR macro at compile time
  //                - on WIN32, this should default to the working directory (*TODO*)
  static std::string getConfigurationDataDirectory(const std::string&, // base directory
                                                   const bool&);       // config ? : data

  static std::string getUserHomeDirectory(const std::string&); // user
  // *NOTE*: (try to) create the directory if it doesn't exist
  static std::string getUserConfigurationDirectory();

  static std::string getDumpDirectory();

  static std::string getLogFilename(const std::string&); // program name

 private:
  ACE_UNIMPLEMENTED_FUNC(RPG_Common_File_Tools());
  ACE_UNIMPLEMENTED_FUNC(virtual ~RPG_Common_File_Tools());
  ACE_UNIMPLEMENTED_FUNC(RPG_Common_File_Tools(const RPG_Common_File_Tools&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Common_File_Tools& operator=(const RPG_Common_File_Tools&));

  // helper methods
//  static int dirent_selector(const dirent*);
//  static int dirent_comparator(const dirent**,
//                               const dirent**);
};

#endif
