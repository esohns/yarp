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

#ifndef RPG_COMMON_H
#define RPG_COMMON_H

#include "rpg_common_physicaldamagetype.h"
#include "rpg_common_terrain.h"

// timer queue
#include <ace/Synch_Traits.h>
#include <ace/Event_Handler_Handle_Timeout_Upcall.h>
#include <ace/Timer_Queue_T.h>
#include <ace/Timer_Heap_T.h>
#include <ace/Timer_Queue_Adapters.h>

#include <set>

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
typedef std::set<RPG_Common_PhysicalDamageType> RPG_Common_PhysicalDamageList_t;
typedef RPG_Common_PhysicalDamageList_t::const_iterator RPG_Common_PhysicalDamageListIterator_t;

typedef std::set<RPG_Common_Terrain> RPG_Common_Terrains_t;
typedef RPG_Common_Terrains_t::const_iterator RPG_Common_TerrainsIterator_t;

// *NOTE*: ensure a minimal amount of locking
//typedef ACE_Event_Handler_Handle_Timeout_Upcall<ACE_SYNCH_NULL_MUTEX> RPG_Common_TimeoutUpcall_t;
typedef ACE_Event_Handler_Handle_Timeout_Upcall RPG_Common_TimeoutUpcall_t;
typedef ACE_Timer_Heap_T<ACE_Event_Handler*,
                         RPG_Common_TimeoutUpcall_t,
                         ACE_SYNCH_NULL_MUTEX> RPG_Common_TimerHeap_t;
typedef ACE_Timer_Heap_Iterator_T<ACE_Event_Handler*,
                                  RPG_Common_TimeoutUpcall_t,
                                  ACE_SYNCH_NULL_MUTEX> RPG_Common_TimerHeapIterator_t;
typedef ACE_Thread_Timer_Queue_Adapter<RPG_Common_TimerHeap_t,
                                       ACE_Event_Handler*> RPG_Common_TimerQueue_t;

#endif
