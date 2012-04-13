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

#include "rpg_item_common_XML_tools.h"

#include "rpg_item_type.h"
#include "rpg_item_instance_base.h"
#include "rpg_item_armor.h"
#include "rpg_item_commodity.h"
#include "rpg_item_weapon.h"
#include "rpg_item_instance_manager.h"
#include "rpg_item_common_tools.h"

#include <rpg_common_macros.h>

#include <ace/Log_Msg.h>

RPG_Item_List_t
RPG_Item_Common_XML_Tools::instantiate(const RPG_Item_InventoryXML_XMLTree_Type& inventory_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Item_Common_XML_Tools::instantiate"));

  RPG_Item_List_t list;

  RPG_Item_Type item_type = RPG_ITEM_TYPE_INVALID;
  RPG_Item_Instance_Base* item_base = NULL;
  bool done = false;
  for (RPG_Item_InventoryXML_XMLTree_Type::item_const_iterator iterator = inventory_in.item().begin();
       iterator != inventory_in.item().end();
       iterator++)
  {
    item_type = RPG_Item_TypeHelper::stringToRPG_Item_Type((*iterator).type());
    item_base = NULL;
    done = false;

    switch (item_type)
    {
      case ITEM_ARMOR:
      {
        ACE_ASSERT((*iterator).armor().present());
        RPG_Item_ArmorPropertiesXML_XMLTree_Type armor_properties_xml = (*iterator).armor().get();

        item_base = RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create(ITEM_ARMOR,
                                                                            RPG_Item_ArmorTypeHelper::stringToRPG_Item_ArmorType(armor_properties_xml.type()));
        ACE_ASSERT(item_base);
        RPG_Item_Armor* armor = dynamic_cast<RPG_Item_Armor*>(item_base);
        ACE_ASSERT(armor);
        list.insert(armor->getID());

        break;
      }
      case ITEM_COMMODITY:
      {
        ACE_ASSERT((*iterator).commodity().present());
        RPG_Item_CommodityPropertiesBase_XMLTree_Type commodity_properties_xml = (*iterator).commodity().get();

        unsigned int type = (RPG_Item_CommodityTypeHelper::stringToRPG_Item_CommodityType(commodity_properties_xml.type()) << (sizeof(unsigned int) * 4));
        RPG_Item_CommodityUnion union_type = RPG_Item_Common_Tools::XMLStringToCommoditySubType(commodity_properties_xml.subType());
        switch (union_type.discriminator)
        {
          case RPG_Item_CommodityUnion::COMMODITYBEVERAGE:
            type |= union_type.commoditybeverage; break;
          //case COMMODITY_FOOD:
          //{
          //  // *TODO*: create these as well
          //  ACE_ASSERT(false);

          //  break;
          //}
          case RPG_Item_CommodityUnion::COMMODITYLIGHT:
            type |= union_type.commoditylight; break;
          //case COMMODITY_OTHER:
          //{
          //  // *TODO*: create these as well
          //  ACE_ASSERT(false);

          //  break;
          //}
          default:
          {
            ACE_DEBUG((LM_ERROR,
                       ACE_TEXT("invalid commodity type (was: \"%d\"), aborting\n"),
                       union_type.discriminator));

            done = true;

            break;
          }
        } // end SWITCH
        if (done)
          break;

        item_base = RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create(ITEM_COMMODITY,
                                                                            type);
        ACE_ASSERT(item_base);
        RPG_Item_Commodity* commodity = dynamic_cast<RPG_Item_Commodity*>(item_base);
        ACE_ASSERT(commodity);
        list.insert(commodity->getID());

        break;
      }
      case ITEM_OTHER:
      case ITEM_VALUABLE:
      {
        // *TODO*
        ACE_ASSERT(false);

        break;
      }
      case ITEM_WEAPON:
      {
        ACE_ASSERT((*iterator).weapon().present());
        RPG_Item_WeaponPropertiesXML_XMLTree_Type weapon_properties_xml = (*iterator).weapon().get();

        item_base = RPG_ITEM_INSTANCE_MANAGER_SINGLETON::instance()->create(ITEM_WEAPON,
                                                                            RPG_Item_WeaponTypeHelper::stringToRPG_Item_WeaponType(weapon_properties_xml.type()));
        ACE_ASSERT(item_base);
        RPG_Item_Weapon* weapon = dynamic_cast<RPG_Item_Weapon*>(item_base);
        ACE_ASSERT(weapon);
        list.insert(weapon->getID());

        break;
      }
      default:
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("invalid item type (was \"%s\"), aborting\n"),
                   (*iterator).type().c_str()));

        break;
      }
    } // end SWITCH
  } // end FOR

  return list;
}
