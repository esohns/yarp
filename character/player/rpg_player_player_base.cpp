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

#include <algorithm>

#include "ace/Log_Msg.h"
#include "ace/OS.h"

#include "common_tools.h"

#include "rpg_common_defines.h"
#include "rpg_common_macros.h"
#include "rpg_common_tools.h"

#include "rpg_character_common_tools.h"
#include "rpg_character_defines.h"
#include "rpg_character_race_common_tools.h"
#include "rpg_character_skills_common_tools.h"

#include "rpg_magic_common_tools.h"

#include "rpg_item_armor.h"
#include "rpg_item_commodity.h"
#include "rpg_item_common.h"
#include "rpg_item_common_tools.h"
#include "rpg_item_dictionary.h"
#include "rpg_item_instance_manager.h"
#include "rpg_item_weapon.h"

#include "rpg_combat_common_tools.h"

RPG_Player_Player_Base::RPG_Player_Player_Base (// base attributes
                                                const std::string& name_in,
                                                enum RPG_Character_Gender gender_in,
                                                const RPG_Character_Race_t& race_in,
                                                const struct RPG_Character_Class& class_in,
                                                const struct RPG_Character_Alignment& alignment_in,
                                                const struct RPG_Character_Attributes& attributes_in,
                                                const RPG_Character_Skills_t& skills_in,
                                                const RPG_Character_Feats_t& feats_in,
                                                const RPG_Character_Abilities_t& abilities_in,
                                                enum RPG_Character_OffHand offHand_in,
                                                ACE_UINT16 maxHitPoints_in,
                                                const RPG_Magic_SpellTypes_t& knownSpells_in,
                                                // current status
                                                const RPG_Character_Conditions_t& condition_in,
                                                ACE_INT16 hitpoints_in,
                                                ACE_UINT64 experience_in,
                                                ACE_UINT64 wealth_in,
                                                const RPG_Magic_Spells_t& spells_in,
                                                const RPG_Item_List_t& inventory_in)
 : inherited (name_in,
              alignment_in,
              attributes_in,
              skills_in,
              feats_in,
              abilities_in,
              maxHitPoints_in,
              knownSpells_in,
              condition_in,
              hitpoints_in,
              wealth_in,
              spells_in,
              inventory_in),
   myGender (gender_in),
   myRace (race_in),
   myClass (class_in),
   myOffHand (offHand_in),
   myExperience (experience_in),
   mySize (RPG_Character_Race_Common_Tools::raceToSize (race_in))
{
  RPG_TRACE (ACE_TEXT ("RPG_Player_Player_Base::RPG_Player_Player_Base"));

}

RPG_Player_Player_Base::RPG_Player_Player_Base (const RPG_Player_Player_Base& playerBase_in)
 : inherited (playerBase_in),
   myGender (playerBase_in.myGender),
   myRace (playerBase_in.myRace),
   myClass (playerBase_in.myClass),
   myOffHand (playerBase_in.myOffHand),
   myExperience (playerBase_in.myExperience),
   mySize (playerBase_in.mySize)
{
  RPG_TRACE (ACE_TEXT ("RPG_Player_Player_Base::RPG_Player_Player_Base"));

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
RPG_Player_Player_Base::initialize (// base attributes
                                    const std::string& name_in,
                                    enum RPG_Character_Gender gender_in,
                                    const RPG_Character_Race_t& race_in,
                                    const struct RPG_Character_Class& class_in,
                                    const struct RPG_Character_Alignment& alignment_in,
                                    const struct RPG_Character_Attributes& attributes_in,
                                    const RPG_Character_Skills_t& skills_in,
                                    const RPG_Character_Feats_t& feats_in,
                                    const RPG_Character_Abilities_t& abilities_in,
                                    enum RPG_Character_OffHand offHand_in,
                                    ACE_UINT16 maxHitPoints_in,
                                    const RPG_Magic_SpellTypes_t& knownSpells_in,
                                    // current status
                                    const RPG_Character_Conditions_t& condition_in,
                                    ACE_INT16 hitpoints_in,
                                    ACE_UINT64 experience_in,
                                    ACE_UINT64 wealth_in,
                                    const RPG_Magic_Spells_t& spells_in,
                                    const RPG_Item_List_t& inventory_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Player_Player_Base::init"));

  // init base class
  inherited::initialize (// base attributes
                         name_in,
                         alignment_in,
                         attributes_in,
                         skills_in,
                         feats_in,
                         abilities_in,
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
  mySize       = RPG_Character_Race_Common_Tools::raceToSize (race_in);
}

RPG_Character_Level_t
RPG_Player_Player_Base::getLevel (enum RPG_Common_SubClass subClass_in) const
{
  RPG_TRACE (ACE_TEXT ("RPG_Player_Player_Base::getLevel"));

  // *TODO*: implement class-specific tables
  ACE_UNUSED_ARG (subClass_in);

  RPG_Character_Level_t result =
    static_cast<RPG_Character_Level_t> (ACE_OS::floor ((1.0f +
                                                        std::sqrt ((myExperience / 125.0f) + 1.0f)) / 2.0f));

  return result;
}

RPG_Character_BaseAttackBonus_t
RPG_Player_Player_Base::getAttackBonus (enum RPG_Common_Attribute modifier_in,
                                        enum RPG_Combat_AttackSituation attackSituation_in) const
{
  RPG_TRACE (ACE_TEXT ("RPG_Player_Player_Base::getAttackBonus"));

  // sanity check(s)
  ACE_ASSERT ((modifier_in == ATTRIBUTE_DEXTERITY) || (modifier_in == ATTRIBUTE_STRENGTH));

  // *NOTE*: Attack Bonus = base attack bonus + STR/DEX modifier + size modifier
  //         [+ range penalty + other modifiers]
  RPG_Character_BaseAttackBonus_t result;

  // attack bonusses stack for multiclass characters...
  for (RPG_Character_SubClassesIterator_t iterator = myClass.subClasses.begin ();
       iterator != myClass.subClasses.end ();
       iterator++)
  {
    RPG_Character_BaseAttackBonus_t bonus =
        RPG_Character_Common_Tools::getBaseAttackBonus (*iterator,
                                                        getLevel (*iterator));
    // append necessary entries
    for (int diff = static_cast<int> (bonus.size () - result.size ());
         diff > 0;
         diff--)
      result.push_back (0);
    int index = 0;
    for (RPG_Character_BaseAttackBonusConstIterator_t iterator2 = bonus.begin ();
         iterator2 != bonus.end ();
         iterator2++, index++)
      result[index] += *iterator2;
  } // end FOR

  ACE_INT8 abilityModifier =
      RPG_Character_Common_Tools::getAttributeAbilityModifier (getAttribute (modifier_in));
  ACE_INT8 sizeModifier = RPG_Common_Tools::getSizeModifier (getSize ());
  for (RPG_Character_BaseAttackBonusIterator_t iterator = result.begin ();
       iterator != result.end ();
       iterator++)
  {
    (*iterator) += abilityModifier;
    (*iterator) += sizeModifier;
  } // end FOR

  return result;
}

ACE_INT8
RPG_Player_Player_Base::getArmorClass (enum RPG_Combat_DefenseSituation defenseSituation_in) const
{
  RPG_TRACE (ACE_TEXT ("RPG_Player_Player_Base::getArmorClass"));

  // *NOTE*: AC = 10 + armor bonus + shield bonus + DEX modifier + size modifier
  //         [+ other modifiers]
  ACE_INT8 result = 10;

  struct RPG_Item_ArmorProperties properties_s;
  ACE_OS::memset (&properties_s, 0, sizeof (struct RPG_Item_ArmorProperties));

  // retrieve equipped armor type
  enum RPG_Item_ArmorType type = myEquipment.getBodyArmor ();
  if (type != ARMOR_NONE)
  {
    properties_s =
      RPG_ITEM_DICTIONARY_SINGLETON::instance ()->getArmorProperties (type);
    result += properties_s.baseBonus;
  } // end IF
  result += getShieldBonus ();

  // consider defense situation
  ACE_INT8 DEX_modifier = 0;
  if (defenseSituation_in != DEFENSE_FLATFOOTED)
  {
    DEX_modifier =
      RPG_Character_Common_Tools::getAttributeAbilityModifier (getAttribute (ATTRIBUTE_DEXTERITY));
    if (type != ARMOR_NONE)
      DEX_modifier = std::min (static_cast<ACE_INT8> (properties_s.maxDexterityBonus),
                               DEX_modifier);
  } // end IF
  result += DEX_modifier;

  // *NOTE*: usually, this is irrelevant (SIZE_MEDIUM --> +/-0), but may have
  // changed temporarily, magically etc...
  result += RPG_Common_Tools::getSizeModifier (getSize ());

  return result;
}

ACE_UINT16
RPG_Player_Player_Base::getReach (ACE_UINT16& baseRange_out,
                                  bool& reachIsAbsolute_out) const
{
  RPG_TRACE (ACE_TEXT ("RPG_Player_Player_Base::getReach"));

  // init return value(s)
  baseRange_out = 0;
  reachIsAbsolute_out = false;

  // *TODO*: consider polymorphed states...
  unsigned short result = RPG_Common_Tools::sizeToReach (mySize, true);

  RPG_Item_WeaponType weapon_type = myEquipment.getPrimaryWeapon(myOffHand);
  // sanity check: equipped any weapon ?
  if (weapon_type == RPG_ITEM_WEAPONTYPE_INVALID)
    return result;

  const RPG_Item_WeaponProperties& properties =
      RPG_ITEM_DICTIONARY_SINGLETON::instance ()->getWeaponProperties (weapon_type);
  if (RPG_Item_Common_Tools::isMeleeWeapon (weapon_type))
  {
    if (properties.isReachWeapon)
    {
      result *= 2;
      reachIsAbsolute_out =
          RPG_Item_Common_Tools::hasAbsoluteReach (weapon_type);
    } // end IF
  } // end IF
  else
  {
    // --> ranged weapon
    ACE_ASSERT(RPG_Item_Common_Tools::isRangedWeapon (weapon_type));

    baseRange_out = properties.rangeIncrement;

    // compute max reach for ranged weapons
    if (RPG_Item_Common_Tools::isThrownWeapon (weapon_type))
      result = baseRange_out * 5;
    else if (RPG_Item_Common_Tools::isProjectileWeapon (weapon_type))
      result = baseRange_out * 10;
    else
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid weapon type (was \"%s\"), continuing\n"),
                  ACE_TEXT (RPG_Item_WeaponTypeHelper::RPG_Item_WeaponTypeToString (weapon_type).c_str ())));
      ACE_ASSERT (false);
    } // end IF
  } // end ELSE

  return result;
}

ACE_UINT8
RPG_Player_Player_Base::getSpeed (bool isRunning_in,
                                  enum RPG_Common_AmbientLighting lighting_in,
                                  enum RPG_Common_Terrain terrain_in,
                                  enum RPG_Common_Track track_in) const
{
  RPG_TRACE (ACE_TEXT ("RPG_Player_Player_Base::getSpeed"));

  // sanity check(s)
  ACE_ASSERT (lighting_in != RPG_COMMON_AMBIENTLIGHTING_INVALID);

  // initialize return value
  ACE_UINT8 result = 0;

  // step1: retrieve base speed (race)
  ACE_UINT8 base_speed = 0;
  unsigned int race_index = 1;
  for (unsigned int index = 0;
       index < myRace.size ();
       index++, race_index++)
    if (myRace.test (index))
    {
      base_speed =
          RPG_Character_Race_Common_Tools::raceToSpeed (static_cast<RPG_Character_Race>(race_index));
      if (base_speed > result)
        result = base_speed;
    } // end IF
  ACE_ASSERT (base_speed);

  // step2: consider encumbrance (armor / load)
  RPG_Character_Encumbrance encumbrance_by_armor = LOAD_LIGHT;
  RPG_Item_ArmorType armor_type =
      const_cast<RPG_Player_Player_Base*> (this)->getEquipment ().getBodyArmor ();
  if (armor_type != ARMOR_NONE)
  {
    const RPG_Item_ArmorProperties& properties =
        RPG_ITEM_DICTIONARY_SINGLETON::instance ()->getArmorProperties (armor_type);
    switch (properties.category)
    {
      case ARMORCATEGORY_LIGHT:
        encumbrance_by_armor = LOAD_LIGHT; break;
      case ARMORCATEGORY_MEDIUM:
        encumbrance_by_armor = LOAD_MEDIUM; break;
      case ARMORCATEGORY_HEAVY:
        encumbrance_by_armor = LOAD_HEAVY; break;
      default:
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("invalid (body) armor category (was \"%s\"), aborting\n"),
                   ACE_TEXT(RPG_Item_ArmorCategoryHelper::RPG_Item_ArmorCategoryToString(properties.category).c_str())));

        return 0;
      }
    } // end SWITCH
  } // end IF
  // *NOTE*: dwarves move at the base speed with any armor...
  if (RPG_Character_Race_Common_Tools::hasRace (myRace, RACE_DWARF))
    encumbrance_by_armor = LOAD_LIGHT;
  // *TODO*: consider non-bipeds...
  RPG_Character_Encumbrance encumbrance_by_load =
      RPG_Character_Common_Tools::getEncumbrance (getAttribute (ATTRIBUTE_STRENGTH),
                                                  getSize (),
                                                  getInventory ().getTotalWeight (),
                                                  true);
  signed char maxDexModifierAC = std::numeric_limits<signed char>::max ();
  signed char armorCheckPenalty = 0;
  unsigned char runModifier = RPG_CHARACTER_RUN_MODIFIER_MEDIUM;
  RPG_Character_Common_Tools::getLoadModifiers (((encumbrance_by_armor > encumbrance_by_load) ? encumbrance_by_armor
                                                                                              : encumbrance_by_load),
                                                base_speed,
                                                maxDexModifierAC,
                                                armorCheckPenalty,
                                                result,
                                                runModifier);

  float modifier = 1.0F;
  // step3: consider vision [equipment / ambient lighting]
  if ((const_cast<RPG_Player_Player_Base*> (this)->getEquipment ().getLightSource () ==
       RPG_ITEM_COMMODITYLIGHT_INVALID) &&
      (lighting_in == AMBIENCE_DARKNESS))
    modifier *= 0.5F;

  // step4: consider terrain [track type]
  modifier *= RPG_Common_Tools::terrainToSpeedModifier (terrain_in, track_in);

  // step5: consider movement mode
  if (isRunning_in)
    modifier *= static_cast<float>(runModifier);

  result = static_cast<unsigned char>(static_cast<float>(result) * modifier);

  // *TODO*: consider other (spell, ...) effects
  return result;
}

enum RPG_Common_SubClass
RPG_Player_Player_Base::gainExperience (ACE_UINT64 XP_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Player_Player_Base::gainExperience"));

  std::vector<ACE_UINT8> levels;
  for (RPG_Character_SubClassesIterator_t iterator = myClass.subClasses.begin ();
       iterator != myClass.subClasses.end ();
       iterator++)
    levels.push_back (getLevel (*iterator));

  myExperience += XP_in;

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("\"%s\" gained %Q XP (total: %Q)...\n"),
              ACE_TEXT (getName ().c_str ()),
              XP_in,
              myExperience));

  // gained a class level ?
  unsigned int index = 0;
  for (RPG_Character_SubClassesIterator_t iterator = myClass.subClasses.begin ();
       iterator != myClass.subClasses.end ();
       iterator++, index++)
    if (getLevel (*iterator) != levels[index])
    {
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("player: \"%s\" (XP: %Q) has reached level %u as %s...\n"),
                  ACE_TEXT (getName ().c_str ()),
                  myExperience,
                  static_cast<unsigned int> (getLevel (*iterator)),
                  ACE_TEXT (RPG_Common_SubClassHelper::RPG_Common_SubClassToString (*iterator).c_str ())));
      return *iterator;
    } // end IF

  return RPG_COMMON_SUBCLASS_INVALID;
}

unsigned int
RPG_Player_Player_Base::stabilize (const RPG_Player_Player_Base* const player_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Player_Player_Base::stabilize"));

  unsigned int result = 0;

  // sanity check(s)
  if (!hasCondition (CONDITION_DYING))
    return 0; // nothing to do...
  if (!player_in) // there is no one (left) to help us out...
    goto natural_healing_; // *TODO*: (if conscious,) cast any healing spells on self first
  ACE_ASSERT (player_in->isPlayerCharacter ());

  // step1: *TODO*: cast any healing spells
  //goto continue_;

//first_aid:
  // step2: (try to) apply 'First Aid' skill to the player
  while (!RPG_Character_Common_Tools::getSkillCheck (RPG_Character_Skills_Common_Tools::getSkillRank (SKILL_HEAL,
                                                                                                      player_in->getSkillPoints (SKILL_HEAL),
                                                                                                      player_in->getClass (),
                                                                                                      player_in->getLevel (SUBCLASS_NONE)),
                                                     RPG_Character_Common_Tools::getAttributeAbilityModifier (player_in->getAttribute (RPG_Character_Skills_Common_Tools::skillToAttribute (SKILL_HEAL))),
                                                     0, // miscellaneous modifiers *TODO*: support healers' kits, feat bonusses, etc...
                                                     RPG_COMMON_SKILL_DC_HEAL_FIRST_AID))
    result += 1; // a HEAL skill check takes a standard action
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("\"%s\" stabilized \"%s\" in %u round(s), continuing\n"),
              ACE_TEXT (player_in->getName ().c_str ()),
              ACE_TEXT (getName ().c_str ()),
              result));

  while (hasCondition (CONDITION_UNCONSCIOUS))
  {
    if (Common_Tools::testRandomProbability (RPG_COMMON_UNAIDED_STABILIZE_P))
    {
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("stabilized \"%s\" regained consciousness in %u round(s), continuing\n"),
                  ACE_TEXT (getName ().c_str ()),
                  result));
      myCondition.erase (CONDITION_UNCONSCIOUS);
      myCondition.insert (CONDITION_DISABLED);
      goto continue_; // success !
    } // end IF
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("\"%s\" failed to regain consciousness, continuing\n"),
                ACE_TEXT (getName ().c_str ())));
    result += 600; // one hour has passed...
  } // end WHILE

  goto continue_;

natural_healing_:
  while (myNumHitPoints > -10)
  {
    if (Common_Tools::testRandomProbability (RPG_COMMON_UNAIDED_STABILIZE_P))
    {
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("\"%s\" stabilized naturally in %u round(s), continuing\n"),
                  ACE_TEXT (getName ().c_str ()),
                  result));
      break; // success !
    } // end IF
    --myNumHitPoints; // just lost one HP
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("\"%s\" failed to stabilize (HP: %d/%u), continuing\n"),
                ACE_TEXT (getName ().c_str ()),
                myNumHitPoints, getNumTotalHitPoints ()));
    result += 1; // one round has passed...
  } // end WHILE
  if (myNumHitPoints == -10)
    goto continue_; // character bled to death !

  while ((myNumHitPoints > -10) &&
         hasCondition (CONDITION_UNCONSCIOUS))
  {
    if (Common_Tools::testRandomProbability (RPG_COMMON_UNAIDED_STABILIZE_P))
    {
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("naturally stabilized \"%s\" regained consciousness in %u round(s), continuing\n"),
                  ACE_TEXT (getName ().c_str ()),
                  result));
      myCondition.erase (CONDITION_UNCONSCIOUS);
      myCondition.insert (CONDITION_DISABLED);
      break; // success !
    } // end IF
    --myNumHitPoints; // just lost one HP
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("\"%s\" failed to regain consciousness (HP: %d/%u), continuing\n"),
                ACE_TEXT (getName ().c_str ()),
                myNumHitPoints, getNumTotalHitPoints ()));
    result += 600;    // one hour has passed...
  } // end WHILE
  if (myNumHitPoints == -10)
    goto continue_; // character bled to death !

  while (myNumHitPoints > -10)
  {
    if (Common_Tools::testRandomProbability (RPG_COMMON_UNAIDED_STABILIZE_P))
    {
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("naturally stabilized \"%s\" started recovery in %u round(s), continuing\n"),
                  ACE_TEXT (getName ().c_str ()),
                  result));
      break; // success !
    } // end IF
    --myNumHitPoints; // just lost one HP
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("\"%s\" failed to start natural recovery (HP: %d/%u), continuing\n"),
                ACE_TEXT (getName ().c_str ()),
                myNumHitPoints, getNumTotalHitPoints ()));
    result += (24 * 600); // one day has passed...
  } // end WHILE

continue_:
  myCondition.erase (CONDITION_DYING);
  if (myNumHitPoints == -10)
    myCondition.insert (CONDITION_DEAD);
  else
    myCondition.insert (CONDITION_STABLE);

  return result;
}

unsigned int
RPG_Player_Player_Base::rest (enum RPG_Common_Camp type_in,
                              unsigned int hours_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Player_Player_Base::rest"));

  unsigned int rested_hours_i = 0;

  // sanity check(s)
start:
  ACE_ASSERT (myNumHitPoints >= -10);
  if (hasCondition (CONDITION_DEAD))
  { // resting does not do anything for this player in this condition
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("(further) resting \"%s\" is futile; player is dead, returning\n"),
                ACE_TEXT (getName ().c_str ())));
    return rested_hours_i * 3600;
  } // end IF

  if (myNumHitPoints < 0)
  {
    if (hasCondition (CONDITION_DYING))
    {
      // (try to) stabilize self
      unsigned int recovery_time_in_rounds_i =
        stabilize (hasCondition (CONDITION_UNCONSCIOUS) ? NULL : this);
      recovery_time_in_rounds_i *= RPG_COMMON_COMBAT_ROUND_INTERVAL_S;
      recovery_time_in_rounds_i /= 3600; // s --> h
      rested_hours_i += recovery_time_in_rounds_i;
      goto start;
    } // end IF
  } // end IF
  ACE_ASSERT (!hasCondition (CONDITION_DYING));

  // *TODO*: cast any healing spells on self first to speed things up

  // consider natural healing
  ACE_INT16 missingHPs = getNumTotalHitPoints () - myNumHitPoints;
  RPG_Character_Level_t level_i = getLevel (SUBCLASS_NONE);
  unsigned int recovery_rate = level_i;
  if (type_in == REST_FULL)
    recovery_rate *= 2;
  if (missingHPs == 0)
    goto continue_;

  // OK: (naturally) recovered some HPs
  if (type_in == REST_FULL)
  {
    unsigned int fraction = 0;
    while ((missingHPs > 0) && (rested_hours_i < hours_in))
    {
      // just a fraction left...
      if ((hours_in - rested_hours_i) < 24)
      {
        fraction = hours_in - rested_hours_i;
        break;
      } // end IF

      missingHPs -= recovery_rate;
      rested_hours_i += 24;
    } // end WHILE

    recovery_rate = level_i;
    while ((missingHPs > 0) && (fraction >= 8))
    {
      missingHPs -= recovery_rate;
      rested_hours_i += 8;

      fraction -= 8;
    } // end WHILE
  } // end IF
  else // *TODO* use a switch statement
  {
    while ((missingHPs > 0) && (rested_hours_i < hours_in))
    {
      missingHPs -= recovery_rate;
      rested_hours_i += 8;
    } // end WHILE
  } // end ELSE
  if (missingHPs < 0)
    missingHPs = 0;
  myNumHitPoints = getNumTotalHitPoints () - missingHPs;

continue_:
  // adjust condition
  if (myNumHitPoints > 0)
  {
    myCondition.insert (CONDITION_NORMAL);
    myCondition.erase (CONDITION_STABLE);
    myCondition.erase (CONDITION_DISABLED);
    myCondition.erase (CONDITION_UNCONSCIOUS);
  } // end IF
  else if (myNumHitPoints == 0)
  {
    myCondition.insert (CONDITION_DISABLED);
    myCondition.erase (CONDITION_STABLE);
    myCondition.erase (CONDITION_UNCONSCIOUS);
  } // end ELSE IF

  return (rested_hours_i * 3600);
}

void
RPG_Player_Player_Base::defaultEquip ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Player_Player_Base::defaultEquip"));

  // remove everything
  inherited::myEquipment.strip ();

  struct RPG_Character_EquipmentSlots slots_s;
  RPG_Item_Base* handle = NULL;
  for (RPG_Item_ListIterator_t iterator = inherited::myInventory.myItems.begin ();
       iterator != inherited::myInventory.myItems.end ();
       iterator++)
  {
    slots_s.slots.clear ();
    slots_s.is_inclusive = false;
    RPG_Item_Common_Tools::itemToSlot (*iterator,
                                       myOffHand,
                                       slots_s);
    if (slots_s.slots.empty ())
    { // *NOTE*: perhaps the item dictionary has not been initialized (yet) ?
      ACE_DEBUG ((LM_WARNING,
                  ACE_TEXT ("failed to RPG_Item_Common_Tools::itemToSlot (id was: %d), continuing\n"),
                  *iterator));
      continue;
    } // end IF

    handle = NULL;
    if (!RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance ()->get (*iterator,
                                                                handle))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid item ID (was: %d), aborting\n"),
                  *iterator));
      return;
    } // end IF
    ACE_ASSERT (handle);

    switch (handle->type ())
    {
      case ITEM_ARMOR:
      {
        // RPG_Item_Armor* armor = static_cast<RPG_Item_Armor*> (handle);
//         RPG_Item_ArmorProperties properties = RPG_ITEM_DICTIONARY_SINGLETON::instance()->getArmorProperties(armor_base->getArmorType());

        if (myEquipment.isEquipped (*iterator,
                                    slots_s.slots.front ()))
          break; // cannot equip...

        myEquipment.equip (*iterator,
                           myOffHand,
                           slots_s.slots.front ());
        break;
      }
      case ITEM_COMMODITY:
      {
        RPG_Item_Commodity* commodity = static_cast<RPG_Item_Commodity*> (handle);
        RPG_Item_CommodityUnion commodity_type = commodity->subtype_;
        //RPG_Item_CommodityProperties properties = RPG_ITEM_DICTIONARY_SINGLETON::instance()->getCommodityProperties(commodity->getCommoditySubType());
        // - by default, equip light sources only
        if (commodity_type.discriminator != RPG_Item_CommodityUnion::COMMODITYLIGHT)
          break;

        myEquipment.equip (*iterator,
                           myOffHand,
                           slots_s.slots.front ());
        break;
      }
      case ITEM_WEAPON:
      {
        RPG_Item_Weapon* weapon = static_cast<RPG_Item_Weapon*> (handle);
//         RPG_Item_WeaponProperties properties = RPG_ITEM_DICTIONARY_SINGLETON::instance()->getWeaponProperties(weapon_base->getWeaponType());
        // - by default, equip melee weapons only
        // *TODO*: what about other types of weapons ?
        if (!RPG_Item_Common_Tools::isMeleeWeapon (weapon->weaponType_))
          break;
        if (myEquipment.isEquipped (*iterator,
                                    slots_s.slots.front ()))
          break; // cannot equip...

        myEquipment.equip (*iterator,
                           myOffHand,
                           slots_s.slots.front ());
        break;
      }
      default:
      {
        //ACE_DEBUG((LM_DEBUG,
        //           ACE_TEXT("item ID %d: invalid type: \"%s\", continuing\n"),
        //           *iterator,
        //           ACE_TEXT(RPG_Item_TypeHelper::RPG_Item_TypeToString(handle->getType()).c_str())));
        break;
      }
    } // end SWITCH
  } // end FOR
}

void
RPG_Player_Player_Base::status () const
{
  RPG_TRACE (ACE_TEXT ("RPG_Player_Player_Base::status"));

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT("name: \"%s\" (XP: %Q (%u))\n"),
              ACE_TEXT (getName ().c_str ()),
              myExperience,
              static_cast<unsigned int> (getLevel ())));

  inherited::status ();
}

void
RPG_Player_Player_Base::dump () const
{
  RPG_TRACE (ACE_TEXT ("RPG_Player_Player_Base::dump"));

  // *TODO*: add items
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("Player \"%s\": \nGender: %s\nRace: %s\nClass: %s\nAlignment: %s\nCondition: %s\nHP: %d/%u\nXP: %Q\nGold: %Q\nAttributes:\n===========\n%sFeats:\n======\n%sAbilities:\n==========\n%sSkills:\n=======\n%sSpells (known):\n=======\n%sSpells (prepared):\n=======\n%sItems:\n======\n"),
              ACE_TEXT (getName ().c_str ()),
              ACE_TEXT (RPG_Character_GenderHelper::RPG_Character_GenderToString (myGender).c_str ()),
              ACE_TEXT (RPG_Character_Common_Tools::toString (myRace).c_str ()),
              ACE_TEXT (RPG_Character_Common_Tools::toString (myClass).c_str ()),
              ACE_TEXT (RPG_Character_Common_Tools::toString (getAlignment ()).c_str ()),
              ACE_TEXT (RPG_Character_Common_Tools::toString (myCondition).c_str ()),
              myNumHitPoints,
              getNumTotalHitPoints (),
              myExperience,
              myWealth,
              ACE_TEXT (RPG_Character_Common_Tools::toString (myAttributes).c_str ()),
              ACE_TEXT (RPG_Character_Skills_Common_Tools::toString (myFeats).c_str ()),
              ACE_TEXT (RPG_Character_Skills_Common_Tools::toString (myAbilities).c_str ()),
              ACE_TEXT (RPG_Character_Skills_Common_Tools::toString (mySkills).c_str ()),
              ACE_TEXT (RPG_Magic_Common_Tools::toString (myKnownSpells).c_str ()),
              ACE_TEXT (RPG_Magic_Common_Tools::toString (mySpells).c_str ())));
}

ACE_INT8
RPG_Player_Player_Base::getShieldBonus () const
{
  RPG_TRACE (ACE_TEXT ("RPG_Player_Player_Base::getShieldBonus"));

  // retrieve equipped armor type
  RPG_Item_ArmorType type = myEquipment.getShield (myOffHand);
  if (type == ARMOR_NONE)
    return 0;

  const RPG_Item_ArmorProperties& properties =
      RPG_ITEM_DICTIONARY_SINGLETON::instance ()->getArmorProperties (type);

  return properties.baseBonus;
}
