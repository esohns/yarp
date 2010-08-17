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
#ifndef RPG_CHARACTER_COMMON_H
#define RPG_CHARACTER_COMMON_H

#include "rpg_character_metaclass.h"

#include <rpg_common_subclass.h>
#include <rpg_common_condition.h>

#include <vector>
#include <set>

// some useful types
typedef std::set<RPG_Common_SubClass> RPG_Character_SubClasses_t;
typedef RPG_Character_SubClasses_t::const_iterator RPG_Character_SubClassesIterator_t;

struct RPG_Character_Class
{
  RPG_Character_MetaClass metaClass;
  RPG_Character_SubClasses_t subClasses;
};

typedef std::vector<signed char> RPG_Character_BaseAttackBonus_t;
typedef RPG_Character_BaseAttackBonus_t::iterator RPG_Character_BaseAttackBonusIterator_t;
typedef RPG_Character_BaseAttackBonus_t::const_iterator RPG_Character_BaseAttackBonusConstIterator_t;

typedef std::set<RPG_Common_Condition> RPG_Character_Conditions_t;
typedef RPG_Character_Conditions_t::const_iterator RPG_Character_ConditionsIterator_t;

#endif
