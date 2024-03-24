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

#include "rpg_item_armor.h"
#include "rpg_item_armortype.h"
#include "rpg_item_commodity.h"
#include "rpg_item_commoditytype.h"
#include "rpg_item_commoditybeverage.h"
#include "rpg_item_commoditylight.h"
#include "rpg_item_common_tools.h"
#include "rpg_item_weapon.h"
#include "rpg_item_weapontype.h"

#include "rpg_common_macros.h"

RPG_Item_Instance_Manager::RPG_Item_Instance_Manager ()
 : myInstanceTable ()
 , myLock ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Item_Instance_Manager::RPG_Item_Instance_Manager"));

}

RPG_Item_Instance_Base*
RPG_Item_Instance_Manager::create (enum RPG_Item_Type type_in,
                                   const unsigned int& subtype_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Item_Instance_Manager::create"));

  // init return value(s)
  RPG_Item_Instance_Base* handle = NULL;

  switch (type_in)
  {
    case ITEM_ARMOR:
    {
      enum RPG_Item_ArmorType armor_type_e =
        static_cast<enum RPG_Item_ArmorType> (subtype_in);
      ACE_NEW_RETURN (handle,
                      RPG_Item_Armor (armor_type_e),
                      NULL);
      ACE_ASSERT (handle);
      break;
    }
    case ITEM_COMMODITY:
    {
      // half MSBs are subtype
      enum RPG_Item_CommodityType commodity_type_e =
        static_cast<RPG_Item_CommodityType> (subtype_in >> (sizeof (unsigned int) * 4));
      RPG_Item_CommodityUnion commodity_subtype_u;
      // half LSBs are subtype
      unsigned int mask =
        (std::numeric_limits<unsigned int>::max () >> (sizeof (unsigned int) * 4));
      switch (commodity_type_e)
      {
        case COMMODITY_BEVERAGE:
        {
          commodity_subtype_u.discriminator = RPG_Item_CommodityUnion::COMMODITYBEVERAGE;
          commodity_subtype_u.commoditybeverage =
            static_cast<enum RPG_Item_CommodityBeverage>(subtype_in & mask);
          break;
        }
        case COMMODITY_FOOD:
        {
          // *TODO*: create these as well
          ACE_ASSERT (false);
          break;
        }
        case COMMODITY_LIGHT:
        {
          commodity_subtype_u.discriminator = RPG_Item_CommodityUnion::COMMODITYLIGHT;
          commodity_subtype_u.commoditylight =
            static_cast<enum RPG_Item_CommodityLight> (subtype_in & mask);
          break;
        }
        case COMMODITY_OTHER:
        {
          // *TODO*: create these as well
          ACE_ASSERT (false);
          break;
        }
        default:
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("invalid commodity type (was: \"%s\"), aborting\n"),
                      ACE_TEXT (RPG_Item_CommodityTypeHelper::RPG_Item_CommodityTypeToString (commodity_type_e).c_str ())));
          break;
        }
      } // end SWITCH

      ACE_NEW_RETURN (handle,
                      RPG_Item_Commodity (commodity_type_e,
                                          commodity_subtype_u),
                      NULL);
      ACE_ASSERT (handle);
      break;
    }
    case ITEM_OTHER:
    case ITEM_VALUABLE:
    {
      // *TODO*: create these as well
      ACE_ASSERT (false);
      break;
    }
    case ITEM_WEAPON:
    {
      enum RPG_Item_WeaponType weapon_type_e =
        static_cast<enum RPG_Item_WeaponType> (subtype_in);
      ACE_NEW_RETURN (handle,
                      RPG_Item_Weapon (weapon_type_e),
                      NULL);
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid item type (was: \"%s\"), aborting\n"),
                  ACE_TEXT (RPG_Item_TypeHelper::RPG_Item_TypeToString (type_in).c_str ())));
      return NULL;
    }
  } // end SWITCH
  ACE_ASSERT (handle);

  return handle;
}

bool
RPG_Item_Instance_Manager::get (RPG_Item_ID_t id_in,
                                RPG_Item_Base*& handle_out) const
{
  RPG_TRACE (ACE_TEXT ("RPG_Item_Instance_Manager::get"));

  // init return value(s)
  handle_out = NULL;

  RPG_Item_InstanceTableConstIterator_t iterator;

  { ACE_GUARD_RETURN (ACE_SYNCH_MUTEX, aGuard, myLock, false);
    iterator = myInstanceTable.find (id_in);
    //ACE_ASSERT(iterator != myInstanceTable.end());
    if (iterator == myInstanceTable.end())
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid item id (was: %u), aborting\n"),
                  id_in));
      return false;
    } // end IF

    handle_out = iterator->second;
  } // end lock scope

  return true;
}

void
RPG_Item_Instance_Manager::registerItem (RPG_Item_ID_t id_in,
                                         RPG_Item_Base* handle_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Item_Instance_Manager::registerItem"));

  // sanity check(s)
  ACE_ASSERT (handle_in);

  ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, myLock);

  myInstanceTable.insert (std::make_pair (id_in, handle_in));
}

void
RPG_Item_Instance_Manager::deregisterItem (RPG_Item_Base* handle_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Item_Instance_Manager::deregisterItem"));

  // sanity check(s)
  ACE_ASSERT (handle_in);
  ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, myLock);
  ACE_ASSERT (!myInstanceTable.empty ());
  
  // need to iterate
  RPG_Item_InstanceTableIterator_t iterator = myInstanceTable.begin ();
  do
  {
    if ((*iterator).second == handle_in)
    {
      myInstanceTable.erase (iterator);
      return;
    } // end IF
    ++iterator;
  } while (iterator != myInstanceTable.end ());

  // debug info
  RPG_Item_Instance_Base* instance_handle =
    dynamic_cast<RPG_Item_Instance_Base*> (handle_in);
  ACE_ASSERT (instance_handle);
  ACE_DEBUG ((LM_ERROR,
              ACE_TEXT ("item (id: %u) not found, returning\n"),
              instance_handle->id()));
  ACE_ASSERT (false);
}
