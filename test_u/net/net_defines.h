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

#ifndef NET_DEFINES_H
#define NET_DEFINES_H

#define NET_UI_GTKEVENT_RESOLUTION              20 // ms
#define NET_UI_DIALOG_NAME                      "dialog"
#define NET_UI_ABOUTDIALOG_NAME                 "aboutdialog"
#define NET_UI_NUMCONNECTIONS_NAME              "label_num_connections"
#define NET_UI_TEXTVIEW_NAME                    "textview"
#define NET_UI_SCROLLMARK_NAME                  "insert"
#define NET_UI_ACTIONS_NAME                     "vbuttonbox"
#define NET_UI_LOG_FONTDESCRIPTION              "Monospace 10"
#define NET_UI_LOG_BASE                         "#000000" // black
#define NET_UI_LOG_TEXT                         "#FFFFFF" // white

// -----------------------------------------------------------------------------

#define NET_CLIENT_DEF_MAX_NUM_OPEN_CONNECTIONS 0
#define NET_CLIENT_DEF_SERVER_HOSTNAME          ACE_LOCALHOST
#define NET_CLIENT_DEF_SERVER_CONNECT_INTERVAL  0
#define NET_CLIENT_DEF_SERVER_PING_INTERVAL     0 // ms {0 --> OFF}
#define NET_CLIENT_DEF_SERVER_STRESS_INTERVAL   50 // ms

#define NET_CLIENT_DEF_UI_FILE                  "net_client.glade"
#define NET_CLIENT_UI_CONNECTBUTTON_NAME        "button_connect"
#define NET_CLIENT_UI_CLOSEBUTTON_NAME          "button_close"
#define NET_CLIENT_UI_CLOSEALLBUTTON_NAME       "button_close_all"
#define NET_CLIENT_UI_PINGBUTTON_NAME           "button_ping"
#define NET_CLIENT_UI_ABOUTBUTTON_NAME          "button_about"
#define NET_CLIENT_UI_QUITBUTTON_NAME           "button_quit"

// define behaviour
#define NET_CLIENT_U_TEST_CONNECT_PROBABILITY   0.02F
#define NET_CLIENT_U_TEST_ABORT_PROBABILITY     0.01F

// -----------------------------------------------------------------------------

#define NET_SERVER_DEF_UI_FILE                  "net_server.glade"
#define NET_SERVER_UI_STARTBUTTON_NAME          "button_start"
#define NET_SERVER_UI_STOPBUTTON_NAME           "button_stop"
#define NET_SERVER_UI_CLOSEALLBUTTON_NAME       "button_close_all"
#define NET_SERVER_UI_REPORTBUTTON_NAME         "button_report"

#endif
