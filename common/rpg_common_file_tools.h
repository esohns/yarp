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

#include <ace/Global_Macros.h>

#include <string>

class RPG_Common_File_Tools
{
 public:
  static const bool isReadable(const std::string&); // FQ filename
  static const bool isEmpty(const std::string&); // FQ filename
  static const bool isDirectory(const std::string&); // directory
  static const bool createDirectory(const std::string&); // directory
  static const bool deleteFile(const std::string&); // FQ filename

 private:
  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Common_File_Tools());
  ACE_UNIMPLEMENTED_FUNC(virtual ~RPG_Common_File_Tools());
  ACE_UNIMPLEMENTED_FUNC(RPG_Common_File_Tools(const RPG_Common_File_Tools&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Common_File_Tools& operator=(const RPG_Common_File_Tools&));
};

#endif
