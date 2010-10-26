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

#include "rpg_item_weapontype.h"
#include "rpg_item_armortype.h"
#include "rpg_item_weapon.h"
#include "rpg_item_armor.h"

#include <rpg_common_macros.h>

RPG_Item_Instance_Manager::RPG_Item_Instance_Manager()
{
  RPG_TRACE(ACE_TEXT("RPG_Item_Instance_Manager::RPG_Item_Instance_Manager"));

}

RPG_Item_Instance_Manager::~RPG_Item_Instance_Manager()
{
  RPG_TRACE(ACE_TEXT("RPG_Item_Instance_Manager::~RPG_Item_Instance_Manager"));

}

RPG_Item_Instance_Base*
RPG_Item_Instance_Manager::create(const RPG_Item_Type& itemType_in,
                                  const unsigned int& itemSubType_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Item_Instance_Manager::create"));

  // init return value(s)
  RPG_Item_Instance_Base* handle = NULL;

  switch (itemType_in)
  {
    case ITEM_ARMOR:
    {
      RPG_Item_ArmorType armorType = static_cast<RPG_Item_ArmorType> (itemSubType_in);

      try
      {
        handle = new RPG_Item_Armor(armorType);
      }
      catch (const std::bad_alloc& exception)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("caught std::bad_alloc, aborting\n")));
      }
      if (!handle)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to allocate RPG_Item_Armor(\"%s\"), aborting\n"),
                   RPG_Item_ArmorTypeHelper::RPG_Item_ArmorTypeToString(armorType).c_str()));
      }

      break;
    }
    case ITEM_WEAPON:
    {
      RPG_Item_WeaponType weaponType = static_cast<RPG_Item_WeaponType> (itemSubType_in);

      try
      {
        handle = new RPG_Item_Weapon(weaponType);
      }
      catch (const std::bad_alloc& exception)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("caught std::bad_alloc, aborting\n")));
      }
      if (!handle)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to allocate RPG_Item_Weapon(\"%s\"), aborting\n"),
                   RPG_Item_WeaponTypeHelper::RPG_Item_WeaponTypeToString(weaponType).c_str()));
      }

      break;
    }
    // *TODO*: create these as well
    case ITEM_GOODS:
    case ITEM_OTHER:
    case ITEM_VALUABLE:
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid item type (was: \"%s\"), aborting\n"),
                 RPG_Item_TypeHelper::RPG_Item_TypeToString(itemType_in).c_str()));

      break;
    }
  } // end SWITCH

  return handle;
}

const bool
RPG_Item_Instance_Manager::get(const RPG_Item_ID_t& itemID_in,
                               RPG_Item_Base*& handle_out) const
{
  RPG_TRACE(ACE_TEXT("RPG_Item_Instance_Manager::get"));

  // init return value(s)
  handle_out = NULL;

  RPG_Item_InstanceTableConstIterator_t iterator = myInstanceTable.find(itemID_in);
  if (iterator != myInstanceTable.end())
  {
    handle_out = iterator->second;
  } // end IF

  return (iterator != myInstanceTable.end());
}

void
RPG_Item_Instance_Manager::registerItem(const RPG_Item_ID_t& itemID_in,
                                        RPG_Item_Base* handle_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Item_Instance_Manager::registerItem"));

  // sanity check(s)
  ACE_ASSERT(handle_in);

  myInstanceTable.insert(std::make_pair(itemID_in, handle_in));
}

void
RPG_Item_Instance_Manager::deregisterItem(RPG_Item_Base* handle_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Item_Instance_Manager::deregisterItem"));

  // sanity check(s)
  ACE_ASSERT(handle_in);
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

  // debug info
  RPG_Item_Instance_Base* instance_handle = dynamic_cast<RPG_Item_Instance_Base*> (handle_in);
  ACE_ASSERT(instance_handle);
  ACE_DEBUG((LM_ERROR,
             ACE_TEXT("item (ID: %u) not found, returning\n"),
             instance_handle->getID()));
}
