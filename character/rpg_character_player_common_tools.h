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
#ifndef RPG_CHARACTER_PLAYER_COMMON_TOOLS_H
#define RPG_CHARACTER_PLAYER_COMMON_TOOLS_H

#include "rpg_character_common.h"
#include "rpg_character_skills_common.h"
#include "rpg_character_XML_tree.h"
#include "rpg_character_player_XML_tree.h"

#include <rpg_magic_common.h>

class RPG_Character_Player_Common_Tools
{
 public:
  static RPG_Character_Skills_t skillsXMLTreeToSkills(const RPG_Character_Skills_XMLTree_Type&); // skills (XML format)
  static RPG_Character_Feats_t featsXMLTreeToFeats(const RPG_Character_Feats_XMLTree_Type&); // feats (XML format)
  static RPG_Character_Abilities_t abilitiesXMLTreeToAbilities(const RPG_Character_Abilities_XMLTree_Type&); // abilities (XML format)

  static RPG_Magic_Spells_t knownSpellXMLTreeToSpells(const RPG_Character_PlayerXML_XMLTree_Type::spell_sequence&); // spells (XML format)
  static RPG_Character_Conditions_t conditionXMLTreeToCondition(const RPG_Character_PlayerXML_XMLTree_Type::condition_sequence&); // condition (XML format)
  static RPG_Magic_SpellTypes_t spellXMLTreeToSpells(const RPG_Character_PlayerXML_XMLTree_Type::spell_sequence&); // spells (XML format)

 private:
  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_Player_Common_Tools());
  ACE_UNIMPLEMENTED_FUNC(~RPG_Character_Player_Common_Tools());
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_Player_Common_Tools(const RPG_Character_Player_Common_Tools&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_Player_Common_Tools& operator=(const RPG_Character_Player_Common_Tools&));
};

#endif
