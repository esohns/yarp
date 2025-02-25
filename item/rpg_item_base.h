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

#ifndef RPG_ITEM_BASE_H
#define RPG_ITEM_BASE_H

#include "ace/Global_Macros.h"

#include "rpg_item_instance_common.h"
#include "rpg_item_type.h"

class RPG_Item_Base
{
 public:
  virtual ~RPG_Item_Base ();

  inline enum RPG_Item_Type type () const { return type_; }

  virtual void dump () const;

 protected:
  RPG_Item_Base (enum RPG_Item_Type,
                 RPG_Item_ID_t);

  enum RPG_Item_Type type_;

 private:
  ACE_UNIMPLEMENTED_FUNC (RPG_Item_Base ())
  ACE_UNIMPLEMENTED_FUNC (RPG_Item_Base (const RPG_Item_Base&))
  ACE_UNIMPLEMENTED_FUNC (RPG_Item_Base& operator= (const RPG_Item_Base&))
};

#endif
