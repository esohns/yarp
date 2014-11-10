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

#ifndef RPG_NET_STREAM_COMMON_H
#define RPG_NET_STREAM_COMMON_H

#include "rpg_net_common.h"
#include "rpg_net_statistichandler.h"

#include "rpg_stream_iallocator.h"
#include "rpg_stream_imodule.h"

#include "rpg_common.h"

#include "ace/Time_Value.h"
#include "ace/Notification_Strategy.h"
#include "ace/Stream.h"
#include "ace/Synch_Traits.h"

//// forward declaration(s)
//class RPG_Stream_Module;

typedef RPG_Stream_IModule<ACE_MT_SYNCH,
                           RPG_Common_TimePolicy_t> IMODULE_TYPE;
typedef ACE_Stream_Iterator<ACE_MT_SYNCH,
                            RPG_Common_TimePolicy_t> STREAM_ITERATOR_TYPE;

struct RPG_Net_RuntimeStatistic
{
  unsigned int numDataMessages; // (protocol) messages
  double       numBytes;        // amount of processed data

  // convenience
  inline RPG_Net_RuntimeStatistic operator+=(const RPG_Net_RuntimeStatistic& rhs)
  {
    numDataMessages += rhs.numDataMessages;
    numBytes += rhs.numBytes;

    return *this;
  };
};

struct RPG_Net_ConfigPOD
{
  // ************************ connection config data ***************************
  unsigned int               peerPingInterval; // ms {0 --> OFF}
  bool                       pingAutoAnswer;
  bool                       printPongMessages;
  int                        socketBufferSize;
  RPG_Stream_IAllocator*     messageAllocator;
  unsigned int               bufferSize;
  bool                       useThreadPerConnection;
  // *IMPORTANT NOTE*: in a threaded environment, workers MAY be
  // dispatching the reactor notification queue concurrently (most notably,
  // ACE_TP_Reactor) --> enforce proper serialization
  bool                       serializeOutput;
  // *************************** stream config data ****************************
  ACE_Notification_Strategy* notificationStrategy;
  MODULE_TYPE*               module;
  bool                       deleteModule;
  unsigned int               sessionID; // (== socket handle !)
  unsigned int               statisticsReportingInterval;
  bool                       printFinalReport;
  // ****************************** runtime data *******************************
  RPG_Net_RuntimeStatistic   currentStatistics;
  ACE_Time_Value             lastCollectionTimestamp;
};

typedef RPG_Net_StatisticHandler_Reactor<RPG_Net_RuntimeStatistic> RPG_Net_StatisticHandler_Reactor_t;
typedef RPG_Net_StatisticHandler_Proactor<RPG_Net_RuntimeStatistic> RPG_Net_StatisticHandler_Proactor_t;

#endif
