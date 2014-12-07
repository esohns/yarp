/***************************************************************************
 *   Copyright (C) 2010 by Erik Sohns   *
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

#include "rpg_net_asynch_tcpsockethandler.h"

#include "ace/OS_Memory.h"
#include "ace/Proactor.h"
#include "ace/Message_Block.h"
#include "ace/INET_Addr.h"
#include "ace/Time_Value.h"

#include "rpg_common_macros.h"
#include "rpg_common_defines.h"

#include "rpg_net_defines.h"
#include "rpg_net_common_tools.h"

RPG_Net_AsynchTCPSocketHandler::RPG_Net_AsynchTCPSocketHandler()
 : inherited()
 //, inherited2(0,    // initial count
 //             true) // delete on zero ?
 , inherited2 ()
 , inherited3 (NULL,                          // event handler handle
               ACE_Event_Handler::WRITE_MASK) // mask
// , myInputStream()
// , myOutputStream()
// , myLocalSAP()
// , myRemoteSAP()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_AsynchTCPSocketHandler::RPG_Net_AsynchTCPSocketHandler"));

}

RPG_Net_AsynchTCPSocketHandler::~RPG_Net_AsynchTCPSocketHandler()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_AsynchTCPSocketHandler::~RPG_Net_AsynchTCPSocketHandler"));

}

void
RPG_Net_AsynchTCPSocketHandler::open(ACE_HANDLE handle_in,
                                     ACE_Message_Block& messageBlock_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_AsynchTCPSocketHandler::open"));

  // step1: tweak socket
  // *TODO*: there is a design glitch here: this class SHOULD NOT make
  // assumptions about ConfigType !
  //if (myUserData.socketBufferSize)
  //  if (!RPG_Net_Common_Tools::setSocketBuffer(handle_in,
  //                                             SO_RCVBUF,
  //                                             myUserData.socketBufferSize))
  //  {
  //    ACE_DEBUG((LM_ERROR,
  //               ACE_TEXT("failed to RPG_Net_Common_Tools::setSocketBuffer(%u) (handle was: %d), aborting\n"),
  //               myUserData.socketBufferSize,
  //               handle_in));

  //    // clean up
  //    handle_close(handle_in,
  //                 ACE_Event_Handler::ALL_EVENTS_MASK);

  //    return;
  //  } // end IF
  if (!RPG_Net_Common_Tools::setNoDelay (handle_in,
                                         RPG_NET_DEFAULT_TCP_NODELAY))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Net_Common_Tools::setNoDelay(%s) (handle was: %d), aborting\n"),
               (RPG_NET_DEFAULT_TCP_NODELAY ? ACE_TEXT("true")
                                            : ACE_TEXT("false")),
               handle_in));

    // clean up
    handle_close (handle_in,
                  ACE_Event_Handler::ALL_EVENTS_MASK);

    return;
  } // end IF
  if (!RPG_Net_Common_Tools::setKeepAlive (handle_in,
                                           RPG_NET_DEFAULT_TCP_KEEPALIVE))
  {
    int error = ACE_OS::last_error();
    if (error != ENOTSOCK) // <-- socket has been closed asynchronously
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to RPG_Net_Common_Tools::setLinger(%s) (handle was: %d), aborting\n"),
                 (RPG_NET_DEFAULT_TCP_LINGER ? ACE_TEXT("true")
                                             : ACE_TEXT("false")),
                 handle_in));

    // clean up
    handle_close (handle_in,
                  ACE_Event_Handler::ALL_EVENTS_MASK);

    return;
  } // end IF
  if (!RPG_Net_Common_Tools::setLinger (handle_in,
                                        RPG_NET_DEFAULT_TCP_LINGER))
  {
    int error = ACE_OS::last_error();
    if (error != ENOTSOCK) // <-- socket has been closed asynchronously
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to RPG_Net_Common_Tools::setLinger(%s) (handle was: %d), aborting\n"),
                 ((RPG_NET_DEFAULT_TCP_LINGER > 0) ? ACE_TEXT("true")
                                                   : ACE_TEXT("false")),
                 handle_in));

    // clean up
    handle_close (handle_in,
                  ACE_Event_Handler::ALL_EVENTS_MASK);

    return;
  } // end IF

  // step2: init i/o streams
  inherited2::proactor(ACE_Proactor::instance());
  if (myInputStream.open(*this,
                         handle_in,
                         NULL,
                         inherited2::proactor()) == -1)
  {
    ACE_ERROR((LM_ERROR,
               ACE_TEXT("failed to init input stream (handle was: %d), aborting\n"),
               handle_in));

    // clean up
    handle_close (handle_in,
                  ACE_Event_Handler::ALL_EVENTS_MASK);

    return;
  } // end IF
  if (myOutputStream.open(*this,
                          handle_in,
                          NULL,
                          inherited2::proactor()) == -1)
  {
    ACE_ERROR((LM_ERROR,
               ACE_TEXT("failed to init output stream (handle was: %d), aborting\n"),
               handle_in));

    // clean up
    handle_close (handle_in,
                  ACE_Event_Handler::ALL_EVENTS_MASK);

    return;
  } // end IF
}

void
RPG_Net_AsynchTCPSocketHandler::addresses(const ACE_INET_Addr& remoteAddress_in,
                                          const ACE_INET_Addr& localAddress_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_AsynchTCPSocketHandler::addresses"));

  myLocalSAP = localAddress_in;
  myRemoteSAP = remoteAddress_in;
}

int
RPG_Net_AsynchTCPSocketHandler::handle_close(ACE_HANDLE handle_in,
                                             ACE_Reactor_Mask mask_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_AsynchTCPSocketHandler::handle_close"));

  ACE_UNUSED_ARG(mask_in);

  int result = -1;

  // clean up
  myInputStream.cancel();
  myOutputStream.cancel();

  if (handle_in != ACE_INVALID_HANDLE)
  {
    result = ACE_OS::closesocket(handle_in);
    if (result == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_OS::closesocket(%d): \"%m\", continuing\n"),
                 handle_in));
  } // end IF

  return result;
}

int
RPG_Net_AsynchTCPSocketHandler::notify(void)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_AsynchTCPSocketHandler::notify"));

  int result = -1;
  try
  {
    result = handle_output(inherited2::handle());
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in RPG_Net_AsynchTCPSocketHandler::handle_output(), aborting")));
  }
  if (result == -1)
    handle_close(inherited2::handle(),
                 ACE_Event_Handler::ALL_EVENTS_MASK);

  return result;
}
int
RPG_Net_AsynchTCPSocketHandler::notify(ACE_Event_Handler* handler_in,
                                       ACE_Reactor_Mask mask_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_AsynchTCPSocketHandler::notify"));

  ACE_UNUSED_ARG(handler_in);
  ACE_UNUSED_ARG(mask_in);

  // *NOTE*: should NEVER be reached !
  ACE_ASSERT(false);

#if defined (_MSC_VER)
  return -1;
#else
  ACE_NOTREACHED(return -1;)
#endif
}

ACE_Message_Block*
RPG_Net_AsynchTCPSocketHandler::allocateMessage (unsigned int requestedSize_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_AsynchTCPSocketHandler::allocateMessage"));

  // init return value(s)
  ACE_Message_Block* message_out = NULL;

  if (inherited::allocator_)
    message_out = static_cast<ACE_Message_Block*>(inherited::allocator_->malloc (requestedSize_in));
  else
    ACE_NEW_NORETURN (message_out,
                      ACE_Message_Block (requestedSize_in,
                                         ACE_Message_Block::MB_DATA,
                                         NULL,
                                         NULL,
                                         NULL,
                                         NULL,
                                         ACE_DEFAULT_MESSAGE_BLOCK_PRIORITY,
                                         ACE_Time_Value::zero,
                                         ACE_Time_Value::max_time,
                                         NULL,
                                         NULL));
  if (!message_out)
    ACE_DEBUG((LM_CRITICAL,
               ACE_TEXT("failed to allocate memory: \"%m\", aborting\n")));

  return message_out;
}

void
RPG_Net_AsynchTCPSocketHandler::handle_write_stream(const ACE_Asynch_Write_Stream::Result& result)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_AsynchTCPSocketHandler::handle_write_stream"));

  bool close = false;

  // sanity check
  if (result.success() == 0)
  {
    // connection reset (by peer) ? --> not an error
    if ((result.error() != ECONNRESET) &&
        (result.error() != EPIPE))
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to write to output stream (%d): \"%s\", aborting\n"),
                 result.handle(),
                 ACE_TEXT(ACE_OS::strerror(result.error()))));

    close = true;
  } // end IF

  switch (result.bytes_transferred())
  {
    case -1:
    case 0:
    {
      // connection reset (by peer) ? --> not an error
      if ((result.error() != ECONNRESET) &&
          (result.error() != EPIPE))
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to write to output stream (%d): \"%s\", aborting\n"),
                   result.handle(),
                   ACE_TEXT(ACE_OS::strerror(result.error()))));

      close = true;

      break;
    }
    // *** GOOD CASES ***
    default:
    {
      // short write ?
      if (result.bytes_to_write() != result.bytes_transferred())
      {
        // *TODO*: handle short writes more gracefully
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("stream (%d): sent %u/%u byte(s) only, aborting"),
                   result.handle(),
                   result.bytes_transferred(),
                   result.bytes_to_write()));

        close = true;
      } // end IF

      break;
    }
  } // end SWITCH

  // clean up
  result.message_block().release();
  if (close)
    handle_close(inherited2::handle(),
                 ACE_Event_Handler::ALL_EVENTS_MASK);
}

void
RPG_Net_AsynchTCPSocketHandler::initiate_read_stream()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_AsynchTCPSocketHandler::initiate_read_stream"));

  // allocate a data buffer
  ACE_Message_Block* message_block = allocateMessage(RPG_NET_STREAM_BUFFER_SIZE);
  if (!message_block)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Net_AsynchTCPSocketHandler::allocateMessage(%u), aborting\n"),
               RPG_NET_STREAM_BUFFER_SIZE));

    // clean up
    handle_close(inherited2::handle(),
                 ACE_Event_Handler::ALL_EVENTS_MASK);
  } // end IF

  // start (asynch) read...
  if (myInputStream.read(*message_block,
                         message_block->size()) == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Asynch_Read_Stream::read(%u): \"%m\", aborting\n"),
               message_block->size()));

    // clean up
    message_block->release();
    handle_close(inherited2::handle(),
                 ACE_Event_Handler::ALL_EVENTS_MASK);
  } // end IF
}

//void
//RPG_Net_AsynchTCPSocketHandler::info(ACE_HANDLE& handle_out,
//                                     ACE_INET_Addr& localSAP_out,
//                                     ACE_INET_Addr& remoteSAP_out) const
//{
//  RPG_TRACE(ACE_TEXT("RPG_Net_AsynchTCPSocketHandler::info"));

//  handle_out = inherited::handle();
//  localSAP_out = myLocalSAP;
//  remoteSAP_out = myRemoteSAP;
//}

//void
//RPG_Net_AsynchTCPSocketHandler::abort()
//{
//  RPG_TRACE(ACE_TEXT("RPG_Net_AsynchTCPSocketHandler::abort"));

//  handle_close(inherited::handle(),
//               ACE_Event_Handler::ALL_EVENTS_MASK);
//}

//unsigned int
//RPG_Net_AsynchTCPSocketHandler::id() const
//{
//  RPG_TRACE(ACE_TEXT("RPG_Net_AsynchTCPSocketHandler::id"));

//  // *PORTABILITY*: this isn't entirely portable...
//#if !defined(ACE_WIN32) && !defined(ACE_WIN64)
//  return static_cast<unsigned int>(handle());
//#else
//  return reinterpret_cast<unsigned int>(handle());
//#endif
//}

//void
//RPG_Net_AsynchTCPSocketHandler::dump_state() const
//{
//  RPG_TRACE(ACE_TEXT("RPG_Net_AsynchTCPSocketHandler::dump_state"));

//  ACE_TCHAR buffer[RPG_COMMON_BUFSIZE];
//  ACE_OS::memset(buffer, 0, sizeof(buffer));
//  std::string localAddress;
//  ACE_INET_Addr address;
//  if (myLocalSAP.addr_to_string(buffer, sizeof(buffer)) == -1)
//    ACE_DEBUG((LM_ERROR,
//               ACE_TEXT("failed to ACE_INET_Addr::addr_to_string(): \"%m\", continuing\n")));
//  localAddress = buffer;
//  ACE_OS::memset(buffer, 0, sizeof(buffer));
//  if (myRemoteSAP.addr_to_string(buffer, sizeof(buffer)) == -1)
//    ACE_DEBUG((LM_ERROR,
//               ACE_TEXT("failed to ACE_INET_Addr::addr_to_string(): \"%m\", continuing\n")));

//  ACE_DEBUG((LM_DEBUG,
//             ACE_TEXT("connection [%u]: (\"%s\") <--> (\"%s\")\n"),
//             id(),
//             localAddress.c_str(),
//             buffer));
//}
