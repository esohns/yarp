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

#include "rpg_client_engine.h"

#include "rpg_client_defines.h"
#include "rpg_client_iwindow_level.h"
#include "rpg_client_iwindow.h"
#include "rpg_client_common_tools.h"
#include "rpg_client_ui_tools.h"
#include "rpg_client_entity_manager.h"

#include "rpg_graphics_defines.h"
#include "rpg_graphics_surface.h"
#include "rpg_graphics_cursor_manager.h"
#include "rpg_graphics_common_tools.h"

#include "rpg_sound_common.h"
#include "rpg_sound_common_tools.h"

#include "rpg_engine.h"

#include "rpg_common_macros.h"
#include "rpg_common_file_tools.h"

#include <ace/Log_Msg.h>

RPG_Client_Engine::RPG_Client_Engine()
//  : myQueue(RPG_ENGINE_MAX_QUEUE_SLOTS),
 : myCondition(myLock),
   myStop(false),
   myEngine(NULL),
   myWindow(NULL),
   myWidgets(NULL),
//   myActions(),
//   mySeenPositions(),
   mySelectionMode(SELECTIONMODE_NORMAL),
   myCenterOnActivePlayer(RPG_CLIENT_DEF_CENTER_ON_ACTIVE_PLAYER)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Engine::RPG_Client_Engine"));

  // use member message queue...
//   inherited::msg_queue(&myQueue);
}

RPG_Client_Engine::~RPG_Client_Engine()
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Engine::~RPG_Client_Engine"));

}

int
RPG_Client_Engine::close(u_long arg_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Engine::close"));

  // *IMPORTANT NOTE*: this method may be invoked
  // [- by an external thread closing down the active object]
  // - by the worker thread which calls this after returning from svc()
  //    --> in this case, this should be a NOP...
  switch (arg_in)
  {
    // called from ACE_Task_Base on clean-up
    case 0:
    {
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("(graphics engine) worker thread (ID: %t) leaving...\n")));

      // don't do anything...
      break;
    }
    case 1:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("should never get here, returning\n")));

      ACE_ASSERT(false);

      // what else can we do ?
      break;
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

int
RPG_Client_Engine::svc(void)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Engine::svc"));

  while (true)
  {
    myLock.acquire();
    if (myActions.empty())
    {
      // wait for an action
      myCondition.wait();
    } // end IF

    if (myStop)
    {
      myLock.release();

      return 0;
    } // end IF

    // sanity check
    ACE_ASSERT(!myActions.empty());

    handleActions();

    myLock.release();
  } // end WHILE

  // SHOULD NEVER-EVER GET HERE !
  ACE_ASSERT(false);

  return -1;
}

void
RPG_Client_Engine::start()
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Engine::start"));

  // sanity check
  if (isRunning())
    return;

  // OK: start worker thread
  ACE_thread_t thread_ids[1];
  thread_ids[0] = 0;
  ACE_hthread_t thread_handles[1];
  thread_handles[0] = 0;

  // *IMPORTANT NOTE*: MUST be THR_JOINABLE !!!
  int ret = 0;
  ret = inherited::activate((THR_NEW_LWP |
                             THR_JOINABLE |
                             THR_INHERIT_SCHED),         // flags
                            1,                           // number of threads
                            0,                           // force spawning
                            ACE_DEFAULT_THREAD_PRIORITY, // priority
                            -1,                          // no group id
                            NULL,                        // corresp. task --> use 'this'
                            thread_handles,              // thread handle(s)
                            NULL,                        // thread stack(s)
                            NULL,                        // thread stack size(s)
                            thread_ids,                  // thread id(s)
														NULL);                       // thread name(s)
  if (ret == -1)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Task_Base::activate(): \"%m\", continuing\n")));
  else
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("(graphics engine) started worker thread (id: %u)...\n"),
               thread_ids[0]));
}

void
RPG_Client_Engine::stop()
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Engine::stop"));

  // sanity check
  if (!isRunning())
    return;

  myLock.acquire();

  myStop = true;

  // wake up the (waiting) worker thread(s)
  myCondition.broadcast();

  myLock.release();

  // ... and wait for the worker thread to join
  if (inherited::wait() == -1)
  {
    ACE_DEBUG((LM_CRITICAL,
               ACE_TEXT("failed to ACE_Task_Base::wait(): \"%m\", aborting\n")));

    return;
  } // end IF

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("worker thread has joined...\n")));
}

bool
RPG_Client_Engine::isRunning() const
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Engine::isRunning"));

  return (inherited::thr_count() > 0);
}

void
RPG_Client_Engine::dump_state() const
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Engine::dump_state"));

  ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("current queue size: %d\n"),
             myActions.size()));
}

void
RPG_Client_Engine::initMap()
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Engine::initMap"));

  // sanity check
  ACE_ASSERT(myWindow);

  RPG_Client_Action new_action;
  new_action.command = COMMAND_WINDOW_INIT;
  new_action.position = std::make_pair(std::numeric_limits<unsigned int>::max(),
                                       std::numeric_limits<unsigned int>::max());
  new_action.window = myWindow;
  new_action.cursor = RPG_GRAPHICS_CURSOR_INVALID;
  new_action.entity_id = 0;
  new_action.radius = 0;

  action(new_action);
}

void
RPG_Client_Engine::redraw()
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Engine::redraw"));

  // sanity check
  ACE_ASSERT(myWindow);

  // step1: draw map window
  RPG_Client_Action new_action;
  new_action.command = COMMAND_WINDOW_DRAW;
  new_action.position = std::make_pair(std::numeric_limits<unsigned int>::max(),
                                       std::numeric_limits<unsigned int>::max());
  new_action.window = myWindow;
  new_action.cursor = RPG_GRAPHICS_CURSOR_INVALID;
  new_action.entity_id = 0;
  new_action.radius = 0;

  action(new_action);

  // step2: refresh the window
  new_action.command = COMMAND_WINDOW_REFRESH;
  action(new_action);
}

void
RPG_Client_Engine::setView(const RPG_Map_Position_t& position_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Engine::setView"));

  // sanity check
  ACE_ASSERT(myWindow);

  RPG_Client_Action new_action;
  new_action.command = COMMAND_SET_VIEW;
  new_action.position = position_in;
  new_action.window = myWindow;
  new_action.cursor = RPG_GRAPHICS_CURSOR_INVALID;
  new_action.entity_id = 0;
  new_action.radius = 0;

  action(new_action);
}

//void
//RPG_Client_Engine::toggleDoor(const RPG_Map_Position_t& position_in)
//{
//  RPG_TRACE(ACE_TEXT("RPG_Client_Engine::toggleDoor"));
//
//  // sanity check
//  ACE_ASSERT(myWindow);
//
//  RPG_Client_Action new_action;
//  new_action.command = COMMAND_TOGGLE_DOOR;
//  new_action.position = position_in;
//  new_action.window = myWindow;
//  new_action.cursor = RPG_GRAPHICS_CURSOR_INVALID;
//  new_action.entity_id = 0;
//
//  action(new_action);
//}
//
//void
//RPG_Client_Engine::addEntity(const RPG_Engine_EntityID_t& id_in,
//                             const SDL_Surface* graphic_in)
//{
//  RPG_TRACE(ACE_TEXT("RPG_Client_Engine::addEntity"));
//
//  RPG_CLIENT_ENTITY_MANAGER_SINGLETON::instance()->add(id_in, graphic_in);
//}
//
//void
//RPG_Client_Engine::removeEntity(const RPG_Engine_EntityID_t& id_in)
//{
//  RPG_TRACE(ACE_TEXT("RPG_Client_Engine::removeEntity"));
//
//  RPG_CLIENT_ENTITY_MANAGER_SINGLETON::instance()->remove(id_in);
//}
//
//void
//RPG_Client_Engine::updateEntity(const RPG_Engine_EntityID_t& id_in)
//{
//  RPG_TRACE(ACE_TEXT("RPG_Client_Engine::updateEntity"));
//
//  // sanity check
//  ACE_ASSERT(myWindow);
//
//  RPG_Client_Action new_action;
//  new_action.command = COMMAND_ENTITY_DRAW;
//  new_action.position = myEngine->getPosition(id_in);
//  new_action.window = myWindow;
//  new_action.cursor = RPG_GRAPHICS_CURSOR_INVALID;
//  new_action.entity_id = id_in;
//
//  action(new_action);
//}

void
RPG_Client_Engine::notify(const RPG_Engine_Command& command_in,
                          const RPG_Engine_ClientNotificationParameters_t& parameters_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Engine::notify"));

  ACE_ASSERT(myEngine);

  bool do_action = true;
  RPG_Client_Action client_action;
  client_action.command = RPG_CLIENT_COMMAND_INVALID;
  client_action.previous = std::make_pair(std::numeric_limits<unsigned int>::max(),
                                          std::numeric_limits<unsigned int>::max());
  client_action.position = std::make_pair(std::numeric_limits<unsigned int>::max(),
                                          std::numeric_limits<unsigned int>::max());
  client_action.window = NULL;
  client_action.cursor = RPG_GRAPHICS_CURSOR_INVALID;
  client_action.entity_id = 0;
  client_action.sound = RPG_SOUND_EVENT_INVALID;
  client_action.source = std::make_pair(std::numeric_limits<unsigned int>::max(),
                                        std::numeric_limits<unsigned int>::max());
  client_action.radius = 0;
  switch (command_in)
  {
    case COMMAND_ATTACK:
    case COMMAND_ATTACK_FULL:
    case COMMAND_ATTACK_STANDARD:
      do_action = false; break;
    case COMMAND_DOOR_CLOSE:
      client_action.sound = EVENT_DOOR_CLOSE;
    case COMMAND_DOOR_OPEN:
    {
      client_action.command = COMMAND_TOGGLE_DOOR;
      client_action.position = parameters_in.position;
      ACE_ASSERT(client_action.position != std::make_pair(std::numeric_limits<unsigned int>::max(),
                                                          std::numeric_limits<unsigned int>::max()));
      client_action.window = myWindow;
      client_action.entity_id = parameters_in.entity_id;
      ACE_ASSERT(client_action.entity_id);
      action(client_action);

      client_action.command = COMMAND_PLAY_SOUND;
      if (command_in == COMMAND_DOOR_OPEN)
        client_action.sound = EVENT_DOOR_OPEN;

      // update seen positions
      {
        ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

        RPG_Client_SeenPositionsIterator_t iterator = mySeenPositions.find(client_action.entity_id);
        ACE_ASSERT(iterator != mySeenPositions.end());

        RPG_Map_Positions_t seen_positions;
        myEngine->getVisiblePositions(client_action.entity_id,
                                      seen_positions,
                                      true);
        (*iterator).second.insert(seen_positions.begin(),
                                  seen_positions.end());
      } // end lock scope

      break;
    }
    case COMMAND_RUN:
    case COMMAND_SEARCH:
    case COMMAND_STEP:
    case COMMAND_STOP:
    case COMMAND_TRAVEL:
      do_action = false; break;
    case COMMAND_E2C_ENTITY_ADD:
    {
      client_action.entity_id = parameters_in.entity_id;
      ACE_ASSERT(client_action.entity_id);

      // step1: load sprite graphics
      SDL_Surface* sprite_graphic = NULL;
      RPG_Graphics_GraphicTypeUnion type;
      type.discriminator = RPG_Graphics_GraphicTypeUnion::SPRITE;
      if (!myEngine->isMonster(client_action.entity_id, true))
        type.sprite = parameters_in.sprite;
      else
        type.sprite = RPG_Client_Common_Tools::monster2Sprite(myEngine->getName(client_action.entity_id, true));
      sprite_graphic = RPG_Graphics_Common_Tools::loadGraphic(type,   // sprite
                                                              true,   // convert to display format
                                                              false); // don't cache
      ACE_ASSERT(sprite_graphic);
      RPG_CLIENT_ENTITY_MANAGER_SINGLETON::instance()->add(client_action.entity_id,
                                                           sprite_graphic);

      // step2: init seen positions
      RPG_Map_Positions_t seen_positions;
      myEngine->getVisiblePositions(client_action.entity_id,
                                    seen_positions,
                                    true);
      {
        ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

        ACE_ASSERT(mySeenPositions.find(client_action.entity_id) == mySeenPositions.end());
        mySeenPositions.insert(std::make_pair(client_action.entity_id, seen_positions));
      } // end lock scope

      do_action = false;
      
      break;
    }
    case COMMAND_E2C_ENTITY_HIT:
    case COMMAND_E2C_ENTITY_MISS:
    {
      client_action.entity_id = parameters_in.entity_id;
      ACE_ASSERT(client_action.entity_id);

      client_action.command = COMMAND_PLAY_SOUND;
      client_action.sound = ((command_in == COMMAND_E2C_ENTITY_HIT) ? EVENT_SWORD_HIT
                                                                    : EVENT_SWORD_MISS);

      break;
    }
    case COMMAND_E2C_ENTITY_POSITION:
    {
      client_action.entity_id = parameters_in.entity_id;
      ACE_ASSERT(client_action.entity_id);
      client_action.position = parameters_in.position;
      ACE_ASSERT(client_action.position != std::make_pair(std::numeric_limits<unsigned int>::max(),
                                                          std::numeric_limits<unsigned int>::max()));
      client_action.previous = parameters_in.previous_position;
      ACE_ASSERT(client_action.previous != std::make_pair(std::numeric_limits<unsigned int>::max(),
                                                          std::numeric_limits<unsigned int>::max()));
      client_action.window = myWindow;

      // *NOTE*: when using (dynamic) lighting, redraw the whole window...
      RPG_Engine_EntityID_t active_entity_id = myEngine->getActive(true);
      client_action.command = ((client_action.entity_id == active_entity_id) ? COMMAND_WINDOW_DRAW
                                                                             : COMMAND_ENTITY_DRAW);

      // adjust view ?
      if (client_action.entity_id == active_entity_id)
      {
        client_action.command = COMMAND_PLAY_SOUND;
        client_action.sound = EVENT_WALK;
        action(client_action);

        // *NOTE*: re-drawing the window will invalidate the hightlight BG...
        client_action.command = COMMAND_TILE_HIGHLIGHT_INVALIDATE_BG;
        action(client_action);

        if (getCenterOnActive())
          client_action.command = COMMAND_SET_VIEW;
        else
          client_action.command = COMMAND_WINDOW_DRAW;
        action(client_action);

        RPG_Client_IWindowLevel* level_window = dynamic_cast<RPG_Client_IWindowLevel*>(myWindow);
        ACE_ASSERT(level_window);
        client_action.position = RPG_Graphics_Common_Tools::screen2Map(RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->position(),
                                                                       myEngine->getSize(true),
                                                                       myWindow->getSize(false),
                                                                       level_window->getView());

        client_action.command = COMMAND_TILE_HIGHLIGHT_STORE_BG;
        action(client_action);

        client_action.command = COMMAND_TILE_HIGHLIGHT_DRAW;
        //action(client_action);

        //client_action.command = COMMAND_CURSOR_INVALIDATE_BG;
        //action(client_action);

        //client_action.position = RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->position();
        //client_action.command = COMMAND_CURSOR_DRAW;
      } // end IF
      else
      {
        // --> (re)draw entity ?
        ACE_ASSERT(client_action.command == COMMAND_ENTITY_DRAW);

        if (active_entity_id)
        {
          // update the minimap (only)
          client_action.command = COMMAND_WINDOW_UPDATE_MINIMAP;
          action(client_action);

          client_action.command = COMMAND_ENTITY_DRAW;
          if (!myEngine->canSee(active_entity_id,
                                client_action.position,
                                true))
          {
            do_action = false; // entity not within field of view...
            
            if (myEngine->canSee(active_entity_id,
                                 client_action.previous,
                                 true))
            {
              // entity has left field of view...
              client_action.command = COMMAND_ENTITY_REMOVE;
              do_action = true;
            } // end IF
          } // end IF
        } // end IF
        else
          do_action = false;
      } // end ELSE

      // update seen positions
      RPG_Map_Positions_t seen_positions;
      myEngine->getVisiblePositions(client_action.entity_id,
                                    seen_positions,
                                    true);
      {
        ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

        ACE_ASSERT(mySeenPositions.find(client_action.entity_id) != mySeenPositions.end());
        mySeenPositions[client_action.entity_id].insert(seen_positions.begin(), seen_positions.end());
      } // end lock scope

      break;
    }
    case COMMAND_E2C_ENTITY_REMOVE:
    {
      client_action.command = COMMAND_ENTITY_REMOVE;
      client_action.window = myWindow;
      client_action.entity_id = parameters_in.entity_id;
      ACE_ASSERT(client_action.entity_id);
      action(client_action);

      // fini seen positions
      {
        ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

        ACE_ASSERT(mySeenPositions.find(client_action.entity_id) != mySeenPositions.end());
        mySeenPositions.erase(client_action.entity_id);
      } // end lock scope

      if (client_action.entity_id == myEngine->getActive(true))
        do_action = false; // don't play a sound...

      client_action.command = COMMAND_PLAY_SOUND;
      client_action.sound = EVENT_CONDITION_WEAK;

      break;
    }
    case COMMAND_E2C_ENTITY_VISION:
    {
      client_action.window = myWindow;
      client_action.entity_id = parameters_in.entity_id;
      ACE_ASSERT(client_action.entity_id);
      client_action.radius = parameters_in.visible_radius;

      // *NOTE*: re-drawing the window will invalidate the hightlight BG...
      client_action.command = COMMAND_TILE_HIGHLIGHT_INVALIDATE_BG;
      action(client_action);

      client_action.command = COMMAND_SET_VISION_RADIUS;
      action(client_action);

      client_action.command = COMMAND_TILE_HIGHLIGHT_STORE_BG;
      action(client_action);

      client_action.command = COMMAND_TILE_HIGHLIGHT_DRAW;
      action(client_action);

      client_action.command = COMMAND_CURSOR_INVALIDATE_BG;
      action(client_action);

      client_action.command = COMMAND_CURSOR_DRAW;

      // update seen positions
      RPG_Map_Positions_t seen_positions;
      myEngine->getVisiblePositions(client_action.entity_id,
                                    seen_positions,
                                    true);
      {
        ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

        ACE_ASSERT(mySeenPositions.find(client_action.entity_id) != mySeenPositions.end());
        mySeenPositions[client_action.entity_id].insert(seen_positions.begin(), seen_positions.end());
      } // end lock scope

      break;
    }
    case COMMAND_E2C_MESSAGE:
    {
      client_action.command = COMMAND_WINDOW_UPDATE_MESSAGEWINDOW;
      client_action.window = myWindow;
      client_action.message = parameters_in.message;

      break;
    }
    case COMMAND_E2C_QUIT:
    {
      if (myWidgets)
      {
        GDK_THREADS_ENTER();
        GtkWidget* widget = glade_xml_get_widget(myWidgets,
                                                 ACE_TEXT_ALWAYS_CHAR("part"));
        ACE_ASSERT(widget);
        // raise dialog window
        GdkWindow* toplevel = gtk_widget_get_parent_window(widget);
        ACE_ASSERT(toplevel);
        gdk_window_deiconify(toplevel);
        // emit a signal...
        gtk_button_clicked(GTK_BUTTON(widget));
        GDK_THREADS_LEAVE();
      } // end IF

      do_action = false;

      break;
    }
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid command (was: \"%s\", aborting\n"),
                 ACE_TEXT(RPG_Engine_CommandHelper::RPG_Engine_CommandToString(command_in).c_str())));

      do_action = false;

      break;
    }
  } // end SWITCH

  if (do_action)
    action(client_action);
}

void
RPG_Client_Engine::init(RPG_Engine* engine_in,
                        RPG_Graphics_IWindow* window_in,
                        GladeXML* widgets_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Engine::init"));

  // sanity check(s)
  ACE_ASSERT(engine_in);
  ACE_ASSERT(window_in);
  //ACE_ASSERT(widgets_in);

  myEngine = engine_in;
  myWindow = window_in;
  myWidgets = widgets_in;
}

void
RPG_Client_Engine::action(const RPG_Client_Action& action_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Engine::action"));

  ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

  myActions.push_back(action_in);

  // wake up the (waiting) worker thread(s)
  myCondition.broadcast();
}

void
RPG_Client_Engine::mode(const RPG_Client_SelectionMode& mode_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Engine::mode"));

  mySelectionMode = mode_in;
}

//void
//RPG_Client_Engine::clear(const RPG_Client_SelectionMode& mode_in)
//{
//  RPG_TRACE(ACE_TEXT("RPG_Client_Engine::clear"));
//
//  mySelectionMode = SELECTIONMODE_NORMAL;
//}

RPG_Client_SelectionMode
RPG_Client_Engine::mode() const
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Engine::hasMode"));

  return mySelectionMode;
}

bool
RPG_Client_Engine::hasSeen(const RPG_Engine_EntityID_t& id_in,
                           const RPG_Map_Position_t& position_in,
                           const bool& lockedAccess_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Engine::hasSeen"));

  if (lockedAccess_in)
    myLock.acquire();

  RPG_Client_SeenPositionsConstIterator_t iterator = mySeenPositions.find(id_in);
  if (iterator == mySeenPositions.end())
  {
    if (lockedAccess_in)
      myLock.release();
    
    return false;
  } // end IF

  RPG_Map_PositionsConstIterator_t iterator2 = (*iterator).second.find(position_in);
  bool result = (iterator2 != (*iterator).second.end());
  
  if (lockedAccess_in)
    myLock.release();
  
  return result;
}

void
RPG_Client_Engine::centerOnActive(const bool& centerOnActive_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Engine::centerOnActive"));

  ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

  myCenterOnActivePlayer = centerOnActive_in;
}

bool
RPG_Client_Engine::getCenterOnActive() const
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Engine::getCenterOnActive"));

  bool result;
  {
    ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

    result = myCenterOnActivePlayer;
  } // end lock scope

  return result;
}

void
RPG_Client_Engine::handleActions()
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Engine::handleActions"));

  bool refresh_window = false;
  SDL_Rect dirtyRegion = {0, 0, 0, 0};
  for (RPG_Client_ActionsIterator_t iterator = myActions.begin();
       iterator != myActions.end();
       iterator++)
  {
    refresh_window = false;

    switch ((*iterator).command)
    {
      case COMMAND_CURSOR_DRAW:
      {
        // sanity check
        ACE_ASSERT(myWindow);

        RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->put((*iterator).position.first,
                                                               (*iterator).position.second,
                                                               myWindow->getScreen(),
                                                               dirtyRegion);
        RPG_Graphics_Surface::update(dirtyRegion,
                                     myWindow->getScreen());

        break;
      }
      case COMMAND_CURSOR_INVALIDATE_BG:
      {
        RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->invalidateBG();

        break;
      }
      case COMMAND_CURSOR_RESTORE_BG:
      {
        // sanity check
        ACE_ASSERT(myWindow);

        RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->restoreBG(myWindow->getScreen(),
                                                                     dirtyRegion);
        RPG_Graphics_Surface::update(dirtyRegion,
                                     myWindow->getScreen());

        break;
      }
      case COMMAND_CURSOR_SET:
      {
        // sanity check
        ACE_ASSERT(myWindow);

        // sanity check
        RPG_Graphics_Cursor current_type = RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->type();
        if (current_type == (*iterator).cursor)
          break; // nothing to do...

        // step1: restore cursor bg
        RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->restoreBG(myWindow->getScreen(),
                                                                     dirtyRegion);
        RPG_Graphics_Surface::update(dirtyRegion,
                                     myWindow->getScreen());

        // step2: set new cursor
        RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->set((*iterator).cursor);

        break;
      }
      case COMMAND_ENTITY_DRAW:
      {
        // sanity check
        ACE_ASSERT((*iterator).position != std::make_pair(std::numeric_limits<unsigned int>::max(),
                                                          std::numeric_limits<unsigned int>::max()));
        ACE_ASSERT((*iterator).window);
        ACE_ASSERT((*iterator).entity_id);

        RPG_Client_IWindowLevel* level_window = dynamic_cast<RPG_Client_IWindowLevel*>(myWindow);
        ACE_ASSERT(level_window);
        RPG_CLIENT_ENTITY_MANAGER_SINGLETON::instance()->put((*iterator).entity_id,
                                                             RPG_Graphics_Common_Tools::map2Screen((*iterator).position,
                                                                                                   (*iterator).window->getSize(false),
                                                                                                   level_window->getView()),
                                                             myWindow->getScreen(),
                                                             dirtyRegion);
        RPG_Graphics_Surface::update(dirtyRegion,
                                     myWindow->getScreen());

        break;
      }
      case COMMAND_ENTITY_REMOVE:
      {
        // sanity check
        ACE_ASSERT(myWindow);
        ACE_ASSERT((*iterator).entity_id);

        // step1: restore bg
        RPG_CLIENT_ENTITY_MANAGER_SINGLETON::instance()->restoreBG((*iterator).entity_id,
                                                                   myWindow->getScreen(),
                                                                   dirtyRegion);
        RPG_Graphics_Surface::update(dirtyRegion,
                                     myWindow->getScreen());

        // step2: free entity resources ?
        if (!myEngine->exists((*iterator).entity_id))
          RPG_CLIENT_ENTITY_MANAGER_SINGLETON::instance()->remove((*iterator).entity_id);

        break;
      }
      case COMMAND_PLAY_SOUND:
      {
        // sanity check
        ACE_ASSERT((*iterator).sound != RPG_SOUND_EVENT_INVALID);

        int channel = -1;
        ACE_Time_Value length = ACE_Time_Value::zero;
        channel = RPG_Sound_Common_Tools::play((*iterator).sound, length);
        if (channel == -1)
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("failed to play sound (was: \"%s\"), continuing\n"),
                     ACE_TEXT(RPG_Sound_EventHelper::RPG_Sound_EventToString((*iterator).sound).c_str())));

        break;
      }
      case COMMAND_SET_VIEW:
      {
        // sanity check
        ACE_ASSERT(myWindow);

        RPG_Client_IWindowLevel* level_window = dynamic_cast<RPG_Client_IWindowLevel*>(myWindow);
        ACE_ASSERT(level_window);
        try
        {
          level_window->setView((*iterator).position);
          myWindow->clear();
          myWindow->draw();
        }
        catch (...)
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("caught exception in [%@]: RPG_Client_IWindowLevel::setView([%u,%u])/clear/draw(), aborting\n"),
                     level_window,
                     (*iterator).position.first, (*iterator).position.second));

          return;
        }

        //// fiddling with the view invalidates the tile highlight BG
        //RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->resetHighlightBG((*iterator).position);
        //// --> store/draw the new tile highlight (BG)
        //RPG_Map_Position_t cursor_position = RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->position();
        //RPG_Map_Position_t map_position = RPG_Graphics_Common_Tools::screen2Map(cursor_position,
        //                                                                        myEngine->getSize(true),
        //                                                                        (*iterator).window->getSize(false),
        //                                                                        (*iterator).window->getView());
        //RPG_Map_Position_t screen_position = RPG_Graphics_Common_Tools::map2Screen(map_position,
        //                                                                           (*iterator).window->getSize(false),
        //                                                                           (*iterator).window->getView());
        //RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->storeHighlightBG(map_position,
        //                                                                    screen_position,
        //                                                                    (*iterator).window->getScreen());
        //RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->drawHighlight(screen_position,
        //                                                                 (*iterator).window->getScreen());

        //// fiddling with the view (probably) invalidates (part of) the cursor BG
        //RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->updateBG((*iterator).window->getScreen());
        //SDL_Rect dirty_region = {0, 0, 0, 0};
        //RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->put(cursor_position.first,
        //                                                       cursor_position.second,
        //                                                       (*iterator).window->getScreen(),
        //                                                       dirty_region);
        ////RPG_Graphics_Surface::update(dirtyRegion,
        ////                             (*iterator).window->getScreen());

        refresh_window = true;

        break;
      }
      case COMMAND_SET_VISION_RADIUS:
      {
        // sanity check(s)
        ACE_ASSERT(myWindow);
				ACE_ASSERT((*iterator).window);

        RPG_Client_IWindowLevel* level_window = dynamic_cast<RPG_Client_IWindowLevel*>(myWindow);
        ACE_ASSERT(level_window);
        try
        {
          level_window->setBlendRadius((*iterator).radius);
          myWindow->clear();
          myWindow->draw();
        }
        catch (...)
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("caught exception in [%@]: RPG_Client_IWindowLevel::setBlendRadius(%u)/clear/draw(), aborting\n"),
                     level_window,
                     (*iterator).radius));

          return;
        }

        // fiddling with the vision invalidates the tile highlight BG
        RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->resetHighlightBG((*iterator).position);
        // --> store/draw the new tile highlight (BG)
        RPG_Map_Position_t cursor_position = RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->position();
        RPG_Map_Position_t map_position = RPG_Graphics_Common_Tools::screen2Map(cursor_position,
                                                                                myEngine->getSize(true),
                                                                                (*iterator).window->getSize(false),
                                                                                level_window->getView());
        RPG_Map_Position_t screen_position = RPG_Graphics_Common_Tools::map2Screen(map_position,
                                                                                   (*iterator).window->getSize(false),
                                                                                   level_window->getView());
        RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->storeHighlightBG(map_position,
                                                                            screen_position,
                                                                            myWindow->getScreen());
        RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->drawHighlight(screen_position,
                                                                         myWindow->getScreen());

        // fiddling with the vision (probably) invalidates (part of) the cursor BG
        RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->updateBG(myWindow->getScreen());
        SDL_Rect dirty_region = {0, 0, 0, 0};
        RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->put(cursor_position.first,
                                                               cursor_position.second,
                                                               myWindow->getScreen(),
                                                               dirty_region);
        //RPG_Graphics_Surface::update(dirtyRegion,
        //                             myWindow->getScreen());

        refresh_window = true;

        break;
      }
      case COMMAND_TILE_HIGHLIGHT_DRAW:
      {
        // sanity check(s)
				ACE_ASSERT((*iterator).window);
        ACE_ASSERT(myWindow);

        RPG_Graphics_Size_t size = (*iterator).window->getSize(false);
        RPG_Client_IWindowLevel* level_window = dynamic_cast<RPG_Client_IWindowLevel*>(myWindow);
        ACE_ASSERT(level_window);
        RPG_Graphics_Position_t view = level_window->getView();
        if ((*iterator).path.empty() &&
            (*iterator).positions.empty())
        {
          RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->drawHighlight(RPG_Graphics_Common_Tools::map2Screen((*iterator).position,
                                                                                                                 size,
                                                                                                                 view),
                                                                           myWindow->getScreen());
        } // end IF
        else if (!(*iterator).path.empty())
        {
          ACE_ASSERT((*iterator).positions.empty());
          RPG_Graphics_Positions_t graphics_positions;
          for (RPG_Map_PathConstIterator_t iterator2 = (*iterator).path.begin();
               iterator2 != (*iterator).path.end();
               iterator2++)
            graphics_positions.push_back(RPG_Graphics_Common_Tools::map2Screen((*iterator2).first,
                                                                               size,
                                                                               view));
          RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->drawHighlight(graphics_positions,
                                                                           myWindow->getScreen());
        } // end ELSE
        else
        {
          ACE_ASSERT(!(*iterator).positions.empty());
          RPG_Graphics_Positions_t graphics_positions;
          for (RPG_Map_PositionsConstIterator_t iterator2 = (*iterator).positions.begin();
               iterator2 != (*iterator).positions.end();
               iterator2++)
            graphics_positions.push_back(RPG_Graphics_Common_Tools::map2Screen(*iterator2,
                                                                               size,
                                                                               view));
          RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->drawHighlight(graphics_positions,
                                                                           myWindow->getScreen());
        } // end ELSE

        break;
      }
      case COMMAND_TILE_HIGHLIGHT_INVALIDATE_BG:
      {
        RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->resetHighlightBG((*iterator).position);

        break;
      }
      case COMMAND_TILE_HIGHLIGHT_RESTORE_BG:
      {
        // sanity check
				ACE_ASSERT(myWindow);

        RPG_Client_IWindowLevel* level_window = dynamic_cast<RPG_Client_IWindowLevel*>(myWindow);
        ACE_ASSERT(level_window);
        RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->restoreHighlightBG(level_window->getView(),
                                                                              myWindow->getScreen());

        break;
      }
      case COMMAND_TILE_HIGHLIGHT_STORE_BG:
      {
        // sanity check(s)
        ACE_ASSERT((*iterator).window);
				ACE_ASSERT(myWindow);

        RPG_Graphics_Size_t size = (*iterator).window->getSize(false);
        RPG_Client_IWindowLevel* level_window = dynamic_cast<RPG_Client_IWindowLevel*>(myWindow);
        ACE_ASSERT(level_window);
        RPG_Graphics_Position_t view = level_window->getView();
        if ((*iterator).path.empty() &&
            (*iterator).positions.empty())
        {
          RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->storeHighlightBG((*iterator).position,
                                                                              RPG_Graphics_Common_Tools::map2Screen((*iterator).position,
                                                                                                                    size,
                                                                                                                    view),
                                                                              myWindow->getScreen());
        }
        else if (!(*iterator).path.empty())
        {
          ACE_ASSERT((*iterator).positions.empty());
          RPG_Map_PositionList_t map_positions;
          RPG_Graphics_Positions_t graphics_positions;
          for (RPG_Map_PathConstIterator_t iterator2 = (*iterator).path.begin();
               iterator2 != (*iterator).path.end();
               iterator2++)
          {
            map_positions.push_back((*iterator2).first);
            graphics_positions.push_back(RPG_Graphics_Common_Tools::map2Screen((*iterator2).first,
                                                                               size,
                                                                               view));
          } // end FOR
          RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->storeHighlightBG(map_positions,
                                                                              graphics_positions,
                                                                              myWindow->getScreen());
        } // end ELSE
        else
        {
          ACE_ASSERT(!(*iterator).positions.empty());
          RPG_Map_PositionList_t map_positions;
          RPG_Graphics_Positions_t graphics_positions;
          for (RPG_Map_PositionsConstIterator_t iterator2 = (*iterator).positions.begin();
               iterator2 != (*iterator).positions.end();
               iterator2++)
          {
            map_positions.push_back(*iterator2);
            graphics_positions.push_back(RPG_Graphics_Common_Tools::map2Screen(*iterator2,
                                                                               size,
                                                                               view));
          } // end FOR
          RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->storeHighlightBG(map_positions,
                                                                              graphics_positions,
                                                                              myWindow->getScreen());
        } // end ELSE

        break;
      }
      case COMMAND_TOGGLE_DOOR:
      {
        // sanity check
        ACE_ASSERT(myWindow);

        RPG_Client_IWindowLevel* level_window = dynamic_cast<RPG_Client_IWindowLevel*>((*iterator).window);
        ACE_ASSERT(level_window);
        try
        {
          level_window->toggleDoor((*iterator).position);
          myWindow->clear();
          myWindow->draw();
        }
        catch (...)
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("caught exception in [%@]: RPG_Client_IWindowLevel::toggleDoor/clear/draw(), aborting\n"),
                     level_window));

          return;
        }

        refresh_window = true;

        break;
      }
      case COMMAND_WINDOW_BORDER_DRAW:
      {
        // sanity check
        ACE_ASSERT((*iterator).window);

        RPG_Client_IWindow* client_window = dynamic_cast<RPG_Client_IWindow*>((*iterator).window);
        ACE_ASSERT(client_window);
        try
        {
          client_window->drawBorder((*iterator).window->getScreen(),
                                    0,
                                    0);
        }
        catch (...)
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("caught exception in [%@]: RPG_Client_IWindow::drawBorder(), aborting\n"),
                     client_window));

          return;
        }

        refresh_window = true;

        break;
      }
      case COMMAND_WINDOW_DRAW:
      {
        // sanity check
        ACE_ASSERT((*iterator).window);

        try
        {
          (*iterator).window->clear();
          (*iterator).window->draw();
        }
        catch (...)
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("caught exception in [%@]: RPG_Graphics_IWindow::draw(), aborting\n"),
                     (*iterator).window));

          return;
        }

        refresh_window = true;

        break;
      }
      case COMMAND_WINDOW_INIT:
      {
        // sanity check
				ACE_ASSERT(myWindow);

        // fiddling with the view invalidates the cursor/tile highlight BG
        RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->invalidateBG();
        RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->resetHighlightBG((*iterator).position);

        RPG_Map_Position_t center = myEngine->getSize(true);
        center.first >>= 1;
        center.second >>= 1;

        // step1: init/(re)draw window
        RPG_Client_IWindowLevel* level_window = dynamic_cast<RPG_Client_IWindowLevel*>(myWindow);
        ACE_ASSERT(level_window);
        try
        {
          level_window->init();
          level_window->setView(center);
          myWindow->clear();
          myWindow->draw();
        }
        catch (...)
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("caught exception in [%@]: RPG_Client_IWindowLevel::init/setView/clear/draw(), aborting\n"),
                     level_window));

          return;
        }

        // step2: (re)set window title caption/iconify
        std::string caption = ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_DEF_GRAPHICS_MAINWINDOW_TITLE);
        const std::string& level_name = myEngine->getMeta(true).name;
        if (!level_name.empty())
        {
          caption = ACE_TEXT_ALWAYS_CHAR("* ");
          caption += level_name;
          caption += ACE_TEXT_ALWAYS_CHAR(" *");
        } // end IF
        else
        {
          // no map --> iconify
          if (SDL_WM_IconifyWindow() == 0)
            ACE_DEBUG((LM_ERROR,
                       ACE_TEXT("failed to SDL_WM_IconifyWindow(): \"%s\", continuing\n"),
                       SDL_GetError()));
        } // end IF
        gchar* caption_utf8 = RPG_Client_UI_Tools::Locale2UTF8(caption);
// *TODO*: this will not return on VS2010...
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
        SDL_WM_SetCaption(caption_utf8,  // window caption
                          caption_utf8); // icon caption
#endif
        // clean up
        g_free(caption_utf8);

        refresh_window = true;

        break;
      }
      case COMMAND_WINDOW_REFRESH:
      {
        refresh_window = true;

        break;
      }
      case COMMAND_WINDOW_UPDATE_MESSAGEWINDOW:
      {
        // sanity check(s)
        ACE_ASSERT(myWindow);
        ACE_ASSERT(!(*iterator).message.empty());

        RPG_Client_IWindowLevel* level_window = dynamic_cast<RPG_Client_IWindowLevel*>(myWindow);
        ACE_ASSERT(level_window);
        try
        {
          level_window->updateMessageWindow((*iterator).message);
        }
        catch (...)
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("caught exception in [%@]: RPG_Client_IWindowLevel::updateMessageWindow(\"%s\"), aborting\n"),
                     level_window,
                     ACE_TEXT((*iterator).message.c_str())));

          return;
        }

        refresh_window = true;

        break;
      }
      case COMMAND_WINDOW_UPDATE_MINIMAP:
      {
        // sanity check
        ACE_ASSERT(myWindow);

        RPG_Client_IWindowLevel* level_window = dynamic_cast<RPG_Client_IWindowLevel*>(myWindow);
        ACE_ASSERT(level_window);
        try
        {
          level_window->updateMinimap();
        }
        catch (...)
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("caught exception in [%@]: RPG_Client_IWindowLevel::updateMinimap(), aborting\n"),
                     level_window));

          return;
        }

        refresh_window = true;

        break;
      }
      default:
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("invalid action (was: %d), continuing\n"),
                   (*iterator).command));

        break;
      }
    } // end SWITCH

    // redraw UI ?
    if (refresh_window)
    {
      // sanity check
      ACE_ASSERT((*iterator).window);

      try
      {
        (*iterator).window->refresh();
      }
      catch (...)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("[%@] caught exception in RPG_Graphics_IWindow::refresh(), continuing\n"),
                   (*iterator).window));
      }
    } // end IF
  } // end FOR

  myActions.clear();
}
