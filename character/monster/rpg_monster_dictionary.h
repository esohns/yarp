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

#include <string>

#include "ace/Global_Macros.h"
#include "ace/Singleton.h"
#include "ace/Synch_Traits.h"

#include "rpg_dice_incl.h"

#include "rpg_common_incl.h"
#include "rpg_common_environment_incl.h"

#include "rpg_character_incl.h"

#include "rpg_monster_common.h"

class RPG_Monster_Dictionary
{
  // we use the singleton pattern, so we need to enable access to the ctor/dtors
  friend class ACE_Singleton<RPG_Monster_Dictionary,
                             ACE_SYNCH_MUTEX>;

 public:
  // init dictionary
  bool initialize (const std::string&, // (XML) dictionary filename
                   bool = false);      // validate XML ?

  RPG_Monster_Properties getProperties (const std::string&) const; // name of monster
  inline unsigned int numEntries () const { return static_cast<unsigned int> (myMonsterDictionary.size ()); }
  RPG_Monster_List_t getEntries () const;
  void find (const struct RPG_Character_Alignment&, // alignment
             const struct RPG_Common_Environment&,  // environment
             const RPG_Monster_OrganizationSet_t&,  // organization(s)
             const RPG_Monster_HitDice&,            // HD
             RPG_Monster_List_t&) const;            // return value: compatible types

  void dump () const;

 private:
  RPG_Monster_Dictionary ();
  inline virtual ~RPG_Monster_Dictionary () {}
  ACE_UNIMPLEMENTED_FUNC (RPG_Monster_Dictionary (const RPG_Monster_Dictionary&))
  ACE_UNIMPLEMENTED_FUNC (RPG_Monster_Dictionary& operator= (const RPG_Monster_Dictionary&))

  // helper types
  typedef RPG_Monster_Dictionary_t::const_iterator RPG_Monster_DictionaryConstIterator_t;

  RPG_Monster_Dictionary_t myMonsterDictionary;
};

typedef ACE_Singleton<RPG_Monster_Dictionary,
                      ACE_SYNCH_MUTEX> RPG_MONSTER_DICTIONARY_SINGLETON;
//RPG_MONSTER_SINGLETON_DECLARE(ACE_Singleton,
//                              RPG_Monster_Dictionary,
//                              ACE_Thread_Mutex)

#endif
