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

const bool RPG_Combat_Common_Tools::isPartyDead(const RPG_Character_Party_t& party_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Combat_Common_Tools::isPartyDead"));

  int numHelpless = 0;
  for (RPG_Character_PartyIterator_t iterator = party_in.begin();
       iterator != party_in.end();
       iterator++)
  {
    if (((*iterator).hasCondition(CONDITION_DEAD)) ||
        ((*iterator).hasCondition(CONDITION_DYING)) ||
        ((*iterator).hasCondition(CONDITION_PETRIFIED)) ||
        ((*iterator).hasCondition(CONDITION_STABLE)) ||
        ((*iterator).hasCondition(CONDITION_UNCONSCIOUS)))
    {
      numHelpless++;
    } // end IF
  } // end FOR

  return (numHelpless == party_in.size());
}

const bool RPG_Combat_Common_Tools::areMonstersDead(const RPG_Character_Monsters_t& monsters_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Combat_Common_Tools::areMonstersDead"));

  int numDeadGroups = 0;
  for (RPG_Character_MonstersIterator_t iterator = monsters_in.begin();
       iterator != monsters_in.end();
       iterator++)
  {
    if (RPG_Combat_Common_Tools::isMonsterGroupDead(*iterator))
    {
      numDeadGroups++;
    } // end IF
  } // end FOR

  return (numDeadGroups == monsters_in.size());
}

const bool RPG_Combat_Common_Tools::isMonsterGroupDead(const RPG_Character_MonsterGroupInstance_t& groupInstance_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Combat_Common_Tools::isMonsterGroupDead"));

  int numDeadMonsters = 0;

  return (numDeadMonsters == groupInstance_in.size());
}
