/***************************************************************************
 *   Copyright (C) 2009 by Erik Sohns                                      *
 *   erik.sohns@web.de                                                     *
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
#ifndef RPG_ITEM_ARMOR_BASE_H
#define RPG_ITEM_ARMOR_BASE_H

#include "rpg_item_base.h"
#include "rpg_item_armortype.h"

#include <ace/Global_Macros.h>

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Item_Armor_Base
 : public RPG_Item_Base
{
 public:
  virtual ~RPG_Item_Armor_Base();

  // what am I ?
  const RPG_Item_ArmorType getArmorType() const;

  virtual void dump() const;

 protected:
  RPG_Item_Armor_Base(const RPG_Item_ArmorType&, // type
                      const RPG_Item_ID_t&);     // ID

 private:
  typedef RPG_Item_Base inherited;

  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Item_Armor_Base());
  ACE_UNIMPLEMENTED_FUNC(RPG_Item_Armor_Base(const RPG_Item_Armor_Base&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Item_Armor_Base& operator=(const RPG_Item_Armor_Base&));

  RPG_Item_ArmorType myArmorType;
};

#endif
