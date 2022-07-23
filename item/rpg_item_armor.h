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

#ifndef RPG_ITEM_ARMOR_H
#define RPG_ITEM_ARMOR_H

#include "ace/Global_Macros.h"

#include "rpg_item_armortype.h"
#include "rpg_item_base.h"
#include "rpg_item_instance_base.h"
#include "rpg_item_instance_manager.h"

class RPG_Item_Armor
 : public RPG_Item_Instance_Base,
   public RPG_Item_Base
{
  typedef RPG_Item_Instance_Base inherited;
  typedef RPG_Item_Base inherited2;

  // grant access to ctor
  friend class RPG_Item_Instance_Manager;

 public:
  enum RPG_Item_ArmorType armorType_;
  inline enum RPG_Item_ArmorType armorType () const { return armorType_; }

  virtual void dump () const;

 private:
  ACE_UNIMPLEMENTED_FUNC (RPG_Item_Armor ())
  RPG_Item_Armor (enum RPG_Item_ArmorType);
  inline virtual ~RPG_Item_Armor () {}
  ACE_UNIMPLEMENTED_FUNC (RPG_Item_Armor (const RPG_Item_Armor&))
  ACE_UNIMPLEMENTED_FUNC (RPG_Item_Armor& operator= (const RPG_Item_Armor&))
};

#endif
