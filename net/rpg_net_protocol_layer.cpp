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

#include "rpg_net_protocol_layer.h"

#include <ace/Log_Msg.h>

void
RPG_Net_Protocol_Layer::ProtocolLayer2String(const ProtocolLayer& protocolType_in,
                                             std::string& typeString_out)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_Layer::ProtocolLayer2String"));

  // init return value(s)
  typeString_out = ACE_TEXT("INVALID_PROTOCOL");

  switch (protocolType_in)
  {
    case ETHERNET:
    {
      typeString_out = ACE_TEXT("ETHERNET");

      break;
    }
    case FDDI_LLC_SNAP:
    {
      typeString_out = ACE_TEXT("FDDI_LLC_SNAP");

      break;
    }
    case IPv4:
    {
      typeString_out = ACE_TEXT("IPv4");

      break;
    }
    case IPv6:
    {
      typeString_out = ACE_TEXT("IPv6");

      break;
    }
    case ICMP:
    {
      typeString_out = ACE_TEXT("ICMP");

      break;
    }
    case IGMP:
    {
      typeString_out = ACE_TEXT("IGMP");

      break;
    }
    case UDP:
    {
      typeString_out = ACE_TEXT("UDP");

      break;
    }
    case TCP:
    {
      typeString_out = ACE_TEXT("TCP");

      break;
    }
    case RAW:
    {
      typeString_out = ACE_TEXT("RAW");

      break;
    }
    case PIM:
    {
      typeString_out = ACE_TEXT("PIM");

      break;
    }
    case ASTERIX_offset:
    {
      typeString_out = ACE_TEXT("ASTERIX_offset");

      break;
    }
    case ASTERIX:
    {
      typeString_out = ACE_TEXT("ASTERIX");

      break;
    }
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid/unknown protocol (type: \"%d\"), continuing\n"),
                 protocolType_in));

      break;
    }
  } // end SWITCH
}
