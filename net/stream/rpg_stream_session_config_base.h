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

#ifndef RPG_STREAM_SESSION_CONFIG_BASE_H
#define RPG_STREAM_SESSION_CONFIG_BASE_H

#include "rpg_common_referencecounter_base.h"

#include <ace/Global_Macros.h>
#include <ace/Time_Value.h>

template <typename DataType>
class RPG_Stream_SessionConfigBase
 : public RPG_Common_ReferenceCounterBase
{
 public:
  RPG_Stream_SessionConfigBase(const DataType&,                              // user data
                               const ACE_Time_Value& = ACE_Time_Value::zero, // "official" start of session
                               const bool& = false);                         // session ended because of user abort ?
  virtual ~RPG_Stream_SessionConfigBase();

  // info
  DataType getUserData() const;
  ACE_Time_Value getStartOfSession() const;
  bool getUserAbort() const;

  // implement RPG_Common_IDumpState
  virtual void dump_state() const;

 private:
  typedef RPG_Common_ReferenceCounterBase inherited;

  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Stream_SessionConfigBase());
  ACE_UNIMPLEMENTED_FUNC(RPG_Stream_SessionConfigBase(const RPG_Stream_SessionConfigBase<DataType>&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Stream_SessionConfigBase<DataType>& operator=(const RPG_Stream_SessionConfigBase<DataType>&));

  DataType       myUserData;
  ACE_Time_Value myStartOfSession;
  bool           myUserAbort;
};

// include template implementation
#include "rpg_stream_session_config_base.inl"

#endif
