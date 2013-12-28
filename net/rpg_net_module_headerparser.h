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

#include "rpg_stream_task_base_synch.h"
#include "rpg_stream_streammodule_base.h"

// forward declaration(s)
class RPG_Net_SessionMessage;
class RPG_Net_Message;

class RPG_Net_Module_HeaderParser
 : public RPG_Stream_TaskBaseSynch<RPG_Net_SessionMessage,
                                   RPG_Net_Message>
{
 public:
  RPG_Net_Module_HeaderParser();
  virtual ~RPG_Net_Module_HeaderParser();

  // initialization
  const bool init();

  // implement (part of) Stream_ITaskBase
  virtual void handleDataMessage(RPG_Net_Message*&, // data message handle
                                 bool&);            // return value: pass message downstream ?

  // implement RPG_Common_IDumpState
  virtual void dump_state() const;

 private:
  typedef RPG_Stream_TaskBaseSynch<RPG_Net_SessionMessage,
                                   RPG_Net_Message> inherited;

  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Module_HeaderParser(const RPG_Net_Module_HeaderParser&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Module_HeaderParser& operator=(const RPG_Net_Module_HeaderParser&));

  bool myIsInitialized;
};

// declare module
DATASTREAM_MODULE_INPUT_ONLY(RPG_Net_Module_HeaderParser);

#endif
