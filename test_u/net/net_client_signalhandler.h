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

#ifndef Net_Client_SignalHandler_H
#define Net_Client_SignalHandler_H

#include "ace/Global_Macros.h"

#include "common_istatistic.h"

#include "common_configuration.h"
#include "common_isignal.h"
#include "common_signal_handler.h"

#include "rpg_net_protocol_configuration.h"
#include "rpg_net_protocol_network.h"

// forward declarations
class Common_IControl;

class Net_Client_SignalHandler
 : public Common_SignalHandler_T<struct Common_SignalHandlerConfiguration>
{
  typedef Common_SignalHandler_T<struct Common_SignalHandlerConfiguration> inherited;

 public:
  Net_Client_SignalHandler (struct Common_EventDispatchConfiguration&,
                            RPG_Net_Protocol_ConnectionConfiguration&);
  inline virtual ~Net_Client_SignalHandler () {}

  // implement Common_ISignal
  virtual void handle (const struct Common_Signal&); // signal

 private:
  ACE_UNIMPLEMENTED_FUNC (Net_Client_SignalHandler ())
  ACE_UNIMPLEMENTED_FUNC (Net_Client_SignalHandler (const Net_Client_SignalHandler&))
  ACE_UNIMPLEMENTED_FUNC (Net_Client_SignalHandler& operator= (const Net_Client_SignalHandler&))

  struct Common_EventDispatchConfiguration* dispatchConfiguration_;
  RPG_Net_Protocol_ConnectionConfiguration* connectionConfiguration_;
  RPG_Net_Protocol_AsynchConnector_t        asynchConnector_;
  RPG_Net_Protocol_Connector_t              connector_;
  ACE_HANDLE                                handle_;
};

#endif
