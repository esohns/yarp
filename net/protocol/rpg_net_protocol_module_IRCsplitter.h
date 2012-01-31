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

#ifndef RPG_NET_PROTOCOL_MODULE_IRCSPLITTER_H
#define RPG_NET_PROTOCOL_MODULE_IRCSPLITTER_H

#include "rpg_net_protocol_defines.h"
#include "rpg_net_protocol_common.h"
#include "rpg_net_protocol_IRCbisect.h"

#include <rpg_net_statistichandler.h>

#include <rpg_common_istatistic.h>

#include <rpg_stream_headmoduletask_base.h>
#include <rpg_stream_session_config_base.h>
#include <rpg_stream_streammodule.h>

#include <ace/Global_Macros.h>

// forward declaration(s)
class RPG_Stream_IAllocator;
//typedef void* yyscan_t;
typedef struct yy_buffer_state* YY_BUFFER_STATE;
class RPG_Net_Protocol_SessionMessage;
class RPG_Net_Protocol_Message;

class RPG_Net_Protocol_Module_IRCSplitter
 : public RPG_Stream_HeadModuleTaskBase<RPG_Net_Protocol_ConfigPOD,
                                        RPG_Stream_SessionConfigBase<RPG_Net_Protocol_ConfigPOD>,
                                        RPG_Net_Protocol_SessionMessage,
                                        RPG_Net_Protocol_Message>,
   // implement this so we can use a generic (timed) event handler to trigger stat collection...
   public RPG_Common_IStatistic<RPG_Net_Protocol_RuntimeStatistic>
{
 public:
  RPG_Net_Protocol_Module_IRCSplitter();
  virtual ~RPG_Net_Protocol_Module_IRCSplitter();

  // configuration / initialization
  const bool init(// *** base class initializers ***
                  RPG_Stream_IAllocator*,   // message allocator
                  // *** base class initializers END ***
                  // *NOTE*: this option may be useful for (downstream) parsers that
                  // only work on one CONTIGUOUS buffer (i.e. cannot parse unaligned bits and pieces)
                  // *WARNING*: will NOT work with multithreaded stream processing --> USE WITH CAUTION !
                  const bool& = false,      // "crunch" completed messages ?
                  const unsigned long& = 0, // statistics collecting interval (second(s))
                                            // 0 --> DON'T collect statistics
                  const bool& = RPG_NET_PROTOCOL_DEF_TRACE_SCANNING); // trace scanning ?

  // implement (part of) RPG_Stream_ITaskBase
  virtual void handleDataMessage(RPG_Net_Protocol_Message*&, // data message handle
                                 bool&);                     // return value: pass message downstream ?

  // catch the session ID...
  virtual void handleSessionMessage(RPG_Net_Protocol_SessionMessage*&, // session message handle
                                    bool&);                            // return value: pass message downstream ?

  // implement RPG_Common_IStatistic
  // *NOTE*: we reuse the interface for our own purposes (to implement timer-based data collection)
  virtual const bool collect(RPG_Net_Protocol_RuntimeStatistic&) const; // return value: (currently unused !)
  virtual void report() const;

 private:
  typedef RPG_Stream_HeadModuleTaskBase<RPG_Net_Protocol_ConfigPOD,
                                        RPG_Stream_SessionConfigBase<RPG_Net_Protocol_ConfigPOD>,
                                        RPG_Net_Protocol_SessionMessage,
                                        RPG_Net_Protocol_Message> inherited;

  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Protocol_Module_IRCSplitter(const RPG_Net_Protocol_Module_IRCSplitter&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Protocol_Module_IRCSplitter& operator=(const RPG_Net_Protocol_Module_IRCSplitter&));

  // convenience types
  typedef RPG_Net_StatisticHandler<RPG_Net_Protocol_RuntimeStatistic> STATISTICHANDLER_TYPE;
  typedef RPG_Stream_SessionConfigBase<RPG_Net_Protocol_ConfigPOD> SESSIONCONFIG_TYPE;

  // helper methods
  const bool putStatisticsMessage(const RPG_Net_Protocol_RuntimeStatistic&, // statistics info
                                  const ACE_Time_Value&) const;             // statistics generation time
  // helper methods (to drive the scanner)
  const bool scan_begin(char*,          // base address
                        const size_t&); // length of data block
  void scan_end();

  bool                      myCrunchMessages;
  unsigned int              mySessionID;

  // timer stuff
  STATISTICHANDLER_TYPE     myStatCollectHandler;
  int                       myStatCollectHandlerID;

  // scanner
  yyscan_t                  myScannerContext;
  YY_BUFFER_STATE           myCurrentBufferState;
  unsigned int              myCurrentNumFrames;

  // message buffers
  RPG_Net_Protocol_Message* myCurrentMessage;
  RPG_Net_Protocol_Message* myCurrentBuffer;
  unsigned int              myCurrentMessageLength;

  bool                      myIsInitialized;
};

// declare module
DATASTREAM_MODULE(RPG_Net_Protocol_Module_IRCSplitter);

#endif
