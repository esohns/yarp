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

#include "rpg_net_module_sockethandler.h"

// #include "rps_flb_common.h"
// #include "rps_flb_common_tools.h"
// #include "rps_flb_common_network_tools.h"
#include "rpg_net_message.h"
// #include "rps_flb_common_message_scheduler.h"
// #include "rps_flb_common_sessionconfigbase.h"

#include <stream_iallocator.h>

// #include <ace/OS.h>
#include <ace/Time_Value.h>
#include <ace/INET_Addr.h>
// #include <ace/Message_Block.h>

RPG_Net_Module_SocketHandler::RPG_Net_Module_SocketHandler()
 : inherited(false), // DON'T auto-start !
   myIsInitialized(false),
   myStatCollectHandler(this,
                        STATISTICHANDLER_TYPE::ACTION_COLLECT),
   myStatCollectHandlerID(0),
   myAllocator(NULL)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Module_SocketHandler::RPG_Net_Module_SocketHandler"));

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("activating timer dispatch queue...\n")));

  // ok: activate timer queue
  if (myTimerQueue.activate() == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to activate() timer dispatch queue: \"%s\", returning\n"),
               ACE_OS::strerror(errno)));

    return;
  } // end IF

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("activating timer dispatch queue...DONE\n")));
}

RPG_Net_Module_SocketHandler::~RPG_Net_Module_SocketHandler()
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Module_SocketHandler::~RPG_Net_Module_SocketHandler"));

  // clean up
  if (myStatCollectHandlerID)
  {
    if (myTimerQueue.cancel(myStatCollectHandlerID) == -1)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to cancel statistics collection timer (ID: %u): \"%s\", continuing\n"),
                 myStatCollectHandlerID,
                 ACE_OS::strerror(errno)));
    } // end IF

    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("deactivated statistics collection timer (ID: %u)...\n"),
               myStatCollectHandlerID));

    myStatCollectHandlerID = 0;
  } // end IF

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("deactivating timer dispatch queue...\n")));

  myTimerQueue.deactivate();
  // make sure the dispatcher thread is really dead...
  myTimerQueue.wait();

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("deactivating timer dispatch queue...DONE\n")));
}

const bool
RPG_Net_Module_SocketHandler::init(Stream_IAllocator* messageAllocator_in,
                                   const std::string& networkInterface_in,
                                   const std::string& libpcapFilterString_in,
                                   const unsigned long& sizeReceiveBuffer_in,
                                   const unsigned long& statCollectInterval_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Module_SocketHandler::init"));

  // sanity check(s)
  if (myIsInitialized)
  {
    ACE_DEBUG((LM_WARNING,
               ACE_TEXT("already initialized --> nothing to do, aborting\n")));

    return false;
  } // end IF

  // sanity check(s)
  if (!messageAllocator_in)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid argument (was NULL), aborting\n")));

    return false;
  } // end IF

  myAllocator = messageAllocator_in;

  // *NOTE*: need to do this BEFORE opening any socket because
  // ::getnameinfo apparently cannot handle AF_PACKET sockets (don't ask !)...
  // retrieve local IP address
  std::string ip_address;
  if (!RPS_FLB_Common_Network_Tools::retrieveLocalIPAddress(networkInterface_in,
                                                            ip_address))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPS_FLB_Common_Network_Tools::retrieveLocalIPAddress(\"%s\"), aborting\n"),
               networkInterface_in.c_str()));

    return false;
  } // end IF

  // init pcap...
  char errbuf[PCAP_ERRBUF_SIZE]; // defined in pcap.h
  // zero errbuf (may contain warnings even if pcap_open_live succeeds)
  ACE_OS::strcpy(errbuf,
                 "");
  myPCAPHandle = pcap_open_live(ACE_const_cast(char*,
                                               networkInterface_in.c_str()),
                                PCAP_SNAPSHOT_LEN, // max frame length
                                1,                 // promiscuous mode
                                PCAP_READ_TIMEOUT, // read timeout (ms)
                                errbuf);           // buffer for error string
  if (!myPCAPHandle)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to pcap_open_live(\"%s\"): \"%s\", aborting\n"),
               networkInterface_in.c_str(),
               errbuf));

    return false;
  } // end IF

  // *IMPORTANT NOTE*: need to clean up myPCAPHandle beyond this point !

  // check errbuf for warnings...
  if (ACE_OS::strlen(errbuf) > 0)
  {
    ACE_DEBUG((LM_WARNING,
               ACE_TEXT("pcap_open_live(\"%s\") succeeded: \"%s\", continuing\n"),
               networkInterface_in.c_str(),
               errbuf));

    // zero errbuf, just in case
    ACE_OS::strcpy(errbuf,
                   "");
  } // end IF

  // debug info
  ACE_DEBUG((LM_INFO,
             ACE_TEXT("opened capture socket (ID: %d)\n"),
             pcap_fileno(myPCAPHandle)));

  // *TODO*: we probably DON'T want to ever block inside pcap_dispatch...
//   if (pcap_setnonblock(myPCAPHandle,
//                        1, // don't block on socket handle...
//                        errbuf) == -1)
//   {
//     ACE_DEBUG((LM_ERROR,
//                ACE_TEXT("failed to pcap_setnonblock(): \"%s\", aborting\n"),
//                errbuf));
//
//     // clean up
//     pcap_close(myPCAPHandle);
//     myPCAPHandle = NULL;
//
//     return false;
//   } // end IF

  // find out whether our capture socket is in blocking mode...
  int nonblocking_mode = -1;
  nonblocking_mode = pcap_getnonblock(myPCAPHandle,
                                      errbuf);
  if (nonblocking_mode == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to pcap_getnonblock(): \"%s\", aborting\n"),
               errbuf));

    // clean up
    pcap_close(myPCAPHandle);
    myPCAPHandle = NULL;

    return false;
  } // end IF

  // debug info
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("capture socket (ID: %d) will %sblock...\n"),
             pcap_fileno(myPCAPHandle),
             (nonblocking_mode ?
              ACE_TEXT("NOT ") :
              ACE_TEXT(""))));

  // find out some basic network characteristics...
  bpf_u_int32 net_number = 0;
  if (pcap_lookupnet(ACE_const_cast(char*,
                                    networkInterface_in.c_str()),
                     &net_number,
                     &myInterfaceNetmask,
                     errbuf) == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to pcap_lookupnet(\"%s\"): \"%s\", aborting\n"),
               networkInterface_in.c_str(),
               errbuf));

    // clean up
    pcap_close(myPCAPHandle);
    myPCAPHandle = NULL;

    return false;
  } // end IF

  // tweak receive buffer size --> reduce packet loss ?!?
  myReceiveBufferSize = sizeReceiveBuffer_in;
  int bufsize = 0;
  if (myReceiveBufferSize)
  {
    bufsize = myReceiveBufferSize;
    // *IMPORTANT NOTE*: Linux will always try to set the DOUBLE buffer size value
    // --> we pass it HALF the value (note that this may produce a warning later on if myReceiveBufferSize is odd)
    // *TODO*: do this at compile time ?
    if (RPS_FLB_Common_Tools::isLinux())
    {
      bufsize /= 2;
    } // end IF

    // *PORTABILITY*: this isn't portable so we do an ugly hack...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (::pcap_setuserbuffer(myPCAPHandle,
                             bufsize) == -1)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to pcap_setuserbuffer(%u): \"%s\", aborting\n"),
                 bufsize,
                 pcap_geterr(myPCAPHandle)));
#else
//    if (ACE_OS::setsockopt(pcap_get_selectable_fd(myPCAPHandle),
    if (ACE_OS::setsockopt(pcap_fileno(myPCAPHandle),
                           SOL_SOCKET,
                           SO_RCVBUF,
                           ACE_reinterpret_cast(const char*, &bufsize),
                           sizeof(bufsize)))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_OS::setsockopt(%d): \"%s\", aborting\n"),
                 pcap_fileno(myPCAPHandle),
                 ACE_OS::strerror(errno)));
#endif

      // clean up
      pcap_close(myPCAPHandle);
      myPCAPHandle = NULL;

      return false;
    } // end IF
  } // end IF

  // sanity check
  bufsize = 0;
  // *PORTABILITY*: this isn't portable so we do an ugly hack...
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
  // *IMPORTANT NOTE*: need to pre-init this value to the CORRECT size, otherwise nothing is returned...
  int retsize = sizeof(bufsize);
//    if (ACE_OS::getsockopt(pcap_get_selectable_fd(myPCAPHandle),
  if (ACE_OS::getsockopt(pcap_fileno(myPCAPHandle),
                         SOL_SOCKET,
                         SO_RCVBUF,
                         ACE_reinterpret_cast(char*, &bufsize),
                         &retsize))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_OS::getsockopt(): \"%s\", aborting\n"),
               ACE_OS::strerror(errno)));

    // clean up
    pcap_close(myPCAPHandle);
    myPCAPHandle = NULL;

    return false;
  } // end IF

  if (ACE_static_cast(unsigned long,
                      bufsize) != myReceiveBufferSize)
  {
    // *IMPORTANT NOTE*: this may also happen on Linux systems if myReceiveBufferSize is odd, see above
    ACE_DEBUG((LM_WARNING,
               ACE_TEXT("ACE_OS::getsockopt(SO_RCVBUF) on handle (ID: \"%d\") returned \"%d\" (expected: \"%u\"), --> check (hard) limit configuration !, continuing\n"),
               pcap_fileno(myPCAPHandle),
               bufsize,
               myReceiveBufferSize));
  } // end IF
  else
  {
    // debug info
    ACE_DEBUG((LM_INFO,
               ACE_TEXT("set SO_RCVBUF option of capture socket (ID: %d) to: %u\n"),
               pcap_fileno(myPCAPHandle),
               myReceiveBufferSize));
  } // end ELSE
#endif

  // filter anything ?
  if (!libpcapFilterString_in.empty())
  {
    bpf_program filter_code;

    // compile and apply the filter
    if (pcap_compile(myPCAPHandle,              // handle
                     &filter_code,              // output: filter code
                     ACE_const_cast(char*,
                                    libpcapFilterString_in.c_str()),
                     1,                         // optimize ? sure !
                     myInterfaceNetmask) == -1) // netmask
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to pcap_compile() filter (\"%s\"): \"%s\", aborting\n"),
                 libpcapFilterString_in.c_str(),
                 pcap_geterr(myPCAPHandle)));

      // clean up
      pcap_close(myPCAPHandle);
      myPCAPHandle = NULL;

      return false;
    } // end IF

    // *IMPORTANT NOTE*: need to clean up filter beyond this point !

    if (pcap_setfilter(myPCAPHandle,
                       &filter_code) == -1)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to pcap_setfilter() filter (\"%s\"): \"%s\", aborting\n"),
                 libpcapFilterString_in.c_str(),
                 pcap_geterr(myPCAPHandle)));

      // clean up
      pcap_freecode(&filter_code);
      pcap_close(myPCAPHandle);
      myPCAPHandle = NULL;

      return false;
    } // end IF

    // clean up
    pcap_freecode(&filter_code);
  } // end IF

  // setup up regular statistics collection...
  if (statCollectInterval_in)
  {
    myPcapStatCollectHandlerID = 0;
    ACE_Time_Value collecting_interval(statCollectInterval_in,
                                       0);
    myPcapStatCollectHandlerID = myTimerQueue.schedule(&myPcapStatCollectHandler,
                                                       NULL,
                                                       ACE_OS::gettimeofday () + collecting_interval,
                                                       collecting_interval);
    if (myPcapStatCollectHandlerID == -1)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to schedule() timer: \"%s\", aborting\n"),
                 ACE_OS::strerror(errno)));

      // reset so we don't get confused in the dtor !
      myPcapStatCollectHandlerID = 0;

      // clean up
      pcap_close(myPCAPHandle);
      myPCAPHandle = NULL;

      return false;
    } // end IF

    // *IMPORTANT NOTE*: need to clean up filter beyond this point !

    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("scheduled statistics collecting timer (ID: %d) for intervals of %u second(s)...\n"),
               myPcapStatCollectHandlerID,
               statCollectInterval_in));
  } // end IF

  // retrieve local hostname
  std::string hostname;
  ACE_TCHAR host[MAXHOSTNAMELEN + 1];
  ACE_OS::memset(&host,
                 0,
                 MAXHOSTNAMELEN + 1);
  if (ACE_OS::hostname(host,
                       MAXHOSTNAMELEN))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_OS::hostname(): \"%s\" -->  --> check implementation !, aborting\n"),
               ACE_OS::strerror(errno)));

    // clean up
    pcap_close(myPCAPHandle);
    myPCAPHandle = NULL;
    if (myPcapStatCollectHandlerID)
    {
      if (myTimerQueue.cancel(myPcapStatCollectHandlerID) == -1)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to cancel timer (ID: %u): \"%s\", continuing\n"),
                   myPcapStatCollectHandlerID,
                   ACE_OS::strerror(errno)));
      } // end IF
      myPcapStatCollectHandlerID = 0;
    } // end IF

    return false;
  } // end IF
  hostname = host;

  // OK: all's well...
  myIsInitialized = true;

  // debug info
  printCaptureInfo(hostname,
                   networkInterface_in,
                   ip_address,
                   net_number,
                   myInterfaceNetmask,
                   myPCAPHandle);

  return myIsInitialized;
}

int RPG_Net_Module_SocketHandler::svc(void)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Module_SocketHandler::svc"));

  ACE_Message_Block* controlMessage = NULL;
  ACE_Time_Value     now;
  int                ret            = 0;
  bool               leave_loop     = false;

  // sanity check(s)
  if (!myIsInitialized)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("not initialized --> check implementation !, aborting\n")));

    // tell the controller we're finished...
    finished();

    return -1;
  } // end IF

  // step1: send corresp. session message downstream...
  if (!inherited::putSessionMessage(RPS_FLB_Common_SessionMessage::SESSION_BEGIN,
                                    ACE_Time_Value::zero,          // N/A
                                    ACE_OS::gettimeofday(),
                                    std::string(""),               // N/A
                                    myPCAPHandle,
                                    pcap_is_swapped(myPCAPHandle), // N/A
                                    false))                        // N/A
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("putSessionMessage failed --> check implementation !, aborting\n")));

    // tell the controller we're finished...
    finished();

    return -1;
  } // end IF

  // step2: process incoming packets
  while (!leave_loop)
  {
    // check user stop condition --> someone stopped() the stream ?
    if (!msg_queue_->is_empty())
    {
      now = ACE_OS::gettimeofday();

      // Note: something SHOULD be in the queue...
      if (getq(controlMessage, &now) == -1)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to ACE_Task::getq(): \"%s\" --> check implementation, aborting\n"),
                   ACE_OS::strerror(errno)));

        // remember we failed...
        ret = -1;

        // leave processing loop
        break;
      } // end IF

      // handle this control message
      bool user_abort = false;
      bool passMessageDownstream = true;
      inherited::handleControlMessage(controlMessage,
                                      user_abort,
                                      passMessageDownstream);

      if (passMessageDownstream)
      {
        // we've been told to pass this (control) message downstream...
        if (put_next(controlMessage) == -1)
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("failed to ACE_Task::put_next(): \"%s\", continuing\n"),
                     ACE_OS::strerror(errno)));

          // remember we failed (at something)...
          ret = -1;
        } // end IF
      } // end IF

      // handle user abort
      if (user_abort)
      {
        ACE_DEBUG((LM_INFO,
                   ACE_TEXT("asynchronous (user) abort, stopping capturing activities...\n")));

        // leave processing loop
        break;
      } // end IF
    } // end IF

    // OK: record next packets
    ret = pcap_dispatch(myPCAPHandle,
                        -1, // <-- "A cnt of -1 processes all the packets received
                            // in one buffer when reading a live capture,..."
                            // http://www.tcpdump.org/pcap3_man.html
                        &RPG_Net_Module_SocketHandler::recordPacket,
                        ACE_reinterpret_cast(u_char*,
                                             this)); // pass handle to ourselves...
    switch (ret)
    {
      // *** GOOD CASE ***
      default:
      {

        break;
      }
      case 0:
      {
        // no packet processed
        // probable causes:
        // - no LAN traffic/read timeout (maybe we are in non-blocking mode ?)
        // - no packets passed the filter
        // - someone (temporarily) pulled the network cable
        // - NIC/network cable broken

        // *IMPORTANT NOTE*: the rationale here is that this state will
        // eventually be identified (and hopefully resolved) by the
        // monitoring TSC, otherwise we will just continually grow our logfile...
        //ACE_DEBUG((LM_WARNING,
        //           ACE_TEXT("read timeout occurred (check cabling/NIC status LEDs/...), continuing\n")));

        break;
      }
      // *** ERROR CASE ***
      case -1:
      {
        // some internal error
        // Probable cause:
        // - someone (accidentally ?) shutdown the network service
        // - no more memory ?
        // - ...
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to pcap_dispatch(): \"%s\" --> check implementation !, aborting\n"),
                   pcap_geterr(myPCAPHandle)));

        // *IMPORTANT NOTE*: we shut ourselves down so we don't come here again
        // and generate (duplicate ?) error messages until we're officially
        // stop()ed from outside...
        // *TODO*: what else can we do ?
        ret = -1;
        leave_loop = true;

        // *IMPORTANT NOTE*: the rationale here is that this state will
        // eventually be identified (and hopefully resolved) by the
        // monitoring TSC, otherwise (problem persists) it MAY happen
        // that we are just continually restarted by the watchdog...
        break;
      }
      case -2:
      {
        // *IMPORTANT NOTE*: we arrive here when pcap_breakloop was called AND there were no packets.
        // If this was invoked from a signal handler, we should have a stop message in our queue by now
        // --> just proceed normally
        ACE_DEBUG((LM_INFO,
                   ACE_TEXT("pcap_breakloop() invoked, continuing\n")));

        break;
      }
    } // end SWITCH
  } // end WHILE

  ACE_DEBUG((LM_TRACE,
             ACE_TEXT("left capturing loop...\n")));

  // step3: send session message downstream...
  if (!inherited::putSessionMessage(RPS_FLB_Common_SessionMessage::SESSION_END,
                                    ACE_Time_Value::zero,          // N/A
                                    ACE_Time_Value::zero,          // N/A
                                    std::string(""),               // N/A
                                    myPCAPHandle,
                                    pcap_is_swapped(myPCAPHandle), // N/A
                                    true))                         // ALWAYS a user abort...
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("putSessionMessage failed --> check implementation !, aborting\n")));

    // *TODO*: what else can we do ?
    // tell the controller we're finished...
    finished();

    return -1;
  } // end IF

  // tell the controller we're finished...
  finished();

  return ret;
}

void RPG_Net_Module_SocketHandler::stop()
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Module_SocketHandler::stop"));

  // invoke base class behaviour
  inherited::stop();

  // sanity check
  if (myPCAPHandle)
  {
    // break out of (potentially blocked) capture call...
    // *IMPORTANT NOTE*: if we're called from within a signal handler, this will work only
    // if blocking system calls are NOT (!) restarted after we return from here...
    pcap_breakloop(myPCAPHandle);
  } // end IF
}

const bool RPG_Net_Module_SocketHandler::collect(RPS_FLB_Common_CaptureRuntimeStatistic_Type& data_out)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Module_SocketHandler::collect"));

  // just a dummy...
  ACE_UNUSED_ARG(data_out);

  // sanity check(s)
  if (!myIsInitialized)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("not initialized --> check implementation !, aborting\n")));

    return false;
  } // end IF

  // init info container POD
  static pcap_stat statistics_data;
  ACE_OS::memset(&statistics_data,
                 0,
                 sizeof(statistics_data));
//   statistics_data.ps_recv = 0;
//   statistics_data.ps_drop = 0;
//   statistics_data.ps_ifdrop = 0;
// #if defined (ACE_WIN32) || defined (ACE_WIN64)
// // *TODO*: this doesn't work on MY lipcap installation !
// //  statistics_data.bs_capt = 0;
// #endif

  // step1: remember time of stats collection
  static ACE_Time_Value statistics_timestamp;
  statistics_timestamp = ACE_OS::gettimeofday();

  // step2: collect libpcap info
  // *WARNING*: myPCAPHandle is being used by a different thread at the same time...

  // *PORTABILITY*: contrary to what the documentation says (i.e. man pcap) on Linux, ps_recv doesn't seem
  // to return the number of network packets seen on the network since the start of the capture, but rather since
  // the last call to pcap_stats() (i.e. invoking pcap_stats seems to reset the counters...)
  // *IMPORTANT NOTE*: this behaviour was changed in a recent release !
  if (pcap_stats(myPCAPHandle,
                 &statistics_data) != 0)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to pcap_stats(): \"%s\", aborting\n"),
               pcap_geterr(myPCAPHandle)));

    return false;
  } // end IF

//   ACE_DEBUG((LM_DEBUG,
//              "*** recv: \"%u\", drop: \"%u\", ifdrop: \"%u\" ***\n",
//              data_out.ps_recv,
//              data_out.ps_drop,
//              data_out.ps_ifdrop));

  // step3: send this information downstream
  if (!putStatisticsMessage(statistics_data,
                            statistics_timestamp))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to putStatisticsMessage(), aborting\n")));

    return false;
  } // end IF

  // OK: all is well...
  return true;
}

void RPG_Net_Module_SocketHandler::report()
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Module_SocketHandler::report"));

  // sanity check(s)
  if (!myIsInitialized)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("not initialized --> check implementation !, returning\n")));

    return;
  } // end IF

  // *TODO*: support (local) reporting here as well ?
  // --> we currently leave this to the runtime statistics module...
  ACE_ASSERT(false);
}

const bool RPG_Net_Module_SocketHandler::allocateMessage(const unsigned long& requiredSize_in,
                                                                  RPS_FLB_Common_NetworkMessage*& networkMessage_out)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Module_SocketHandler::allocateMessage"));

  // init return value(s)
  networkMessage_out = NULL;

  try
  {
    // *IMPORTANT NOTE*: the argument we pass to the allocator is irrelevant,
    // the allocator allocates RPS_FLB_Common_NetworkMessages...
    networkMessage_out = ACE_static_cast(RPS_FLB_Common_NetworkMessage*,
                                         myAllocator->malloc(requiredSize_in));
  }
  catch(...)
  {
    ACE_DEBUG((LM_CRITICAL,
               ACE_TEXT("caught exception in ACE_Allocator::malloc(%u) --> check implementation !, aborting\n"),
               requiredSize_in));

    return false;
  }
  if (!networkMessage_out)
  {
    ACE_DEBUG((LM_CRITICAL,
               ACE_TEXT("failed to ACE_Allocator::malloc(%u) --> check implementation !, aborting\n"),
               requiredSize_in));

    return false;
  } // end IF

  return true;
}

const bool RPG_Net_Module_SocketHandler::putStatisticsMessage(const RPS_FLB_Common_CaptureRuntimeStatistic_Type& info_in,
                                                                       const ACE_Time_Value& time_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Module_SocketHandler::putStatisticsMessage"));

  // create/collect session data
  RPS_FLB_Common_SessionConfigBase* config = NULL;
  try
  {
    config = new RPS_FLB_Common_SessionConfigBase(info_in,
                                                  time_in);
  }
  catch(...)
  {
    ACE_DEBUG((LM_CRITICAL,
               ACE_TEXT("caught exception in new, aborting\n")));

    return false;
  }
  if (!config)
  {
    ACE_DEBUG((LM_CRITICAL,
               ACE_TEXT("unable to allocate memory, aborting\n")));

    return false;
  } // end IF

  // *IMPORTANT NOTE*: this is fire-and-forget, so we don't need to
  // worry about config any longer !
  return inherited::putSessionMessage(RPS_FLB_Common_SessionMessage::SESSION_STATS,
                                      config);
}

void RPG_Net_Module_SocketHandler::printCaptureInfo(const std::string& hostName_in,
                                                             const std::string& interfaceName_in,
                                                             const std::string& IPAddress_in,
                                                             const bpf_u_int32& networkNumber_in,
                                                             const bpf_u_int32& netMask_in,
                                                             pcap_t* pcapHandle_in) const
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Module_SocketHandler::printCaptureInfo"));

  // debug info
  // network/netmask
  const unsigned char* p = ACE_reinterpret_cast(const unsigned char*,
                                                &networkNumber_in);
  const unsigned char* q = ACE_reinterpret_cast(const unsigned char*,
                                                &netMask_in);

  // data link
  int dataLinkType = -1;
  dataLinkType = pcap_datalink(pcapHandle_in);
  std::string datalink_type_string;
  if (!pcap_datalink_val_to_name(dataLinkType))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to pcap_datalink_val_to_name(%d) --> check implementation !, returning\n"),
               dataLinkType));

    return;
  } // end IF
  datalink_type_string = pcap_datalink_val_to_name(dataLinkType);
  std::string datalink_type_desc_string;
  if (!pcap_datalink_val_to_description(dataLinkType))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to pcap_datalink_val_to_description(%d) --> check implementation !, returning\n"),
               dataLinkType));

    return;
  } // end IF
  datalink_type_desc_string = pcap_datalink_val_to_description(dataLinkType);

  ACE_DEBUG((LM_INFO,
             ACE_TEXT("host: \"%s\"; interface \"%s\"; datalink: \"%s\" (\"%s\"); ip: \"%s\"; network: \"%d.%d.%d.%d\" (--> mask: \"%d.%d.%d.%d\")\n"),
             hostName_in.c_str(),
             interfaceName_in.c_str(),
             datalink_type_string.c_str(),
             datalink_type_desc_string.c_str(),
             IPAddress_in.c_str(),
             *p,
             *(p + 1),
             *(p + 2),
             *(p + 3),
             *q,
             *(q + 1),
             *(q + 2),
             *(q + 3)));
}

// const bool RPG_Net_Module_SocketHandler::putSessionMessage(const RPS_FLB_Common_SessionMessage::SessionMessageType& messageType_in,
//                                                                     const std::string& currentFilename_in,
//                                                                     pcap_t* pcapHandle_in,
//                                                                     const bool& dataIsSwapped_in)
// {
//   ACE_TRACE(ACE_TEXT("RPG_Net_Module_SocketHandler::putSessionMessage"));
//
//   // create/collect session data
//   RPS_FLB_Common_SessionConfigBase* config = NULL;
//   try
//   {
//     config = new RPS_FLB_Common_SessionConfigBase(currentFilename_in,
//                                                   pcapHandle_in,
//                                                   dataIsSwapped_in);
//   }
//   catch(...)
//   {
//     ACE_DEBUG((LM_CRITICAL,
//                ACE_TEXT("caught exception in new, aborting\n")));
//
//     return false;
//   }
//   if (!config)
//   {
//     ACE_DEBUG((LM_CRITICAL,
//                ACE_TEXT("unable to allocate memory, aborting\n")));
//
//     return false;
//   } // end IF
//
//   // *IMPORTANT NOTE*: this is fire-and-forget, so we don't need to
//   // worry about config any longer !
//   return inherited::putSessionMessage(messageType_in,
//                                       config);
// }

void RPG_Net_Module_SocketHandler::recordPacket(u_char* args_in,
                                                         const pcap_pkthdr* header_in,
                                                         const u_char* body_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Module_SocketHandler::recordPacket"));

  static RPS_FLB_Common_NetworkMessage* message;
  static RPG_Net_Module_SocketHandler* this_ptr = ACE_reinterpret_cast(RPG_Net_Module_SocketHandler*,
                                                                                args_in);

  // we need to do this:
  // 1. allocate a message buffer
  // 2. copy the data into the buffer
  // 3. perform message initialization
  // 4. send the message buffer downstream

  // step1
  message = NULL;
  if (!this_ptr->allocateMessage(header_in->caplen,
                                 message))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to allocateMessage(%u) --> check implementation !, returning\n"),
               header_in->caplen));

    return;
  } // end IF

  // sanity check: message has enough space to hold our data ?
  ACE_ASSERT(message->capacity() >= header_in->caplen);

  // *IMPORTANT NOTE*: from this point on, we need to make sure we recycle message !

  // step2
  if (message->copy(ACE_reinterpret_cast(const char*,
                                         body_in),
                    header_in->caplen) == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("message (ID: %u): failed to ACE_Message_Block::copy(): \"%s\" --> check implementation !, returning\n"),
               message->getID(),
               ACE_OS::strerror(errno)));

    // clean up
    message->release();

    return;
  } // end IF

  // step3
  message->init(//ACE_Time_Value::zero, // this is irrelevant !
                *header_in);

  // step4
  if (this_ptr->put_next(message) == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Task::put_next(): \"%s\" --> check implementation !, returning\n"),
               ACE_OS::strerror(errno)));

    // clean up
    message->release();
  } // end IF

  // OK: all finished !
}
