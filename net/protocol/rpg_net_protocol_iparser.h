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

#ifndef RPG_NET_PROTOCOL_IPARSER_T_H
#define RPG_NET_PROTOCOL_IPARSER_T_H

#include "ace/Basic_Types.h"

#include "common_iscanner.h"
#include "common_iparser.h"

#include "common_parser_common.h"

#include "rpg_net_protocol_common.h"

class RPG_Net_Protocol_IParser
 : public Common_IYaccRecordParser_T<struct Common_FlexBisonParserConfiguration,
                                     struct RPG_Net_Protocol_Command>
 , virtual public Common_ILexScanner_T<struct Common_FlexScannerState,
                                       RPG_Net_Protocol_IParser>
{
 public:
  // convenient types
  typedef Common_IYaccRecordParser_T<struct Common_FlexBisonParserConfiguration,
                                     struct RPG_Net_Protocol_Command> IPARSER_T;
  typedef Common_ILexScanner_T<struct Common_FlexScannerState,
                               RPG_Net_Protocol_IParser> ISCANNER_T;

  using IPARSER_T::error;
//  using Common_IScanner::error;

  virtual void scannedBytes (ACE_UINT32) = 0; // add to scanned bytes
  virtual ACE_UINT32 scannedBytes () = 0; // get scanned bytes
  virtual void length (ACE_UINT32) = 0; // set message length
  virtual ACE_UINT32 length () = 0; // get message length
};

//////////////////////////////////////////

typedef Common_ILexScanner_T<struct Common_FlexScannerState,
                             RPG_Net_Protocol_IParser> RPG_Net_Protocol_IScanner_t;

#endif
