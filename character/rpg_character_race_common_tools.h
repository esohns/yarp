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

#ifndef RPG_CHARACTER_RACE_COMMON_TOOLS_H
#define RPG_CHARACTER_RACE_COMMON_TOOLS_H

#include "rpg_character_exports.h"
#include "rpg_character_race_common.h"
#include "rpg_character_race.h"

#include <rpg_common_size.h>

#include <ace/Global_Macros.h>

class RPG_Character_Export RPG_Character_Race_Common_Tools
{
 public:
  static RPG_Common_Size race2Size(const RPG_Character_Race_t&); // (player) race(s)
  static unsigned char race2Speed(const RPG_Character_Race&);
  static bool isCompatible(const RPG_Character_Race&,  // race 1
                           const RPG_Character_Race&); // race 2

  static bool hasRace(const RPG_Character_Race_t&, // (player) race(s)
                      const RPG_Character_Race&);  // specific race

  private:
  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_Race_Common_Tools());
  ACE_UNIMPLEMENTED_FUNC(~RPG_Character_Race_Common_Tools());
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_Race_Common_Tools(const RPG_Character_Race_Common_Tools&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_Race_Common_Tools& operator=(const RPG_Character_Race_Common_Tools&));

  // helper method(s)
  static RPG_Common_Size race2Size(const RPG_Character_Race&); // race
};

#endif
