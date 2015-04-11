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

#ifndef RPG_ITEM_INSTANCE_BASE_H
#define RPG_ITEM_INSTANCE_BASE_H

#include "ace/Global_Macros.h"
#include "ace/Atomic_Op.h"
#include "ace/Synch.h"

#include "common_referencecounter_base.h"

#include "rpg_item_exports.h"
#include "rpg_item_instance_common.h"

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Item_Export RPG_Item_Instance_Base
 : public Common_ReferenceCounterBase
{
 public:
  // info
  const RPG_Item_ID_t getID() const;

 protected:
  // *NOTE*: invocation generates a new item ID
  RPG_Item_Instance_Base();
  // *TODO*: should be 'private'
  virtual ~RPG_Item_Instance_Base();

 private:
  typedef Common_ReferenceCounterBase inherited;

  ACE_UNIMPLEMENTED_FUNC(RPG_Item_Instance_Base(const RPG_Item_Instance_Base&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Item_Instance_Base& operator=(const RPG_Item_Instance_Base&));

  // atomic ID generator
  static ACE_Atomic_Op<ACE_Thread_Mutex, RPG_Item_ID_t> myCurrentID;

  RPG_Item_ID_t myID;
};

#endif
