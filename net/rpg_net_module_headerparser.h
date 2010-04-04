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

#ifndef RPG_NET_MODULE_HEADERPARSER_H
#define RPG_NET_MODULE_HEADERPARSER_H

#include "rpg_net_sessionmessage.h"

#include <stream_task_base_synch.h>
#include <stream_streammodule_base.h>

#include <set>
#include <map>
#include <vector>

// forward declaration(s)
class Stream_MessageBase;

class RPG_Net_Module_HeaderParser
 : public Stream_TaskBaseSynch<RPG_Net_SessionMessage>
{
 public:
  RPG_Net_Module_HeaderParser();
  virtual ~RPG_Net_Module_HeaderParser();

  // initialization
  const bool init();

  // implement (part of) Stream_ITaskBase
  virtual void handleDataMessage(Stream_MessageBase*&, // data message handle
                                 bool&);               // return value: pass message downstream ?

  // implement RPG_Common_IDumpState
  virtual void dump_state() const;

 private:
  typedef Stream_TaskBaseSynch<RPG_Net_SessionMessage> inherited;

  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Module_HeaderParser(const RPG_Net_Module_HeaderParser&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Module_HeaderParser& operator=(const RPG_Net_Module_HeaderParser&));

  // debug info
  // some convenience typedefs --> save us some typing...
  typedef std::set<unsigned short> PACKETTYPECONTAINER_TYPE;
  typedef PACKETTYPECONTAINER_TYPE::const_iterator PACKETTYPECONTAINER_CONSTITERATOR_TYPE;
  typedef std::multimap<unsigned short,
                        unsigned long> PACKETTYPE2MESSAGEIDMAP_TYPE;
  typedef std::pair<unsigned short,
                    unsigned long> PACKETTYPE2MESSAGEIDPAIR_TYPE;
  typedef PACKETTYPE2MESSAGEIDMAP_TYPE::const_iterator PACKETTYPE2MESSAGEIDMAP_CONSTITERATOR_TYPE;

  mutable PACKETTYPE2MESSAGEIDMAP_TYPE myUnknownPacketTypes2MessageID; // network byte order !
  mutable std::vector<unsigned long>   myIEEE802_3Frames;
  mutable std::vector<unsigned long>   myFailedMessages; // we failed to decode these...

  bool myIsInitialized;
};

// declare module
DATASTREAM_MODULE(RPG_Net_Module_HeaderParser);

#endif
