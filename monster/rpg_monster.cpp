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
#include "rpg_monster.h"

#include "rpg_monster_common.h"
#include "rpg_monster_dictionary.h"

#include <rpg_item_common.h>
#include <rpg_item_dictionary.h>

#include <rpg_common_macros.h>

#include <ace/Log_Msg.h>

RPG_Monster::RPG_Monster(// base attributes
                         const std::string& name_in,
                         const RPG_Common_CreatureType& type_in,
                         const RPG_Character_Alignment& alignment_in,
                         const RPG_Character_Attributes& attributes_in,
                         const RPG_Character_Skills_t& skills_in,
                         const RPG_Character_Feats_t& feats_in,
                         const RPG_Character_Abilities_t& abilities_in,
                         const RPG_Common_Size& defaultSize_in,
                         const unsigned short int& maxHitPoints_in,
                         const RPG_Magic_SpellTypes_t& knownSpells_in,
                         // current status
                         const RPG_Character_Conditions_t& condition_in,
                         const unsigned short int& hitpoints_in,
                         const unsigned int& wealth_in,
                         const RPG_Magic_Spells_t& spells_in,
                         const RPG_Item_List_t& inventory_in,
                         const bool& isSummoned_in)
 : inherited(// base attributes
             name_in,
             alignment_in,
             attributes_in,
             skills_in,
             feats_in,
             abilities_in,
             defaultSize_in,
             maxHitPoints_in,
             knownSpells_in,
             // current status
             condition_in,
             hitpoints_in,
             wealth_in,
             spells_in,
             inventory_in),
   myType(type_in),
   myIsSummoned(isSummoned_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Monster::RPG_Monster"));

}

RPG_Monster::RPG_Monster(const RPG_Monster& monster_in)
 : inherited(monster_in),
   myType(monster_in.myType),
   myIsSummoned(monster_in.myIsSummoned)
{
  RPG_TRACE(ACE_TEXT("RPG_Monster::RPG_Monster"));

}

RPG_Monster::~RPG_Monster()
{
  RPG_TRACE(ACE_TEXT("RPG_Monster::~RPG_Monster"));

}

// RPG_Monster&
// RPG_Monster::operator=(const RPG_Monster& monster_in)
// {
//   RPG_TRACE(ACE_TEXT("RPG_Monster::operator="));
//
//   myType = monster_in.myType;
//   myIsSummoned = monster_in.myIsSummoned;
//   inherited::operator=(monster_in);
//
//   return *this;
// }

const RPG_Common_CreatureType
RPG_Monster::getType() const
{
  RPG_TRACE(ACE_TEXT("RPG_Monster::getType"));

  return myType;
}

const bool
RPG_Monster::isSummoned() const
{
  RPG_TRACE(ACE_TEXT("RPG_Monster::isSummoned"));

  return myIsSummoned;
}

const signed char
RPG_Monster::getArmorClass(const RPG_Combat_DefenseSituation& defenseSituation_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Monster::getArmorClass"));

  // AC = 10 + (natural) armor bonus (+ shield bonus) + DEX modifier + size modifier [+ other modifiers]
  signed char result = 0;

  // *TODO*: consider any (additional, equipped) armor...
  RPG_Monster_Properties properties = RPG_MONSTER_DICTIONARY_SINGLETON::instance()->getProperties(getName());
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
      result = properties.armorClass.normal;

      break;
    }
    case DEFENSE_TOUCH:
    {
      result = properties.armorClass.touch;

      break;
    }
    case DEFENSE_FLATFOOTED:
    {
      result = properties.armorClass.flatFooted;

      break;
    }
  } // end SWITCH
//   result += getShieldBonus();

  return result;
}

void
RPG_Monster::gainExperience(const unsigned int& XP_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Monster::gainExperience"));

  // *TODO*
  ACE_ASSERT(false);
}

const bool
RPG_Monster::isPlayerCharacter() const
{
  RPG_TRACE(ACE_TEXT("RPG_Monster::isPlayerCharacter"));

  return false;
}

void
RPG_Monster::dump() const
{
  RPG_TRACE(ACE_TEXT("RPG_Monster::dump"));

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("Monster (type: \"%s\")\n"),
//              RPG_Monster_Common_Tools::typeToString(myType).c_str()));

  inherited::dump();
}

const RPG_Character_BaseAttackBonus_t
RPG_Monster::getAttackBonus(const RPG_Common_Attribute& modifier_in,
                            const RPG_Combat_AttackSituation& attackSituation_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Monster::getAttackBonus"));

  ACE_NOTREACHED();
  ACE_ASSERT(false);

  ACE_ASSERT((modifier_in == ATTRIBUTE_DEXTERITY) ||
             (modifier_in == ATTRIBUTE_STRENGTH));

  RPG_Character_BaseAttackBonus_t result;

  return result;
}

const signed char
RPG_Monster::getShieldBonus() const
{
  RPG_TRACE(ACE_TEXT("RPG_Monster::getShieldBonus"));

  ACE_NOTREACHED();
  ACE_ASSERT(false);

  signed char result = 0;

  return result;
}
