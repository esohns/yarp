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

#include "rpg_net_defines.h"
#include "rpg_net_common_tools.h"

#include "rpg_stream_iallocator.h"

#include "rpg_common_macros.h"

#include <ace/Proactor.h>
#include <ace/Message_Block.h>
#include <ace/INET_Addr.h>

template <typename ConfigType,
          typename StatisticsContainerType>
RPG_Net_AsynchSocketHandler_T<ConfigType,
		                          StatisticsContainerType>::RPG_Net_AsynchSocketHandler_T()
 : inherited(),
//    myUserData(),
   myIsInitialized(false),
   myInputStream(),
   myOutputStream(),
   myPeer(),
   myID(0),
   myIsRegistered(false),
   myManager(NULL)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_AsynchSocketHandler_T::RPG_Net_AsynchSocketHandler_T"));

}

template <typename ConfigType,
          typename StatisticsContainerType>
RPG_Net_AsynchSocketHandler_T<ConfigType,
		                          StatisticsContainerType>::RPG_Net_AsynchSocketHandler_T(MANAGER_t* manager_in)
 : inherited(),
//    myUserData(),
   myIsInitialized(false),
   myInputStream(),
   myOutputStream(),
   myPeer(),
   myID(0),
   myIsRegistered(false),
   myManager(manager_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_AsynchSocketHandler_T::RPG_Net_AsynchSocketHandler_T"));

//   // init user data
//   ACE_OS::memset(&myUserData,
//                  0,
//                  sizeof(ConfigType));

  if (myManager)
  {
    // (try to) register with the connection manager...
    // *NOTE*: we do it here because we WANT to init() myUserData early...
    // *WARNING*: as we register BEFORE the connection has fully opened, there
    // may be a small window for races...
    try
    {
      myIsRegistered = myManager->registerConnection(this);
    }
    catch (...)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("caught exception in RPG_Net_IConnectionManager::registerConnection(), continuing\n")));
    }
  } // end IF
}

template <typename ConfigType,
          typename StatisticsContainerType>
RPG_Net_AsynchSocketHandler_T<ConfigType,
		                          StatisticsContainerType>::~RPG_Net_AsynchSocketHandler_T()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_AsynchSocketHandler_T::~RPG_Net_AsynchSocketHandler_T"));

  if (myManager)
  { // (try to) de-register with connection manager
    if (myIsRegistered)
    {
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
  } // end IF

	myInputStream.cancel();
  myOutputStream.cancel();
  ACE_OS::closesocket(handle());
}

template <typename ConfigType,
          typename StatisticsContainerType>
void
RPG_Net_AsynchSocketHandler_T<ConfigType,
		                          StatisticsContainerType>::open(ACE_HANDLE handle_in,
                  																					 ACE_Message_Block& messageBlock_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_AsynchSocketHandler_T::open"));

  // sanity check(s)
  if (!myIsRegistered)
  {
    // too many connections...
    ACE_OS::last_error(EBUSY);

//     ACE_DEBUG((LM_ERROR,
//                ACE_TEXT("failed to register connection (ID: %u), aborting\n"),
//                getID()));

		// clean up
		delete this;

    return;
  } // end IF
  ACE_ASSERT(myIsInitialized);

  // step1: tweak socket
  // *TODO*: assumptions about ConfigType ?!?: clearly a design glitch
  // --> implement higher up !
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

	// init i/o streams
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
  }
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
  }

  // need to pass any data ?
  if (messageBlock_in.length() == 0)
	{
		// init asynch reading
    initiate_read_stream();

    return;
	}

  ACE_Message_Block& duplicate = *messageBlock_in.duplicate();
  // fake a result to emulate regular behavior
  ACE_Asynch_Read_Stream_Result_Impl* fake_result =
    proactor()->create_asynch_read_stream_result(proxy(),            // handler proxy
                                                 handle_in,          // socket handle
                                                 duplicate,          // buffer
                                                 duplicate.size(),   // (max) bytes to read
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
  }
	size_t bytes_transferred = duplicate.length();
  // <complete> for Accept would have already moved the <wr_ptr>
  // forward. Update it to the beginning position.
  duplicate.wr_ptr(duplicate.wr_ptr() - bytes_transferred);

  // invoke ourselves (see handle_read_stream)
  fake_result->complete(duplicate.length(), // bytes read
                        1,                  // success
                        NULL,               // ACT
                        0);                 // error

  // clean up
  delete fake_result;
}

template <typename ConfigType,
          typename StatisticsContainerType>
void
RPG_Net_AsynchSocketHandler_T<ConfigType,
		                          StatisticsContainerType>::addresses(const ACE_INET_Addr& remoteAddress_in,
                                                                  const ACE_INET_Addr& localAddress_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_AsynchSocketHandler_T::addresses"));

	myPeer = remoteAddress_in;
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

  //   // de-register with connection manager
  //   // *NOTE*: we do it here, while our handle is still "valid"...
  //   if (myIsRegistered)
  //   {
  //     RPG_NET_CONNECTIONMANAGER_SINGLETON::instance()->deregisterConnection(getID());
  //
  //     // remember this...
  //     myIsRegistered = false;
  //   } // end IF

  // *NOTE*: called when:
  // - the client closes the socket --> child handle_xxx() returns -1
  // - we reject the connection (too many open)
  delete this;
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

	// sanity check
	ACE_ASSERT(myIsInitialized);

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
	{
		ACE_DEBUG((LM_ERROR,
		           ACE_TEXT("failed to write to output stream (%d): %d, \"%m\", continuing\n"),
  		         result.handle(),
    		       result.error()));
	}
	// sanity check: handle short writes gracefully ?
	if (result.bytes_to_write() != result.bytes_transferred())
	{
		// *TODO*: handle short writes gracefully
		ACE_DEBUG((LM_ERROR,
		           ACE_TEXT("sent %u/%u byte(s) only, continuing\n"),
		           result.bytes_transferred(),
		           result.bytes_to_write()));
	}

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
  ACE_Message_Block* message_block = allocateMessage(RPG_NET_DEF_NETWORK_BUFFER_SIZE);
  if (!message_block)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Net_AsynchSocketHandler_T::allocateMessage(%u), aborting\n"),
               RPG_NET_DEF_NETWORK_BUFFER_SIZE));
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
	}
}

template <typename ConfigType,
          typename StatisticsContainerType>
void
RPG_Net_AsynchSocketHandler_T<ConfigType,
		                          StatisticsContainerType>::init(const ConfigType& userData_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_AsynchSocketHandler_T::init"));

  myUserData = userData_in;
  myIsInitialized = true;
}

template <typename ConfigType,
          typename StatisticsContainerType>
void
RPG_Net_AsynchSocketHandler_T<ConfigType,
		                          StatisticsContainerType>::abort()
{
  RPG_TRACE(ACE_TEXT("RPG_Net_AsynchSocketHandler_T::abort"));

  // call baseclass - will clean everything (including ourselves !) up
  // --> triggers handle_close()
  if (close(1) == -1)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Net_AsynchSocketHandler_T::close(1): \"%m\", continuing\n")));
}

template <typename ConfigType,
          typename StatisticsContainerType>
unsigned int
RPG_Net_AsynchSocketHandler_T<ConfigType,
		                          StatisticsContainerType>::getID() const
{
  RPG_TRACE(ACE_TEXT("RPG_Net_AsynchSocketHandler_T::getID"));

  // *PORTABILITY*: this isn't entirely portable...
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
  return handle();
#else
  // *TODO*: clean this up !
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

  // debug info
  ACE_TCHAR buf[BUFSIZ];
  ACE_OS::memset(buf,
                 0,
                 (BUFSIZ * sizeof(ACE_TCHAR)));
  std::string remoteAddress;
  if (myPeer.addr_to_string(buf, (BUFSIZ * sizeof(ACE_TCHAR))) == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_INET_Addr::addr_to_string(): \"%m\", aborting\n")));

    return;
  } // end IF

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("connection [%u]: --> (\"%s\")\n"),
             getID(),
             buf));
}
