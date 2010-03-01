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
#include "rpg_character_player_base.h"

#include "rpg_character_common_tools.h"

#include <rpg_item_common.h>
#include <rpg_item_dictionary.h>

#include <rpg_common_tools.h>

#include <ace/OS.h>
#include <ace/Log_Msg.h>

RPG_Character_Player_Base::RPG_Character_Player_Base(const std::string& name_in,
                                                     const RPG_Character_Gender& gender_in,
                                                     const RPG_Character_Race& race_in,
                                                     const RPG_Character_Class& class_in,
                                                     const RPG_Character_Alignment& alignment_in,
                                                     const RPG_Character_Attributes& attributes_in,
                                                     const RPG_Character_Skills_t& skills_in,
                                                     const RPG_Character_Feats_t& feats_in,
                                                     const RPG_Character_Abilities_t& abilities_in,
                                                     const RPG_Character_OffHand& offhand_in,
                                                     const unsigned int& experience_in,
                                                     const unsigned short int& hitpoints_in,
                                                     const unsigned int& wealth_in,
                                                     const RPG_Item_List_t& inventory_in)
 : inherited(name_in,
             alignment_in,
             attributes_in,
             skills_in,
             feats_in,
             abilities_in,
             SIZE_MEDIUM, // standard PCs are all medium-sized
             hitpoints_in,
             wealth_in,
             inventory_in),
   myGender(gender_in),
   myRace(race_in),
   myClass(class_in),
   myOffHand(offhand_in),
   myExperience(experience_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Player_Base::RPG_Character_Player_Base"));

}

RPG_Character_Player_Base::RPG_Character_Player_Base(const RPG_Character_Player_Base& playerBase_in)
 : inherited(playerBase_in),
   myGender(playerBase_in.myGender),
   myRace(playerBase_in.myRace),
   myClass(playerBase_in.myClass),
   myOffHand(playerBase_in.myOffHand),
   myExperience(playerBase_in.myExperience)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Player_Base::RPG_Character_Player_Base"));

}

RPG_Character_Player_Base::~RPG_Character_Player_Base()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Player_Base::~RPG_Character_Player_Base"));

}

RPG_Character_Player_Base& RPG_Character_Player_Base::operator=(const RPG_Character_Player_Base& playerBase_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Player_Base::operator="));

  inherited::operator=(playerBase_in);
  myGender = playerBase_in.myGender;
  myRace = playerBase_in.myRace;
  myClass = playerBase_in.myClass;
  myOffHand = playerBase_in.myOffHand;
  myExperience = playerBase_in.myExperience;

  return *this;
}

const RPG_Character_Gender RPG_Character_Player_Base::getGender() const
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Player_Base::getGender"));

  return myGender;
}

const RPG_Character_Race RPG_Character_Player_Base::getRace() const
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Player_Base::getRace"));

  return myRace;
}

const RPG_Character_Class RPG_Character_Player_Base::getClass() const
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Player_Base::getClass"));

  return myClass;
}

const RPG_Character_OffHand RPG_Character_Player_Base::getOffHand() const
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Player_Base::getOffHand"));

  return myOffHand;
}

const unsigned char RPG_Character_Player_Base::getLevel(const RPG_Common_SubClass& subClass_in) const
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Player_Base::getLevel"));

  // *TODO*: consider implementing class-specific tables...
  ACE_UNUSED_ARG(subClass_in);

  unsigned char result = 0;

  result = ACE_static_cast(unsigned int,
                           ACE_OS::floor((1.0 + ::sqrt((myExperience / 125) + 1)) / 2.0));

  return result;
}

const unsigned int RPG_Character_Player_Base::getExperience() const
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Player_Base::getExperience"));

  return myExperience;
}

const RPG_Character_Equipment* RPG_Character_Player_Base::getEquipment() const
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Player_Base::getEquipment"));

  return &(inherited::myEquipment);
}

const RPG_Character_BaseAttackBonus_t RPG_Character_Player_Base::getAttackBonus(const RPG_Common_Attribute& modifier_in,
                                                                                const RPG_Combat_AttackSituation& attackSituation_in) const
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Player_Base::getAttackBonus"));

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
    for (int diff = bonus.size() - result.size();
         diff > 0;
         diff--)
      result.push_back(0);
    int index = 0;
    for (RPG_Character_BaseAttackBonusIterator_t iterator2 = bonus.begin();
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

const signed char RPG_Character_Player_Base::getArmorClass(const RPG_Combat_DefenseSituation& defenseSituation_in) const
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Player_Base::getArmorClass"));

  // AC = 10 + armor bonus + shield bonus + DEX modifier + size modifier [+ other modifiers]
  signed char result = 10;

  // retrieve equipped armor type
  RPG_Item_ArmorType type = myEquipment.getArmor();
  RPG_Item_ArmorProperties properties;
  if (type != ARMOR_NONE)
  {
    properties = RPG_ITEM_DICTIONARY_SINGLETON::instance()->getArmorProperties(type);
    result += properties.baseArmorBonus;
  } // end IF
  result += getShieldBonus();

  // consider defense situation
  int DEX_modifier = 0;
  if (defenseSituation_in != DEFENSE_FLATFOOTED)
  {
    DEX_modifier = RPG_Character_Common_Tools::getAttributeAbilityModifier(getAttribute(ATTRIBUTE_DEXTERITY));
    if (type != ARMOR_NONE)
    {
      DEX_modifier = std::min(ACE_static_cast(int, properties.maxDexterityBonus),
                              ACE_static_cast(int, DEX_modifier));
    } // end IF
  } // end IF
  result += DEX_modifier;

  // usually, this is irrelevant (SIZE_MEDIUM --> +/-0), but may have changed temporarily, magically etc...
  result += RPG_Common_Tools::getSizeModifier(getSize());

  return result;
}

const bool RPG_Character_Player_Base::isPlayerCharacter() const
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Player_Base::isPlayerCharacter"));

  return true;
}

void RPG_Character_Player_Base::gainExperience(const unsigned int& XP_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Player_Base::gainExperience"));

  unsigned char old_level = getLevel(*myClass.subClasses.begin());

  myExperience += XP_in;

  // *TODO*: trigger level-up
  if (old_level != getLevel(*myClass.subClasses.begin()))
  {
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("player: \"%s\" (XP: %d) has gained a level (%d)...\n"),
               getName().c_str(),
               myExperience,
               ACE_static_cast(int, getLevel(*myClass.subClasses.begin()))));
  } // end IF
}

const unsigned int RPG_Character_Player_Base::rest(const RPG_Common_Camp& type_in,
                                                   const unsigned int& hours_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Player_Base::rest"));

  // *TODO*: consider dead/dying players !
  if (myNumCurrentHitPoints < 0)
  {
    // cannot rest --> will die
    return 0;
  } // end IF

  // consider natural healing...
  unsigned int restedPeriod = 0;
  int missingHPs = getNumTotalHitPoints() - myNumCurrentHitPoints;
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
    myNumCurrentHitPoints = getNumTotalHitPoints() - missingHPs;
  } // end IF

  // adjust condition
  if (myNumCurrentHitPoints > 0)
  {
    myConditions.insert(CONDITION_NORMAL);
    myConditions.erase(CONDITION_DISABLED);
  } // end IF

  return (restedPeriod * 24);
}

void RPG_Character_Player_Base::status() const
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Player_Base::status"));

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("name: \"%s\" (XP: %d (%d))\n"),
             getName().c_str(),
             myExperience,
             ACE_static_cast(int, getLevel())));

  inherited::status();
}

void RPG_Character_Player_Base::dump() const
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Player_Base::dump"));

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("Player: \nGender: %s\nRace: %s\nClass: %s\nXP: %d\n"),
             RPG_Character_GenderHelper::RPG_Character_GenderToString(myGender).c_str(),
             RPG_Character_RaceHelper::RPG_Character_RaceToString(myRace).c_str(),
             RPG_Character_Common_Tools::classToString(myClass).c_str(),
             myExperience));

  inherited::dump();
}

const signed char RPG_Character_Player_Base::getShieldBonus() const
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Player_Base::getShieldBonus"));

  // retrieve equipped armor type
  RPG_Item_ArmorType type = myEquipment.getShield(myOffHand);
  if (type == ARMOR_NONE)
    return 0;

  RPG_Item_ArmorProperties properties = RPG_ITEM_DICTIONARY_SINGLETON::instance()->getArmorProperties(type);
  return properties.baseArmorBonus;
}
