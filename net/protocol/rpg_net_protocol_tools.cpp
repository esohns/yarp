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

#include "rpg_net_protocol_tools.h"

#include "rpg_net_protocol_common.h"

#include <rpg_common_macros.h>

#include <sstream>

const std::string
RPG_Net_Protocol_Tools::dump(const RPG_Net_Protocol_IRCMessage& message_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_Tools::dump"));

  std::ostringstream converter;
  // see RFC1459
  if (!message_in.prefix.origin.empty())
  {
    converter << ACE_TEXT("PREFIX [origin: \"") << message_in.prefix.origin << ACE_TEXT("\"]");
    if (!message_in.prefix.user.empty())
      converter << ACE_TEXT(", [user: \"") << message_in.prefix.user << ACE_TEXT("\"]");
    if (!message_in.prefix.host.empty())
      converter << ACE_TEXT(", [host: \"") << message_in.prefix.host << ACE_TEXT("\"]");
    converter << ACE_TEXT(" \\PREFIX") << std::endl;
  } // end IF
  converter << ACE_TEXT("COMMAND ");
  switch (message_in.command.discriminator)
  {
    case RPG_Net_Protocol_IRCMessage::Command::STRING:
    {
      converter << ACE_TEXT("[\"") << message_in.command.string->c_str() << ACE_TEXT("\"]");

      break;
    }
    case RPG_Net_Protocol_IRCMessage::Command::NUMERIC:
    {
      converter << ACE_TEXT("\"") << RPG_Net_Protocol_Tools::IRCCode2String(message_in.command.numeric) << ACE_TEXT("\"") << ACE_TEXT(" [") << message_in.command.numeric << ACE_TEXT("]");

      break;
    }
    // *NOTE*: command hasn't been set (yet)
    case RPG_Net_Protocol_IRCMessage::Command::INVALID:
    {
      converter << ACE_TEXT("\"INVALID\"");

      break;
    }
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid command discriminator (was: %d), aborting\n"),
                 message_in.command.discriminator));

      return std::string();
    }
  } // end SWITCH
  converter << ACE_TEXT(" \\COMMAND") << std::endl;
  if (!message_in.params.empty())
  {
    converter << ACE_TEXT("PARAMS") << std::endl;
    int i = 1;
    for (std::list<std::string>::const_iterator iterator = message_in.params.begin();
          iterator != message_in.params.end();
          iterator++, i++)
    {
      converter << ACE_TEXT("#") << i << ACE_TEXT(" : \"") << *iterator << ACE_TEXT("\"") << std::endl;
    } // end FOR
    converter << ACE_TEXT("\\PARAMS") << std::endl;
  } // end IF

  return converter.str();
}

const std::string
RPG_Net_Protocol_Tools::dump(const RPG_Net_Protocol_UserModes_t& userModes_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_Tools::dump"));

  // init result
  std::string result = ACE_TEXT_ALWAYS_CHAR("+");

  for (int i = 0;
       i < USERMODE_MAX;
       i++)
    if (userModes_in.test(i))
      result += RPG_Net_Protocol_Tools::IRCUserMode2Char(static_cast<RPG_Net_Protocol_UserMode> (i));

  return result;
}

const std::string
RPG_Net_Protocol_Tools::dump(const RPG_Net_Protocol_ChannelModes_t& channelModes_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_Tools::dump"));

  // init result
  std::string result = ACE_TEXT_ALWAYS_CHAR("+");

  for (int i = 0;
       i < CHANNELMODE_MAX;
       i++)
    if (channelModes_in.test(i))
      result += RPG_Net_Protocol_Tools::IRCChannelMode2Char(static_cast<RPG_Net_Protocol_ChannelMode> (i));

  return result;
}

const RPG_Net_Protocol_CommandType_t
RPG_Net_Protocol_Tools::IRCCommandString2Type(const std::string& commandString_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_Tools::IRCCommandString2Type"));

  if ((commandString_in) == ACE_TEXT("PASS"))
    return RPG_Net_Protocol_IRCMessage::PASS;
  else if ((commandString_in) == ACE_TEXT("NICK"))
    return RPG_Net_Protocol_IRCMessage::NICK;
  else if ((commandString_in) == ACE_TEXT("USER"))
    return RPG_Net_Protocol_IRCMessage::USER;
  else if ((commandString_in) == ACE_TEXT("SERVER"))
    return RPG_Net_Protocol_IRCMessage::SERVER;
  else if ((commandString_in) == ACE_TEXT("OPER"))
    return RPG_Net_Protocol_IRCMessage::OPER;
  else if ((commandString_in) == ACE_TEXT("QUIT"))
    return RPG_Net_Protocol_IRCMessage::QUIT;
  else if ((commandString_in) == ACE_TEXT("SQUIT"))
    return RPG_Net_Protocol_IRCMessage::SQUIT;
  else if ((commandString_in) == ACE_TEXT("JOIN"))
    return RPG_Net_Protocol_IRCMessage::JOIN;
  else if ((commandString_in) == ACE_TEXT("PART"))
    return RPG_Net_Protocol_IRCMessage::PART;
  else if ((commandString_in) == ACE_TEXT("MODE"))
    return RPG_Net_Protocol_IRCMessage::MODE;
  else if ((commandString_in) == ACE_TEXT("TOPIC"))
    return RPG_Net_Protocol_IRCMessage::TOPIC;
  else if ((commandString_in) == ACE_TEXT("NAMES"))
    return RPG_Net_Protocol_IRCMessage::NAMES;
  else if ((commandString_in) == ACE_TEXT("LIST"))
    return RPG_Net_Protocol_IRCMessage::LIST;
  else if ((commandString_in) == ACE_TEXT("INVITE"))
    return RPG_Net_Protocol_IRCMessage::INVITE;
  else if ((commandString_in) == ACE_TEXT("KICK"))
    return RPG_Net_Protocol_IRCMessage::KICK;
  else if ((commandString_in) == ACE_TEXT("VERSION"))
    return RPG_Net_Protocol_IRCMessage::SVERSION;
  else if ((commandString_in) == ACE_TEXT("STATS"))
    return RPG_Net_Protocol_IRCMessage::STATS;
  else if ((commandString_in) == ACE_TEXT("LINKS"))
    return RPG_Net_Protocol_IRCMessage::LINKS;
  else if ((commandString_in) == ACE_TEXT("TIME"))
    return RPG_Net_Protocol_IRCMessage::TIME;
  else if ((commandString_in) == ACE_TEXT("CONNECT"))
    return RPG_Net_Protocol_IRCMessage::CONNECT;
  else if ((commandString_in) == ACE_TEXT("TRACE"))
    return RPG_Net_Protocol_IRCMessage::TRACE;
  else if ((commandString_in) == ACE_TEXT("ADMIN"))
    return RPG_Net_Protocol_IRCMessage::ADMIN;
  else if ((commandString_in) == ACE_TEXT("INFO"))
    return RPG_Net_Protocol_IRCMessage::INFO;
  else if ((commandString_in) == ACE_TEXT("PRIVMSG"))
    return RPG_Net_Protocol_IRCMessage::PRIVMSG;
  else if ((commandString_in) == ACE_TEXT("NOTICE"))
    return RPG_Net_Protocol_IRCMessage::NOTICE;
  else if ((commandString_in) == ACE_TEXT("WHO"))
    return RPG_Net_Protocol_IRCMessage::WHO;
  else if ((commandString_in) == ACE_TEXT("WHOIS"))
    return RPG_Net_Protocol_IRCMessage::WHOIS;
  else if ((commandString_in) == ACE_TEXT("WHOWAS"))
    return RPG_Net_Protocol_IRCMessage::WHOWAS;
  else if ((commandString_in) == ACE_TEXT("KILL"))
    return RPG_Net_Protocol_IRCMessage::KILL;
  else if (commandString_in == ACE_TEXT("PING"))
    return RPG_Net_Protocol_IRCMessage::PING;
  else if (commandString_in == ACE_TEXT("PONG"))
    return RPG_Net_Protocol_IRCMessage::PONG;
  else if (commandString_in == ACE_TEXT("ERROR"))
    return RPG_Net_Protocol_IRCMessage::ERROR;
  else if ((commandString_in) == ACE_TEXT("AWAY"))
    return RPG_Net_Protocol_IRCMessage::AWAY;
  else if ((commandString_in) == ACE_TEXT("REHASH"))
    return RPG_Net_Protocol_IRCMessage::REHASH;
  else if ((commandString_in) == ACE_TEXT("RESTART"))
    return RPG_Net_Protocol_IRCMessage::RESTART;
  else if ((commandString_in) == ACE_TEXT("SUMMON"))
    return RPG_Net_Protocol_IRCMessage::SUMMON;
  else if ((commandString_in) == ACE_TEXT("USERS"))
    return RPG_Net_Protocol_IRCMessage::USERS;
  else if (commandString_in == ACE_TEXT("WALLOPS"))
    return RPG_Net_Protocol_IRCMessage::WALLOPS;
  else if (commandString_in == ACE_TEXT("USERHOST"))
    return RPG_Net_Protocol_IRCMessage::USERHOST;
  else if (commandString_in == ACE_TEXT("ISON"))
    return RPG_Net_Protocol_IRCMessage::ISON;
  else
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid/unknown command (was: \"%s\"), aborting\n"),
               commandString_in.c_str()));

    return RPG_Net_Protocol_IRCMessage::RPG_NET_PROTOCOL_COMMANDTYPE_INVALID;
  } // end ELSE
}

void
RPG_Net_Protocol_Tools::merge(const std::string& modes_in,
                              RPG_Net_Protocol_UserModes_t& modes_inout)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_Tools::merge"));

  RPG_Net_Protocol_UserMode user_mode = USERMODE_INVALID;

  // *NOTE* format is {[+|-]|i|w|s|o}
  bool assign = (modes_in[0] == '+');
  std::string::const_iterator iterator = modes_in.begin();
  for (iterator++;
       iterator != modes_in.end();
       iterator++)
  {
    user_mode = RPG_Net_Protocol_Tools::IRCUserModeChar2UserMode(*iterator);
    if (user_mode >= USERMODE_MAX)
      continue; // unknown user mode...

    modes_inout.set(user_mode, assign);
  } // end FOR
}

void
RPG_Net_Protocol_Tools::merge(const std::string& modes_in,
                              RPG_Net_Protocol_ChannelModes_t& modes_inout)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_Tools::merge"));

  RPG_Net_Protocol_ChannelMode channel_mode = CHANNELMODE_INVALID;

  // *NOTE* format is {[+|-]|o|p|s|i|t|n|m|l|b|v|k}
  bool assign = (modes_in[0] == '+');
  std::string::const_iterator iterator = modes_in.begin();
  for (iterator++;
       iterator != modes_in.end();
       iterator++)
  {
    channel_mode = RPG_Net_Protocol_Tools::IRCChannelModeChar2ChannelMode(*iterator);
    if (channel_mode >= CHANNELMODE_MAX)
      continue; // unknown channel mode...

    modes_inout.set(channel_mode, assign);
  } // end FOR
}

const char
RPG_Net_Protocol_Tools::IRCChannelMode2Char(const RPG_Net_Protocol_ChannelMode& mode_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_Tools::IRCChannelMode2Char"));

  switch (mode_in)
  {
    case CHANNELMODE_PASSWORD:
      return 'k';
    case CHANNELMODE_VOICE:
      return 'v';
    case CHANNELMODE_BAN:
      return 'b';
    case CHANNELMODE_USERLIMIT:
      return 'l';
    case CHANNELMODE_MODERATED:
      return 'm';
    case CHANNELMODE_BLOCKFOREIGNMSGS:
      return 'n';
    case CHANNELMODE_RESTRICTEDTOPIC:
      return 't';
    case CHANNELMODE_INVITEONLY:
      return 'i';
    case CHANNELMODE_SECRET:
      return 's';
    case CHANNELMODE_PRIVATE:
      return 'p';
    case CHANNELMODE_OPERATOR:
      return 'o';
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid channel mode (was: \"%s\"), aborting\n"),
                 RPG_Net_Protocol_Tools::IRCChannelMode2String(mode_in).c_str()));

      break;
    }
  } // end SWITCH

  // what can we do ?
  return ' ';
}

const char
RPG_Net_Protocol_Tools::IRCUserMode2Char(const RPG_Net_Protocol_UserMode& mode_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_Tools::IRCUserMode2Char"));

  switch (mode_in)
  {
    case USERMODE_LOCALOPERATOR:
      return 'O';
    case USERMODE_OPERATOR:
      return 'o';
    case USERMODE_RESTRICTEDCONN:
      return 'r';
    case USERMODE_RECVWALLOPS:
      return 'w';
    case USERMODE_RECVNOTICES:
      return 's';
    case USERMODE_INVISIBLE:
      return 'i';
    case USERMODE_AWAY:
      return 'a';
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid user mode (was: \"%s\"), aborting\n"),
                 RPG_Net_Protocol_Tools::IRCUserMode2String(mode_in).c_str()));

      break;
    }
  } // end SWITCH

  // what can we do ?
  return ' ';
}

const bool
RPG_Net_Protocol_Tools::isValidIRCChannelName(const std::string& string_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_Tools::isValidIRCChannelName"));

  // *NOTE*: RFC1459:
  //   <channel>    ::= ('#' | '&') <chstring>
  //   <chstring>   ::= <any 8bit code except SPACE, BELL, NUL, CR, LF and
  //                     comma (',')>
  return ((string_in.find('#', 0) == 0) ||
          (string_in.find('&', 0) == 0));
}

const std::string
RPG_Net_Protocol_Tools::IRCCode2String(const RPG_Net_Protocol_IRCNumeric_t& numeric_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_Tools::IRCCode2String"));

  // init result
  std::string result = ACE_TEXT("INVALID/UNKNOWN");

  switch (numeric_in)
  {
    case RPG_Net_Protocol_IRC_Codes::RPL_WELCOME:
      result = ACE_TEXT("RPL_WELCOME"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_YOURHOST:
      result = ACE_TEXT("RPL_YOURHOST"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_CREATED:
      result = ACE_TEXT("RPL_CREATED"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_MYINFO:
      result = ACE_TEXT("RPL_MYINFO"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_PROTOCTL:
      result = ACE_TEXT("RPL_PROTOCTL"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_MAPMORE:
      result = ACE_TEXT("RPL_MAPMORE"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_MAPEND:
      result = ACE_TEXT("RPL_MAPEND"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_SNOMASK:
      result = ACE_TEXT("RPL_SNOMASK"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_STATMEMTOT:
      result = ACE_TEXT("RPL_STATMEMTOT"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_STATMEM:
      result = ACE_TEXT("RPL_STATMEM"); break;

    case RPG_Net_Protocol_IRC_Codes::RPL_YOURCOOKIE:
      result = ACE_TEXT("RPL_YOURCOOKIE"); break;

    case RPG_Net_Protocol_IRC_Codes::RPL_YOURID:
      result = ACE_TEXT("RPL_YOURID"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_SAVENICK:
      result = ACE_TEXT("RPL_SAVENICK"); break;

    case RPG_Net_Protocol_IRC_Codes::RPL_ATTEMPTINGJUNC:
      result = ACE_TEXT("RPL_ATTEMPTINGJUNC"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_ATTEMPTINGREROUTE:
      result = ACE_TEXT("RPL_ATTEMPTINGREROUTE"); break;

    case RPG_Net_Protocol_IRC_Codes::RPL_TRACELINK:
      result = ACE_TEXT("RPL_TRACELINK"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_TRACECONNECTING:
      result = ACE_TEXT("RPL_TRACECONNECTING"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_TRACEHANDSHAKE:
      result = ACE_TEXT("RPL_TRACEHANDSHAKE"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_TRACEUNKNOWN:
      result = ACE_TEXT("RPL_TRACEUNKNOWN"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_TRACEOPERATOR:
      result = ACE_TEXT("RPL_TRACEOPERATOR"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_TRACEUSER:
      result = ACE_TEXT("RPL_TRACEUSER"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_TRACESERVER:
      result = ACE_TEXT("RPL_TRACESERVER"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_TRACESERVICE:
      result = ACE_TEXT("RPL_TRACESERVICE"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_TRACENEWTYPE:
      result = ACE_TEXT("RPL_TRACENEWTYPE"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_TRACECLASS:
      result = ACE_TEXT("RPL_TRACECLASS"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_TRACERECONNECT:
      result = ACE_TEXT("RPL_TRACERECONNECT"); break;

    case RPG_Net_Protocol_IRC_Codes::RPL_STATSLINKINFO:
      result = ACE_TEXT("RPL_STATSLINKINFO"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_STATSCOMMANDS:
      result = ACE_TEXT("RPL_STATSCOMMANDS"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_STATSCLINE:
      result = ACE_TEXT("RPL_STATSCLINE"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_STATSNLINE:
      result = ACE_TEXT("RPL_STATSNLINE"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_STATSILINE:
      result = ACE_TEXT("RPL_STATSILINE"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_STATSKLINE:
      result = ACE_TEXT("RPL_STATSKLINE"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_STATSQLINE:
      result = ACE_TEXT("RPL_STATSQLINE"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_STATSYLINE:
      result = ACE_TEXT("RPL_STATSYLINE"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_ENDOFSTATS:
      result = ACE_TEXT("RPL_ENDOFSTATS"); break;

    case RPG_Net_Protocol_IRC_Codes::RPL_STATSPLINE:
      result = ACE_TEXT("RPL_STATSPLINE"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_UMODEIS:
      result = ACE_TEXT("RPL_UMODEIS"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_SQLINE_NICK:
      result = ACE_TEXT("RPL_SQLINE_NICK"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_STATSELINE:
      result = ACE_TEXT("RPL_STATSELINE"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_STATSFLINE:
      result = ACE_TEXT("RPL_STATSFLINE"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_STATSGLINE:
      result = ACE_TEXT("RPL_STATSGLINE"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_STATSCOUNT:
      result = ACE_TEXT("RPL_STATSCOUNT"); break;

    case RPG_Net_Protocol_IRC_Codes::RPL_SERVICEINFO:
      result = ACE_TEXT("RPL_SERVICEINFO"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_ENDOFSERVICES:
      result = ACE_TEXT("RPL_ENDOFSERVICES"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_SERVICE:
      result = ACE_TEXT("RPL_SERVICE"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_SERVLIST:
      result = ACE_TEXT("RPL_SERVLIST"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_SERVLISTEND:
      result = ACE_TEXT("RPL_SERVLISTEND"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_STATSVERBOSE:
      result = ACE_TEXT("RPL_STATSVERBOSE"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_STATSENGINE:
      result = ACE_TEXT("RPL_STATSENGINE"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_STATSIAUTH:
      result = ACE_TEXT("RPL_STATSIAUTH"); break;

    case RPG_Net_Protocol_IRC_Codes::RPL_STATSVLINE:
      result = ACE_TEXT("RPL_STATSVLINE"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_STATSLLINE:
      result = ACE_TEXT("RPL_STATSLLINE"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_STATSUPTIME:
      result = ACE_TEXT("RPL_STATSUPTIME"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_STATSOLINE:
      result = ACE_TEXT("RPL_STATSOLINE"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_STATSHLINE:
      result = ACE_TEXT("RPL_STATSHLINE"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_STATSSLINE:
      result = ACE_TEXT("RPL_STATSSLINE"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_STATSPING:
      result = ACE_TEXT("RPL_STATSPING"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_STATSBLINE:
      result = ACE_TEXT("RPL_STATSBLINE"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_STATSDEFINE:
      result = ACE_TEXT("RPL_STATSDEFINE"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_STATSDEBUG:
      result = ACE_TEXT("RPL_STATSDEBUG"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_STATSDLINE:
      result = ACE_TEXT("RPL_STATSDLINE"); break;

    case RPG_Net_Protocol_IRC_Codes::RPL_LUSERCLIENT:
      result = ACE_TEXT("RPL_LUSERCLIENT"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_LUSEROP:
      result = ACE_TEXT("RPL_LUSEROP"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_LUSERUNKNOWN:
      result = ACE_TEXT("RPL_LUSERUNKNOWN"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_LUSERCHANNELS:
      result = ACE_TEXT("RPL_LUSERCHANNELS"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_LUSERME:
      result = ACE_TEXT("RPL_LUSERME"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_ADMINME:
      result = ACE_TEXT("RPL_ADMINME"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_ADMINLOC1:
      result = ACE_TEXT("RPL_ADMINLOC1"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_ADMINLOC2:
      result = ACE_TEXT("RPL_ADMINLOC2"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_ADMINEMAIL:
      result = ACE_TEXT("RPL_ADMINEMAIL"); break;

    case RPG_Net_Protocol_IRC_Codes::RPL_TRACELOG:
      result = ACE_TEXT("RPL_TRACELOG"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_TRACEEND:
      result = ACE_TEXT("RPL_TRACEEND"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_TRYAGAIN:
      result = ACE_TEXT("RPL_TRYAGAIN"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_LOCALUSERS:
      result = ACE_TEXT("RPL_LOCALUSERS"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_GLOBALUSERS:
      result = ACE_TEXT("RPL_GLOBALUSERS"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_START_NETSTAT:
      result = ACE_TEXT("RPL_START_NETSTAT"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_NETSTAT:
      result = ACE_TEXT("RPL_NETSTAT"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_END_NETSTAT:
      result = ACE_TEXT("RPL_END_NETSTAT"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_PRIVS:
      result = ACE_TEXT("RPL_PRIVS"); break;

    case RPG_Net_Protocol_IRC_Codes::RPL_SILELIST:
      result = ACE_TEXT("RPL_SILELIST"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_ENDOFSILELIST:
      result = ACE_TEXT("RPL_ENDOFSILELIST"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_NOTIFY:
      result = ACE_TEXT("RPL_NOTIFY"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_STATSDELTA:
      result = ACE_TEXT("RPL_STATSDELTA"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_VCHANEXIST:
      result = ACE_TEXT("RPL_VCHANEXIST"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_VCHANLIST:
      result = ACE_TEXT("RPL_VCHANLIST"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_VCHANHELP:
      result = ACE_TEXT("RPL_VCHANHELP"); break;

    case RPG_Net_Protocol_IRC_Codes::RPL_GLIST:
      result = ACE_TEXT("RPL_GLIST"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_ENDOFGLIST:
      result = ACE_TEXT("RPL_ENDOFGLIST"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_ENDOFACCEPT:
      result = ACE_TEXT("RPL_ENDOFACCEPT"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_ALIST:
      result = ACE_TEXT("RPL_ALIST"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_ENDOFALIST:
      result = ACE_TEXT("RPL_ENDOFALIST"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_GLIST_HASH:
      result = ACE_TEXT("RPL_GLIST_HASH"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_CHANINFO_USERS:
      result = ACE_TEXT("RPL_CHANINFO_USERS"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_CHANINFO_CHOPS:
      result = ACE_TEXT("RPL_CHANINFO_CHOPS"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_CHANINFO_VOICES:
      result = ACE_TEXT("RPL_CHANINFO_VOICES"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_CHANINFO_AWAY:
      result = ACE_TEXT("RPL_CHANINFO_AWAY"); break;

    case RPG_Net_Protocol_IRC_Codes::RPL_HELPHDR:
      result = ACE_TEXT("RPL_HELPHDR"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_HELPOP:
      result = ACE_TEXT("RPL_HELPOP"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_HELPTLR:
      result = ACE_TEXT("RPL_HELPTLR"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_HELPHLP:
      result = ACE_TEXT("RPL_HELPHLP"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_HELPFWD:
      result = ACE_TEXT("RPL_HELPFWD"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_HELPIGN:
      result = ACE_TEXT("RPL_HELPIGN"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_CHANINFO_KICKS:
      result = ACE_TEXT("RPL_CHANINFO_KICKS"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_END_CHANINFO:
      result = ACE_TEXT("RPL_END_CHANINFO"); break;

    case RPG_Net_Protocol_IRC_Codes::RPL_NONE:
      result = ACE_TEXT("RPL_NONE"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_AWAY:
      result = ACE_TEXT("RPL_AWAY"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_USERHOST:
      result = ACE_TEXT("RPL_USERHOST"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_ISON:
      result = ACE_TEXT("RPL_ISON"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_TEXT:
      result = ACE_TEXT("RPL_TEXT"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_UNAWAY:
      result = ACE_TEXT("RPL_UNAWAY"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_NOWAWAY:
      result = ACE_TEXT("RPL_NOWAWAY"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_USERIP:
      result = ACE_TEXT("RPL_USERIP"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_WHOISADMIN:
      result = ACE_TEXT("RPL_WHOISADMIN"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_WHOISSADMIN:
      result = ACE_TEXT("RPL_WHOISSADMIN"); break;

    case RPG_Net_Protocol_IRC_Codes::RPL_WHOISHELPOP:
      result = ACE_TEXT("RPL_WHOISHELPOP"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_WHOISUSER:
      result = ACE_TEXT("RPL_WHOISUSER"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_WHOISSERVER:
      result = ACE_TEXT("RPL_WHOISSERVER"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_WHOISOPERATOR:
      result = ACE_TEXT("RPL_WHOISOPERATOR"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_WHOWASUSER:
      result = ACE_TEXT("RPL_WHOWASUSER"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_ENDOFWHO:
      result = ACE_TEXT("RPL_ENDOFWHO"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_WHOISCHANOP:
      result = ACE_TEXT("RPL_WHOISCHANOP"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_WHOISIDLE:
      result = ACE_TEXT("RPL_WHOISIDLE"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_ENDOFWHOIS:
      result = ACE_TEXT("RPL_ENDOFWHOIS"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_WHOISCHANNELS:
      result = ACE_TEXT("RPL_WHOISCHANNELS"); break;

    case RPG_Net_Protocol_IRC_Codes::RPL_WHOISVIRT:
      result = ACE_TEXT("RPL_WHOISVIRT"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_LISTSTART:
      result = ACE_TEXT("RPL_LISTSTART"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_LIST:
      result = ACE_TEXT("RPL_LIST"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_LISTEND:
      result = ACE_TEXT("RPL_LISTEND"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_CHANNELMODEIS:
      result = ACE_TEXT("RPL_CHANNELMODEIS"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_UNIQOPIS:
      result = ACE_TEXT("RPL_UNIQOPIS"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_NOCHANPASS:
      result = ACE_TEXT("RPL_NOCHANPASS"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_CHPASSUNKNOWN:
      result = ACE_TEXT("RPL_CHPASSUNKNOWN"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_CHANNEL_URL:
      result = ACE_TEXT("RPL_CHANNEL_URL"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_CREATIONTIME:
      result = ACE_TEXT("RPL_CREATIONTIME"); break;

    case RPG_Net_Protocol_IRC_Codes::RPL_WHOWAS_TIME:
      result = ACE_TEXT("RPL_WHOWAS_TIME"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_NOTOPIC:
      result = ACE_TEXT("RPL_NOTOPIC"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_TOPIC:
      result = ACE_TEXT("RPL_TOPIC"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_TOPICWHOTIME:
      result = ACE_TEXT("RPL_TOPICWHOTIME"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_LISTUSAGE:
      result = ACE_TEXT("RPL_LISTUSAGE"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_WHOISBOT:
      result = ACE_TEXT("RPL_WHOISBOT"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_CHANPASSOK:
      result = ACE_TEXT("RPL_CHANPASSOK"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_BADCHANPASS:
      result = ACE_TEXT("RPL_BADCHANPASS"); break;

    case RPG_Net_Protocol_IRC_Codes::RPL_INVITING:
      result = ACE_TEXT("RPL_INVITING"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_SUMMONING:
      result = ACE_TEXT("RPL_SUMMONING"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_INVITED:
      result = ACE_TEXT("RPL_INVITED"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_INVITELIST:
      result = ACE_TEXT("RPL_INVITELIST"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_ENDOFINVITELIST:
      result = ACE_TEXT("RPL_ENDOFINVITELIST"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_EXCEPTLIST:
      result = ACE_TEXT("RPL_EXCEPTLIST"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_ENDOFEXCEPTLIST:
      result = ACE_TEXT("RPL_ENDOFEXCEPTLIST"); break;

    case RPG_Net_Protocol_IRC_Codes::RPL_VERSION:
      result = ACE_TEXT("RPL_VERSION"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_WHOREPLY:
      result = ACE_TEXT("RPL_WHOREPLY"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_NAMREPLY:
      result = ACE_TEXT("RPL_NAMREPLY"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_WHOSPCRPL:
      result = ACE_TEXT("RPL_WHOSPCRPL"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_NAMREPLY_:
      result = ACE_TEXT("RPL_NAMREPLY_"); break;

    case RPG_Net_Protocol_IRC_Codes::RPL_KILLDONE:
      result = ACE_TEXT("RPL_KILLDONE"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_CLOSING:
      result = ACE_TEXT("RPL_CLOSING"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_CLOSEEND:
      result = ACE_TEXT("RPL_CLOSEEND"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_LINKS:
      result = ACE_TEXT("RPL_LINKS"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_ENDOFLINKS:
      result = ACE_TEXT("RPL_ENDOFLINKS"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_ENDOFNAMES:
      result = ACE_TEXT("RPL_ENDOFNAMES"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_BANLIST:
      result = ACE_TEXT("RPL_BANLIST"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_ENDOFBANLIST:
      result = ACE_TEXT("RPL_ENDOFBANLIST"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_ENDOFWHOWAS:
      result = ACE_TEXT("RPL_ENDOFWHOWAS"); break;

    case RPG_Net_Protocol_IRC_Codes::RPL_INFO:
      result = ACE_TEXT("RPL_INFO"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_MOTD:
      result = ACE_TEXT("RPL_MOTD"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_INFOSTART:
      result = ACE_TEXT("RPL_INFOSTART"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_ENDOFINFO:
      result = ACE_TEXT("RPL_ENDOFINFO"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_MOTDSTART:
      result = ACE_TEXT("RPL_MOTDSTART"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_ENDOFMOTD:
      result = ACE_TEXT("RPL_ENDOFMOTD"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_FORCE_MOTD:
      result = ACE_TEXT("RPL_FORCE_MOTD"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_BANEXPIRED:
      result = ACE_TEXT("RPL_BANEXPIRED"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_KICKLINKED:
      result = ACE_TEXT("RPL_KICKLINKED"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_BANLINKED:
      result = ACE_TEXT("RPL_BANLINKED"); break;

    case RPG_Net_Protocol_IRC_Codes::RPL_YOUREOPER:
      result = ACE_TEXT("RPL_YOUREOPER"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_REHASHING:
      result = ACE_TEXT("RPL_REHASHING"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_YOURESERVICE:
      result = ACE_TEXT("RPL_YOURESERVICE"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_MYPORTIS:
      result = ACE_TEXT("RPL_MYPORTIS"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_NOTOPERANYMORE:
      result = ACE_TEXT("RPL_NOTOPERANYMORE"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_QLIST:
      result = ACE_TEXT("RPL_QLIST"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_ENDOFQLIST:
      result = ACE_TEXT("RPL_ENDOFQLIST"); break;

    case RPG_Net_Protocol_IRC_Codes::RPL_TIME:
      result = ACE_TEXT("RPL_TIME"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_USERSSTART:
      result = ACE_TEXT("RPL_USERSSTART"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_USERS:
      result = ACE_TEXT("RPL_USERS"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_ENDOFUSERS:
      result = ACE_TEXT("RPL_ENDOFUSERS"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_NOUSERS:
      result = ACE_TEXT("RPL_NOUSERS"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_HOSTHIDDEN:
      result = ACE_TEXT("RPL_HOSTHIDDEN"); break;

    case RPG_Net_Protocol_IRC_Codes::ERR_UNKNOWNERROR:
      result = ACE_TEXT("ERR_UNKNOWNERROR"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_NOSUCHNICK:
      result = ACE_TEXT("ERR_NOSUCHNICK"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_NOSUCHSERVER:
      result = ACE_TEXT("ERR_NOSUCHSERVER"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_NOSUCHCHANNEL:
      result = ACE_TEXT("ERR_NOSUCHCHANNEL"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_CANNOTSENDTOCHAN:
      result = ACE_TEXT("ERR_CANNOTSENDTOCHAN"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_TOOMANYCHANNELS:
      result = ACE_TEXT("ERR_TOOMANYCHANNELS"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_WASNOSUCHNICK:
      result = ACE_TEXT("ERR_WASNOSUCHNICK"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_TOOMANYTARGETS:
      result = ACE_TEXT("ERR_TOOMANYTARGETS"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_NOSUCHSERVICE:
      result = ACE_TEXT("ERR_NOSUCHSERVICE"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_NOORIGIN:
      result = ACE_TEXT("ERR_NOORIGIN"); break;

    case RPG_Net_Protocol_IRC_Codes::ERR_NORECIPIENT:
      result = ACE_TEXT("ERR_NORECIPIENT"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_NOTEXTTOSEND:
      result = ACE_TEXT("ERR_NOTEXTTOSEND"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_NOTOPLEVEL:
      result = ACE_TEXT("ERR_NOTOPLEVEL"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_WILDTOPLEVEL:
      result = ACE_TEXT("ERR_WILDTOPLEVEL"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_BADMASK:
      result = ACE_TEXT("ERR_BADMASK"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_TOOMANYMATCHES:
      result = ACE_TEXT("ERR_TOOMANYMATCHES"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_LENGTHTRUNCATED:
      result = ACE_TEXT("ERR_LENGTHTRUNCATED"); break;

    case RPG_Net_Protocol_IRC_Codes::ERR_UNKNOWNCOMMAND:
      result = ACE_TEXT("ERR_UNKNOWNCOMMAND"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_NOMOTD:
      result = ACE_TEXT("ERR_NOMOTD"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_NOADMININFO:
      result = ACE_TEXT("ERR_NOADMININFO"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_FILEERROR:
      result = ACE_TEXT("ERR_FILEERROR"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_NOOPERMOTD:
      result = ACE_TEXT("ERR_NOOPERMOTD"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_TOOMANYAWAY:
      result = ACE_TEXT("ERR_TOOMANYAWAY"); break;

    case RPG_Net_Protocol_IRC_Codes::ERR_EVENTNICKCHANGE:
      result = ACE_TEXT("ERR_EVENTNICKCHANGE"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_NONICKNAMEGIVEN:
      result = ACE_TEXT("ERR_NONICKNAMEGIVEN"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_ERRONEUSNICKNAME:
      result = ACE_TEXT("ERR_ERRONEUSNICKNAME"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_NICKNAMEINUSE:
      result = ACE_TEXT("ERR_NICKNAMEINUSE"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_SERVICENAMEINUSE:
      result = ACE_TEXT("ERR_SERVICENAMEINUSE"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_SERVICECONFUSED:
      result = ACE_TEXT("ERR_SERVICECONFUSED"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_NICKCOLLISION:
      result = ACE_TEXT("ERR_NICKCOLLISION"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_UNAVAILRESOURCE:
      result = ACE_TEXT("ERR_UNAVAILRESOURCE"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_NICKTOOFAST:
      result = ACE_TEXT("ERR_NICKTOOFAST"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_TARGETTOOFAST:
      result = ACE_TEXT("ERR_TARGETTOOFAST"); break;

    case RPG_Net_Protocol_IRC_Codes::ERR_SERVICESDOWN:
      result = ACE_TEXT("ERR_SERVICESDOWN"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_USERNOTINCHANNEL:
      result = ACE_TEXT("ERR_USERNOTINCHANNEL"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_NOTONCHANNEL:
      result = ACE_TEXT("ERR_NOTONCHANNEL"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_USERONCHANNEL:
      result = ACE_TEXT("ERR_USERONCHANNEL"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_NOLOGIN:
      result = ACE_TEXT("ERR_NOLOGIN"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_SUMMONDISABLED:
      result = ACE_TEXT("ERR_SUMMONDISABLED"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_USERSDISABLED:
      result = ACE_TEXT("ERR_USERSDISABLED"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_NONICKCHANGE:
      result = ACE_TEXT("ERR_NONICKCHANGE"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_NOTIMPLEMENTED:
      result = ACE_TEXT("ERR_NOTIMPLEMENTED"); break;

    case RPG_Net_Protocol_IRC_Codes::ERR_NOTREGISTERED:
      result = ACE_TEXT("ERR_NOTREGISTERED"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_IDCOLLISION:
      result = ACE_TEXT("ERR_IDCOLLISION"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_NICKLOST:
      result = ACE_TEXT("ERR_NICKLOST"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_HOSTILENAME:
      result = ACE_TEXT("ERR_HOSTILENAME"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_ACCEPTFULL:
      result = ACE_TEXT("ERR_ACCEPTFULL"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_ACCEPTEXIST:
      result = ACE_TEXT("ERR_ACCEPTEXIST"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_ACCEPTNOT:
      result = ACE_TEXT("ERR_ACCEPTNOT"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_NOHIDING:
      result = ACE_TEXT("ERR_NOHIDING"); break;

    case RPG_Net_Protocol_IRC_Codes::ERR_NOTFORHALFOPS:
      result = ACE_TEXT("ERR_NOTFORHALFOPS"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_NEEDMOREPARAMS:
      result = ACE_TEXT("ERR_NEEDMOREPARAMS"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_ALREADYREGISTRED:
      result = ACE_TEXT("ERR_ALREADYREGISTRED"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_NOPERMFORHOST:
      result = ACE_TEXT("ERR_NOPERMFORHOST"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_PASSWDMISMATCH:
      result = ACE_TEXT("ERR_PASSWDMISMATCH"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_YOUREBANNEDCREEP:
      result = ACE_TEXT("ERR_YOUREBANNEDCREEP"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_YOUWILLBEBANNED:
      result = ACE_TEXT("ERR_YOUWILLBEBANNED"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_KEYSET:
      result = ACE_TEXT("ERR_KEYSET"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_ONLYSERVERSCANCHANGE:
      result = ACE_TEXT("ERR_ONLYSERVERSCANCHANGE"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_LINKSET:
      result = ACE_TEXT("ERR_LINKSET"); break;

    case RPG_Net_Protocol_IRC_Codes::ERR_LINKCHANNEL:
      result = ACE_TEXT("ERR_LINKCHANNEL"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_CHANNELISFULL:
      result = ACE_TEXT("ERR_CHANNELISFULL"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_UNKNOWNMODE:
      result = ACE_TEXT("ERR_UNKNOWNMODE"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_INVITEONLYCHAN:
      result = ACE_TEXT("ERR_INVITEONLYCHAN"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_BANNEDFROMCHAN:
      result = ACE_TEXT("ERR_BANNEDFROMCHAN"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_BADCHANNELKEY:
      result = ACE_TEXT("ERR_BADCHANNELKEY"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_BADCHANMASK:
      result = ACE_TEXT("ERR_BADCHANMASK"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_NOCHANMODES:
      result = ACE_TEXT("ERR_NOCHANMODES"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_BANLISTFULL:
      result = ACE_TEXT("ERR_BANLISTFULL"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_BADCHANNAME:
      result = ACE_TEXT("ERR_BADCHANNAME"); break;

    case RPG_Net_Protocol_IRC_Codes::ERR_CANNOTKNOCK:
      result = ACE_TEXT("ERR_CANNOTKNOCK"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_NOPRIVILEGES:
      result = ACE_TEXT("ERR_NOPRIVILEGES"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_CHANOPRIVSNEEDED:
      result = ACE_TEXT("ERR_CHANOPRIVSNEEDED"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_CANTKILLSERVER:
      result = ACE_TEXT("ERR_CANTKILLSERVER"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_RESTRICTED:
      result = ACE_TEXT("ERR_RESTRICTED"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_UNIQOPPRIVSNEEDED:
      result = ACE_TEXT("ERR_UNIQOPPRIVSNEEDED"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_NONONREG:
      result = ACE_TEXT("ERR_NONONREG"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_CHANTOORECENT:
      result = ACE_TEXT("ERR_CHANTOORECENT"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_TSLESSCHAN:
      result = ACE_TEXT("ERR_TSLESSCHAN"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_VOICENEEDED:
      result = ACE_TEXT("ERR_VOICENEEDED"); break;

    case RPG_Net_Protocol_IRC_Codes::ERR_NOOPERHOST:
      result = ACE_TEXT("ERR_NOOPERHOST"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_NOSERVICEHOST:
      result = ACE_TEXT("ERR_NOSERVICEHOST"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_NOFEATURE:
      result = ACE_TEXT("ERR_NOFEATURE"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_BADFEATURE:
      result = ACE_TEXT("ERR_BADFEATURE"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_BADLOGTYPE:
      result = ACE_TEXT("ERR_BADLOGTYPE"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_BADLOGSYS:
      result = ACE_TEXT("ERR_BADLOGSYS"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_BADLOGVALUE:
      result = ACE_TEXT("ERR_BADLOGVALUE"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_ISOPERLCHAN:
      result = ACE_TEXT("ERR_ISOPERLCHAN"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_CHANOWNPRIVNEEDED:
      result = ACE_TEXT("ERR_CHANOWNPRIVNEEDED"); break;

    case RPG_Net_Protocol_IRC_Codes::ERR_UMODEUNKNOWNFLAG:
      result = ACE_TEXT("ERR_UMODEUNKNOWNFLAG"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_USERSDONTMATCH:
      result = ACE_TEXT("ERR_USERSDONTMATCH"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_GHOSTEDCLIENT:
      result = ACE_TEXT("ERR_GHOSTEDCLIENT"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_USERNOTONSERV:
      result = ACE_TEXT("ERR_USERNOTONSERV"); break;

    case RPG_Net_Protocol_IRC_Codes::ERR_SILELISTFULL:
      result = ACE_TEXT("ERR_SILELISTFULL"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_NOSUCHGLINE:
      result = ACE_TEXT("ERR_NOSUCHGLINE"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_BADPING:
      result = ACE_TEXT("ERR_BADPING"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_INVALID_ERR:
      result = ACE_TEXT("ERR_INVALID_ERR"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_BADEXPIRE:
      result = ACE_TEXT("ERR_BADEXPIRE"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_DONTCHEAT:
      result = ACE_TEXT("ERR_DONTCHEAT"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_DISABLED:
      result = ACE_TEXT("ERR_DISABLED"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_NOINVITE:
      result = ACE_TEXT("ERR_NOINVITE"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_ADMONLY:
      result = ACE_TEXT("ERR_ADMONLY"); break;

    case RPG_Net_Protocol_IRC_Codes::ERR_OPERONLY:
      result = ACE_TEXT("ERR_OPERONLY"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_LISTSYNTAX:
      result = ACE_TEXT("ERR_LISTSYNTAX"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_WHOSYNTAX:
      result = ACE_TEXT("ERR_WHOSYNTAX"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_WHOLIMEXCEED:
      result = ACE_TEXT("ERR_WHOLIMEXCEED"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_OPERSPVERIFY:
      result = ACE_TEXT("ERR_OPERSPVERIFY"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_REMOTEPFX:
      result = ACE_TEXT("ERR_REMOTEPFX"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_PFXUNROUTABLE:
      result = ACE_TEXT("ERR_PFXUNROUTABLE"); break;

    case RPG_Net_Protocol_IRC_Codes::ERR_BADHOSTMASK:
      result = ACE_TEXT("ERR_BADHOSTMASK"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_HOSTUNAVAIL:
      result = ACE_TEXT("ERR_HOSTUNAVAIL"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_USINGSLINE:
      result = ACE_TEXT("ERR_USINGSLINE"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_STATSSLINE:
      result = ACE_TEXT("ERR_STATSSLINE"); break;

    case RPG_Net_Protocol_IRC_Codes::RPL_LOGON:
      result = ACE_TEXT("RPL_LOGON"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_LOGOFF:
      result = ACE_TEXT("RPL_LOGOFF"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_WATCHOFF:
      result = ACE_TEXT("RPL_WATCHOFF"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_WATCHSTAT:
      result = ACE_TEXT("RPL_WATCHSTAT"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_NOWON:
      result = ACE_TEXT("RPL_NOWON"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_NOWOFF:
      result = ACE_TEXT("RPL_NOWOFF"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_WATCHLIST:
      result = ACE_TEXT("RPL_WATCHLIST"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_ENDOFWATCHLIST:
      result = ACE_TEXT("RPL_ENDOFWATCHLIST"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_WATCHCLEAR:
      result = ACE_TEXT("RPL_WATCHCLEAR"); break;

    case RPG_Net_Protocol_IRC_Codes::RPL_ISOPER:
      result = ACE_TEXT("RPL_ISOPER"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_ISLOCOP:
      result = ACE_TEXT("RPL_ISLOCOP"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_ISNOTOPER:
      result = ACE_TEXT("RPL_ISNOTOPER"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_ENDOFISOPER:
      result = ACE_TEXT("RPL_ENDOFISOPER"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_WHOISMODES:
      result = ACE_TEXT("RPL_WHOISMODES"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_WHOISHOST:
      result = ACE_TEXT("RPL_WHOISHOST"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_DCCSTATUS:
      result = ACE_TEXT("RPL_DCCSTATUS"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_DCCLIST:
      result = ACE_TEXT("RPL_DCCLIST"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_WHOWASHOST:
      result = ACE_TEXT("RPL_WHOWASHOST"); break;

    case RPG_Net_Protocol_IRC_Codes::RPL_RULESSTART:
      result = ACE_TEXT("RPL_RULESSTART"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_RULES:
      result = ACE_TEXT("RPL_RULES"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_ENDOFRULES:
      result = ACE_TEXT("RPL_ENDOFRULES"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_OMOTDSTART:
      result = ACE_TEXT("RPL_OMOTDSTART"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_OMOTD:
      result = ACE_TEXT("RPL_OMOTD"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_ENDOFO:
      result = ACE_TEXT("RPL_ENDOFO"); break;

    case RPG_Net_Protocol_IRC_Codes::RPL_SETTINGS:
      result = ACE_TEXT("RPL_SETTINGS"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_ENDOFSETTINGS:
      result = ACE_TEXT("RPL_ENDOFSETTINGS"); break;

    case RPG_Net_Protocol_IRC_Codes::RPL_DUMPING:
      result = ACE_TEXT("RPL_DUMPING"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_DUMPRPL:
      result = ACE_TEXT("RPL_DUMPRPL"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_EODUMP:
      result = ACE_TEXT("RPL_EODUMP"); break;

    case RPG_Net_Protocol_IRC_Codes::RPL_TRACEROUTE_HOP:
      result = ACE_TEXT("RPL_TRACEROUTE_HOP"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_TRACEROUTE_START:
      result = ACE_TEXT("RPL_TRACEROUTE_START"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_MODECHANGEWARN:
      result = ACE_TEXT("RPL_MODECHANGEWARN"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_CHANREDIR:
      result = ACE_TEXT("RPL_CHANREDIR"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_SERVMODEIS:
      result = ACE_TEXT("RPL_SERVMODEIS"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_OTHERUMODEIS:
      result = ACE_TEXT("RPL_OTHERUMODEIS"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_ENDOF_GENERIC:
      result = ACE_TEXT("RPL_ENDOF_GENERIC"); break;

    case RPG_Net_Protocol_IRC_Codes::RPL_WHOWASDETAILS:
      result = ACE_TEXT("RPL_WHOWASDETAILS"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_WHOISSECURE:
      result = ACE_TEXT("RPL_WHOISSECURE"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_UNKNOWNMODES:
      result = ACE_TEXT("RPL_UNKNOWNMODES"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_CANNOTSETMODES:
      result = ACE_TEXT("RPL_CANNOTSETMODES"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_LUSERSTAFF:
      result = ACE_TEXT("RPL_LUSERSTAFF"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_TIMEONSERVERIS:
      result = ACE_TEXT("RPL_TIMEONSERVERIS"); break;

    case RPG_Net_Protocol_IRC_Codes::RPL_NETWORKS:
      result = ACE_TEXT("RPL_NETWORKS"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_YOURLANGUAGEIS:
      result = ACE_TEXT("RPL_YOURLANGUAGEIS"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_LANGUAGE:
      result = ACE_TEXT("RPL_LANGUAGE"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_WHOISSTAFF:
      result = ACE_TEXT("RPL_WHOISSTAFF"); break;

    case RPG_Net_Protocol_IRC_Codes::RPL_WHOISLANGUAGE:
      result = ACE_TEXT("RPL_WHOISLANGUAGE"); break;

    case RPG_Net_Protocol_IRC_Codes::RPL_MODLIST:
      result = ACE_TEXT("RPL_MODLIST"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_ENDOFMODLIST:
      result = ACE_TEXT("RPL_ENDOFMODLIST"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_HELPSTART:
      result = ACE_TEXT("RPL_HELPSTART"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_HELPTXT:
      result = ACE_TEXT("RPL_HELPTXT"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_ENDOFHELP:
      result = ACE_TEXT("RPL_ENDOFHELP"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_ETRACEFULL:
      result = ACE_TEXT("RPL_ETRACEFULL"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_ETRACE:
      result = ACE_TEXT("RPL_ETRACE"); break;

    case RPG_Net_Protocol_IRC_Codes::RPL_KNOCK:
      result = ACE_TEXT("RPL_KNOCK"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_KNOCKDLVR:
      result = ACE_TEXT("RPL_KNOCKDLVR"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_TOOMANYKNOCK:
      result = ACE_TEXT("RPL_TOOMANYKNOCK"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_CHANOPEN:
      result = ACE_TEXT("RPL_CHANOPEN"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_KNOCKONCHAN:
      result = ACE_TEXT("RPL_KNOCKONCHAN"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_KNOCKDISABLED:
      result = ACE_TEXT("RPL_KNOCKDISABLED"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_TARGUMODEG:
      result = ACE_TEXT("RPL_TARGUMODEG"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_TARGNOTIFY:
      result = ACE_TEXT("RPL_TARGNOTIFY"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_UMODEGMSG:
      result = ACE_TEXT("RPL_UMODEGMSG"); break;

    case RPG_Net_Protocol_IRC_Codes::RPL_ENDOFOMOTD:
      result = ACE_TEXT("RPL_ENDOFOMOTD"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_NOPRIVS:
      result = ACE_TEXT("RPL_NOPRIVS"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_TESTMARK:
      result = ACE_TEXT("RPL_TESTMARK"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_TESTLINE:
      result = ACE_TEXT("RPL_TESTLINE"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_NOTESTLINE:
      result = ACE_TEXT("RPL_NOTESTLINE"); break;

    case RPG_Net_Protocol_IRC_Codes::RPL_XINFO:
      result = ACE_TEXT("RPL_XINFO"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_XINFOSTART:
      result = ACE_TEXT("RPL_XINFOSTART"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_XINFOEND:
      result = ACE_TEXT("RPL_XINFOEND"); break;

    case RPG_Net_Protocol_IRC_Codes::ERR_CANNOTDOCOMMAND:
      result = ACE_TEXT("ERR_CANNOTDOCOMMAND"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_CANNOTCHANGEUMODE:
      result = ACE_TEXT("ERR_CANNOTCHANGEUMODE"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_CANNOTCHANGECHANMODE:
      result = ACE_TEXT("ERR_CANNOTCHANGECHANMODE"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_CANNOTCHANGESERVERMODE:
      result = ACE_TEXT("ERR_CANNOTCHANGESERVERMODE"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_CANNOTSENDTONICK:
      result = ACE_TEXT("ERR_CANNOTSENDTONICK"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_UNKNOWNSERVERMODE:
      result = ACE_TEXT("ERR_UNKNOWNSERVERMODE"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_SERVERMODELOCK:
      result = ACE_TEXT("ERR_SERVERMODELOCK"); break;

    case RPG_Net_Protocol_IRC_Codes::ERR_BADCHARENCODING:
      result = ACE_TEXT("ERR_BADCHARENCODING"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_TOOMANYLANGUAGES:
      result = ACE_TEXT("ERR_TOOMANYLANGUAGES"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_NOLANGUAGE:
      result = ACE_TEXT("ERR_NOLANGUAGE"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_TEXTTOOSHORT:
      result = ACE_TEXT("ERR_TEXTTOOSHORT"); break;

    case RPG_Net_Protocol_IRC_Codes::ERR_NUMERIC_ERR:
      result = ACE_TEXT("ERR_NUMERIC_ERR"); break;

    default:
    {
      // *NOTE*: according to the RFC2812, codes between 001-099 are reserved
      // for client-server connections...
      // --> maybe in use by some extension
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("invalid/unknown numeric (was: %d), aborting\n"),
                 numeric_in));

      break;
    }
  } // end SWITCH

  return result;
}

const RPG_Net_Protocol_ChannelMode
RPG_Net_Protocol_Tools::IRCChannelModeChar2ChannelMode(const char& mode_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_Tools::IRCChannelModeChar2ChannelMode"));

  // init result
  RPG_Net_Protocol_ChannelMode result = CHANNELMODE_INVALID;

  switch (mode_in)
  {
    case 'k':
      result = CHANNELMODE_PASSWORD; break;
    case 'v':
      result = CHANNELMODE_VOICE; break;
    case 'b':
      result = CHANNELMODE_BAN; break;
    case 'l':
      result = CHANNELMODE_USERLIMIT; break;
    case 'm':
      result = CHANNELMODE_MODERATED; break;
    case 'n':
      result = CHANNELMODE_BLOCKFOREIGNMSGS; break;
    case 't':
      result = CHANNELMODE_RESTRICTEDTOPIC; break;
    case 'i':
      result = CHANNELMODE_INVITEONLY; break;
    case 's':
      result = CHANNELMODE_SECRET; break;
    case 'p':
      result = CHANNELMODE_PRIVATE; break;
    case 'o':
      result = CHANNELMODE_OPERATOR; break;
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("unknown/invalid channel mode (was: \"%c\"), aborting\n"),
                 mode_in));

      break;
    }
  } // end SWITCH

  return result;
}

const RPG_Net_Protocol_UserMode
RPG_Net_Protocol_Tools::IRCUserModeChar2UserMode(const char& mode_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_Tools::IRCUserModeChar2UserMode"));

  // init result
  RPG_Net_Protocol_UserMode result = USERMODE_INVALID;

  switch (mode_in)
  {
    case 'O':
      result = USERMODE_LOCALOPERATOR; break;
    case 'o':
      result = USERMODE_OPERATOR; break;
    case 'r':
      result = USERMODE_RESTRICTEDCONN; break;
    case 'w':
      result = USERMODE_RECVWALLOPS; break;
    case 's':
      result = USERMODE_RECVNOTICES; break;
    case 'i':
      result = USERMODE_INVISIBLE; break;
    case 'a':
      result = USERMODE_AWAY; break;
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("unknown/invalid user mode (was: \"%c\"), aborting\n"),
                 mode_in));

      break;
    }
  } // end SWITCH

  return result;
}

const std::string
RPG_Net_Protocol_Tools::IRCChannelMode2String(const RPG_Net_Protocol_ChannelMode& mode_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_Tools::IRCChannelMode2String"));

  // init result
  std::string result;

  switch (mode_in)
  {
    case CHANNELMODE_PASSWORD:
      result = ACE_TEXT_ALWAYS_CHAR("CHANNELMODE_PASSWORD"); break;
    case CHANNELMODE_VOICE:
      result = ACE_TEXT_ALWAYS_CHAR("CHANNELMODE_VOICE"); break;
    case CHANNELMODE_BAN:
      result = ACE_TEXT_ALWAYS_CHAR("CHANNELMODE_BAN"); break;
    case CHANNELMODE_USERLIMIT:
      result = ACE_TEXT_ALWAYS_CHAR("CHANNELMODE_USERLIMIT"); break;
    case CHANNELMODE_MODERATED:
      result = ACE_TEXT_ALWAYS_CHAR("CHANNELMODE_MODERATED"); break;
    case CHANNELMODE_BLOCKFOREIGNMSGS:
      result = ACE_TEXT_ALWAYS_CHAR("CHANNELMODE_BLOCKFOREIGNMSGS"); break;
    case CHANNELMODE_RESTRICTEDTOPIC:
      result = ACE_TEXT_ALWAYS_CHAR("CHANNELMODE_RESTRICTEDTOPIC"); break;
    case CHANNELMODE_INVITEONLY:
      result = ACE_TEXT_ALWAYS_CHAR("CHANNELMODE_INVITEONLY"); break;
    case CHANNELMODE_SECRET:
      result = ACE_TEXT_ALWAYS_CHAR("CHANNELMODE_SECRET"); break;
    case CHANNELMODE_PRIVATE:
      result = ACE_TEXT_ALWAYS_CHAR("CHANNELMODE_PRIVATE"); break;
    case CHANNELMODE_OPERATOR:
      result = ACE_TEXT_ALWAYS_CHAR("CHANNELMODE_OPERATOR"); break;
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid channel mode (was: %d), aborting\n"),
                 mode_in));

      break;
    }
  } // end SWITCH

  return result;
}

const std::string
RPG_Net_Protocol_Tools::IRCUserMode2String(const RPG_Net_Protocol_UserMode& mode_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_Tools::IRCUserMode2String"));

  // init result
  std::string result;

  switch (mode_in)
  {
    case USERMODE_LOCALOPERATOR:
      result = ACE_TEXT_ALWAYS_CHAR("USERMODE_LOCALOPERATOR"); break;
    case USERMODE_OPERATOR:
      result = ACE_TEXT_ALWAYS_CHAR("USERMODE_OPERATOR"); break;
    case USERMODE_RESTRICTEDCONN:
      result = ACE_TEXT_ALWAYS_CHAR("USERMODE_RESTRICTEDCONN"); break;
    case USERMODE_RECVWALLOPS:
      result = ACE_TEXT_ALWAYS_CHAR("USERMODE_RECVWALLOPS"); break;
    case USERMODE_RECVNOTICES:
      result = ACE_TEXT_ALWAYS_CHAR("USERMODE_RECVNOTICES"); break;
    case USERMODE_INVISIBLE:
      result = ACE_TEXT_ALWAYS_CHAR("USERMODE_INVISIBLE"); break;
    case USERMODE_AWAY:
      result = ACE_TEXT_ALWAYS_CHAR("USERMODE_AWAY"); break;
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid user mode (was: %d), aborting\n"),
                 mode_in));

      break;
    }
  } // end SWITCH

  return result;
}

const std::string
RPG_Net_Protocol_Tools::IRCMessage2String(const RPG_Net_Protocol_IRCMessage& message_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_Tools::IRCMessage2String"));

  // init result
  std::string result;

  switch (message_in.command.discriminator)
  {
    case RPG_Net_Protocol_IRCMessage::Command::STRING:
    {
      RPG_Net_Protocol_IRCMessage::CommandType command = RPG_Net_Protocol_Tools::IRCCommandString2Type(*message_in.command.string);
      switch (command)
      {
        case RPG_Net_Protocol_IRCMessage::NICK:
        {
          result = message_in.prefix.origin;
          result += ACE_TEXT_ALWAYS_CHAR(" --> ");
          result += message_in.params.back();

          break;
        }
        case RPG_Net_Protocol_IRCMessage::QUIT:
        {
          result = message_in.prefix.origin;
          result += ACE_TEXT_ALWAYS_CHAR(" has QUIT IRC (reason: \"%s\")");
          result += message_in.params.back();

          break;
        }
        case RPG_Net_Protocol_IRCMessage::NOTICE:
        {
          result = RPG_Net_Protocol_Tools::concatParams(message_in.params,
                                                        1);

          break;
        }
        case RPG_Net_Protocol_IRCMessage::MODE:
        case RPG_Net_Protocol_IRCMessage::TOPIC:
        case RPG_Net_Protocol_IRCMessage::INVITE:
        case RPG_Net_Protocol_IRCMessage::KICK:
        case RPG_Net_Protocol_IRCMessage::ERROR:
        case RPG_Net_Protocol_IRCMessage::AWAY:
        case RPG_Net_Protocol_IRCMessage::USERS:
        case RPG_Net_Protocol_IRCMessage::USERHOST:
        {
          result = RPG_Net_Protocol_Tools::concatParams(message_in.params);

          break;
        }
        default:
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("invalid command (was: \"%s\"), aborting\n"),
                     message_in.command.string->c_str()));

          message_in.dump_state();

          return result;
        }
      } // end SWITCH

      break;
    }
    case RPG_Net_Protocol_IRCMessage::Command::NUMERIC:
    {
      switch (message_in.command.numeric)
      {
        case RPG_Net_Protocol_IRC_Codes::RPL_WELCOME:       //   1
        case RPG_Net_Protocol_IRC_Codes::RPL_YOURHOST:      //   2
        case RPG_Net_Protocol_IRC_Codes::RPL_CREATED:       //   3
        case RPG_Net_Protocol_IRC_Codes::RPL_LUSERCLIENT:   // 251
        case RPG_Net_Protocol_IRC_Codes::RPL_LUSERME:       // 255
        case RPG_Net_Protocol_IRC_Codes::RPL_TRYAGAIN:      // 263
        case RPG_Net_Protocol_IRC_Codes::RPL_LOCALUSERS:    // 265
        case RPG_Net_Protocol_IRC_Codes::RPL_GLOBALUSERS:   // 266
        case RPG_Net_Protocol_IRC_Codes::RPL_UNAWAY:        // 305
        case RPG_Net_Protocol_IRC_Codes::RPL_NOWAWAY:       // 306
        case RPG_Net_Protocol_IRC_Codes::RPL_ENDOFBANLIST:  // 368
        case RPG_Net_Protocol_IRC_Codes::RPL_MOTD:          // 372
        case RPG_Net_Protocol_IRC_Codes::RPL_MOTDSTART:     // 375
        case RPG_Net_Protocol_IRC_Codes::RPL_ENDOFMOTD:     // 376
        case RPG_Net_Protocol_IRC_Codes::ERR_YOUREBANNEDCREEP: // 465
        case RPG_Net_Protocol_IRC_Codes::ERR_CHANOPRIVSNEEDED: // 482
        {
          result = RPG_Net_Protocol_Tools::concatParams(message_in.params,
                                                        -1);

          break;
        }
        case RPG_Net_Protocol_IRC_Codes::RPL_BANLIST:       // 367
        {
          result = RPG_Net_Protocol_Tools::concatParams(message_in.params);

          break;
        }
        case RPG_Net_Protocol_IRC_Codes::RPL_MYINFO:        //   4
        case RPG_Net_Protocol_IRC_Codes::RPL_PROTOCTL:      //   5
        case RPG_Net_Protocol_IRC_Codes::RPL_YOURID:        //  42
        case RPG_Net_Protocol_IRC_Codes::RPL_STATSDLINE:    // 250
        case RPG_Net_Protocol_IRC_Codes::RPL_LUSEROP:       // 252
        case RPG_Net_Protocol_IRC_Codes::RPL_LUSERUNKNOWN:  // 253
        case RPG_Net_Protocol_IRC_Codes::RPL_LUSERCHANNELS: // 254
        case RPG_Net_Protocol_IRC_Codes::RPL_USERHOST:      // 302
        case RPG_Net_Protocol_IRC_Codes::RPL_LISTSTART:     // 321
        case RPG_Net_Protocol_IRC_Codes::RPL_LIST:          // 322
        case RPG_Net_Protocol_IRC_Codes::RPL_LISTEND:       // 323
        case RPG_Net_Protocol_IRC_Codes::RPL_INVITING:      // 341
        case RPG_Net_Protocol_IRC_Codes::ERR_NOSUCHNICK:    // 401
        case RPG_Net_Protocol_IRC_Codes::ERR_NICKNAMEINUSE: // 433
        case RPG_Net_Protocol_IRC_Codes::ERR_BADCHANNAME:   // 479
        case RPG_Net_Protocol_IRC_Codes::ERR_UMODEUNKNOWNFLAG: // 501
        {
          result = RPG_Net_Protocol_Tools::concatParams(message_in.params,
                                                        1);

          break;
        }
        case RPG_Net_Protocol_IRC_Codes::RPL_ENDOFWHO:      // 315
        {
          result = RPG_Net_Protocol_Tools::concatParams(message_in.params,
                                                        2);

          break;
        }
        case RPG_Net_Protocol_IRC_Codes::RPL_WHOREPLY:      // 352
        {
          result = RPG_Net_Protocol_Tools::concatParams(message_in.params,
                                                        5);

          break;
        }
        default:
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("invalid (numeric) command (was: %u [\"%s\"]), aborting\n"),
                     message_in.command.numeric,
                     RPG_Net_Protocol_Tools::IRCCode2String(message_in.command.numeric).c_str()));

          message_in.dump_state();

          return result;
        }
      } // end SWITCH

      break;
    }
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid command discriminator (was: %d), aborting\n"),
                 message_in.command.discriminator));

      return result;
    }
  } // end SWITCH

  // append newline
  result += ACE_TEXT_ALWAYS_CHAR("\n");

  return result;
}

const std::string
RPG_Net_Protocol_Tools::concatParams(const RPG_Net_Protocol_Parameters_t& params_in,
                                     const int& index_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Protocol_Tools::concatParams"));

  std::string result;

  // sanity check(s)
  if (params_in.empty() ||
      (index_in > static_cast<int> ((params_in.size() - 1))))
    return result;

  if (index_in == -1)
    return params_in.back();

  RPG_Net_Protocol_ParametersIterator_t iterator = params_in.begin();
  std::advance(iterator, index_in);
  for (;
       iterator != params_in.end();
       iterator++)
  {
    result += *iterator;
    result += ACE_TEXT_ALWAYS_CHAR(" ");
  } // end FOR
  if (index_in < static_cast<int> ((params_in.size() - 1)))
    result.erase(--result.end());

  return result;
}
