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

#ifndef TEST_U_NET_CLIENT_COMMON_H
#define TEST_U_NET_CLIENT_COMMON_H

#include <list>
#include <string>

#define _SDL_main_h
#define SDL_main_h_
#include "SDL.h"

#include "ace/Synch_Traits.h"

#include "common_parser_common.h"

#include "common_ui_gtk_common.h"

#include "stream_allocatorheap.h"
#include "stream_common.h"
#include "stream_isessionnotify.h"

#include "rpg_net_defines.h"

#include "rpg_net_protocol_message.h"
#include "rpg_net_protocol_session_message.h"
#include "rpg_net_protocol_stream_common.h"

#include "rpg_engine_common.h"

// forward declaration(s)
struct RPG_Client_Configuration;
typedef Stream_ISessionDataNotify_T<struct RPG_Net_Protocol_SessionData,
                                    enum Stream_SessionMessageType,
                                    RPG_Net_Protocol_Message,
                                    RPG_Net_Protocol_SessionMessage> RPG_Net_Protocol_ISessionNotify_t;
typedef std::list<RPG_Net_Protocol_ISessionNotify_t*> RPG_Net_Protocol_Subscribers_t;
typedef RPG_Net_Protocol_Subscribers_t::iterator RPG_Net_Protocol_SubscribersIterator_t;
class RPG_Engine;
class RPG_Client_Engine;
class RPG_Graphics_IWindowBase;

struct Net_Client_GTK_CBData
 : Common_UI_GTK_CBData
{
  Net_Client_GTK_CBData ()
   : Common_UI_GTK_CBData ()
   , allocatorConfiguration ()
   , allowUserRuntimeStatistic (true)
   , configuration (NULL)
   , heapAllocator ()
   , messageAllocator (RPG_NET_MAXIMUM_NUMBER_OF_INFLIGHT_MESSAGES,
                       &heapAllocator,
                       true) // block ?
   , entity ()
   , level ()
   , mapWindow (NULL)
   , schemaRepository ()
#if defined (SDL_USE)
   , screen (NULL)
#elif defined (SDL2_USE) || defined (SDL3_USE)
   , renderer (NULL)
   , screen (NULL)
   , GLContext (NULL)
#endif // SDL_USE || SDL2_USE || SDL3_USE
   , doHover (true)
   , hoverTime (0)
   , levelEngine (NULL)
   , clientEngine (NULL)
   , entities ()
  {}

  struct Common_Parser_FlexAllocatorConfiguration              allocatorConfiguration;
  bool                                                         allowUserRuntimeStatistic;
  struct RPG_Client_Configuration*                             configuration;
  Stream_AllocatorHeap_T<ACE_MT_SYNCH,
                         struct Stream_AllocatorConfiguration> heapAllocator;
  RPG_Net_MessageAllocator_t                                   messageAllocator;
  struct RPG_Engine_Entity                                     entity;
  struct RPG_Engine_LevelData                                  level;
  class RPG_Graphics_IWindowBase*                              mapWindow;
  std::string                                                  schemaRepository;
#if defined (SDL_USE)
  SDL_Surface*                                                 screen;
#elif defined (SDL2_USE) || defined (SDL3_USE)
  SDL_Renderer*                                                renderer;
  SDL_Window*                                                  screen;
  SDL_GLContext                                                GLContext;
#endif // SDL_USE || SDL2_USE || SDL3_USE
  bool                                                         doHover;
  unsigned int                                                 hoverTime;
  RPG_Engine*                                                  levelEngine;
  RPG_Client_Engine*                                           clientEngine;
  RPG_Engine_Entities_t                                        entities;
};

#endif
