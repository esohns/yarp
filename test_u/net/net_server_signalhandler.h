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

#ifndef NET_SERVER_SIGNALHANDLER_H
#define NET_SERVER_SIGNALHANDLER_H

#include "ace/Global_Macros.h"

#include "common_istatistic.h"

#include "common_isignal.h"
#include "common_signal_handler.h"

#include "rpg_net_protocol_configuration.h"

// forward declarations
class Common_IControl;

class Net_Server_SignalHandler
 : public Common_SignalHandler
 , public Common_ISignal
{
 public:
  Net_Server_SignalHandler (long,                                                    // timer ID
                            Common_IControl*,                                        // controller handle
                            Common_IStatistic_T<RPG_Net_Protocol_RuntimeStatistic>*, // reporter handle
                            // ---------------------------------------------------------------
                            bool = true);                                            // use reactor ?
  virtual ~Net_Server_SignalHandler ();

  // implement Common_ISignal
  virtual bool handleSignal (int); // signal

 private:
  typedef Common_SignalHandler inherited;

  ACE_UNIMPLEMENTED_FUNC (Net_Server_SignalHandler ());
  ACE_UNIMPLEMENTED_FUNC (Net_Server_SignalHandler (const Net_Server_SignalHandler&));
  ACE_UNIMPLEMENTED_FUNC (Net_Server_SignalHandler& operator= (const Net_Server_SignalHandler&));

  Common_IControl*                                        control_;
  Common_IStatistic_T<RPG_Net_Protocol_RuntimeStatistic>* report_;
  long                                                    timerID_;
  bool                                                    useReactor_;
};

#endif
