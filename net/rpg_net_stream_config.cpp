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
#include "stdafx.h"

#include "rpg_net_stream_config.h"

#include <rpg_common_macros.h>

RPG_Net_StreamConfig::RPG_Net_StreamConfig(const RPG_Net_ConfigPOD& config_in,
                                           const ACE_Time_Value& startOfSession_in,
                                           const bool& userAbort_in)
 : inherited(config_in,
             startOfSession_in,
             userAbort_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_StreamConfig::RPG_Net_StreamConfig"));

}

RPG_Net_StreamConfig::~RPG_Net_StreamConfig()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_StreamConfig::~RPG_Net_StreamConfig"));

}

void
RPG_Net_StreamConfig::dump_state() const
{
  RPG_TRACE(ACE_TEXT("RPG_Net_StreamConfig::dump_state"));

  // *TODO*
  ACE_ASSERT(false);
//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("start of session: %d\n%s"),
//              myStartOfSession,
//              (myUserAbort ? ACE_TEXT("user abort !")
//                           : ACE_TEXT(""))));
  inherited::dump_state();
}
