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

#ifndef RPG_PLAYER_COMMON_TOOLS_H
#define RPG_PLAYER_COMMON_TOOLS_H

#include "rpg_player_exports.h"
#include "rpg_player_common.h"
#include "rpg_player_XML_tree.h"

#include "rpg_character_common.h"
#include "rpg_character_skills_common.h"

#include "rpg_magic_common.h"

class RPG_Player_Export RPG_Player_Common_Tools
{
 public:
  static unsigned int restParty(RPG_Player_Party_t&); // party of players

  // *WARNING*: result needs to be delete()d !
  static RPG_Player* playerXMLToPlayer(const RPG_Player_PlayerXML_XMLTree_Type&, // player model
                                       // current status
                                       const RPG_Character_Conditions_t&,        // condition
                                       const short int&);                        // HP
  // *WARNING*: result needs to be delete()d !
  static RPG_Player_PlayerXML_XMLTree_Type* playerToPlayerXML(const RPG_Player&);

  static std::string getPlayerProfilesDirectory();

 private:
  ACE_UNIMPLEMENTED_FUNC(RPG_Player_Common_Tools());
  ACE_UNIMPLEMENTED_FUNC(~RPG_Player_Common_Tools());
  ACE_UNIMPLEMENTED_FUNC(RPG_Player_Common_Tools(const RPG_Player_Common_Tools&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Player_Common_Tools& operator=(const RPG_Player_Common_Tools&));

  static RPG_Character_Race_t raceXMLTreeToRace(const RPG_Player_PlayerXML_XMLTree_Type::race_sequence&); // race (XML format)
  static RPG_Character_Abilities_t abilitiesXMLTreeToAbilities(const RPG_Player_Abilities_XMLTree_Type&); // abilities (XML format)

  static RPG_Magic_SpellTypes_t spellsXMLTreeToSpellTypes(const RPG_Player_Spells_XMLTree_Type&); // spells (XML format)
  static RPG_Character_Conditions_t conditionsXMLTreeToConditions(const RPG_Player_Conditions_XMLTree_Type&); // condition (XML format)
  static RPG_Magic_Spells_t spellsXMLTreeToSpells(const RPG_Player_Spells_XMLTree_Type&); // spells (XML format)
};

#endif
