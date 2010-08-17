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
#ifndef RPG_MONSTER_DICTIONARY_H
#define RPG_MONSTER_DICTIONARY_H

#include "rpg_monster_common.h"

#include "rpg_XMLSchema_XML_types.h"

#include <rpg_character_alignment.h>
#include <rpg_common_environment.h>

#include <rpg_dice_dietype.h>
#include <rpg_dice_roll.h>

#include <ace/Global_Macros.h>
#include <ace/Singleton.h>
#include <ace/Synch.h>

#include <string>

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Monster_Dictionary
{
  // we use the singleton pattern, so we need to enable access to the ctor/dtors
  friend class ACE_Singleton<RPG_Monster_Dictionary,
                             ACE_Thread_Mutex>;

 public:
  // init dictionary
  void init(const std::string&,  // (XML) dictionary filename
            const bool& = true); // validate XML ?
  const RPG_Monster_Properties getProperties(const std::string&) const; // name of monster
  const unsigned long numEntries() const;
  void find(const RPG_Character_Alignment&,       // alignment
            const RPG_Common_Environment&,        // environment
            const RPG_Monster_OrganizationSet_t&, // organization(s)
            RPG_Monster_List_t&) const;           // return value: compatible types

  // debug info
  void dump() const;

 private:
  // safety measures
  RPG_Monster_Dictionary();
  virtual ~RPG_Monster_Dictionary();
  ACE_UNIMPLEMENTED_FUNC(RPG_Monster_Dictionary(const RPG_Monster_Dictionary&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Monster_Dictionary& operator=(const RPG_Monster_Dictionary&));

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

  // helper types
  typedef RPG_Monster_Dictionary_t::const_iterator RPG_Monster_DictionaryConstIterator_t;

  RPG_Monster_Dictionary_t myMonsterDictionary;
  XSD_Error_Handler        myXSDErrorHandler;
};

typedef ACE_Singleton<RPG_Monster_Dictionary,
                      ACE_Thread_Mutex> RPG_MONSTER_DICTIONARY_SINGLETON;

#endif
