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

#include "rpg_net_module_eventhandler.h"

#include "ace/Log_Msg.h"

#include "rpg_common_macros.h"

RPG_Net_MessageHandler::RPG_Net_MessageHandler (ISTREAM_T* stream_in)
 : inherited (stream_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_MessageHandler::RPG_Net_MessageHandler"));

}

//Stream_Module_t*
//RPG_Net_MessageHandler::clone ()
//{
//  RPG_TRACE (ACE_TEXT ("RPG_Net_MessageHandler::clone"));
//
//  // initialize return value(s)
//  Stream_Module_t* module_p = NULL;
//
//  ACE_NEW_NORETURN (module_p,
//                    RPG_Net_MessageHandler_Module (NULL, // *TODO*: pass stream handle
//                                                 ACE_TEXT_ALWAYS_CHAR (inherited::name ()),
//                    NULL));
//  if (unlikely (!module_p))
//    ACE_DEBUG ((LM_CRITICAL,
//                ACE_TEXT ("failed to allocate memory(%u): %m, aborting\n"),
//                sizeof (RPG_Net_MessageHandler_Module)));
//  else
//  {
//    RPG_Net_MessageHandler* eventHandler_impl = NULL;
//    eventHandler_impl =
//      dynamic_cast<RPG_Net_MessageHandler*> (module_p->writer ());
//    if (unlikely (!eventHandler_impl))
//    {
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("dynamic_cast<RPG_Net_MessageHandler> failed, aborting\n")));
//      delete module_p; module_p = NULL;
//      return NULL;
//    } // end IF
//    eventHandler_impl->initialize (inherited::subscribers_,
//                                   inherited::lock_);
//  } // end ELSE
//
//  return module_p;
//}
