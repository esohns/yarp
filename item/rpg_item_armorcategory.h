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

#ifndef RPG_ITEM_ARMORCATEGORY_H
#define RPG_ITEM_ARMORCATEGORY_H

enum RPG_Item_ArmorCategory
{
  ARMORCATEGORY_LIGHT = 0,
  ARMORCATEGORY_MEDIUM,
  ARMORCATEGORY_HEAVY,
  ARMORCATEGORY_SHIELD,
  //
  RPG_ITEM_ARMORCATEGORY_MAX,
  RPG_ITEM_ARMORCATEGORY_INVALID
};

#include <ace/Global_Macros.h>

#include <map>
#include <string>

typedef std::map<RPG_Item_ArmorCategory, std::string> RPG_Item_ArmorCategoryToStringTable_t;
typedef RPG_Item_ArmorCategoryToStringTable_t::const_iterator RPG_Item_ArmorCategoryToStringTableIterator_t;

class RPG_Item_ArmorCategoryHelper
{
 public:
  inline static void init()
  {
    myRPG_Item_ArmorCategoryToStringTable.clear();
    myRPG_Item_ArmorCategoryToStringTable.insert(std::make_pair(ARMORCATEGORY_LIGHT, ACE_TEXT_ALWAYS_CHAR("ARMORCATEGORY_LIGHT")));
    myRPG_Item_ArmorCategoryToStringTable.insert(std::make_pair(ARMORCATEGORY_MEDIUM, ACE_TEXT_ALWAYS_CHAR("ARMORCATEGORY_MEDIUM")));
    myRPG_Item_ArmorCategoryToStringTable.insert(std::make_pair(ARMORCATEGORY_HEAVY, ACE_TEXT_ALWAYS_CHAR("ARMORCATEGORY_HEAVY")));
    myRPG_Item_ArmorCategoryToStringTable.insert(std::make_pair(ARMORCATEGORY_SHIELD, ACE_TEXT_ALWAYS_CHAR("ARMORCATEGORY_SHIELD")));
  };

  inline static std::string RPG_Item_ArmorCategoryToString(const RPG_Item_ArmorCategory& element_in)
  {
    std::string result;
    RPG_Item_ArmorCategoryToStringTableIterator_t iterator = myRPG_Item_ArmorCategoryToStringTable.find(element_in);
    if (iterator != myRPG_Item_ArmorCategoryToStringTable.end())
      result = iterator->second;
    else
      result = ACE_TEXT_ALWAYS_CHAR("RPG_ITEM_ARMORCATEGORY_INVALID");

    return result;
  };

  inline static RPG_Item_ArmorCategory stringToRPG_Item_ArmorCategory(const std::string& string_in)
  {
    RPG_Item_ArmorCategoryToStringTableIterator_t iterator = myRPG_Item_ArmorCategoryToStringTable.begin();
    do
    {
      if (iterator->second == string_in)
        return iterator->first;

      iterator++;
    } while (iterator != myRPG_Item_ArmorCategoryToStringTable.end());

    return RPG_ITEM_ARMORCATEGORY_INVALID;
  };

  static RPG_Item_ArmorCategoryToStringTable_t myRPG_Item_ArmorCategoryToStringTable;

 private:
  ACE_UNIMPLEMENTED_FUNC(RPG_Item_ArmorCategoryHelper());
  ACE_UNIMPLEMENTED_FUNC(RPG_Item_ArmorCategoryHelper(const RPG_Item_ArmorCategoryHelper&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Item_ArmorCategoryHelper& operator=(const RPG_Item_ArmorCategoryHelper&));
};

#endif
