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

#ifndef RPG_Net_EventHandler_H
#define RPG_Net_EventHandler_H

#include "ace/Global_Macros.h"

#include "common.h"
#include "common_iclone.h"

#include "common_time_common.h"

#include "stream_common.h"
#include "stream_configuration.h"
#include "stream_streammodule_base.h"

#include "stream_misc_messagehandler.h"

#include "rpg_net_protocol_session_message.h"
#include "rpg_net_protocol_configuration.h"
#include "rpg_net_protocol_message.h"

class RPG_Net_EventHandler
 : public Stream_Module_MessageHandler_T<ACE_MT_SYNCH,
                                         Common_TimePolicy_t,
                                         struct RPG_Net_Protocol_ModuleHandlerConfiguration,
                                         Stream_ControlMessage_t,
                                         RPG_Net_Protocol_Message,
                                         RPG_Net_Protocol_SessionMessage,
                                         struct RPG_Net_Protocol_SessionData,
                                         struct Stream_UserData>
{
  typedef Stream_Module_MessageHandler_T<ACE_MT_SYNCH,
                                         Common_TimePolicy_t,
                                         struct RPG_Net_Protocol_ModuleHandlerConfiguration,
                                         Stream_ControlMessage_t,
                                         RPG_Net_Protocol_Message,
                                         RPG_Net_Protocol_SessionMessage,
                                         struct RPG_Net_Protocol_SessionData,
                                         struct Stream_UserData> inherited;

 public:
  RPG_Net_EventHandler (ISTREAM_T*); // stream handle
  inline virtual ~RPG_Net_EventHandler () {}

  // implement Common_IClone_T
  inline virtual ACE_Task<ACE_MT_SYNCH, Common_TimePolicy_t>* clone () { ACE_ASSERT (false); ACE_NOTSUP_RETURN (NULL); ACE_NOTREACHED (return NULL;) }
  //virtual Stream_Module_t* clone ();

 private:
  ACE_UNIMPLEMENTED_FUNC (RPG_Net_EventHandler ())
  ACE_UNIMPLEMENTED_FUNC (RPG_Net_EventHandler (const RPG_Net_EventHandler&))
  ACE_UNIMPLEMENTED_FUNC (RPG_Net_EventHandler& operator= (const RPG_Net_EventHandler&))
};

// declare module
DATASTREAM_MODULE_INPUT_ONLY (struct RPG_Net_Protocol_SessionData,       // session data type
                              enum Stream_SessionMessageType,            // session event type
                              struct RPG_Net_Protocol_ModuleHandlerConfiguration, // module handler configuration type
                              libacestream_default_misc_messagehandler_module_name_string,
                              Stream_INotify_t,                          // stream notification interface type
                              RPG_Net_EventHandler);                     // writer type

#endif
