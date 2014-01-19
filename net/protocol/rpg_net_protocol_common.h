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

#include "rpg_net_protocol_exports.h"

#include "rpg_net_connection_manager.h"

#include <ace/Time_Value.h>
#include <ace/Date_Time.h>
#include <ace/Singleton.h>
#include <ace/Synch.h>
#include <ace/Module.h>

#include <string>
#include <set>
#include <vector>
#include <map>
#include <bitset>

// forward declaration(s)
class ACE_Notification_Strategy;
class RPG_Stream_IAllocator;
class RPG_Stream_Module;
template <typename ConfigType,
          typename StatisticsContainerType> class RPG_Net_Connection_Manager;

typedef ACE_Module<ACE_MT_SYNCH> MODULE_TYPE;

struct RPG_Net_Protocol_IRCLoginOptions
{
  std::string password;
  std::string nick;
  struct User
  {
    std::string username;
    struct Hostname
    {
      union
      {
        // *NOTE*: "traditional" connects (see RFC1459 Section 4.1.3)
        std::string* string;
        // *NOTE*: "modern" connects (see RFC2812 Section 3.1.3)
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

// see (RFC1459 section 4.2.3.1)
//            o - give/take channel operator privileges
//            p - private channel flag
//            s - secret channel flag
//            i - invite-only channel flag
//            t - topic settable by channel operator only flag
//            n - no messages to channel from clients on the outside
//            m - moderated channel
//            l - set the user limit to channel
//            b - set a ban mask to keep users out
//            v - give/take the ability to speak on a moderated channel
//            k - set a channel key (password)
// see (RFC2812 section 4.2 and following)
//            a - anonymous channel ('&!'-channels only)
//            i - invite-only channel flag
//            m - moderated channel
//            n - no messages to channel from clients on the outside
//            q - quiet channel (server use only)
//            p - private channel flag
//            s - secret channel flag
//            r - server reop flag ('!'-channels only)
//            t - topic settable by channel operator only flag
//            l - set the user limit to channel
//            k - set a channel key (password)
enum RPG_Net_Protocol_ChannelMode
{
  CHANNELMODE_PASSWORD = 0,
  CHANNELMODE_VOICE,
  CHANNELMODE_BAN,
  CHANNELMODE_USERLIMIT,
  CHANNELMODE_MODERATED,
  CHANNELMODE_BLOCKFOREIGNMSGS,
  CHANNELMODE_RESTRICTEDTOPIC,
  CHANNELMODE_INVITEONLY,
  CHANNELMODE_SECRET,
  CHANNELMODE_PRIVATE,
  CHANNELMODE_OPERATOR,
  //
  CHANNELMODE_MAX,
  CHANNELMODE_INVALID
};
typedef std::bitset<11> RPG_Net_Protocol_ChannelModes_t;

// see (RFC1459 section 4.2.3.2)
//            i - marks a users as invisible
//            s - marks a user for receipt of server notices
//            w - user receives wallops
//            o - operator flag
// see (RFC2812 section 3.1.5)
//            a - user is flagged as away (server-to-server only)
//            i - marks a users as invisible
//            w - user receives wallops
//            r - restricted user connection
//            o - operator flag
//            O - local operator flag
//            s - marks a user for receipt of server notices
enum RPG_Net_Protocol_UserMode
{
  USERMODE_LOCALOPERATOR = 0,
  USERMODE_OPERATOR,
  USERMODE_RESTRICTEDCONN,
  USERMODE_RECVWALLOPS,
  USERMODE_RECVNOTICES,
  USERMODE_INVISIBLE,
  USERMODE_AWAY,
  //
  USERMODE_MAX,
  USERMODE_INVALID
};
typedef std::bitset<7> RPG_Net_Protocol_UserModes_t;

// phonebook
typedef std::set<std::string> RPG_Net_Protocol_Networks_t;
typedef RPG_Net_Protocol_Networks_t::const_iterator RPG_Net_Protocol_NetworksIterator_t;
typedef std::pair<unsigned short, unsigned short> RPG_Net_Protocol_PortRange_t;
typedef std::vector<RPG_Net_Protocol_PortRange_t> RPG_Net_Protocol_PortRanges_t;
typedef RPG_Net_Protocol_PortRanges_t::const_iterator RPG_Net_Protocol_PortRangesIterator_t;
struct RPG_Net_Protocol_ConnectionEntry
{
  std::string                   hostName;
  RPG_Net_Protocol_PortRanges_t listeningPorts;
  std::string                   network;
};
typedef std::multimap<std::string, RPG_Net_Protocol_ConnectionEntry> RPG_Net_Protocol_Servers_t;
typedef RPG_Net_Protocol_Servers_t::const_iterator RPG_Net_Protocol_ServersIterator_t;
struct RPG_Net_Protocol_PhoneBook
{
  ACE_Date_Time               timestamp;
  // *NOTE*: this member is redundant (but still useful ?)...
  RPG_Net_Protocol_Networks_t networks;
  RPG_Net_Protocol_Servers_t  servers;
};

struct RPG_Net_Protocol_RuntimeStatistic
{
  unsigned int numDataMessages; // (protocol) messages
  double       numBytes;        // amount of processed data

  // convenience
  inline RPG_Net_Protocol_RuntimeStatistic operator+=(const RPG_Net_Protocol_RuntimeStatistic& rhs_in)
  {
    numDataMessages += rhs_in.numDataMessages;
    numBytes += rhs_in.numBytes;

    return *this;
  };
};

struct RPG_Net_Protocol_ConfigPOD
{
  // ************ connection config data ************
  int                               socketBufferSize;
  RPG_Stream_IAllocator*            messageAllocator;
  unsigned int                      bufferSize;
  bool                              useThreadPerConnection; // used by the server...
  // ************ protocol config data **************
  unsigned int                      clientPingInterval; // used by the server...
  RPG_Net_Protocol_IRCLoginOptions  loginOptions;
  // ************ stream config data ****************
  ACE_Notification_Strategy*        notificationStrategy;
  MODULE_TYPE*                      module;
  bool                              crunchMessageBuffers;
  bool                              debugScanner;
  bool                              debugParser;
  unsigned int                      sessionID; // (== socket handle !)
  unsigned int                      statisticsReportingInterval;
  // ************ runtime statistics data ***********
  RPG_Net_Protocol_RuntimeStatistic currentStatistics;
  ACE_Time_Value                    lastCollectionTimestamp;
};

typedef RPG_Net_Connection_Manager<RPG_Net_Protocol_ConfigPOD,
                                   RPG_Net_Protocol_RuntimeStatistic> RPG_Net_Protocol_Connection_Manager_t;
typedef ACE_Singleton<RPG_Net_Protocol_Connection_Manager_t,
                      ACE_Recursive_Thread_Mutex> RPG_PROTOCOL_CONNECTIONMANAGER_SINGLETON;
RPG_PROTOCOL_SINGLETON_DECLARE(ACE_Singleton,
			       RPG_Net_Protocol_Connection_Manager_t,
			       ACE_Recursive_Thread_Mutex);

#endif
