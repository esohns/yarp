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

#include <string>

#include "ace/Global_Macros.h"
#include "ace/Time_Value.h"
#include "ace/Signal.h"

#include "rpg_dice_incl.h"

#include "rpg_common.h"
#include "rpg_common_incl.h"
#include "rpg_common_environment_incl.h"

// forward declaration(s)
class ACE_Log_Msg_Backend;
class ACE_Event_Handler;

class RPG_Common_Tools
{
 public:
  static void initializeStringConversionTables();

  static enum RPG_Common_Attribute savingThrowToAttribute (const enum RPG_Common_SavingThrow&); // save
  static std::string toString (const struct RPG_Common_CreatureType&); // type
  static std::string toString (const struct RPG_Common_SavingThrowCheck&); // save
  static std::string toString (const struct RPG_Common_Environment&); // environment

  static enum RPG_Common_Plane terrainToPlane (const enum RPG_Common_Terrain&);
  static bool match (const struct RPG_Common_Environment&,  // a
                     const struct RPG_Common_Environment&); // b

  // *NOTE*: use for attack bonus / armor class
  static signed char getSizeModifier (const enum RPG_Common_Size&);
  // *NOTE*: use for encumbrance
  static float getSizeModifierLoad (const enum RPG_Common_Size&,
                                    bool = true); // is biped ?
  static unsigned short sizeToReach (const enum RPG_Common_Size&,
                                     bool = true); // is tall ? : long
  static unsigned short environmentToRadius (const struct RPG_Common_Environment&);
  static float terrainToSpeedModifier (const enum RPG_Common_Terrain& = TERRAIN_ANY, // terrain
                                       const enum RPG_Common_Track& = TRACK_NONE);   // track type

  // use this to "pretty-print" enumerated (i.e. XML-) values
  // e.g. "SUBCLASS_MONK" --> "Monk"
  static std::string enumToString (const std::string&, // string representation
                                   bool = true);       // chop prefix ?

  static std::string sanitizeURI (const std::string&); // URI
  static std::string sanitize (const std::string&); // string
  //static std::string strip(const std::string&); // string

 private:
  ACE_UNIMPLEMENTED_FUNC (RPG_Common_Tools ())
  ACE_UNIMPLEMENTED_FUNC (~RPG_Common_Tools ())
  ACE_UNIMPLEMENTED_FUNC (RPG_Common_Tools (const RPG_Common_Tools&))
  ACE_UNIMPLEMENTED_FUNC (RPG_Common_Tools& operator= (const RPG_Common_Tools&))
};

#endif
