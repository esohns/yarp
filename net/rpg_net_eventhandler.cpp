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
#include "stdafx.h"

#include "rpg_net_eventhandler.h"

#include "rpg_common_macros.h"

RPG_Net_EventHandler::RPG_Net_EventHandler ()
 : inherited ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_EventHandler::RPG_Net_EventHandler"));

}

RPG_Net_EventHandler::~RPG_Net_EventHandler ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_EventHandler::~RPG_Net_EventHandler"));

}

void
RPG_Net_EventHandler::start (const Stream_ModuleConfiguration_t& configuration_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_EventHandler::start"));

  ACE_UNUSED_ARG (configuration_in);
}

void
RPG_Net_EventHandler::notify (const Net_Message& message_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_EventHandler::notify"));

  ACE_UNUSED_ARG (message_in);
}

void
RPG_Net_EventHandler::end ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_EventHandler::end"));

}
