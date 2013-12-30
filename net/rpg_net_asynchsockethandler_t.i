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
//    myUserData(),
   myInputStream(),
   myOutputStream(),
   myManager(manager_in),
   myIsRegistered(false),
   myLocalSAP(),
   myRemoteSAP()//,
   //myID(0)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_AsynchSocketHandler_T::RPG_Net_AsynchSocketHandler_T"));

  // init user data
  ACE_OS::memset(&myUserData, 0, sizeof(ConfigType));

  if (myManager)
  { // (try to) get user data from the connection manager...
    try
    {
      myManager->getConfig(myUserData);
    }
    catch (...)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("caught exception in RPG_Net_IConnectionManager::getConfig(), continuing\n")));
    }
  } // end IF
}

template <typename ConfigType,
          typename StatisticsContainerType>
RPG_Net_AsynchSocketHandler_T<ConfigType,
                              StatisticsContainerType>::~RPG_Net_AsynchSocketHandler_T()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_AsynchSocketHandler_T::~RPG_Net_AsynchSocketHandler_T"));

  if (myManager && myIsRegistered)
  { // (try to) de-register with connection manager
    try
    {
      myManager->deregisterConnection(this);
    }
    catch (...)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("caught exception in RPG_Net_IConnectionManager::deregisterConnection(), continuing\n")));
    }
  } // end IF
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
  {
    if (!RPG_Net_Common_Tools::setSocketBuffer(handle_in,
                                               SO_RCVBUF,
                                               myUserData.socketBufferSize))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to setSocketBuffer(%u) for %u, aborting\n"),
                 myUserData.socketBufferSize,
                 handle_in));

      // clean up
      delete this;

      return;
    } // end IF
  } // end IF
  if (!RPG_Net_Common_Tools::setNoDelay(handle_in,
                                        RPG_NET_DEF_SOCK_NODELAY))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to setNoDelay(%u, %s), aborting\n"),
               handle_in,
               (RPG_NET_DEF_SOCK_NODELAY ? ACE_TEXT("true") : ACE_TEXT("false"))));

    // clean up
    delete this;

    return;
  } // end IF
  if (!RPG_Net_Common_Tools::setKeepAlive(handle_in,
                                          RPG_NET_DEF_SOCK_KEEPALIVE))
  {
    int error = ACE_OS::last_error();
    if (error != ENOTSOCK) // <-- socket has been closed asynchronously
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to setLinger(%u, %s), aborting\n"),
                 getID(),
                 (RPG_NET_DEF_SOCK_LINGER ? ACE_TEXT("true") : ACE_TEXT("false"))));

    // clean up
    delete this;

    return;
  } // end IF
  if (!RPG_Net_Common_Tools::setLinger(handle_in,
                                       RPG_NET_DEF_SOCK_LINGER))
  {
    int error = ACE_OS::last_error();
    if (error != ENOTSOCK) // <-- socket has been closed asynchronously
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to setLinger(%u, %s), aborting\n"),
                 getID(),
                 ((RPG_NET_DEF_SOCK_LINGER > 0) ? ACE_TEXT("true") : ACE_TEXT("false"))));

    // clean up
    delete this;

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
    delete this;

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
    delete this;

    return;
  } // end IF

  // step3: start reading (need to pass any data ?)
  if (messageBlock_in.length() == 0)
    initiate_read_stream();
  else
  {
    ACE_Message_Block* duplicate = messageBlock_in.duplicate();
    if (!duplicate)
    {
      ACE_ERROR((LM_ERROR,
                 ACE_TEXT("failed to ACE_Message_Block::duplicate(): \"%m\", aborting\n")));

      // clean up
      delete this;

      return;
    } // end IF
    // fake a result to emulate regular behavior...
    ACE_Asynch_Read_Stream_Result_Impl* fake_result = proactor()->create_asynch_read_stream_result(proxy(),            // handler proxy
                                                                                                   handle_in,          // socket handle
                                                                                                   *duplicate,         // buffer
                                                                                                   duplicate->size(),  // (max) bytes to read
                                                                                                   NULL,               // ACT
                                                                                                   ACE_INVALID_HANDLE, // event
                                                                                                   0,                  // priority
                                                                                                   0);                 // signal number
    if (!fake_result)
    {
      ACE_ERROR((LM_ERROR,
                 ACE_TEXT("failed to ACE_Proactor::create_asynch_read_stream_result: \"%m\", aborting\n")));

      // clean up
      delete this;

      return;
    } // end IF
    size_t bytes_transferred = duplicate->length();
    // <complete> for Accept would have already moved the <wr_ptr>
    // forward; update it to the beginning position
    duplicate->wr_ptr(duplicate->wr_ptr() - bytes_transferred);
    // invoke ourselves (see handle_read_stream)
    fake_result->complete(duplicate->length(), // bytes read
                          1,                   // success
                          NULL,                // ACT
                          0);                  // error

    // clean up
    delete fake_result;
  }

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

/*template <typename ConfigType,
          typename StatisticsContainerType>
int
RPG_Net_AsynchSocketHandler_T<ConfigType,
                              StatisticsContainerType>::close(u_long arg_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_AsynchSocketHandler_T::close"));

  // *NOTE*: this method will be invoked
  // - by the worker which calls this after returning from svc()
  //    --> in this case, this should be a NOP (triggered from handle_close(),
  //        which was invoked by the reactor) - we override the default
  //        behavior of a ACE_Svc_Handler, which would call handle_close() AGAIN
  // - by the connector/acceptor when open() fails (e.g. too many connections !)
  //    --> in this case, we WANT the default behavior (call handle_close())
  // - by an external thread closing down the connection (see abort())
  //    --> close the socket !

  switch (arg_in)
  {
    // called by:
    // - any worker from ACE_Task_Base on clean-up
    // - acceptor/connector if there are too many connections (i.e. open() returned -1)
    case 0:
    {
      // check specifically for the first case...
      if (ACE_OS::thr_equal(ACE_Thread::self(), last_thread()))
      {
//       if (module())
//         ACE_DEBUG((LM_DEBUG,
//                    ACE_TEXT("\"%s\" worker thread (ID: %t) leaving...\n"),
//                    ACE_TEXT_ALWAYS_CHAR(name())));
//       else
//         ACE_DEBUG((LM_DEBUG,
//                    ACE_TEXT("worker thread (ID: %t) leaving...\n")));

        // don't do anything...
        break;
      } // end IF

      // too many connections: invoke inherited default behavior
      // --> close() --> handle_close() --> ... --> delete "this"
      // --> simply fall through to the next case
    }
    // called by external thread wanting to close our connection (see abort())
    case 1:
    {
      // *NOTE*: this is NOT the elegant way to go about "aborting" a connection
      // (even though it works), as it will confuse the reactor
      // --> simply call handle_close instead...
//       int result = inherited::peer_.close();
//       if (result == -1)
//         ACE_DEBUG((LM_ERROR,
//                    ACE_TEXT("failed to ACE_SOCK_Stream::close(): \"%m\", returning\n")));
      return inherited::close();
    }
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid argument: %u, returning\n"),
                 arg_in));

      // what else can we do ?
      break;
    }
  } // end SWITCH

  return 0;
}
*/

template <typename ConfigType,
          typename StatisticsContainerType>
int
RPG_Net_AsynchSocketHandler_T<ConfigType,
                              StatisticsContainerType>::handle_close(ACE_HANDLE handle_in,
                                                                     ACE_Reactor_Mask mask_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_AsynchSocketHandler_T::handle_close"));

  // clean up
  myInputStream.cancel();
  myOutputStream.cancel();

  // *NOTE*: called when:
  // - the client closes the socket (handle_xxx() returned -1)
  // - the connection has been rejected (e.g. too many open)
  // - the connection has been aborted locally
  delete this;

  return 0;
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
    message_out = static_cast<ACE_Message_Block*>(myUserData.messageAllocator->malloc(requestedSize_in));
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

  ACE_DEBUG ((LM_DEBUG, "********************\n"));
  ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "bytes_to_write", result.bytes_to_write()));
  ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "handle", result.handle()));
  ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "bytes_transfered", result.bytes_transferred()));
  ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "act", (uintptr_t)result.act()));
  ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "success", result.success()));
  ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "completion_key", (uintptr_t)result.completion_key()));
  ACE_DEBUG ((LM_DEBUG, "%s = %d\n", "error", result.error()));
  ACE_DEBUG ((LM_DEBUG, "********************\n"));

  // sanity check
  if (result.success() == 0)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to write to output stream (%d): %d, \"%m\", continuing\n"),
               result.handle(),
               result.error()));

  // sanity check: handle short writes gracefully ?
  if (result.bytes_to_write() != result.bytes_transferred())
  {
    // *TODO*: handle short writes gracefully
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("sent %u/%u byte(s) only, continuing\n"),
               result.bytes_transferred(),
               result.bytes_to_write()));
  } // end IF

  // clean up
  result.message_block().release();
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

  ACE_HANDLE socket_handle = handle();
  if (socket_handle != ACE_INVALID_HANDLE)
    if (ACE_OS::closesocket(socket_handle) == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_OS::closesocket(): \"%m\", continuing\n")));
}

template <typename ConfigType,
          typename StatisticsContainerType>
unsigned int
RPG_Net_AsynchSocketHandler_T<ConfigType,
                              StatisticsContainerType>::getID() const
{
  RPG_TRACE(ACE_TEXT("RPG_Net_AsynchSocketHandler_T::getID"));

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
             getID(),
             localAddress.c_str(),
             buffer));
}
