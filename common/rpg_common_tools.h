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

#ifndef RPG_COMMON_TOOLS_H
#define RPG_COMMON_TOOLS_H

#include <rpg_dice_incl.h>
#include "rpg_common_incl.h"
#include "rpg_common_environment_incl.h"

#include "rpg_common_exports.h"

#include <ace/Global_Macros.h>
#include <ace/Time_Value.h>

#include <string>

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Common_Export RPG_Common_Tools
{
 public:
  static void initStringConversionTables();

  static RPG_Common_Attribute savingThrowToAttribute(const RPG_Common_SavingThrow&); // save
  static std::string creatureTypeToString(const RPG_Common_CreatureType&); // type
  static std::string savingThrowToString(const RPG_Common_SavingThrowCheck&); // save
  static std::string environmentToString(const RPG_Common_Environment&); // environment

  static RPG_Common_Plane terrainToPlane(const RPG_Common_Terrain&);
  static bool match(const RPG_Common_Environment&,  // a
                    const RPG_Common_Environment&); // b

  // *NOTE*: use for attack bonus / armor class
  static signed char getSizeModifier(const RPG_Common_Size&);
  // *NOTE*: use for encumbrance
  static float getSizeModifierLoad(const RPG_Common_Size&,
                                   const bool& = true); // is biped ?
  static unsigned char sizeToReach(const RPG_Common_Size&);
  static unsigned char environment2Radius(const RPG_Common_Environment&);

  // use this to "pretty-print" enumerated (i.e. XML-) values
  // e.g. "SUBCLASS_MONK" --> "Monk"
  static std::string enumToString(const std::string&,  // string representation
                                  const bool& = true); // chop prefix ?

  // use this to generate a "condensed" period string
  // - uses snprintf internally: "%H:%M:%S.usec"
  static bool period2String(const ACE_Time_Value&, // period
                            std::string&);         // return value: corresp. string

  static bool isLinux();

  static bool getUserName(std::string&,  // return value: username
                          std::string&); // return value: "real" name
  static std::string getHostName(); // return value: hostname

 private:
  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Common_Tools());
  ACE_UNIMPLEMENTED_FUNC(~RPG_Common_Tools());
  ACE_UNIMPLEMENTED_FUNC(RPG_Common_Tools(const RPG_Common_Tools&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Common_Tools& operator=(const RPG_Common_Tools&));
};

#endif
