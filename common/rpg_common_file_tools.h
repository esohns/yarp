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

#include <string>

#include "ace/Global_Macros.h"

class RPG_Common_File_Tools
{
 public:
  static std::string getSourceDirectory (const std::string&,  // package name
                                         const std::string&); // module name
  static std::string getConfigurationDataDirectory (const std::string&, // package name
                                                    const std::string&, // module name
                                                    const std::string&, // module2/executable name
                                                    bool);              // configuration ? : data

  // *NOTE*: (try to) create the directory if it doesn't exist
  static std::string getUserConfigurationDirectory ();

  static std::string getLogFilename (const std::string&); // program name

 private:
  ACE_UNIMPLEMENTED_FUNC (RPG_Common_File_Tools ());
  ACE_UNIMPLEMENTED_FUNC (virtual ~RPG_Common_File_Tools ());
  ACE_UNIMPLEMENTED_FUNC (RPG_Common_File_Tools (const RPG_Common_File_Tools&));
  ACE_UNIMPLEMENTED_FUNC (RPG_Common_File_Tools& operator= (const RPG_Common_File_Tools&));

  // helper methods
//  static int dirent_selector(const dirent*);
//  static int dirent_comparator(const dirent**,
//                               const dirent**);
  static std::string getLogDirectory ();
};

#endif
