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

#ifndef TEST_U_NET_DEFINES_H
#define TEST_U_NET_DEFINES_H

#define NET_UI_GTKEVENT_RESOLUTION                200 // ms --> 5 FPS
#define NET_UI_GTK_SPINBUTTON_NUMCONNECTIONS_NAME "spinbutton"
#define NET_UI_GTK_TEXTVIEW_NAME                  "textview"
#define NET_UI_GTK_ADJUSTMENT_NAME                "scrolledwindow_vadjustment"
#define NET_UI_GTK_SCROLLMARK_NAME                "insert"
#define NET_UI_GTK_BUTTONBOX_ACTIONS_NAME         "vbuttonbox"
#define NET_UI_GTK_BUTTON_ABOUT_NAME              "about"
#define NET_UI_GTK_BUTTON_CLOSEALL_NAME           "close_all"
#define NET_UI_GTK_BUTTON_REPORT_NAME             "report"
#define NET_UI_GTK_BUTTON_QUIT_NAME               "quit"
#define NET_UI_LOG_FONTDESCRIPTION                "Monospace 10"
#define NET_UI_LOG_BASE                           "#000000" // black
#define NET_UI_LOG_TEXT                           "#FFFFFF" // white

// -----------------------------------------------------------------------------

#define NET_CLIENT_DEF_MAX_NUM_OPEN_CONNECTIONS   0
#define NET_CLIENT_DEF_SERVER_HOSTNAME            ACE_LOCALHOST
#define NET_CLIENT_DEF_SERVER_CONNECT_INTERVAL    0
#define NET_CLIENT_DEF_SERVER_PING_INTERVAL       0 // ms {0 --> OFF}
#define NET_CLIENT_DEF_SERVER_STRESS_INTERVAL     50 // ms

#define NET_CLIENT_UI_FILE                        "net_client.glade"
#define NET_CLIENT_UI_GTK_BUTTON_CONNECT_NAME     "connect"
#define NET_CLIENT_UI_GTK_BUTTON_CLOSE_NAME       "close"
#define NET_CLIENT_UI_GTK_BUTTON_PING_NAME        "ping"
#define NET_CLIENT_UI_GTK_BUTTON_STRESS_NAME      "stress"

// define behaviour
#define NET_CLIENT_U_TEST_CONNECT_PROBABILITY     0.2F // 20%
#define NET_CLIENT_U_TEST_ABORT_PROBABILITY       0.1F // 10%

// -----------------------------------------------------------------------------

#define NET_SERVER_UI_FILE                        "net_server.glade"
#define NET_SERVER_UI_GTK_BUTTON_START_NAME       "start"
#define NET_SERVER_UI_GTK_BUTTON_STOP_NAME        "stop"
#define NET_SERVER_UI_GTK_BUTTON_LISTEN_NAME      "listen"

#endif
