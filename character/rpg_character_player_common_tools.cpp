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
#include "rpg_character_player_common_tools.h"

#include <rpg_common_macros.h>

#include <ace/Log_Msg.h>

RPG_Character_Skills_t
RPG_Character_Player_Common_Tools::skillsXMLTreeToSkills(const RPG_Character_Skills_XMLTree_Type& skills_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Character_Player_Common_Tools::skillsXMLTreeToSkills"));

  // init result
  RPG_Character_Skills_t result;

  for (RPG_Character_Skills_XMLTree_Type::skill_const_iterator iterator = skills_in.skill().begin();
       iterator != skills_in.skill().end();
       iterator++)
    result[RPG_Common_SkillHelper::stringToRPG_Common_Skill((*iterator).skill())] = (*iterator).rank();

  return result;
}

RPG_Character_Feats_t
RPG_Character_Player_Common_Tools::featsXMLTreeToFeats(const RPG_Character_Feats_XMLTree_Type& feats_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Character_Player_Common_Tools::featsXMLTreeToFeats"));

  // init result
  RPG_Character_Feats_t result;

  for (RPG_Character_Feats_XMLTree_Type::feat_const_iterator iterator = feats_in.feat().begin();
       iterator != feats_in.feat().end();
       iterator++)
    result.insert(RPG_Character_FeatHelper::stringToRPG_Character_Feat(*iterator));

  return result;
}

RPG_Character_Abilities_t
RPG_Character_Player_Common_Tools::abilitiesXMLTreeToAbilities(const RPG_Character_Abilities_XMLTree_Type& abilities_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Character_Player_Common_Tools::abilitiesXMLTreeToAbilities"));

  // init result
  RPG_Character_Abilities_t result;

  for (RPG_Character_Abilities_XMLTree_Type::ability_const_iterator iterator = abilities_in.ability().begin();
       iterator != abilities_in.ability().end();
       iterator++)
    result.insert(RPG_Character_AbilityHelper::stringToRPG_Character_Ability(*iterator));

  return result;
}

RPG_Magic_SpellTypes_t
RPG_Character_Player_Common_Tools::knownSpellXMLTreeToSpells(const RPG_Character_PlayerXML_XMLTree_Type::spell_sequence& spells_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Character_Player_Common_Tools::knownSpellXMLTreeToSpells"));

  RPG_Magic_SpellTypes_t result;

  for (RPG_Character_PlayerXML_XMLTree_Type::spell_const_iterator iterator = spells_in.begin();
       iterator != spells_in.end();
       iterator++)
    result.insert(RPG_Magic_SpellTypeHelper::stringToRPG_Magic_SpellType(*iterator));

  return result;
}

RPG_Character_Conditions_t
RPG_Character_Player_Common_Tools::conditionXMLTreeToCondition(const RPG_Character_PlayerXML_XMLTree_Type::condition_sequence& condition_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Character_Player_Common_Tools::conditionXMLTreeToCondition"));

  RPG_Character_Conditions_t result;

  for (RPG_Character_PlayerXML_XMLTree_Type::condition_const_iterator iterator = condition_in.begin();
       iterator != condition_in.end();
       iterator++)
    result.insert(RPG_Common_ConditionHelper::stringToRPG_Common_Condition(*iterator));

  return result;
}

RPG_Magic_Spells_t
RPG_Character_Player_Common_Tools::spellXMLTreeToSpells(const RPG_Character_PlayerXML_XMLTree_Type::spell_sequence& spells_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Character_Player_Common_Tools::spellXMLTreeToSpells"));

  RPG_Magic_Spells_t result;

  for (RPG_Character_PlayerXML_XMLTree_Type::spell_const_iterator iterator = spells_in.begin();
       iterator != spells_in.end();
       iterator++)
    result.push_back(RPG_Magic_SpellTypeHelper::stringToRPG_Magic_SpellType(*iterator));

  return result;
}
