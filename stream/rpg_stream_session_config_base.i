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

#include "rpg_stream_tools.h"

#include <ace/OS.h>

template <typename DataType>
RPG_Stream_SessionConfigBase<DataType>::RPG_Stream_SessionConfigBase(const DataType& userData_in,
                                                             const ACE_Time_Value& startOfSession_in,
                                                             const bool& userAbort_in)
 : inherited(1),
   myUserData(userData_in),
   myStartOfSession(startOfSession_in),
   myUserAbort(userAbort_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Stream_SessionConfigBase::RPG_Stream_SessionConfigBase"));

}

template <typename DataType>
RPG_Stream_SessionConfigBase<DataType>::~RPG_Stream_SessionConfigBase()
{
  ACE_TRACE(ACE_TEXT("RPG_Stream_SessionConfigBase::~RPG_Stream_SessionConfigBase"));

}

template <typename DataType>
const DataType
RPG_Stream_SessionConfigBase<DataType>::getUserData() const
{
  ACE_TRACE(ACE_TEXT("RPG_Stream_SessionConfigBase::getUserData"));

  return myUserData;
}

template <typename DataType>
const ACE_Time_Value
RPG_Stream_SessionConfigBase<DataType>::getStartOfSession() const
{
  ACE_TRACE(ACE_TEXT("RPG_Stream_SessionConfigBase::getStartOfSession"));

  return myStartOfSession;
}

template <typename DataType>
const bool
RPG_Stream_SessionConfigBase<DataType>::getUserAbort() const
{
  ACE_TRACE(ACE_TEXT("RPG_Stream_SessionConfigBase::getUserAbort"));

  return myUserAbort;
}

template <typename DataType>
void
RPG_Stream_SessionConfigBase<DataType>::decrease()
{
  ACE_TRACE(ACE_TEXT("RPG_Stream_SessionConfigBase::decrease"));

  bool destroy = false;

  {
    ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard(inherited::myLock);

    inherited::decrease();
    destroy = (refcount() == 0);
  } // end lock scope

  if (destroy)
  {
    // self-destruct
    delete this;
  } // end IF
}

template <typename DataType>
void
RPG_Stream_SessionConfigBase<DataType>::dump_state() const
{
  ACE_TRACE(ACE_TEXT("RPG_Stream_SessionConfigBase::dump_state"));

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("start of session: %s%s\n"),
             RPG_Stream_Tools::timestamp2LocalString(myStartOfSession).c_str(),
             (myUserAbort ? ACE_TEXT(", [user abort !]")
                          : ACE_TEXT(""))));
  inherited::dump_state();
}
