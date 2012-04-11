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

#ifndef RPG_ITEM_DICTIONARY_H
#define RPG_ITEM_DICTIONARY_H

#include <rpg_dice_incl.h>
#include <rpg_common_incl.h>
#include <rpg_common_environment_incl.h>
#include <rpg_character_incl.h>
#include <rpg_magic_incl.h>
#include "rpg_item_incl.h"

#include "rpg_item_exports.h"
#include "rpg_item_common.h"

#include <ace/Global_Macros.h>
#include <ace/Singleton.h>
#include <ace/Synch.h>

#include <string>

/**
  @author Erik Sohns <erik.sohns@web.de>
 */
class RPG_Item_Export RPG_Item_Dictionary
{
  // singleton requires access to the ctor/dtor
 friend class ACE_Singleton<RPG_Item_Dictionary,
                            ACE_Thread_Mutex>;

 public:
  // init item dictionary
  void init(const std::string&,  // filename
            const bool& = true); // validate XML ?

  const RPG_Item_ArmorProperties& getArmorProperties(const RPG_Item_ArmorType&) const;
  const RPG_Item_CommodityProperties& getCommodityProperties(const RPG_Item_CommodityUnion&) const;
  const RPG_Item_WeaponProperties& getWeaponProperties(const RPG_Item_WeaponType&) const;

  // debug info
  void dump() const;

 private:
  // safety measures
  RPG_Item_Dictionary();
  virtual ~RPG_Item_Dictionary();
  ACE_UNIMPLEMENTED_FUNC(RPG_Item_Dictionary(const RPG_Item_Dictionary&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Item_Dictionary& operator=(const RPG_Item_Dictionary&));

  RPG_Item_ArmorDictionary_t             myArmorDictionary;
  RPG_Item_CommodityBeverageDictionary_t myCommodityBeverageDictionary;
  RPG_Item_CommodityLightDictionary_t    myCommodityLightDictionary;
  RPG_Item_WeaponDictionary_t            myWeaponDictionary;
};

typedef ACE_Singleton<RPG_Item_Dictionary,
                      ACE_Thread_Mutex> RPG_ITEM_DICTIONARY_SINGLETON;
RPG_ITEM_SINGLETON_DECLARE(ACE_Singleton, RPG_Item_Dictionary, ACE_Thread_Mutex);

#endif
