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

#include "rpg_item_instance_base.h"

#include "ace/Log_Msg.h"

#include "rpg_common_macros.h"

// initialize statics
ACE_Atomic_Op<ACE_SYNCH_MUTEX, RPG_Item_ID_t> RPG_Item_Instance_Base::myCurrentId = 1;

RPG_Item_Instance_Base::RPG_Item_Instance_Base ()
 : inherited (1,     // initial reference count
              true), // "delete this" on refcount == 0
   id_ (myCurrentId++) // *WARNING*: prefix increment leads to corruption !
{
  RPG_TRACE (ACE_TEXT ("RPG_Item_Instance_Base::RPG_Item_Instance_Base"));

}
