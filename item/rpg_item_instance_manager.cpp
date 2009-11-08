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
#include "rpg_item_instance_manager.h"

RPG_Item_Instance_Manager::RPG_Item_Instance_Manager()
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Instance_Manager::RPG_Item_Instance_Manager"));

}

RPG_Item_Instance_Manager::~RPG_Item_Instance_Manager()
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Instance_Manager::~RPG_Item_Instance_Manager"));

}

const bool RPG_Item_Instance_Manager::getItem(const RPG_Item_ID_t& itemID_in,
                                              RPG_Item_Base*& handle_out) const
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Instance_Manager::getItem"));

  // init return value(s)
  handle_out = NULL;

  RPG_Item_InstanceTableConstIterator_t iterator = myInstanceTable.find(itemID_in);
  if (iterator != myInstanceTable.end())
  {
    handle_out = iterator->second;
  } // end IF

  return (iterator != myInstanceTable.end());
}

void RPG_Item_Instance_Manager::registerItem(const RPG_Item_ID_t& itemID_in,
                                             RPG_Item_Base* handle_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Instance_Manager::registerItem"));

  myInstanceTable.insert(std::make_pair(itemID_in, handle_in));
}

void RPG_Item_Instance_Manager::deregisterItem(RPG_Item_Base* handle_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Instance_Manager::deregisterItem"));

  // sanity check
  ACE_ASSERT(!myInstanceTable.empty());

  // need to iterate...
  RPG_Item_InstanceTableIterator_t iterator = myInstanceTable.begin();
  do
  {
    if (iterator->second == handle_in)
    {
      myInstanceTable.erase(iterator);

      // done
      return;
    } // end IF

    iterator++;
  } while (iterator != myInstanceTable.end());

  ACE_DEBUG((LM_ERROR,
             ACE_TEXT("item not found --> check implementation !, returning\n")));
}
