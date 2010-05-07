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

#include "IRC_client_gui_messagehandler.h"

#include <rpg_net_protocol_tools.h>

#include <gtk/gtk.h>

#include <string>

IRC_Client_GUI_MessageHandler::IRC_Client_GUI_MessageHandler(GtkTextBuffer* buffer_in)
 : myTargetBuffer(buffer_in)
{
  ACE_TRACE(ACE_TEXT("IRC_Client_GUI_MessageHandler::IRC_Client_GUI_MessageHandler"));

}

IRC_Client_GUI_MessageHandler::~IRC_Client_GUI_MessageHandler()
{
  ACE_TRACE(ACE_TEXT("IRC_Client_GUI_MessageHandler::~IRC_Client_GUI_MessageHandler"));

}

void
IRC_Client_GUI_MessageHandler::notify(const RPG_Net_Protocol_IRCMessage& message_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_SignalHandler::notify"));

  // sanity check(s)
  ACE_ASSERT(myTargetBuffer);

  std::string message_text = RPG_Net_Protocol_Tools::IRCMessage2String(message_in);
//   // always insert new text at the END of the buffer...
//   GtkTextIter iter;
//   gtk_text_buffer_get_end_iter(myTargetBuffer,
//                                &iter);
//   gtk_text_buffer_insert(myTargetBuffer,
//                          &iter,
//                          message_text.c_str(),
//                          message_text.size());
  gtk_text_buffer_insert_at_cursor(myTargetBuffer,
                                   message_text.c_str(),
                                   message_text.size());
}
