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
#include "rpg_character_inventory.h"

#include <rpg_item_base.h>
#include <rpg_item_instance_manager.h>

#include <ace/Log_Msg.h>

RPG_Character_Inventory::RPG_Character_Inventory(const RPG_Item_List_t& items_in)
 : myItems(items_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Inventory::RPG_Character_Inventory"));

}

RPG_Character_Inventory::RPG_Character_Inventory(const RPG_Character_Inventory& inventory_in)
 : myItems(inventory_in.myItems)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Inventory::RPG_Character_Inventory"));

}

RPG_Character_Inventory::~RPG_Character_Inventory()
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Inventory::~RPG_Character_Inventory"));

}

RPG_Character_Inventory& RPG_Character_Inventory::operator=(const RPG_Character_Inventory& inventory_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Inventory::operator="));

  myItems = inventory_in.myItems;

  return *this;
}

void RPG_Character_Inventory::pickUp(const RPG_Item_ID_t& itemID_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Inventory::pickUp"));

  myItems.insert(itemID_in);
}

void RPG_Character_Inventory::drop(const RPG_Item_ID_t& itemID_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Inventory::drop"));

  myItems.erase(itemID_in);
}

void RPG_Character_Inventory::dump() const
{
  ACE_TRACE(ACE_TEXT("RPG_Character_Inventory::dump"));

  RPG_Item_Base* base = NULL;
  for (RPG_Item_ListIterator_t iterator = myItems.begin();
       iterator != myItems.end();
       iterator++)
  {
    if (!RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->getItem(*iterator,
                                                                  base))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid item (ID: %d), continuing\n"),
                 *iterator));

      continue;
    } // end IF

    base->dump();
  } // end FOR
}
