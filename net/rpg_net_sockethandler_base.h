/***************************************************************************
 *   Copyright (C) 2010 by Erik Sohns   *
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

#ifndef RPG_NET_SOCKETHANDLER_BASE_H
#define RPG_NET_SOCKETHANDLER_BASE_H

#include "ace/Malloc_Base.h"

class RPG_Net_SocketHandlerBase
{
 public:
  virtual ~RPG_Net_SocketHandlerBase ();

  void init (ACE_Allocator*); // message allocator handle

 protected:
  RPG_Net_SocketHandlerBase ();

  ACE_Allocator* allocator_;

 private:
//  ACE_UNIMPLEMENTED_FUNC(RPG_Net_SocketHandlerBase());
  ACE_UNIMPLEMENTED_FUNC (RPG_Net_SocketHandlerBase (const RPG_Net_SocketHandlerBase&));
  ACE_UNIMPLEMENTED_FUNC (RPG_Net_SocketHandlerBase& operator= (const RPG_Net_SocketHandlerBase&));
};

#endif
