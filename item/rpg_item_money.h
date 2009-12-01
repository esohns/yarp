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

#ifndef RPG_ITEM_MONEY_H
#define RPG_ITEM_MONEY_H

enum RPG_Item_Money
{
  MONEY_COIN_COPPER = 0,
  MONEY_COIN_SILVER,
  MONEY_COIN_GOLD,
  MONEY_COIN_PLATINUM,
  MONEY_GEM,
  MONEY_PRECIOUS,
  //
  RPG_ITEM_MONEY_MAX,
  RPG_ITEM_MONEY_INVALID
};

#include <ace/Global_Macros.h>

#include <map>
#include <string>

typedef std::map<RPG_Item_Money, std::string> RPG_Item_MoneyToStringTable_t;
typedef RPG_Item_MoneyToStringTable_t::const_iterator RPG_Item_MoneyToStringTableIterator_t;

class RPG_Item_MoneyHelper
{
 public:
  inline static void init()
  {
    myRPG_Item_MoneyToStringTable.clear();
    myRPG_Item_MoneyToStringTable.insert(std::make_pair(MONEY_COIN_COPPER, ACE_TEXT_ALWAYS_CHAR("MONEY_COIN_COPPER")));
    myRPG_Item_MoneyToStringTable.insert(std::make_pair(MONEY_COIN_SILVER, ACE_TEXT_ALWAYS_CHAR("MONEY_COIN_SILVER")));
    myRPG_Item_MoneyToStringTable.insert(std::make_pair(MONEY_COIN_GOLD, ACE_TEXT_ALWAYS_CHAR("MONEY_COIN_GOLD")));
    myRPG_Item_MoneyToStringTable.insert(std::make_pair(MONEY_COIN_PLATINUM, ACE_TEXT_ALWAYS_CHAR("MONEY_COIN_PLATINUM")));
    myRPG_Item_MoneyToStringTable.insert(std::make_pair(MONEY_GEM, ACE_TEXT_ALWAYS_CHAR("MONEY_GEM")));
    myRPG_Item_MoneyToStringTable.insert(std::make_pair(MONEY_PRECIOUS, ACE_TEXT_ALWAYS_CHAR("MONEY_PRECIOUS")));
  };

  inline static std::string RPG_Item_MoneyToString(const RPG_Item_Money& element_in)
  {
    std::string result;
    RPG_Item_MoneyToStringTableIterator_t iterator = myRPG_Item_MoneyToStringTable.find(element_in);
    if (iterator != myRPG_Item_MoneyToStringTable.end())
      result = iterator->second;
    else
      result = ACE_TEXT_ALWAYS_CHAR("RPG_ITEM_MONEY_INVALID");

    return result;
  };

  inline static RPG_Item_Money stringToRPG_Item_Money(const std::string& string_in)
  {
    RPG_Item_MoneyToStringTableIterator_t iterator = myRPG_Item_MoneyToStringTable.begin();
    do
    {
      if (iterator->second == string_in)
        return iterator->first;

      iterator++;
    } while (iterator != myRPG_Item_MoneyToStringTable.end());

    return RPG_ITEM_MONEY_INVALID;
  };

 private:
  static RPG_Item_MoneyToStringTable_t myRPG_Item_MoneyToStringTable;
};

#endif
