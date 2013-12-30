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

#ifndef RPG_NET_PROTOCOL_MODULE_IRCSTREAMER_H
#define RPG_NET_PROTOCOL_MODULE_IRCSTREAMER_H

#include "rpg_stream_task_base_synch.h"
#include "rpg_stream_streammodule_base.h"

#include <ace/Global_Macros.h>
#include <ace/Synch_Traits.h>

// forward declaration(s)
class RPG_Net_Protocol_SessionMessage;
class RPG_Net_Protocol_Message;

class RPG_Net_Protocol_Module_IRCStreamer
 : public RPG_Stream_TaskBaseSynch<RPG_Net_Protocol_SessionMessage,
                                   RPG_Net_Protocol_Message>
{
 public:
  RPG_Net_Protocol_Module_IRCStreamer();
  virtual ~RPG_Net_Protocol_Module_IRCStreamer();

  // implement (part of) Stream_ITaskBase
  virtual void handleDataMessage(RPG_Net_Protocol_Message*&, // data message handle
                                 bool&);                     // return value: pass message downstream ?

 private:
  typedef RPG_Stream_TaskBaseSynch<RPG_Net_Protocol_SessionMessage,
                                   RPG_Net_Protocol_Message> inherited;

  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Protocol_Module_IRCStreamer(const RPG_Net_Protocol_Module_IRCStreamer&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Protocol_Module_IRCStreamer& operator=(const RPG_Net_Protocol_Module_IRCStreamer&));
};

// declare module
DATASTREAM_MODULE_INPUT_ONLY(ACE_MT_SYNCH,                         // task synch type
                             RPG_Net_Protocol_Module_IRCStreamer); // writer type

#endif
