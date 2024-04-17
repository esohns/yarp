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

#include <deque>
#include <string>
#include <vector>

#include "ace/Global_Macros.h"

#include "rpg_common_subclass.h"

#include "rpg_character_class_common.h"

#include "rpg_item_instance_common.h"

#include "rpg_player_common.h"
#include "rpg_player_XML_tree.h"

#include "rpg_monster_common.h"

#include "rpg_combat_attacksituation.h"
#include "rpg_combat_defensesituation.h"

#include "rpg_map_common.h"

#include "rpg_engine_common.h"
#include "rpg_engine_defines.h"
#include "rpg_engine_event_common.h"
#include "rpg_engine_level.h"
#include "rpg_engine_XML_tree.h"

// forward declarations
class RPG_Player_Base;

class RPG_Engine_Common_Tools
{
 public:
  // *NOTE*: initializes (static) data from ALL individual modules
  // in particular: - randomization
  //                - string conversion facilities
  //                - static tables (skills, spells, experience, ...)
  //                - XML parsing facilities
  //                - dictionaries (spells, items, monsters, ...)
  static void initialize (const std::vector<std::string>&, // schema directories
                          const std::string&,              // magic dictionary file
                          const std::string&,              // item dictionary file
                          const std::string&);             // monster dictionary file
  static void finalize ();

	// ***** event-related *****
	static bool isOneShotEvent (enum RPG_Engine_EventType);

  // ***** state-related *****
  static std::string getEngineStateDirectory ();

  // ***** entity-related *****
  static struct RPG_Engine_Entity createEntity ();
  static struct RPG_Engine_Entity createEntity (// base attributes
                                                const std::string&,                // creature type
                                                ACE_UINT16,                        // max HP
                                                // extended data
                                                ACE_UINT64,                        // wealth (GP)
                                                const RPG_Item_List_t&,            // list of (carried) items
                                                // current status
                                                const RPG_Character_Conditions_t&, // condition
                                                ACE_INT16,                         // HP
                                                const RPG_Magic_Spells_t&);        // set of memorized/prepared spells (if any)

  static std::string info (const struct RPG_Engine_Entity&); // entity
  static RPG_Item_List_t generateStandardItems (enum RPG_Common_SubClass);

  // ***** combat-related *****
  static unsigned int range (const RPG_Map_Position_t&,  // A
                             const RPG_Map_Position_t&); // B
  static bool isCharacterHelpless (const RPG_Player_Base* const); // character handle
  static bool isCharacterDisabled (const RPG_Player_Base* const); // character handle
  static bool isPartyHelpless (const RPG_Player_Party_t&); // party
  static bool areMonstersHelpless (const RPG_Monster_Groups_t&); // monsters
  static void getCombatantSequence (const RPG_Player_Party_t&,        // party
                                    const RPG_Monster_Groups_t&,      // monsters
                                    RPG_Engine_CombatantSequence_t&); // battle sequence
  static void performCombatRound (enum RPG_Combat_AttackSituation,        // attack situation
                                  enum RPG_Combat_DefenseSituation,       // defense situation
                                  const RPG_Engine_CombatantSequence_t&); // battle sequence
  static void attack (const RPG_Player_Base*,                            // attacker
                      RPG_Player_Base*,                                  // defender
                      ACE_UINT32&,                                       // return value: damage HP
                      enum RPG_Combat_AttackSituation = ATTACK_NORMAL,   // attack situation
                      enum RPG_Combat_DefenseSituation = DEFENSE_NORMAL, // defense situation
                      bool = true,                                       // full-round action ?
                      ACE_UINT16 = RPG_ENGINE_FEET_PER_SQUARE);          // distance (feet)
	static unsigned int partyToACL (const RPG_Player_Party_t&);
  static ACE_UINT64 combatToXP (const std::string&, // type
                                unsigned int,       // ACL (average character level)
                                unsigned int = 1,   // number of foes
                                ACE_UINT8 = 1);     // number of party members

 private:
  ACE_UNIMPLEMENTED_FUNC (RPG_Engine_Common_Tools ())
  ACE_UNIMPLEMENTED_FUNC (~RPG_Engine_Common_Tools ())
  ACE_UNIMPLEMENTED_FUNC (RPG_Engine_Common_Tools (const RPG_Engine_Common_Tools&))
  ACE_UNIMPLEMENTED_FUNC (RPG_Engine_Common_Tools& operator= (const RPG_Engine_Common_Tools&))

  // helper types
  typedef std::deque<struct RPG_Engine_CombatantSequenceElement> RPG_Engine_CombatSequenceList_t;
  typedef RPG_Engine_CombatSequenceList_t::iterator RPG_Engine_CombatSequenceListIterator_t;

  // helper methods
  static void initializeCRToExperienceMap ();
  static bool isMonsterGroupHelpless (const RPG_Monster_Group_t&); // group instance
  static bool isValidFoeAvailable (bool,                                   // monsters ? : players
                                   const RPG_Engine_CombatantSequence_t&); // battle sequence

  static unsigned int numCompatibleMonsterAttackActions (enum RPG_Combat_AttackForm,
                                                         const RPG_Monster_AttackActions_t&);
  static bool isCompatibleMonsterAttackAction (enum RPG_Combat_AttackForm,
                                               const RPG_Monster_AttackAction&);

//  // *WARNING*: result needs to be delete()d !
//  static RPG_Engine_EntityState_XMLTree_Type* playerXMLToEntityStateXML(const RPG_Player_PlayerXML_XMLTree_Type&);

  static RPG_Engine_CRToExperienceMap_t myCRToExperienceMap;
};

#endif
