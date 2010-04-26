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

#ifndef RPG_NET_PROTOCOL_DEFINES_H
#define RPG_NET_PROTOCOL_DEFINES_H

// *NOTE*: according to RFC1459, IRC messages SHALL not exceed 512 bytes.
// - a size of 512 bytes will allow "crunching" messages into a single buffer
//   --> while this arguably "wastes" some memory, it allows easier
//       scanning / parsing...
// - provide an extra 2 '\0' "resilience" bytes needed for scanning with "flex"
// *WARNING*: be aware that a single read from the connected socket may well
// cover MORE than one complete message at a time, so this value is just a
// (somewhat balanced) suggestion...
#define RPG_NET_PROTOCOL_DEF_NETWORK_BUFFER_SIZE   (512 + 2)

// "\0\0"
#define RPG_NET_PROTOCOL_FLEX_BUFFER_BOUNDARY_SIZE 2
// CRLF = "\r\n"
#define RPG_NET_PROTOCOL_IRC_FRAME_BOUNDARY        ACE_TEXT_ALWAYS_CHAR("\r\n")
#define RPG_NET_PROTOCOL_IRC_FRAME_BOUNDARY_SIZE   2

// output more debugging information
#define RPG_NET_PROTOCOL_DEF_TRACE_SCANNING        false
#define RPG_NET_PROTOCOL_DEF_TRACE_PARSING         false

#endif
