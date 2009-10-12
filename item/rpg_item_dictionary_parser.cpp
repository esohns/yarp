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
#include "rpg_item_dictionary_parser.h"

#include <ace/Log_Msg.h>

RPG_Item_Dictionary_Parser::RPG_Item_Dictionary_Parser(RPG_ITEM_WEAPON_DICTIONARY_T* weaponDictionary_in,
                                                       RPG_ITEM_ARMOR_DICTIONARY_T* armorDictionary_in)
 : myWeaponDictionary(weaponDictionary_in),
   myArmorDictionary(armorDictionary_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Dictionary_Parser::RPG_Item_Dictionary_Parser"));

}

RPG_Item_Dictionary_Parser::~RPG_Item_Dictionary_Parser()
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Dictionary_Parser::~RPG_Item_Dictionary_Parser"));

}

void RPG_Item_Dictionary_Parser::pre()
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Dictionary_Parser::pre"));

}

void RPG_Item_Dictionary_Parser::RPG_Item_Weapon_Dictionary(const RPG_ITEM_WEAPON_DICTIONARY_T& weaponDictionary_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Dictionary_Parser::RPG_Item_Weapon_Dictionary"));

  // TODO
  //
}

void RPG_Item_Dictionary_Parser::RPG_Item_Armor_Dictionary(const RPG_ITEM_ARMOR_DICTIONARY_T& armorDictionary_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Dictionary_Parser::RPG_Item_Armor_Dictionary"));

  // TODO
  //
}

void RPG_Item_Dictionary_Parser::post_RPG_Item_Dictionary_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Dictionary_Parser::RPG_Item_Armor_Dictionary"));

  // TODO
  //
  // return ... ;
}
