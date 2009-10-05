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
#ifndef RPG_CHARACTER_EQUIPMENT_COMMON_H
#define RPG_CHARACTER_EQUIPMENT_COMMON_H

#include <map>

enum RPG_Character_Equipment_Slot
{
  EQUIPMENT_SLOT_HEAD,
  EQUIPMENT_SLOT_NECK,
  EQUIPMENT_SLOT_APPAREL,
  EQUIPMENT_SLOT_RIGHT_HAND,
  EQUIPMENT_SLOT_LEFT_HAND,
  EQUIPMENT_SLOT_GAUNTLETS,
  EQUIPMENT_SLOT_RING,
  EQUIPMENT_SLOT_FEET
};

// useful types
typedef std::multimap<RPG_Character_Equipment_Slot, unsigned int> RPG_CHARACTER_EQUIPMENT_T;
typedef RPG_CHARACTER_EQUIPMENT_T::const_iterator RPG_CHARACTER_EQUIPMENT_ITERATOR_T;

#endif
