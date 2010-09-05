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

#include "rpg_net_protocol_inotify.h"

#include <ace/Global_Macros.h>
#include <ace/Synch.h>

#include <string>
#include <deque>

// forward declaration(s)
typedef struct _GtkBuilder GtkBuilder;
typedef struct _GtkTextBuffer GtkTextBuffer;
typedef struct _GtkTextView GtkTextView;

class IRC_Client_GUI_MessageHandler
 : public RPG_Net_Protocol_INotify
{
 public:
  IRC_Client_GUI_MessageHandler(GtkBuilder*); // widget tree handler
  virtual ~IRC_Client_GUI_MessageHandler();

  // asynch start of session (connection has been opened)
  virtual void start();
  // asynch arrival of data
  virtual void notify(const RPG_Net_Protocol_IRCMessage&); // message data
  // asynch end of session (connection has been closed)
  virtual void end();
  // display (local) text
  void queueForDisplay(const std::string&);
  // *WARNING*: to be called from gtk_main (trigger with g_idle_add())
  // --> do NOT invoke this from any other context (GTK is NOT threadsafe)
  void update();

 private:
  typedef RPG_Net_Protocol_INotify inherited;

  // safety measures
  ACE_UNIMPLEMENTED_FUNC(IRC_Client_GUI_MessageHandler());
  ACE_UNIMPLEMENTED_FUNC(IRC_Client_GUI_MessageHandler(const IRC_Client_GUI_MessageHandler&));
  ACE_UNIMPLEMENTED_FUNC(IRC_Client_GUI_MessageHandler& operator=(const IRC_Client_GUI_MessageHandler&));

  ACE_Thread_Mutex        myLock;
  std::deque<std::string> myDisplayQueue;

  bool                    myGtkInitialized;
  GtkBuilder*             myBuilder;
  GtkTextView*            myTargetView;
  GtkTextBuffer*          myTargetBuffer;
};

#endif
