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

#include "rpg_net_common_tools.h"

#include "rpg_net_defines.h"
#include "rpg_net_packet_headers.h"

#include <rpg_common_tools.h>
#include <rpg_common_file_tools.h>

#include <ace/Log_Msg.h>
#include <ace/INET_Addr.h>
#include <ace/Dirent_Selector.h>

#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
#include <netinet/ether.h>
#endif

#include <sstream>

// init statics
unsigned long RPG_Net_Common_Tools::myMaxNumberOfLogFiles = RPG_NET_DEF_LOG_MAXNUMFILES;

const bool
RPG_Net_Common_Tools::getNextLogFilename(const bool& isServerProcess_in,
                                         const std::string& directory_in,
                                         std::string& FQLogFilename_out)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Common_Tools::getNextLogFilename"));

  // sanity check(s)
  ACE_ASSERT(RPG_Net_Common_Tools::myMaxNumberOfLogFiles >= 1);

  // init return value(s)
  FQLogFilename_out.resize(0);

  // sanity check(s): log directory exists ?
  // No ? --> try to create it then !
  if (!RPG_Common_File_Tools::isDirectory(directory_in))
  {
    if (!RPG_Common_File_Tools::createDirectory(directory_in))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to RPG_Common_File_Tools::createDirectory(\"%s\"), aborting\n"),
                 directory_in.c_str()));

      return false;
    } // end IF

//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("created directory: \"%s\"...\n"),
//                directory_in.c_str()));
  } // end IF

  // construct correct logfilename...
  FQLogFilename_out = directory_in;
  FQLogFilename_out += ACE_DIRECTORY_SEPARATOR_STR;
  std::string logFileName = (isServerProcess_in ? RPG_NET_DEF_LOG_SERVER_FILENAME_PREFIX
                                                : RPG_NET_DEF_LOG_CLIENT_FILENAME_PREFIX);
  logFileName += RPG_NET_DEF_LOG_FILENAME_SUFFIX;
  FQLogFilename_out += logFileName;

  // retrieve all existing logs and sort them alphabetically...
  ACE_Dirent_Selector entries;
  if (entries.open(directory_in.c_str(),
                   (isServerProcess_in ? &RPG_Net_Common_Tools::server_selector
                                       : &RPG_Net_Common_Tools::client_selector),
                   &RPG_Net_Common_Tools::comparator) == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Dirent_Selector::open(\"%s\"): \"%s\", aborting\n"),
               directory_in.c_str(),
               ACE_OS::strerror(errno)));

    return false;
  } // end IF

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("found %d logfiles...\n"),
//              entries.length()));

  // OK: iterate over the entries and perform some magic...
  // *IMPORTANT NOTE*: entries have been sorted alphabetically:
  //                   1 current 2 4 3 --> current 1 2 3 4
  // *TODO*: some malicious user could inject "fake" logfiles which can
  //         "confuse" this algorithm...
  // skip handling of "<PREFIX><SUFFIX>" (if found)...
  // *IMPORTANT NOTE*: <PREFIX><SUFFIX> will become <PREFIX>_1<SUFFIX>...
  bool found_current = false;
  // sscanf settings
  int number = 0;
  int return_val = -1;
  std::string format_string("%d");
  format_string += RPG_NET_DEF_LOG_FILENAME_SUFFIX;
  std::stringstream converter;
  for (int i = entries.length() - 1, index = RPG_Net_Common_Tools::myMaxNumberOfLogFiles - 1;
       i >= 0;
       i--)
  {
    // perform "special treatment" if "<PREFIX><SUFFIX>" found...
    // *TODO*: do this in C++...
    if (ACE_OS::strcmp(entries[i]->d_name,
                       logFileName.c_str()) == 0)
    {
      found_current = true;

      // skip this one for now
      continue;
    } // end IF

    // scan number...
    try
    {
      // *TODO*: do this in C++...
      return_val = ::sscanf(entries[i]->d_name +
                            // skip some characters...
                            (ACE_OS::strlen((isServerProcess_in ? RPG_NET_DEF_LOG_SERVER_FILENAME_PREFIX
                                                                : RPG_NET_DEF_LOG_CLIENT_FILENAME_PREFIX)) + 1),
                            format_string.c_str(),
                            &number);
      if (return_val != 1)
      {
        if (return_val != 0)
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("::sscanf() failed for \"%s\": \"%s\", continuing\n"),
                     entries[i]->d_name,
                     ACE_OS::strerror(errno)));
        } // end IF

        continue;
      } // end IF
    }
    catch (...)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("caught exception in ::sscanf() for \"%s\": \"%s\", continuing\n"),
                 entries[i]->d_name,
                 ACE_OS::strerror(errno)));

      continue;
    }

    // adjust the index, if the number is smaller than max
    if (number < index)
      index = number + 1;

    // if the number is bigger than the max AND we have more than enough logs --> delete it !
    if ((ACE_static_cast(unsigned long, number) >= (RPG_Net_Common_Tools::myMaxNumberOfLogFiles - 1)) &&
        (ACE_static_cast(unsigned long, entries.length()) >= RPG_Net_Common_Tools::myMaxNumberOfLogFiles))
    {
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("removing surplus logfile \"%s\"...\n"),
                 entries[i]->d_name));

      // clean up
      std::string FQfilename = directory_in;
      FQfilename += ACE_DIRECTORY_SEPARATOR_STR;
      FQfilename += entries[i]->d_name;
      RPG_Common_File_Tools::deleteFile(FQfilename);

      continue;
    } // end IF

    // logrotate file...
    std::string oldFQfilename = directory_in;
    oldFQfilename += ACE_DIRECTORY_SEPARATOR_STR;
    oldFQfilename += entries[i]->d_name;

    std::string newFQfilename = directory_in;
    newFQfilename += ACE_DIRECTORY_SEPARATOR_STR;
    newFQfilename += (isServerProcess_in ? RPG_NET_DEF_LOG_SERVER_FILENAME_PREFIX
                                         : RPG_NET_DEF_LOG_CLIENT_FILENAME_PREFIX);
    newFQfilename += "_";
    converter.str(ACE_TEXT_ALWAYS_CHAR(""));
    converter << index;
    newFQfilename += converter.str();
    newFQfilename += RPG_NET_DEF_LOG_FILENAME_SUFFIX;
    // *IMPORTANT NOTE*: last parameter affects Win32 behaviour only,
    // see "ace/OS_NS_stdio.inl" !
    if (ACE_OS::rename(oldFQfilename.c_str(),
                       newFQfilename.c_str(),
                       -1))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_OS::rename() \"%s\" to \"%s\": \"%s\", aborting\n"),
                 oldFQfilename.c_str(),
                 newFQfilename.c_str(),
                 ACE_OS::strerror(errno)));

      return false;
    } // end IF

    // debug info
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("renamed file \"%s\" to \"%s\"...\n"),
               oldFQfilename.c_str(),
               newFQfilename.c_str()));

    index--;
  } // end FOR

  if (found_current)
  {
    std::string newFQfilename = directory_in;
    newFQfilename += ACE_DIRECTORY_SEPARATOR_STR;
    newFQfilename += (isServerProcess_in ? RPG_NET_DEF_LOG_SERVER_FILENAME_PREFIX
                                         : RPG_NET_DEF_LOG_CLIENT_FILENAME_PREFIX);
    newFQfilename += "_1";
    newFQfilename += RPG_NET_DEF_LOG_FILENAME_SUFFIX;

    // *TODO*: last parameter affects Win32 behaviour only, see "ace/OS_NS_stdio.inl" !
    if (ACE_OS::rename(FQLogFilename_out.c_str(),
                       newFQfilename.c_str(),
                       -1))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_OS::rename() \"%s\" to \"%s\": \"%s\", aborting\n"),
                 FQLogFilename_out.c_str(),
                 newFQfilename.c_str(),
                 ACE_OS::strerror(errno)));

      return false;
    } // end IF

    // debug info
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("renamed file \"%s\" to \"%s\"...\n"),
               FQLogFilename_out.c_str(),
               newFQfilename.c_str()));
  } // end IF

  return true;
}

const std::string
RPG_Net_Common_Tools::messageType2String(const RPG_Net_Remote_Comm::MessageType& messageType_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Common_Tools::messageType2String"));

  std::string result;

  switch (messageType_in)
  {
    case RPG_Net_Remote_Comm::RPG_NET_PING:
    {
      result = ACE_TEXT("RPG_NET_PING");

      break;
    }
    case RPG_Net_Remote_Comm::RPG_NET_PONG:
    {
      result = ACE_TEXT("RPG_NET_PONG");

      break;
    }
    default:
    {
      // debug info
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid message header: %d, aborting\n"),
                 messageType_in));

      break;
    }
  } // end SWITCH

  return result;
}

const std::string
RPG_Net_Common_Tools::IPAddress2String(const unsigned short& port_in,
                                       const unsigned long& IPAddress_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Common_Tools::IPAddress2String"));

  // init return value(s)
  std::string result;

  ACE_INET_Addr inet_addr;
  ACE_TCHAR buffer[32]; // "xxx.xxx.xxx.xxx:yyyyy\0"
  ACE_OS::memset(&buffer,
                 0,
                 sizeof(buffer));
  if (inet_addr.set(port_in,
                    IPAddress_in,
                    0,  // no need to encode, data IS in network byte order !
                    0)) // only needed for IPv6...
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Inet_Addr::set: \"%s\", aborting\n"),
               ACE_OS::strerror(errno)));

    return std::string(ACE_TEXT("invalid_IP_address"));
  }
  if (inet_addr.addr_to_string(buffer,
                               sizeof(buffer),
                               1)) // want IP address, not hostname !
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Inet_Addr::addr_to_string: \"%s\", aborting\n"),
               ACE_OS::strerror(errno)));

    return std::string(ACE_TEXT("invalid_IP_address"));
  }

  // copy string from buffer
  result = buffer;

  // clean up: if port number was 0, we cut off the trailing ":0" !
  if (!port_in)
  {
    std::string::size_type last_colon_pos = result.find_last_of(':',
                                                                std::string::npos); // begin searching at the end !
    if (last_colon_pos != std::string::npos)
    {
      result = result.substr(0, last_colon_pos);
    } // end IF
  } // end IF

  return result;
}

const std::string
RPG_Net_Common_Tools::IPProtocol2String(const unsigned char& protocol_in)
{
  ACE_TRACE("RPG_Net_Common_Tools::IPProtocol2String");

    // init return value(s)
  std::string result = ACE_TEXT("INVALID_PROTOCOL");

  switch (protocol_in)
  {
    case IPPROTO_IP: // OR case IPPROTO_HOPOPTS:
    {
      result = ACE_TEXT("IPPROTO_IP/IPPROTO_HOPOPTS");

      break;
    }
    case IPPROTO_ICMP:
    {
      result = ACE_TEXT("IPPROTO_ICMP");

      break;
    }
    case IPPROTO_IGMP:
    {
      result = ACE_TEXT("IPPROTO_IGMP");

      break;
    }
    case IPPROTO_IPIP:
    {
      result = ACE_TEXT("IPPROTO_IPIP");

      break;
    }
    case IPPROTO_TCP:
    {
      result = ACE_TEXT("IPPROTO_TCP");

      break;
    }
    case IPPROTO_EGP:
    {
      result = ACE_TEXT("IPPROTO_EGP");

      break;
    }
    case IPPROTO_PUP:
    {
      result = ACE_TEXT("IPPROTO_PUP");

      break;
    }
    case IPPROTO_UDP:
    {
      result = ACE_TEXT("IPPROTO_UDP");

      break;
    }
    case IPPROTO_IDP:
    {
      result = ACE_TEXT("IPPROTO_IDP");

      break;
    }
    case IPPROTO_TP:
    {
      result = ACE_TEXT("IPPROTO_TP");

      break;
    }
    case IPPROTO_IPV6:
    {
      result = ACE_TEXT("IPPROTO_IPV6");

      break;
    }
    case IPPROTO_ROUTING:
    {
      result = ACE_TEXT("IPPROTO_ROUTING");

      break;
    }
    case IPPROTO_FRAGMENT:
    {
      result = ACE_TEXT("IPPROTO_FRAGMENT");

      break;
    }
    case IPPROTO_RSVP:
    {
      result = ACE_TEXT("IPPROTO_RSVP");

      break;
    }
    case IPPROTO_GRE:
    {
      result = ACE_TEXT("IPPROTO_GRE");

      break;
    }
    case IPPROTO_ESP:
    {
      result = ACE_TEXT("IPPROTO_ESP");

      break;
    }
    case IPPROTO_AH:
    {
      result = ACE_TEXT("IPPROTO_AH");

      break;
    }
    case IPPROTO_ICMPV6:
    {
      result = ACE_TEXT("IPPROTO_ICMPV6");

      break;
    }
    case IPPROTO_NONE:
    {
      result = ACE_TEXT("IPPROTO_NONE");

      break;
    }
    case IPPROTO_DSTOPTS:
    {
      result = ACE_TEXT("IPPROTO_DSTOPTS");

      break;
    }
    case IPPROTO_MTP:
    {
      result = ACE_TEXT("IPPROTO_MTP");

      break;
    }
    case IPPROTO_ENCAP:
    {
      result = ACE_TEXT("IPPROTO_ENCAP");

      break;
    }
    case IPPROTO_PIM:
    {
      result = ACE_TEXT("IPPROTO_PIM");

      break;
    }
    case IPPROTO_COMP:
    {
      result = ACE_TEXT("IPPROTO_COMP");

      break;
    }
    case IPPROTO_SCTP:
    {
      result = ACE_TEXT("IPPROTO_SCTP");

      break;
    }
    case IPPROTO_RAW:
    {
      result = ACE_TEXT("IPPROTO_RAW");

      break;
    }
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid/unknown IP protocol: %1u, continuing\n"),
                 protocol_in));

      break;
    }
  } // end SWITCH

  return result;
}

const std::string
RPG_Net_Common_Tools::MACAddress2String(const char* const addressDataPtr_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Common_Tools::MACAddress2String"));

  // init return value(s)
  std::string result;

  // convert 6 bytes to ASCII
  // *IMPORTANT NOTE*: ether_ntoa_r is not portable...
  // *TODO*: ether_ntoa_r is not portable...
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
  // *TODO*: make this thread-safe !
  static char address[(ETH_ALEN * 2) + ETH_ALEN + 1]; // "ab:cd:ef:gh:ij:kl\0"
  ACE_OS::memset(&address,
                 0,
                 sizeof(address));
  if (::ether_ntoa_r(ACE_reinterpret_cast(const ether_addr*, addressDataPtr_in),
                     address) != address)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ::ether_ntoa_r(): \"%s\", returning\n"),
               ACE_OS::strerror(errno)));

    return std::string(ACE_TEXT("invalid_FDDI_address"));
  } // end IF

  result = address;
#else
  // *TODO*: implement this
  ACE_ASSERT(false);
#endif

  return result;
}

const std::string
RPG_Net_Common_Tools::EthernetProtocolTypeID2String(const unsigned short& frameType_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Common_Tools::EthernetProtocolTypeID2String"));

  // init return value(s)
  std::string result = ACE_TEXT("UNKNOWN_ETHERNET_FRAME_TYPE");

  switch (ACE_NTOHS(frameType_in))
  {
    case ETH_P_LOOP:
    {
      result = ACE_TEXT("ETH_P_LOOP");

      break;
    }
    case ETHERTYPE_GRE_ISO:
    {
      result = ACE_TEXT("ETHERTYPE_GRE_ISO");

      break;
    }
    case ETH_P_PUP:
    {
      result = ACE_TEXT("ETH_P_PUP");

      break;
    }
    case ETH_P_PUPAT:
    {
      result = ACE_TEXT("ETH_P_PUPAT");

      break;
    }
    case ETHERTYPE_SPRITE:
    {
      result = ACE_TEXT("ETHERTYPE_SPRITE");

      break;
    }
    case ETH_P_IP:
    {
      result = ACE_TEXT("ETH_P_IP");

      break;
    }
    case ETH_P_X25:
    {
      result = ACE_TEXT("ETH_P_X25");

      break;
    }
    case ETH_P_ARP:
    {
      result = ACE_TEXT("ETH_P_ARP");

      break;
    }
    case ETH_P_BPQ:
    {
      result = ACE_TEXT("ETH_P_BPQ");

      break;
    }
    case ETH_P_IEEEPUP:
    {
      result = ACE_TEXT("ETH_P_IEEEPUP");

      break;
    }
    case ETH_P_IEEEPUPAT:
    {
      result = ACE_TEXT("ETH_P_IEEEPUPAT");

      break;
    }
    case ETHERTYPE_NS:
    {
      result = ACE_TEXT("ETHERTYPE_NS");

      break;
    }
    case ETHERTYPE_TRAIL:
    {
      result = ACE_TEXT("ETHERTYPE_TRAIL");

      break;
    }
    case ETH_P_DEC:
    {
      result = ACE_TEXT("ETH_P_DEC");

      break;
    }
    case ETH_P_DNA_DL:
    {
      result = ACE_TEXT("ETH_P_DNA_DL");

      break;
    }
    case ETH_P_DNA_RC:
    {
      result = ACE_TEXT("ETH_P_DNA_RC");

      break;
    }
    case ETH_P_DNA_RT:
    {
      result = ACE_TEXT("ETH_P_DNA_RT");

      break;
    }
    case ETH_P_LAT:
    {
      result = ACE_TEXT("ETH_P_LAT");

      break;
    }
    case ETH_P_DIAG:
    {
      result = ACE_TEXT("ETH_P_DIAG");

      break;
    }
    case ETH_P_CUST:
    {
      result = ACE_TEXT("ETH_P_CUST");

      break;
    }
    case ETH_P_SCA:
    {
      result = ACE_TEXT("ETH_P_SCA");

      break;
    }
    case ETH_P_RARP:
    {
      result = ACE_TEXT("ETH_P_RARP");

      break;
    }
    case ETHERTYPE_LANBRIDGE:
    {
      result = ACE_TEXT("ETHERTYPE_LANBRIDGE");

      break;
    }
    case ETHERTYPE_DECDNS:
    {
      result = ACE_TEXT("ETHERTYPE_DECDNS");

      break;
    }
    case ETHERTYPE_DECDTS:
    {
      result = ACE_TEXT("ETHERTYPE_DECDTS");

      break;
    }
    case ETHERTYPE_VEXP:
    {
      result = ACE_TEXT("ETHERTYPE_VEXP");

      break;
    }
    case ETHERTYPE_VPROD:
    {
      result = ACE_TEXT("ETHERTYPE_VPROD");

      break;
    }
    case ETH_P_ATALK:
    {
      result = ACE_TEXT("ETH_P_ATALK");

      break;
    }
    case ETH_P_AARP:
    {
      result = ACE_TEXT("ETH_P_AARP");

      break;
    }
    case ETH_P_8021Q:
    {
      result = ACE_TEXT("ETH_P_8021Q");

      break;
    }
    case ETH_P_IPX:
    {
      result = ACE_TEXT("ETH_P_IPX");

      break;
    }
    case ETH_P_IPV6:
    {
      result = ACE_TEXT("ETH_P_IPV6");

      break;
    }
    case ETHERTYPE_MPCP:
    {
      result = ACE_TEXT("ETHERTYPE_MPCP");

      break;
    }
    case ETHERTYPE_SLOW:
    {
      result = ACE_TEXT("ETHERTYPE_SLOW");

      break;
    }
    case ETHERTYPE_PPP:
    {
      result = ACE_TEXT("ETHERTYPE_PPP");

      break;
    }
    case ETH_P_WCCP:
    {
      result = ACE_TEXT("ETH_P_WCCP");

      break;
    }
    case ETH_P_MPLS_UC:
    {
      result = ACE_TEXT("ETH_P_MPLS_UC");

      break;
    }
    case ETH_P_MPLS_MC:
    {
      result = ACE_TEXT("ETH_P_MPLS_MC");

      break;
    }
    case ETH_P_ATMMPOA:
    {
      result = ACE_TEXT("ETH_P_ATMMPOA");

      break;
    }
    case ETH_P_PPP_DISC:
    {
      result = ACE_TEXT("ETH_P_PPP_DISC");

      break;
    }
    case ETH_P_PPP_SES:
    {
      result = ACE_TEXT("ETH_P_PPP_SES");

      break;
    }
    case ETHERTYPE_JUMBO:
    {
      result = ACE_TEXT("ETHERTYPE_JUMBO");

      break;
    }
    case ETH_P_ATMFATE:
    {
      result = ACE_TEXT("ETH_P_ATMFATE");

      break;
    }
    case ETHERTYPE_EAPOL:
    {
      result = ACE_TEXT("ETHERTYPE_EAPOL");

      break;
    }
    case ETH_P_AOE:
    {
      result = ACE_TEXT("ETH_P_AOE");

      break;
    }
    case ETH_P_TIPC:
    {
      result = ACE_TEXT("ETH_P_TIPC");

      break;
    }
    case ETHERTYPE_LOOPBACK:
    {
      result = ACE_TEXT("ETHERTYPE_LOOPBACK");

      break;
    }
    case ETHERTYPE_VMAN:
    {
      result = ACE_TEXT("ETHERTYPE_VMAN");

      break;
    }
    case ETHERTYPE_ISO:
    {
      result = ACE_TEXT("ETHERTYPE_ISO");

      break;
    }
// ********************* Non DIX types ***********************
    case ETH_P_802_3:
    {
      result = ACE_TEXT("ETH_P_802_3");

      break;
    }
    case ETH_P_AX25:
    {
      result = ACE_TEXT("ETH_P_AX25");

      break;
    }
    case ETH_P_ALL:
    {
      result = ACE_TEXT("ETH_P_ALL");

      break;
    }
    case ETH_P_802_2:
    {
      result = ACE_TEXT("ETH_P_802_2");

      break;
    }
    case ETH_P_SNAP:
    {
      result = ACE_TEXT("ETH_P_SNAP");

      break;
    }
    case ETH_P_DDCMP:
    {
      result = ACE_TEXT("ETH_P_DDCMP");

      break;
    }
    case ETH_P_WAN_PPP:
    {
      result = ACE_TEXT("ETH_P_WAN_PPP");

      break;
    }
    case ETH_P_PPP_MP:
    {
      result = ACE_TEXT("ETH_P_PPP_MP");

      break;
    }
    case ETH_P_LOCALTALK:
    {
      result = ACE_TEXT("ETH_P_LOCALTALK");

      break;
    }
    case ETH_P_PPPTALK:
    {
      result = ACE_TEXT("ETH_P_PPPTALK");

      break;
    }
    case ETH_P_TR_802_2:
    {
      result = ACE_TEXT("ETH_P_TR_802_2");

      break;
    }
    case ETH_P_MOBITEX:
    {
      result = ACE_TEXT("ETH_P_MOBITEX");

      break;
    }
    case ETH_P_CONTROL:
    {
      result = ACE_TEXT("ETH_P_CONTROL");

      break;
    }
    case ETH_P_IRDA:
    {
      result = ACE_TEXT("ETH_P_IRDA");

      break;
    }
    case ETH_P_ECONET:
    {
      result = ACE_TEXT("ETH_P_ECONET");

      break;
    }
    case ETH_P_HDLC:
    {
      result = ACE_TEXT("ETH_P_HDLC");

      break;
    }
    case ETH_P_ARCNET:
    {
      result = ACE_TEXT("ETH_P_ARCNET");

      break;
    }
    default:
    {
      ACE_DEBUG((LM_WARNING,
                 ACE_TEXT("invalid/unknown ethernet frame type: \"0x%x\" --> check implementation !, continuing\n"),
                 ACE_NTOHS(frameType_in)));

      // IEEE 802.3 ? --> change result string...
      if (ACE_NTOHS(frameType_in) <= ETH_DATA_LEN)
      {
        result = ACE_TEXT("UNKNOWN_IEEE_802_3_FRAME_TYPE");
      } // end IF

      break;
    }
  } // end SWITCH

  return result;
}

// const bool
// RPG_Net_Common_Tools::selectNetworkInterface(const std::string& defaultInterfaceIdentifier_in,
//                                              std::string& interfaceIdentifier_out)
// {
//   ACE_TRACE(ACE_TEXT("RPG_Net_Common_Tools::selectNetworkInterface"));
//
//   // init return value(s)
//   interfaceIdentifier_out.resize(0);
//
//   pcap_if_t* all_devices = NULL;
//   char errbuf[PCAP_ERRBUF_SIZE]; // defined in pcap.h
//   if (pcap_findalldevs(&all_devices,
//                        errbuf) == -1)
//   {
//     ACE_DEBUG((LM_ERROR,
//                ACE_TEXT("failed to pcap_findalldevs(): \"%s\", aborting\n"),
//                errbuf));
//
//     return false;
//   } // end IF
//
//   // found default interface ?
//   unsigned long i = 1;
//   bool found_default = false;
//   pcap_if_t* device = NULL;
//   for (device = all_devices;
//        device;
//        device = device->next, i++)
//   {
//     // debug info
//     ACE_DEBUG((LM_INFO,
//                ACE_TEXT("#%u \"%s\": \"%s\"\n"),
//                i,
//                device->name,
//                (device->description ?
//                 device->description :
//                 ACE_TEXT("no description available"))));
//
//     if (defaultInterfaceIdentifier_in == std::string(device->name))
//     {
//       interfaceIdentifier_out = defaultInterfaceIdentifier_in;
//       found_default = true;
//     } // end IF
//   } // end FOR
//   i--;
//
//   // sanity check: found any suitable device at all ?
//   if (!all_devices)
//   {
//     ACE_DEBUG((LM_ERROR,
//                ACE_TEXT("no interfaces found, aborting\n")));
//
//     // clean up
//     pcap_freealldevs(all_devices);
//
//     return false;
//   } // end IF
//
//   // couldn't find default interface ? ask user !
//   if (!found_default)
//   {
//     unsigned long device_number = 0;
//     std::cout << ACE_TEXT("default interface \"")
//               << defaultInterfaceIdentifier_in.c_str()
//               << ACE_TEXT("\" not found, please enter a valid interface number (1-")
//               << i
//               << ACE_TEXT("): ");
//     std::cin >> device_number;
//
//     // sanity check: out of range ?
//     if ((device_number < 1) ||
//          (device_number > i))
//     {
//       ACE_DEBUG((LM_ERROR,
//                  ACE_TEXT("selection: %u was out of range, aborting\n"),
//                  device_number));
//
//       // clean up
//       pcap_freealldevs(all_devices);
//
//       return false;
//     } // end IF
//
//     // get selected device name
//     for (device = all_devices, i = 0;
//          i < (device_number - 1);
//          device = device->next, i++);
//
//     interfaceIdentifier_out = device->name;
//   } // end IF
//
//   // clean up
//   pcap_freealldevs(all_devices);
//
//   return true;
// }

const bool
RPG_Net_Common_Tools::retrieveLocalIPAddress(const std::string& interfaceIdentifier_in,
                                             std::string& IPaddress_out)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Common_Tools::retrieveLocalIPAddress"));

  // init return value(s)
  IPaddress_out.resize(0);

//   // validate/retrieve interface identifier index
//   unsigned long index = 0;
//   // *PORTABILITY*
//   index = ACE_OS::if_nametoindex(interfaceIdentifier_in.c_str());
//   if (!index)
//   {
//     ACE_DEBUG((LM_ERROR,
//                ACE_TEXT("failed to ACE_OS::if_nametoindex(\"%s\"): \"%s\", aborting\n"),
//                interfaceIdentifier_in.c_str(),
//                ACE_OS::strerror(errno)));
  //
//     return false;
//   } // end IF

  // *NOTE*: we ask the kernel...
  size_t count = 0;
  ACE_INET_Addr* addr_array = NULL;
  if (ACE::get_ip_interfaces(count,
                             addr_array))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE::get_ip_interfaces(): \"%s\", aborting\n"),
               ACE_OS::strerror(errno)));

    return false;
  } // end IF
  if (count == 0)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("could not find any IP network interfaces, aborting\n")));

    return false;
  } // end IF

  // *NOTE*: beyond this point, we need to clean up addr_array...

  ACE_TCHAR ip[MAXHOSTNAMELEN + 1];
  ACE_OS::memset(&ip,
                 0,
                 sizeof(ip));
  sockaddr_in* addr_handle = NULL;
  for (size_t i = 0;
       i < count;
       i++)
  {
    // reset buffer
    ACE_OS::memset(&ip,
                   0,
                   sizeof(ip));

    // reset address handle
    addr_handle = NULL;

    // sanity check: only support IPv4 (for now)
    if (addr_array[i].get_type() != AF_INET)
    {
      // try next one...
      continue;
    } // end IF

//     // sanity check: ignore loopback
//     if (addr_array[i].is_loopback())
//     {
//       // try next one...
//       continue;
//     } // end IF

    if (addr_array[i].get_host_addr(ip,
                                    MAXHOSTNAMELEN) == NULL)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_INET_Addr::get_host_addr(): \"%s\", aborting\n"),
                 ACE_OS::strerror(errno)));

      // clean up
      delete[] addr_array;
      addr_array = NULL;

      return false;
    } // end IF

    // debug info
    // *TODO*: sanity check: correct interface ?
    // --> how can we find out which interface this address is assigned to ?
//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("found host address: \"%s\"...\n"),
//                ip));

//     addr_handle = ACE_static_cast(sockaddr_in*,
//                                   addr_array[i].get_addr());

    IPaddress_out = ip;
  } // end FOR

  // clean up
  delete[] addr_array;
  addr_array = NULL;

  return true;
}

const bool
RPG_Net_Common_Tools::retrieveLocalHostname(std::string& hostname_out)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Common_Tools::retrieveLocalHostname"));

  // init return value(s)
  hostname_out.resize(0);

  ACE_TCHAR host[MAXHOSTNAMELEN + 1];
  ACE_OS::memset(&host,
                 0,
                 MAXHOSTNAMELEN + 1);
  if (ACE_OS::hostname(host, MAXHOSTNAMELEN))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_OS::hostname(): \"%s\", aborting\n"),
               ACE_OS::strerror(errno)));

    return false;
  } // end IF

  hostname_out = host;

  return true;
}

const bool
RPG_Net_Common_Tools::setSocketBuffer(const ACE_HANDLE& handle_in,
                                      const int& option_in,
                                      const int& size_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Common_Tools::setSocketBuffer"));

  // sanity check
  ACE_ASSERT((option_in == SO_RCVBUF) ||
             (option_in == SO_SNDBUF));

  int size = size_in;
  // *NOTE*: for some reason, Linux will actually set TWICE the size value
  // --> we pass it HALF the value so we get our will...
  if (RPG_Common_Tools::isLinux())
  {
    size /= 2;

    if (size_in % 2)
    { // debug info
      ACE_DEBUG((LM_WARNING,
                 ACE_TEXT("requested %s buffer size %u is ODD...\n"),
                 ((option_in == SO_SNDBUF) ? ACE_TEXT("SO_SNDBUF") : ACE_TEXT("SO_RCVBUF")),
                 size_in));
    } // end IF
  } // end IF

  if (ACE_OS::setsockopt(handle_in,
                         SOL_SOCKET,
                         option_in,
                         ACE_reinterpret_cast(const char*, &size),
                         sizeof(int)))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_OS::setsockopt(%u, %s): \"%s\", aborting\n"),
               handle_in,
               ((option_in == SO_SNDBUF) ? ACE_TEXT("SO_SNDBUF") : ACE_TEXT("SO_RCVBUF")),
               ACE_OS::strerror(errno)));

    return false;
  } // end IF

  // validate result
  size = 0;
  int retsize = sizeof(int);
  if (ACE_OS::getsockopt(handle_in,
                         SOL_SOCKET,
                         option_in,
                         ACE_reinterpret_cast(char*, &size),
                         &retsize))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_OS::getsockopt(%u, %s): \"%s\", aborting\n"),
               handle_in,
               ((option_in == SO_SNDBUF) ? ACE_TEXT("SO_SNDBUF") : ACE_TEXT("SO_RCVBUF")),
               ACE_OS::strerror(errno)));

    return false;
  } // end IF

  if (size != size_in)
  {
    ACE_DEBUG((LM_WARNING,
               ACE_TEXT("ACE_OS::getsockopt(%s) on handle (ID: %d) returned %d (expected: %d), aborting\n"),
               ((option_in == SO_SNDBUF) ? ACE_TEXT("SO_SNDBUF") : ACE_TEXT("SO_RCVBUF")),
               handle_in,
               size,
               size_in));

    // *NOTE*: may happen on Linux systems (e.g. IF size_in is odd, see above)
    if (RPG_Common_Tools::isLinux() && (size_in % 2))
      return true; // this may lead to a false positive...

      return false;
  } // end IF

//   // debug info
//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("set \"%s\" option of socket (ID: %d) to: %d\n"),
//              ((buffer_in == SO_RCVBUF) ? ACE_TEXT("SO_RCVBUF")
//                                        : ACE_TEXT("SO_SNDBUF")),
//              handle_in,
//              size));

  return true;
}

const bool
RPG_Net_Common_Tools::setNoDelay(const ACE_HANDLE& handle_in,
                                 const bool& noDelay_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Common_Tools::setNoDelay"));

  int value = (noDelay_in ? 1 : 0);
  if (ACE_OS::setsockopt(handle_in,
                         IPPROTO_TCP,
                         TCP_NODELAY,
                         ACE_reinterpret_cast(const char*, &value),
                         sizeof(int)))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_OS::setsockopt(%u, TCP_NODELAY): \"%s\", aborting\n"),
               handle_in,
               ACE_OS::strerror(errno)));

    return false;
  } // end IF

  // validate result
  int retsize = sizeof(int);
  value = 0;
  if (ACE_OS::getsockopt(handle_in,
                         IPPROTO_TCP,
                         TCP_NODELAY,
                         ACE_reinterpret_cast(char*, &value),
                         &retsize))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_OS::getsockopt(%u, TCP_NODELAY): \"%s\", aborting\n"),
               handle_in,
               ACE_OS::strerror(errno)));

    return false;
  } // end IF

  return (noDelay_in ? (value == 1) : (value == 0));
}

int
RPG_Net_Common_Tools::client_selector(const dirent* dirEntry_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Common_Tools::client_selector"));

  // *IMPORTANT NOTE*: we need to select those files which follow our naming
  // schema for logfiles: "<PREFIX>[_<NUMBER>]<SUFFIX>"

  // sanity check --> prefix ok ?
  if (ACE_OS::strncmp(dirEntry_in->d_name,
                      RPG_NET_DEF_LOG_CLIENT_FILENAME_PREFIX,
                      ACE_OS::strlen(RPG_NET_DEF_LOG_CLIENT_FILENAME_PREFIX)) != 0)
  {
//     // debug info
//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("ignoring \"%s\"...\n"),
//                dirEntry_in->d_name));

    return 0;
  } // end IF

  return 1;
}

int
RPG_Net_Common_Tools::server_selector(const dirent* dirEntry_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Common_Tools::server_selector"));

  // *IMPORTANT NOTE*: we need to select those files which follow our naming
  // schema for logfiles: "<PREFIX>[_<NUMBER>]<SUFFIX>"

  // sanity check --> prefix ok ?
  if (ACE_OS::strncmp(dirEntry_in->d_name,
                      RPG_NET_DEF_LOG_SERVER_FILENAME_PREFIX,
                      ACE_OS::strlen(RPG_NET_DEF_LOG_SERVER_FILENAME_PREFIX)) != 0)
  {
//     // debug info
//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("ignoring \"%s\"...\n"),
//                dirEntry_in->d_name));

    return 0;
  } // end IF

  return 1;
}

int
RPG_Net_Common_Tools::comparator(const dirent** d1,
                                 const dirent** d2)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Common_Tools::comparator"));

  return ACE_OS::strcmp((*d1)->d_name,
                        (*d2)->d_name);
}
