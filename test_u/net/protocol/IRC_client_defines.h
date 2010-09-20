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

#ifndef IRC_CLIENT_DEFINES_H
#define IRC_CLIENT_DEFINES_H

#define IRC_CLIENT_CNF_DEF_INI_FILE              ACE_TEXT("./IRC_client.ini")
// ini section headers
#define IRC_CLIENT_CNF_LOGIN_SECTION_HEADER      ACE_TEXT("login")
#define IRC_CLIENT_CNF_CONNECTION_SECTION_HEADER ACE_TEXT("connections")

// phonebook section headers
#define IRC_CLIENT_CNF_TIMESTAMP_SECTION_HEADER  ACE_TEXT("timestamp")
#define IRC_CLIENT_CNF_NETWORKS_SECTION_HEADER   ACE_TEXT("networks")
#define IRC_CLIENT_CNF_SERVERS_SECTION_HEADER    ACE_TEXT("servers")

#define IRC_CLIENT_DEF_CLIENT_USES_TP            false
#define IRC_CLIENT_DEF_NUM_TP_THREADS            5

#define IRC_CLIENT_DEF_STATSINTERVAL             0 // seconds: 0 --> OFF

#define IRC_CLIENT_DEF_SERVER_HOSTNAME           ACE_LOCALHOST
#define IRC_CLIENT_DEF_SERVER_PORT               6667

// use traditional/modern USER message syntax for connections ?
// *NOTE*: refer to RFC1459 Section 4.1.3 - RFC2812 Section 3.1.3
#define IRC_CLIENT_CNF_IRC_USERMSG_TRADITIONAL   false
// *NOTE*: hybrid-7.2.3 seems to have a bug: 0,4,8 --> +i
// *TODO*: --> ./etc/ircd.conf ?
#define IRC_CLIENT_DEF_IRC_USERMODE              0
#define IRC_CLIENT_DEF_IRC_NICK                  ACE_TEXT_ALWAYS_CHAR("Wiz")
#define IRC_CLIENT_DEF_IRC_CHANNEL               ACE_TEXT_ALWAYS_CHAR("#foobar")
#define IRC_CLIENT_DEF_IRC_LEAVE_REASON          ACE_TEXT("quitting...")

#endif