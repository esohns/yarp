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

#include <ace/Proactor.h>
#include <ace/Message_Block.h>
#include <ace/INET_Addr.h>

#include "rpg_common_macros.h"
#include "rpg_common_defines.h"

#include "rpg_stream_iallocator.h"

#include "rpg_net_defines.h"
#include "rpg_net_common_tools.h"

template <typename ConfigType,
          typename StatisticsContainerType>
RPG_Net_AsynchSocketHandler_T<ConfigType,
                              StatisticsContainerType>::RPG_Net_AsynchSocketHandler_T(MANAGER_T* manager_in)
 : inherited(),
   ACE_Notification_Strategy(NULL,                           // event handler handle
                             ACE_Event_Handler::WRITE_MASK), // mask
   inherited2(0,     // initial count
	            true), // delete on zero ?
//    myUserData(),
   myInputStream(),
   myOutputStream(),
   myManager(manager_in),
   myIsRegistered(false),
   myLocalSAP(),
   myRemoteSAP()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_AsynchSocketHandler_T::RPG_Net_AsynchSocketHandler_T"));

  // init user data
  ACE_OS::memset(&myUserData, 0, sizeof(ConfigType));

  if (myManager)
  { // (try to) get user data from the connection manager...
    try
    {
      myManager->getConfiguration(myUserData);
    }
    catch (...)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("caught exception in RPG_Net_IConnectionManager::getConfiguration(), continuing\n")));
    }
  } // end IF
}

template <typename ConfigType,
          typename StatisticsContainerType>
RPG_Net_AsynchSocketHandler_T<ConfigType,
                              StatisticsContainerType>::~RPG_Net_AsynchSocketHandler_T()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_AsynchSocketHandler_T::~RPG_Net_AsynchSocketHandler_T"));

}

template <typename ConfigType,
          typename StatisticsContainerType>
void
RPG_Net_AsynchSocketHandler_T<ConfigType,
                              StatisticsContainerType>::open(ACE_HANDLE handle_in,
                                                             ACE_Message_Block& messageBlock_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_AsynchSocketHandler_T::open"));

  // step1: tweak socket
  // *TODO*: there is a design glitch here: this class SHOULD NOT make
  // assumptions about ConfigType !
  if (myUserData.socketBufferSize)
    if (!RPG_Net_Common_Tools::setSocketBuffer(handle_in,
                                               SO_RCVBUF,
                                               myUserData.socketBufferSize))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to setSocketBuffer(%u) for %u, aborting\n"),
                 myUserData.socketBufferSize,
                 handle_in));

      // clean up
      handle_close(inherited::handle(),
                   ACE_Event_Handler::ALL_EVENTS_MASK);

      return;
    } // end IF
  if (!RPG_Net_Common_Tools::setNoDelay(handle_in,
                                        RPG_NET_DEFAULT_TCP_NODELAY))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to setNoDelay(%u, %s), aborting\n"),
               handle_in,
               (RPG_NET_DEFAULT_TCP_NODELAY ? ACE_TEXT("true")
                                            : ACE_TEXT("false"))));

    // clean up
    handle_close(inherited::handle(),
                 ACE_Event_Handler::ALL_EVENTS_MASK);

    return;
  } // end IF
  if (!RPG_Net_Common_Tools::setKeepAlive(handle_in,
                                          RPG_NET_DEFAULT_TCP_KEEPALIVE))
  {
    int error = ACE_OS::last_error();
    if (error != ENOTSOCK) // <-- socket has been closed asynchronously
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to setLinger(%u, %s), aborting\n"),
                 id(),
                 (RPG_NET_DEFAULT_TCP_LINGER ? ACE_TEXT("true")
                                             : ACE_TEXT("false"))));

    // clean up
    handle_close(inherited::handle(),
                 ACE_Event_Handler::ALL_EVENTS_MASK);

    return;
  } // end IF
  if (!RPG_Net_Common_Tools::setLinger(handle_in,
                                       RPG_NET_DEFAULT_TCP_LINGER))
  {
    int error = ACE_OS::last_error();
    if (error != ENOTSOCK) // <-- socket has been closed asynchronously
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to setLinger(%u, %s), aborting\n"),
                 id(),
                 ((RPG_NET_DEFAULT_TCP_LINGER > 0) ? ACE_TEXT("true")
                                                   : ACE_TEXT("false"))));

    // clean up
    handle_close(inherited::handle(),
                 ACE_Event_Handler::ALL_EVENTS_MASK);

    return;
  } // end IF

  // step2: init i/o streams
  inherited::proactor(ACE_Proactor::instance());
  if (myInputStream.open(*this,
                       	 handle_in,
                         NULL,
                         inherited::proactor()) == -1)
  {
    ACE_ERROR((LM_ERROR,
               ACE_TEXT("failed to init input stream (handle: %u), aborting\n"),
               handle_in));

    // clean up
    handle_close(inherited::handle(),
                 ACE_Event_Handler::ALL_EVENTS_MASK);

    return;
  } // end IF
  if (myOutputStream.open(*this,
                          handle_in,
                          NULL,
                          inherited::proactor()) == -1)
  {
    ACE_ERROR((LM_ERROR,
               ACE_TEXT("failed to init output stream (handle: %u), aborting\n"),
               handle_in));

    // clean up
    handle_close(inherited::handle(),
                 ACE_Event_Handler::ALL_EVENTS_MASK);

    return;
  } // end IF

//  if (myManager)
//  { // (try to) register with the connection manager...
//    try
//    {
//      myManager->registerConnection(this);
//    }
//    catch (...)
//    {
//      ACE_DEBUG((LM_ERROR,
//                 ACE_TEXT("caught exception in RPG_Net_IConnectionManager::registerConnection(), continuing\n")));
//    }
//  } // end IF
}

template <typename ConfigType,
          typename StatisticsContainerType>
void
RPG_Net_AsynchSocketHandler_T<ConfigType,
                              StatisticsContainerType>::addresses(const ACE_INET_Addr& remoteAddress_in,
                                                                  const ACE_INET_Addr& localAddress_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_AsynchSocketHandler_T::addresses"));

  myLocalSAP = localAddress_in;
  myRemoteSAP = remoteAddress_in;
}

template <typename ConfigType,
          typename StatisticsContainerType>
int
RPG_Net_AsynchSocketHandler_T<ConfigType,
                              StatisticsContainerType>::handle_close(ACE_HANDLE handle_in,
                                                                     ACE_Reactor_Mask mask_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_AsynchSocketHandler_T::handle_close"));

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
                 ACE_TEXT("failed to ACE_OS::closesocket(): \"%m\", continuing\n")));
  } // end IF

  return result;
}

template <typename ConfigType,
          typename StatisticsContainerType>
int
RPG_Net_AsynchSocketHandler_T<ConfigType,
                              StatisticsContainerType>::notify(void)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_AsynchSocketHandler_T::notify"));

  int result = -1;
  try
  {
    result = handle_output(inherited::handle());
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in RPG_Net_AsynchSocketHandler_T::handle_output(), aborting")));
  }
  if (result == -1)
    handle_close(inherited::handle(),
                 ACE_Event_Handler::ALL_EVENTS_MASK);

  return result;
}
template <typename ConfigType,
          typename StatisticsContainerType>
int
RPG_Net_AsynchSocketHandler_T<ConfigType,
                              StatisticsContainerType>::notify(ACE_Event_Handler* handler_in,
                                                               ACE_Reactor_Mask mask_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_AsynchSocketHandler_T::notify"));

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

template <typename ConfigType,
          typename StatisticsContainerType>
ACE_Message_Block*
RPG_Net_AsynchSocketHandler_T<ConfigType,
                              StatisticsContainerType>::allocateMessage(const unsigned int& requestedSize_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_AsynchSocketHandler_T::allocateMessage"));

  // init return value(s)
  ACE_Message_Block* message_out = NULL;

  // sanity check(s)
  ACE_ASSERT(myUserData.messageAllocator);

  try
  {
    message_out =
        static_cast<ACE_Message_Block*>(myUserData.messageAllocator->malloc(requestedSize_in));
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in Stream_IAllocator::malloc(%u), aborting\n"),
               requestedSize_in));
  }
  if (!message_out)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to Stream_IAllocator::malloc(%u), aborting\n"),
               requestedSize_in));
  } // end IF

  return message_out;
}

template <typename ConfigType,
          typename StatisticsContainerType>
void
RPG_Net_AsynchSocketHandler_T<ConfigType,
                              StatisticsContainerType>::handle_write_stream(const ACE_Asynch_Write_Stream::Result& result)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_AsynchSocketHandler_T::handle_write_stream"));

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
    handle_close(inherited::handle(),
                 ACE_Event_Handler::ALL_EVENTS_MASK);
}

template <typename ConfigType,
          typename StatisticsContainerType>
void
RPG_Net_AsynchSocketHandler_T<ConfigType,
                              StatisticsContainerType>::initiate_read_stream()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_AsynchSocketHandler_T::initiate_read_stream"));

  // allocate a data buffer
  ACE_Message_Block* message_block = allocateMessage(RPG_NET_STREAM_BUFFER_SIZE);
  if (!message_block)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Net_AsynchSocketHandler_T::allocateMessage(%u), aborting\n"),
               RPG_NET_STREAM_BUFFER_SIZE));

    // clean up
    handle_close(inherited::handle(),
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
    handle_close(inherited::handle(),
                 ACE_Event_Handler::ALL_EVENTS_MASK);
  } // end IF
}

template <typename ConfigType,
          typename StatisticsContainerType>
void
RPG_Net_AsynchSocketHandler_T<ConfigType,
                              StatisticsContainerType>::info(ACE_HANDLE& handle_out,
                                                             ACE_INET_Addr& localSAP_out,
                                                             ACE_INET_Addr& remoteSAP_out) const
{
  RPG_TRACE(ACE_TEXT("RPG_Net_AsynchSocketHandler_T::info"));

  handle_out = handle();
  localSAP_out = myLocalSAP;
  remoteSAP_out = myRemoteSAP;
}

template <typename ConfigType,
          typename StatisticsContainerType>
void
RPG_Net_AsynchSocketHandler_T<ConfigType,
                              StatisticsContainerType>::abort()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_AsynchSocketHandler_T::abort"));

  handle_close(inherited::handle(),
               ACE_Event_Handler::ALL_EVENTS_MASK);
}

template <typename ConfigType,
          typename StatisticsContainerType>
unsigned int
RPG_Net_AsynchSocketHandler_T<ConfigType,
                              StatisticsContainerType>::id() const
{
  RPG_TRACE(ACE_TEXT("RPG_Net_AsynchSocketHandler_T::id"));

  // *PORTABILITY*: this isn't entirely portable...
#if !defined(ACE_WIN32) && !defined(ACE_WIN64)
  return static_cast<unsigned int>(handle());
#else
  return reinterpret_cast<unsigned int>(handle());
#endif
}

template <typename ConfigType,
          typename StatisticsContainerType>
void
RPG_Net_AsynchSocketHandler_T<ConfigType,
                              StatisticsContainerType>::dump_state() const
{
  RPG_TRACE(ACE_TEXT("RPG_Net_AsynchSocketHandler_T::dump_state"));

  ACE_TCHAR buffer[RPG_COMMON_BUFSIZE];
  ACE_OS::memset(buffer, 0, sizeof(buffer));
  std::string localAddress;
  ACE_INET_Addr address;
  if (myLocalSAP.addr_to_string(buffer, sizeof(buffer)) == -1)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_INET_Addr::addr_to_string(): \"%m\", continuing\n")));
  localAddress = buffer;
  ACE_OS::memset(buffer, 0, sizeof(buffer));
  if (myRemoteSAP.addr_to_string(buffer, sizeof(buffer)) == -1)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_INET_Addr::addr_to_string(): \"%m\", continuing\n")));

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("connection [%u]: (\"%s\") <--> (\"%s\")\n"),
             id(),
             localAddress.c_str(),
             buffer));
}
