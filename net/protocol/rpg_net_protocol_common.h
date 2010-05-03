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

#ifndef RPG_NET_PROTOCOL_COMMON_H
#define RPG_NET_PROTOCOL_COMMON_H

#include "rpg_net_protocol_IRCmessage.h"

#include <stream_streammodule_base.h>

#include <ace/Time_Value.h>
#include <ace/Singleton.h>
#include <ace/Synch.h>

#include <string>

// forward declaration(s)
class Stream_IAllocator;
class Stream_Module;
template <typename ConfigType,
          typename StatisticsContainerType> class RPG_Net_Connection_Manager;
class RPG_Net_Protocol_Module_IRCSplitter;
class RPG_Net_Protocol_Module_IRCStreamer;
class RPG_Net_Protocol_SessionMessage;
class RPG_Net_Protocol_Message;
template <typename SessionMessageType,
          typename ProtocolMessageType,
          typename ProtocolCommandType,
          typename StatisticsContainerType> class RPG_Net_Module_RuntimeStatistic;

struct RPG_Net_Protocol_RuntimeStatistic
{
  unsigned long numDataMessages; // (protocol) messages
  double        numBytes;        // amount of processed data

  // convenience
  inline RPG_Net_Protocol_RuntimeStatistic operator+=(const RPG_Net_Protocol_RuntimeStatistic& rhs)
  {
    numDataMessages += rhs.numDataMessages;
    numBytes += rhs.numBytes;

    return *this;
  };
};

struct RPG_Net_Protocol_IRCLoginOptions
{
  // *NOTE*: corresponds to the [client] section of .ini file
  std::string password;
  std::string nick;
  struct User
  {
    std::string username;
    struct Hostname
    {
      union
      {
        std::string* string;
        unsigned char mode;
      };
      enum discriminator_t
      {
        STRING = 0,
        BITMASK,
        INVALID
      };
      discriminator_t discriminator;
    } hostname;
    std::string servername;
    std::string realname;
  } user;
  std::string channel;
};

struct RPG_Net_Protocol_ConfigPOD
{
  // ************ connection config data ************
  int                               socketBufferSize;
  Stream_IAllocator*                messageAllocator;
  unsigned long                     defaultBufferSize;
  // ************ protocol config data **************
  unsigned long                     clientPingInterval; // used by the server...
  RPG_Net_Protocol_IRCLoginOptions  loginOptions;
  // ************ stream config data ****************
  Stream_Module*                    module;
  bool                              debugParser;
  unsigned long                     sessionID; // (== socket handle !)
  unsigned long                     statisticsReportingInterval;
  // ************ runtime statistics data ***********
  RPG_Net_Protocol_RuntimeStatistic currentStatistics;
  ACE_Time_Value                    lastCollectionTimestamp;
};

typedef ACE_Singleton<RPG_Net_Connection_Manager<RPG_Net_Protocol_ConfigPOD,
                                                 RPG_Net_Protocol_RuntimeStatistic>,
                      ACE_Recursive_Thread_Mutex> RPG_NET_PROTOCOL_CONNECTIONMANAGER_SINGLETON;

// declare module(s)
DATASTREAM_MODULE_DUPLEX(RPG_Net_Protocol_Module_IRCSplitter,
                         RPG_Net_Protocol_Module_IRCStreamer,
                         RPG_Net_Protocol_Module_IRCMarshal);
typedef RPG_Net_Module_RuntimeStatistic<RPG_Net_Protocol_SessionMessage,
                                        RPG_Net_Protocol_Message,
                                        RPG_Net_Protocol_CommandType_t,
                                        RPG_Net_Protocol_RuntimeStatistic> RPG_NET_PROTOCOL_MODULE_RUNTIMESTATISTICS_T;
DATASTREAM_MODULE_WRITER_ONLY_T(RPG_NET_PROTOCOL_MODULE_RUNTIMESTATISTICS_T,
                                RPG_Net_Module_RuntimeStatistic);

#endif
