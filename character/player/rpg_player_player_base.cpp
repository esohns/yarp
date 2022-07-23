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

#include "rpg_combat_common_tools.h"

#include "rpg_item_common.h"
#include "rpg_item_armor.h"
#include "rpg_item_commodity.h"
#include "rpg_item_weapon.h"
#include "rpg_item_dictionary.h"
#include "rpg_item_instance_manager.h"
#include "rpg_item_common_tools.h"

#include "rpg_character_defines.h"
#include "rpg_character_common_tools.h"
#include "rpg_character_race_common_tools.h"
#include "rpg_character_skills_common_tools.h"

#include "rpg_magic_common_tools.h"

#include "rpg_common_macros.h"
#include "rpg_common_tools.h"

#include <ace/OS.h>
#include <ace/Log_Msg.h>

#include <algorithm>

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
                                                unsigned short maxHitPoints_in,
                                                const RPG_Magic_SpellTypes_t& knownSpells_in,
                                                // current status
                                                const RPG_Character_Conditions_t& condition_in,
                                                short hitpoints_in,
                                                unsigned int experience_in,
                                                unsigned int wealth_in,
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
                                    unsigned short maxHitPoints_in,
                                    const RPG_Magic_SpellTypes_t& knownSpells_in,
                                    // current status
                                    const RPG_Character_Conditions_t& condition_in,
                                    short hitpoints_in,
                                    unsigned int experience_in,
                                    unsigned int wealth_in,
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
  mySize       = RPG_Character_Race_Common_Tools::raceToSize(race_in);
}

enum RPG_Character_Gender
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

const struct RPG_Character_Class&
RPG_Player_Player_Base::getClass() const
{
  RPG_TRACE(ACE_TEXT("RPG_Player_Player_Base::getClass"));

  return myClass;
}

enum RPG_Character_OffHand
RPG_Player_Player_Base::getOffHand() const
{
  RPG_TRACE(ACE_TEXT("RPG_Player_Player_Base::getOffHand"));

  return myOffHand;
}

enum RPG_Common_Size
RPG_Player_Player_Base::getSize() const
{
  RPG_TRACE(ACE_TEXT("RPG_Player_Player_Base::getSize"));

  return mySize;
}

unsigned char
RPG_Player_Player_Base::getLevel(const RPG_Common_SubClass& subClass_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Player_Player_Base::getLevel"));

  // *TODO*: implement class-specific tables
  ACE_UNUSED_ARG(subClass_in);

  unsigned char result = 0;

  result =
      static_cast<unsigned char>(ACE_OS::floor((1.0 +
                                                ::sqrt(static_cast<double>(myExperience / 125) + 1.0)) / 2.0));

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

  int abilityModifier =
      RPG_Character_Common_Tools::getAttributeAbilityModifier (getAttribute (modifier_in));
  int sizeModifier = RPG_Common_Tools::getSizeModifier (getSize ());
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
  signed char result = 10;

  // retrieve equipped armor type
  RPG_Item_ArmorProperties properties_s;
  ACE_OS::memset (&properties_s, 0, sizeof (RPG_Item_ArmorProperties));
  RPG_Item_ArmorType type = myEquipment.getBodyArmor();
  if (type != ARMOR_NONE)
  {
    properties_s =
        RPG_ITEM_DICTIONARY_SINGLETON::instance ()->getArmorProperties (type);
    result += properties_s.baseBonus;
  } // end IF
  result += getShieldBonus ();

  // consider defense situation
  int DEX_modifier = 0;
  if (defenseSituation_in != DEFENSE_FLATFOOTED)
  {
    DEX_modifier =
        RPG_Character_Common_Tools::getAttributeAbilityModifier (getAttribute (ATTRIBUTE_DEXTERITY));
    if (type != ARMOR_NONE)
      DEX_modifier =
          std::min<int> (static_cast<int> (properties_s.maxDexterityBonus),
                         DEX_modifier);
  } // end IF
  result += DEX_modifier;

  // *NOTE*: usually, this is irrelevant (SIZE_MEDIUM --> +/-0), but may have
  // changed temporarily, magically etc...
  result += RPG_Common_Tools::getSizeModifier (getSize ());

  return result;
}

unsigned short
RPG_Player_Player_Base::getReach(unsigned short& baseRange_out,
                                 bool& reachIsAbsolute_out) const
{
  RPG_TRACE(ACE_TEXT("RPG_Player_Player_Base::getReach"));

  // init return value(s)
  baseRange_out = 0;
  reachIsAbsolute_out = false;

  // *TODO*: consider polymorphed states...
  unsigned short result = RPG_Common_Tools::sizeToReach(mySize, true);

  RPG_Item_WeaponType weapon_type = myEquipment.getPrimaryWeapon(myOffHand);
  // sanity check: equipped any weapon ?
  if (weapon_type == RPG_ITEM_WEAPONTYPE_INVALID)
    return result;

  const RPG_Item_WeaponProperties& properties =
      RPG_ITEM_DICTIONARY_SINGLETON::instance()->getWeaponProperties(weapon_type);
  if (RPG_Item_Common_Tools::isMeleeWeapon(weapon_type))
  {
    if (properties.isReachWeapon)
    {
      result *= 2;
      reachIsAbsolute_out =
          RPG_Item_Common_Tools::hasAbsoluteReach(weapon_type);
    } // end IF
  } // end IF
  else
  {
    // --> ranged weapon
    ACE_ASSERT(RPG_Item_Common_Tools::isRangedWeapon(weapon_type));

    baseRange_out = properties.rangeIncrement;

    // compute max reach for ranged weapons
    if (RPG_Item_Common_Tools::isThrownWeapon(weapon_type))
      result = baseRange_out * 5;
    else if (RPG_Item_Common_Tools::isProjectileWeapon(weapon_type))
      result = baseRange_out * 10;
    else
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid weapon type (was \"%s\"), continuing\n"),
                 ACE_TEXT(RPG_Item_WeaponTypeHelper::RPG_Item_WeaponTypeToString(weapon_type).c_str())));

      ACE_ASSERT(false);
    } // end IF
  } // end ELSE

  return result;
}

ACE_UINT8
RPG_Player_Player_Base::getSpeed(bool isRunning_in,
                                 enum RPG_Common_AmbientLighting lighting_in,
                                 enum RPG_Common_Terrain terrain_in,
                                 enum RPG_Common_Track track_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Player_Player_Base::getSpeed"));

  // sanity check(s)
  ACE_ASSERT(lighting_in != RPG_COMMON_AMBIENTLIGHTING_INVALID);

  // init return value
  unsigned char result = 0;

  // step1: retrieve base speed (race)
  unsigned char base_speed = 0;
  unsigned int race_index = 1;
  for (unsigned int index = 0;
       index < myRace.size();
       index++, race_index++)
    if (myRace.test(index))
    {
      base_speed =
          RPG_Character_Race_Common_Tools::raceToSpeed(static_cast<RPG_Character_Race>(race_index));
      if (base_speed > result)
        result = base_speed;
    } // end IF
  ACE_ASSERT(base_speed);

  // step2: consider encumbrance (armor / load)
  RPG_Character_Encumbrance encumbrance_by_armor = LOAD_LIGHT;
  RPG_Item_ArmorType armor_type =
      const_cast<RPG_Player_Player_Base*>(this)->getEquipment().getBodyArmor();
  if (armor_type != ARMOR_NONE)
  {
    const RPG_Item_ArmorProperties& properties =
        RPG_ITEM_DICTIONARY_SINGLETON::instance()->getArmorProperties(armor_type);
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
  if (RPG_Character_Race_Common_Tools::hasRace(myRace, RACE_DWARF))
    encumbrance_by_armor = LOAD_LIGHT;
  // *TODO*: consider non-bipeds...
  RPG_Character_Encumbrance encumbrance_by_load =
      RPG_Character_Common_Tools::getEncumbrance(getAttribute(ATTRIBUTE_STRENGTH),
                                                 getSize(),
                                                 getInventory().getTotalWeight(),
                                                 true);
  signed char maxDexModifierAC = std::numeric_limits<signed char>::max();
  signed char armorCheckPenalty = 0;
  unsigned char runModifier = RPG_CHARACTER_DEF_RUN_MODIFIER_MEDIUM;
  RPG_Character_Common_Tools::getLoadModifiers(((encumbrance_by_armor > encumbrance_by_load) ? encumbrance_by_armor
                                                                                             : encumbrance_by_load),
                                               base_speed,
                                               maxDexModifierAC,
                                               armorCheckPenalty,
                                               result,
                                               runModifier);

  float modifier = 1.0F;
  // step3: consider vision [equipment / ambient lighting]
  if ((const_cast<RPG_Player_Player_Base*>(this)->getEquipment().getLightSource() ==
       RPG_ITEM_COMMODITYLIGHT_INVALID) &&
      (lighting_in == AMBIENCE_DARKNESS))
    modifier *= 0.5F;

  // step4: consider terrain [track type]
  modifier *= RPG_Common_Tools::terrainToSpeedModifier(terrain_in, track_in);

  // step5: consider movement mode
  if (isRunning_in)
    modifier *= static_cast<float>(runModifier);

  result = static_cast<unsigned char>(static_cast<float>(result) * modifier);

  // *TODO*: consider other (spell, ...) effects
  return result;
}

bool
RPG_Player_Player_Base::isPlayerCharacter() const
{
  RPG_TRACE(ACE_TEXT("RPG_Player_Player_Base::isPlayerCharacter"));

  return true;
}

bool
RPG_Player_Player_Base::gainExperience (unsigned int XP_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Player_Player_Base::gainExperience"));

  std::vector<unsigned char> levels;
  for (RPG_Character_SubClassesIterator_t iterator = myClass.subClasses.begin ();
       iterator != myClass.subClasses.end ();
       iterator++)
    levels.push_back (getLevel (*iterator));

  myExperience += XP_in;

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("\"%s\" gained %u XP (total: %u)...\n"),
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
                  ACE_TEXT ("player: \"%s\" (XP: %d) has reached level %u as %s...\n"),
                  ACE_TEXT (getName ().c_str ()),
                  myExperience,
                  static_cast<unsigned int> (getLevel (*iterator)),
                  ACE_TEXT (RPG_Common_SubClassHelper::RPG_Common_SubClassToString (*iterator).c_str ())));
      return true;
    } // end IF

  return false;
}

unsigned int
RPG_Player_Player_Base::rest (const RPG_Common_Camp& type_in,
                              unsigned int hours_in)
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
RPG_Player_Player_Base::defaultEquip()
{
  RPG_TRACE(ACE_TEXT("RPG_Player_Player_Base::defaultEquip"));

  // remove everything
  inherited::myEquipment.strip();

  RPG_Character_EquipmentSlots slots;
  RPG_Item_Base* handle = NULL;
  RPG_Item_ID_t item_id = 0;
  for (RPG_Item_ListIterator_t iterator = inherited::myInventory.myItems.begin();
       iterator != inherited::myInventory.myItems.end();
       iterator++)
  {
    slots.slots.clear();
    slots.is_inclusive = false;
    RPG_Item_Common_Tools::itemToSlot (*iterator,
                                       myOffHand,
                                       slots);
    ACE_ASSERT(!slots.slots.empty());

    handle = NULL;
    if (!RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->get(*iterator,
                                                              handle))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid item ID (was: %d), aborting\n"),
                 *iterator));

      return;
    } // end IF
    ACE_ASSERT(handle);

    switch (handle->type())
    {
      case ITEM_ARMOR:
      {
        RPG_Item_Armor* armor = dynamic_cast<RPG_Item_Armor*>(handle);
        ACE_ASSERT(armor);
//         RPG_Item_ArmorProperties properties = RPG_ITEM_DICTIONARY_SINGLETON::instance()->getArmorProperties(armor_base->getArmorType());

        if (myEquipment.isEquipped(slots.slots.front(),
                                   item_id))
          break; // cannot equip...

        myEquipment.equip(*iterator,
                          myOffHand,
                          slots.slots.front());

        break;
      }
      case ITEM_COMMODITY:
      {
        RPG_Item_Commodity* commodity =
            dynamic_cast<RPG_Item_Commodity*>(handle);
        ACE_ASSERT(commodity);
        RPG_Item_CommodityUnion commodity_type = commodity->subtype_;
        //RPG_Item_CommodityProperties properties = RPG_ITEM_DICTIONARY_SINGLETON::instance()->getCommodityProperties(commodity->getCommoditySubType());
        // - by default, equip light sources only
        if (commodity_type.discriminator !=
            RPG_Item_CommodityUnion::COMMODITYLIGHT)
          break;
        //if (myEquipment.isEquipped(slot, item_id))
        //  break; // cannot equip...

        myEquipment.equip(*iterator,
                          myOffHand,
                          slots.slots.front());

        break;
      }
      case ITEM_WEAPON:
      {
        RPG_Item_Weapon* weapon = dynamic_cast<RPG_Item_Weapon*>(handle);
        ACE_ASSERT(weapon);
//         RPG_Item_WeaponProperties properties = RPG_ITEM_DICTIONARY_SINGLETON::instance()->getWeaponProperties(weapon_base->getWeaponType());
        // - by default, equip melee weapons only
        // *TODO*: what about other types of weapons ?
        if (!RPG_Item_Common_Tools::isMeleeWeapon(weapon->weaponType_))
          break;
        if (myEquipment.isEquipped(slots.slots.front(),
                                   item_id))
          break; // cannot equip...

        myEquipment.equip(*iterator,
                          myOffHand,
                          slots.slots.front());

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
RPG_Player_Player_Base::status() const
{
  RPG_TRACE(ACE_TEXT("RPG_Player_Player_Base::status"));

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("name: \"%s\" (XP: %d (%u))\n"),
             ACE_TEXT(getName().c_str()),
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
             ACE_TEXT(getName().c_str()),
             ACE_TEXT(RPG_Character_GenderHelper::RPG_Character_GenderToString(myGender).c_str()),
             ACE_TEXT(RPG_Character_Common_Tools::toString(myRace).c_str()),
             ACE_TEXT(RPG_Character_Common_Tools::toString(myClass).c_str()),
             ACE_TEXT(RPG_Character_Common_Tools::toString(getAlignment()).c_str()),
             ACE_TEXT(RPG_Character_Common_Tools::toString(myCondition).c_str()),
             myNumHitPoints,
             getNumTotalHitPoints(),
             myExperience,
             myWealth,
             ACE_TEXT(RPG_Character_Common_Tools::toString(myAttributes).c_str()),
             ACE_TEXT(RPG_Character_Skills_Common_Tools::toString(myFeats).c_str()),
             ACE_TEXT(RPG_Character_Skills_Common_Tools::toString(myAbilities).c_str()),
             ACE_TEXT(RPG_Character_Skills_Common_Tools::toString(mySkills).c_str()),
             ACE_TEXT(RPG_Magic_Common_Tools::toString(myKnownSpells).c_str()),
             ACE_TEXT(RPG_Magic_Common_Tools::toString(mySpells).c_str())));
}

ACE_INT8
RPG_Player_Player_Base::getShieldBonus() const
{
  RPG_TRACE(ACE_TEXT("RPG_Player_Player_Base::getShieldBonus"));

  // retrieve equipped armor type
  RPG_Item_ArmorType type = myEquipment.getShield(myOffHand);
  if (type == ARMOR_NONE)
    return 0;

  const RPG_Item_ArmorProperties& properties =
      RPG_ITEM_DICTIONARY_SINGLETON::instance()->getArmorProperties(type);
  return properties.baseBonus;
}
