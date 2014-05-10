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
//  : myQueue(RPG_CLIENT_MAX_QUEUE_SLOTS),
 : myCondition(myLock),
   myStop(false),
   myEngine(NULL),
   myWindow(NULL),
   myWidgets(NULL),
//   myActions(),
//   mySeenPositions(),
   mySelectionMode(SELECTIONMODE_NORMAL),
   myCenterOnActivePlayer(RPG_CLIENT_DEF_CENTER_ON_ACTIVE_PLAYER)//,
//   myScreenLock()
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Engine::RPG_Client_Engine"));

  // use member message queue...
//   inherited::msg_queue(&myQueue);

	// set group ID for worker thread(s)
  inherited::grp_id(RPG_CLIENT_TASK_GROUP_ID);
}

RPG_Client_Engine::~RPG_Client_Engine()
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Engine::~RPG_Client_Engine"));

	if (isRunning())
		stop();
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

      return 0;
    }
    case 1:
    {
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

  ACE_ASSERT(false);
#if defined (_MSC_VER)
  return -1;
#else
  ACE_NOTREACHED(return -1;)
#endif
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

  ACE_ASSERT(false);
#if defined (_MSC_VER)
  return -1;
#else
  ACE_NOTREACHED(return -1;)
#endif
}

void
RPG_Client_Engine::start()
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Engine::start"));

  // sanity check
  if (isRunning())
    return;

  // OK: start worker thread
  ACE_hthread_t thread_handles[1];
  thread_handles[0] = 0;
  ACE_thread_t thread_ids[1];
  thread_ids[0] = 0;
	const char* thread_names[1];
	thread_names[0] = ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_TASK_THREAD_NAME);
  // *IMPORTANT NOTE*: MUST be THR_JOINABLE !!!
  int ret = 0;
  ret = inherited::activate((THR_NEW_LWP |
                             THR_JOINABLE |
                             THR_INHERIT_SCHED),         // flags
                            1,                           // number of threads
                            0,                           // force spawning
                            ACE_DEFAULT_THREAD_PRIORITY, // priority
                            inherited::grp_id(),         // group id --> has been set (see above)
                            NULL,                        // corresp. task --> use 'this'
                            thread_handles,              // thread handle(s)
                            NULL,                        // thread stack(s)
                            NULL,                        // thread stack size(s)
                            thread_ids,                  // thread id(s)
														thread_names);               // thread name(s)
  if (ret == -1)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Task_Base::activate(): \"%m\", continuing\n")));
  else
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("(graphics engine) started worker thread (group: %d, id: %u)...\n"),
               inherited::grp_id(),
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
             ACE_TEXT("joined (graphics engine) worker thread...\n")));
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
RPG_Client_Engine::lock()
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Engine::lock"));

  if (myScreenLock.acquire(NULL) == -1)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Thread_Mutex::acquire: \"%m\", continuing\n")));
}

void
RPG_Client_Engine::unlock()
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Engine::unlock"));

  if (myScreenLock.release() == -1)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Thread_Mutex::release: \"%m\", continuing\n")));
}

void
RPG_Client_Engine::initMap()
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Engine::initMap"));

  // sanity check
  ACE_ASSERT(myWindow);

  RPG_Client_Action new_action;
  new_action.command = COMMAND_WINDOW_INIT;
  new_action.position =
      std::make_pair(std::numeric_limits<unsigned int>::max(),
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
  new_action.position =
      std::make_pair(std::numeric_limits<unsigned int>::max(),
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
  client_action.previous =
      std::make_pair(std::numeric_limits<unsigned int>::max(),
                     std::numeric_limits<unsigned int>::max());
  client_action.position =
      std::make_pair(std::numeric_limits<unsigned int>::max(),
                     std::numeric_limits<unsigned int>::max());
  client_action.window = NULL;
  client_action.cursor = RPG_GRAPHICS_CURSOR_INVALID;
  client_action.entity_id = 0;
  client_action.sound = RPG_SOUND_EVENT_INVALID;
  client_action.source =
      std::make_pair(std::numeric_limits<unsigned int>::max(),
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
      ACE_ASSERT(client_action.position !=
          std::make_pair(std::numeric_limits<unsigned int>::max(),
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

        RPG_Client_SeenPositionsIterator_t iterator =
            mySeenPositions.find(client_action.entity_id);
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
        type.sprite =
            RPG_Client_Common_Tools::monster2Sprite(myEngine->getName(client_action.entity_id, true));
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

        ACE_ASSERT(mySeenPositions.find(client_action.entity_id) ==
                   mySeenPositions.end());
        mySeenPositions.insert(std::make_pair(client_action.entity_id,
                                              seen_positions));
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
      client_action.sound =
          ((command_in == COMMAND_E2C_ENTITY_HIT) ? EVENT_SWORD_HIT
                                                  : EVENT_SWORD_MISS);

      break;
    }
    case COMMAND_E2C_ENTITY_POSITION:
    {
      client_action.entity_id = parameters_in.entity_id;
      ACE_ASSERT(client_action.entity_id);
      client_action.position = parameters_in.position;
      ACE_ASSERT(client_action.position !=
          std::make_pair(std::numeric_limits<unsigned int>::max(),
                         std::numeric_limits<unsigned int>::max()));
      client_action.previous = parameters_in.previous_position;
      ACE_ASSERT(client_action.previous !=
          std::make_pair(std::numeric_limits<unsigned int>::max(),
                         std::numeric_limits<unsigned int>::max()));
      client_action.window = myWindow;

      // *NOTE*: when using (dynamic) lighting, redraw the whole window...
      RPG_Engine_EntityID_t active_entity_id = myEngine->getActive(true);
      client_action.command =
          ((client_action.entity_id == active_entity_id) ? COMMAND_WINDOW_DRAW
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

        client_action.command = COMMAND_CURSOR_INVALIDATE_BG;
        action(client_action);

        if (getCenterOnActive())
          client_action.command = COMMAND_SET_VIEW;
        else
          client_action.command = COMMAND_WINDOW_DRAW;
        action(client_action);

        SDL_Rect map_area;
        myWindow->getArea(map_area);
        RPG_Client_IWindowLevel* level_window =
            dynamic_cast<RPG_Client_IWindowLevel*>(myWindow);
        ACE_ASSERT(level_window);
        client_action.position =
            RPG_Graphics_Common_Tools::screen2Map(RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->position(),
                                                  myEngine->getSize(true),
                                                  std::make_pair(map_area.w,
                                                                 map_area.h),
                                                  level_window->getView());

//        client_action.command = COMMAND_TILE_HIGHLIGHT_STORE_BG;
//        action(client_action);

        client_action.command = COMMAND_TILE_HIGHLIGHT_DRAW;
        action(client_action);

        client_action.position =
            RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->position();
        client_action.command = COMMAND_CURSOR_DRAW;
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

        ACE_ASSERT(mySeenPositions.find(client_action.entity_id) !=
            mySeenPositions.end());
        mySeenPositions[client_action.entity_id].insert(seen_positions.begin(),
                                                        seen_positions.end());
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

        ACE_ASSERT(mySeenPositions.find(client_action.entity_id) !=
            mySeenPositions.end());
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

//      client_action.command = COMMAND_TILE_HIGHLIGHT_STORE_BG;
//      action(client_action);

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

        ACE_ASSERT(mySeenPositions.find(client_action.entity_id) !=
            mySeenPositions.end());
        mySeenPositions[client_action.entity_id].insert(seen_positions.begin(),
                                                        seen_positions.end());
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
        gdk_threads_enter();
        GtkWidget* widget = glade_xml_get_widget(myWidgets,
                                                 ACE_TEXT_ALWAYS_CHAR("part"));
        ACE_ASSERT(widget);
        // raise dialog window
        GdkWindow* toplevel = gtk_widget_get_parent_window(widget);
        ACE_ASSERT(toplevel);
        gdk_window_deiconify(toplevel);
        // emit a signal...
        gtk_button_clicked(GTK_BUTTON(widget));
        gdk_threads_leave();
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
                        RPG_Graphics_IWindowBase* window_in,
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

  RPG_Client_SeenPositionsConstIterator_t iterator =
      mySeenPositions.find(id_in);
  if (iterator == mySeenPositions.end())
  {
    if (lockedAccess_in)
      myLock.release();
    
    return false;
  } // end IF

  RPG_Map_PositionsConstIterator_t iterator2 =
      (*iterator).second.find(position_in);
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

  SDL_Rect dirty_region;
  ACE_OS::memset(&dirty_region, 0, sizeof(dirty_region));
  for (RPG_Client_ActionsIterator_t iterator = myActions.begin();
       iterator != myActions.end();
       iterator++)
  {
    switch ((*iterator).command)
    {
      // sanity check(s)
      ACE_ASSERT((*iterator).window);

      case COMMAND_CURSOR_DRAW:
      {
        RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->putCursor((*iterator).position,
                                                                     dirty_region);
        (*iterator).window->invalidate(dirty_region);

        break;
      }
      case COMMAND_CURSOR_INVALIDATE_BG:
      {
        RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->invalidateBG();

        break;
      }
      case COMMAND_CURSOR_SET:
      {
        // sanity check(s)
        ACE_ASSERT((*iterator).window);
        RPG_Graphics_Cursor current_type =
            RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->type();
        if (current_type == (*iterator).cursor)
          break; // nothing to do...

        // set new cursor
        RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->setCursor((*iterator).cursor,
                                                                     dirty_region);
        (*iterator).window->invalidate(dirty_region);

        break;
      }
      case COMMAND_ENTITY_DRAW:
      {
        // sanity check
        ACE_ASSERT((*iterator).position !=
            std::make_pair(std::numeric_limits<unsigned int>::max(),
                           std::numeric_limits<unsigned int>::max()));
        ACE_ASSERT((*iterator).window);
        ACE_ASSERT((*iterator).entity_id);

        SDL_Rect map_area;
        (*iterator).window->getArea(map_area);
        RPG_Client_IWindowLevel* level_window =
            dynamic_cast<RPG_Client_IWindowLevel*>((*iterator).window);
        ACE_ASSERT(level_window);
        RPG_CLIENT_ENTITY_MANAGER_SINGLETON::instance()->put((*iterator).entity_id,
                                                             RPG_Graphics_Common_Tools::map2Screen((*iterator).position,
                                                                                                   std::make_pair(map_area.w,
                                                                                                                  map_area.h),
                                                                                                   level_window->getView()),
                                                             dirty_region);
        (*iterator).window->invalidate(dirty_region);

        break;
      }
      case COMMAND_ENTITY_REMOVE:
      {
        // sanity check
        ACE_ASSERT((*iterator).entity_id);
        ACE_ASSERT((*iterator).window);

        RPG_CLIENT_ENTITY_MANAGER_SINGLETON::instance()->remove((*iterator).entity_id,
                                                                dirty_region);
        (*iterator).window->invalidate(dirty_region);

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
        ACE_ASSERT((*iterator).window);
        RPG_Client_IWindowLevel* level_window =
            dynamic_cast<RPG_Client_IWindowLevel*>((*iterator).window);
        ACE_ASSERT(level_window);

        try
        {
          level_window->setView((*iterator).position);
//          myWindow->clear();
          level_window->draw();
        }
        catch (...)
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("caught exception in [%@]: RPG_Client_IWindowLevel::setView([%u,%u])/clear/draw(), aborting\n"),
                     level_window,
                     (*iterator).position.first, (*iterator).position.second));

          return;
        }

        // --> update whole window
        (*iterator).window->invalidate(dirty_region);

        break;
      }
      case COMMAND_SET_VISION_RADIUS:
      {
        // sanity check(s)
				ACE_ASSERT((*iterator).window);

        RPG_Client_IWindowLevel* level_window =
            dynamic_cast<RPG_Client_IWindowLevel*>(myWindow);
        ACE_ASSERT(level_window);
        try
        {
          level_window->setBlendRadius((*iterator).radius);
//          myWindow->clear();
          level_window->draw();
        }
        catch (...)
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("caught exception in [%@]: RPG_Client_IWindowLevel::setBlendRadius(%u)/clear/draw(), aborting\n"),
                     level_window,
                     (*iterator).radius));

          return;
        }

        // --> update whole window
        (*iterator).window->invalidate(dirty_region);

        break;
      }
      case COMMAND_TILE_HIGHLIGHT_DRAW:
      {
        // sanity check(s)
				ACE_ASSERT((*iterator).window);

        SDL_Rect map_area;
        (*iterator).window->getArea(map_area);
        RPG_Client_IWindowLevel* level_window =
            dynamic_cast<RPG_Client_IWindowLevel*>((*iterator).window);
        ACE_ASSERT(level_window);
        RPG_Graphics_Position_t view = level_window->getView();
        if ((*iterator).path.empty() &&
            (*iterator).positions.empty())
        {
          RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->putHighlight((*iterator).position,
                                                                          RPG_Graphics_Common_Tools::map2Screen((*iterator).position,
                                                                                                                std::make_pair(map_area.w,
                                                                                                                               map_area.h),
                                                                                                                view),
                                                                          view,
                                                                          dirty_region);
        } // end IF
        else if (!(*iterator).path.empty())
        {
          ACE_ASSERT((*iterator).positions.empty());
          RPG_Map_PositionList_t map_positions;
          RPG_Graphics_Offsets_t graphics_positions;
          for (RPG_Map_PathConstIterator_t iterator2 = (*iterator).path.begin();
               iterator2 != (*iterator).path.end();
               iterator2++)
          {
            map_positions.push_back((*iterator2).first);
            graphics_positions.push_back(RPG_Graphics_Common_Tools::map2Screen((*iterator2).first,
                                                                               std::make_pair(map_area.w,
                                                                                              map_area.h),
                                                                               view));
          } // end FOR
          RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->putHighlights(map_positions,
                                                                           graphics_positions,
                                                                           view,
                                                                           dirty_region);
        } // end ELSE
        else
        {
          ACE_ASSERT(!(*iterator).positions.empty());
          RPG_Map_PositionList_t map_positions;
          RPG_Graphics_Offsets_t graphics_positions;
          for (RPG_Map_PositionsConstIterator_t iterator2 =
               (*iterator).positions.begin();
               iterator2 != (*iterator).positions.end();
               iterator2++)
          {
            map_positions.push_back(*iterator2);
            graphics_positions.push_back(RPG_Graphics_Common_Tools::map2Screen(*iterator2,
                                                                               std::make_pair(map_area.w,
                                                                                              map_area.h),
                                                                               view));
          } // end FOR
          RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->putHighlights(map_positions,
                                                                           graphics_positions,
                                                                           view,
                                                                           dirty_region);
        } // end ELSE

        (*iterator).window->invalidate(dirty_region);

        break;
      }
      case COMMAND_TILE_HIGHLIGHT_INVALIDATE_BG:
      {
        RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->resetHighlightBG((*iterator).position);

        break;
      }
      case COMMAND_TILE_HIGHLIGHT_RESTORE_BG:
      {
        // sanity check(s)
        ACE_ASSERT((*iterator).window);
        RPG_Client_IWindowLevel* level_window =
            dynamic_cast<RPG_Client_IWindowLevel*>((*iterator).window);
        ACE_ASSERT(level_window);

        RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->restoreHighlightBG(level_window->getView(),
                                                                              dirty_region);

        (*iterator).window->invalidate(dirty_region);

        break;
      }
//      case COMMAND_TILE_HIGHLIGHT_STORE_BG:
//      {
//        // sanity check(s)
//        ACE_ASSERT((*iterator).window);

//        RPG_Graphics_Size_t size = (*iterator).window->getSize(false);
//        RPG_Client_IWindowLevel* level_window =
//            dynamic_cast<RPG_Client_IWindowLevel*>((*iterator).window);
//        ACE_ASSERT(level_window);
//        RPG_Graphics_Position_t view = level_window->getView();
//        if ((*iterator).path.empty() &&
//            (*iterator).positions.empty())
//        {
//          RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->storeHighlightBG((*iterator).position,
//                                                                              RPG_Graphics_Common_Tools::map2Screen((*iterator).position,
//                                                                                                                    size,
//                                                                                                                    view));
//        }
//        else if (!(*iterator).path.empty())
//        {
//          ACE_ASSERT((*iterator).positions.empty());
//          RPG_Map_PositionList_t map_positions;
//          RPG_Graphics_Positions_t graphics_positions;
//          for (RPG_Map_PathConstIterator_t iterator2 = (*iterator).path.begin();
//               iterator2 != (*iterator).path.end();
//               iterator2++)
//          {
//            map_positions.push_back((*iterator2).first);
//            graphics_positions.push_back(RPG_Graphics_Common_Tools::map2Screen((*iterator2).first,
//                                                                               size,
//                                                                               view));
//          } // end FOR
//          RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->storeHighlightBG(map_positions,
//                                                                              graphics_positions);
//        } // end ELSE
//        else
//        {
//          ACE_ASSERT(!(*iterator).positions.empty());
//          RPG_Map_PositionList_t map_positions;
//          RPG_Graphics_Positions_t graphics_positions;
//          for (RPG_Map_PositionsConstIterator_t iterator2 = (*iterator).positions.begin();
//               iterator2 != (*iterator).positions.end();
//               iterator2++)
//          {
//            map_positions.push_back(*iterator2);
//            graphics_positions.push_back(RPG_Graphics_Common_Tools::map2Screen(*iterator2,
//                                                                               size,
//                                                                               view));
//          } // end FOR
//          RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->storeHighlightBG(map_positions,
//                                                                              graphics_positions);
//        } // end ELSE

//        break;
//      }
      case COMMAND_TOGGLE_DOOR:
      {
        // sanity check(s)
        ACE_ASSERT((*iterator).window);
        RPG_Client_IWindowLevel* level_window =
            dynamic_cast<RPG_Client_IWindowLevel*>((*iterator).window);
        ACE_ASSERT(level_window);

        try
        {
          level_window->toggleDoor((*iterator).position);
//          myWindow->clear();
          level_window->draw();
        }
        catch (...)
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("caught exception in [%@]: RPG_Client_IWindowLevel::toggleDoor/clear/draw(), aborting\n"),
                     level_window));

          return;
        }

        // --> update whole window
        (*iterator).window->invalidate(dirty_region);

        break;
      }
      case COMMAND_WINDOW_BORDER_DRAW:
      {
        // sanity check
        ACE_ASSERT((*iterator).window);
        RPG_Client_IWindow* client_window =
            dynamic_cast<RPG_Client_IWindow*>((*iterator).window);
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

        // --> update whole window
        (*iterator).window->invalidate(dirty_region);

        break;
      }
      case COMMAND_WINDOW_DRAW:
      {
        // sanity check
        ACE_ASSERT((*iterator).window);

        try
        {
//          (*iterator).window->clear();
          (*iterator).window->draw();
        }
        catch (...)
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("caught exception in [%@]: RPG_Graphics_IWindow::draw(), aborting\n"),
                     (*iterator).window));

          return;
        }

        // --> update whole window
        (*iterator).window->invalidate(dirty_region);

        break;
      }
      case COMMAND_WINDOW_INIT:
      {
        // sanity check(s)
        ACE_ASSERT((*iterator).window);
        RPG_Client_IWindowLevel* level_window =
            dynamic_cast<RPG_Client_IWindowLevel*>((*iterator).window);
        ACE_ASSERT(level_window);

        // step0: reset bg caches
        RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->invalidateBG();
        RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->resetHighlightBG(std::make_pair(std::numeric_limits<unsigned int>::max(),
                                                                                           std::numeric_limits<unsigned int>::max()));

        // step1: init/(re)draw window
        myEngine->lock();
        RPG_Graphics_MapStyle level_style = myEngine->getStyle(false);
        RPG_Map_Position_t center = myEngine->getSize(false);
        myEngine->unlock();
        center.first >>= 1;
        center.second >>= 1;
        try
        {
          level_window->init(level_style);
          level_window->setView(center);
//          level_window->clear();
          level_window->draw();
        }
        catch (...)
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("caught exception in [%@]: RPG_Client_IWindowLevel::init/setView/clear/draw(), aborting\n"),
                     level_window));

          return;
        }

        // step2: (re)set window title caption/iconify
        std::string caption =
            ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_DEF_GRAPHICS_MAINWINDOW_TITLE);
        const std::string& level_name = myEngine->getMetaData(true).name;
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

        // --> update whole window
        (*iterator).window->invalidate(dirty_region);

        break;
      }
      case COMMAND_WINDOW_REFRESH:
      {
        // sanity check(s)
        ACE_ASSERT((*iterator).window);

        // --> update whole window
        (*iterator).window->invalidate(dirty_region);

        break;
      }
      case COMMAND_WINDOW_UPDATE_MESSAGEWINDOW:
      {
        // sanity check(s)
        ACE_ASSERT((*iterator).window);
        ACE_ASSERT(!(*iterator).message.empty());
        RPG_Client_IWindowLevel* level_window =
            dynamic_cast<RPG_Client_IWindowLevel*>((*iterator).window);
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

        // --> update whole window
        (*iterator).window->invalidate(dirty_region);

        break;
      }
      case COMMAND_WINDOW_UPDATE_MINIMAP:
      {
        // sanity check
        ACE_ASSERT((*iterator).window);
        RPG_Client_IWindowLevel* level_window =
            dynamic_cast<RPG_Client_IWindowLevel*>((*iterator).window);
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

        // --> update whole window
        (*iterator).window->invalidate(dirty_region);

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

    // update screen
    ACE_ASSERT((*iterator).window);
    try
    {
      (*iterator).window->update();
    }
    catch (...)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("[%@] caught exception in RPG_Graphics_IWindow::update(), continuing\n"),
                 (*iterator).window));
    }
  } // end FOR

  myActions.clear();
}
