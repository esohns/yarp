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

#ifndef RPG_CHARACTER_CLASS_COMMON_TOOLS_H
#define RPG_CHARACTER_CLASS_COMMON_TOOLS_H

#include "rpg_character_class_common.h"
#include "rpg_character_metaclass.h"
#include "rpg_character_XML_tree.h"

#include <rpg_common_subclass.h>

class RPG_Character_Class_Common_Tools
{
 public:
  static const bool hasSubClass(const RPG_Character_Class&,  // class(es)
                                const RPG_Common_SubClass&); // subclass
  static const RPG_Character_MetaClass subClassToMetaClass(const RPG_Common_SubClass&); // subclass

  static const bool hasCasterClass(const RPG_Character_Class&); // class(es)
  static const bool hasDivineCasterClass(const RPG_Character_Class&); // class(es)
  static const bool hasArcaneCasterClass(const RPG_Character_Class&); // class(es)

  static RPG_Character_Class classXMLTreeToClass(const RPG_Character_ClassXML_XMLTree_Type&); // class (XML format)

 private:
  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_Class_Common_Tools());
  ACE_UNIMPLEMENTED_FUNC(~RPG_Character_Class_Common_Tools());
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_Class_Common_Tools(const RPG_Character_Class_Common_Tools&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_Class_Common_Tools& operator=(const RPG_Character_Class_Common_Tools&));
};

#endif
