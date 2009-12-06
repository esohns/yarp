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

#include <rpg_character_player_common.h>
#include <rpg_character_common_tools.h>

#include <rpg_chance_common_tools.h>

#include <ace/Log_Msg.h>

const bool RPG_Combat_Common_Tools::isPartyDeadOrHelpless(const RPG_Character_Party_t& party_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Combat_Common_Tools::isPartyDeadOrHelpless"));

  int numDeadOrHelpless = 0;
  for (RPG_Character_PartyIterator_t iterator = party_in.begin();
       iterator != party_in.end();
       iterator++)
  {
    if (isCharacterDeadOrHelpless(&(*iterator)))
    {
      numDeadOrHelpless++;
    } // end IF
  } // end FOR

  return (numDeadOrHelpless == party_in.size());
}

const bool RPG_Combat_Common_Tools::areMonstersDeadOrHelpless(const RPG_Character_Monsters_t& monsters_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Combat_Common_Tools::areMonstersDeadOrHelpless"));

  int numDeadOrHelplessGroups = 0;
  for (RPG_Character_MonstersIterator_t iterator = monsters_in.begin();
       iterator != monsters_in.end();
       iterator++)
  {
    if (RPG_Combat_Common_Tools::isMonsterGroupDeadOrHelpless(*iterator))
    {
      numDeadOrHelplessGroups++;
    } // end IF
  } // end FOR

  return (numDeadOrHelplessGroups == monsters_in.size());
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

  // step1: go through the list and compute initiatives
  int modifier = 0;
  char initiative = 0;
  std::pair<RPG_Combat_CombatantSequence_t::iterator, bool> preliminarySequencePosition;
  RPG_Combat_CombatSequenceConflict_t conflict;
  RPG_Combat_CombatSequenceConflicts_t conflicts;
  for (RPG_Character_ListIterator_t iterator = listOfCombatants.begin();
       iterator != listOfCombatants.end();
       iterator++)
  {
    RPG_Combat_CombatantSequenceElement element = {0, 0, *iterator};
    // compute initiative: DEX check
    element.DEXModifier = RPG_Character_Common_Tools::getAttributeAbilityModifier((*iterator)->getDexterity());
    element.initiative = RPG_Chance_Common_Tools::getCheck(element.DEXModifier);
//     element.handle = &(*iterator);

    // make sure there is a PROPER sequence:
    // if the set already contains this value we must resolve the conflict (see below)
    // *IMPORTANT NOTE*: this algorithm implements the notion of fairness as appropriate between two HUMAN actors,
    // i.e. we could have just re-rolled the current element until it doesn't clash. In a real-world situation this
    // would trigger discussions of WHO would re-roll...
    preliminarySequencePosition = battleSequence_out.insert(element);
    if (preliminarySequencePosition.second == false)
    {
      // find conflicting element
      RPG_Combat_CombatantSequenceIterator_t iterator2 = battleSequence_out.find(element);
      ACE_ASSERT(iterator2 != battleSequence_out.end());
      conflict = std::make_pair(element, *iterator2);
      conflicts.push_back(conflict);

      // erase conflicting element from the preliminary sequence
      battleSequence_out.erase(iterator2);
    } // end IF
  } // end FOR

  // step2: resolve conflicts
  while (!conflicts.empty())
  {
    // get first conflict
    RPG_Combat_CombatSequenceConflictsIterator_t iterator = conflicts.begin();

    // re-roll initiative for both elements
    // compute initiative: DEX check
    (*iterator).first.initiative = RPG_Chance_Common_Tools::getCheck((*iterator).first.DEXModifier);
    (*iterator).second.initiative = RPG_Chance_Common_Tools::getCheck((*iterator).second.DEXModifier);

    // make sure there is a PROPER sequence:
    // if the set already contains this value we must resolve the conflict (again)
    // *IMPORTANT NOTE*: this algorithm implements the notion of fairness as appropriate between two HUMAN actors,
    // i.e. we could have just re-rolled the current element until it doesn't clash. In a real-world situation this
    // would trigger discussions of WHO would re-roll...
    preliminarySequencePosition = battleSequence_out.insert((*iterator).first);
    if (preliminarySequencePosition.second == false)
    {
      // find conflicting element
      RPG_Combat_CombatantSequenceIterator_t iterator2 = battleSequence_out.find((*iterator).first);
      ACE_ASSERT(iterator2 != battleSequence_out.end());
      conflict = std::make_pair((*iterator).first, *iterator2);
      conflicts.push_back(conflict);

      // erase conflicting element from the preliminary sequence
      battleSequence_out.erase(iterator2);
    } // end IF

    preliminarySequencePosition = battleSequence_out.insert((*iterator).second);
    if (preliminarySequencePosition.second == false)
    {
      // find conflicting element
      RPG_Combat_CombatantSequenceIterator_t iterator2 = battleSequence_out.find((*iterator).second);
      ACE_ASSERT(iterator2 != battleSequence_out.end());
      conflict = std::make_pair((*iterator).second, *iterator2);
      conflicts.push_back(conflict);

      // erase conflicting element from the preliminary sequence
      battleSequence_out.erase(iterator2);
    } // end IF

    // OK: handled one conflict
    conflicts.erase(iterator);
  } // end WHILE

  ACE_ASSERT(listOfCombatants.size() == battleSequence_out.size());
  // debug info
  int i = 1;
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

const bool RPG_Combat_Common_Tools::isMonsterGroupDeadOrHelpless(const RPG_Character_MonsterGroupInstance_t& groupInstance_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Combat_Common_Tools::isMonsterGroupDeadOrHelpless"));

  int numDeadOrHelplessMonsters = 0;
  for (RPG_Character_MonsterGroupInstanceIterator_t iterator = groupInstance_in.begin();
       iterator != groupInstance_in.end();
       iterator++)
  {
    if (isCharacterDeadOrHelpless(&(*iterator)))
    {
      numDeadOrHelplessMonsters++;
    } // end IF
  } // end FOR

  return (numDeadOrHelplessMonsters == groupInstance_in.size());
}

const bool RPG_Combat_Common_Tools::isCharacterDeadOrHelpless(const RPG_Character_Base* character_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Combat_Common_Tools::isCharacterDeadOrHelpless"));

  ACE_ASSERT(character_in);

  if ((character_in->hasCondition(CONDITION_DEAD)) ||
      (character_in->hasCondition(CONDITION_DYING)) ||
      (character_in->hasCondition(CONDITION_PETRIFIED)) ||
      (character_in->hasCondition(CONDITION_STABLE)) ||
      (character_in->hasCondition(CONDITION_UNCONSCIOUS)))
  {
    return true;
  } // end IF

  return false;
}
