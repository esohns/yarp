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

#ifndef RPG_Net_Client_AsynchConnector_H
#define RPG_Net_Client_AsynchConnector_H

#include "rpg_net_exports.h"
#include "rpg_net_stream_common.h"

#include <ace/Global_Macros.h>
#include <ace/Asynch_Connector.h>

class RPG_Net_Export RPG_Net_Client_AsynchConnector
 : public ACE_Asynch_Connector<RPG_Net_StreamHandler_t>
{
 public:
	// override default creation strategy
	virtual RPG_Net_StreamHandler_t* make_handler(void);

 private:
  typedef ACE_Asynch_Connector<RPG_Net_StreamHandler_t> inherited;

  // safety measures
  RPG_Net_Client_AsynchConnector();
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Client_AsynchConnector(const RPG_Net_Client_AsynchConnector&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Client_AsynchConnector& operator=(const RPG_Net_Client_AsynchConnector&));
  virtual ~RPG_Net_Client_AsynchConnector();
};

#endif
