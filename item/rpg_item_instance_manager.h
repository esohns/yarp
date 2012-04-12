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

#ifndef RPG_ITEM_INSTANCE_MANAGER_H
#define RPG_ITEM_INSTANCE_MANAGER_H

#include "rpg_item_exports.h"
#include "rpg_item_instance_base.h"
#include "rpg_item_base.h"

#include <ace/Singleton.h>
#include <ace/Synch.h>

#include <map>

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Item_Export RPG_Item_Instance_Manager
{
  // we use the singleton pattern, so we need to enable access to the ctor/dtors
  friend class ACE_Singleton<RPG_Item_Instance_Manager,
                             ACE_Thread_Mutex>;

  // grant access to (de-)register methods...
  friend class RPG_Item_Base;

 public:
  RPG_Item_Instance_Base* create(const RPG_Item_Type&, // type of item
                                 const unsigned int&); // specific (sub)type
  bool get(const RPG_Item_ID_t&,   // id
           RPG_Item_Base*&) const; // return value: handle

 private:
  typedef std::map<RPG_Item_ID_t, RPG_Item_Base*> RPG_Item_InstanceTable_t;
  typedef RPG_Item_InstanceTable_t::iterator RPG_Item_InstanceTableIterator_t;
  typedef RPG_Item_InstanceTable_t::const_iterator RPG_Item_InstanceTableConstIterator_t;

  RPG_Item_Instance_Manager();
  virtual ~RPG_Item_Instance_Manager();
  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Item_Instance_Manager(const RPG_Item_Instance_Manager&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Item_Instance_Manager& operator=(const RPG_Item_Instance_Manager&));

  // *NOTE*: these are called from RPG_Item_Base ctors/dtors...
  virtual void registerItem(const RPG_Item_ID_t&, // id
                            RPG_Item_Base*);      // handle
  virtual void deregisterItem(RPG_Item_Base*); // handle

  mutable ACE_Thread_Mutex myLock;
  RPG_Item_InstanceTable_t myInstanceTable;
};

typedef ACE_Singleton<RPG_Item_Instance_Manager,
                      ACE_Thread_Mutex> RPG_ITEM_INSTANCE_MANAGER_SINGLETON;
RPG_ITEM_SINGLETON_DECLARE(ACE_Singleton, RPG_Item_Instance_Manager, ACE_Thread_Mutex);

#endif
