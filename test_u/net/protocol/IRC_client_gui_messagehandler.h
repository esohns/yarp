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

#ifndef IRC_CLIENT_GUI_MESSAGEHANDLER_H
#define IRC_CLIENT_GUI_MESSAGEHANDLER_H

#include "IRC_client_gui_common.h"

#include <rpg_net_protocol_iIRCControl.h>

#include <gtk/gtk.h>

#include <ace/Global_Macros.h>
#include <ace/Synch.h>

#include <string>
#include <deque>

class IRC_Client_GUI_MessageHandler
{
 public:
  // ctor for default handler (== server log)
  // *WARNING*: make sure the ctor/dtor calls are made either:
  // - by the main thread (servicing the gtk_main event loop)
  // - protected by GDK_THREADS_ENTER/GDK_THREADS_LEAVE
  IRC_Client_GUI_MessageHandler(GtkTextView*);                 // text view
  IRC_Client_GUI_MessageHandler(RPG_Net_Protocol_IIRCControl*, // controller handle
                                const std::string&,            // channel
                                const std::string&,            // UI (glade) file directory
                                GtkNotebook*);                 // parent widget
  virtual ~IRC_Client_GUI_MessageHandler();

  // display (local) text
  void queueForDisplay(const std::string&);
  // *WARNING*: to be called from gtk_main (trigger with g_idle_add())
  // --> do NOT invoke this from any other context (GTK is NOT threadsafe)
  void update();

  // *WARNING*: returns the toplevel widget FOR THIS CHANNEL TAB
  // --> use with care !
  GtkWidget* getTopLevel();

  const std::string getChannel() const;
  void setTopic(const std::string&);
  void appendMembers(const string_list_t&);
  void endMembers();

 private:
  // safety measures
  ACE_UNIMPLEMENTED_FUNC(IRC_Client_GUI_MessageHandler());
  ACE_UNIMPLEMENTED_FUNC(IRC_Client_GUI_MessageHandler(const IRC_Client_GUI_MessageHandler&));
  ACE_UNIMPLEMENTED_FUNC(IRC_Client_GUI_MessageHandler& operator=(const IRC_Client_GUI_MessageHandler&));

  // helper methods
  void clearMembers();

  channel_cb_data_t       myCBData;

  ACE_Thread_Mutex        myLock;
  std::deque<std::string> myDisplayQueue;
  GtkTextView*            myView;

  bool                    myIsFirstNameListMsg;
  GtkNotebook*            myParent;
};

#endif
