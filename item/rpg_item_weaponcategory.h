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

#ifndef RPG_ITEM_WEAPONCATEGORY_H
#define RPG_ITEM_WEAPONCATEGORY_H

enum RPG_Item_WeaponCategory
{
  WEAPONCATEGORY_SIMPLE = 0,
  WEAPONCATEGORY_MARTIAL,
  WEAPONCATEGORY_EXOTIC,
  //
  RPG_ITEM_WEAPONCATEGORY_MAX,
  RPG_ITEM_WEAPONCATEGORY_INVALID
};

#include <map>
#include <string>

typedef std::map<RPG_Item_WeaponCategory, std::string> RPG_Item_WeaponCategoryToStringTable_t;
typedef RPG_Item_WeaponCategoryToStringTable_t::const_iterator RPG_Item_WeaponCategoryToStringTableIterator_t;

class RPG_Item_WeaponCategoryHelper
{
 public:
  inline static void init()
  {
    myRPG_Item_WeaponCategoryToStringTable.clear();
    myRPG_Item_WeaponCategoryToStringTable.insert(std::make_pair(WEAPONCATEGORY_SIMPLE, ACE_TEXT_ALWAYS_CHAR("WEAPONCATEGORY_SIMPLE")));
    myRPG_Item_WeaponCategoryToStringTable.insert(std::make_pair(WEAPONCATEGORY_MARTIAL, ACE_TEXT_ALWAYS_CHAR("WEAPONCATEGORY_MARTIAL")));
    myRPG_Item_WeaponCategoryToStringTable.insert(std::make_pair(WEAPONCATEGORY_EXOTIC, ACE_TEXT_ALWAYS_CHAR("WEAPONCATEGORY_EXOTIC")));
  };

  inline static std::string RPG_Item_WeaponCategoryToString(const RPG_Item_WeaponCategory& element_in)
  {
    std::string result;
    RPG_Item_WeaponCategoryToStringTableIterator_t iterator = myRPG_Item_WeaponCategoryToStringTable.find(element_in);
    if (iterator != myRPG_Item_WeaponCategoryToStringTable.end())
      result = iterator->second;
    else
      result = ACE_TEXT_ALWAYS_CHAR("RPG_ITEM_WEAPONCATEGORY_INVALID");

    return result;
  };

  inline static RPG_Item_WeaponCategory stringToRPG_Item_WeaponCategory(const std::string& string_in)
  {
    RPG_Item_WeaponCategoryToStringTableIterator_t iterator = myRPG_Item_WeaponCategoryToStringTable.begin();
    do
    {
      if (iterator->second == string_in)
        return iterator->first;

      iterator++;
    } while (iterator != myRPG_Item_WeaponCategoryToStringTable.end());

    return RPG_ITEM_WEAPONCATEGORY_INVALID;
  };

 private:
  static RPG_Item_WeaponCategoryToStringTable_t myRPG_Item_WeaponCategoryToStringTable;
};

#endif
