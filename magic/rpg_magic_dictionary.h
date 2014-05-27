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

#ifndef RPG_MAGIC_DICTIONARY_H
#define RPG_MAGIC_DICTIONARY_H

#include "rpg_dice_incl.h"
#include "rpg_common_incl.h"
#include "rpg_common_environment_incl.h"
#include "rpg_character_incl.h"
#include "rpg_magic_incl.h"

#include "rpg_magic_exports.h"
#include "rpg_magic_common.h"

#include <ace/Global_Macros.h>
#include <ace/Singleton.h>
#include <ace/Synch.h>

#include <string>

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Magic_Export RPG_Magic_Dictionary
{
  // singleton requires access to the ctor/dtor
  friend class ACE_Singleton<RPG_Magic_Dictionary,
                             ACE_Thread_Mutex>;

 public:
  // init magic dictionary
  bool init(const std::string&,   // filename
            const bool& = false); // validate XML ?

  RPG_Magic_Spell_Properties getSpellProperties(const std::string&) const; // name of spell
  RPG_Magic_Spell_Properties getSpellProperties(const RPG_Magic_SpellType&, // type of spell
                                                std::string&) const;        // return value: name
  RPG_Magic_SpellTypes_t getSpells(const RPG_Magic_CasterClassUnion&,  // caster class
                                   const unsigned char& = 0xFF) const; // spell level (default: ALL)

  // debug info
  void dump(const bool& = true) const; // group Levels ?

 private:
  RPG_Magic_Dictionary();
  virtual ~RPG_Magic_Dictionary();
  ACE_UNIMPLEMENTED_FUNC(RPG_Magic_Dictionary(const RPG_Magic_Dictionary&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Magic_Dictionary& operator=(const RPG_Magic_Dictionary&));

  // helper methods
  void dumpLevels() const;

  RPG_Magic_Dictionary_t myDictionary;
};

typedef ACE_Singleton<RPG_Magic_Dictionary,
                      ACE_Thread_Mutex> RPG_MAGIC_DICTIONARY_SINGLETON;
RPG_MAGIC_SINGLETON_DECLARE(ACE_Singleton, RPG_Magic_Dictionary, ACE_Thread_Mutex);

#endif
