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

#ifndef RPG_NET_PROTOCOL_MESSAGEALLOCATOR_H
#define RPG_NET_PROTOCOL_MESSAGEALLOCATOR_H

#include "stream_cachedmessageallocatorheap_base.h"

#include "rpg_net_protocol_exports.h"
#include "rpg_net_protocol_message.h"
#include "rpg_net_protocol_sessionmessage.h"

// forward declarations
class ACE_Allocator;

class RPG_Protocol_Export RPG_Net_Protocol_MessageAllocator
 : public Stream_CachedMessageAllocatorHeapBase_T<RPG_Net_Protocol_Message,
                                                  RPG_Net_Protocol_SessionMessage>
{
 public:
  RPG_Net_Protocol_MessageAllocator (unsigned int,    // total number of concurrent messages
                                     ACE_Allocator*); // (heap) memory allocator...
  virtual ~RPG_Net_Protocol_MessageAllocator ();

 private:
  typedef Stream_CachedMessageAllocatorHeapBase_T<RPG_Net_Protocol_Message,
                                                  RPG_Net_Protocol_SessionMessage> inherited;

  ACE_UNIMPLEMENTED_FUNC (RPG_Net_Protocol_MessageAllocator (const RPG_Net_Protocol_MessageAllocator&));
  ACE_UNIMPLEMENTED_FUNC (RPG_Net_Protocol_MessageAllocator& operator=(const RPG_Net_Protocol_MessageAllocator&));
};

#endif
