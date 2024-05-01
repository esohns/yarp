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
#include "stdafx.h"

#include "rpg_player_base.h"

#include <string>

#include "ace/Log_Msg.h"

#include "rpg_common_macros.h"

#include "rpg_dice_incl.h"
#include "rpg_dice_common.h"
#include "rpg_dice.h"

#include "rpg_magic_common_tools.h"

#include "rpg_item_common.h"
#include "rpg_item_dictionary.h"

#include "rpg_character_common_tools.h"
#include "rpg_character_skills_common_tools.h"

#include "rpg_combat_incl.h"
#include "rpg_combat_common.h"

RPG_Player_Base::RPG_Player_Base (// base attributes
                                  const std::string& name_in,
                                  const RPG_Character_Alignment& alignment_in,
                                  const RPG_Character_Attributes& attributes_in,
                                  const RPG_Character_Skills_t& skills_in,
                                  const RPG_Character_Feats_t& feats_in,
                                  const RPG_Character_Abilities_t& abilities_in,
                                  ACE_UINT16 maxHitPoints_in,
                                  const RPG_Magic_SpellTypes_t& knownSpells_in,
                                  // current status
                                  const RPG_Character_Conditions_t& condition_in,
                                  ACE_INT16 hitpoints_in,
                                  ACE_UINT64 wealth_in,
                                  const RPG_Magic_Spells_t& spells_in,
                                  const RPG_Item_List_t& inventory_in)
 : myWealth (wealth_in),
   myKnownSpells (knownSpells_in),
   mySpells (spells_in),
   myInventory (inventory_in),
   myEquipment (), // start naked
   myNumHitPoints (hitpoints_in),
   myCondition (condition_in),
   myAttributes (attributes_in),
   myFeats (feats_in),
   myAbilities (abilities_in),
   mySkills (skills_in),
   myName (name_in),
   myAlignment (alignment_in),
   myNumTotalHitPoints (maxHitPoints_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Player_Base::RPG_Player_Base"));

}

RPG_Player_Base::RPG_Player_Base (const RPG_Player_Base& playerBase_in)
 : myWealth (playerBase_in.myWealth),
   myKnownSpells (playerBase_in.myKnownSpells),
   mySpells (playerBase_in.mySpells),
   myInventory (playerBase_in.myInventory),
   myEquipment (playerBase_in.myEquipment),
   myNumHitPoints (playerBase_in.myNumHitPoints),
   myCondition (playerBase_in.myCondition),
   myAttributes (playerBase_in.myAttributes),
   myFeats (playerBase_in.myFeats),
   myAbilities (playerBase_in.myAbilities),
   mySkills (playerBase_in.mySkills),
   myName (playerBase_in.myName),
   myAlignment (playerBase_in.myAlignment),
   myNumTotalHitPoints (playerBase_in.myNumTotalHitPoints)
{
  RPG_TRACE (ACE_TEXT ("RPG_Player_Base::RPG_Player_Base"));

}

// RPG_Player_Base&
// RPG_Player_Base::operator= (const RPG_Player_Base& playerBase_in)
// {
//   RPG_TRACE (ACE_TEXT ("RPG_Player_Base::operator="));
//
//   myWealth = playerBase_in.myWealth;
//   myKnownSpells = playerBase_in.myKnownSpells;
//   mySpells = playerBase_in.mySpells;
//   myInventory = playerBase_in.myInventory;
//   myEquipment = playerBase_in.myEquipment;
//   myNumHitPoints = playerBase_in.myNumHitPoints;
//   myCondition = playerBase_in.myCondition;
//   myAttributes = playerBase_in.myAttributes;
//   myFeats = playerBase_in.myFeats;
//   myAbilities = playerBase_in.myAbilities;
//   mySkills = playerBase_in.mySkills;
//
//   myName = playerBase_in.myName;
//   myAlignment = playerBase_in.myAlignment;
//   myNumTotalHitPoints = playerBase_in.myNumTotalHitPoints;
//
//   return *this;
// }

void
RPG_Player_Base::initialize (// base attributes
                             const std::string& name_in,
                             const RPG_Character_Alignment& alignment_in,
                             const RPG_Character_Attributes& attributes_in,
                             const RPG_Character_Skills_t& skills_in,
                             const RPG_Character_Feats_t& feats_in,
                             const RPG_Character_Abilities_t& abilities_in,
                             ACE_UINT16 maxHitPoints_in,
                             const RPG_Magic_SpellTypes_t& knownSpells_in,
                             // current status
                             const RPG_Character_Conditions_t& condition_in,
                             ACE_INT16 hitpoints_in,
                             ACE_UINT64 wealth_in,
                             const RPG_Magic_Spells_t& spells_in,
                             const RPG_Item_List_t& inventory_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Player_Base::initialize"));

  myWealth            = wealth_in;
  myKnownSpells       = knownSpells_in;
  mySpells            = spells_in;
  myInventory         = inventory_in;
  myEquipment.strip ();
  myNumHitPoints      = hitpoints_in;
  myCondition         = condition_in;
  myAttributes        = attributes_in;
  myFeats             = feats_in;
  myAbilities         = abilities_in;
  mySkills            = skills_in;
  myName              = name_in;
  myAlignment         = alignment_in;
  myNumTotalHitPoints = hitpoints_in;
}

ACE_UINT8
RPG_Player_Base::getAttribute (enum RPG_Common_Attribute attribute_in) const
{
  RPG_TRACE (ACE_TEXT ("RPG_Player_Base::getAttribute"));

  switch (attribute_in)
  {
    case ATTRIBUTE_STRENGTH:
      return myAttributes.strength;
    case ATTRIBUTE_DEXTERITY:
      return myAttributes.dexterity;
    case ATTRIBUTE_CONSTITUTION:
      return myAttributes.constitution;
    case ATTRIBUTE_INTELLIGENCE:
      return myAttributes.intelligence;
    case ATTRIBUTE_WISDOM:
      return myAttributes.wisdom;
    case ATTRIBUTE_CHARISMA:
      return myAttributes.charisma;
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid attribute: \"%s\", aborting\n"),
                  ACE_TEXT (RPG_Common_AttributeHelper::RPG_Common_AttributeToString (attribute_in).c_str ())));
      break;
    }
  } // end SWITCH

  return 0;
}

void
RPG_Player_Base::setAttribute (enum RPG_Common_Attribute attribute_in,
                               ACE_UINT8 value_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Player_Base::getAttribute"));

  switch (attribute_in)
  {
    case ATTRIBUTE_STRENGTH:
      myAttributes.strength = value_in;
      break;
    case ATTRIBUTE_DEXTERITY:
      myAttributes.dexterity = value_in;
      break;
    case ATTRIBUTE_CONSTITUTION:
      myAttributes.constitution = value_in;
      break;
    case ATTRIBUTE_INTELLIGENCE:
      myAttributes.intelligence = value_in;
      break;
    case ATTRIBUTE_WISDOM:
      myAttributes.wisdom = value_in;
      break;
    case ATTRIBUTE_CHARISMA:
      myAttributes.charisma = value_in;
      break;
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid attribute: \"%s\", returning\n"),
                  ACE_TEXT (RPG_Common_AttributeHelper::RPG_Common_AttributeToString (attribute_in).c_str ())));
      break;
    }
  } // end SWITCH
}

ACE_UINT8
RPG_Player_Base::getSkillRank (enum RPG_Common_Skill skill_in) const
{
  RPG_TRACE (ACE_TEXT ("RPG_Player_Base::getSkillRank"));

  RPG_Character_SkillsConstIterator_t iter = mySkills.find (skill_in);
  if (iter != mySkills.end ())
    return iter->second;

  return 0;
}

void
RPG_Player_Base::setSkillRank (enum RPG_Common_Skill skill_in,
                               ACE_UINT8 value_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Player_Base::setSkillRank"));

  RPG_Character_SkillsIterator_t iter = mySkills.find (skill_in);
  if (iter != mySkills.end ())
  {
    iter->second = value_in;
    return;
  } // end IF

  mySkills.insert (std::make_pair (skill_in, value_in));
}

bool
RPG_Player_Base::hasFeat (enum RPG_Character_Feat feat_in) const
{
  RPG_TRACE (ACE_TEXT ("RPG_Player_Base::hasFeat"));

  return (myFeats.find (feat_in) != myFeats.end ());
}

bool
RPG_Player_Base::hasAbility (enum RPG_Character_Ability ability_in) const
{
  RPG_TRACE (ACE_TEXT ("RPG_Player_Base::hasAbility"));

  return (myAbilities.find (ability_in) != myAbilities.end ());
}

// const RPG_Player_Equipment
// RPG_Player_Base::getEquipment () const
// {
//   RPG_TRACE (ACE_TEXT ("RPG_Player_Base::getEquipment"));
//
//   return myEquipment;
// }

bool
RPG_Player_Base::hasCondition (enum RPG_Common_Condition condition_in) const
{
  RPG_TRACE (ACE_TEXT ("RPG_Player_Base::hasCondition"));

  return (myCondition.find (condition_in) != myCondition.end ());
}

ACE_UINT32
RPG_Player_Base::sustainDamage (const RPG_Combat_Damage& damage_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Player_Base::sustainDamage"));

  ACE_UINT32 result = 0;

  ACE_INT16 num_hit_points_initial_i = myNumHitPoints;
  ACE_INT16 damage_value;
  RPG_Dice_RollResult_t result_2;
  for (RPG_Combat_DamageElementsConstIterator_t iterator = damage_in.elements.begin ();
       iterator != damage_in.elements.end ();
       iterator++)
  {
    // compute damage
    result_2.clear ();
    RPG_Dice::simulateRoll ((*iterator).amount,
                            1,
                            result_2);
    damage_value = result_2.front ();

    // *TODO*: consider defenses, resistances, (partial) immunities...

    // minimum damage is 1 HP
    if (damage_value <= 0)
      damage_value = 1;

    myNumHitPoints -= damage_value;

    result += damage_value;
  } // end FOR

  if (myNumHitPoints < -10)
    myNumHitPoints = -10;

  // adjust condition
  if (myNumHitPoints <= 0)
  {
    myCondition.erase (CONDITION_NORMAL);

    if (myNumHitPoints == -10)
      myCondition.insert (CONDITION_DEAD);
    else if (myNumHitPoints < 0)
      myCondition.insert (CONDITION_DYING);
    else
      myCondition.insert (CONDITION_DISABLED);

    if (myNumHitPoints < 0)
      myCondition.insert (CONDITION_UNCONSCIOUS);
  } // end IF

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("\"%s\" (HP: %d/%u) suffers damage of %u HP%s...\n"),
              ACE_TEXT (getName ().c_str ()),
              num_hit_points_initial_i,
              myNumTotalHitPoints,
              result,
              (!hasCondition (CONDITION_NORMAL) ? ACE_TEXT (" --> DOWN") : ACE_TEXT (""))));

  return result;
}

void
RPG_Player_Base::status () const
{
  RPG_TRACE (ACE_TEXT ("RPG_Player_Base::status"));

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("condition: %s\nHP: %d/%u\nwealth: %Q GP\n"),
              ACE_TEXT (RPG_Character_Common_Tools::toString (myCondition).c_str ()),
              myNumHitPoints,
              myNumTotalHitPoints,
              myWealth));
}

void
RPG_Player_Base::dump () const
{
  RPG_TRACE (ACE_TEXT ("RPG_Player_Base::dump"));

  std::string spells;
  if (!myKnownSpells.empty ())
  {
    spells += ACE_TEXT_ALWAYS_CHAR ("known:\n");
    spells += RPG_Magic_Common_Tools::toString (myKnownSpells);
  } // end IF
  if (!mySpells.empty ())
  {
    spells += ACE_TEXT_ALWAYS_CHAR ("memorized:\n");
    spells += RPG_Magic_Common_Tools::toString (mySpells);
  } // end IF

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("Name: \"%s\"\nAlignment: \"%s\"\nAttributes:\n===========\n%sSkills:\n=======\n%sFeats:\n======\n%sAbilities:\n==========\n%sSpells:\n==========\n%sItems:\n======\n"),
              ACE_TEXT (myName.c_str ()),
              ACE_TEXT (RPG_Character_Common_Tools::toString (myAlignment).c_str ()),
              ACE_TEXT (RPG_Character_Common_Tools::toString (myAttributes).c_str ()),
              ACE_TEXT (RPG_Character_Skills_Common_Tools::toString (mySkills).c_str ()),
              ACE_TEXT (RPG_Character_Skills_Common_Tools::toString (myFeats).c_str ()),
              ACE_TEXT (RPG_Character_Skills_Common_Tools::toString (myAbilities).c_str ()),
              ACE_TEXT (spells.c_str ())));

  // dump items
  myInventory.dump ();
}
