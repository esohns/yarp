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
#ifndef RPG_COMBAT_DAMAGE_H
#define RPG_COMBAT_DAMAGE_H

#include <rpg_character_attribute.h>
#include <rpg_character_condition.h>

#include <rpg_dice_dietype.h>
#include <rpg_dice_roll.h>

typedef RPG_Dice_Roll RPG_Combat_PhysicalDamage;

struct RPG_Combat_Damage_AbilityDrained
{
  RPG_Character_Attribute attribute;
  char                    modifier;
};

union RPG_Combat_Damage
{
  RPG_Combat_PhysicalDamage        physical;
  RPG_Character_Condition          condition;
  RPG_Combat_Damage_AbilityDrained drain;
};

#endif
