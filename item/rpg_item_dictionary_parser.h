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
#ifndef RPG_ITEM_DICTIONARY_PARSER_H
#define RPG_ITEM_DICTIONARY_PARSER_H

#include "rpg_item_common.h"
#include "rpg_item_dictionary_parser_base.h"

#include <ace/Global_Macros.h>

/**
  @author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Item_Dictionary_Parser : public RPG_Item_Dictionary_Type_pimpl
{
 public:
  RPG_Item_Dictionary_Parser(RPG_ITEM_WEAPON_DICTIONARY_T*, // weapon dictionary
                             RPG_ITEM_ARMOR_DICTIONARY_T*); // armor dictionary
  virtual ~RPG_Item_Dictionary_Parser();

  virtual void pre();
  virtual void RPG_Item_Weapon_Dictionary(const RPG_ITEM_WEAPON_DICTIONARY_T&);
  virtual void RPG_Item_Armor_Dictionary(const RPG_ITEM_ARMOR_DICTIONARY_T&);
  virtual void post_RPG_Item_Dictionary_Type();

 private:
  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Item_Dictionary_Parser());
  ACE_UNIMPLEMENTED_FUNC(RPG_Item_Dictionary_Parser(const RPG_Item_Dictionary_Parser&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Item_Dictionary_Parser& operator=(const RPG_Item_Dictionary_Parser&));

  RPG_ITEM_WEAPON_DICTIONARY_T* myWeaponDictionary;
  RPG_ITEM_ARMOR_DICTIONARY_T*  myArmorDictionary;
};

#endif
