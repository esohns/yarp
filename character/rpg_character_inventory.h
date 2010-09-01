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
#ifndef RPG_CHARACTER_INVENTORY_H
#define RPG_CHARACTER_INVENTORY_H

#include <rpg_item_instance_common.h>

#include <ace/Global_Macros.h>

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Character_Inventory
{
 public:
  RPG_Character_Inventory(const RPG_Item_List_t&); // list of (initial) items
  RPG_Character_Inventory(const RPG_Character_Inventory&);
  virtual ~RPG_Character_Inventory();

  RPG_Character_Inventory& operator=(const RPG_Character_Inventory&);

  void pickUp(const RPG_Item_ID_t&); // item ID
  void drop(const RPG_Item_ID_t&); // item ID

  void dump() const;

  // *WARNING*: do NOT insert/remove elements directly (use the API instead)
  RPG_Item_List_t myItems;

 private:
  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Character_Inventory());
};

#endif
