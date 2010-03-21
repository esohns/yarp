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

#ifndef RPG_NET_DEFINES_H
#define RPG_NET_DEFINES_H

// trace log
// *PORTABILITY*: pathnames are not portable, so we (try to) use %TEMP% for Windows...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#define RPG_NET_DEF_LOG_DIRECTORY            ACE_OS::getenv(ACE_TEXT_ALWAYS_CHAR("TEMP"))
#else
#define RPG_NET_DEF_LOG_DIRECTORY            ACE_TEXT("/var/tmp")
#endif
#define RPG_NET_DEF_LOG_SERVER_FILENAME_PREFIX      ACE_TEXT("net_server")
#define RPG_NET_DEF_LOG_CLIENT_FILENAME_PREFIX      ACE_TEXT("net_client")
#define RPG_NET_DEF_LOG_FILENAME_SUFFIX      ACE_TEXT(".log")
// *IMPORTANT NOTE*:
// - WARNING: current implementation cannot support numbers that have
//   more than 7 digits !!!
// - WARNING: current implementation cannot support 0 !!!
#define RPG_NET_DEF_LOG_MAXNUMFILES          5

#define RPG_NET_DEF_KEEPALIVE                60
#define RPG_NET_DEF_PING_INTERVAL            5
#define RPG_NET_DEF_LISTENING_PORT           10101
#define RPG_NET_DEF_MAX_NUM_OPEN_CONNECTIONS 10

#endif
