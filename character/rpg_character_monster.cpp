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
#include "rpg_character_monster.h"

#include <rpg_monster_common.h>
#include <rpg_monster_dictionary.h>

#include <rpg_item_common.h>
#include <rpg_item_dictionary.h>

#include <ace/Log_Msg.h>

RPG_Character_Monster::RPG_Character_Monster(const std::string& name_in,
                                             const RPG_Monster_Type& type_in,
                                             const RPG_Character_Alignment& alignment_in,
                                             const RPG_Character_Attributes& attributes_in,
                                             const RPG_Character_Skills_t& skills_in,
                                             const RPG_Character_Feats_t& feats_in,
                                             const RPG_Character_Abilities_t& abilities_in,
                                             const RPG_Character_Size& defaultSize_in,
                                             const unsigned short int& hitpoints_in,
                                             const unsigned int& wealth_in,
                                             const RPG_Item_List_t& inventory_in)
 : myType(type_in),
   inherited(name_in,
             alignment_in,
             attributes_in,
             skills_in,
             feats_in,
             abilities_in,
             defaultSize_in,
             hitpoints_in,
             wealth_in,
             inventory_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Monster::RPG_Character_Monster"));

}

RPG_Character_Monster::RPG_Character_Monster(const RPG_Character_Monster& monster_in)
 : myType(monster_in.myType),
   inherited(monster_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Monster::RPG_Character_Monster"));

}

RPG_Character_Monster::~RPG_Character_Monster()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Monster::~RPG_Character_Monster"));

}

RPG_Character_Monster& RPG_Character_Monster::operator=(const RPG_Character_Monster& monster_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Monster::operator="));

  myType = monster_in.myType;
  inherited::operator=(monster_in);

  return *this;
}

const RPG_Monster_Type RPG_Character_Monster::getType() const
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Monster::getType"));

  return myType;
}

const signed char RPG_Character_Monster::getArmorClass(const RPG_Combat_DefenseSituation& defenseSituation_in) const
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Monster::getArmorClass"));

  // retrieve natural armor
  RPG_Monster_Properties properties = RPG_MONSTER_DICTIONARY_SINGLETON::instance()->getMonsterProperties(getName());

  // *TODO*: consider any equipped armor
  switch (defenseSituation_in)
  {
    default:
    {
      // debug info
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid defense situation (\"%s\"): returning value for \"%s\"...\n"),
                 RPG_Combat_DefenseSituationHelper::RPG_Combat_DefenseSituationToString(defenseSituation_in).c_str(),
                 RPG_Combat_DefenseSituationHelper::RPG_Combat_DefenseSituationToString(DEFENSE_NORMAL).c_str()));
    }
    case DEFENSE_NORMAL:
    {
      return properties.armorClass.normal;
    }
    case DEFENSE_TOUCH:
    {
      return properties.armorClass.touch;
    }
    case DEFENSE_FLATFOOTED:
    {
      return properties.armorClass.flatFooted;
    }
  } // end SWITCH
}

const signed char RPG_Character_Monster::getShieldBonus() const
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Monster::getShieldBonus"));

  // retrieve equipped armor type
  RPG_Item_ArmorType type = myEquipment.getShield();
  RPG_Item_ArmorProperties properties = RPG_ITEM_DICTIONARY_SINGLETON::instance()->getArmorProperties(type);

  return properties.baseArmorBonus;
}

void RPG_Character_Monster::gainExperience(const unsigned int& XP_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Monster::gainExperience"));

  // *TODO*
  ACE_ASSERT(false);
}

const bool RPG_Character_Monster::isPlayerCharacter() const
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Monster::isPlayerCharacter"));

  return false;
}

void RPG_Character_Monster::dump() const
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Monster::dump"));

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("Monster (type: \"%s\")\n"),
//              RPG_Monster_Common_Tools::typeToString(myType).c_str()));

  inherited::dump();
}
