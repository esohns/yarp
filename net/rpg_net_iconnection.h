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

#ifndef RPG_NET_ICONNECTION_H
#define RPG_NET_ICONNECTION_H

#include "rpg_common_istatistic.h"
#include "rpg_common_irefcount.h"

#include "rpg_net_itransportlayer.h"

#include "ace/INET_Addr.h"

template <typename StatisticsContainerType>
class RPG_Net_IConnection
 : public RPG_Common_IStatistic<StatisticsContainerType>,
   virtual public RPG_Common_IRefCount,
   virtual public RPG_Net_ITransportLayer
{
 public:
  virtual ~RPG_Net_IConnection() {};
};

#endif
