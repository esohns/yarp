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

#include "rpg_net_stream.h"

#include "rpg_net_defines.h"

#include <stream_iallocator.h>

#include <string>

RPG_Net_Stream::RPG_Net_Stream()
 : //inherited(),
   mySocketHandler(std::string("SocketHandler"),
                   NULL),
   myHeaderParser(std::string("HeaderParser"),
                  NULL),
   myRuntimeStatistic(std::string("RuntimeStatistic"),
                      NULL)
//    myProtocolHandler(std::string("ProtocolHandler"),
//                      NULL)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Stream::RPG_Net_Stream"));

  // remember the ones we "own"...
  // *TODO*: clean this up
  // *NOTE*: one problem is that we need to explicitly close() all
  // modules which we have NOT enqueued onto the stream (e.g. because init()
  // failed...)
  myAvailableModules.push_back(&mySocketHandler);
  myAvailableModules.push_back(&myHeaderParser);
  myAvailableModules.push_back(&myRuntimeStatistic);
//   myAvailableModules.push_back(&myProtocolHandler);

  // fix ACE bug: modules should initialize their "next" member to NULL !
  for (MODULE_CONTAINERITERATOR_TYPE iter = myAvailableModules.begin();
       iter != myAvailableModules.end();
       iter++)
  {
    (*iter)->next(NULL);
  } // end FOR
}

RPG_Net_Stream::~RPG_Net_Stream()
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Stream::~RPG_Net_Stream"));

  // *NOTE*: this implements an ordered shutdown on destruction...
  inherited::shutdown();
}

const bool
RPG_Net_Stream::init(Stream_IAllocator* allocator_in,
                     const RPG_Net_StreamConfigPOD& config_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Stream::init"));

  // sanity check(s)
  if (myIsInitialized)
  {
    // *TODO*: implement re-initialization ?
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("already initialized, aborting\n")));

    return false;
  } // end IF

  // things to be done here:
  // - create modules (done for the ones we "own")
  // - init modules
  // - push them onto the stream (tail-first) !
//   // ******************* Protocol Handler ************************
//   RPG_Net_Module_ProtocolHandler* protocolHandler_impl = NULL;
//   protocolHandler_impl = ACE_dynamic_cast(RPG_Net_Module_ProtocolHandler*,
//                                           myProtocolHandler.writer());
//   if (!protocolHandler_impl)
//   {
//     ACE_DEBUG((LM_ERROR,
//                ACE_TEXT("ACE_dynamic_cast(RPG_Net_Module_ProtocolHandler) failed, aborting\n")));
//
//     return false;
//   } // end IF
//   if (!protocolHandler_impl->init(config_in.storagePath,
//                                   DEF_CAPTURE_FILENAME_PREFIX,
//                                   DEF_CAPTURE_FILENAME_SUFFIX,
//                                   config_in.recordingIntervalPerFile,
//                                   false)) // try using separate PCAP handle...
//   {
//     ACE_DEBUG((LM_ERROR,
//                ACE_TEXT("failed to initialize module: \"%s\", aborting\n"),
//                myProtocolHandler.name()));
//
//     return false;
//   } // end IF
//
//   // enqueue the module...
//   if (push(&myProtocolHandler))
//   {
//     ACE_DEBUG((LM_ERROR,
//                ACE_TEXT("failed to ACE_Stream::push() module: \"%s\", aborting\n"),
//                myProtocolHandler.name()));
//
//     return false;
//   } // end IF

  // ******************* Runtime Statistics ************************
  RPG_Net_Module_RuntimeStatistic* runtimeStatistic_impl = NULL;
  runtimeStatistic_impl = ACE_dynamic_cast(RPG_Net_Module_RuntimeStatistic*,
                                           myRuntimeStatistic.writer());
  if (!runtimeStatistic_impl)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("ACE_dynamic_cast(RPG_Net_Module_RuntimeStatistic) failed, aborting\n")));

    return false;
  } // end IF
  if (!runtimeStatistic_impl->init(false, // do not print hash ("#") marks...
                                   true, // print pcap stats...
                                   allocator_in,
                                   config_in.statisticsReportingInterval))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to initialize module: \"%s\", aborting\n"),
               myRuntimeStatistic.name()));

    return false;
  } // end IF

  // enqueue the module...
  if (push(&myRuntimeStatistic))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Stream::push() module: \"%s\", aborting\n"),
               myRuntimeStatistic.name()));

    return false;
  } // end IF

  // ******************* Header Parser ************************
  RPG_Net_Module_HeaderParser* headerParser_impl = NULL;
  headerParser_impl = ACE_dynamic_cast(RPG_Net_Module_HeaderParser*,
                                       myHeaderParser.writer());
  if (!headerParser_impl)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("ACE_dynamic_cast(RPG_Net_Module_HeaderParser) failed, aborting\n")));

    return false;
  } // end IF
  if (!headerParser_impl->init())
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to initialize module: \"%s\", aborting\n"),
               myHeaderParser.name()));

    return false;
  } // end IF

  // enqueue the module...
  if (push(&myHeaderParser))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Stream::push() module: \"%s\", aborting\n"),
               myHeaderParser.name()));

    return false;
  } // end IF

  // ******************* Socket Handler ************************
  RPG_Net_Module_SocketHandler* socketHandler_impl = NULL;
  socketHandler_impl = ACE_dynamic_cast(RPG_Net_Module_SocketHandler*,
                                        mySocketHandler.writer());
  if (!socketHandler_impl)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("ACE_dynamic_cast(RPG_Net_Module_SocketHandler) failed, aborting\n")));

    return false;
  } // end IF
  if (!socketHandler_impl->init(allocator_in,
                                config_in.networkInterface,
                                RPG_NET_DEF_PCAP_SOCK_RECVBUF_SIZE,
                                RPG_NET_DEF_STATISTICS_COLLECT_INTERVAL))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to initialize module: \"%s\", aborting\n"),
               mySocketHandler.name()));

    return false;
  } // end IF

  // enqueue the module...
  if (push(&mySocketHandler))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Stream::push() module: \"%s\", aborting\n"),
               mySocketHandler.name()));

    return false;
  } // end IF

  // OK: all went well
  myIsInitialized = true;

  // debug info
  inherited::dump_state();

  return true;
}

const bool
RPG_Net_Stream::collect(RPG_Net_RuntimeStatistic& data_out)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Stream::collect"));

  RPG_Net_Module_RuntimeStatistic* runtimeStatistic_impl = NULL;
  runtimeStatistic_impl = ACE_dynamic_cast(RPG_Net_Module_RuntimeStatistic*,
                                           myRuntimeStatistic.writer());
  if (!runtimeStatistic_impl)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("ACE_dynamic_cast(RPG_Net_Module_RuntimeStatistic) failed, aborting\n")));

    return false;
  } // end IF

  // delegate to this module...
  return (runtimeStatistic_impl->collect(data_out));
}

void
RPG_Net_Stream::report()
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Stream::report"));

//   RPG_Net_Module_RuntimeStatistic* runtimeStatistic_impl = NULL;
//   runtimeStatistic_impl = ACE_dynamic_cast(RPG_Net_Module_RuntimeStatistic*,
//                                            myRuntimeStatistic.writer());
//   if (!runtimeStatistic_impl)
//   {
//     ACE_DEBUG((LM_ERROR,
//                ACE_TEXT("ACE_dynamic_cast(RPG_Net_Module_RuntimeStatistic) failed, aborting\n")));
//
//     return;
//   } // end IF
//
//   // delegate to this module...
//   return (runtimeStatistic_impl->report());

  // just a dummy
  ACE_ASSERT(false);

  ACE_NOTREACHED()
}
