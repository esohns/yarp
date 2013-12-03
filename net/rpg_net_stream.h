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

#include "rpg_net_exports.h"
#include "rpg_net_common.h"
#include "rpg_net_common_modules.h"
#include "rpg_net_stream_config.h"
#include "rpg_net_module_sockethandler.h"
#include "rpg_net_module_headerparser.h"
#include "rpg_net_module_protocolhandler.h"
#include "rpg_net_module_runtimestatistic.h"

#include "rpg_common_istatistic.h"

#include "rpg_stream_base.h"

#include <ace/Global_Macros.h>

// forward declaration(s)
class RPG_Net_SessionMessage;
class RPG_Net_Message;

class RPG_Net_Export RPG_Net_Stream
 : public RPG_Stream_Base<RPG_Net_ConfigPOD,
                          RPG_Net_StreamConfig,
                          RPG_Net_SessionMessage,
                          RPG_Net_Message>,
   public RPG_Common_IStatistic<RPG_Net_RuntimeStatistic>
{
 public:
   RPG_Net_Stream();
   virtual ~RPG_Net_Stream();

  // convenience types
  typedef RPG_Common_IStatistic<RPG_Net_RuntimeStatistic> StatisticsInterface_Type;

  // init stream
  bool init(const RPG_Net_ConfigPOD&); // stream/module configuration

  unsigned int getSessionID() const;

  // implement RPG_Common_IStatistic
  // *NOTE*: delegate this to myRuntimeStatistic
  virtual bool collect(RPG_Net_RuntimeStatistic&) const; // return value: statistic data
  // this is just a dummy (use statisticsReportingInterval instead)
  virtual void report() const;

 private:
  typedef RPG_Stream_Base<RPG_Net_ConfigPOD,
                          RPG_Net_StreamConfig,
                          RPG_Net_SessionMessage,
                          RPG_Net_Message> inherited;

  // safety measures
//   ACE_UNIMPLEMENTED_FUNC(RPG_Net_Stream());
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Stream(const RPG_Net_Stream&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Stream& operator=(const RPG_Net_Stream&));

  // fini stream
  // *NOTE*: need this to clean up queued modules if something goes wrong during init() !
  bool fini(const RPG_Net_ConfigPOD&); // configuration

  // modules
  RPG_Net_Module_SocketHandler_Module    mySocketHandler;
  RPG_Net_Module_HeaderParser_Module     myHeaderParser;
  RPG_Net_Module_ProtocolHandler_Module  myProtocolHandler;
  RPG_Net_Module_RuntimeStatistic_Module myRuntimeStatistic;
};

#endif
