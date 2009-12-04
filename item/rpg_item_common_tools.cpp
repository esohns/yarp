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
#include "rpg_item_common_tools.h"

#include "rpg_item_type.h"
#include "rpg_item_money.h"
#include "rpg_item_weaponcategory.h"
#include "rpg_item_weaponclass.h"
#include "rpg_item_weapontype.h"
#include "rpg_item_physicaldamagetype.h"
#include "rpg_item_armorcategory.h"
#include "rpg_item_armortype.h"

#include <rpg_chance_dicetype.h>

#include <ace/Log_Msg.h>

#include <sstream>

// init statics
RPG_Item_TypeToStringTable_t RPG_Item_TypeHelper::myRPG_Item_TypeToStringTable;
RPG_Item_MoneyToStringTable_t RPG_Item_MoneyHelper::myRPG_Item_MoneyToStringTable;
RPG_Item_WeaponCategoryToStringTable_t RPG_Item_WeaponCategoryHelper::myRPG_Item_WeaponCategoryToStringTable;
RPG_Item_WeaponClassToStringTable_t RPG_Item_WeaponClassHelper::myRPG_Item_WeaponClassToStringTable;
RPG_Item_WeaponTypeToStringTable_t RPG_Item_WeaponTypeHelper::myRPG_Item_WeaponTypeToStringTable;
RPG_Item_PhysicalDamageTypeToStringTable_t RPG_Item_PhysicalDamageTypeHelper::myRPG_Item_PhysicalDamageTypeToStringTable;
RPG_Item_ArmorCategoryToStringTable_t RPG_Item_ArmorCategoryHelper::myRPG_Item_ArmorCategoryToStringTable;
RPG_Item_ArmorTypeToStringTable_t RPG_Item_ArmorTypeHelper::myRPG_Item_ArmorTypeToStringTable;

void RPG_Item_Common_Tools::initStringConversionTables()
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Common_Tools::initStringConversionTables"));

  RPG_Item_TypeHelper::init();
  RPG_Item_MoneyHelper::init();
  RPG_Item_WeaponCategoryHelper::init();
  RPG_Item_WeaponClassHelper::init();
  RPG_Item_WeaponTypeHelper::init();
  RPG_Item_PhysicalDamageTypeHelper::init();
  RPG_Item_ArmorCategoryHelper::init();
  RPG_Item_ArmorTypeHelper::init();
}

const std::string RPG_Item_Common_Tools::weaponDamageToString(const RPG_Item_WeaponDamage& weaponDamage_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Common_Tools::weaponDamageToString"));

  std::string result;

  // sanity check
  if (weaponDamage_in.none())
  {
    return RPG_Item_PhysicalDamageTypeHelper::RPG_Item_PhysicalDamageTypeToString(PHYSICALDAMAGE_NONE);
  } // end IF

  int damageType = PHYSICALDAMAGE_NONE;
  for (int i = 0;
       i < weaponDamage_in.size();
       i++, damageType++)
  {
    if (weaponDamage_in.test(i))
    {
      RPG_Item_PhysicalDamageType damageTypeEnum = ACE_static_cast(RPG_Item_PhysicalDamageType,
                                                                   damageType);

      result += RPG_Item_PhysicalDamageTypeHelper::RPG_Item_PhysicalDamageTypeToString(damageTypeEnum);
      result += ACE_TEXT_ALWAYS_CHAR("|");
    } // end IF
  } // end FOR

  // crop last character
  std::string::iterator position = result.end();
  position--;
  result.erase(position);

//   // debug info
//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("weapon damage: \"%s\" --> \"%s\"\n"),
//              weaponDamage_in.to_string().c_str(),
//              result.c_str()));

  return result;
}

const std::string RPG_Item_Common_Tools::damageToString(const RPG_Item_Damage& damage_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Common_Tools::damageToString"));

  std::string result;
  std::stringstream str;

  if (damage_in.typeDice != D_0)
  {
    str << damage_in.numDice;
    result += str.str();
    result += RPG_Chance_DiceTypeHelper::RPG_Chance_DiceTypeToString(damage_in.typeDice);
  } // end IF

  if (damage_in.modifier == 0)
  {
    return result;
  } // end IF
  else if ((damage_in.modifier > 0) &&
           (damage_in.typeDice != D_0))
  {
    result += ACE_TEXT_ALWAYS_CHAR("+");
  } // end IF
  str.str(ACE_TEXT_ALWAYS_CHAR(""));
  str << damage_in.modifier;
  result += str.str();

  return result;
}
