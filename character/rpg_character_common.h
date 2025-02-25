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

#include <set>
#include <vector>

#include "ace/Basic_Types.h"

#include "rpg_common_condition.h"

#include "rpg_character_equipmentslot.h"

typedef ACE_UINT8 RPG_Character_Level_t;

typedef std::set<enum RPG_Common_Condition> RPG_Character_Conditions_t;
typedef RPG_Character_Conditions_t::const_iterator RPG_Character_ConditionsIterator_t;

typedef std::vector<ACE_INT8> RPG_Character_BaseAttackBonus_t;
typedef RPG_Character_BaseAttackBonus_t::iterator RPG_Character_BaseAttackBonusIterator_t;
typedef RPG_Character_BaseAttackBonus_t::const_iterator RPG_Character_BaseAttackBonusConstIterator_t;

typedef std::vector<enum RPG_Character_EquipmentSlot> RPG_Character_EquipmentSlots_t;
typedef RPG_Character_EquipmentSlots_t::const_iterator RPG_Character_EquipmentSlotsIterator_t;
struct RPG_Character_EquipmentSlots
{
  RPG_Character_EquipmentSlots_t slots;
  bool is_inclusive; // logic AND ? : OR
};

#endif
