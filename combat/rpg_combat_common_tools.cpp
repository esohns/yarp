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

#include <rpg_character_player_common.h>
#include <rpg_character_common_tools.h>

#include <rpg_dice.h>
#include <rpg_chance_common_tools.h>

#include <ace/Log_Msg.h>

// init statics
RPG_Combat_AttackFormToStringTable_t RPG_Combat_AttackFormHelper::myRPG_Combat_AttackFormToStringTable;

void RPG_Combat_Common_Tools::initStringConversionTables()
{
  ACE_TRACE(ACE_TEXT("RPG_Combat_Common_Tools::initStringConversionTables"));

  RPG_Combat_AttackFormHelper::init();

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
    element.DEXModifier = RPG_Character_Common_Tools::getAttributeAbilityModifier((*iterator)->getDexterity());
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

void RPG_Combat_Common_Tools::attackFoe(const RPG_Character_Base* const attacker_in,
                                        RPG_Character_Base* const target_inout)
{
  ACE_TRACE(ACE_TEXT("RPG_Combat_Common_Tools::attackFoe"));

  // sanity check
  ACE_ASSERT(attacker_in && target_inout);

  // step1: choose primary weapon

  if (attacker_in->isPlayerCharacter())
  {

  } // end IF
  else
  {
    // if the attacker is a "regular" monster, we have a description of its weapons

  } // end ELSE
}
