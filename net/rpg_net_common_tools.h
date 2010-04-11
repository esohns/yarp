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

#ifndef RPG_NET_COMMON_TOOLS_H
#define RPG_NET_COMMON_TOOLS_H

#include "rpg_net_remote_comm.h"

#include <ace/Global_Macros.h>
#include <ace/Signal.h>

#include <string>

// forward declaration(s)
struct dirent;

class RPG_Net_Common_Tools
{
 public:
  // *NOTE*: this implements log rotation...
  static const bool getNextLogFilename(const bool&,        // server ?
                                       const std::string&, // log directory
                                       std::string&);      // return value: FQ current log filename
  static unsigned long myMaxNumberOfLogFiles;

  // debug info
  static const std::string messageType2String(const RPG_Net_Remote_Comm::MessageType&);

  // --- general tools ---
  // *NOTE*: if (the first argument == 0), the trailing ":0" will be cropped from the return value !
  static const std::string IPAddress2String(const unsigned short&, // port (network byte order !)
                                            const unsigned long&); // IP address (network byte order !)
  static const std::string IPProtocol2String(const unsigned char&); // protocol
  static const std::string MACAddress2String(const char* const); // pointer to message data (START of ethernet header address field !)
  static const std::string EthernetProtocolTypeID2String(const unsigned short&); // ethernet frame type (network byte order !)

//   static const bool selectNetworkInterface(const std::string&, // default interface identifier
//                                            std::string&);      // return value: interface identifier
  static const bool retrieveLocalIPAddress(const std::string&, // interface identifier
                                           std::string&);      // return value: IP address (dotted-decimal)
  static const bool retrieveLocalHostname(std::string&); // return value: hostname
  static const bool setSocketBuffer(const ACE_HANDLE&, // socket handle
                                    const int&,        // option (SO_RCVBUF || SO_SNDBUF)
                                    const int&);       // size (bytes)
  // *NOTE*: this should toggle Nagle's algorithm
  static const bool setNoDelay(const ACE_HANDLE&, // socket handle
                               const bool&);      // TCP_NODELAY ?

  static void retrieveSignalInfo(const int&,        // signal
                                 const siginfo_t&,  // information pertaining to a signal
//                               const ucontext_t&, // context information
                                 std::string&);     // return value: condensed signal information

 private:
  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Common_Tools());
  ACE_UNIMPLEMENTED_FUNC(virtual ~RPG_Net_Common_Tools());
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Common_Tools(const RPG_Net_Common_Tools&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_Common_Tools& operator=(const RPG_Net_Common_Tools&));

  // callback used for scandir...
  static int client_selector(const dirent*); // directory entry
  static int server_selector(const dirent*); // directory entry
  static int comparator(const dirent**,  // directory entry
                        const dirent**); // directory entry
};

#endif
