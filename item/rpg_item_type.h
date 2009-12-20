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

#ifndef RPG_ITEM_TYPE_H
#define RPG_ITEM_TYPE_H

enum RPG_Item_Type
{
  ITEM_ARMOR = 0,
  ITEM_GOODS,
  ITEM_OTHER,
  ITEM_VALUABLE,
  ITEM_WEAPON,
  //
  RPG_ITEM_TYPE_MAX,
  RPG_ITEM_TYPE_INVALID
};

#include <ace/Global_Macros.h>

#include <map>
#include <string>

typedef std::map<RPG_Item_Type, std::string> RPG_Item_TypeToStringTable_t;
typedef RPG_Item_TypeToStringTable_t::const_iterator RPG_Item_TypeToStringTableIterator_t;

class RPG_Item_TypeHelper
{
 public:
  inline static void init()
  {
    myRPG_Item_TypeToStringTable.clear();
    myRPG_Item_TypeToStringTable.insert(std::make_pair(ITEM_ARMOR, ACE_TEXT_ALWAYS_CHAR("ITEM_ARMOR")));
    myRPG_Item_TypeToStringTable.insert(std::make_pair(ITEM_GOODS, ACE_TEXT_ALWAYS_CHAR("ITEM_GOODS")));
    myRPG_Item_TypeToStringTable.insert(std::make_pair(ITEM_OTHER, ACE_TEXT_ALWAYS_CHAR("ITEM_OTHER")));
    myRPG_Item_TypeToStringTable.insert(std::make_pair(ITEM_VALUABLE, ACE_TEXT_ALWAYS_CHAR("ITEM_VALUABLE")));
    myRPG_Item_TypeToStringTable.insert(std::make_pair(ITEM_WEAPON, ACE_TEXT_ALWAYS_CHAR("ITEM_WEAPON")));
  };

  inline static std::string RPG_Item_TypeToString(const RPG_Item_Type& element_in)
  {
    std::string result;
    RPG_Item_TypeToStringTableIterator_t iterator = myRPG_Item_TypeToStringTable.find(element_in);
    if (iterator != myRPG_Item_TypeToStringTable.end())
      result = iterator->second;
    else
      result = ACE_TEXT_ALWAYS_CHAR("RPG_ITEM_TYPE_INVALID");

    return result;
  };

  inline static RPG_Item_Type stringToRPG_Item_Type(const std::string& string_in)
  {
    RPG_Item_TypeToStringTableIterator_t iterator = myRPG_Item_TypeToStringTable.begin();
    do
    {
      if (iterator->second == string_in)
        return iterator->first;

      iterator++;
    } while (iterator != myRPG_Item_TypeToStringTable.end());

    return RPG_ITEM_TYPE_INVALID;
  };

  static RPG_Item_TypeToStringTable_t myRPG_Item_TypeToStringTable;

 private:
  ACE_UNIMPLEMENTED_FUNC(RPG_Item_TypeHelper());
  ACE_UNIMPLEMENTED_FUNC(RPG_Item_TypeHelper(const RPG_Item_TypeHelper&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Item_TypeHelper& operator=(const RPG_Item_TypeHelper&));
};

#endif