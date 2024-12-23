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

#include "rpg_monster.h"

#include "ace/Log_Msg.h"

#include "rpg_common_macros.h"
#include "rpg_common_tools.h"

#include "rpg_item_common.h"
#include "rpg_item_dictionary.h"

#include "rpg_character_defines.h"
#include "rpg_character_common_tools.h"
#include "rpg_character_race_common_tools.h"

#include "rpg_monster_common.h"
#include "rpg_monster_dictionary.h"

RPG_Monster::RPG_Monster (// base attributes
                          const std::string& name_in,
                          const struct RPG_Common_CreatureType& type_in,
                          const struct RPG_Character_Alignment& alignment_in,
                          const struct RPG_Character_Attributes& attributes_in,
                          const RPG_Character_Skills_t& skills_in,
                          const RPG_Character_Feats_t& feats_in,
                          const RPG_Character_Abilities_t& abilities_in,
                          const struct RPG_Monster_Size& defaultSize_in,
                          ACE_UINT16 maxHitPoints_in,
                          const RPG_Magic_SpellTypes_t& knownSpells_in,
                          // extended data
                          ACE_UINT64 wealth_in,
                          const RPG_Magic_Spells_t& spells_in,
                          const RPG_Item_List_t& inventory_in,
                          // current status
                          const RPG_Character_Conditions_t& condition_in,
                          ACE_INT16 hitpoints_in,
                          // ...more extended data
                          bool isSummoned_in)
 : inherited (// base attributes
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
              inventory_in),
   myType (type_in),
   mySize (defaultSize_in),
   myIsSummoned (isSummoned_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Monster::RPG_Monster"));

}

RPG_Monster::RPG_Monster (const RPG_Monster& monster_in)
 : inherited (monster_in),
   myType (monster_in.myType),
   mySize (monster_in.mySize),
   myIsSummoned (monster_in.myIsSummoned)
{
  RPG_TRACE (ACE_TEXT ("RPG_Monster::RPG_Monster"));

}

// RPG_Monster&
// RPG_Monster::operator=(const RPG_Monster& monster_in)
// {
//   RPG_TRACE(ACE_TEXT("RPG_Monster::operator="));
//
//   myType = monster_in.myType;
//   mySize = monster_in.mySize;
//   myIsSummoned = monster_in.myIsSummoned;
//   inherited::operator=(monster_in);
//
//   return *this;
// }

ACE_INT8
RPG_Monster::getArmorClass (enum RPG_Combat_DefenseSituation defenseSituation_in) const
{
  RPG_TRACE (ACE_TEXT ("RPG_Monster::getArmorClass"));

  // AC = 10 + (natural) armor bonus (+ shield bonus) + DEX modifier + size modifier [+ other modifiers]
  ACE_INT8 result = 0;

  // *TODO*: consider any (additional, equipped) armor...
  const RPG_Monster_Properties& properties =
    RPG_MONSTER_DICTIONARY_SINGLETON::instance ()->getProperties (getName ());
  switch (defenseSituation_in)
  {
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid defense situation (\"%s\"): returning value for \"%s\"...\n"),
                  ACE_TEXT (RPG_Combat_DefenseSituationHelper::RPG_Combat_DefenseSituationToString (defenseSituation_in).c_str ()),
                  ACE_TEXT (RPG_Combat_DefenseSituationHelper::RPG_Combat_DefenseSituationToString (DEFENSE_NORMAL).c_str ())));
    }
    case DEFENSE_NORMAL:
      result = properties.armorClass.normal; break;
    case DEFENSE_TOUCH:
      result = properties.armorClass.touch; break;
    case DEFENSE_FLATFOOTED:
      result = properties.armorClass.flatFooted; break;
  } // end SWITCH
//   result += getShieldBonus();

  return result;
}

ACE_UINT16
RPG_Monster::getReach (ACE_UINT16& baseRange_out,
                       bool& reachIsAbsolute_out) const
{
  RPG_TRACE (ACE_TEXT ("RPG_Monster::getReach"));

  // init return value(s)
  baseRange_out = 0;
  reachIsAbsolute_out = false;

  // step1: retrieve base speed (type)
  const RPG_Monster_Properties& properties =
    RPG_MONSTER_DICTIONARY_SINGLETON::instance ()->getProperties (getName ());
  ACE_UINT16 result = properties.reach;

  //// *TODO*: consider polymorphed states...
  //unsigned short result = RPG_Common_Tools::sizeToReach(mySize, true);

  //RPG_Item_WeaponType weapon_type = myEquipment.getPrimaryWeapon(myOffHand);
  //const RPG_Item_WeaponProperties& properties = RPG_ITEM_DICTIONARY_SINGLETON::instance()->getWeaponProperties(weapon_type);
  //if (RPG_Item_Common_Tools::isMeleeWeapon(weapon_type))
  //{
  //  if (properties.isReachWeapon)
  //  {
  //    result *= 2;
  //    reachIsAbsolute_out = RPG_Item_Common_Tools::hasAbsoluteReach(weapon_type);
  //  } // end IF
  //} // end IF
  //else
  //{
  //  // --> ranged weapon
  //  ACE_ASSERT(RPG_Item_Common_Tools::isRangedWeapon(weapon_type));

  //  baseRange_out = properties.rangeIncrement;

  //  // compute max reach for ranged weapons
  //  if (RPG_Item_Common_Tools::isThrownWeapon(weapon_type))
  //    result = baseRange_out * 5;
  //  else if (RPG_Item_Common_Tools::isProjectileWeapon(weapon_type))
  //    result = baseRange_out * 10;
  //  else
  //  {
  //    ACE_DEBUG((LM_ERROR,
  //               ACE_TEXT("invalid weapon type (was \"%s\"), continuing\n"),
  //               RPG_Item_WeaponTypeHelper::RPG_Item_WeaponTypeToString(weapon_type).c_str()));

  //    ACE_ASSERT(false);
  //  } // end IF
  //} // end ELSE

  return result;
}

ACE_UINT8
RPG_Monster::getSpeed (bool isRunning_in,
                       enum RPG_Common_AmbientLighting lighting_in,
                       enum RPG_Common_Terrain terrain_in,
                       enum RPG_Common_Track track_in) const
{
  RPG_TRACE (ACE_TEXT ("RPG_Monster::getSpeed"));

  // sanity check(s)
  ACE_ASSERT (lighting_in != RPG_COMMON_AMBIENTLIGHTING_INVALID);

  // init return value
  ACE_UINT8 result = 0;

  // step1: retrieve base speed (type)
  const RPG_Monster_Properties& properties =
    RPG_MONSTER_DICTIONARY_SINGLETON::instance ()->getProperties (getName ());
  result = properties.speed;

  // step2: consider encumbrance (armor / load)
  RPG_Character_Encumbrance encumbrance_by_armor = LOAD_LIGHT;
  const RPG_Item_ArmorType& armor_type = inherited::myEquipment.getBodyArmor ();
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
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid (body) armor category (was \"%s\"), aborting\n"),
                    ACE_TEXT (RPG_Item_ArmorCategoryHelper::RPG_Item_ArmorCategoryToString (properties.category).c_str ())));
        return 0;
      }
    } // end SWITCH
  } // end IF
  // *TODO*: consider race (if any)
  //// *NOTE*: dwarves move at the base speed with any armor...
  //if (RPG_Character_Race_Common_Tools::hasRace(myRace, RACE_DWARF))
  //  encumbrance_by_armor = LOAD_LIGHT;

  // *TODO*: consider non-bipeds...
  RPG_Character_Encumbrance encumbrance_by_load =
    RPG_Character_Common_Tools::getEncumbrance (getAttribute (ATTRIBUTE_STRENGTH),
                                                getSize ().size,
                                                getInventory ().getTotalWeight (),
                                                true);
  signed char maxDexModifierAC = std::numeric_limits<signed char>::max ();
  signed char armorCheckPenalty = 0;
  unsigned char runModifier = RPG_CHARACTER_RUN_MODIFIER_MEDIUM;
  RPG_Character_Common_Tools::getLoadModifiers (std::max (encumbrance_by_armor, encumbrance_by_load),
                                                properties.speed,
                                                maxDexModifierAC,
                                                armorCheckPenalty,
                                                result,
                                                runModifier);

  float modifier = 1.0f;
  // step3: consider vision [equipment / ambient lighting]
  if ((inherited::myEquipment.getLightSource () == RPG_ITEM_COMMODITYLIGHT_INVALID) &&
      (lighting_in == AMBIENCE_DARKNESS))
    modifier *= 0.5f;

  // step4: consider terrain [track type]
  modifier *= RPG_Common_Tools::terrainToSpeedModifier (terrain_in,
                                                        track_in);

  // step5: consider movement mode
  if (isRunning_in)
    modifier *= static_cast<float> (runModifier);

  result = static_cast<ACE_UINT8> (result * modifier);

  // *TODO*: consider other (spell, ...) effects
  return result;
}

enum RPG_Common_SubClass
RPG_Monster::gainExperience (ACE_UINT64 XP_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Monster::gainExperience"));

  // *TODO*
  ACE_ASSERT (false);

  return RPG_COMMON_SUBCLASS_INVALID;
}

void
RPG_Monster::dump () const
{
  RPG_TRACE (ACE_TEXT ("RPG_Monster::dump"));

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("Monster (type: \"%s\")\n"),
//              RPG_Monster_Common_Tools::typeToString(myType).c_str()));

  inherited::dump ();
}

RPG_Character_BaseAttackBonus_t
RPG_Monster::getAttackBonus (enum RPG_Common_Attribute modifier_in,
                             enum RPG_Combat_AttackSituation situation_in) const
{
  RPG_TRACE (ACE_TEXT ("RPG_Monster::getAttackBonus"));

  // sanity check(s)
  ACE_ASSERT ((modifier_in == ATTRIBUTE_DEXTERITY) || (modifier_in == ATTRIBUTE_STRENGTH));

  ACE_ASSERT (false); // *TODO*

  RPG_Character_BaseAttackBonus_t result;
#if defined (_MSC_VER)
  return result;
#else
  ACE_NOTREACHED (return result;)
#endif // _MSC_VER
}

ACE_INT8
RPG_Monster::getShieldBonus () const
{
  RPG_TRACE (ACE_TEXT ("RPG_Monster::getShieldBonus"));

  ACE_ASSERT (false); // *TODO*

  ACE_INT8 result = 0;
#if defined (_MSC_VER)
  return result;
#else
  ACE_NOTREACHED (return result;)
#endif // _MSC_VER
}

struct RPG_Dice_Roll
RPG_Monster::getHitDicePerLevel (enum RPG_Common_SubClass subClass_in) const
{
  RPG_TRACE (ACE_TEXT ("RPG_Monster::getHitDicePerLevel"));

  ACE_UNUSED_ARG (subClass_in);

  // step1: retrieve base hit dice (type)
  const RPG_Monster_Properties& properties =
      RPG_MONSTER_DICTIONARY_SINGLETON::instance()->getProperties (getName ());

  ACE_ASSERT (false); // *TODO*
  return properties.hitDice;
}

ACE_UINT8
RPG_Monster::getFeatsPerLevel (enum RPG_Common_SubClass subClass_in) const
{
  RPG_TRACE (ACE_TEXT ("RPG_Monster::getFeatsPerLevel"));

  ACE_UNUSED_ARG (subClass_in);

  ACE_UINT8 result = 0;

  ACE_ASSERT (false); // *TODO*
  return result;
}

ACE_UINT8
RPG_Monster::getSkillsPerLevel (enum RPG_Common_SubClass subClass_in) const
{
  RPG_TRACE (ACE_TEXT ("RPG_Monster::getSkillsPerLevel"));

  ACE_UNUSED_ARG (subClass_in);

  ACE_UINT8 result = 0;

  ACE_ASSERT (false); // *TODO*
  return result;
}

ACE_UINT16
RPG_Monster::getKnownSpellsPerLevel (enum RPG_Common_SubClass subClass_in,
                                     ACE_UINT8 spellLevel_in) const
{
  ACE_UNUSED_ARG (subClass_in);
  ACE_UNUSED_ARG (spellLevel_in);

  ACE_UINT16 result = 0;

  ACE_ASSERT (false); // *TODO*
  return result;
}
