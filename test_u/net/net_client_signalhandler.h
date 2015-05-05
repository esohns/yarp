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

#ifndef NET_CLIENT_SIGNALHANDLER_H
#define NET_CLIENT_SIGNALHANDLER_H

#include "ace/Global_Macros.h"
#include "ace/INET_Addr.h"

#include "common_isignal.h"
#include "common_signalhandler.h"

#include "net_client_connector_common.h"

class Net_Client_SignalHandler
 : public Common_SignalHandler,
   public Common_ISignal
{
 public:
  Net_Client_SignalHandler (long,                     // action timer id
                            const ACE_INET_Addr&,     // peer SAP
                            Net_Client_IConnector_t*, // connector
                            // -------------------------------------------
                            bool);                    // use reactor ?
  virtual ~Net_Client_SignalHandler ();

  // implement Common_ISignal
  virtual bool handleSignal (int); // signal

 private:
  typedef Common_SignalHandler inherited;

  ACE_UNIMPLEMENTED_FUNC (Net_Client_SignalHandler ());
  ACE_UNIMPLEMENTED_FUNC (Net_Client_SignalHandler (const Net_Client_SignalHandler&));
  ACE_UNIMPLEMENTED_FUNC (Net_Client_SignalHandler& operator= (const Net_Client_SignalHandler&));

  long                     actionTimerID_;
  Net_Client_IConnector_t* connector_;
  ACE_INET_Addr            peerAddress_;
  bool                     useReactor_;
};

#endif
