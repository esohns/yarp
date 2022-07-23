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

#include "ace/Log_Msg.h"

#include "common_ui_gtk_tools.h"

#include "rpg_common_defines.h"
#include "rpg_common_file_tools.h"
#include "rpg_common_macros.h"

#include "rpg_engine.h"

#include "rpg_sound_common.h"
#include "rpg_sound_common_tools.h"

#include "rpg_graphics_common_tools.h"
#include "rpg_graphics_cursor_manager.h"
#include "rpg_graphics_defines.h"
#include "rpg_graphics_surface.h"

#include "rpg_client_common_tools.h"
#include "rpg_client_defines.h"
#include "rpg_client_entity_manager.h"
#include "rpg_client_iwindow.h"
#include "rpg_client_iwindow_level.h"

RPG_Client_Engine::RPG_Client_Engine ()
//  : myQueue(RPG_CLIENT_MAX_QUEUE_SLOTS),
 : inherited (ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_ENGINE_THREAD_NAME),
              RPG_CLIENT_ENGINE_THREAD_GROUP_ID,
              1,
              false,
              NULL),
   condition_ (lock_),
   shutDown_ (false),
   engine_ (NULL),
   window_ (NULL),
   //myWidgetInterface(NULL),
   actions_ (),
   state_ (),
//   mySeenPositions(),
   selectionMode_ (SELECTIONMODE_NORMAL),
   centerOnActivePlayer_ (RPG_CLIENT_DEF_CENTER_ON_ACTIVE_PLAYER),
   screenLock_ (),
   debug_ (RPG_CLIENT_DEF_DEBUG)
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Engine::RPG_Client_Engine"));

  // use member message queue...
//   inherited::msg_queue(&myQueue);

  state_.style.door = RPG_CLIENT_GRAPHICS_DEF_DOORSTYLE;
  state_.style.edge = RPG_CLIENT_GRAPHICS_DEF_EDGESTYLE;
  state_.style.floor = RPG_CLIENT_GRAPHICS_DEF_FLOORSTYLE;
  state_.style.half_height_walls = RPG_CLIENT_GRAPHICS_DEF_WALLSTYLE_HALF;
  state_.style.wall = RPG_CLIENT_GRAPHICS_DEF_WALLSTYLE;

  // set group ID for worker thread(s)
  inherited::grp_id (RPG_CLIENT_ENGINE_THREAD_GROUP_ID);
}

RPG_Client_Engine::~RPG_Client_Engine ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Engine::~RPG_Client_Engine"));

  if (isRunning ())
    stop (false, false);
}

int
RPG_Client_Engine::close (u_long arg_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Engine::close"));

  // *IMPORTANT NOTE*: this method may be invoked
  // [- by an external thread closing down the active object]
  // - by the worker thread which calls this after returning from svc()
  //    --> in this case, this should be a NOP...
  switch (arg_in)
  {
    // called from ACE_Task_Base on clean-up
    case 0:
    {
//      ACE_DEBUG((LM_DEBUG,
//                 ACE_TEXT("(%s) worker thread (ID: %t) leaving...\n"),
//                 ACE_TEXT(RPG_CLIENT_ENGINE_THREAD_NAME)));

      return 0;
    }
    case 1:
    {
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid argument: %u, returning\n"),
                  arg_in));
      break;
    }
  } // end SWITCH

  ACE_ASSERT (false);
#if defined (_MSC_VER)
  return -1;
#else
  ACE_NOTREACHED (return -1;)
#endif // _MSC_VER
}

int
RPG_Client_Engine::svc (void)
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Engine::svc"));

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#if COMMON_OS_WIN32_TARGET_PLATFORM(0x0A00) // _WIN32_WINNT_WIN10
  Common_Error_Tools::setThreadName (ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_ENGINE_THREAD_NAME),
                                     NULL);
#else
  Common_Error_Tools::setThreadName (ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_ENGINE_THREAD_NAME),
                                     0);
#endif // _WIN32_WINNT_WIN10
#endif // ACE_WIN32 || ACE_WIN64

  while (true)
  {
    { ACE_GUARD_RETURN (ACE_Thread_Mutex, aGuard, lock_, -1);
      while (likely (actions_.empty ()))
      {
        condition_.wait (); // wait for an action
        if (isShuttingDown ())
          return 0;
      } // end WHILE
    } // end lock scope
    handleActions();
  } // end WHILE

  ACE_ASSERT (false);
#if defined (_MSC_VER)
  return -1;
#else
  ACE_NOTREACHED (return -1;)
#endif // _MSC_VER
}

void
RPG_Client_Engine::dump_state () const
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Engine::dump_state"));

  ACE_Guard<ACE_Thread_Mutex> aGuard (lock_);

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("current queue size: %d\n"),
              actions_.size ()));
}

bool
RPG_Client_Engine::lock (bool block_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Engine::lock"));

  ACE_UNUSED_ARG (block_in);

  if (screenLock_.acquire (NULL) == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Thread_Mutex::acquire: \"%m\", continuing\n")));

  return true;
}

int
RPG_Client_Engine::unlock (bool unlock_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Engine::unlock"));

  ACE_UNUSED_ARG (unlock_in);

  if (screenLock_.release () == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Thread_Mutex::release: \"%m\", continuing\n")));

  return 0;
}

void
RPG_Client_Engine::redraw (bool refresh_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Engine::redraw"));

  // sanity check
  ACE_ASSERT (window_);

  // step1: draw map window
  RPG_Client_Action new_action;
  new_action.command = COMMAND_WINDOW_DRAW;
  new_action.position =
      std::make_pair (std::numeric_limits<unsigned int>::max (),
                      std::numeric_limits<unsigned int>::max ());
  new_action.window = window_;
  new_action.cursor = RPG_GRAPHICS_CURSOR_INVALID;
  new_action.entity_id = 0;
  new_action.radius = 0;

  action (new_action);

  // step2: refresh the window ?
  if (refresh_in)
  {
    new_action.command = COMMAND_WINDOW_REFRESH;
    action (new_action);
  } // end IF
}

void
RPG_Client_Engine::setView (const RPG_Map_Position_t& position_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Engine::setView"));

  // sanity check
  ACE_ASSERT (window_);

  RPG_Client_Action new_action;
  new_action.command = COMMAND_SET_VIEW;
  new_action.position = position_in;
  new_action.window = window_;
  new_action.cursor = RPG_GRAPHICS_CURSOR_INVALID;
  new_action.entity_id = 0;
  new_action.radius = 0;

  action (new_action);
}

void
RPG_Client_Engine::notify (enum RPG_Engine_Command command_in,
                           const struct RPG_Engine_ClientNotificationParameters& parameters_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Engine::notify"));

  bool do_action = true;
  RPG_Client_Action client_action;
  client_action.command = RPG_CLIENT_COMMAND_INVALID;
  client_action.previous =
      std::make_pair (std::numeric_limits<unsigned int>::max (),
                      std::numeric_limits<unsigned int>::max ());
  client_action.position =
      std::make_pair (std::numeric_limits<unsigned int>::max (),
                      std::numeric_limits<unsigned int>::max ());
  client_action.window = NULL;
  client_action.cursor = RPG_GRAPHICS_CURSOR_INVALID;
  client_action.entity_id = 0;
  client_action.sound = RPG_SOUND_EVENT_INVALID;
  client_action.source =
      std::make_pair (std::numeric_limits<unsigned int>::max (),
                      std::numeric_limits<unsigned int>::max ());
  client_action.radius = 0;

  switch (command_in)
  {
    case COMMAND_ATTACK:
    case COMMAND_ATTACK_FULL:
    case COMMAND_ATTACK_STANDARD:
    {
      do_action = false;
      break;
    }
    case COMMAND_DOOR_CLOSE:
    {
      client_action.sound = EVENT_DOOR_CLOSE;
      // *WARNING*: falls through !
    }
    case COMMAND_DOOR_OPEN:
    {
      // sanity check(s)
      ACE_ASSERT (!parameters_in.positions.empty ());
      client_action.position = *parameters_in.positions.begin ();
      ACE_ASSERT (client_action.position != std::make_pair (std::numeric_limits<unsigned int>::max (),
                                                            std::numeric_limits<unsigned int>::max ()));
      client_action.command = COMMAND_TOGGLE_DOOR;
      ACE_ASSERT (window_);
      client_action.window = window_;
      ACE_ASSERT (parameters_in.entity_id);
      client_action.entity_id = parameters_in.entity_id;
      action (client_action);

      client_action.command = COMMAND_PLAY_SOUND;
      if (command_in == COMMAND_DOOR_OPEN)
        client_action.sound = EVENT_DOOR_OPEN;

      break;
    }
    case COMMAND_RUN:
    case COMMAND_SEARCH:
    case COMMAND_STEP:
    case COMMAND_STOP:
    case COMMAND_TRAVEL:
    {
      do_action = false;

      break;
    }
    case COMMAND_E2C_ENTITY_ADD:
    {
      // sanity check(s)
      ACE_ASSERT (engine_);
      client_action.command = COMMAND_ENTITY_DRAW;
      ACE_ASSERT (!parameters_in.positions.empty ());
      client_action.position = *parameters_in.positions.begin();
      ACE_ASSERT (window_);
      client_action.window = window_;
      ACE_ASSERT (parameters_in.entity_id);
      client_action.entity_id = parameters_in.entity_id;

      // step1: load sprite graphics
      SDL_Surface* sprite_graphic = NULL;
      RPG_Graphics_GraphicTypeUnion type;
      type.discriminator = RPG_Graphics_GraphicTypeUnion::SPRITE;
      engine_->lock ();
      type.sprite =
          (engine_->isMonster (parameters_in.entity_id, false) ? RPG_Client_Common_Tools::monsterToSprite (engine_->getName (parameters_in.entity_id,
                                                                                                                             false))
                                                               : RPG_Client_Common_Tools::classToSprite (engine_->getClass (parameters_in.entity_id,
                                                                                                                            false)));
      engine_->unlock ();
      sprite_graphic = RPG_Graphics_Common_Tools::loadGraphic (type,   // sprite
                                                               true,   // convert to display format
                                                               false); // don't cache
      if (!sprite_graphic)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to RPG_Graphics_Common_Tools::loadGraphic(%s), returning\n"),
                    ACE_TEXT (RPG_Graphics_SpriteHelper::RPG_Graphics_SpriteToString (type.sprite).c_str ())));
        return;
      } // end IF
      RPG_CLIENT_ENTITY_MANAGER_SINGLETON::instance()->add (parameters_in.entity_id,
                                                            sprite_graphic,
                                                            false);

      // step3: draw the sprite --> delegate to the engine

      break;
    }
    case COMMAND_E2C_ENTITY_REMOVE:
    {
      // sanity check(s)
      // step1: erase the sprite --> delegate to the engine
      client_action.command = COMMAND_ENTITY_REMOVE;
      ACE_ASSERT (window_);
      client_action.window = window_;
      ACE_ASSERT (parameters_in.entity_id);
      client_action.entity_id = parameters_in.entity_id;
      action (client_action);

      // step3: play a sound ? --> delegate to the engine
      if (parameters_in.entity_id == engine_->getActive (true))
        do_action = false; // don't play a sound...

      client_action.command = COMMAND_PLAY_SOUND;
      client_action.sound = EVENT_CONDITION_WEAK;

      break;
    }
    case COMMAND_E2C_ENTITY_HIT:
    case COMMAND_E2C_ENTITY_MISS:
    { // sanity check(s)
      ACE_ASSERT (parameters_in.entity_id);
      client_action.entity_id = parameters_in.entity_id;
      client_action.command = COMMAND_PLAY_SOUND;
      client_action.sound =
        ((command_in == COMMAND_E2C_ENTITY_HIT) ? EVENT_SWORD_HIT
                                                : EVENT_SWORD_MISS);

      break;
    }
    case COMMAND_E2C_ENTITY_CONDITION:
    {
      client_action.command = COMMAND_PLAY_SOUND;
      bool entity_is_down_b = false;
      switch (parameters_in.condition)
      {
        case CONDITION_ALTERNATE_FORM:
        case CONDITION_NORMAL:
        case CONDITION_INCORPOREAL:
        case CONDITION_INVISIBLE:
        case CONDITION_STABLE:
          client_action.sound = EVENT_CONDITION_ILL; // *TODO*
          break;
        case CONDITION_DEAD:
        case CONDITION_DISABLED:
        case CONDITION_DYING:
        case CONDITION_UNCONSCIOUS:
        {
          entity_is_down_b = true;

          // *WARNING*: falls through
        }
        default:
        {
          client_action.sound = EVENT_CONDITION_WEAK;
          break;
        }
      } // end SWITCH

      if (entity_is_down_b && 
          parameters_in.entity_id == engine_->getActive (true))
      {
        // raise the UI
        SDL_Event sdl_event_u;
        sdl_event_u.type = SDL_KEYDOWN;
        sdl_event_u.key.keysym.sym = SDLK_u;
        if (SDL_PushEvent (&sdl_event_u) < 0)
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to SDL_PushEvent(): \"%s\", continuing\n"),
                      ACE_TEXT (SDL_GetError ())));

        // leave the game
        sdl_event_u.key.keysym.sym = SDLK_l;
        if (SDL_PushEvent (&sdl_event_u) < 0)
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to SDL_PushEvent(): \"%s\", continuing\n"),
                      ACE_TEXT (SDL_GetError ())));
      } // end IF

      break;
    }
    case COMMAND_E2C_ENTITY_POSITION:
    {
      // sanity check(s)
      ACE_ASSERT (engine_);

      ACE_ASSERT (parameters_in.entity_id);
      client_action.entity_id = parameters_in.entity_id;
      ACE_ASSERT (!parameters_in.positions.empty ());
      client_action.position  = *parameters_in.positions.begin();
      ACE_ASSERT (parameters_in.previous_position != std::make_pair (std::numeric_limits<unsigned int>::max (),
                                                                     std::numeric_limits<unsigned int>::max ()));
      client_action.previous  = parameters_in.previous_position;
      ACE_ASSERT (window_);
      client_action.window    = window_;

      // *NOTE*: when using (dynamic) lighting, redraw the whole window...
      RPG_Engine_EntityID_t active_entity_id = engine_->getActive (true);
      RPG_Client_IWindowLevel* window_p =
          dynamic_cast<RPG_Client_IWindowLevel*> (window_);
      ACE_ASSERT (window_p);

      if (parameters_in.entity_id == active_entity_id)
      {
        // *NOTE*: re-drawing the window will invalidate cursor/hightlight BG...
        client_action.command = COMMAND_TILE_HIGHLIGHT_INVALIDATE_BG;
        action(client_action);
        client_action.command = COMMAND_CURSOR_INVALIDATE_BG;
        action(client_action);

        if (getCenterOnActive ())
        {
          client_action.command = COMMAND_SET_VIEW;
          action (client_action);
        } // end IF
        else
        {
          SDL_Rect window_area, map_area;
          window_->getArea (window_area, true);
          window_->getArea (map_area, false);
          RPG_Graphics_Positions_t positions;
          positions.push_back (client_action.position);
          if (RPG_Client_Common_Tools::isVisible (positions,
                                                  std::make_pair (window_area.w,
                                                                  window_area.h),
                                                  window_p->getView (),
                                                  map_area,
                                                  false)) // all
          {
            client_action.command = COMMAND_WINDOW_DRAW;
            action (client_action);
          } // end IF
        } // end ELSE

        client_action.command = COMMAND_PLAY_SOUND;
        client_action.sound = EVENT_WALK;
        action (client_action);

//        client_action.command = COMMAND_TILE_HIGHLIGHT_STORE_BG;
//        action(client_action);
        //client_action.command = COMMAND_TILE_HIGHLIGHT_DRAW;
        //action (client_action);
        //client_action.command = COMMAND_CURSOR_DRAW;
        do_action = false;

      } // end IF
      else
      {
        // update the minimap ?
        if (active_entity_id &&
            engine_->hasSeen (active_entity_id,
                              client_action.position,
                              true))
        {
          client_action.command = COMMAND_WINDOW_UPDATE_MINIMAP;
          action (client_action);
        } // end IF

        // draw sprite ?
        SDL_Rect window_area, map_area;
        window_->getArea (window_area, true);
        window_->getArea (map_area, false);
        RPG_Graphics_Positions_t positions;
        positions.insert (positions.begin (),
                          parameters_in.positions.begin (),
                          parameters_in.positions.end ());
        if (RPG_Client_Common_Tools::isVisible (positions,
                                                std::make_pair (window_area.w,
                                                                window_area.h),
                                                window_p->getView (),
                                                map_area,
                                                false)) // all
        {
          engine_->lock ();
          RPG_Engine_EntityID_t active_entity_id = engine_->getActive (false);
          if ((active_entity_id == parameters_in.entity_id) ||
              (active_entity_id &&
               engine_->canSee (active_entity_id,
                                parameters_in.entity_id,
                                false))                     ||
              debug_)
            client_action.command = COMMAND_ENTITY_DRAW;
          else
            do_action = false;
          engine_->unlock ();
        } // end IF
        else
        {
          // entity has left field of view...
          RPG_Graphics_Positions_t positions;
          positions.push_back (parameters_in.previous_position);
          if (RPG_Client_Common_Tools::isVisible (positions,
                                                  std::make_pair (window_area.w,
                                                                  window_area.h),
                                                  window_p->getView (),
                                                  map_area,
                                                  false)) // all
            client_action.command = COMMAND_ENTITY_REMOVE;
          else
            do_action = false;
        } // end ELSE
      } // end ELSE

      break;
    }
    case COMMAND_E2C_ENTITY_VISION:
    {
      // sanity check(s)
      ACE_ASSERT (window_);
      client_action.window    = window_;
      ACE_ASSERT (parameters_in.entity_id);
      client_action.entity_id = parameters_in.entity_id;
      client_action.radius = parameters_in.visible_radius;

      // *NOTE*: re-drawing the window invalidates cursor/hightlight BG...
      client_action.command = COMMAND_TILE_HIGHLIGHT_INVALIDATE_BG;
      action (client_action);
      client_action.command = COMMAND_CURSOR_INVALIDATE_BG;
      action (client_action);

      client_action.command = COMMAND_SET_VISION_RADIUS;
      action (client_action);

      // update vision cache
      //{ ACE_Guard<ACE_Thread_Mutex> aGuard (lock_);
//        RPG_Engine_SeenPositionsIterator_t iterator =
//          mySeenPositions.find (parameters_in.entity_id);
//        ACE_ASSERT (iterator != mySeenPositions.end ());
//        std::set_difference (client_action.positions.begin (),
//                             client_action.positions.end (),
//                             (*iterator).second.begin (),
//                             (*iterator).second.end (),
//                             new_positions.begin ());
//        (*iterator).second.insert (client_action.positions.begin (),
//                                   client_action.positions.end ());
      //} // end lock scope

      // *NOTE*: schedule a draw iff any of the new positions are currently
      //         visible on-screen
      SDL_Rect window_area, map_area;
      window_->getArea (window_area, true);
      window_->getArea (map_area, false);
      RPG_Client_IWindowLevel* window_p =
          dynamic_cast<RPG_Client_IWindowLevel*> (window_);
      ACE_ASSERT (window_p);
      if (RPG_Client_Common_Tools::isVisible (RPG_Client_Common_Tools::mapToGraphicsPositions (client_action.positions),
                                              std::make_pair (window_area.w,
                                                              window_area.h),
                                              window_p->getView (),
                                              map_area,
                                              true)) // any
      {
        client_action.command = COMMAND_WINDOW_DRAW;
        action (client_action);
      } // end IF

      client_action.command = COMMAND_TILE_HIGHLIGHT_DRAW;
      action (client_action);
      client_action.command = COMMAND_CURSOR_DRAW;
      action (client_action);

      break;
    }
    case COMMAND_E2C_ENTITY_LEVEL_UP:
    {
      client_action.command = COMMAND_PLAY_SOUND;
      client_action.sound = EVENT_XP_LEVELUP;

      break;
    }
    case COMMAND_E2C_INIT:
    {
      // sanity check(s)
      client_action.command = COMMAND_WINDOW_INIT;
      ACE_ASSERT (window_);
      client_action.window = window_;

      break;
    }
    case COMMAND_E2C_MESSAGE:
    {
      // sanity check(s)
      client_action.command = COMMAND_WINDOW_UPDATE_MESSAGEWINDOW;
      ACE_ASSERT (window_);
      client_action.window = window_;
      client_action.message = parameters_in.message;

      break;
    }
    case COMMAND_E2C_QUIT:
    {
      //if (myWidgetInterface)
      //{
      //  try {
      //    myWidgetInterface->finalize ();
      //  } catch (...) {
      //    ACE_DEBUG ((LM_ERROR,
      //                ACE_TEXT ("caught exception in RPG_Client_IWidgetUI_t::finalize(), continuing\n")));
      //  }
      //} // end IF

      do_action = false;

      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid command (was: \"%s\", returning\n"),
                  ACE_TEXT (RPG_Engine_CommandHelper::RPG_Engine_CommandToString (command_in).c_str ())));

      return;
    }
  } // end SWITCH

  if (do_action)
    action (client_action);
}

void
RPG_Client_Engine::initialize (RPG_Engine* engine_in,
                               RPG_Graphics_IWindowBase* window_in,
                               //RPG_Client_IWidgetUI_t* widgetInterface_in,
                               bool debug_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Engine::initialize"));

  // sanity check(s)
  ACE_ASSERT (engine_in);
  ACE_ASSERT (window_in);
  //ACE_ASSERT(widgetInterface_in);

  engine_ = engine_in;
  window_ = window_in;
  //myWidgetInterface = widgetInterface_in;
  debug_ = debug_in;
}

void
RPG_Client_Engine::action (const RPG_Client_Action& action_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Engine::action"));

  ACE_Guard<ACE_Thread_Mutex> aGuard (lock_);

  actions_.push_back (action_in);

  // wake up the (waiting) worker thread(s)
  condition_.broadcast ();
}

void
RPG_Client_Engine::centerOnActive (bool centerOnActive_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Engine::centerOnActive"));

  ACE_Guard<ACE_Thread_Mutex> aGuard (lock_);

  centerOnActivePlayer_ = centerOnActive_in;
}

bool
RPG_Client_Engine::getCenterOnActive () const
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Engine::getCenterOnActive"));

  bool result;

  { ACE_Guard<ACE_Thread_Mutex> aGuard (lock_);
    result = centerOnActivePlayer_;
  } // end lock scope

  return result;
}

void
RPG_Client_Engine::handleActions ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Engine::handleActions"));

  struct RPG_Client_Action client_action;
  SDL_Rect dirty_region;

next:
  { ACE_GUARD (ACE_Thread_Mutex, aGuard, lock_);
    if (actions_.empty ())
      return; // done (for now)
    client_action = actions_.front ();
    actions_.pop_front ();
  } // end lock scope

  ACE_OS::memset (&dirty_region, 0, sizeof (SDL_Rect));

  switch (client_action.command)
  {
    case COMMAND_CURSOR_DRAW:
    { ACE_ASSERT (client_action.window);
      RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance ()->putCursor (client_action.position,
                                                                     dirty_region);
      try {
        client_action.window->invalidate (dirty_region);
      } catch (...) {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("caught exception in RPG_Graphics_IWindowBase::invalidate(), continuing\n")));
        goto continue_;
      }

      break;
    }
    case COMMAND_CURSOR_INVALIDATE_BG:
    {
      RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance ()->invalidateBG ();
      break;
    }
    case COMMAND_CURSOR_SET:
    { ACE_ASSERT (client_action.window);
      RPG_Graphics_Cursor current_type =
        RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance ()->type ();
      if (current_type == client_action.cursor)
        break; // nothing to do...

      // set new cursor
      RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance ()->setCursor (client_action.cursor,
                                                                     dirty_region);
      try {
        client_action.window->invalidate (dirty_region);
      } catch (...) {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("caught exception in RPG_Graphics_IWindowBase::invalidate(), continuing\n")));
        goto continue_;
      }

      break;
    }
    case COMMAND_ENTITY_DRAW:
    { ACE_ASSERT (client_action.entity_id);
      ACE_ASSERT (client_action.position != std::make_pair (std::numeric_limits<unsigned int>::max (),
                                                            std::numeric_limits<unsigned int>::max ()));
      ACE_ASSERT (client_action.window);

      SDL_Rect window_area;
      client_action.window->getArea (window_area, true);
      RPG_Client_IWindowLevel* level_window = NULL;
      try {
        level_window =
            dynamic_cast<RPG_Client_IWindowLevel*> (client_action.window);
      } catch (...) {
        level_window = NULL;
      }
      if (!level_window)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to dynamic_cast<RPG_Client_IWindowLevel*>(%@), continuing\n"),
                    client_action.window));
        goto continue_;
      }
      RPG_CLIENT_ENTITY_MANAGER_SINGLETON::instance ()->put (client_action.entity_id, // entity ID
                                                             RPG_Graphics_Common_Tools::mapToScreen (client_action.position,
                                                                                                     std::make_pair (window_area.w,
                                                                                                                     window_area.h),
                                                                                                     level_window->getView ()), // position
                                                             dirty_region,            // return value: "dirty" region
                                                             true,                    // clip window ?
                                                             false,                   // locked access ? (screen-lock)
                                                             debug_);                 // debug ?
      try {
        client_action.window->invalidate (dirty_region);
      } catch (...) {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("caught exception in RPG_Graphics_IWindowBase::invalidate(), continuing\n")));
        goto continue_;
      }

      break;
    }
    case COMMAND_ENTITY_REMOVE:
    { ACE_ASSERT (client_action.entity_id);
      ACE_ASSERT (client_action.window);
      RPG_CLIENT_ENTITY_MANAGER_SINGLETON::instance ()->remove (client_action.entity_id, // entity ID
                                                                dirty_region,            // return value: "dirty" region
                                                                false,                   // locked access ? (screen-lock)
                                                                debug_);                 // debug ?
      try {
        client_action.window->invalidate (dirty_region);
      } catch (...) {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("caught exception in RPG_Graphics_IWindowBase::invalidate(), continuing\n")));
        goto continue_;
      }

      break;
    }
    case COMMAND_PLAY_SOUND:
    { ACE_ASSERT (client_action.sound != RPG_SOUND_EVENT_INVALID);
      int channel = -1;
      ACE_Time_Value length = ACE_Time_Value::zero;
      channel = RPG_Sound_Common_Tools::play (client_action.sound, length);
      if (channel == -1)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to play sound (was: \"%s\"), continuing\n"),
                    ACE_TEXT (RPG_Sound_EventHelper::RPG_Sound_EventToString (client_action.sound).c_str ())));
        goto continue_;
      } // end IF

      break;
    }
    case COMMAND_SET_VIEW:
    { ACE_ASSERT (client_action.window);
      RPG_Client_IWindowLevel* level_window = NULL;
      try {
        level_window =
          dynamic_cast<RPG_Client_IWindowLevel*> (client_action.window);
      } catch (...) {
        level_window = NULL;
      }
      if (!level_window)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to dynamic_cast<RPG_Client_IWindowLevel*>(%@), continuing\n"),
                    client_action.window));
        goto continue_;
      }
      try {
        level_window->setView (client_action.position);
        level_window->draw ();
      } catch (...) {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("caught exception in [%@]: RPG_Client_IWindowLevel::setView([%u,%u])/clear/draw(), continuing\n"),
                    level_window,
                    client_action.position.first, client_action.position.second));
        goto continue_;
      }

      // --> update whole window (*NOTE*: the draw() call above invalidates the whole window)
      //try {
      //  (*iterator).window->invalidate (dirty_region);
      //} catch (...) {
      //  ACE_DEBUG ((LM_ERROR,
      //              ACE_TEXT ("caught exception in RPG_Graphics_IWindowBase::invalidate(), continuing\n")));
      //  continue;
      //}

      break;
    }
    case COMMAND_SET_VISION_RADIUS:
    { ACE_ASSERT (engine_);
      ACE_ASSERT(client_action.entity_id);
      RPG_Engine_EntityID_t active_entity_id = engine_->getActive (true);
      if (!active_entity_id ||
          (active_entity_id != client_action.entity_id))
        break; // nothing to do (yet)

      ACE_ASSERT (client_action.window);
      RPG_Client_IWindowLevel* level_window = NULL;
      try {
        level_window =
          dynamic_cast<RPG_Client_IWindowLevel*> (client_action.window);
      } catch (...) {
        level_window = NULL;
      }
      if (!level_window)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to dynamic_cast<RPG_Client_IWindowLevel*>(%@), continuing\n"),
                    client_action.window));
        goto continue_;
      }
      try {
        level_window->setBlendRadius (client_action.radius);
        level_window->draw ();
      } catch (...) {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("caught exception in [%@]: RPG_Client_IWindowLevel::setBlendRadius(%u)/draw(), continuing\n"),
                    level_window,
                    client_action.radius));
        goto continue_;
      }

      break;
    }
    case COMMAND_TILE_HIGHLIGHT_DRAW:
    { ACE_ASSERT (client_action.window);
      SDL_Rect window_area;
      client_action.window->getArea (window_area, true);
      RPG_Client_IWindowLevel* level_window = NULL;
      try {
        level_window =
          dynamic_cast<RPG_Client_IWindowLevel*> (client_action.window);
      } catch (...) {
        level_window = NULL;
      }
      if (!level_window)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to dynamic_cast<RPG_Client_IWindowLevel*>(%@), continuing\n"),
                    client_action.window));
        goto continue_;
      }
      RPG_Graphics_Position_t view = level_window->getView ();
      RPG_Map_PositionList_t positions;
      RPG_Graphics_Offsets_t screen_positions;
      if (client_action.path.empty () &&
          client_action.positions.empty ())
      {
        positions.push_back (client_action.position);
        screen_positions.push_back (RPG_Graphics_Common_Tools::mapToScreen (client_action.position,
                                                                            std::make_pair (window_area.w,
                                                                                            window_area.h),
                                                                            view));
      } // end IF
      else if (!client_action.path.empty ())
      { ACE_ASSERT (client_action.positions.empty ());
        for (RPG_Map_PathConstIterator_t iterator2 = client_action.path.begin ();
             iterator2 != client_action.path.end ();
             iterator2++)
        {
          positions.push_back ((*iterator2).first);
          screen_positions.push_back (RPG_Graphics_Common_Tools::mapToScreen ((*iterator2).first,
                                                                              std::make_pair (window_area.w,
                                                                                              window_area.h),
                                                                              view));
        } // end FOR
      } // end ELSE
      else
      { ACE_ASSERT (!client_action.positions.empty ());
        for (RPG_Map_PositionsConstIterator_t iterator2 = client_action.positions.begin ();
             iterator2 != client_action.positions.end ();
             iterator2++)
        {
          positions.push_back (*iterator2);
          screen_positions.push_back (RPG_Graphics_Common_Tools::mapToScreen (*iterator2,
                                                                              std::make_pair (window_area.w,
                                                                                              window_area.h),
                                                                              view));
        } // end FOR
      } // end ELSE
      RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance ()->putHighlights (positions,
                                                                          screen_positions,
                                                                          view,
                                                                          dirty_region,
                                                                          true,
                                                                          debug_);

      try {
        client_action.window->invalidate (dirty_region);
      } catch (...) {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("caught exception in RPG_Graphics_IWindowBase::invalidate(), continuing\n")));
        goto continue_;
      }

      break;
    }
    case COMMAND_TILE_HIGHLIGHT_INVALIDATE_BG:
    {
      RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance ()->resetHighlightBG (client_action.position);
      break;
    }
    case COMMAND_TILE_HIGHLIGHT_RESTORE_BG:
    { ACE_ASSERT (client_action.window);
      RPG_Client_IWindowLevel* level_window = NULL;
      try {
        level_window =
          dynamic_cast<RPG_Client_IWindowLevel*> (client_action.window);
      } catch (...) {
        level_window = NULL;
      }
      if (!level_window)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to dynamic_cast<RPG_Client_IWindowLevel*>(%@), continuing\n"),
                    client_action.window));
        goto continue_;
      }
      RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance ()->restoreHighlightBG (level_window->getView (),
                                                                              dirty_region,
                                                                              NULL,
                                                                              true,
                                                                              debug_);

      try {
        client_action.window->invalidate (dirty_region);
      } catch (...) {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("caught exception in RPG_Graphics_IWindowBase::invalidate, continuing\n")));
        goto continue_;
      }

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
    { ACE_ASSERT (client_action.window);
      RPG_Client_IWindowLevel* level_window = NULL;
      try {
        level_window =
          dynamic_cast<RPG_Client_IWindowLevel*> (client_action.window);
      } catch (...) {
        level_window = NULL;
      }
      if (!level_window)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to dynamic_cast<RPG_Client_IWindowLevel*>(%@), continuing\n"),
                    client_action.window));
        goto continue_;
      }
      try {
        level_window->toggleDoor (client_action.position);
        level_window->draw ();
      } catch (...) {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("caught exception in [%@]: RPG_Client_IWindowLevel::toggleDoor/clear/draw(), continuing\n"),
                    level_window));
        goto continue_;
      }

      // --> update whole window (*NOTE*: the draw() call above invalidates the whole window)
      //try {
      //  client_action.window->invalidate (dirty_region);
      //} catch (...) {
      //  ACE_DEBUG ((LM_ERROR,
      //              ACE_TEXT ("caught exception in RPG_Graphics_IWindowBase::invalidate(), continuing\n")));
      //  continue;
      //}

      break;
    }
    case COMMAND_WINDOW_BORDER_DRAW:
    { ACE_ASSERT (client_action.window);
      RPG_Client_IWindow* client_window = NULL;
      try {
        client_window =
          dynamic_cast<RPG_Client_IWindow*> (client_action.window);
      } catch (...) {
        client_window = NULL;
      }
      if (!client_window)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to dynamic_cast<RPG_Client_IWindow*>(%@), continuing\n"),
                    client_action.window));
        goto continue_;
      }
#if defined (SDL_USE)
      SDL_Surface* surface_p = client_action.window->getScreen ();
#elif defined (SDL2_USE)
      SDL_Surface* surface_p =
        SDL_GetWindowSurface (client_action.window->getScreen ());
#endif // SDL_USE || SDL2_USE
      try {
        client_window->drawBorder (surface_p,
                                   0,
                                   0);
      } catch (...) {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("caught exception in [%@]: RPG_Client_IWindow::drawBorder(), continuing\n"),
                    client_window));
        goto continue_;
      }

      // --> update whole window (*NOTE*: the drawBorder() call above invalidates the whole window)
      //try {
      //  (*iterator).window->invalidate (dirty_region);
      //} catch (...) {
      //  ACE_DEBUG ((LM_ERROR,
      //              ACE_TEXT ("caught exception in RPG_Graphics_IWindowBase::invalidate(), continuing\n")));
      //  continue;
      //}

      break;
    }
    case COMMAND_WINDOW_DRAW:
    { ACE_ASSERT (client_action.window);
      try {
        client_action.window->draw ();
      } catch (...) {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("caught exception in [%@]: RPG_Graphics_IWindow::draw(), continuing\n"),
                    client_action.window));
        goto continue_;
      }

      break;
    }
    case COMMAND_WINDOW_HIDE:
    { ACE_ASSERT (client_action.window);
      RPG_Graphics_IWindow* window = NULL;
      try {
        window = dynamic_cast<RPG_Graphics_IWindow*> (client_action.window);
      } catch (...) {
        window = NULL;
      }
      if (!window)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to dynamic_cast<RPG_Graphics_IWindow*>(%@), continuing\n"),
                    client_action.window));
        goto continue_;
      }
      SDL_Rect dirty_region;
      try {
        window->hide (dirty_region);
      } catch (...) {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("caught exception in [%@]: RPG_Graphics_IWindow::hide(), continuing\n"),
                    window));
        goto continue_;
      }

      break;
    }
    case COMMAND_WINDOW_INIT:
    { ACE_ASSERT (client_action.window);
      RPG_Client_IWindowLevel* level_window_p = NULL;
      try {
        level_window_p =
          dynamic_cast<RPG_Client_IWindowLevel*> (client_action.window);
      } catch (...) {
        level_window_p = NULL;
      }
      if (!level_window_p)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to dynamic_cast<RPG_Client_IWindowLevel*>(%@), continuing\n"),
                    client_action.window));
        goto continue_;
      }

      // step0: reset bg caches
      RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance ()->invalidateBG ();
      RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance ()->resetHighlightBG (std::make_pair (std::numeric_limits<unsigned int>::max (),
                                                                                            std::numeric_limits<unsigned int>::max ()));

      // step1: init/(re)draw window
      engine_->lock ();
      RPG_Map_Position_t center = engine_->getSize (false);
      setStyle (RPG_Client_Common_Tools::environmentToStyle (engine_->getMetaData (false).environment));
      engine_->unlock ();
      center.first >>= 1; center.second >>= 1;
      try {
        level_window_p->initialize (state_.style);
        level_window_p->setView (center);
        level_window_p->draw ();
      } catch (...) {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("caught exception in [%@]: RPG_Client_IWindowLevel::initialize/setView/draw(), continuing\n"),
                    level_window_p));
        goto continue_;
      }

      // step2: (re)set window title caption/iconify
      std::string caption
        //= ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GRAPHICS_WINDOW_MAIN_DEF_TITLE);
        ;
      const std::string& level_name = engine_->getMetaData (true).name;
      if (!level_name.empty ())
      {
        caption = ACE_TEXT_ALWAYS_CHAR ("* ");
        caption += level_name;
        caption += ACE_TEXT_ALWAYS_CHAR (" *");
      } // end IF
      else
      {
        // no map --> iconify
#if defined (SDL_USE)
        if (SDL_WM_IconifyWindow () == 0)
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to SDL_WM_IconifyWindow(): \"%s\", continuing\n"),
                      ACE_TEXT (SDL_GetError ())));
#elif defined (SDL2_USE)
        SDL_MinimizeWindow (client_action.window->getScreen ());
#endif // SDL_USE || SDL2_USE
      } // end IF
      gchar* caption_utf8 = Common_UI_GTK_Tools::localeToUTF8 (caption);
// *TODO*: this will not return on VS2010...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
#if defined (SDL_USE)
      SDL_WM_SetCaption (caption_utf8,  // window caption
                          caption_utf8); // icon caption
#endif // SDL_USE
#endif // ACE_WIN32 || ACE_WIN64
      // clean up
      g_free (caption_utf8);

      break;
    }
    case COMMAND_WINDOW_REFRESH:
    { ACE_ASSERT (client_action.window);
      // --> update whole window
      client_action.window->getArea (dirty_region);
      try {
        client_action.window->invalidate (dirty_region);
      } catch (...) {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("caught exception in RPG_Graphics_IWindowBase::invalidate, continuing\n")));
        goto continue_;
      }

      break;
    }
    case COMMAND_WINDOW_UPDATE_MESSAGEWINDOW:
    { ACE_ASSERT (client_action.window);
      ACE_ASSERT (!client_action.message.empty ());
      RPG_Client_IWindowLevel* level_window = NULL;
      try {
        level_window =
          dynamic_cast<RPG_Client_IWindowLevel*> (client_action.window);
      } catch (...) {
        level_window = NULL;
      }
      if (!level_window)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to dynamic_cast<RPG_Client_IWindowLevel*>(%@), continuing\n"),
                    client_action.window));
        goto continue_;
      }
      try {
        level_window->updateMessageWindow (client_action.message);
      } catch (...) {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("caught exception in [%@]: RPG_Client_IWindowLevel::updateMessageWindow(\"%s\"), continuing\n"),
                    level_window,
                    ACE_TEXT (client_action.message.c_str ())));
        goto continue_;
      }

      break;
    }
    case COMMAND_WINDOW_UPDATE_MINIMAP:
    { ACE_ASSERT (client_action.window);
      RPG_Client_IWindowLevel* level_window = NULL;
      try {
        level_window =
          dynamic_cast<RPG_Client_IWindowLevel*> (client_action.window);
      } catch (...) {
        level_window = NULL;
      }
      if (!level_window)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to dynamic_cast<RPG_Client_IWindowLevel*>(%@), continuing\n"),
                    client_action.window));
        goto continue_;
      }
      try {
        level_window->updateMinimap ();
      } catch (...) {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("caught exception in [%@]: RPG_Client_IWindowLevel::updateMinimap(), continuing\n"),
                    level_window));
        goto continue_;
      }

      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid action (was: %d), continuing\n"),
                  client_action.command));
      goto continue_;
    }
  } // end SWITCH

  // update screen ?
  if (client_action.window)
    try {
      client_action.window->update ();
    } catch (...) {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("[%@] caught exception in RPG_Graphics_IWindow::update(), continuing\n"),
                  client_action.window));
    }

continue_:
  { ACE_GUARD (ACE_Thread_Mutex, aGuard, lock_);
    if (!actions_.empty ())
      goto next; // continue
  } // end lock scope
}
