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

#ifndef RPG_NET_SERVER_COMMON_TOOLS_H
#define RPG_NET_SERVER_COMMON_TOOLS_H

#include "rpg_net_server_exports.h"

#include <ace/Global_Macros.h>

#include <string>

// forward declaration(s)
struct dirent;

class RPG_Net_Server_Export RPG_Net_Server_Common_Tools
{
 public:
  // *NOTE*: implements log rotation
  static bool getNextLogFilename(const std::string&, // directory
                                 std::string&);      // return value: log filename

 private:
  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Server_Common_Tools());
  ACE_UNIMPLEMENTED_FUNC(virtual ~RPG_Net_Server_Common_Tools());
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Server_Common_Tools(const RPG_Net_Server_Common_Tools&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Server_Common_Tools& operator=(const RPG_Net_Server_Common_Tools&));

  // *NOTE*: implements log rotation
  // callbacks used for scandir...
  static int selector(const dirent*); // directory entry
  static int comparator(const dirent**,  // directory entry
                        const dirent**); // directory entry
  static unsigned int myMaxNumberOfLogFiles;
};

#endif
