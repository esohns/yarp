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

#include "rpg_player_player_base.h"

#include <rpg_combat_common_tools.h>

#include <rpg_item_common.h>
#include <rpg_item_dictionary.h>

#include <rpg_character_common_tools.h>
#include <rpg_character_race_common_tools.h>
#include <rpg_character_skills_common_tools.h>

#include <rpg_magic_common_tools.h>

#include <rpg_common_macros.h>
#include <rpg_common_tools.h>

#include <ace/OS.h>
#include <ace/Log_Msg.h>

#include <algorithm>

RPG_Player_Player_Base::RPG_Player_Player_Base(// base attributes
											                         const std::string& name_in,
											                         const RPG_Character_Gender& gender_in,
											                         const RPG_Character_Race_t& race_in,
											                         const RPG_Character_Class& class_in,
											                         const RPG_Character_Alignment& alignment_in,
											                         const RPG_Character_Attributes& attributes_in,
											                         const RPG_Character_Skills_t& skills_in,
											                         const RPG_Character_Feats_t& feats_in,
											                         const RPG_Character_Abilities_t& abilities_in,
											                         const RPG_Character_OffHand& offHand_in,
											                         const unsigned short int& maxHitPoints_in,
											                         const RPG_Magic_SpellTypes_t& knownSpells_in,
											                         // current status
											                         const RPG_Character_Conditions_t& condition_in,
											                         const short int& hitpoints_in,
											                         const unsigned int& experience_in,
											                         const unsigned int& wealth_in,
											                         const RPG_Magic_Spells_t& spells_in,
											                         const RPG_Item_List_t& inventory_in)
 : inherited(name_in,
             alignment_in,
             attributes_in,
             skills_in,
             feats_in,
             abilities_in,
             RPG_Character_Race_Common_Tools::race2Size(race_in),
             maxHitPoints_in,
             knownSpells_in,
             condition_in,
             hitpoints_in,
             wealth_in,
             spells_in,
             inventory_in),
   myGender(gender_in),
   myRace(race_in),
   myClass(class_in),
   myOffHand(offHand_in),
   myExperience(experience_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Player_Player_Base::RPG_Player_Player_Base"));

}

RPG_Player_Player_Base::RPG_Player_Player_Base(const RPG_Player_Player_Base& playerBase_in)
 : inherited(playerBase_in),
   myGender(playerBase_in.myGender),
   myRace(playerBase_in.myRace),
   myClass(playerBase_in.myClass),
   myOffHand(playerBase_in.myOffHand),
   myExperience(playerBase_in.myExperience)
{
  RPG_TRACE(ACE_TEXT("RPG_Player_Player_Base::RPG_Player_Player_Base"));

}

RPG_Player_Player_Base::~RPG_Player_Player_Base()
{
  RPG_TRACE(ACE_TEXT("RPG_Player_Player_Base::~RPG_Player_Player_Base"));

}

// RPG_Player_Player_Base&
// RPG_Player_Player_Base::operator=(const RPG_Player_Player_Base& playerBase_in)
// {
//   RPG_TRACE(ACE_TEXT("RPG_Player_Player_Base::operator="));
//
//   myGender = playerBase_in.myGender;
//   myRace = playerBase_in.myRace;
//   myClass = playerBase_in.myClass;
//   myOffHand = playerBase_in.myOffHand;
//
//   myExperience = playerBase_in.myExperience;
//
//   return *this;
// }

void
RPG_Player_Player_Base::init(// base attributes
							               const std::string& name_in,
							               const RPG_Character_Gender& gender_in,
							               const RPG_Character_Race_t& race_in,
							               const RPG_Character_Class& class_in,
							               const RPG_Character_Alignment& alignment_in,
							               const RPG_Character_Attributes& attributes_in,
							               const RPG_Character_Skills_t& skills_in,
							               const RPG_Character_Feats_t& feats_in,
							               const RPG_Character_Abilities_t& abilities_in,
							               const RPG_Character_OffHand& offHand_in,
							               const unsigned short int& maxHitPoints_in,
							               const RPG_Magic_SpellTypes_t& knownSpells_in,
							               // current status
							               const RPG_Character_Conditions_t& condition_in,
							               const short int& hitpoints_in,
							               const unsigned int& experience_in,
							               const unsigned int& wealth_in,
							               const RPG_Magic_Spells_t& spells_in,
							               const RPG_Item_List_t& inventory_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Player_Player_Base::init"));

  // init base class
  inherited::init(// base attributes
                  name_in,
                  alignment_in,
                  attributes_in,
                  skills_in,
                  feats_in,
                  abilities_in,
                  RPG_Character_Race_Common_Tools::race2Size(race_in),
                  maxHitPoints_in,
                  knownSpells_in,
                  // current status
                  condition_in,
                  hitpoints_in,
                  wealth_in,
                  spells_in,
                  inventory_in);

  myGender     = gender_in;
  myRace       = race_in;
  myClass      = class_in;
  myOffHand    = offHand_in;
  myExperience = experience_in;
}

RPG_Character_Gender
RPG_Player_Player_Base::getGender() const
{
  RPG_TRACE(ACE_TEXT("RPG_Player_Player_Base::getGender"));

  return myGender;
}

const RPG_Character_Race_t&
RPG_Player_Player_Base::getRace() const
{
  RPG_TRACE(ACE_TEXT("RPG_Player_Player_Base::getRace"));

  return myRace;
}

const RPG_Character_Class&
RPG_Player_Player_Base::getClass() const
{
  RPG_TRACE(ACE_TEXT("RPG_Player_Player_Base::getClass"));

  return myClass;
}

RPG_Character_OffHand
RPG_Player_Player_Base::getOffHand() const
{
  RPG_TRACE(ACE_TEXT("RPG_Player_Player_Base::getOffHand"));

  return myOffHand;
}

unsigned char
RPG_Player_Player_Base::getLevel(const RPG_Common_SubClass& subClass_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Player_Player_Base::getLevel"));

  // *TODO*: consider implementing class-specific tables...
  ACE_UNUSED_ARG(subClass_in);

  unsigned char result = 0;

  result = static_cast<unsigned char>(ACE_OS::floor((1.0 + ::sqrt(static_cast<double>(myExperience / 125) + 1.0)) / 2.0));

  return result;
}

unsigned int
RPG_Player_Player_Base::getExperience() const
{
  RPG_TRACE(ACE_TEXT("RPG_Player_Player_Base::getExperience"));

  return myExperience;
}

const RPG_Player_Equipment&
RPG_Player_Player_Base::getEquipment() const
{
  RPG_TRACE(ACE_TEXT("RPG_Player_Player_Base::getEquipment"));

  return inherited::myEquipment;
}

RPG_Character_BaseAttackBonus_t
RPG_Player_Player_Base::getAttackBonus(const RPG_Common_Attribute& modifier_in,
                                       const RPG_Combat_AttackSituation& attackSituation_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Player_Player_Base::getAttackBonus"));

  ACE_ASSERT((modifier_in == ATTRIBUTE_DEXTERITY) ||
             (modifier_in == ATTRIBUTE_STRENGTH));

  // Attack Bonus = base attack bonus + STR/DEX modifier + size modifier [+ range penalty + other modifiers]
  RPG_Character_BaseAttackBonus_t result;

  // attack bonusses stack for multiclass characters...
  for (RPG_Character_SubClassesIterator_t iterator = myClass.subClasses.begin();
       iterator != myClass.subClasses.end();
       iterator++)
  {
    RPG_Character_BaseAttackBonus_t bonus = RPG_Character_Common_Tools::getBaseAttackBonus(*iterator,
                                                                                           getLevel(*iterator));
    // append necessary entries
    for (int diff = (bonus.size() - result.size());
         diff > 0;
         diff--)
      result.push_back(0);
    int index = 0;
    for (RPG_Character_BaseAttackBonusConstIterator_t iterator2 = bonus.begin();
         iterator2 != bonus.end();
         iterator2++, index++)
      result[index] += *iterator2;
  } // end FOR

  int abilityModifier = RPG_Character_Common_Tools::getAttributeAbilityModifier(getAttribute(modifier_in));
  int sizeModifier = RPG_Common_Tools::getSizeModifier(getSize());
  for (RPG_Character_BaseAttackBonusIterator_t iterator = result.begin();
       iterator != result.end();
       iterator++)
  {
    (*iterator) += abilityModifier;
    (*iterator) += sizeModifier;
  } // end FOR

  return result;
}

signed char
RPG_Player_Player_Base::getArmorClass(const RPG_Combat_DefenseSituation& defenseSituation_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Player_Player_Base::getArmorClass"));

  // AC = 10 + armor bonus + shield bonus + DEX modifier + size modifier [+ other modifiers]
  signed char result = 10;

  // retrieve equipped armor type
  RPG_Item_ArmorType type = myEquipment.getBodyArmor();
  RPG_Item_ArmorProperties properties;
  if (type != ARMOR_NONE)
  {
    properties = RPG_ITEM_DICTIONARY_SINGLETON::instance()->getArmorProperties(type);
    result += properties.baseBonus;
  } // end IF
  result += getShieldBonus();

  // consider defense situation
  int DEX_modifier = 0;
  if (defenseSituation_in != DEFENSE_FLATFOOTED)
  {
    DEX_modifier = RPG_Character_Common_Tools::getAttributeAbilityModifier(getAttribute(ATTRIBUTE_DEXTERITY));
    if (type != ARMOR_NONE)
      DEX_modifier = std::min<int>(static_cast<int>(properties.maxDexterityBonus), DEX_modifier);
  } // end IF
  result += DEX_modifier;

  // usually, this is irrelevant (SIZE_MEDIUM --> +/-0), but may have changed temporarily, magically etc...
  result += RPG_Common_Tools::getSizeModifier(getSize());

  return result;
}

unsigned char
RPG_Player_Player_Base::getSpeed() const
{
  RPG_TRACE(ACE_TEXT("RPG_Player_Player_Base::getSpeed"));

  unsigned char result = 0;

  // step1: retrieve base speed (race)
  unsigned char base_speed = 0;
  unsigned int race_index = 1;
  for (unsigned int index = 0;
       index < myRace.size();
       index++, race_index++)
    if (myRace.test(index))
    {
      base_speed = RPG_Character_Race_Common_Tools::race2Speed(static_cast<RPG_Character_Race>(race_index));
      if (base_speed > result)
        result = base_speed;
    } // end IF

  // step2: test encumbrance (race)
  const RPG_Item_ArmorType& armor_type = getEquipment().getBodyArmor();
  // *NOTE*: dwarves move at the base speed with any armor...
  if ((armor_type != ARMOR_NONE) &&
      RPG_Character_Race_Common_Tools::hasRace(myRace, RACE_DWARF))
  {
    const RPG_Item_ArmorProperties& properties = RPG_ITEM_DICTIONARY_SINGLETON::instance()->getArmorProperties(armor_type);
    switch (properties.category)
    {
      case ARMORCATEGORY_LIGHT:
        break;
      case ARMORCATEGORY_MEDIUM:
      case ARMORCATEGORY_HEAVY:
      {
        if (RPG_Character_Race_Common_Tools::hasRace(myRace, RACE_GNOME) ||
            RPG_Character_Race_Common_Tools::hasRace(myRace, RACE_HALFLING))
          result = 15;

        result = 20;
      }
      default:
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("invalid armor category (was \"%s\"), aborting\n"),
                   RPG_Item_ArmorCategoryHelper::RPG_Item_ArmorCategoryToString(properties.category).c_str()));

        return 0;
      }
    } // end SWITCH
  } // end IF

  // *TODO*: test load (inventory) and other (spell, ...) effects
  return result;
}

bool
RPG_Player_Player_Base::isPlayerCharacter() const
{
  RPG_TRACE(ACE_TEXT("RPG_Player_Player_Base::isPlayerCharacter"));

  return true;
}

void
RPG_Player_Player_Base::gainExperience(const unsigned int& XP_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Player_Player_Base::gainExperience"));

  unsigned char old_level = getLevel(*myClass.subClasses.begin());

  myExperience += XP_in;

  // *TODO*: trigger level-up
  if (old_level != getLevel(*myClass.subClasses.begin()))
  {
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("player: \"%s\" (XP: %d) has gained a level (%d)...\n"),
               getName().c_str(),
               myExperience,
               static_cast<int>(getLevel(*myClass.subClasses.begin()))));
  } // end IF
}

unsigned int
RPG_Player_Player_Base::rest(const RPG_Common_Camp& type_in,
                             const unsigned int& hours_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Player_Player_Base::rest"));

  // *TODO*: consider dead/dying players !
  if (myNumHitPoints < 0)
  {
    // cannot rest --> will die
    return 0;
  } // end IF

  // consider natural healing...
  unsigned int restedPeriod = 0;
  int missingHPs = getNumTotalHitPoints() - myNumHitPoints;
  unsigned int recoveryRate = getLevel();
  if (type_in == REST_FULL) recoveryRate *= 2;
  if ((missingHPs > 0) && (hours_in >= 24))
  {
    // OK: we've (naturally) recovered some HPs...
    while ((missingHPs > 0) && (restedPeriod < hours_in))
    {
      // just a fraction left...
      if ((hours_in - restedPeriod) < 24)
        break;

      missingHPs -= recoveryRate;
      restedPeriod += 24;
    } // end WHILE

    if (missingHPs < 0)
      missingHPs = 0;
    myNumHitPoints = getNumTotalHitPoints() - missingHPs;
  } // end IF

  // adjust condition
  if (myNumHitPoints > 0)
  {
    myCondition.insert(CONDITION_NORMAL);
    myCondition.erase(CONDITION_DISABLED);
  } // end IF

  return (restedPeriod * 24);
}

void
RPG_Player_Player_Base::status() const
{
  RPG_TRACE(ACE_TEXT("RPG_Player_Player_Base::status"));

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("name: \"%s\" (XP: %d (%u))\n"),
             getName().c_str(),
             myExperience,
             static_cast<unsigned int>(getLevel())));

  inherited::status();
}

void
RPG_Player_Player_Base::dump() const
{
  RPG_TRACE(ACE_TEXT("RPG_Player_Player_Base::dump"));

  // *TODO*: add items
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("Player \"%s\": \nGender: %s\nRace: %s\nClass: %s\nAlignment: %s\nCondition: %s\nHP: %d/%u\nXP: %u\nGold: %u\nAttributes:\n===========\n%sFeats:\n======\n%sAbilities:\n==========\n%sSkills:\n=======\n%sSpells (known):\n=======\n%sSpells (prepared):\n=======\n%sItems:\n======\n"),
             getName().c_str(),
             RPG_Character_GenderHelper::RPG_Character_GenderToString(myGender).c_str(),
             RPG_Character_Common_Tools::raceToString(myRace).c_str(),
             RPG_Character_Common_Tools::classToString(myClass).c_str(),
             RPG_Character_Common_Tools::alignmentToString(getAlignment()).c_str(),
             RPG_Character_Common_Tools::conditionToString(myCondition).c_str(),
             myNumHitPoints,
             getNumTotalHitPoints(),
             myExperience,
             myWealth,
             RPG_Character_Common_Tools::attributesToString(myAttributes).c_str(),
             RPG_Character_Skills_Common_Tools::featsToString(myFeats).c_str(),
             RPG_Character_Skills_Common_Tools::abilitiesToString(myAbilities).c_str(),
             RPG_Character_Skills_Common_Tools::skillsToString(mySkills).c_str(),
             RPG_Magic_Common_Tools::spellsToString(myKnownSpells).c_str(),
             RPG_Magic_Common_Tools::spellsToString(mySpells).c_str()));
}

signed char
RPG_Player_Player_Base::getShieldBonus() const
{
  RPG_TRACE(ACE_TEXT("RPG_Player_Player_Base::getShieldBonus"));

  // retrieve equipped armor type
  RPG_Item_ArmorType type = myEquipment.getShield(myOffHand);
  if (type == ARMOR_NONE)
    return 0;

  const RPG_Item_ArmorProperties& properties = RPG_ITEM_DICTIONARY_SINGLETON::instance()->getArmorProperties(type);
  return properties.baseBonus;
}
