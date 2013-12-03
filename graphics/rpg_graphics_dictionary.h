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

#ifndef RPG_GRAPHICS_DICTIONARY_H
#define RPG_GRAPHICS_DICTIONARY_H

#include "rpg_graphics_exports.h"
#include "rpg_graphics_common.h"

#include <ace/Global_Macros.h>
#include <ace/Singleton.h>
#include <ace/Synch.h>

#include <string>

/**
@author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Graphics_Export RPG_Graphics_Dictionary
{
  // we use the singleton pattern, so we need to enable access to the ctor/dtors
  friend class ACE_Singleton<RPG_Graphics_Dictionary,
                             ACE_Thread_Mutex>;

 public:
  void init(const std::string&,   // (XML) dictionary file
            const bool& = false); // validate XML ?

  const RPG_Graphics_t get(const RPG_Graphics_GraphicTypeUnion&) const; // type
  RPG_Graphics_Fonts_t getFonts() const;

  // debug info
  void dump() const;

 private:
  // safety measures
  RPG_Graphics_Dictionary();
  virtual ~RPG_Graphics_Dictionary();
  ACE_UNIMPLEMENTED_FUNC(RPG_Graphics_Dictionary(const RPG_Graphics_Dictionary&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Graphics_Dictionary& operator=(const RPG_Graphics_Dictionary&));

  RPG_Graphics_Dictionary_t myDictionary;
};

typedef ACE_Singleton<RPG_Graphics_Dictionary,
                      ACE_Thread_Mutex> RPG_GRAPHICS_DICTIONARY_SINGLETON;
RPG_GRAPHICS_SINGLETON_DECLARE(ACE_Singleton, RPG_Graphics_Dictionary, ACE_Thread_Mutex);

#endif
