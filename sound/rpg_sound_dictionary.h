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

#ifndef RPG_SOUND_DICTIONARY_H
#define RPG_SOUND_DICTIONARY_H

#include <string>

#include "ace/Global_Macros.h"
#include "ace/Singleton.h"
#include "ace/Synch_Traits.h"

#include "rpg_sound_common.h"
#include "rpg_sound_event.h"

class RPG_Sound_Dictionary
{
  // we use the singleton pattern, so we need to enable access to the ctor/dtors
  friend class ACE_Singleton<RPG_Sound_Dictionary,
                             ACE_SYNCH_MUTEX>;

 public:
  // initialize sound dictionary
  bool initialize (const std::string&, // filename
                   bool = false);      // validate XML ?

  RPG_Sound_t get (enum RPG_Sound_Event) const; // event

  // debug info
  void dump () const;

 private:
  RPG_Sound_Dictionary ();
  inline virtual ~RPG_Sound_Dictionary () {}
  ACE_UNIMPLEMENTED_FUNC (RPG_Sound_Dictionary (const RPG_Sound_Dictionary&))
  ACE_UNIMPLEMENTED_FUNC (RPG_Sound_Dictionary& operator= (const RPG_Sound_Dictionary&))

  RPG_Sound_Dictionary_t myDictionary;
};

typedef ACE_Singleton<RPG_Sound_Dictionary,
                      ACE_SYNCH_MUTEX> RPG_SOUND_DICTIONARY_SINGLETON;
//RPG_SOUND_SINGLETON_DECLARE(ACE_Singleton, RPG_Sound_Dictionary, ACE_Thread_Mutex);

#endif
