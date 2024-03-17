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

#include "rpg_player_common_tools.h"

#include <numeric>

#include "ace/Log_Msg.h"
#include "ace/OS.h"

#include "common_file_tools.h"

#if defined (HAVE_CONFIG_H)
#include "rpg_config.h"
#endif // HAVE_CONFIG_H

#include "rpg_dice.h"
#include "rpg_dice_common.h"

#include "rpg_common_defines.h"
#include "rpg_common_file_tools.h"
#include "rpg_common_macros.h"
#include "rpg_common_tools.h"

#include "rpg_magic_common_tools.h"
#include "rpg_magic_defines.h"

#include "rpg_item_armor.h"
#include "rpg_item_commodity.h"
#include "rpg_item_common_tools.h"
#include "rpg_item_common_XML_tools.h"
#include "rpg_item_dictionary.h"
#include "rpg_item_instance_manager.h"
#include "rpg_item_weapon.h"

#include "rpg_character_class_common_tools.h"
#include "rpg_character_common_tools.h"
#include "rpg_character_skills_common_tools.h"
#include "rpg_character_XML_tools.h"

#include "rpg_player.h"
#include "rpg_player_defines.h"

#include "rpg_engine_defines.h"

RPG_Character_Race_t
RPG_Player_Common_Tools::raceXMLTreeToRace (const RPG_Player_CharacterXML_XMLTree_Type::race_sequence& races_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Player_Common_Tools::raceXMLTreeToRace"));

  RPG_Character_Race_t result;

  RPG_Character_Race current;
  for (RPG_Player_PlayerXML_XMLTree_Type::race_const_iterator iterator = races_in.begin();
       iterator != races_in.end();
       iterator++)
  {
    current = RPG_Character_RaceHelper::stringToRPG_Character_Race(*iterator);
    if (current > RACE_NONE)
      result.set(current - 1); // *NOTE*: -1 !
  } // end FOR

  return result;
}

RPG_Character_Abilities_t
RPG_Player_Common_Tools::abilitiesXMLTreeToAbilities (const RPG_Player_Abilities_XMLTree_Type& abilities_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Player_Common_Tools::abilitiesXMLTreeToAbilities"));

  // init result
  RPG_Character_Abilities_t result;

  for (RPG_Player_Abilities_XMLTree_Type::ability_const_iterator iterator = abilities_in.ability().begin();
       iterator != abilities_in.ability().end();
       iterator++)
    result.insert(RPG_Character_AbilityHelper::stringToRPG_Character_Ability(*iterator));

  return result;
}

RPG_Magic_SpellTypes_t
RPG_Player_Common_Tools::spellsXMLTreeToSpellTypes (const RPG_Player_Spells_XMLTree_Type& spells_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Player_Common_Tools::spellsXMLTreeToSpellTypes"));

  RPG_Magic_SpellTypes_t result;

  for (RPG_Player_Spells_XMLTree_Type::spell_const_iterator iterator = spells_in.spell().begin();
       iterator != spells_in.spell().end();
       iterator++)
    result.insert(RPG_Magic_SpellTypeHelper::stringToRPG_Magic_SpellType(*iterator));

  return result;
}

RPG_Character_Conditions_t
RPG_Player_Common_Tools::conditionsXMLTreeToConditions (const RPG_Player_Conditions_XMLTree_Type& conditions_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Player_Common_Tools::conditionsXMLTreeToConditions"));

  RPG_Character_Conditions_t result;

  for (RPG_Player_Conditions_XMLTree_Type::condition_const_iterator iterator = conditions_in.condition().begin();
       iterator != conditions_in.condition().end();
       iterator++)
    result.insert(RPG_Common_ConditionHelper::stringToRPG_Common_Condition(*iterator));

  return result;
}

RPG_Magic_Spells_t
RPG_Player_Common_Tools::spellsXMLTreeToSpells (const RPG_Player_Spells_XMLTree_Type& spells_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Player_Common_Tools::spellsXMLTreeToSpells"));

  RPG_Magic_Spells_t result;

  for (RPG_Player_Spells_XMLTree_Type::spell_const_iterator iterator = spells_in.spell().begin();
       iterator != spells_in.spell().end();
       iterator++)
    result.push_back(RPG_Magic_SpellTypeHelper::stringToRPG_Magic_SpellType(*iterator));

  return result;
}

unsigned int
RPG_Player_Common_Tools::restParty (RPG_Player_Party_t& party_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Player_Common_Tools::restParty"));

  // debug info
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("party status:\n-------------\n")));
  int index = 1;
  RPG_Player_PartyIterator_t iterator;
  for (iterator = party_in.begin();
       iterator != party_in.end();
       iterator++, index++)
  {
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("#%d] \"%s\" (lvl: %d), HP: %d/%d --> %s\n"),
               index,
               ACE_TEXT((*iterator)->getName().c_str()),
               (*iterator)->getLevel(),
               (*iterator)->getNumHitPoints(),
               (*iterator)->getNumTotalHitPoints(),
               ((*iterator)->hasCondition(CONDITION_NORMAL) ? ACE_TEXT_ALWAYS_CHAR("OK") : ACE_TEXT_ALWAYS_CHAR("DOWN"))));
  } // end FOR

  // check party status
  unsigned int diff = 0;
  unsigned int fraction = 0;
  unsigned int recoveryTime = 0;
  unsigned int maxRecoveryTime = 0;
  for (iterator = party_in.begin();
       iterator != party_in.end();
       iterator++)
  {
    // *TODO*: consider dead/dying players !
    if ((*iterator)->getNumHitPoints() < 0)
      continue;

    diff = ((*iterator)->getNumTotalHitPoints() -
            (*iterator)->getNumHitPoints());
    fraction = (diff % ((*iterator)->getLevel() * 2));
    diff -= fraction;
    recoveryTime = ((diff / ((*iterator)->getLevel() * 2)) + // days of complete bed-rest +
                    (fraction / (*iterator)->getLevel()));   // days of good night's sleep
    if (recoveryTime > maxRecoveryTime)
      maxRecoveryTime = recoveryTime;
  } // end FOR

  // debug info
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("max. recovery time: %d days\n"),
              maxRecoveryTime));

  for (iterator = party_in.begin ();
       iterator != party_in.end ();
       iterator++)
  {
    (*iterator)->rest (REST_FULL,
                       (maxRecoveryTime * 24));
  } // end FOR

  return (maxRecoveryTime * 24 * 3600);
}

RPG_Player*
RPG_Player_Common_Tools::playerXMLToPlayer (const RPG_Player_PlayerXML_XMLTree_Type& player_in,
                                            // current status
                                            const RPG_Character_Conditions_t& condition_in,
                                            ACE_INT16 HP_in,
                                            const RPG_Magic_Spells_t& spells_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Player_Common_Tools::playerXMLToPlayer"));

  RPG_Character_Alignment alignment;
  alignment.civic =
      RPG_Character_AlignmentCivicHelper::stringToRPG_Character_AlignmentCivic (player_in.alignment ().civic ());
  alignment.ethic =
      RPG_Character_AlignmentEthicHelper::stringToRPG_Character_AlignmentEthic (player_in.alignment ().ethic ());
  RPG_Character_Attributes attributes;
  attributes.strength = player_in.attributes ().strength ();
  attributes.dexterity = player_in.attributes ().dexterity ();
  attributes.constitution = player_in.attributes ().constitution ();
  attributes.intelligence = player_in.attributes ().intelligence ();
  attributes.wisdom = player_in.attributes ().wisdom ();
  attributes.charisma = player_in.attributes ().charisma ();
  RPG_Character_Skills_t skills;
  if (player_in.skills ().present ())
    skills =
        RPG_Character_XML_Tools::skillsXMLTreeToSkills (player_in.skills ().get ());
  RPG_Character_Feats_t feats;
  if (player_in.feats ().present ())
    feats =
        RPG_Character_XML_Tools::featsXMLTreeToFeats (player_in.feats ().get ());
  RPG_Character_Abilities_t abilities;
  if (player_in.abilities ().present ())
    abilities =
        RPG_Player_Common_Tools::abilitiesXMLTreeToAbilities (player_in.abilities ().get ());
  RPG_Magic_SpellTypes_t known_spells;
  if (player_in.knownSpells ().present ())
    known_spells =
        RPG_Player_Common_Tools::spellsXMLTreeToSpellTypes (player_in.knownSpells ().get ());

  RPG_Character_Conditions_t condition = condition_in;
  if (condition.empty ())
    condition.insert (CONDITION_NORMAL);
  RPG_Player* player_p = NULL;
  ACE_NEW_NORETURN (player_p,
                    RPG_Player (// base attributes
                                player_in.name(),
                                RPG_Character_GenderHelper::stringToRPG_Character_Gender (player_in.gender ()),
                                RPG_Player_Common_Tools::raceXMLTreeToRace (const_cast<RPG_Player_PlayerXML_XMLTree_Type&> (player_in).race ()),
                                RPG_Character_Class_Common_Tools::classXMLTreeToClass (player_in.classXML ()),
                                alignment,
                                attributes,
                                skills,
                                feats,
                                abilities,
                                RPG_Character_OffHandHelper::stringToRPG_Character_OffHand (player_in.offhand ()),
                                player_in.maxHP (),
                                known_spells,
                                // extended data
                                player_in.XP (),
                                player_in.gold (),
                                RPG_Item_Common_XML_Tools::itemXMLTreeToItems (player_in.inventory ()),
                                // current status
                                condition,
                                ((HP_in == std::numeric_limits<ACE_INT16>::max ()) ? player_in.maxHP ()
                                                                                   : HP_in),
                                spells_in));
  if (!player_p)
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory(%u), aborting\n"),
                sizeof (RPG_Player)));
    return NULL;
  } // end IF

  return player_p;
}

RPG_Player_PlayerXML_XMLTree_Type*
RPG_Player_Common_Tools::playerToPlayerXML (const RPG_Player& player_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Player_Common_Tools::playerToPlayerXML"));

  RPG_Character_Alignment_XMLTree_Type alignment (RPG_Character_AlignmentCivicHelper::RPG_Character_AlignmentCivicToString (player_in.getAlignment ().civic),
  RPG_Character_AlignmentEthicHelper::RPG_Character_AlignmentEthicToString (player_in.getAlignment ().ethic));

  RPG_Character_Attributes_XMLTree_Type attributes (player_in.getAttribute (ATTRIBUTE_STRENGTH),
                                                    player_in.getAttribute (ATTRIBUTE_DEXTERITY),
                                                    player_in.getAttribute (ATTRIBUTE_CONSTITUTION),
                                                    player_in.getAttribute (ATTRIBUTE_INTELLIGENCE),
                                                    player_in.getAttribute (ATTRIBUTE_WISDOM),
                                                    player_in.getAttribute (ATTRIBUTE_CHARISMA));

  RPG_Player_Conditions_XMLTree_Type conditions;
  RPG_Character_Conditions_t character_condition = player_in.getCondition ();
  for (RPG_Character_ConditionsIterator_t iterator = character_condition.begin ();
       iterator != character_condition.end ();
       iterator++)
    conditions.condition ().push_back (RPG_Common_ConditionHelper::RPG_Common_ConditionToString (*iterator));

  RPG_Item_InventoryXML_XMLTree_Type inventory;
  RPG_Player_Inventory character_inventory = player_in.getInventory ();
  RPG_Item_Base* item_base = NULL;
  for (RPG_Item_ListIterator_t iterator = character_inventory.myItems.begin ();
       iterator != character_inventory.myItems.end ();
       iterator++)
  {
    // retrieve item details
    RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance ()->get (*iterator,
                                                           item_base);
    ACE_ASSERT (item_base);
    RPG_Item_BaseXML_XMLTree_Type item (RPG_Item_TypeHelper::RPG_Item_TypeToString (item_base->type ()));
    switch (item_base->type ())
    {
      case ITEM_ARMOR:
      {
        RPG_Item_Armor* armor = static_cast<RPG_Item_Armor*> (item_base);

        RPG_Item_ArmorProperties armor_properties =
            RPG_ITEM_DICTIONARY_SINGLETON::instance ()->getArmorProperties (armor->armorType ());

        RPG_Item_StorePrice_XMLTree_Type store_price;
        if (armor_properties.baseStorePrice.numGoldPieces)
          store_price.numGoldPieces (armor_properties.baseStorePrice.numGoldPieces);
        if (armor_properties.baseStorePrice.numSilverPieces)
          store_price.numSilverPieces (armor_properties.baseStorePrice.numSilverPieces);
        RPG_Item_ArmorPropertiesXML_XMLTree_Type armor_properties_xml (armor_properties.baseWeight,
                                                                       store_price,
                                                                       RPG_Item_ArmorTypeHelper::RPG_Item_ArmorTypeToString (armor->armorType ()),
                                                                       RPG_Item_ArmorCategoryHelper::RPG_Item_ArmorCategoryToString (armor_properties.category),
                                                                       armor_properties.baseBonus,
                                                                       armor_properties.maxDexterityBonus,
                                                                       armor_properties.checkPenalty,
                                                                       armor_properties.arcaneSpellFailure,
                                                                       armor_properties.baseSpeed);
        if (armor_properties.defenseModifier)
          armor_properties_xml.defenseModifier (armor_properties.defenseModifier);
        if (armor_properties.aura != RPG_MAGIC_SCHOOL_INVALID)
          armor_properties_xml.aura (RPG_Magic_SchoolHelper::RPG_Magic_SchoolToString (armor_properties.aura));
        if (armor_properties.prerequisites.minCasterLevel)
        {
          RPG_Item_MagicalPrerequisites_XMLTree_Type magical_prerequisites;
          magical_prerequisites.minCasterLevel (armor_properties.prerequisites.minCasterLevel);
          armor_properties_xml.prerequisites (magical_prerequisites);
        } // end IF
        if (armor_properties.costToCreate.numGoldPieces ||
            armor_properties.costToCreate.numExperiencePoints)
        {
          RPG_Item_CreationCost_XMLTree_Type costs_to_create;
          if (armor_properties.costToCreate.numGoldPieces)
            costs_to_create.numGoldPieces (armor_properties.costToCreate.numGoldPieces);
          if (armor_properties.costToCreate.numExperiencePoints)
            costs_to_create.numExperiencePoints (armor_properties.costToCreate.numExperiencePoints);
          armor_properties_xml.costToCreate (costs_to_create);
        } // end IF

        item.armor (armor_properties_xml);
        inventory.item ().push_back (item);

        break;
      }
      case ITEM_COMMODITY:
      {
        RPG_Item_Commodity* commodity = static_cast<RPG_Item_Commodity*>(item_base);

        RPG_Item_CommodityProperties commodity_properties =
            RPG_ITEM_DICTIONARY_SINGLETON::instance ()->getCommodityProperties (commodity->subtype ());

        RPG_Item_StorePrice_XMLTree_Type store_price;
        if (commodity_properties.baseStorePrice.numGoldPieces)
          store_price.numGoldPieces (commodity_properties.baseStorePrice.numGoldPieces);
        if (commodity_properties.baseStorePrice.numSilverPieces)
          store_price.numSilverPieces (commodity_properties.baseStorePrice.numSilverPieces);
        RPG_Item_CommodityPropertiesBase_XMLTree_Type commodity_properties_xml (commodity_properties.baseWeight,
                                                                                store_price,
                                                                                RPG_Item_CommodityTypeHelper::RPG_Item_CommodityTypeToString (commodity->commodityType ()),
                                                                                RPG_Item_Common_Tools::commoditySubTypeToXMLString (commodity->subtype ()));
        if (commodity_properties.aura != RPG_MAGIC_SCHOOL_INVALID)
          commodity_properties_xml.aura (RPG_Magic_SchoolHelper::RPG_Magic_SchoolToString (commodity_properties.aura));
        if (commodity_properties.prerequisites.minCasterLevel)
        {
          RPG_Item_MagicalPrerequisites_XMLTree_Type magical_prerequisites;
          magical_prerequisites.minCasterLevel (commodity_properties.prerequisites.minCasterLevel);
          commodity_properties_xml.prerequisites (magical_prerequisites);
        } // end IF
        if (commodity_properties.costToCreate.numGoldPieces ||
            commodity_properties.costToCreate.numExperiencePoints)
        {
          RPG_Item_CreationCost_XMLTree_Type costs_to_create;
          if (commodity_properties.costToCreate.numGoldPieces)
            costs_to_create.numGoldPieces (commodity_properties.costToCreate.numGoldPieces);
          if (commodity_properties.costToCreate.numExperiencePoints)
            costs_to_create.numExperiencePoints (commodity_properties.costToCreate.numExperiencePoints);
          commodity_properties_xml.costToCreate (costs_to_create);
        } // end IF

        item.commodity (commodity_properties_xml);
        inventory.item ().push_back (item);

        break;
      }
      case ITEM_OTHER:
      case ITEM_VALUABLE:
      {
        // *TODO*
        ACE_ASSERT(false);

        break;
      }
      case ITEM_WEAPON:
      {
        RPG_Item_Weapon* weapon = static_cast<RPG_Item_Weapon*>(item_base);

        RPG_Item_WeaponProperties weapon_properties =
            RPG_ITEM_DICTIONARY_SINGLETON::instance ()->getWeaponProperties (weapon->weaponType_);

        RPG_Item_BaseXML_XMLTree_Type item(RPG_Item_TypeHelper::RPG_Item_TypeToString (item_base->type ()));
        RPG_Item_StorePrice_XMLTree_Type store_price;
        if (weapon_properties.baseStorePrice.numGoldPieces)
          store_price.numGoldPieces (weapon_properties.baseStorePrice.numGoldPieces);
        if (weapon_properties.baseStorePrice.numSilverPieces)
          store_price.numSilverPieces (weapon_properties.baseStorePrice.numSilverPieces);
        RPG_Dice_Roll_XMLTree_Type base_damage (RPG_Dice_DieTypeHelper::RPG_Dice_DieTypeToString (weapon_properties.baseDamage.typeDice));
        if (weapon_properties.baseDamage.numDice)
          base_damage.numDice (weapon_properties.baseDamage.numDice);
        if (weapon_properties.baseDamage.modifier)
          base_damage.modifier (weapon_properties.baseDamage.modifier);
        RPG_Item_CriticalHitProperties_XMLTree_Type critical_hit_properties (weapon_properties.criticalHit.minToHitRoll,
                                                                             weapon_properties.criticalHit.damageModifier);
        RPG_Item_WeaponPropertiesXML_XMLTree_Type weapon_properties_xml (weapon_properties.baseWeight,
                                                                         store_price,
                                                                         RPG_Item_WeaponTypeHelper::RPG_Item_WeaponTypeToString (weapon->weaponType_),
                                                                         RPG_Item_WeaponCategoryHelper::RPG_Item_WeaponCategoryToString (weapon_properties.category),
                                                                         RPG_Item_WeaponClassHelper::RPG_Item_WeaponClassToString (weapon_properties.weaponClass),
                                                                         base_damage,
                                                                         critical_hit_properties);
        if (weapon_properties.toHitModifier)
          weapon_properties_xml.toHitModifier (weapon_properties.toHitModifier);
        if (weapon_properties.rangeIncrement)
          weapon_properties_xml.rangeIncrement (weapon_properties.rangeIncrement);
        RPG_Item_WeaponPropertiesBase_XMLTree_Type::typeOfDamage_sequence type_of_damage;
        int index = PHYSICALDAMAGE_NONE; index++;
        for (unsigned int i = 0;
             i < weapon_properties.typeOfDamage.size();
             i++, index++)
         if (weapon_properties.typeOfDamage.test (i))
           type_of_damage.push_back (RPG_Common_PhysicalDamageTypeHelper::RPG_Common_PhysicalDamageTypeToString (static_cast<RPG_Common_PhysicalDamageType> (index)));
        weapon_properties_xml.typeOfDamage (type_of_damage);
        weapon_properties_xml.isNonLethal (weapon_properties.isNonLethal);
        weapon_properties_xml.isReachWeapon (weapon_properties.isReachWeapon);
        weapon_properties_xml.isDoubleWeapon (weapon_properties.isDoubleWeapon);
        if (weapon_properties.aura != RPG_MAGIC_SCHOOL_INVALID)
          weapon_properties_xml.aura (RPG_Magic_SchoolHelper::RPG_Magic_SchoolToString (weapon_properties.aura));
        if (weapon_properties.prerequisites.minCasterLevel)
        {
          RPG_Item_MagicalPrerequisites_XMLTree_Type magical_prerequisites;
          magical_prerequisites.minCasterLevel (weapon_properties.prerequisites.minCasterLevel);
          weapon_properties_xml.prerequisites (magical_prerequisites);
        } // end IF
        if (weapon_properties.costToCreate.numGoldPieces ||
            weapon_properties.costToCreate.numExperiencePoints)
        {
          RPG_Item_CreationCost_XMLTree_Type costs_to_create;
          if (weapon_properties.costToCreate.numGoldPieces)
            costs_to_create.numGoldPieces (weapon_properties.costToCreate.numGoldPieces);
          if (weapon_properties.costToCreate.numExperiencePoints)
            costs_to_create.numExperiencePoints (weapon_properties.costToCreate.numExperiencePoints);
          weapon_properties_xml.costToCreate (costs_to_create);
        } // end IF

        item.weapon (weapon_properties_xml);
        inventory.item ().push_back (item);

        break;
      }
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid item type (was: \"%s\"), aborting\n"),
                    ACE_TEXT (RPG_Item_TypeHelper::RPG_Item_TypeToString (item_base->type ()).c_str ())));
        return NULL;
      }
    } // end SWITCH
  } // end FOR

  RPG_Character_Class character_class = player_in.getClass ();
  RPG_Character_ClassXML_XMLTree_Type classXML (RPG_Character_MetaClass_XMLTree_Type (static_cast<RPG_Character_MetaClass_XMLTree_Type::value> (character_class.metaClass)));
  for (RPG_Character_SubClassesIterator_t iterator = character_class.subClasses.begin ();
       iterator != character_class.subClasses.end ();
       iterator++)
    classXML.subClass ().push_back (RPG_Common_SubClass_XMLTree_Type (static_cast<RPG_Common_SubClass_XMLTree_Type::value> (*iterator)));

  RPG_Player_PlayerXML_XMLTree_Type* player_p = NULL;
  ACE_NEW_NORETURN (player_p,
                    RPG_Player_PlayerXML_XMLTree_Type (player_in.getName (),
                                                       alignment,
                                                       attributes,
                                                       RPG_Common_Size_XMLTree_Type (static_cast<RPG_Common_Size_XMLTree_Type::value> (player_in.getSize ())),
                                                       player_in.getNumTotalHitPoints (),
                                                       classXML,
                                                       RPG_Character_Gender_XMLTree_Type (static_cast<RPG_Character_Gender_XMLTree_Type::value> (player_in.getGender ())),
                                                       RPG_Character_OffHand_XMLTree_Type (static_cast<RPG_Character_OffHand_XMLTree_Type::value> (player_in.getOffHand ())),
                                                       player_in.getWealth (),
                                                       inventory,
                                                       player_in.getExperience ()));
  if (!player_p)
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory(%u), aborting\n"),
                sizeof (RPG_Player_PlayerXML_XMLTree_Type)));
    return NULL;
  } // end IF
  ACE_ASSERT (player_p);

  // *NOTE*: add race, skills, feats, abilities, known spells, prepared spells sequences "manually"
  RPG_Character_Race_t character_race = player_in.getRace ();
  int index = RACE_NONE; index++;
  for (unsigned int i = 0;
       i < character_race.size ();
       i++, index++)
    if (character_race.test (i))
      player_p->race ().push_back (RPG_Character_RaceHelper::RPG_Character_RaceToString (static_cast<RPG_Character_Race> (index)));

  RPG_Character_Skills_XMLTree_Type skills;
  RPG_Character_Skills_t character_skills = player_in.getSkills ();
  for (RPG_Character_SkillsConstIterator_t iterator = character_skills.begin ();
       iterator != character_skills.end ();
       iterator++)
  {
    RPG_Character_SkillValue_XMLTree_Type skill (RPG_Common_SkillHelper::RPG_Common_SkillToString ((*iterator).first),
                                                 (*iterator).second);
    skills.skill ().push_back (skill);
  } // end FOR
  player_p->skills ().set (skills);

  RPG_Character_Feats_XMLTree_Type feats;
  RPG_Character_Feats_t character_feats = player_in.getFeats ();
  for (RPG_Character_FeatsConstIterator_t iterator = character_feats.begin ();
       iterator != character_feats.end ();
       iterator++)
    feats.feat ().push_back (RPG_Character_FeatHelper::RPG_Character_FeatToString (*iterator));
  player_p->feats ().set (feats);

  RPG_Player_Abilities_XMLTree_Type abilities;
  RPG_Character_Abilities_t character_abilities = player_in.getAbilities ();
  for (RPG_Character_AbilitiesConstIterator_t iterator = character_abilities.begin ();
       iterator != character_abilities.end ();
       iterator++)
    abilities.ability ().push_back (RPG_Character_AbilityHelper::RPG_Character_AbilityToString (*iterator));
  player_p->abilities ().set (abilities);

  RPG_Player_Spells_XMLTree_Type spell_list;
  RPG_Magic_SpellTypes_t character_known_spells = player_in.getKnownSpells ();
  for (RPG_Magic_SpellTypesIterator_t iterator = character_known_spells.begin ();
       iterator != character_known_spells.end ();
       iterator++)
    spell_list.spell ().push_back (RPG_Magic_SpellTypeHelper::RPG_Magic_SpellTypeToString (*iterator));
  player_p->knownSpells ().set (spell_list);

  return player_p;
}

std::string
RPG_Player_Common_Tools::getPlayerProfilesDirectory ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Player_Common_Tools::getPlayerProfilesDirectory"));

  std::string result =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (yarp_PACKAGE_NAME),
                                                          ACE_TEXT_ALWAYS_CHAR (""),
                                                          ACE_TEXT_ALWAYS_CHAR (RPG_ENGINE_SUB_DIRECTORY_STRING),
                                                          false); // data-

  if (!Common_File_Tools::isDirectory (result))
  {
    if (!Common_File_Tools::createDirectory (result))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Common_File_Tools::createDirectory(\"%s\"), falling back\n"),
                  ACE_TEXT (result.c_str ())));

      // fallback
      result = Common_File_Tools::getTempDirectory ();
    } // end IF
    else
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("created player profiles directory \"%s\"\n"),
                  ACE_TEXT (result.c_str ())));
  } // end IF

  return result;
}
