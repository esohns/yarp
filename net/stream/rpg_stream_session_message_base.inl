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

#include "rpg_stream_message_base.h"

#include <rpg_common_macros.h>

#include <ace/Malloc_Base.h>
#include <ace/Log_Msg.h>

template <typename ConfigType>
RPG_Stream_SessionMessageBase<ConfigType>::RPG_Stream_SessionMessageBase(const unsigned long& sessionID_in,
                                                                 const RPG_Stream_SessionMessageType& messageType_in,
                                                                 ConfigType*& config_inout)
 : inherited(0,                                     // size
             RPG_Stream_MessageBase::MB_STREAM_SESSION, // type
             NULL,                                  // continuation
             NULL,                                  // data
             NULL,                                  // buffer allocator
             NULL,                                  // locking strategy
             ACE_DEFAULT_MESSAGE_BLOCK_PRIORITY,    // priority
             ACE_Time_Value::zero,                  // execution time
             ACE_Time_Value::max_time,              // deadline time
             NULL,                                  // data block allocator
             NULL),                                 // message block allocator
   myConfig(config_inout),
   myID(sessionID_in),
   myMessageType(messageType_in),
   myIsInitialized(true)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_SessionMessageBase::RPG_Stream_SessionMessageBase"));

  // set return value
  config_inout = NULL;
}

template <typename ConfigType>
RPG_Stream_SessionMessageBase<ConfigType>::RPG_Stream_SessionMessageBase(ACE_Allocator* messageAllocator_in)
 : inherited(messageAllocator_in), // message block allocator
   myConfig(NULL),
   myID(0),
   myMessageType(RPG_Stream_SessionMessage::MB_BEGIN_STREAM_SESSION_MAP), // == RPG_Stream_MessageBase::MB_STREAM_SESSION
   myIsInitialized(false)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_SessionMessageBase::RPG_Stream_SessionMessageBase"));

  // set correct message type
  // *WARNING*: this doesn't work, as we're assigned a (different) data block later...
  // --> do it in init()
//   msg_type(RPG_Stream_MessageBase::MB_STREAM_SESSION);

  // reset read/write pointers
  reset();
}

template <typename ConfigType>
RPG_Stream_SessionMessageBase<ConfigType>::RPG_Stream_SessionMessageBase(ACE_Data_Block* dataBlock_in,
                                                                 ACE_Allocator* messageAllocator_in)
 : inherited(dataBlock_in,         // use (don't own (!) memory of-) this data block
             0,                    // flags --> also "free" our data block upon destruction !
             messageAllocator_in), // re-use the same allocator
   myConfig(NULL),
   myID(0),
   myMessageType(RPG_Stream_SessionMessage::MB_BEGIN_STREAM_SESSION_MAP), // == RPG_Stream_MessageBase::MB_STREAM_SESSION
   myIsInitialized(false)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_MessageBase::RPG_Stream_MessageBase"));

  // set correct message type
  // *WARNING*: need to finalize initialization through init() !
  msg_type(RPG_Stream_MessageBase::MB_STREAM_SESSION);

  // reset read/write pointers
  reset();
}

template <typename ConfigType>
RPG_Stream_SessionMessageBase<ConfigType>::RPG_Stream_SessionMessageBase(const RPG_Stream_SessionMessageBase<ConfigType>& message_in)
 : inherited(message_in.data_block_->duplicate(),  // make a "shallow" copy of the data block
             0,                                    // "own" the duplicate
             message_in.message_block_allocator_), // message allocator
   myConfig(message_in.myConfig),
   myID(message_in.myID),
   myMessageType(message_in.myMessageType),
   myIsInitialized(message_in.myIsInitialized)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_SessionMessageBase::RPG_Stream_SessionMessageBase"));

  // increment reference counter
  if (myConfig)
  {
    myConfig->increase();
  } // end IF

  // set read/write pointers
  rd_ptr(message_in.rd_ptr());
  wr_ptr(message_in.wr_ptr());
}

template <typename ConfigType>
RPG_Stream_SessionMessageBase<ConfigType>::~RPG_Stream_SessionMessageBase()
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_SessionMessageBase::~RPG_Stream_SessionMessageBase"));

  myID = 0;
  myMessageType = RPG_Stream_SessionMessage::MB_BEGIN_STREAM_SESSION_MAP; // == RPG_Stream_MessageBase::MB_STREAM_SESSION
  // clean up
  if (myConfig)
  {
    // decrease reference counter...
    myConfig->decrease();
    myConfig = NULL;
  } // end IF

  myIsInitialized = false;
}

template <typename ConfigType>
const unsigned long
RPG_Stream_SessionMessageBase<ConfigType>::getID() const
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_SessionMessageBase::getID"));

  return myID;
}

template <typename ConfigType>
const RPG_Stream_SessionMessageType
RPG_Stream_SessionMessageBase<ConfigType>::getType() const
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_SessionMessageBase::getType"));

  return myMessageType;
}

template <typename ConfigType>
const ConfigType* const
RPG_Stream_SessionMessageBase<ConfigType>::getConfig() const
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_SessionMessageBase::getConfig"));

  return myConfig;
}

template <typename ConfigType>
void
RPG_Stream_SessionMessageBase<ConfigType>::dump_state() const
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_SessionMessageBase::dump_state"));

  std::string type_string;
  RPG_Stream_SessionMessage::SessionMessageType2String(myMessageType,
                                                       type_string);

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("session (ID: %u) message type: \"%s\"\n"),
             myID,
             type_string.c_str()));

  if (myConfig)
  {
    try
    {
      myConfig->dump_state();
    }
    catch (...)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("caught exception in RPG_Stream_SessionConfigBase::dump_state(), continuing")));
    }
  } // end IF
}

template <typename ConfigType>
ACE_Message_Block*
RPG_Stream_SessionMessageBase<ConfigType>::duplicate(void) const
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_SessionMessageBase::duplicate"));

  RPG_Stream_SessionMessageBase<ConfigType>* nb = NULL;

  // create a new <RPG_Stream_SessionMessageBase> that contains unique copies of
  // the message block fields, but a reference counted duplicate of
  // the <ACE_Data_Block>.

  // if there is no allocator, use the standard new and delete calls.
  if (message_block_allocator_ == NULL)
  {
    ACE_NEW_RETURN(nb,
                   RPG_Stream_SessionMessageBase<ConfigType>(*this),
                   NULL);
  } // end IF

  // *WARNING*:we tell the allocator to return a RPG_Stream_SessionMessageBase<ConfigType>
  // by passing a 0 as argument to malloc()...
  ACE_NEW_MALLOC_RETURN(nb,
                        static_cast<RPG_Stream_SessionMessageBase<ConfigType>*>(message_block_allocator_->malloc(0)),
                        RPG_Stream_SessionMessageBase<ConfigType>(*this),
                        NULL);

  // increment the reference counts of all the continuation messages
  if (cont_)
  {
    nb->cont_ = cont_->duplicate();

    // If things go wrong, release all of our resources and return
    if (nb->cont_ == 0)
    {
      nb->release();
      nb = NULL;
    } // end IF
  } // end IF

  // *NOTE*: if "this" is initialized, so is the "clone" (and vice-versa)...

  return nb;
}

template <typename ConfigType>
void
RPG_Stream_SessionMessageBase<ConfigType>::init(const unsigned long& sessionID_in,
                                            const RPG_Stream_SessionMessageType& messageType_in,
                                            ConfigType*& config_inout)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_SessionMessageBase::init"));

  // sanity checks
  ACE_ASSERT(!myIsInitialized);
  ACE_ASSERT(myID == 0);
  ACE_ASSERT(myMessageType == RPG_Stream_SessionMessage::MB_BEGIN_STREAM_SESSION_MAP), // == RPG_Stream_MessageBase::MB_STREAM_SESSION
  ACE_ASSERT(myConfig == NULL);

  myID = sessionID_in;
  myMessageType = messageType_in;
  myConfig = config_inout;

  // bye bye... we take on the responsibility for config_inout
  config_inout = NULL;

  // OK !
  myIsInitialized = true;
}
