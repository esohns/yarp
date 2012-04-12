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
#include "stdafx.h"

#include "rpg_player_inventory.h"

#include <rpg_item_base.h>
#include <rpg_item_dictionary.h>
#include <rpg_item_instance_manager.h>

#include <rpg_common_macros.h>

#include <ace/Log_Msg.h>

RPG_Player_Inventory::RPG_Player_Inventory(const RPG_Item_List_t& items_in)
 : myItems(items_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Player_Inventory::RPG_Player_Inventory"));

  // increase item references
  RPG_Item_Base* item = NULL;
  RPG_Item_Instance_Base* instance = NULL;
  for (RPG_Item_ListIterator_t iterator = myItems.begin();
       iterator != myItems.end();
       iterator++)
  {
    // retrieve item handle
    if (!RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->get(*iterator,
                                                              item))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid item (ID: %d), continuing\n"),
                 *iterator));

      continue;
    } // end IF
    ACE_ASSERT(item);

    // retrieve instance handle
    instance = dynamic_cast<RPG_Item_Instance_Base*>(item);
    ACE_ASSERT(instance);
    instance->increase();
  } // end FOR
}

RPG_Player_Inventory::RPG_Player_Inventory(const RPG_Player_Inventory& inventory_in)
 : myItems(inventory_in.myItems)
{
  RPG_TRACE(ACE_TEXT("RPG_Player_Inventory::RPG_Player_Inventory"));

  // increase item references
  RPG_Item_Base* item = NULL;
  RPG_Item_Instance_Base* instance = NULL;
  for (RPG_Item_ListIterator_t iterator = myItems.begin();
       iterator != myItems.end();
       iterator++)
  {
    // retrieve item handle
    if (!RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->get(*iterator,
                                                              item))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid item (ID: %d), continuing\n"),
                 *iterator));

      continue;
    } // end IF
    ACE_ASSERT(item);

    // retrieve instance handle
    instance = dynamic_cast<RPG_Item_Instance_Base*>(item);
    ACE_ASSERT(instance);
    instance->increase();
  } // end FOR
}

RPG_Player_Inventory::~RPG_Player_Inventory()
{
  RPG_TRACE(ACE_TEXT("RPG_Player_Inventory::~RPG_Player_Inventory"));

  // remove item references
  RPG_Item_Base* item = NULL;
  RPG_Item_Instance_Base* instance = NULL;
  for (RPG_Item_ListIterator_t iterator = myItems.begin();
       iterator != myItems.end();
       iterator++)
  {
    // retrieve item handle
    if (!RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->get(*iterator,
                                                              item))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid item (ID: %d), continuing\n"),
                 *iterator));

      continue;
    } // end IF
    ACE_ASSERT(item);

    // retrieve instance handle
    instance = dynamic_cast<RPG_Item_Instance_Base*>(item);
    ACE_ASSERT(instance);
    instance->decrease();
  } // end FOR
}

RPG_Player_Inventory&
RPG_Player_Inventory::operator=(const RPG_Player_Inventory& inventory_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Player_Inventory::operator="));

  // remove item references
  RPG_Item_Base* item = NULL;
  RPG_Item_Instance_Base* instance = NULL;
  for (RPG_Item_ListIterator_t iterator = myItems.begin();
       iterator != myItems.end();
       iterator++)
  {
    // retrieve item handle
    if (!RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->get(*iterator,
                                                              item))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid item (ID: %d), continuing\n"),
                 *iterator));

      continue;
    } // end IF
    ACE_ASSERT(item);

    // retrieve instance handle
    instance = dynamic_cast<RPG_Item_Instance_Base*>(item);
    ACE_ASSERT(instance);
    instance->decrease();
  } // end FOR

  myItems = inventory_in.myItems;

  // increase item references
  for (RPG_Item_ListIterator_t iterator = myItems.begin();
       iterator != myItems.end();
       iterator++)
  {
    // retrieve item handle
    if (!RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->get(*iterator,
                                                              item))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid item (ID: %d), continuing\n"),
                 *iterator));

      continue;
    } // end IF
    ACE_ASSERT(item);

    // retrieve instance handle
    instance = dynamic_cast<RPG_Item_Instance_Base*>(item);
    ACE_ASSERT(instance);
    instance->increase();
  } // end FOR

  return *this;
}

void
RPG_Player_Inventory::pickUp(const RPG_Item_ID_t& itemID_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Player_Inventory::pickUp"));

  // increase item reference
  RPG_Item_Base* item = NULL;
  RPG_Item_Instance_Base* instance = NULL;
  // retrieve item handle
  if (!RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->get(itemID_in,
                                                            item))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid item (ID: %d), aborting\n"),
               itemID_in));

    return;
  } // end IF
  ACE_ASSERT(item);
  // retrieve instance handle
  instance = dynamic_cast<RPG_Item_Instance_Base*>(item);
  ACE_ASSERT(instance);
  instance->increase();

  myItems.insert(itemID_in);
}

void
RPG_Player_Inventory::drop(const RPG_Item_ID_t& itemID_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Player_Inventory::drop"));

  // sanity check
  if (myItems.find(itemID_in) == myItems.end())
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid item (ID: %d), aborting\n"),
               itemID_in));

    return;
  } // end IF

  // decrease item reference
  RPG_Item_Base* item = NULL;
  RPG_Item_Instance_Base* instance = NULL;
  // retrieve item handle
  if (!RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->get(itemID_in,
                                                            item))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid item (ID: %d), aborting\n"),
               itemID_in));

    return;
  } // end IF
  ACE_ASSERT(item);
  // retrieve instance handle
  instance = dynamic_cast<RPG_Item_Instance_Base*>(item);
  ACE_ASSERT(instance);
  instance->decrease();

  myItems.erase(itemID_in);
}

unsigned short
RPG_Player_Inventory::getTotalWeight() const
{
  RPG_TRACE(ACE_TEXT("RPG_Player_Inventory::getTotalWeight"));

  unsigned short result = 0;

  RPG_Item_Base* base = NULL;
  for (RPG_Item_ListIterator_t iterator = myItems.begin();
       iterator != myItems.end();
       iterator++)
  {
    if (!RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->get(*iterator,
                                                              base))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid item (ID: %d), continuing\n"),
                 *iterator));

      continue;
    } // end IF
    ACE_ASSERT(base);

    const RPG_Item_PropertiesBase& properties = RPG_ITEM_DICTIONARY_SINGLETON::instance()->getProperties(base);
    result += properties.baseWeight;
  } // end FOR

  return result;
}

void
RPG_Player_Inventory::dump() const
{
  RPG_TRACE(ACE_TEXT("RPG_Player_Inventory::dump"));

  RPG_Item_Base* base = NULL;
  for (RPG_Item_ListIterator_t iterator = myItems.begin();
       iterator != myItems.end();
       iterator++)
  {
    if (!RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->get(*iterator,
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
