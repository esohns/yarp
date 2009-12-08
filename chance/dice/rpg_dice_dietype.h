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

#ifndef RPG_DICE_DIETYPE_H
#define RPG_DICE_DIETYPE_H

enum RPG_Dice_DieType
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
  RPG_DICE_DIETYPE_MAX,
  RPG_DICE_DIETYPE_INVALID
};

#include <ace/Assert.h>

// prefix increment
inline RPG_Dice_DieType& operator++(RPG_Dice_DieType& dieType_inout)
{
  switch (dieType_inout)
  {
    case D_0: dieType_inout = D_2; break;
    case D_2: dieType_inout = D_3; break;
    case D_3: dieType_inout = D_4; break;
    case D_4: dieType_inout = D_6; break;
    case D_6: dieType_inout = D_8; break;
    case D_8: dieType_inout = D_10; break;
    case D_10: dieType_inout = D_12; break;
    case D_12: dieType_inout = D_20; break;
    case D_20: dieType_inout = D_100; break;
    case D_100: dieType_inout = D_0; break;
    default: ACE_ASSERT(false); // changed enum but forgot to adjust the operator...
  } // end SWITCH

  return dieType_inout;
};

// postfix increment
inline RPG_Dice_DieType operator++(RPG_Dice_DieType& dieType_inout, int)
{
  RPG_Dice_DieType result = ++dieType_inout;
  return result;
};

// prefix decrement
inline RPG_Dice_DieType& operator--(RPG_Dice_DieType& dieType_inout)
{
  switch (dieType_inout)
  {
    case D_0: dieType_inout = D_100; break;
    case D_2: dieType_inout = D_0; break;
    case D_3: dieType_inout = D_2; break;
    case D_4: dieType_inout = D_3; break;
    case D_6: dieType_inout = D_4; break;
    case D_8: dieType_inout = D_6; break;
    case D_10: dieType_inout = D_8; break;
    case D_12: dieType_inout = D_10; break;
    case D_20: dieType_inout = D_12; break;
    case D_100: dieType_inout = D_20; break;
    default: ACE_ASSERT(false); // changed enum but forgot to adjust the operator...
  } // end SWITCH

  return dieType_inout;
};

// postfix decrement
inline RPG_Dice_DieType operator--(RPG_Dice_DieType& dieType_inout, int)
{
  RPG_Dice_DieType result = --dieType_inout;
  return result;
};

#include <ace/Global_Macros.h>

#include <map>
#include <string>

typedef std::map<RPG_Dice_DieType, std::string> RPG_Dice_DieTypeToStringTable_t;
typedef RPG_Dice_DieTypeToStringTable_t::const_iterator RPG_Dice_DieTypeToStringTableIterator_t;

class RPG_Dice_DieTypeHelper
{
 public:
  inline static void init()
  {
    myRPG_Dice_DieTypeToStringTable.clear();
    myRPG_Dice_DieTypeToStringTable.insert(std::make_pair(D_0, ACE_TEXT_ALWAYS_CHAR("D_0")));
    myRPG_Dice_DieTypeToStringTable.insert(std::make_pair(D_2, ACE_TEXT_ALWAYS_CHAR("D_2")));
    myRPG_Dice_DieTypeToStringTable.insert(std::make_pair(D_3, ACE_TEXT_ALWAYS_CHAR("D_3")));
    myRPG_Dice_DieTypeToStringTable.insert(std::make_pair(D_4, ACE_TEXT_ALWAYS_CHAR("D_4")));
    myRPG_Dice_DieTypeToStringTable.insert(std::make_pair(D_6, ACE_TEXT_ALWAYS_CHAR("D_6")));
    myRPG_Dice_DieTypeToStringTable.insert(std::make_pair(D_8, ACE_TEXT_ALWAYS_CHAR("D_8")));
    myRPG_Dice_DieTypeToStringTable.insert(std::make_pair(D_10, ACE_TEXT_ALWAYS_CHAR("D_10")));
    myRPG_Dice_DieTypeToStringTable.insert(std::make_pair(D_12, ACE_TEXT_ALWAYS_CHAR("D_12")));
    myRPG_Dice_DieTypeToStringTable.insert(std::make_pair(D_20, ACE_TEXT_ALWAYS_CHAR("D_20")));
    myRPG_Dice_DieTypeToStringTable.insert(std::make_pair(D_100, ACE_TEXT_ALWAYS_CHAR("D_100")));
  };

  inline static std::string RPG_Dice_DieTypeToString(const RPG_Dice_DieType& element_in)
  {
    std::string result;
    RPG_Dice_DieTypeToStringTableIterator_t iterator = myRPG_Dice_DieTypeToStringTable.find(element_in);
    if (iterator != myRPG_Dice_DieTypeToStringTable.end())
      result = iterator->second;
    else
      result = ACE_TEXT_ALWAYS_CHAR("RPG_DICE_DIETYPE_INVALID");

    return result;
  };

  inline static RPG_Dice_DieType stringToRPG_Dice_DieType(const std::string& string_in)
  {
    RPG_Dice_DieTypeToStringTableIterator_t iterator = myRPG_Dice_DieTypeToStringTable.begin();
    do
    {
      if (iterator->second == string_in)
        return iterator->first;

      iterator++;
    } while (iterator != myRPG_Dice_DieTypeToStringTable.end());

    return RPG_DICE_DIETYPE_INVALID;
  };

  static RPG_Dice_DieTypeToStringTable_t myRPG_Dice_DieTypeToStringTable;

 private:
  ACE_UNIMPLEMENTED_FUNC(RPG_Dice_DieTypeHelper());
  ACE_UNIMPLEMENTED_FUNC(RPG_Dice_DieTypeHelper(const RPG_Dice_DieTypeHelper&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Dice_DieTypeHelper& operator=(const RPG_Dice_DieTypeHelper&));
};

#endif
