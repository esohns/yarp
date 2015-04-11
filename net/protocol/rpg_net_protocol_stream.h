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

#ifndef RPG_NET_PROTOCOL_STREAM_H
#define RPG_NET_PROTOCOL_STREAM_H

#include "ace/Global_Macros.h"
#include "ace/Synch_Traits.h"

#include "common.h"

#include "stream_base.h"

#include "rpg_net_protocol_common.h"
#include "rpg_net_protocol_common_modules.h"
#include "rpg_net_protocol_exports.h"
#include "rpg_net_protocol_module_IRCparser.h"
#include "rpg_net_protocol_module_IRCsplitter.h"
#include "rpg_net_protocol_module_IRCstreamer.h"

// forward declaration(s)
class RPG_Net_Protocol_Message;

class RPG_Protocol_Export RPG_Net_Protocol_Stream
 : public Stream_Base_T<ACE_MT_SYNCH,
                        Common_TimePolicy_t,
                        Stream_State_t,
                        RPG_Net_Protocol_RuntimeStatistic,
                        RPG_Net_Protocol_SessionData,
                        RPG_Net_Protocol_StreamSessionData_t,
                        RPG_Net_Protocol_SessionMessage,
                        RPG_Net_Protocol_Message>
{
 public:
  RPG_Net_Protocol_Stream ();
  virtual ~RPG_Net_Protocol_Stream ();

  // initialize stream
  bool initialize (const RPG_Net_Protocol_Configuration&); // stream/module configuration

  // implement Common_IStatistic_T
  // *NOTE*: delegate this to myRuntimeStatistic
  virtual bool collect (RPG_Net_Protocol_RuntimeStatistic&); // return value: statistic data
  // this is just a dummy (use statisticsReportingInterval instead)
  virtual void report () const;

 private:
  typedef Stream_Base_T<ACE_MT_SYNCH,
                        Common_TimePolicy_t,
                        Stream_State_t,
                        RPG_Net_Protocol_RuntimeStatistic,
                        RPG_Net_Protocol_SessionData,
                        RPG_Net_Protocol_StreamSessionData_t,
                        RPG_Net_Protocol_SessionMessage,
                        RPG_Net_Protocol_Message> inherited;

//   ACE_UNIMPLEMENTED_FUNC(RPG_Net_Protocol_Stream());
  ACE_UNIMPLEMENTED_FUNC (RPG_Net_Protocol_Stream (const RPG_Net_Protocol_Stream&));
  ACE_UNIMPLEMENTED_FUNC (RPG_Net_Protocol_Stream& operator=(const RPG_Net_Protocol_Stream&));

  // fini stream
  // *NOTE*: need this to clean up queued modules if something goes wrong during init() !
  bool finalize (const RPG_Net_Protocol_Configuration&); // configuration

  // modules
  RPG_Net_Protocol_Module_IRCMarshal_Module       myIRCMarshal;
  RPG_Net_Protocol_Module_IRCParser_Module        myIRCParser;
  RPG_Net_Protocol_Module_RuntimeStatistic_Module myRuntimeStatistic;
  //   RPG_Net_Protocol_Module_IRCHandler_Module myIRCHandler;
};

#endif
