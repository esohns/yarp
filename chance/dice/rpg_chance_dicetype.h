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

#ifndef RPG_CHANCE_DICETYPE_H
#define RPG_CHANCE_DICETYPE_H

enum RPG_Chance_DiceType
{
  D_0   = 0,
  D_2   = 2,
  D_3   = 3,
  D_4   = 4,
  D_6   = 6,
  D_8   = 8,
  D_10  = 10,
  D_12  = 12,
  D_20  = 20,
  D_100 = 100,
  //
  RPG_CHANCE_DICETYPE_MAX,
  RPG_CHANCE_DICETYPE_INVALID
};

#include <ace/Assert.h>

// prefix increment
inline RPG_Chance_DiceType& operator++(RPG_Chance_DiceType& diceType_in)
{
  switch (diceType_in)
  {
    case D_0: diceType_in = D_2; break;
    case D_2: diceType_in = D_3; break;
    case D_3: diceType_in = D_4; break;
    case D_4: diceType_in = D_6; break;
    case D_6: diceType_in = D_8; break;
    case D_8: diceType_in = D_10; break;
    case D_10: diceType_in = D_12; break;
    case D_12: diceType_in = D_20; break;
    case D_20: diceType_in = D_100; break;
    case D_100: diceType_in = D_0; break;
    default: ACE_ASSERT(false); // changed enum but forgot to adjust the operator...
  } // end SWITCH

  return diceType_in;
};

// postfix increment
inline RPG_Chance_DiceType operator++(RPG_Chance_DiceType& diceType_in, int)
{
  RPG_Chance_DiceType result = ++diceType_in;
  return result;
};

// prefix decrement
inline RPG_Chance_DiceType& operator--(RPG_Chance_DiceType& diceType_in)
{
  switch (diceType_in)
  {
    case D_0: diceType_in = D_100; break;
    case D_2: diceType_in = D_0; break;
    case D_3: diceType_in = D_2; break;
    case D_4: diceType_in = D_3; break;
    case D_6: diceType_in = D_4; break;
    case D_8: diceType_in = D_6; break;
    case D_10: diceType_in = D_8; break;
    case D_12: diceType_in = D_10; break;
    case D_20: diceType_in = D_12; break;
    case D_100: diceType_in = D_20; break;
    default: ACE_ASSERT(false); // changed enum but forgot to adjust the operator...
  } // end SWITCH

  return diceType_in;
};

// postfix decrement
inline RPG_Chance_DiceType operator--(RPG_Chance_DiceType& diceType_in, int)
{
  RPG_Chance_DiceType result = --diceType_in;
  return result;
};

#include <map>
#include <string>

typedef std::map<RPG_Chance_DiceType, std::string> RPG_Chance_DiceTypeToStringTable_t;
typedef RPG_Chance_DiceTypeToStringTable_t::const_iterator RPG_Chance_DiceTypeToStringTableIterator_t;

class RPG_Chance_DiceTypeHelper
{
 public:
  inline static void init()
  {
    myRPG_Chance_DiceTypeToStringTable.clear();
    myRPG_Chance_DiceTypeToStringTable.insert(std::make_pair(D_0, ACE_TEXT_ALWAYS_CHAR("D_0")));
    myRPG_Chance_DiceTypeToStringTable.insert(std::make_pair(D_2, ACE_TEXT_ALWAYS_CHAR("D_2")));
    myRPG_Chance_DiceTypeToStringTable.insert(std::make_pair(D_3, ACE_TEXT_ALWAYS_CHAR("D_3")));
    myRPG_Chance_DiceTypeToStringTable.insert(std::make_pair(D_4, ACE_TEXT_ALWAYS_CHAR("D_4")));
    myRPG_Chance_DiceTypeToStringTable.insert(std::make_pair(D_6, ACE_TEXT_ALWAYS_CHAR("D_6")));
    myRPG_Chance_DiceTypeToStringTable.insert(std::make_pair(D_8, ACE_TEXT_ALWAYS_CHAR("D_8")));
    myRPG_Chance_DiceTypeToStringTable.insert(std::make_pair(D_10, ACE_TEXT_ALWAYS_CHAR("D_10")));
    myRPG_Chance_DiceTypeToStringTable.insert(std::make_pair(D_12, ACE_TEXT_ALWAYS_CHAR("D_12")));
    myRPG_Chance_DiceTypeToStringTable.insert(std::make_pair(D_20, ACE_TEXT_ALWAYS_CHAR("D_20")));
    myRPG_Chance_DiceTypeToStringTable.insert(std::make_pair(D_100, ACE_TEXT_ALWAYS_CHAR("D_100")));
  };

  inline static std::string RPG_Chance_DiceTypeToString(const RPG_Chance_DiceType& element_in)
  {
    std::string result;
    RPG_Chance_DiceTypeToStringTableIterator_t iterator = myRPG_Chance_DiceTypeToStringTable.find(element_in);
    if (iterator != myRPG_Chance_DiceTypeToStringTable.end())
      result = iterator->second;
    else
      result = ACE_TEXT_ALWAYS_CHAR("RPG_CHANCE_DICETYPE_INVALID");

    return result;
  };

  inline static RPG_Chance_DiceType stringToRPG_Chance_DiceType(const std::string& string_in)
  {
    RPG_Chance_DiceTypeToStringTableIterator_t iterator = myRPG_Chance_DiceTypeToStringTable.begin();
    do
    {
      if (iterator->second == string_in)
        return iterator->first;

      iterator++;
    } while (iterator != myRPG_Chance_DiceTypeToStringTable.end());

    return RPG_CHANCE_DICETYPE_INVALID;
  };

 private:
  static RPG_Chance_DiceTypeToStringTable_t myRPG_Chance_DiceTypeToStringTable;
};

#endif
