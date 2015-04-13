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
#include "ace/Synch.h"

#include "stream_common.h"

#include "net_message.h"
#include "net_messagehandler_base.h"

#include "rpg_net_exports.h"

class RPG_Net_Export RPG_Net_EventHandler
 : public Net_MessageHandlerBase_T<Stream_ModuleConfiguration_t,
                                   Net_Message>
{
 public:
  RPG_Net_EventHandler ();
  virtual ~RPG_Net_EventHandler ();

  // override Common_INotify_T
  virtual void start (const Stream_ModuleConfiguration_t&);
  virtual void notify (const Net_Message&);
  virtual void end ();

 private:
  typedef Net_MessageHandlerBase_T<Stream_ModuleConfiguration_t,
                                   Net_Message> inherited;

  ACE_UNIMPLEMENTED_FUNC (RPG_Net_EventHandler (const RPG_Net_EventHandler&));
  ACE_UNIMPLEMENTED_FUNC (RPG_Net_EventHandler& operator= (const RPG_Net_EventHandler&));
};

#endif
