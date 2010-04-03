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

#ifndef RPG_NET_STREAM_H
#define RPG_NET_STREAM_H

#include "rpg_net_common.h"
#include "rpg_net_module_sockethandler.h"
#include "rpg_net_module_headerparser.h"
#include "rpg_net_module_runtimestatistic.h"
// #include "rpg_net_module_protocolhandler.h"

#include <rpg_common_istatistic.h>

#include <stream_base.h>

#include <ace/Global_Macros.h>

// forward declaration(s)
class Stream_IAllocator;

class RPG_Net_Stream
 : public Stream_Base<RPG_Net_ConfigPOD>,
   // *NOTE*: implement this in order to successfuly encapsulate stream specifics...
   // --> delegate the actual functionality to one of our modules
   public RPG_Common_IStatistic<RPG_Net_RuntimeStatistic>
{
 public:
   RPG_Net_Stream();
   virtual ~RPG_Net_Stream();

  // convenience types
  typedef RPG_Common_IStatistic<RPG_Net_RuntimeStatistic> StatisticsInterface_Type;

  // init stream
  const bool init(const RPG_Net_ConfigPOD&); // stream/module configuration

  // implement RPG_Common_IStatistic
  // *NOTE*: delegate this to myRuntimeStatistic
  virtual const bool collect(RPG_Net_RuntimeStatistic&); // return value: statistic data
  // this is just a dummy (use statisticsReportingInterval instead)
  virtual void report();

 private:
  typedef Stream_Base<RPG_Net_ConfigPOD> inherited;

  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Stream(const RPG_Net_Stream&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Stream& operator=(const RPG_Net_Stream&));

  // fini stream
  // *NOTE*: need this to clean up queued modules if something goes wrong during init() !
  const bool fini(const RPG_Net_ConfigPOD&); // configuration

  // modules
  RPG_Net_Module_SocketHandler_Module    mySocketHandler;
  RPG_Net_Module_HeaderParser_Module     myHeaderParser;
  RPG_Net_Module_RuntimeStatistic_Module myRuntimeStatistic;
//   RPG_Net_ProtocolHandler_Module  myProtocolHandler;
};

typedef ACE_Unmanaged_Singleton<RPG_Net_Stream,
                                ACE_Thread_Mutex> RPG_NET_STREAM_SINGLETON;

#endif
