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

#include "rpg_item_instance_manager.h"

#include "rpg_item_armortype.h"
#include "rpg_item_commoditytype.h"
#include "rpg_item_commoditybeverage.h"
#include "rpg_item_commoditylight.h"
#include "rpg_item_weapontype.h"
#include "rpg_item_armor.h"
#include "rpg_item_commodity.h"
#include "rpg_item_weapon.h"
#include "rpg_item_common_tools.h"

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
      RPG_Item_ArmorType armorType = static_cast<RPG_Item_ArmorType>(itemSubType_in);
      handle = new(std::nothrow) RPG_Item_Armor(armorType);
      if (!handle)
      {
        ACE_DEBUG((LM_CRITICAL,
                   ACE_TEXT("failed to allocate memory, aborting\n")));

        return NULL;
      } // end IF

      break;
    }
    case ITEM_COMMODITY:
    {
      // half MSBs are subtype
      RPG_Item_CommodityType commodityType = static_cast<RPG_Item_CommodityType>(itemSubType_in >> (sizeof(unsigned int) * 4));
      RPG_Item_CommodityUnion commoditySubType;
      // half LSBs are subtype
      unsigned int mask = (std::numeric_limits<unsigned int>::max() >> (sizeof(unsigned int) * 4));
      switch (commodityType)
      {
        case COMMODITY_BEVERAGE:
        {
          commoditySubType.discriminator = RPG_Item_CommodityUnion::COMMODITYBEVERAGE;
          commoditySubType.commoditybeverage = static_cast<RPG_Item_CommodityBeverage>(itemSubType_in & mask);

          break;
        }
        case COMMODITY_FOOD:
        {
          // *TODO*: create these as well
          ACE_ASSERT(false);

          break;
        }
        case COMMODITY_LIGHT:
        {
          commoditySubType.discriminator = RPG_Item_CommodityUnion::COMMODITYLIGHT;
          commoditySubType.commoditylight = static_cast<RPG_Item_CommodityLight>(itemSubType_in & mask);

          break;
        }
        case COMMODITY_OTHER:
        {
          // *TODO*: create these as well
          ACE_ASSERT(false);

          break;
        }
        default:
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("invalid commodity type (was: \"%s\"), aborting\n"),
                     RPG_Item_CommodityTypeHelper::RPG_Item_CommodityTypeToString(commodityType).c_str()));

          break;
        }
      } // end SWITCH

      handle = new(std::nothrow) RPG_Item_Commodity(commodityType,
                                                    commoditySubType);
      if (!handle)
      {
        ACE_DEBUG((LM_CRITICAL,
                   ACE_TEXT("failed to allocate memory, aborting\n")));

        return NULL;
      } // end IF

      break;
    }
    case ITEM_OTHER:
    case ITEM_VALUABLE:
    {
      // *TODO*: create these as well
      ACE_ASSERT(false);

      break;
    }
    case ITEM_WEAPON:
    {
      RPG_Item_WeaponType weaponType = static_cast<RPG_Item_WeaponType>(itemSubType_in);
      handle = new(std::nothrow) RPG_Item_Weapon(weaponType);
      if (!handle)
      {
        ACE_DEBUG((LM_CRITICAL,
                   ACE_TEXT("failed to allocate memory, aborting\n")));

        return NULL;
      } // end IF

      break;
    }
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

bool
RPG_Item_Instance_Manager::get(const RPG_Item_ID_t& itemID_in,
                               RPG_Item_Base*& handle_out) const
{
  RPG_TRACE(ACE_TEXT("RPG_Item_Instance_Manager::get"));

  // init return value(s)
  handle_out = NULL;

  {
    ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

    RPG_Item_InstanceTableConstIterator_t iterator = myInstanceTable.find(itemID_in);
    ACE_ASSERT(iterator != myInstanceTable.end());
    if (iterator == myInstanceTable.end())
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid item id (was: %u), aborting\n"),
                 itemID_in));

      return false;
    } // end IF

    handle_out = iterator->second;
  } // end lock scope

  return true;
}

void
RPG_Item_Instance_Manager::registerItem(const RPG_Item_ID_t& itemID_in,
                                        RPG_Item_Base* handle_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Item_Instance_Manager::registerItem"));

  // sanity check(s)
  ACE_ASSERT(handle_in);

  ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

  myInstanceTable.insert(std::make_pair(itemID_in, handle_in));
}

void
RPG_Item_Instance_Manager::deregisterItem(RPG_Item_Base* handle_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Item_Instance_Manager::deregisterItem"));

  // sanity check(s)
  ACE_ASSERT(handle_in);

  ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);
  ACE_ASSERT(!myInstanceTable.empty());
  
  // need to iterate...
  RPG_Item_InstanceTableIterator_t iterator = myInstanceTable.begin();
  do
  {
    if ((*iterator).second == handle_in)
    {
      myInstanceTable.erase(iterator);

      // done
      return;
    } // end IF

    iterator++;
  } while (iterator != myInstanceTable.end());

  // debug info
  RPG_Item_Instance_Base* instance_handle = dynamic_cast<RPG_Item_Instance_Base*>(handle_in);
  ACE_ASSERT(instance_handle);
  ACE_DEBUG((LM_ERROR,
             ACE_TEXT("item (ID: %u) not found, returning\n"),
             instance_handle->getID()));
}
