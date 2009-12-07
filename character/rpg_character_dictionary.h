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
#ifndef RPG_CHARACTER_DICTIONARY_H
#define RPG_CHARACTER_DICTIONARY_H

#include "rpg_character_monster_common.h"
#include "rpg_XMLSchema.h"

#include <rpg_chance_dicetype.h>
#include <rpg_chance_diceroll.h>

#include <ace/Global_Macros.h>
#include <ace/Singleton.h>
#include <ace/Synch.h>

#include <string>

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Character_Dictionary
{
  // we use the singleton pattern, so we need to enable access to the ctor/dtors
  friend class ACE_Singleton<RPG_Character_Dictionary,
                             ACE_Thread_Mutex>;

 public:
  // init item dictionary
  void initCharacterDictionary(const std::string&); // filename
  const RPG_Character_MonsterProperties getMonsterProperties(const std::string&) const; // name of monster
  void generateRandomEncounter(const unsigned int&,                     // # of different monster types
                               const RPG_Character_Alignment&,          // alignment
                               const RPG_Character_Environment&,        // environment
                               const RPG_Character_OrganizationList_t&, // allowed organizations
                               RPG_Character_Encounter_t&) const;       // return value: encounter

  // debug info
  void dump() const;

 private:
  // safety measures
  RPG_Character_Dictionary();
  virtual ~RPG_Character_Dictionary();
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_Dictionary(const RPG_Character_Dictionary&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_Dictionary& operator=(const RPG_Character_Dictionary&));

  // private error handler
  class XSD_Error_Handler
   : public ::xml_schema::error_handler
  {
   public:
    virtual bool handle(const std::string&,                    // id
                        unsigned long,                         // line
                        unsigned long,                         // column
                        ::xml_schema::error_handler::severity, // severity
                        const std::string&);                   // message
  };

  // helper methods
  void organizationStepToRoll(const RPG_Character_OrganizationStep&, // organization step
                              RPG_Chance_DiceRoll&) const;           // return value: roll
  const bool environmentMatches(const RPG_Character_Environment&,        // a
                                const RPG_Character_Environment&) const; // b
  const bool alignmentMatches(const RPG_Character_Alignment&,        // a
                              const RPG_Character_Alignment&) const; // b

  RPG_Character_MonsterDictionary_t myMonsterDictionary;
  XSD_Error_Handler                 myXSDErrorHandler;
};

typedef ACE_Singleton<RPG_Character_Dictionary,
                      ACE_Thread_Mutex> RPG_CHARACTER_DICTIONARY_SINGLETON;

#endif
