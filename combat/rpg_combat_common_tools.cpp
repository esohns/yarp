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
#include "rpg_combat_common_tools.h"

#include "rpg_combat_attackform.h"
#include "rpg_combat_specialattack.h"
#include "rpg_combat_specialdamagetype.h"
#include "rpg_combat_damageeffecttype.h"

#include <rpg_character_common_tools.h>

#include <rpg_dice.h>
#include <rpg_dice_common_tools.h>
#include <rpg_chance_common_tools.h>

#include <ace/Log_Msg.h>

#include <sstream>

// init statics
RPG_Combat_AttackFormToStringTable_t RPG_Combat_AttackFormHelper::myRPG_Combat_AttackFormToStringTable;
RPG_Combat_SpecialAttackToStringTable_t RPG_Combat_SpecialAttackHelper::myRPG_Combat_SpecialAttackToStringTable;
RPG_Combat_SpecialDamageTypeToStringTable_t RPG_Combat_SpecialDamageTypeHelper::myRPG_Combat_SpecialDamageTypeToStringTable;
RPG_Combat_DamageEffectTypeToStringTable_t RPG_Combat_DamageEffectTypeHelper::myRPG_Combat_DamageEffectTypeToStringTable;

void RPG_Combat_Common_Tools::initStringConversionTables()
{
  ACE_TRACE(ACE_TEXT("RPG_Combat_Common_Tools::initStringConversionTables"));

  RPG_Combat_AttackFormHelper::init();
  RPG_Combat_SpecialAttackHelper::init();
  RPG_Combat_SpecialDamageTypeHelper::init();
  RPG_Combat_DamageEffectTypeHelper::init();

  // debug info
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("RPG_Combat_Common_Tools: initialized string conversion tables...\n")));
}


const std::string RPG_Combat_Common_Tools::attackFormsToString(const RPG_Combat_AttackForms_t& attackForms_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Combat_Common_Tools::attackFormsToString"));

  std::string result;

  for (RPG_Combat_AttackFormsIterator_t iterator = attackForms_in.begin();
       iterator != attackForms_in.end();
       iterator++)
  {
    result += RPG_Combat_AttackFormHelper::RPG_Combat_AttackFormToString(*iterator);
    result += ACE_TEXT_ALWAYS_CHAR("|");
  } // end FOR

  if (!result.empty())
  {
    result.erase(--(result.end()));
  } // end IF

  return result;
}

const std::string RPG_Combat_Common_Tools::damageToString(const RPG_Combat_Damage& damage_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Combat_Common_Tools::damageToString"));

  std::string result;
  std::stringstream converter;

  for (std::vector<RPG_Combat_DamageElement>::const_iterator iterator = damage_in.elements.begin();
       iterator != damage_in.elements.end();
       iterator++)
  {
    converter.str(ACE_TEXT_ALWAYS_CHAR(""));

    result += ACE_TEXT_ALWAYS_CHAR("type: ");
//     result += RPG_Combat_DamageTypeUnionHelper::RPG_Combat_DamageTypeUnionToString((*iterator).type);
    result += ACE_TEXT_ALWAYS_CHAR("\ndamage: ");
    result += RPG_Dice_Common_Tools::rollToString((*iterator).damage);
    converter << (*iterator).duration;
    result += ACE_TEXT_ALWAYS_CHAR("\nduration: ");
    result += converter.str();
    result += ACE_TEXT_ALWAYS_CHAR("\neffect: ");
    result += RPG_Combat_DamageEffectTypeHelper::RPG_Combat_DamageEffectTypeToString((*iterator).effect);
    result += ACE_TEXT_ALWAYS_CHAR("\n");
  } // end FOR

  if (!result.empty())
  {
    result.erase(--(result.end()));
  } // end IF

  return result;
}

const bool RPG_Combat_Common_Tools::isPartyHelpless(const RPG_Character_Party_t& party_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Combat_Common_Tools::isPartyHelpless"));

  int numDeadOrHelpless = 0;
  for (RPG_Character_PartyIterator_t iterator = party_in.begin();
       iterator != party_in.end();
       iterator++)
  {
    if (isCharacterHelpless(&(*iterator)))
    {
      numDeadOrHelpless++;
    } // end IF
  } // end FOR

  return (numDeadOrHelpless == party_in.size());
}

const bool RPG_Combat_Common_Tools::areMonstersHelpless(const RPG_Character_Monsters_t& monsters_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Combat_Common_Tools::areMonstersHelpless"));

  int numHelplessGroups = 0;
  for (RPG_Character_MonstersIterator_t iterator = monsters_in.begin();
       iterator != monsters_in.end();
       iterator++)
  {
    if (RPG_Combat_Common_Tools::isMonsterGroupHelpless(*iterator))
    {
      numHelplessGroups++;
    } // end IF
  } // end FOR

  return (numHelplessGroups == monsters_in.size());
}

void RPG_Combat_Common_Tools::getCombatantSequence(const RPG_Character_Party_t& party_in,
                                                   const RPG_Character_Monsters_t& monsters_in,
                                                   RPG_Combat_CombatantSequence_t& battleSequence_out)
{
  ACE_TRACE(ACE_TEXT("RPG_Combat_Common_Tools::getCombatantSequence"));

  // init result
  battleSequence_out.clear();

  // step0: throw everybody into a list
  RPG_Character_List_t listOfCombatants;
  for (RPG_Character_PartyIterator_t iterator = party_in.begin();
       iterator != party_in.end();
       iterator++)
  {
    listOfCombatants.push_back(ACE_const_cast(RPG_Character_Player*,
                                              &(*iterator)));
  } // end FOR
  for (RPG_Character_MonstersIterator_t iterator = monsters_in.begin();
       iterator != monsters_in.end();
       iterator++)
  {
    for (RPG_Character_MonsterGroupInstanceIterator_t iterator2 = (*iterator).begin();
         iterator2 != (*iterator).end();
         iterator2++)
    {
      listOfCombatants.push_back(ACE_const_cast(RPG_Character_Monster*,
                                                &(*iterator2)));
    } // end FOR
  } // end FOR

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("sorting %d combatants...\n"),
             listOfCombatants.size()));

  // step1: go through the list and compute initiatives
  RPG_Combat_CombatSequenceList_t preliminarySequence;
  for (RPG_Character_ListIterator_t iterator = listOfCombatants.begin();
       iterator != listOfCombatants.end();
       iterator++)
  {
    RPG_Combat_CombatantSequenceElement element = {0, 0, *iterator};
    // compute initiative: DEX check
    element.DEXModifier = RPG_Character_Common_Tools::getAttributeAbilityModifier((*iterator)->getAttribute(ATTRIBUTE_DEXTERITY));
    // make sure there are enough SLOTS for large armies !
    element.initiative = RPG_Chance_Common_Tools::getCheck(element.DEXModifier,
                                                           D_20);
//     element.handle = &(*iterator);

    preliminarySequence.push_back(element);
  } // end FOR

  // step2: sort according to criteria: descending initiative / DEX modifier
  std::pair<RPG_Combat_CombatantSequenceIterator_t, bool> preliminarySequencePosition;
  RPG_Combat_CombatSequenceList_t conflicts;
  for (RPG_Combat_CombatSequenceListIterator_t iterator = preliminarySequence.begin();
       iterator != preliminarySequence.end();
       iterator++)
  {
    // make sure there is a PROPER sequence:
    // if the set already contains this value we must resolve the conflict (again)
    // *IMPORTANT NOTE*: this algorithm implements the notion of fairness as appropriate between two HUMAN/HUMAN-Monster actors,
    // i.e. we could have just re-rolled the current element until it doesn't clash. In a real-world situation this
    // would trigger discussions of WHO would re-roll...
    preliminarySequencePosition = battleSequence_out.insert(*iterator);
    if (preliminarySequencePosition.second == false)
    {
      // find conflicting element
      RPG_Combat_CombatantSequenceIterator_t iterator2 = battleSequence_out.find(*iterator);
      ACE_ASSERT(iterator2 != battleSequence_out.end());

      conflicts.push_back(*iterator);
      conflicts.push_back(*iterator2);

      // erase conflicting element from the preliminary sequence
      battleSequence_out.erase(iterator2);
    } // end IF
  } // end FOR

  // step3: resolve conflicts
  // *TODO* there's a potential bug here for large armies: change Die Type D_20 --> D_100/D_1000/... ?
  while (!conflicts.empty())
  {
//     // handle first conflict
//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("resolving %d conflicts...\n"),
//                conflicts.size()));

    // re-roll initiative
    // compute initiative: DEX check
    conflicts.front().initiative = RPG_Chance_Common_Tools::getCheck(conflicts.front().DEXModifier,
                                                                     D_20);

    // make sure there is a PROPER sequence:
    // if the set already contains this value we must resolve the conflict (again)
    // *IMPORTANT NOTE*: this algorithm implements the notion of fairness as appropriate between two HUMAN/HUMAN-Monster actors,
    // i.e. we could have just re-rolled the current element until it doesn't clash. In a real-world situation this
    // would trigger discussions of WHO would re-roll...
    preliminarySequencePosition = battleSequence_out.insert(conflicts.front());
    if (preliminarySequencePosition.second == false)
    {
      // find conflicting element
      RPG_Combat_CombatantSequenceIterator_t iterator2 = battleSequence_out.find(conflicts.front());
      ACE_ASSERT(iterator2 != battleSequence_out.end());

      conflicts.push_back(conflicts.front());
      conflicts.push_back(*iterator2);

      // erase conflicting element from the preliminary sequence
      battleSequence_out.erase(iterator2);
    } // end IF

    conflicts.pop_front();
  } // end WHILE

  ACE_ASSERT(listOfCombatants.size() == battleSequence_out.size());
  // debug info
  int i = 1;
//   for (RPG_Combat_CombatantSequenceRIterator_t iterator = battleSequence_out.rbegin();
//        iterator != battleSequence_out.rend();
//        iterator++, i++)
  for (RPG_Combat_CombatantSequenceIterator_t iterator = battleSequence_out.begin();
       iterator != battleSequence_out.end();
       iterator++, i++)
  {
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("combatant #%d: DEXModifier: %d, Initiative: %d, Name: \"%s\"\n"),
               i,
               (*iterator).DEXModifier,
               (*iterator).initiative,
               (*iterator).handle->getName().c_str()));
  } // end FOR
}

void RPG_Combat_Common_Tools::performCombatRound(const RPG_Combat_CombatantSequence_t& battleSequence_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Combat_Common_Tools::performCombatRound"));

  // everybody gets their turn
  bool isPlayer = false;
  RPG_Combat_CombatantSequenceIterator_t iterator;
  RPG_Dice_RollResult_t result;
  for (RPG_Combat_CombatantSequenceIterator_t iterator2 = battleSequence_in.begin();
       iterator2 != battleSequence_in.end();
       iterator2++)
  {
    // step1: find (random) opponent
    // step1a: determine friend or foe
    isPlayer = (*iterator2).handle->isPlayerCharacter();
    iterator = battleSequence_in.begin();
    do
    {
      result.clear();
      RPG_Dice::generateRandomNumbers(battleSequence_in.size(),
                                      1,
                                      result);
      std::advance(iterator, result.front() - 1);
    } while ((iterator == iterator2) ||                               // dont't attack ourselves !
             ((*iterator).handle->isPlayerCharacter() == isPlayer) || // don't attack friends
             (isCharacterDeadOrDying((*iterator).handle)));           // leave the dead/dying alone (...for now)

    // step2: attack foe !
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("\"%s\" attacks \"%s\"...\n"),
               (*iterator2).handle->getName().c_str(),
               (*iterator).handle->getName().c_str()));

    attackFoe((*iterator2).handle,
              ACE_const_cast(RPG_Character_Base*, (*iterator).handle));
  } // end FOR
}

const bool RPG_Combat_Common_Tools::isMonsterGroupHelpless(const RPG_Character_MonsterGroupInstance_t& groupInstance_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Combat_Common_Tools::isMonsterGroupHelpless"));

  int numHelplessMonsters = 0;
  for (RPG_Character_MonsterGroupInstanceIterator_t iterator = groupInstance_in.begin();
       iterator != groupInstance_in.end();
       iterator++)
  {
    if (isCharacterHelpless(&(*iterator)))
    {
      numHelplessMonsters++;
    } // end IF
  } // end FOR

  return (numHelplessMonsters == groupInstance_in.size());
}

const bool RPG_Combat_Common_Tools::isCharacterHelpless(const RPG_Character_Base* const character_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Combat_Common_Tools::isCharacterHelpless"));

  ACE_ASSERT(character_in);

  if ((character_in->hasCondition(CONDITION_PARALYZED)) || // spell, ...
       (character_in->hasCondition(CONDITION_HELD)) ||     // bound as per spell, ...
       (character_in->hasCondition(CONDITION_BOUND)) ||    // bound ase per rope, ...
       (character_in->hasCondition(CONDITION_SLEEPING)) || // natural, spell, ...
       isCharacterDeadOrDying(character_in))               // dead/dying
  {
    return true;
  } // end IF

  return false;
}

const bool RPG_Combat_Common_Tools::isCharacterDeadOrDying(const RPG_Character_Base* const character_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Combat_Common_Tools::isCharacterDeadOrDying"));

  ACE_ASSERT(character_in);

  if ((character_in->hasCondition(CONDITION_DISABLED)) ||    // (HP==0) || (STABLE && CONSCIOUS)
      (character_in->hasCondition(CONDITION_DEAD)) ||        // HP<-10
      (character_in->hasCondition(CONDITION_DYING)) ||       // -10<HP<0
      (character_in->hasCondition(CONDITION_STABLE)) ||      // -10<HP<0 && !DYING
      (character_in->hasCondition(CONDITION_UNCONSCIOUS)) || // -10<HP<0 && (DYING || STABLE)
      (character_in->hasCondition(CONDITION_PETRIFIED)))     // turned to stone
  {
    return true;
  } // end IF

  return false;
}

RPG_Combat_AttackForm RPG_Combat_Common_Tools::weaponTypeToAttackForm(const RPG_Item_WeaponType& weaponType_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Combat_Common_Tools::weaponTypeToAttackForm"));

  switch (weaponType_in)
  {
    case UNARMED_WEAPON_GAUNTLET:
    case UNARMED_WEAPON_STRIKE:
    case LIGHT_MELEE_WEAPON_DAGGER:
    case LIGHT_MELEE_WEAPON_GAUNTLET_SPIKED:
    case LIGHT_MELEE_WEAPON_MACE_LIGHT:
    case LIGHT_MELEE_WEAPON_SICKLE:
    case ONE_HANDED_MELEE_WEAPON_CLUB:
    case ONE_HANDED_MELEE_WEAPON_MACE_HEAVY:
    case ONE_HANDED_MELEE_WEAPON_MORNINGSTAR:
    case ONE_HANDED_MELEE_WEAPON_SHORTSPEAR:
    case TWO_HANDED_MELEE_WEAPON_LONGSPEAR:
    case TWO_HANDED_MELEE_WEAPON_QUARTERSTAFF:
    case TWO_HANDED_MELEE_WEAPON_SPEAR:
    case LIGHT_MELEE_WEAPON_AXE_THROWING:
    case LIGHT_MELEE_WEAPON_HAMMER_LIGHT:
    case LIGHT_MELEE_WEAPON_AXE_HAND:
    case LIGHT_MELEE_WEAPON_KUKRI:
    case LIGHT_MELEE_WEAPON_PICK_LIGHT:
    case LIGHT_MELEE_WEAPON_SAP:
    case LIGHT_MELEE_WEAPON_SHIELD_LIGHT:
    case LIGHT_MELEE_WEAPON_ARMOR_SPIKED:
    case LIGHT_MELEE_WEAPON_SHIELD_LIGHT_SPIKED:
    case LIGHT_MELEE_WEAPON_SWORD_SHORT:
    case ONE_HANDED_MELEE_WEAPON_AXE_BATTLE:
    case ONE_HANDED_MELEE_WEAPON_FLAIL_LIGHT:
    case ONE_HANDED_MELEE_WEAPON_SWORD_LONG:
    case ONE_HANDED_MELEE_WEAPON_PICK_HEAVY:
    case ONE_HANDED_MELEE_WEAPON_RAPIER:
    case ONE_HANDED_MELEE_WEAPON_SCIMITAR:
    case ONE_HANDED_MELEE_WEAPON_SHIELD_HEAVY:
    case ONE_HANDED_MELEE_WEAPON_SHIELD_HEAVY_SPIKED:
    case ONE_HANDED_MELEE_WEAPON_TRIDENT:
    case ONE_HANDED_MELEE_WEAPON_HAMMER_WAR:
    case TWO_HANDED_MELEE_WEAPON_FALCHION:
    case TWO_HANDED_MELEE_WEAPON_GLAIVE:
    case TWO_HANDED_MELEE_WEAPON_AXE_GREAT:
    case TWO_HANDED_MELEE_WEAPON_CLUB_GREAT:
    case TWO_HANDED_MELEE_WEAPON_FLAIL_HEAVY:
    case TWO_HANDED_MELEE_WEAPON_SWORD_GREAT:
    case TWO_HANDED_MELEE_WEAPON_GUISARME:
    case TWO_HANDED_MELEE_WEAPON_HALBERD:
    case TWO_HANDED_MELEE_WEAPON_LANCE:
    case TWO_HANDED_MELEE_WEAPON_RANSEUR:
    case TWO_HANDED_MELEE_WEAPON_SCYTHE:
    case LIGHT_MELEE_WEAPON_KAMA:
    case LIGHT_MELEE_WEAPON_NUNCHAKU:
    case LIGHT_MELEE_WEAPON_SAI:
    case LIGHT_MELEE_WEAPON_SIANGHAM:
    case ONE_HANDED_MELEE_WEAPON_SWORD_BASTARD:
    case ONE_HANDED_MELEE_WEAPON_AXE_WAR_DWARVEN:
    case ONE_HANDED_MELEE_WEAPON_WHIP:
    case TWO_HANDED_MELEE_WEAPON_AXE_ORC_DOUBLE:
    case TWO_HANDED_MELEE_WEAPON_CHAIN_SPIKED:
    case TWO_HANDED_MELEE_WEAPON_FLAIL_DIRE:
    case TWO_HANDED_MELEE_WEAPON_HAMMER_GNOME_HOOKED:
    case TWO_HANDED_MELEE_WEAPON_SWORD_TWO_BLADED:
    case TWO_HANDED_MELEE_WEAPON_URGROSH_DWARVEN:
    {
      return ATTACK_MELEE;
    }
    case RANGED_WEAPON_CROSSBOW_LIGHT:
    case RANGED_WEAPON_CROSSBOW_HEAVY:
    case RANGED_WEAPON_DART:
    case RANGED_WEAPON_JAVELIN:
    case RANGED_WEAPON_SLING:
    case RANGED_WEAPON_BOW_SHORT:
    case RANGED_WEAPON_BOW_SHORT_COMPOSITE:
    case RANGED_WEAPON_BOW_LONG:
    case RANGED_WEAPON_BOW_LONG_COMPOSITE:
    case RANGED_WEAPON_BOLAS:
    case RANGED_WEAPON_CROSSBOW_HAND:
    case RANGED_WEAPON_CROSSBOW_REPEATING_LIGHT:
    case RANGED_WEAPON_CROSSBOW_REPEATING_HEAVY:
    case RANGED_WEAPON_NET:
    case RANGED_WEAPON_SHURIKEN:
    {
      return ATTACK_RANGED;
    }
    default:
    {
      // debug info
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid weapon type: \"%s\", aborting\n"),
                 RPG_Item_WeaponTypeHelper::RPG_Item_WeaponTypeToString(weaponType_in).c_str()));

      break;
    }
  } // end SWITCH

  return RPG_COMBAT_ATTACKFORM_INVALID;
}

const signed char RPG_Combat_Common_Tools::getSizeModifier(const RPG_Character_Size& size_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Combat_Common_Tools::getSizeModifier"));

  // SIZE_FINE:       8
  // SIZE_DIMINUTIVE: 4
  // SIZE_TINY:       2
  // SIZE_SMALL:      1
  // SIZE_MEDIUM:     0
  // SIZE_LARGE:      -1
  // SIZE_HUGE:       -2
  // SIZE_GARGANTUAN: -4
  // SIZE_COLOSSAL:   -8
  // --> 2**(distance to medium - 1);
  if (size_in == SIZE_MEDIUM)
    return 0;

  signed char result = 1;
  result <<= ::abs(SIZE_MEDIUM - size_in - 1);

  return ((size_in > SIZE_MEDIUM) ? -result : result);

//   switch (size_in)
//   {
//     case SIZE_FINE:
//     {
//       return 8;
//     }
//     case SIZE_DIMINUTIVE:
//     {
//       return 4;
//     }
//     case SIZE_TINY:
//     {
//       return 2;
//     }
//     case SIZE_SMALL:
//     {
//       return 1;
//     }
//     case SIZE_MEDIUM:
//     {
//       return 0;
//     }
//     case SIZE_LARGE:
//     {
//       return -1;
//     }
//     case SIZE_HUGE:
//     {
//       return -2;
//     }
//     case SIZE_GARGANTUAN:
//     {
//       return -4;
//     }
//     case SIZE_COLOSSAL:
//     {
//       return -8;
//     }
//     default:
//     {
//       // debug info
//       ACE_DEBUG((LM_ERROR,
//                  ACE_TEXT("invalid size: \"%s\", aborting\n"),
//                  RPG_Character_SizeHelper::RPG_Character_SizeToString(size_in).c_str()));
//
//       break;
//     }
//   } // end SWITCH
//
//   return 0;
}

void RPG_Combat_Common_Tools::attackFoe(const RPG_Character_Base* const attacker_in,
                                        RPG_Character_Base* const target_inout,
                                        const RPG_Character_AttackSituation& attackSituation_in,
                                        const RPG_Character_DefenseSituation& defenseSituation_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Combat_Common_Tools::attackFoe"));

  // sanity check
  ACE_ASSERT(attacker_in && target_inout);

  RPG_Dice_Roll roll;
  RPG_Dice_RollResult_t result;
  if (attacker_in->isPlayerCharacter())
  {
    // attack roll: D_20 + attack bonus + other modifiers
    // step1: compute attack bonus(ses) --> number of attacks
    const RPG_Character_Player_Base* player_base = ACE_dynamic_cast(const RPG_Character_Player_Base*,
                                                                    attacker_in);
    ACE_ASSERT(player_base);
    RPG_Character_Classes_t classes = player_base->getClasses();
    // attack bonusses stack for multiclass characters...
    RPG_Character_BaseAttackBonus_t baseAttackBonus;
    for (RPG_Character_ClassesIterator_t iterator = classes.begin();
         iterator != classes.end();
         iterator++)
    {
      RPG_Character_BaseAttackBonus_t bonus = RPG_Character_Common_Tools::getBaseAttackBonus((*iterator).subClass,
          player_base->getLevel((*iterator).subClass));
      // append necessary entries
      for (int diff = bonus.size() - baseAttackBonus.size();
           diff > 0;
           diff--)
        baseAttackBonus.push_back(0);
      int index = 0;
      for (RPG_Character_BaseAttackBonusIterator_t iterator2 = bonus.begin();
           iterator2 != bonus.end();
           iterator2++, index++)
        baseAttackBonus[index] += *iterator2;
    } // end FOR

    // step2: perform attack(s)
    int attack_roll = 0;
    for (RPG_Character_BaseAttackBonusIterator_t iterator = baseAttackBonus.begin();
         iterator != baseAttackBonus.end();
         iterator++)
    {
      // step2a: roll D_20
      result.clear();
      roll.numDice = 1;
      roll.typeDice = D_20;
      roll.modifier = 0;
      RPG_Dice::simulateRoll(roll,
                             1,
                             result);
      attack_roll = result.front();
      // attack bonus: base attack bonus + STR/DEX modifier + size modifier (+ range penalty)
      attack_roll += *iterator;
      // --> check primary weapon
      RPG_Character_Attribute attribute = ATTRIBUTE_STRENGTH;
      RPG_Combat_AttackForm attackForm = weaponTypeToAttackForm(player_base->getEquipment()->getPrimaryWeapon());
      if (attackForm == ATTACK_RANGED)
        attribute = ATTRIBUTE_DEXTERITY;
      attack_roll += RPG_Character_Common_Tools::getAttributeAbilityModifier(attacker_in->getAttribute(attribute));
      attack_roll += RPG_Combat_Common_Tools::getSizeModifier(attacker_in->getSize());
      // *TODO*: consider other modifiers (e.g. range penalty)...

      // step2b: compute target AC
      // AC = 10 + armor bonus + shield bonus + DEX modifier + size modifier + other modifiers
      int AC = 10;
      RPG_Character_Monster* monster = ACE_dynamic_cast(RPG_Character_Monster*,
                                                        target_inout);
      ACE_ASSERT(monster);
      AC += monster->getArmorBonus();
      AC += monster->getShieldBonus();
      AC += RPG_Character_Common_Tools::getAttributeAbilityModifier(target_inout->getAttribute(ATTRIBUTE_DEXTERITY));
      AC += RPG_Combat_Common_Tools::getSizeModifier(target_inout->getSize());
    } // end FOR
  } // end IF
  else
  {
    // if the attacker is a "regular" monster, we have a description of its weapons

  } // end ELSE
}