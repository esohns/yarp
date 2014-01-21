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

#include "net_eventhandler.h"

#include "net_defines.h"
#include "net_common.h"

#include "rpg_client_ui_tools.h"

#include "rpg_net_common.h"

#include "rpg_common_macros.h"

#include <glade/glade.h>
#include <gtk/gtk.h>

#include <sstream>

Net_EventHandler::Net_EventHandler(Net_GTK_CBData_t* CBData_in)
 : myCBData(CBData_in)
{
  RPG_TRACE(ACE_TEXT("Net_EventHandler::Net_EventHandler"));

  // sanity check(s)
  ACE_ASSERT(myCBData);
}

Net_EventHandler::~Net_EventHandler()
{
  RPG_TRACE(ACE_TEXT("Net_EventHandler::~Net_EventHandler"));

}

void
Net_EventHandler::start()
{
  RPG_TRACE(ACE_TEXT("Net_EventHandler::start"));

  // sanity check(s)
  ACE_ASSERT(myCBData->xml);

  GDK_THREADS_ENTER();

  // update info label
  GtkLabel* label =
      GTK_LABEL(glade_xml_get_widget(myCBData->xml,
                                     ACE_TEXT_ALWAYS_CHAR(NET_UI_NUMCONNECTIONS_NAME)));
  if (!label)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to glade_xml_get_widget(\"%s\"): \"%m\", aborting\n"),
               ACE_TEXT_ALWAYS_CHAR(NET_UI_NUMCONNECTIONS_NAME)));

    // clean up
    GDK_THREADS_LEAVE();

    return;
  } // end IF
  std::stringstream converter;
  converter.str(ACE_TEXT_ALWAYS_CHAR(""));
  converter << RPG_NET_CONNECTIONMANAGER_SINGLETON::instance()->numConnections();
  gchar* converted_text = RPG_Client_UI_Tools::Locale2UTF8(converter.str());
  if (!converted_text)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to convert message text (was: \"%s\"), aborting\n"),
               converter.str().c_str()));

    // clean up
    GDK_THREADS_LEAVE();

    return;
  } // end IF
  gtk_label_set_text(label,
                     converted_text);

  // clean up
  g_free(converted_text);
  GDK_THREADS_LEAVE();
}

void
Net_EventHandler::notify(const RPG_Net_Message& message_in)
{
  RPG_TRACE(ACE_TEXT("Net_EventHandler::notify"));

  ACE_UNUSED_ARG(message_in);
}

void
Net_EventHandler::end()
{
  RPG_TRACE(ACE_TEXT("Net_EventHandler::end"));

  start();
}
