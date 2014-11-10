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

#include "rpg_net_stream_common.h"

#include "rpg_common_signalhandler.h"
#include "rpg_common_isignal.h"
#include "rpg_common_istatistic.h"

// forward declarations
class RPG_Common_IControl;

class Net_Server_SignalHandler
 : public RPG_Common_SignalHandler,
   public RPG_Common_ISignal
{
 public:
  Net_Server_SignalHandler(const long&,                                      // timer ID
                           RPG_Common_IControl*,                             // controller
                           RPG_Common_IStatistic<RPG_Net_RuntimeStatistic>*, // reporter
                           // ---------------------------------------------------------------
                           const bool& = true);                              // use reactor ?
  virtual ~Net_Server_SignalHandler();

  // implement RPG_Common_ISignal
  virtual bool handleSignal(const int&); // signal

 private:
  typedef RPG_Common_SignalHandler inherited;

  ACE_UNIMPLEMENTED_FUNC(Net_Server_SignalHandler());
  ACE_UNIMPLEMENTED_FUNC(Net_Server_SignalHandler(const Net_Server_SignalHandler&));
  ACE_UNIMPLEMENTED_FUNC(Net_Server_SignalHandler& operator=(const Net_Server_SignalHandler&));

  long                                             myTimerID;
	RPG_Common_IControl*                             myControl;
  RPG_Common_IStatistic<RPG_Net_RuntimeStatistic>* myReport;
	bool                                             myUseReactor;
};

#endif
