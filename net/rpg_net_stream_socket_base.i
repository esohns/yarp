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

#include "rpg_net_common.h"
#include "rpg_net_defines.h"
#include "rpg_net_remote_comm.h"
#include "rpg_net_common_tools.h"
#include "rpg_net_message.h"

template <typename StreamType>
RPG_Net_StreamSocketBase<StreamType>::RPG_Net_StreamSocketBase()
 : myAllocator(NULL),
   myCurrentBuffer(NULL)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_StreamSocketBase::RPG_Net_StreamSocketBase"));

}

template <typename StreamType>
RPG_Net_StreamSocketBase<StreamType>::~RPG_Net_StreamSocketBase()
{
  ACE_TRACE(ACE_TEXT("RPG_Net_StreamSocketBase::~RPG_Net_StreamSocketBase"));

}

template <typename StreamType>
int
RPG_Net_StreamSocketBase<StreamType>::open(void* arg_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_StreamSocketBase::open"));

  // sanity check
  ACE_ASSERT(arg_in);
  // *NOTE*: we should have registered/initialized by now...
  // --> make sure this was successful before we proceed
  if (!inherited::isRegistered())
  {
    // too many connections...
//     ACE_DEBUG((LM_ERROR,
//                ACE_TEXT("failed to register connection (ID: %u), aborting\n"),
//                getID()));

    // reactor will invoke handle_close() --> we commit suicide
    return -1;
  } // end IF

  // *NOTE*: need to do this BEFORE opening any socket because
  // ::getnameinfo apparently cannot handle AF_PACKET sockets (don't ask !)...
  // retrieve local IP address
  std::string ip_address;
  std::string interface;
  if (!RPG_Net_Common_Tools::retrieveLocalIPAddress(interface,
                                                    ip_address))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Net_Common_Tools::retrieveLocalIPAddress(\"%s\"), aborting\n"),
               interface.c_str()));

    // reactor will invoke handle_close() --> we commit suicide
    return -1;
  } // end IF

  // retrieve local hostname
  std::string hostname;
  if (!RPG_Net_Common_Tools::retrieveLocalHostname(hostname))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Net_Common_Tools::retrieveLocalHostname(), aborting\n")));

    // reactor will invoke handle_close() --> we commit suicide
    return -1;
  } // end IF

  // retrieve remote host/address/port
  ACE_INET_Addr remoteAddress;
  if (peer().get_remote_addr(remoteAddress) == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_SOCK_Stream::get_remote_addr(): \"%s\", aborting\n"),
               ACE_OS::strerror(errno)));

    // reactor will invoke handle_close() --> we commit suicide
    return -1;
  } // end IF

  // debug info
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("established connection [ID: %u]: \"%s\" | \"%s\" <--> \"%s:%u\"\n"),
             getID(),
             hostname.c_str(),
             ip_address.c_str(),
             remoteAddress.get_host_name(),
             remoteAddress.get_port_number()));

  // step0: retrieve config, init ourselves
  myAllocator = myUserData.messageAllocator;
  // sanity check
  ACE_ASSERT(myAllocator);

  // step1: tweak socket
  if (myUserData.socketBufferSize)
  {
    if (!RPG_Net_Common_Tools::setSocketBuffer(get_handle(),
                                               SO_RCVBUF,
                                               myUserData.socketBufferSize))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to setSocketBuffer(%u) for %u, aborting\n"),
                 myUserData.socketBufferSize,
                 get_handle()));

      // reactor will invoke handle_close() --> we commit suicide
      return -1;
    } // end IF
  } // end IF

  // step2: start client ping timer and register us at the reactor
  // *NOTE*: this is done by the base class !
  // *WARNING*: as soon as this returns, data will start arriving
  // at handle_input() and fill our stream...
  int result = inherited::open(arg_in);
  if (result == -1)
  {
    // *NOTE*: this might have happened because there are too many
    // open connections... --> not an error !
    if (ACE_OS::last_error())
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to inherited::open(): \"%s\", aborting\n"),
                 ACE_OS::strerror(errno)));
    } // end IF

    // reactor will invoke handle_close() --> we commit suicide
    return -1;
  } // end IF

  // step3: init/start data processing stream
  // *NOTE*: need to do this AFTER inherited::open() so that get_handle() works...
//   myUserData.connectionID = getID();
  myUserData.sessionID = get_handle();
  if (!myStream.init(myUserData))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to init processing stream, aborting\n")));

    // reactor will invoke handle_close() --> we commit suicide
    return -1;
  } // end IF
  myStream.start();
  if (!myStream.isRunning())
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to start processing stream, aborting\n")));

    // reactor will invoke handle_close() --> we commit suicide
    return -1;
  } // end IF

  return 0;
}

template <typename StreamType>
int
RPG_Net_StreamSocketBase<StreamType>::handle_input(ACE_HANDLE handle_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_StreamSocketBase::handle_input"));

  ACE_UNUSED_ARG(handle_in);

  size_t bytes_received = 0;

  // sanity check
  ACE_ASSERT(myCurrentBuffer == NULL);

  // read some data from the socket
  myCurrentBuffer = allocateMessage(RPG_NET_DEF_NETWORK_BUFFER_SIZE);
  if (myCurrentBuffer == NULL)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to allocateMessage(%u), aborting\n"),
               RPG_NET_DEF_NETWORK_BUFFER_SIZE));

    // reactor will invoke handle_close() --> we commit suicide
    return -1;
  } // end IF

  // read some data from the socket...
  bytes_received = peer_.recv(myCurrentBuffer->wr_ptr(),
                              myCurrentBuffer->size());
  switch (bytes_received)
  {
    case -1:
    {
      ACE_DEBUG((LM_ERROR,
                  ACE_TEXT("failed to ACE_SOCK_Stream::recv(): \"%s\", returning\n"),
                  ACE_OS::strerror(errno)));

      // clean up
      myCurrentBuffer->release();
      myCurrentBuffer = NULL;

      // reactor will invoke handle_close() --> we commit suicide
      return -1;
    }
    // *** GOOD CASES ***
    case 0:
    {
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("[%u]: socket was closed by the peer...\n"),
                 handle_in));

      // clean up
      myCurrentBuffer->release();
      myCurrentBuffer = NULL;

      // reactor will invoke handle_close() --> we commit suicide
      return -1;
    }
    default:
    {
      // debug info
//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("[%u]: received %u bytes...\n"),
//                  handle_in,
//                  bytes_received));

      // adjust write pointer
      myCurrentBuffer->wr_ptr(bytes_received);

      break;
    }
  } // end SWITCH

  // push the buffer onto our stream for processing
  if (myStream.put(myCurrentBuffer) == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Stream::put(): \"%s\", aborting\n"),
               ACE_OS::strerror(errno)));

      // clean up
    myCurrentBuffer->release();
    myCurrentBuffer = NULL;

    // reactor will invoke handle_close() --> we commit suicide
    return -1;
  } // end IF

  // ... bye bye
  myCurrentBuffer = NULL;

  return 0;
}

template <typename StreamType>
int
RPG_Net_StreamSocketBase<StreamType>::handle_close(ACE_HANDLE handle_in,
                                                   ACE_Reactor_Mask mask_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_StreamSocketBase::handle_close"));

  // clean up
  if (myStream.isRunning())
  {
    myStream.stop();
    myStream.waitForCompletion();
  } // end IF

  // *NOTE*: base class will commit suicide properly --> cleans us up
  return inherited::handle_close(handle_in,
                                 mask_in);
}

template <typename StreamType>
RPG_Net_Message*
RPG_Net_StreamSocketBase<StreamType>::allocateMessage(const unsigned long& requestedSize_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_StreamSocketBase::allocateMessage"));

  // init return value(s)
  RPG_Net_Message* message_out = NULL;

  try
  {
    message_out = ACE_static_cast(RPG_Net_Message*,
                                  myAllocator->malloc(requestedSize_in));
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
