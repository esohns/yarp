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
#include "rpg_item_instance_base.h"

// init statics
ACE_Atomic_Op<ACE_Thread_Mutex, RPG_ITEM_ID_T> RPG_Item_Instance_Base::myCurrentID = 1;

RPG_Item_Instance_Base::RPG_Item_Instance_Base()
 : myID(myCurrentID++) // *WARNING*: prefix increment leads to corruption !
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Instance_Base::RPG_Item_Instance_Base"));

}

RPG_Item_Instance_Base::~RPG_Item_Instance_Base()
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Instance_Base::~RPG_Item_Instance_Base"));

}

const RPG_ITEM_ID_T RPG_Item_Instance_Base::getID() const
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Instance_Base::getID"));

  return myID;
}
