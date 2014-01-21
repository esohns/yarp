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
#include "stdafx.h"

#include "rpg_net_common_tools.h"

#if !defined(ACE_WIN32) && !defined(ACE_WIN64)
#include <netinet/ether.h>
#endif

#include <sstream>

#include <ace/Log_Msg.h>
#include <ace/INET_Addr.h>
#include <ace/Dirent_Selector.h>
#include <ace/Reactor.h>
#include <ace/TP_Reactor.h>
#if defined(ACE_WIN32) || defined(ACE_WIN64)
#include <ace/WFMO_Reactor.h>
#else
#include <ace/Dev_Poll_Reactor.h>
#endif
#include <ace/Proactor.h>

#include "rpg_common_macros.h"
#include "rpg_common_defines.h"
#include "rpg_common_tools.h"
#include "rpg_common_file_tools.h"

#include "rpg_net_defines.h"
#include "rpg_net_packet_headers.h"

ACE_THR_FUNC_RETURN
tp_event_dispatcher_func(void* args_in)
{
  RPG_TRACE(ACE_TEXT("::tp_event_dispatcher_func"));

  bool use_reactor = *reinterpret_cast<bool*>(args_in);

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("(%t) worker starting...\n")));

	// *IMPORTANT NOTE*: "The signal disposition is a per-process attribute: in a multithreaded
  //                   application, the disposition of a particular signal is the same for
  //                   all threads." (see man 7 signal)
//  // ignore SIGPIPE: need this to continue gracefully after a client
//  // suddenly disconnects (i.e. application/system crash, etc...)
//  // --> specify ignore action
//  // *IMPORTANT NOTE*: don't actually need to keep this around after registration
//  // *NOTE*: do NOT restart system calls in this case (see manual)
//  ACE_Sig_Action no_sigpipe(static_cast<ACE_SignalHandler>(SIG_IGN), // ignore action
//                            ACE_Sig_Set(1),                          // mask of signals to be blocked when servicing
//                                                                     // --> block them all (bar KILL/STOP; see manual)
//                            SA_SIGINFO);                             // flags
////                               (SA_RESTART | SA_SIGINFO));              // flags
//  ACE_Sig_Action original_action;
//  if (no_sigpipe.register_action(SIGPIPE, &original_action) == -1)
//    ACE_DEBUG((LM_DEBUG,
//               ACE_TEXT("failed to ACE_Sig_Action::register_action(SIGPIPE): \"%m\", continuing\n")));
//
  int success = -1;
  // handle any events...
  if (use_reactor)
    success = ACE_Reactor::instance()->run_reactor_event_loop(0);
  else
    success = ACE_Proactor::instance()->proactor_run_event_loop(0);
  if (success == -1)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("(%t) failed to handle events: \"%m\", aborting\n")));

  //// clean up
  //if (no_sigpipe.restore_action(SIGPIPE, original_action) == -1)
  //  ACE_DEBUG((LM_DEBUG,
  //             ACE_TEXT("failed to ACE_Sig_Action::restore_action(SIGPIPE): \"%m\", continuing\n")));

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("(%t) worker leaving...\n")));

  return (success == 0 ? NULL : NULL);
}

bool
RPG_Net_Common_Tools::initEventDispatch(const bool& useReactor_in,
                                        const unsigned int& numDispatchThreads_in,
                                        bool& serializeOutput_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Common_Tools::initEventDispatch"));

  // init return value(s)
  serializeOutput_out = false;

  // step1: init reactor/proactor
	if (useReactor_in)
	{
    if (numDispatchThreads_in > 1)
		{
			ACE_Reactor_Impl* reactor_implementation = NULL;
	#if !defined(ACE_WIN32) && !defined(ACE_WIN64)
			if (RPG_COMMON_USE_DEV_POLL_REACTOR)
			  ACE_NEW_RETURN(reactor_implementation,
											 ACE_Dev_Poll_Reactor(ACE::max_handles(),        // max num handles (1024)
																						true,                      // restart after EINTR ?
																						NULL,                      // signal handler handle
																						NULL,                      // timer queue handle
																						0,                         // disable notify pipe ?
																						NULL,                      // notification handler handle
																						1,                         // mask signals ?
																						ACE_DEV_POLL_TOKEN::FIFO), // signal queue
										   false);
			else
			{
			  ACE_NEW_RETURN(reactor_implementation,
											 ACE_TP_Reactor(ACE::max_handles(),              // max num handles (1024)
																		  true,                            // restart after EINTR ?
																			NULL,                            // signal handler handle
																			NULL,                            // timer queue handle
																		  true,                            // mask signals ?
																		  ACE_Select_Reactor_Token::FIFO), // signal queue
										   false);

				serializeOutput_out = true;
			} // end ELSE
	#else
			if (RPG_COMMON_USE_WFMO_REACTOR)
				ACE_NEW_RETURN(reactor_implementation,
											 ACE_WFMO_Reactor(ACE_WFMO_Reactor::DEFAULT_SIZE, // max num handles (62 [+ 2])
																				0,                              // unused
																				NULL,                           // signal handler handle
																				NULL,                           // timer queue handle
																				NULL),                          // notification handler handle
											 false);
			else
			{
			  ACE_NEW_RETURN(reactor_implementation,
											 ACE_TP_Reactor(ACE::max_handles(),              // max num handles (1024)
																		  true,                            // restart after EINTR ?
																			NULL,                            // signal handler handle
																			NULL,                            // timer queue handle
																		  true,                            // mask signals ?
																		  ACE_Select_Reactor_Token::FIFO), // signal queue
										   false);

				serializeOutput_out = true;
			} // end ELSE
	#endif
			ACE_Reactor* new_reactor = NULL;
			ACE_NEW_RETURN(new_reactor,
										 ACE_Reactor(reactor_implementation, 1), // delete in dtor
										 false);
			// make this the "default" reactor...
			ACE_Reactor::instance(new_reactor, 1); // delete in dtor
		} // end IF
	} // end IF
  else
  {
		// *NOTE* using default platform proactor...
  //  ACE_Proactor* proactor = NULL;
  //  ACE_NEW_RETURN(proactor,
  //                 ACE_Proactor(NULL, true, NULL),
  //                 false);
  //  // make this the "default" proactor...
		//ACE_Proactor* previous_proactor = ACE_Proactor::instance(proactor,
		//	                                                       1);       // delete in dtor
		//delete previous_proactor;
  } // end ELSE

  return true;
}

bool
RPG_Net_Common_Tools::startEventDispatch(const bool& useReactor_in,
                                         const unsigned int& numDispatchThreads_in,
                                         int& groupID_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Common_Tools::startEventDispatch"));

  // init return value(s)
  groupID_out = -1;

  // spawn worker(s) ?
  if (numDispatchThreads_in > 1)
  {
    // start a (group of) worker thread(s)...
		ACE_hthread_t* thread_handles = NULL;
		thread_handles = new(std::nothrow) ACE_hthread_t[numDispatchThreads_in];
//    ACE_NEW_NORETURN(thread_handles,
//                     ACE_hthread_t[numThreadPoolThreads_in]);
    if (!thread_handles)
    {
      ACE_DEBUG((LM_CRITICAL,
                 ACE_TEXT("failed to allocate memory, aborting\n")));

      return false;
    } // end IF
		ACE_OS::memset(thread_handles, 0, sizeof(thread_handles));
    const char** thread_names = NULL;
    thread_names = new(std::nothrow) const char*[numDispatchThreads_in];
//    ACE_NEW_NORETURN(thread_names,
//                     const char*[numThreadPoolThreads_in]);
    if (!thread_names)
    {
      ACE_DEBUG((LM_CRITICAL,
                 ACE_TEXT("failed to allocate memory, aborting\n")));

			// clean up
			delete [] thread_handles;

      return false;
    } // end IF
    char* thread_name = NULL;
		std::string buffer;
    std::ostringstream converter;
    for (unsigned int i = 0;
			   i < numDispatchThreads_in;
				 i++)
    {
      thread_name = NULL;
      thread_name = new(std::nothrow) char[RPG_COMMON_BUFSIZE];
//      ACE_NEW_NORETURN(thread_name,
//                       char[RPG_COMMON_BUFSIZE]);
      if (!thread_name)
      {
        ACE_DEBUG((LM_CRITICAL,
                   ACE_TEXT("failed to allocate memory, aborting\n")));

        // clean up
				delete [] thread_handles;
				for (unsigned int j = 0; j < i; j++)
          delete [] thread_names[j];
        delete [] thread_names;

        return false;
      } // end IF
      converter.clear();
      converter.str(ACE_TEXT_ALWAYS_CHAR(""));
      converter << (i + 1);
      buffer = RPG_COMMON_EVENT_DISPATCH_THREAD_NAME;
      buffer += ACE_TEXT_ALWAYS_CHAR(" #");
      buffer += converter.str();
      ACE_OS::memset(thread_name, 0, sizeof(thread_name));
      ACE_OS::strcpy(thread_name,
				             buffer.c_str());
      thread_names[i] = thread_name;
    } // end FOR
    groupID_out = ACE_Thread_Manager::instance()->spawn_n(numDispatchThreads_in,                     // # threads
                                                          ::tp_event_dispatcher_func,                // function
                                                          &const_cast<bool&>(useReactor_in),         // argument
                                                          (THR_NEW_LWP |
																													 THR_JOINABLE |
																													 THR_INHERIT_SCHED),                       // flags
                                                          ACE_DEFAULT_THREAD_PRIORITY,               // priority
                                                          RPG_COMMON_EVENT_DISPATCH_THREAD_GROUP_ID, // group id
                                                          NULL,                                      // task
                                                          thread_handles,                            // handle(s)
                                                          NULL,                                      // stack(s)
                                                          NULL,                                      // stack size(s)
                                                          thread_names);                             // name(s)
    if (groupID_out == -1)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_Thread_Manager::spawn_n(%u): \"%m\", aborting\n"),
                 numDispatchThreads_in));

      // clean up
			delete [] thread_handles;
			for (unsigned int i = 0; i < numDispatchThreads_in; i++)
        delete [] thread_names[i];
      delete [] thread_names;

      return false;
    } // end IF

    // clean up
		delete [] thread_handles;
		for (unsigned int i = 0; i < numDispatchThreads_in; i++)
      delete [] thread_names[i];
    delete [] thread_names;

    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("spawned %u event handlers (group ID: %d)...\n"),
               numDispatchThreads_in,
               groupID_out));
  } // end IF

  return true;
}

void
RPG_Net_Common_Tools::finiEventDispatch(const bool& stopReactor_in,
                                        const bool& stopProactor_in,
                                        const int& groupID_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Common_Tools::finiEventDispatch"));

  // step1: stop reactor/proactor
	// *IMPORTANT NOTE*: current proactor implementations start a pseudo-task that runs the
	// reactor --> stop that as well
  if (stopReactor_in || stopProactor_in)
    if (ACE_Reactor::instance()->end_event_loop() == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_Reactor::end_event_loop: \"%m\", continuing\n")));

  if (stopProactor_in)
    if (ACE_Proactor::instance()->end_event_loop() == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_Proactor::end_event_loop: \"%m\", continuing\n")));

  // step2: wait for any worker(s)
  if (groupID_in != -1)
    if (ACE_Thread_Manager::instance()->wait_grp(groupID_in) == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_Thread_Manager::wait_grp(%d): \"%m\", continuing\n"),
                 groupID_in));
}

std::string
RPG_Net_Common_Tools::IPAddress2String(const unsigned short& port_in,
                                       const unsigned int& IPAddress_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Common_Tools::IPAddress2String"));

  // init return value(s)
  std::string result;

  ACE_INET_Addr inet_addr;
  ACE_TCHAR buffer[32]; // "xxx.xxx.xxx.xxx:yyyyy\0"
  ACE_OS::memset(&buffer, 0, sizeof(buffer));
  if (inet_addr.set(port_in,
                    IPAddress_in,
                    0,  // no need to encode, data IS in network byte order !
                    0)) // only needed for IPv6...
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Inet_Addr::set: \"%m\", aborting\n")));

    return std::string(ACE_TEXT("invalid_IP_address"));
  }
  if (inet_addr.addr_to_string(buffer,
                               sizeof(buffer),
                               1)) // want IP address, not hostname !
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Inet_Addr::addr_to_string: \"%m\", aborting\n")));

    return std::string(ACE_TEXT("invalid_IP_address"));
  }

  // copy string from buffer
  result = buffer;

  // clean up: if port number was 0, we cut off the trailing ":0" !
  if (!port_in)
  {
    std::string::size_type last_colon_pos = result.find_last_of(':',
                                                                std::string::npos); // begin searching at the end !
    if (last_colon_pos != std::string::npos)
      result = result.substr(0, last_colon_pos);
  } // end IF

  return result;
}

std::string
RPG_Net_Common_Tools::IPProtocol2String(const unsigned char& protocol_in)
{
  RPG_TRACE("RPG_Net_Common_Tools::IPProtocol2String");

    // init return value(s)
  std::string result = ACE_TEXT("INVALID_PROTOCOL");

  switch (protocol_in)
  {
    case IPPROTO_IP: // OR case IPPROTO_HOPOPTS:
    {
      result = ACE_TEXT("IPPROTO_IP/IPPROTO_HOPOPTS");

      break;
    }
    case IPPROTO_ICMP:
    {
      result = ACE_TEXT("IPPROTO_ICMP");

      break;
    }
    case IPPROTO_IGMP:
    {
      result = ACE_TEXT("IPPROTO_IGMP");

      break;
    }
    case IPPROTO_IPIP:
    {
      result = ACE_TEXT("IPPROTO_IPIP");

      break;
    }
    case IPPROTO_TCP:
    {
      result = ACE_TEXT("IPPROTO_TCP");

      break;
    }
    case IPPROTO_EGP:
    {
      result = ACE_TEXT("IPPROTO_EGP");

      break;
    }
    case IPPROTO_PUP:
    {
      result = ACE_TEXT("IPPROTO_PUP");

      break;
    }
    case IPPROTO_UDP:
    {
      result = ACE_TEXT("IPPROTO_UDP");

      break;
    }
    case IPPROTO_IDP:
    {
      result = ACE_TEXT("IPPROTO_IDP");

      break;
    }
    case IPPROTO_TP:
    {
      result = ACE_TEXT("IPPROTO_TP");

      break;
    }
    case IPPROTO_IPV6:
    {
      result = ACE_TEXT("IPPROTO_IPV6");

      break;
    }
    case IPPROTO_ROUTING:
    {
      result = ACE_TEXT("IPPROTO_ROUTING");

      break;
    }
    case IPPROTO_FRAGMENT:
    {
      result = ACE_TEXT("IPPROTO_FRAGMENT");

      break;
    }
    case IPPROTO_RSVP:
    {
      result = ACE_TEXT("IPPROTO_RSVP");

      break;
    }
    case IPPROTO_GRE:
    {
      result = ACE_TEXT("IPPROTO_GRE");

      break;
    }
    case IPPROTO_ESP:
    {
      result = ACE_TEXT("IPPROTO_ESP");

      break;
    }
    case IPPROTO_AH:
    {
      result = ACE_TEXT("IPPROTO_AH");

      break;
    }
    case IPPROTO_ICMPV6:
    {
      result = ACE_TEXT("IPPROTO_ICMPV6");

      break;
    }
    case IPPROTO_NONE:
    {
      result = ACE_TEXT("IPPROTO_NONE");

      break;
    }
    case IPPROTO_DSTOPTS:
    {
      result = ACE_TEXT("IPPROTO_DSTOPTS");

      break;
    }
    case IPPROTO_MTP:
    {
      result = ACE_TEXT("IPPROTO_MTP");

      break;
    }
    case IPPROTO_ENCAP:
    {
      result = ACE_TEXT("IPPROTO_ENCAP");

      break;
    }
    case IPPROTO_PIM:
    {
      result = ACE_TEXT("IPPROTO_PIM");

      break;
    }
    case IPPROTO_COMP:
    {
      result = ACE_TEXT("IPPROTO_COMP");

      break;
    }
    case IPPROTO_SCTP:
    {
      result = ACE_TEXT("IPPROTO_SCTP");

      break;
    }
    case IPPROTO_RAW:
    {
      result = ACE_TEXT("IPPROTO_RAW");

      break;
    }
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid/unknown IP protocol: %1u, continuing\n"),
                 protocol_in));

      break;
    }
  } // end SWITCH

  return result;
}

std::string
RPG_Net_Common_Tools::MACAddress2String(const char* const addressDataPtr_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Common_Tools::MACAddress2String"));

  // init return value(s)
  std::string result;

  // convert 6 bytes to ASCII
  // *IMPORTANT NOTE*: ether_ntoa_r is not portable...
  // *TODO*: ether_ntoa_r is not portable...
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
  // *TODO*: make this thread-safe !
  static char address[(ETH_ALEN * 2) + ETH_ALEN + 1]; // "ab:cd:ef:gh:ij:kl\0"
  ACE_OS::memset(&address,
                 0,
                 sizeof(address));
  if (::ether_ntoa_r(reinterpret_cast<const ether_addr*> (addressDataPtr_in),
                     address) != address)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ::ether_ntoa_r(): \"%s\", returning\n"),
               ACE_OS::strerror(ACE_OS::last_error())));

    return std::string(ACE_TEXT("invalid_FDDI_address"));
  } // end IF

  result = address;
#else
  // *TODO*: implement this
  ACE_ASSERT(false);
#endif

  return result;
}

std::string
RPG_Net_Common_Tools::EthernetProtocolTypeID2String(const unsigned short& frameType_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Common_Tools::EthernetProtocolTypeID2String"));

  // init return value(s)
  std::string result = ACE_TEXT("UNKNOWN_ETHERNET_FRAME_TYPE");

  switch (ACE_NTOHS(frameType_in))
  {
    case ETH_P_LOOP:
    {
      result = ACE_TEXT("ETH_P_LOOP");

      break;
    }
    case ETHERTYPE_GRE_ISO:
    {
      result = ACE_TEXT("ETHERTYPE_GRE_ISO");

      break;
    }
    case ETH_P_PUP:
    {
      result = ACE_TEXT("ETH_P_PUP");

      break;
    }
    case ETH_P_PUPAT:
    {
      result = ACE_TEXT("ETH_P_PUPAT");

      break;
    }
    case ETHERTYPE_SPRITE:
    {
      result = ACE_TEXT("ETHERTYPE_SPRITE");

      break;
    }
    case ETH_P_IP:
    {
      result = ACE_TEXT("ETH_P_IP");

      break;
    }
    case ETH_P_X25:
    {
      result = ACE_TEXT("ETH_P_X25");

      break;
    }
    case ETH_P_ARP:
    {
      result = ACE_TEXT("ETH_P_ARP");

      break;
    }
    case ETH_P_BPQ:
    {
      result = ACE_TEXT("ETH_P_BPQ");

      break;
    }
    case ETH_P_IEEEPUP:
    {
      result = ACE_TEXT("ETH_P_IEEEPUP");

      break;
    }
    case ETH_P_IEEEPUPAT:
    {
      result = ACE_TEXT("ETH_P_IEEEPUPAT");

      break;
    }
    case ETHERTYPE_NS:
    {
      result = ACE_TEXT("ETHERTYPE_NS");

      break;
    }
    case ETHERTYPE_TRAIL:
    {
      result = ACE_TEXT("ETHERTYPE_TRAIL");

      break;
    }
    case ETH_P_DEC:
    {
      result = ACE_TEXT("ETH_P_DEC");

      break;
    }
    case ETH_P_DNA_DL:
    {
      result = ACE_TEXT("ETH_P_DNA_DL");

      break;
    }
    case ETH_P_DNA_RC:
    {
      result = ACE_TEXT("ETH_P_DNA_RC");

      break;
    }
    case ETH_P_DNA_RT:
    {
      result = ACE_TEXT("ETH_P_DNA_RT");

      break;
    }
    case ETH_P_LAT:
    {
      result = ACE_TEXT("ETH_P_LAT");

      break;
    }
    case ETH_P_DIAG:
    {
      result = ACE_TEXT("ETH_P_DIAG");

      break;
    }
    case ETH_P_CUST:
    {
      result = ACE_TEXT("ETH_P_CUST");

      break;
    }
    case ETH_P_SCA:
    {
      result = ACE_TEXT("ETH_P_SCA");

      break;
    }
    case ETH_P_RARP:
    {
      result = ACE_TEXT("ETH_P_RARP");

      break;
    }
    case ETHERTYPE_LANBRIDGE:
    {
      result = ACE_TEXT("ETHERTYPE_LANBRIDGE");

      break;
    }
    case ETHERTYPE_DECDNS:
    {
      result = ACE_TEXT("ETHERTYPE_DECDNS");

      break;
    }
    case ETHERTYPE_DECDTS:
    {
      result = ACE_TEXT("ETHERTYPE_DECDTS");

      break;
    }
    case ETHERTYPE_VEXP:
    {
      result = ACE_TEXT("ETHERTYPE_VEXP");

      break;
    }
    case ETHERTYPE_VPROD:
    {
      result = ACE_TEXT("ETHERTYPE_VPROD");

      break;
    }
    case ETH_P_ATALK:
    {
      result = ACE_TEXT("ETH_P_ATALK");

      break;
    }
    case ETH_P_AARP:
    {
      result = ACE_TEXT("ETH_P_AARP");

      break;
    }
    case ETH_P_8021Q:
    {
      result = ACE_TEXT("ETH_P_8021Q");

      break;
    }
    case ETH_P_IPX:
    {
      result = ACE_TEXT("ETH_P_IPX");

      break;
    }
    case ETH_P_IPV6:
    {
      result = ACE_TEXT("ETH_P_IPV6");

      break;
    }
    case ETHERTYPE_MPCP:
    {
      result = ACE_TEXT("ETHERTYPE_MPCP");

      break;
    }
    case ETHERTYPE_SLOW:
    {
      result = ACE_TEXT("ETHERTYPE_SLOW");

      break;
    }
    case ETHERTYPE_PPP:
    {
      result = ACE_TEXT("ETHERTYPE_PPP");

      break;
    }
    case ETH_P_WCCP:
    {
      result = ACE_TEXT("ETH_P_WCCP");

      break;
    }
    case ETH_P_MPLS_UC:
    {
      result = ACE_TEXT("ETH_P_MPLS_UC");

      break;
    }
    case ETH_P_MPLS_MC:
    {
      result = ACE_TEXT("ETH_P_MPLS_MC");

      break;
    }
    case ETH_P_ATMMPOA:
    {
      result = ACE_TEXT("ETH_P_ATMMPOA");

      break;
    }
    case ETH_P_PPP_DISC:
    {
      result = ACE_TEXT("ETH_P_PPP_DISC");

      break;
    }
    case ETH_P_PPP_SES:
    {
      result = ACE_TEXT("ETH_P_PPP_SES");

      break;
    }
    case ETHERTYPE_JUMBO:
    {
      result = ACE_TEXT("ETHERTYPE_JUMBO");

      break;
    }
    case ETH_P_ATMFATE:
    {
      result = ACE_TEXT("ETH_P_ATMFATE");

      break;
    }
    case ETHERTYPE_EAPOL:
    {
      result = ACE_TEXT("ETHERTYPE_EAPOL");

      break;
    }
    case ETH_P_AOE:
    {
      result = ACE_TEXT("ETH_P_AOE");

      break;
    }
    case ETH_P_TIPC:
    {
      result = ACE_TEXT("ETH_P_TIPC");

      break;
    }
    case ETHERTYPE_LOOPBACK:
    {
      result = ACE_TEXT("ETHERTYPE_LOOPBACK");

      break;
    }
    case ETHERTYPE_VMAN:
    {
      result = ACE_TEXT("ETHERTYPE_VMAN");

      break;
    }
    case ETHERTYPE_ISO:
    {
      result = ACE_TEXT("ETHERTYPE_ISO");

      break;
    }
// ********************* Non DIX types ***********************
    case ETH_P_802_3:
    {
      result = ACE_TEXT("ETH_P_802_3");

      break;
    }
    case ETH_P_AX25:
    {
      result = ACE_TEXT("ETH_P_AX25");

      break;
    }
    case ETH_P_ALL:
    {
      result = ACE_TEXT("ETH_P_ALL");

      break;
    }
    case ETH_P_802_2:
    {
      result = ACE_TEXT("ETH_P_802_2");

      break;
    }
    case ETH_P_SNAP:
    {
      result = ACE_TEXT("ETH_P_SNAP");

      break;
    }
    case ETH_P_DDCMP:
    {
      result = ACE_TEXT("ETH_P_DDCMP");

      break;
    }
    case ETH_P_WAN_PPP:
    {
      result = ACE_TEXT("ETH_P_WAN_PPP");

      break;
    }
    case ETH_P_PPP_MP:
    {
      result = ACE_TEXT("ETH_P_PPP_MP");

      break;
    }
    case ETH_P_LOCALTALK:
    {
      result = ACE_TEXT("ETH_P_LOCALTALK");

      break;
    }
    case ETH_P_PPPTALK:
    {
      result = ACE_TEXT("ETH_P_PPPTALK");

      break;
    }
    case ETH_P_TR_802_2:
    {
      result = ACE_TEXT("ETH_P_TR_802_2");

      break;
    }
    case ETH_P_MOBITEX:
    {
      result = ACE_TEXT("ETH_P_MOBITEX");

      break;
    }
    case ETH_P_CONTROL:
    {
      result = ACE_TEXT("ETH_P_CONTROL");

      break;
    }
    case ETH_P_IRDA:
    {
      result = ACE_TEXT("ETH_P_IRDA");

      break;
    }
    case ETH_P_ECONET:
    {
      result = ACE_TEXT("ETH_P_ECONET");

      break;
    }
    case ETH_P_HDLC:
    {
      result = ACE_TEXT("ETH_P_HDLC");

      break;
    }
    case ETH_P_ARCNET:
    {
      result = ACE_TEXT("ETH_P_ARCNET");

      break;
    }
    default:
    {
      ACE_DEBUG((LM_WARNING,
                 ACE_TEXT("invalid/unknown ethernet frame type: \"0x%x\", continuing\n"),
                 ACE_NTOHS(frameType_in)));

      // IEEE 802.3 ? --> change result string...
      if (ACE_NTOHS(frameType_in) <= ETH_DATA_LEN)
      {
        result = ACE_TEXT("UNKNOWN_IEEE_802_3_FRAME_TYPE");
      } // end IF

      break;
    }
  } // end SWITCH

  return result;
}

// bool
// RPG_Net_Common_Tools::selectNetworkInterface(const std::string& defaultInterfaceIdentifier_in,
//                                              std::string& interfaceIdentifier_out)
// {
//   RPG_TRACE(ACE_TEXT("RPG_Net_Common_Tools::selectNetworkInterface"));
//
//   // init return value(s)
//   interfaceIdentifier_out.resize(0);
//
//   pcap_if_t* all_devices = NULL;
//   char errbuf[PCAP_ERRBUF_SIZE]; // defined in pcap.h
//   if (pcap_findalldevs(&all_devices,
//                        errbuf) == -1)
//   {
//     ACE_DEBUG((LM_ERROR,
//                ACE_TEXT("failed to pcap_findalldevs(): \"%s\", aborting\n"),
//                errbuf));
//
//     return false;
//   } // end IF
//
//   // found default interface ?
//   unsigned long i = 1;
//   bool found_default = false;
//   pcap_if_t* device = NULL;
//   for (device = all_devices;
//        device;
//        device = device->next, i++)
//   {
//     // debug info
//     ACE_DEBUG((LM_INFO,
//                ACE_TEXT("#%u \"%s\": \"%s\"\n"),
//                i,
//                device->name,
//                (device->description ?
//                 device->description :
//                 ACE_TEXT("no description available"))));
//
//     if (defaultInterfaceIdentifier_in == std::string(device->name))
//     {
//       interfaceIdentifier_out = defaultInterfaceIdentifier_in;
//       found_default = true;
//     } // end IF
//   } // end FOR
//   i--;
//
//   // sanity check: found any suitable device at all ?
//   if (!all_devices)
//   {
//     ACE_DEBUG((LM_ERROR,
//                ACE_TEXT("no interfaces found, aborting\n")));
//
//     // clean up
//     pcap_freealldevs(all_devices);
//
//     return false;
//   } // end IF
//
//   // couldn't find default interface ? ask user !
//   if (!found_default)
//   {
//     unsigned long device_number = 0;
//     std::cout << ACE_TEXT("default interface \"")
//               << defaultInterfaceIdentifier_in.c_str()
//               << ACE_TEXT("\" not found, please enter a valid interface number (1-")
//               << i
//               << ACE_TEXT("): ");
//     std::cin >> device_number;
//
//     // sanity check: out of range ?
//     if ((device_number < 1) ||
//          (device_number > i))
//     {
//       ACE_DEBUG((LM_ERROR,
//                  ACE_TEXT("selection: %u was out of range, aborting\n"),
//                  device_number));
//
//       // clean up
//       pcap_freealldevs(all_devices);
//
//       return false;
//     } // end IF
//
//     // get selected device name
//     for (device = all_devices, i = 0;
//          i < (device_number - 1);
//          device = device->next, i++);
//
//     interfaceIdentifier_out = device->name;
//   } // end IF
//
//   // clean up
//   pcap_freealldevs(all_devices);
//
//   return true;
// }

bool
RPG_Net_Common_Tools::retrieveLocalIPAddress(const std::string& interfaceIdentifier_in,
                                             std::string& IPaddress_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Common_Tools::retrieveLocalIPAddress"));

  // init return value(s)
  IPaddress_out.resize(0);

//   // validate/retrieve interface identifier index
//   unsigned long index = 0;
//   // *PORTABILITY*
//   index = ACE_OS::if_nametoindex(interfaceIdentifier_in.c_str());
//   if (!index)
//   {
//     ACE_DEBUG((LM_ERROR,
//                ACE_TEXT("failed to ACE_OS::if_nametoindex(\"%s\"): \"%s\", aborting\n"),
//                interfaceIdentifier_in.c_str(),
//                ACE_OS::strerror(ACE_OS::last_error())));
  //
//     return false;
//   } // end IF

  // *NOTE*: we ask the kernel...
  size_t count = 0;
  ACE_INET_Addr* addr_array = NULL;
  if (ACE::get_ip_interfaces(count,
                             addr_array))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE::get_ip_interfaces(): \"%s\", aborting\n"),
               ACE_OS::strerror(ACE_OS::last_error())));

    return false;
  } // end IF
  if (count == 0)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("could not find any IP network interfaces, aborting\n")));

    return false;
  } // end IF

  // *NOTE*: beyond this point, we need to clean up addr_array...

  ACE_TCHAR ip[MAXHOSTNAMELEN + 1];
  ACE_OS::memset(&ip,
                 0,
                 sizeof(ip));
//   sockaddr_in* addr_handle = NULL;
  for (size_t i = 0;
       i < count;
       i++)
  {
    // reset buffer
    ACE_OS::memset(&ip, 0, sizeof(ip));

    // reset address handle
//     addr_handle = NULL;

    // sanity check: only support IPv4 (for now)
    if (addr_array[i].get_type() != AF_INET)
    {
      // try next one...
      continue;
    } // end IF

//     // sanity check: ignore loopback
//     if (addr_array[i].is_loopback())
//     {
//       // try next one...
//       continue;
//     } // end IF

    if (addr_array[i].get_host_addr(ip,
                                    MAXHOSTNAMELEN) == NULL)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_INET_Addr::get_host_addr(): \"%s\", aborting\n"),
                 ACE_OS::strerror(ACE_OS::last_error())));

      // clean up
      delete[] addr_array;
      addr_array = NULL;

      return false;
    } // end IF

    // debug info
    // *TODO*: sanity check: correct interface ?
    // --> how can we find out which interface this address is assigned to ?
//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("found host address: \"%s\"...\n"),
//                ip));

//     addr_handle = static_cast<sockaddr_in*> (// addr_array[i].get_addr());

    IPaddress_out = ip;
  } // end FOR

  // clean up
  delete[] addr_array;
  addr_array = NULL;

  return true;
}

bool
RPG_Net_Common_Tools::retrieveLocalHostname(std::string& hostname_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Common_Tools::retrieveLocalHostname"));

  // init return value(s)
  hostname_out.resize(0);

  ACE_TCHAR host[MAXHOSTNAMELEN + 1];
  ACE_OS::memset(&host, 0, MAXHOSTNAMELEN + 1);
  if (ACE_OS::hostname(host, MAXHOSTNAMELEN))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_OS::hostname(): \"%m\", aborting\n")));

    return false;
  } // end IF

  hostname_out = host;

  return true;
}

bool
RPG_Net_Common_Tools::setSocketBuffer(const ACE_HANDLE& handle_in,
                                      const int& optname_in,
                                      const int& size_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Common_Tools::setSocketBuffer"));

  // sanity check
  if ((optname_in != SO_RCVBUF) &&
      (optname_in != SO_SNDBUF))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid socket option (was: %d), aborting\n"),
               optname_in));

    return false;
  } // end IF

  int optval = size_in;
  int optlen = sizeof(optval);
  if (ACE_OS::setsockopt(handle_in,
                         SOL_SOCKET,
                         optname_in,
                         reinterpret_cast<const char*>(&optval),
                         optlen))
  {
    // *PORTABILITY*
#if defined(ACE_WIN32) || defined(ACE_WIN64)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_OS::setsockopt(%@, %s): \"%m\", aborting\n"),
               handle_in,
               ((optname_in == SO_SNDBUF) ? ACE_TEXT("SO_SNDBUF") : ACE_TEXT("SO_RCVBUF"))));
#else
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_OS::setsockopt(%d, %s): \"%m\", aborting\n"),
               handle_in,
               ((optname_in == SO_SNDBUF) ? ACE_TEXT("SO_SNDBUF") : ACE_TEXT("SO_RCVBUF"))));
#endif

    return false;
  } // end IF

  // validate result
  optval = 0;
  if (ACE_OS::getsockopt(handle_in,
                         SOL_SOCKET,
                         optname_in,
                         reinterpret_cast<char*>(&optval),
                         &optlen))
  {
    // *PORTABILITY*
#if defined(ACE_WIN32) || defined(ACE_WIN64)
		ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_OS::getsockopt(%@, %s): \"%m\", aborting\n"),
               handle_in,
               ((optname_in == SO_SNDBUF) ? ACE_TEXT("SO_SNDBUF") : ACE_TEXT("SO_RCVBUF"))));
#else
		ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_OS::getsockopt(%d, %s): \"%m\", aborting\n"),
               handle_in,
               ((optname_in == SO_SNDBUF) ? ACE_TEXT("SO_SNDBUF") : ACE_TEXT("SO_RCVBUF"))));
#endif

    return false;
  } // end IF

  if (optval != size_in)
  {
    // *NOTE*: for some reason, Linux will actually set TWICE the size value...
    if (RPG_Common_Tools::isLinux() && (optval == (size_in * 2)))
      return true;

    // *PORTABILITY*
#if defined(ACE_WIN32) || defined(ACE_WIN64)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("ACE_OS::getsockopt(%@, %s) returned %d (expected: %d), aborting\n"),
               handle_in,
               ((optname_in == SO_SNDBUF) ? ACE_TEXT("SO_SNDBUF") : ACE_TEXT("SO_RCVBUF")),
               optval,
               size_in));
#else
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("ACE_OS::getsockopt(%d, %s) returned %d (expected: %d), aborting\n"),
               handle_in,
               ((optname_in == SO_SNDBUF) ? ACE_TEXT("SO_SNDBUF") : ACE_TEXT("SO_RCVBUF")),
               optval,
               size_in));
#endif

    return false;
  } // end IF

  //ACE_DEBUG((LM_DEBUG,
  //           ACE_TEXT("set \"%s\" option of socket %d to: %d\n"),
  //           ((optname_in == SO_RCVBUF) ? ACE_TEXT("SO_RCVBUF")
  //                                      : ACE_TEXT("SO_SNDBUF")),
  //           handle_in,
  //           size_in));

  return true;
}

bool
RPG_Net_Common_Tools::setNoDelay(const ACE_HANDLE& handle_in,
                                 const bool& noDelay_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Common_Tools::setNoDelay"));

  int optval = (noDelay_in ? 1 : 0);
  int optlen = sizeof(optval);
  if (ACE_OS::setsockopt(handle_in,
                         IPPROTO_TCP,
                         TCP_NODELAY,
                         reinterpret_cast<const char*>(&optval),
                         optlen))
  {
    // *PORTABILITY*
#if defined(ACE_WIN32) || defined(ACE_WIN64)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_OS::setsockopt(%@, TCP_NODELAY): \"%m\", aborting\n"),
               handle_in));
#else
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_OS::setsockopt(%d, TCP_NODELAY): \"%m\", aborting\n"),
               handle_in));
#endif

    return false;
  } // end IF

  // validate result
  optval = 0;
  if (ACE_OS::getsockopt(handle_in,
                         IPPROTO_TCP,
                         TCP_NODELAY,
                         reinterpret_cast<char*>(&optval),
                         &optlen))
  {
    // *PORTABILITY*
#if defined(ACE_WIN32) || defined(ACE_WIN64)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_OS::getsockopt(%@, TCP_NODELAY): \"%m\", aborting\n"),
               handle_in));
#else
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_OS::getsockopt(%d, TCP_NODELAY): \"%m\", aborting\n"),
               handle_in));
#endif

    return false;
  } // end IF

  //ACE_DEBUG((LM_DEBUG,
  //           ACE_TEXT("setsockopt(%d, TCP_NODELAY): %s\n"),
  //           handle_in,
  //           (noDelay_in ? ((optval == 1) ? "on" : "off")
  //                       : ((optval == 0) ? "off" : "on"))));

  return (noDelay_in ? (optval == 1) : (optval == 0));
}

bool
RPG_Net_Common_Tools::setKeepAlive(const ACE_HANDLE& handle_in,
                                   const bool& keepAlive_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Common_Tools::setKeepAlive"));

  int optval = (keepAlive_in ? 1 : 0);
  int optlen = sizeof(optval);
  if (ACE_OS::setsockopt(handle_in,
                         SOL_SOCKET,
                         SO_KEEPALIVE,
                         reinterpret_cast<const char*>(&optval),
                         optlen))
  {
    // *PORTABILITY*
#if defined(ACE_WIN32) || defined(ACE_WIN64)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_OS::setsockopt(%@, SO_KEEPALIVE): \"%m\", aborting\n"),
               handle_in));
#else
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_OS::setsockopt(%d, SO_KEEPALIVE): \"%m\", aborting\n"),
               handle_in));
#endif

    return false;
  } // end IF

  // validate result
  optval = 0;
  if (ACE_OS::getsockopt(handle_in,
                         SOL_SOCKET,
                         SO_KEEPALIVE,
                         reinterpret_cast<char*>(&optval),
                         &optlen))
  {
    // *PORTABILITY*
#if defined(ACE_WIN32) || defined(ACE_WIN64)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_OS::getsockopt(%@, SO_KEEPALIVE): \"%m\", aborting\n"),
               handle_in));
#else
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_OS::getsockopt(%d, SO_KEEPALIVE): \"%m\", aborting\n"),
               handle_in));
#endif

    return false;
  } // end IF

  //ACE_DEBUG((LM_DEBUG,
  //           ACE_TEXT("setsockopt(%d, SO_KEEPALIVE): %s\n"),
  //           handle_in,
  //           (keepAlive_in ? ((optval == 1) ? "on" : "off")
  //                         : ((optval == 0) ? "off" : "on"))));

  return (keepAlive_in ? (optval == 1) : (optval == 0));
}

bool
RPG_Net_Common_Tools::setLinger(const ACE_HANDLE& handle_in,
                                const unsigned int& seconds_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Common_Tools::setLinger"));

  linger optval;
  optval.l_onoff = ((seconds_in > 0) ? 1 : 0);
  optval.l_linger = static_cast<int>(seconds_in);
  int optlen = sizeof(optval);
  if (ACE_OS::setsockopt(handle_in,
                         SOL_SOCKET,
                         SO_LINGER,
                         reinterpret_cast<const char*>(&optval),
                         optlen))
  {
    // *PORTABILITY*
#if defined(ACE_WIN32) || defined(ACE_WIN64)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_OS::setsockopt(%@, SO_LINGER): \"%m\", aborting\n"),
                 handle_in));
#else
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE_OS::setsockopt(%d, SO_LINGER): \"%m\", aborting\n"),
                 handle_in));
#endif

    return false;
  } // end IF

  // validate result
  ACE_OS::memset(&optval, 0, sizeof(optval));
  if (ACE_OS::getsockopt(handle_in,
                         SOL_SOCKET,
                         SO_LINGER,
                         reinterpret_cast<char*>(&optval),
                         &optlen))
  {
    // *PORTABILITY*
#if defined(ACE_WIN32) || defined(ACE_WIN64)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_OS::getsockopt(%@, SO_LINGER): \"%m\", aborting\n"),
               handle_in));
#else
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_OS::getsockopt(%d, SO_LINGER): \"%m\", aborting\n"),
               handle_in));
#endif

    return false;
  } // end IF

  //ACE_DEBUG((LM_DEBUG,
  //           ACE_TEXT("setsockopt(%d, SO_LINGER): %s\n"),
  //           handle_in,
  //           ((seconds_in > 0) ? ((optval.l_onoff == 1) ? "on" : "off")
  //                             : ((optval.l_onoff == 0) ? "off" : "on"))));

  return ((seconds_in > 0) ? ((optval.l_onoff == 1) &&
                              (optval.l_linger == static_cast<int>(seconds_in)))
                           : (optval.l_onoff == 0));
}

void
RPG_Net_Common_Tools::retrieveSignalInfo(const int& signal_in,
                                         const siginfo_t& info_in,
                                         const ucontext_t* context_in,
                                         std::string& information_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Net_Common_Tools::retrieveSignalInfo"));

  // init return value
  information_out.resize(0);

  std::ostringstream information;
#if !defined(ACE_WIN32) && !defined(ACE_WIN64)
  // step0: common information (on POSIX.1b)
  information << ACE_TEXT("PID/UID: ");
  information << info_in.si_pid;
  information << ACE_TEXT("/");
  information << info_in.si_uid;

  // (try to) get user name
  char pw_buf[BUFSIZ];
  passwd pw_struct;
  passwd* pw_ptr = NULL;
  // *PORTABILITY*: this isn't completely portable... (man getpwuid_r)
  if (::getpwuid_r(info_in.si_uid,
                   &pw_struct,
                   pw_buf,
                   sizeof(pw_buf),
                   &pw_ptr))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ::getpwuid_r(%d) : \"%m\", continuing\n"),
               info_in.si_uid));
  } // end IF
  else
  {
    information << ACE_TEXT("[\"");
    information << pw_struct.pw_name;
    information << ACE_TEXT("\"]");
  } // end ELSE

  // "si_signo,  si_errno  and  si_code are defined for all signals..."
  information << ACE_TEXT(", errno: ");
  information << info_in.si_errno;
  information << ACE_TEXT("[\"");
  information << ACE_OS::strerror(info_in.si_errno);
  information << ACE_TEXT("\"], code: ");

  // step1: retrieve signal code...
  switch (info_in.si_code)
  {
    case SI_USER:
      information << ACE_TEXT("SI_USER"); break;
    case SI_KERNEL:
      information << ACE_TEXT("SI_KERNEL"); break;
    case SI_QUEUE:
      information << ACE_TEXT("SI_QUEUE"); break;
    case SI_TIMER:
      information << ACE_TEXT("SI_TIMER"); break;
    case SI_MESGQ:
      information << ACE_TEXT("SI_MESGQ"); break;
    case SI_ASYNCIO:
      information << ACE_TEXT("SI_ASYNCIO"); break;
    case SI_SIGIO:
      information << ACE_TEXT("SI_SIGIO"); break;
    case SI_TKILL:
      information << ACE_TEXT("SI_TKILL"); break;
    default:
    { // (signal-dependant) codes...
      switch (signal_in)
      {
        case SIGILL:
        {
          switch (info_in.si_code)
          {
            case ILL_ILLOPC:
              information << ACE_TEXT("ILL_ILLOPC"); break;
            case ILL_ILLOPN:
              information << ACE_TEXT("ILL_ILLOPN"); break;
            case ILL_ILLADR:
              information << ACE_TEXT("ILL_ILLADR"); break;
            case ILL_ILLTRP:
              information << ACE_TEXT("ILL_ILLTRP"); break;
            case ILL_PRVOPC:
              information << ACE_TEXT("ILL_PRVOPC"); break;
            case ILL_PRVREG:
              information << ACE_TEXT("ILL_PRVREG"); break;
            case ILL_COPROC:
              information << ACE_TEXT("ILL_COPROC"); break;
            case ILL_BADSTK:
              information << ACE_TEXT("ILL_BADSTK"); break;
            default:
            {
              ACE_DEBUG((LM_DEBUG,
                         ACE_TEXT("invalid/unknown signal code: %d, continuing\n"),
                         info_in.si_code));

              break;
            }
          } // end SWITCH

          break;
        }
        case SIGFPE:
        {
          switch (info_in.si_code)
          {
            case FPE_INTDIV:
              information << ACE_TEXT("FPE_INTDIV"); break;
            case FPE_INTOVF:
              information << ACE_TEXT("FPE_INTOVF"); break;
            case FPE_FLTDIV:
              information << ACE_TEXT("FPE_FLTDIV"); break;
            case FPE_FLTOVF:
              information << ACE_TEXT("FPE_FLTOVF"); break;
            case FPE_FLTUND:
              information << ACE_TEXT("FPE_FLTUND"); break;
            case FPE_FLTRES:
              information << ACE_TEXT("FPE_FLTRES"); break;
            case FPE_FLTINV:
              information << ACE_TEXT("FPE_FLTINV"); break;
            case FPE_FLTSUB:
              information << ACE_TEXT("FPE_FLTSUB"); break;
            default:
            {
              ACE_DEBUG((LM_DEBUG,
                         ACE_TEXT("invalid/unknown signal code: %d, continuing\n"),
                         info_in.si_code));

              break;
            }
          } // end SWITCH

          break;
        }
        case SIGSEGV:
        {
          switch (info_in.si_code)
          {
            case SEGV_MAPERR:
              information << ACE_TEXT("SEGV_MAPERR"); break;
            case SEGV_ACCERR:
              information << ACE_TEXT("SEGV_ACCERR"); break;
            default:
            {
              ACE_DEBUG((LM_DEBUG,
                         ACE_TEXT("invalid/unknown signal code: %d, continuing\n"),
                         info_in.si_code));

              break;
            }
          } // end SWITCH

          break;
        }
        case SIGBUS:
        {
          switch (info_in.si_code)
          {
            case BUS_ADRALN:
              information << ACE_TEXT("BUS_ADRALN"); break;
            case BUS_ADRERR:
              information << ACE_TEXT("BUS_ADRERR"); break;
            case BUS_OBJERR:
              information << ACE_TEXT("BUS_OBJERR"); break;
            default:
            {
              ACE_DEBUG((LM_DEBUG,
                         ACE_TEXT("invalid/unknown signal code: %d, continuing\n"),
                         info_in.si_code));

              break;
            }
          } // end SWITCH

          break;
        }
        case SIGTRAP:
        {
          switch (info_in.si_code)
          {
            case TRAP_BRKPT:
              information << ACE_TEXT("TRAP_BRKPT"); break;
            case TRAP_TRACE:
              information << ACE_TEXT("TRAP_TRACE"); break;
            default:
            {
              ACE_DEBUG((LM_DEBUG,
                         ACE_TEXT("invalid/unknown signal code: %d, continuing\n"),
                         info_in.si_code));

              break;
            }
          } // end SWITCH

          break;
        }
        case SIGCHLD:
        {
          switch (info_in.si_code)
          {
            case CLD_EXITED:
              information << ACE_TEXT("CLD_EXITED"); break;
            case CLD_KILLED:
              information << ACE_TEXT("CLD_KILLED"); break;
            case CLD_DUMPED:
              information << ACE_TEXT("CLD_DUMPED"); break;
            case CLD_TRAPPED:
              information << ACE_TEXT("CLD_TRAPPED"); break;
            case CLD_STOPPED:
              information << ACE_TEXT("CLD_STOPPED"); break;
            case CLD_CONTINUED:
              information << ACE_TEXT("CLD_CONTINUED"); break;
            default:
            {
              ACE_DEBUG((LM_DEBUG,
                         ACE_TEXT("invalid/unknown signal code: %d, continuing\n"),
                         info_in.si_code));

              break;
            }
          } // end SWITCH

          break;
        }
        case SIGPOLL:
        {
          switch (info_in.si_code)
          {
            case POLL_IN:
              information << ACE_TEXT("POLL_IN"); break;
            case POLL_OUT:
              information << ACE_TEXT("POLL_OUT"); break;
            case POLL_MSG:
              information << ACE_TEXT("POLL_MSG"); break;
            case POLL_ERR:
              information << ACE_TEXT("POLL_ERR"); break;
            case POLL_PRI:
              information << ACE_TEXT("POLL_PRI"); break;
            case POLL_HUP:
              information << ACE_TEXT("POLL_HUP"); break;
            default:
            {
              ACE_DEBUG((LM_DEBUG,
                         ACE_TEXT("invalid/unknown signal code: %d, continuing\n"),
                         info_in.si_code));

              break;
            }
          } // end SWITCH

          break;
        }
        default:
        {
          ACE_DEBUG((LM_DEBUG,
                     ACE_TEXT("invalid/unknown signal code: %d, continuing\n"),
                     info_in.si_code));

          break;
        }
      } // end SWITCH

      break;
    }
  } // end SWITCH

  // step2: retrieve more (signal-dependant) information
  switch (signal_in)
  {
    case SIGALRM:
    {
      information << ACE_TEXT(", overrun: ");
      information << info_in.si_overrun;
      information << ACE_TEXT(", (internal) id: ");
      information << info_in.si_timerid;

      break;
    }
    case SIGCHLD:
    {
      information << ACE_TEXT(", (exit) status: ");
      information << info_in.si_status;
      information << ACE_TEXT(", time consumed (user): ");
      information << info_in.si_utime;
      information << ACE_TEXT(" / (system): ");
      information << info_in.si_stime;

      break;
    }
    case SIGILL:
    case SIGFPE:
    case SIGSEGV:
    case SIGBUS:
    {
      // *TODO*: more data ?
      information << ACE_TEXT(", fault at address: ");
      information << info_in.si_addr;

      break;
    }
    case SIGPOLL:
    {
      information << ACE_TEXT(", band event: ");
      information << info_in.si_band;
      information << ACE_TEXT(", (file) descriptor: ");
      information << info_in.si_fd;

      break;
    }
    default:
    {
      // *TODO*: handle more signals here ?
//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("no additional information for signal: \"%S\"...\n"),
//                  signal_in));

      break;
    }
  } // end SWITCH
#else
  switch (signal_in)
  {
    case SIGINT:
      information << ACE_TEXT("SIGINT"); break;
    case SIGILL:
      information << ACE_TEXT("SIGILL"); break;
    case SIGFPE:
      information << ACE_TEXT("SIGFPE"); break;
    case SIGSEGV:
      information << ACE_TEXT("SIGSEGV"); break;
    case SIGTERM:
      information << ACE_TEXT("SIGTERM"); break;
    case SIGBREAK:
      information << ACE_TEXT("SIGBREAK"); break;
    case SIGABRT:
      information << ACE_TEXT("SIGABRT"); break;
    case SIGABRT_COMPAT:
      information << ACE_TEXT("SIGABRT_COMPAT"); break;
    default:
	  {
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("invalid/unknown signal: %S, continuing\n"),
                 signal_in));

			break;
		}
	} // end SWITCH

  information << ACE_TEXT(", signalled handle: ");
  information << info_in.si_handle_;
  //information << ACE_TEXT(", array of signalled handle(s): ");
  //information << info_in.si_handles_;
#endif

  // OK: set return value
  information_out = information.str();
}
