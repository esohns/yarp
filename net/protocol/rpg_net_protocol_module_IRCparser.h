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

#ifndef RPG_NET_PROTOCOL_MODULE_IRCPARSER_H
#define RPG_NET_PROTOCOL_MODULE_IRCPARSER_H

#include <rpg_net_sessionmessage.h>

#include <stream_task_base_synch.h>
#include <stream_streammodule_base.h>

#include <ace/Global_Macros.h>

// forward declaration(s)
class Stream_IAllocator;
class Stream_MessageBase;

class RPG_Net_Protocol_Module_IRCParser
 : public Stream_TaskBaseSynch<RPG_Net_SessionMessage>
{
 public:
  RPG_Net_Protocol_Module_IRCParser();
  virtual ~RPG_Net_Protocol_Module_IRCParser();

  // configuration / initialization
  const bool init(Stream_IAllocator*); // message allocator

  // implement (part of) Stream_ITaskBase
  virtual void handleDataMessage(Stream_MessageBase*&, // data message handle
                                 bool&);               // return value: pass message downstream ?

 private:
  typedef Stream_TaskBaseSynch<RPG_Net_SessionMessage> inherited;

  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Protocol_Module_IRCParser(const RPG_Net_Protocol_Module_IRCParser&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Protocol_Module_IRCParser& operator=(const RPG_Net_Protocol_Module_IRCParser&));

  bool myIsInitialized;
};

// declare module
DATASTREAM_MODULE(RPG_Net_Protocol_Module_IRCParser);

#endif
