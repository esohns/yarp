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

#include "common_parser_defines.h"

#define RPG_NET_PROTOCOL_MAXIMUM_FRAME_SIZE            8192

// *NOTE*: according to RFC1459, IRC messages SHALL not exceed 512 bytes.
// - a size of 512 bytes will allow "crunching" messages into a single buffer
//   --> while this arguably "wastes" some memory, it allows [easier/more robust]
//       scanning / parsing...
// - provide an extra 2 '\0' "resilience" bytes needed for scanning with "flex"
// *WARNING*: be aware that a single read from the connected socket may well
// cover MORE than one complete message at a time, so this value is just a
// (somewhat qualified) suggestion...
//#define RPG_NET_PROTOCOL_BUFFER_SIZE                   (IRC_MAXIMUM_FRAME_SIZE + COMMON_PARSER_FLEX_BUFFER_BOUNDARY_SIZE)

#define RPG_NET_PROTOCOL_BUFFER_SIZE                   (RPG_NET_PROTOCOL_MAXIMUM_FRAME_SIZE + COMMON_PARSER_FLEX_BUFFER_BOUNDARY_SIZE)

// "crunch" messages for easier parsing ?
// *NOTE*: comes at the cost of alloc/free, memcopy and locking per fragmented
// message --> should probably be avoided ...
// OTOH, setting up the buffer correctly allows using the yy_scan_buffer()
// (instead of yy_scan_bytes()) call, avoiding a copy of the data at that stage
// --> adding the easier/more robust parsing, this MAY be a viable tradeoff...
// *NOTE*: the current implementation uses both approaches in different phases:
// - yy_scan_bytes (extra copy) for bisecting the frames
// - yy_scan_buffer (crunching) during parsing/analysis
// *TODO*: write a (robust) flex-scanner/bison parser that can handle
// switching of buffers/"backing-up" reliably and stress-test the application
// to see which option proves to be better...
//#define RPG_NET_PROTOCOL_DEF_CRUNCH_MESSAGES           true

// output more debugging information
#define RPG_NET_PROTOCOL_DEF_TRACE_SCANNING            false
#define RPG_NET_PROTOCOL_DEF_TRACE_PARSING             false

// // default IRC configuration
// *NOTE*: bitset: "1100" [2]: +w; [3]: +i (see rfc2812 3.1.3/3.1.5 and rfc1459 4.1.3)
//#define RPG_NET_PROTOCOL_DEF_IRC_USERMODE              0 // (!wallops && !invisible)
//// #define RPG_NET_PROTOCOL_DEF_IRC_HOSTNAME              "localhost"
//// #define RPG_NET_PROTOCOL_DEF_IRC_SERVERNAME            "localhost"
//#define RPG_NET_PROTOCOL_DEF_IRC_SERVERNAME            "*"

// chat-specific
#define RPG_NET_PROTOCOL_DEF_SERVER                    "localhost"
#define RPG_NET_PROTOCOL_DEF_PORT                      6667
#define RPG_NET_PROTOCOL_DEF_PASSWORD                  "secretpasswordhere"
#define RPG_NET_PROTOCOL_DEF_NICKNAME                  "Wiz"
#define RPG_NET_PROTOCOL_DEF_USERNAME                  "user"
#define RPG_NET_PROTOCOL_DEF_REALNAME                  "Ronnie Reagan"
#define RPG_NET_PROTOCOL_DEF_CHANNEL                   "#foobar"

#endif
