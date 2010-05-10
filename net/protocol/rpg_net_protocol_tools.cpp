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

#include <sstream>

const std::string
RPG_Net_Protocol_Tools::IRCMessage2String(const RPG_Net_Protocol_IRCMessage& message_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_Tools::IRCMessage2String"));

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
      converter << RPG_Net_Protocol_Tools::IRCCode2String(message_in.command.numeric) << ACE_TEXT(" [") << message_in.command.numeric << ACE_TEXT("]");

      break;
    }
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid command discriminator (was: %d), aborting\n"),
                 message_in.command.discriminator));

      return converter.str();
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

const RPG_Net_Protocol_CommandType_t
RPG_Net_Protocol_Tools::IRCCommandString2Type(const std::string& commandString_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_Tools::IRCCommandString2Type"));

  if ((commandString_in) == ACE_TEXT("PASS"))
    return RPG_Net_Protocol_IRCMessage::PASS;
  else if ((commandString_in) == ACE_TEXT("NICK"))
    return RPG_Net_Protocol_IRCMessage::NICK;
  else if ((commandString_in) == ACE_TEXT("USER"))
    return RPG_Net_Protocol_IRCMessage::USER;
  else if ((commandString_in) == ACE_TEXT("QUIT"))
    return RPG_Net_Protocol_IRCMessage::QUIT;
  else if ((commandString_in) == ACE_TEXT("JOIN"))
    return RPG_Net_Protocol_IRCMessage::JOIN;
  else if ((commandString_in) == ACE_TEXT("MODE"))
    return RPG_Net_Protocol_IRCMessage::MODE;
  else if ((commandString_in) == ACE_TEXT("PRIVMSG"))
    return RPG_Net_Protocol_IRCMessage::PRIVMSG;
  else if ((commandString_in) == ACE_TEXT("NOTICE"))
    return RPG_Net_Protocol_IRCMessage::NOTICE;
  else if (commandString_in == ACE_TEXT("PING"))
    return RPG_Net_Protocol_IRCMessage::PING;
  else if (commandString_in == ACE_TEXT("PONG"))
    return RPG_Net_Protocol_IRCMessage::PONG;
  else if (commandString_in == ACE_TEXT("ERROR"))
    return RPG_Net_Protocol_IRCMessage::ERROR;
  else
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid/unknown command (was: \"%s\"), aborting\n"),
               commandString_in.c_str()));

    return RPG_Net_Protocol_IRCMessage::RPG_NET_PROTOCOL_COMMANDTYPE_INVALID;
  } // end ELSE
}

const std::string
RPG_Net_Protocol_Tools::IRCCode2String(const RPG_Net_Protocol_IRCNumeric_t& numeric_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_Tools::IRCCode2String"));

  // init result
  std::string result = ACE_TEXT("RPG_NET_PROTOCOL_IRC_CODE_INVALID");

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
    case RPG_Net_Protocol_IRC_Codes::RPL_BOUNCE:
      result = ACE_TEXT("RPL_BOUNCE"); break;

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

    case RPG_Net_Protocol_IRC_Codes::RPL_STATSLINKINFO:
      result = ACE_TEXT("RPL_STATSLINKINFO"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_STATSCOMMANDS:
      result = ACE_TEXT("RPL_STATSCOMMANDS"); break;

    case RPG_Net_Protocol_IRC_Codes::RPL_STATSQLINE:
      result = ACE_TEXT("RPL_STATSQLINE"); break;

    case RPG_Net_Protocol_IRC_Codes::RPL_ENDOFSTATS:
      result = ACE_TEXT("RPL_ENDOFSTATS"); break;

    case RPG_Net_Protocol_IRC_Codes::RPL_UMODEIS:
      result = ACE_TEXT("RPL_UMODEIS"); break;

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

    case RPG_Net_Protocol_IRC_Codes::RPL_STATSUPTIME:
      result = ACE_TEXT("RPL_STATSUPTIME"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_STATSOLINE:
      result = ACE_TEXT("RPL_STATSOLINE"); break;

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

    case RPG_Net_Protocol_IRC_Codes::RPL_AWAY:
      result = ACE_TEXT("RPL_AWAY"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_USERHOST:
      result = ACE_TEXT("RPL_USERHOST"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_ISON:
      result = ACE_TEXT("RPL_ISON"); break;

    case RPG_Net_Protocol_IRC_Codes::RPL_UNAWAY:
      result = ACE_TEXT("RPL_UNAWAY"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_NOWAWAY:
      result = ACE_TEXT("RPL_NOWAWAY"); break;

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

    case RPG_Net_Protocol_IRC_Codes::RPL_LIST:
      result = ACE_TEXT("RPL_LIST"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_LISTEND:
      result = ACE_TEXT("RPL_LISTEND"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_CHANNELMODEIS:
      result = ACE_TEXT("RPL_CHANNELMODEIS"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_UNIQOPIS:
      result = ACE_TEXT("RPL_UNIQOPIS"); break;

    case RPG_Net_Protocol_IRC_Codes::RPL_NOTOPIC:
      result = ACE_TEXT("RPL_NOTOPIC"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_TOPIC:
      result = ACE_TEXT("RPL_TOPIC"); break;

    case RPG_Net_Protocol_IRC_Codes::RPL_INVITING:
      result = ACE_TEXT("RPL_INVITING"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_SUMMONING:
      result = ACE_TEXT("RPL_SUMMONING"); break;

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

    case RPG_Net_Protocol_IRC_Codes::RPL_YOUREOPER:
      result = ACE_TEXT("RPL_YOUREOPER"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_REHASHING:
      result = ACE_TEXT("RPL_REHASHING"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_YOURESERVICE:
      result = ACE_TEXT("RPL_YOURESERVICE"); break;
    case RPG_Net_Protocol_IRC_Codes::RPL_MYPORTIS:
      result = ACE_TEXT("RPL_MYPORTIS"); break;

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

    case RPG_Net_Protocol_IRC_Codes::ERR_UNKNOWNCOMMAND:
      result = ACE_TEXT("ERR_UNKNOWNCOMMAND"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_NOMOTD:
      result = ACE_TEXT("ERR_NOMOTD"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_NOADMININFO:
      result = ACE_TEXT("ERR_NOADMININFO"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_FILEERROR:
      result = ACE_TEXT("ERR_FILEERROR"); break;

    case RPG_Net_Protocol_IRC_Codes::ERR_NONICKNAMEGIVEN:
      result = ACE_TEXT("ERR_NONICKNAMEGIVEN"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_ERRONEUSNICKNAME:
      result = ACE_TEXT("ERR_ERRONEUSNICKNAME"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_NICKNAMEINUSE:
      result = ACE_TEXT("ERR_NICKNAMEINUSE"); break;

    case RPG_Net_Protocol_IRC_Codes::ERR_NICKCOLLISION:
      result = ACE_TEXT("ERR_NICKCOLLISION"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_UNAVAILRESOURCE:
      result = ACE_TEXT("ERR_UNAVAILRESOURCE"); break;

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

    case RPG_Net_Protocol_IRC_Codes::ERR_NOTREGISTERED:
      result = ACE_TEXT("ERR_NOTREGISTERED"); break;

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

    case RPG_Net_Protocol_IRC_Codes::ERR_NOOPERHOST:
      result = ACE_TEXT("ERR_NOOPERHOST"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_NOSERVICEHOST:
      result = ACE_TEXT("ERR_NOSERVICEHOST"); break;

    case RPG_Net_Protocol_IRC_Codes::ERR_UMODEUNKNOWNFLAG:
      result = ACE_TEXT("ERR_UMODEUNKNOWNFLAG"); break;
    case RPG_Net_Protocol_IRC_Codes::ERR_USERSDONTMATCH:
      result = ACE_TEXT("ERR_USERSDONTMATCH"); break;

    default:
    {
      // *NOTE*: according to the RFC2812, codes between 001-099 are reserved
      // for client-server connections...
      // --> maybe in use by some obscure extension
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("invalid/unknown numeric (was: %d), aborting\n"),
                 numeric_in));

      break;
    }
  } // end SWITCH

  return result;
}
