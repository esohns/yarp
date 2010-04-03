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

#ifndef RPG_NET_STREAM_CONFIG_H
#define RPG_NET_STREAM_CONFIG_H

#include "rpg_net_common.h"

#include <stream_session_config_base.h>

#include <ace/Global_Macros.h>

class RPG_Net_StreamConfig
 : public Stream_SessionConfigBase<RPG_Net_ConfigPOD>
{
 public:
  RPG_Net_StreamConfig(const RPG_Net_ConfigPOD&,                     // user data
                       const ACE_Time_Value& = ACE_Time_Value::zero, // "official" start of session
                       const bool& = false);                         // session ended because of user abort ?

  // override RPG_Common_IDumpState
  virtual void dump_state() const;

 private:
  typedef Stream_SessionConfigBase<RPG_Net_ConfigPOD> inherited;

  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_StreamConfig());
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_StreamConfig(const RPG_Net_StreamConfig&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Net_StreamConfig& operator=(const RPG_Net_StreamConfig&));
  virtual ~RPG_Net_StreamConfig();
};

#endif
