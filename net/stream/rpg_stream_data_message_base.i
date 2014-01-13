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

#include <ace/Malloc_Base.h>
#include <ace/Log_Msg.h>

#include "rpg_common_macros.h"

#include "rpg_stream_defines.h"

template <typename DataType,
          typename CommandType>
RPG_Stream_DataMessageBase<DataType,
                           CommandType>::RPG_Stream_DataMessageBase(DataType*& data_inout)
 : inherited(0,                                  // size
             RPG_Stream_MessageBase::MB_STREAM_OBJ,  // type
             NULL,                               // continuation
             NULL,                               // data
             NULL,                               // buffer allocator
             NULL,                               // locking strategy
             ACE_DEFAULT_MESSAGE_BLOCK_PRIORITY, // priority
             ACE_Time_Value::zero,               // execution time
             ACE_Time_Value::max_time,           // deadline time
             NULL,                               // data block allocator
             NULL),                              // message block allocator
   myData(data_inout),
   myIsInitialized(true)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_DataMessageBase::RPG_Stream_DataMessageBase"));

  // clean up
  data_inout = NULL;
}

template <typename DataType,
          typename CommandType>
RPG_Stream_DataMessageBase<DataType,
                           CommandType>::RPG_Stream_DataMessageBase(const RPG_Stream_DataMessageBase<DataType,
                                                                                                     CommandType>& message_in)
 : inherited(message_in),
   myData(NULL),
   myIsInitialized(false)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_DataMessageBase::RPG_Stream_DataMessageBase"));

  // maintain the same message type
  msg_type(message_in.msg_type());

  // ... and read/write pointers
  rd_ptr(message_in.rd_ptr());
  wr_ptr(message_in.wr_ptr());
}

template <typename DataType,
          typename CommandType>
RPG_Stream_DataMessageBase<DataType,
                           CommandType>::RPG_Stream_DataMessageBase(ACE_Allocator* messageAllocator_in)
 : inherited(messageAllocator_in), // message block allocator
   myData(NULL),
   myIsInitialized(false)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_DataMessageBase::RPG_Stream_DataMessageBase"));

  // set correct message type
  msg_type(RPG_Stream_MessageBase::MB_STREAM_OBJ);

  // reset read/write pointers
  reset();
}

template <typename DataType,
          typename CommandType>
RPG_Stream_DataMessageBase<DataType,
                           CommandType>::RPG_Stream_DataMessageBase(ACE_Data_Block* dataBlock_in,
                                                                    ACE_Allocator* messageAllocator_in)
 : inherited(dataBlock_in,
             messageAllocator_in),
   myData(NULL),
   myIsInitialized(false)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_MessageBase::RPG_Stream_MessageBase"));

  // set correct message type
  msg_type(RPG_Stream_MessageBase::MB_STREAM_OBJ);

  // reset read/write pointers
  reset();
}

template <typename DataType,
          typename CommandType>
RPG_Stream_DataMessageBase<DataType,
                           CommandType>::~RPG_Stream_DataMessageBase()
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_DataMessageBase::~RPG_Stream_DataMessageBase"));

  // clean up
  if (myData)
  {
    // decrease reference counter...
    try
    {
      myData->decrease();
    }
    catch (...)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("caught exception in decrease(), continuing")));
    }
    myData = NULL;
  } // end IF

  myIsInitialized = false;
}

template <typename DataType,
          typename CommandType>
void
RPG_Stream_DataMessageBase<DataType,
                           CommandType>::init(DataType*& data_inout,
                                              ACE_Data_Block* dataBlock_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_DataMessageBase::init"));

  // sanity check(s)
  ACE_ASSERT(!myIsInitialized);
  ACE_ASSERT(data_inout);

  myData = data_inout;

  // clean up
  data_inout = NULL;

  // set our data block (if any)
  if (dataBlock_in)
  {
    inherited::init(dataBlock_in);

    // (re)set correct message type
    msg_type(RPG_Stream_MessageBase::MB_STREAM_OBJ);
  } // end IF

  myIsInitialized = true;
}

template <typename DataType,
          typename CommandType>
const DataType* const
RPG_Stream_DataMessageBase<DataType,
                           CommandType>::getData() const
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_DataMessageBase::getData"));

  // sanity check
  ACE_ASSERT(myIsInitialized);

  return myData;
}

template <typename DataType,
          typename CommandType>
void
RPG_Stream_DataMessageBase<DataType,
                           CommandType>::dump_state() const
{
  RPG_TRACE(ACE_TEXT("RPG_Stream_DataMessageBase::dump_state"));

  // dump our data...
  if (myData)
  {
    try
    {
      myData->dump_state();
    }
    catch (...)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("caught exception in dump_state(), continuing")));
    }
  } // end IF
//   //delegate to base
//   inherited::dump_state();
}

//template <typename DataType,
//          typename CommandType>
//ACE_Message_Block*
//RPG_Stream_DataMessageBase<DataType,
//                           CommandType>::duplicate(void) const
//{
//  RPG_TRACE(ACE_TEXT("RPG_Stream_DataMessageBase::duplicate"));

//  own_type* new_message = NULL;

//  // create a new <RPG_Stream_DataMessageBase> that contains unique copies of
//  // the message block fields, but a reference counted duplicate of
//  // the <ACE_Data_Block>.

//  // if there is no allocator, use the standard new and delete calls.
//  if (message_block_allocator_ == NULL)
//    ACE_NEW_RETURN(new_message,
//                   own_type(*this), // invoke copy ctor
//                   NULL);

//  ACE_NEW_MALLOC_RETURN(new_message,
//                        static_cast<own_type*>(message_block_allocator_->malloc(capacity())),
//                        own_type(*this), // invoke copy ctor
//                        NULL);

//  // increment the reference counts of all the continuation messages
//  if (cont_)
//  {
//    new_message->cont_ = cont_->duplicate();

//    // when things go wrong, release all resources and return
//    if (new_message->cont_ == 0)
//    {
//      new_message->release();
//      new_message = NULL;
//    } // end IF
//  } // end IF

//  // *NOTE*: if "this" is initialized, so is the "clone" (and vice-versa)...

//  return new_message;
//}
