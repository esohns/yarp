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

#ifndef RPG_COMMON_CAMP_H
#define RPG_COMMON_CAMP_H

enum RPG_Common_Camp
{
  REST_FULL = 0,
  REST_NORMAL,
  //
  RPG_COMMON_CAMP_MAX,
  RPG_COMMON_CAMP_INVALID
};

#include <ace/Global_Macros.h>

#include <map>
#include <string>

typedef std::map<RPG_Common_Camp, std::string> RPG_Common_CampToStringTable_t;
typedef RPG_Common_CampToStringTable_t::const_iterator RPG_Common_CampToStringTableIterator_t;

class RPG_Common_CampHelper
{
 public:
  inline static void init()
  {
    myRPG_Common_CampToStringTable.clear();
    myRPG_Common_CampToStringTable.insert(std::make_pair(REST_FULL, ACE_TEXT_ALWAYS_CHAR("REST_FULL")));
    myRPG_Common_CampToStringTable.insert(std::make_pair(REST_NORMAL, ACE_TEXT_ALWAYS_CHAR("REST_NORMAL")));
  };

  inline static std::string RPG_Common_CampToString(const RPG_Common_Camp& element_in)
  {
    std::string result;
    RPG_Common_CampToStringTableIterator_t iterator = myRPG_Common_CampToStringTable.find(element_in);
    if (iterator != myRPG_Common_CampToStringTable.end())
      result = iterator->second;
    else
      result = ACE_TEXT_ALWAYS_CHAR("RPG_COMMON_CAMP_INVALID");

    return result;
  };

  inline static RPG_Common_Camp stringToRPG_Common_Camp(const std::string& string_in)
  {
    RPG_Common_CampToStringTableIterator_t iterator = myRPG_Common_CampToStringTable.begin();
    do
    {
      if (iterator->second == string_in)
        return iterator->first;

      iterator++;
    } while (iterator != myRPG_Common_CampToStringTable.end());

    return RPG_COMMON_CAMP_INVALID;
  };

  static RPG_Common_CampToStringTable_t myRPG_Common_CampToStringTable;

 private:
  ACE_UNIMPLEMENTED_FUNC(RPG_Common_CampHelper());
  ACE_UNIMPLEMENTED_FUNC(RPG_Common_CampHelper(const RPG_Common_CampHelper&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Common_CampHelper& operator=(const RPG_Common_CampHelper&));
};

#endif
