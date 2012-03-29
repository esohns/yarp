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

#ifndef RPG_ENGINE_COMMON_TOOLS_H
#define RPG_ENGINE_COMMON_TOOLS_H

#include "rpg_engine_exports.h"
#include "rpg_engine_common.h"
#include "rpg_engine_level.h"
#include "rpg_engine_XML_tree.h"

#include <rpg_graphics_orientation.h>
#include <rpg_graphics_cursor.h>
#include <rpg_graphics_sprite.h>

#include <rpg_map_common.h>

#include <rpg_monster_common.h>

#include <rpg_player_common.h>
#include <rpg_player_XML_tree.h>

#include <rpg_character_class_common.h>

#include <rpg_item_instance_common.h>

#include <rpg_common_subclass.h>

#include <ace/Global_Macros.h>

#include <string>
#include <deque>

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Engine_Export RPG_Engine_Common_Tools
{
 public:
  // *NOTE*: initializes (static) data from ALL individual modules
  // namely: - randomization
  //         - string conversion facilities
  //         - static tables (skills, spells, ...)
  //         - dictionaries (spells, items, monsters, ...)
  static void init(const std::string&,  // magic dictionary file
                   const std::string&,  // item dictionary file
                   const std::string&); // monster dictionary file
  // *NOTE*: entity.character must be deleted() by the caller !
  static RPG_Engine_Entity loadEntity(const std::string&, // FQ filename
                                      const std::string&, // schema repository (directory)
                                      const bool&);       // load sprite graphic ?
  static const bool saveEntity(const RPG_Engine_Entity&, // entity
                               const std::string&);      // FQ filename
  // *NOTE*: return value entity.character must be deleted() by the caller !
  static RPG_Engine_Entity createEntity(const bool&); // load sprite graphic ?
  static RPG_Engine_Entity createEntity(const std::string&); // creature type
  static std::string info(const RPG_Engine_Entity&); // entity

  static RPG_Item_List_t generateStandardItems(const RPG_Common_SubClass&);

  // ***** combat-related *****
  static const bool isPartyHelpless(const RPG_Player_Party_t&); // party
  static const bool areMonstersHelpless(const RPG_Monster_Groups_t&); // monsters
  static void getCombatantSequence(const RPG_Player_Party_t&,        // party
                                   const RPG_Monster_Groups_t&,      // monsters
                                   RPG_Engine_CombatantSequence_t&); // battle sequence
  static void performCombatRound(const RPG_Combat_AttackSituation&,      // attack situation
                                 const RPG_Combat_DefenseSituation&,     // defense situation
                                 const RPG_Engine_CombatantSequence_t&); // battle sequence

  // ***** map/graphics-related *****
  static const RPG_Graphics_Sprite class2Sprite(const RPG_Character_Class&);

  static const bool hasCeiling(const RPG_Map_Position_t&,
                               const RPG_Engine_Level&);
  // either floor or an open (!) door
  static const bool isValid(const RPG_Map_Position_t&,
                            const RPG_Engine_Level&);

  static const RPG_Graphics_Orientation getDoorOrientation(const RPG_Engine_Level&,    // state
                                                           const RPG_Map_Position_t&); // door
  static const RPG_Graphics_Cursor getCursor(const RPG_Map_Position_t&, // position
                                             const RPG_Engine_Level&);  // state

 private:
  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Engine_Common_Tools());
  ACE_UNIMPLEMENTED_FUNC(~RPG_Engine_Common_Tools());
  ACE_UNIMPLEMENTED_FUNC(RPG_Engine_Common_Tools(const RPG_Engine_Common_Tools&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Engine_Common_Tools& operator=(const RPG_Engine_Common_Tools&));

  // helper types
  typedef std::deque<RPG_Engine_CombatantSequenceElement> RPG_Engine_CombatSequenceList_t;
  typedef RPG_Engine_CombatSequenceList_t::iterator RPG_Engine_CombatSequenceListIterator_t;

  static const bool isMonsterGroupHelpless(const RPG_Monster_Group_t&); // group instance
  static const bool isCharacterHelpless(const RPG_Player_Base* const); // character handle
  static const bool isValidFoeAvailable(const bool&,                            // monsters ? : players
                                        const RPG_Engine_CombatantSequence_t&); // battle sequence
  static const bool isCharacterDisabled(const RPG_Player_Base* const); // character handle

  static const unsigned int numCompatibleMonsterAttackActions(const RPG_Combat_AttackForm&,
                                                              const RPG_Monster_AttackActions_t&);
  static const bool isCompatibleMonsterAttackAction(const RPG_Combat_AttackForm&,
                                                    const RPG_Monster_AttackAction&);
  static void attackFoe(const RPG_Player_Base* const,       // attacker
                        RPG_Player_Base* const,             // target
                        const RPG_Combat_AttackSituation&,  // attacker situation
                        const RPG_Combat_DefenseSituation&, // defender situation
                        const bool&,                        // is this a Full-Round Action ?
                        const unsigned short&);             // distance (feet)

  static const bool isCorner(const RPG_Map_Position_t&,
                             const RPG_Engine_Level&);

  static const RPG_Graphics_Sprite monster2Sprite(const std::string&);

  // *WARNING*: result needs to be delete()d !
  static RPG_Engine_Player_XMLTree_Type* playerXMLToEntityXML(const RPG_Player_PlayerXML_XMLTree_Type&);
};

#endif
