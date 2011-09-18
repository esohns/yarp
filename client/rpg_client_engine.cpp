﻿/***************************************************************************
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

#include "rpg_client_engine.h"

#include "rpg_client_defines.h"
#include "rpg_client_window_main.h"
#include "rpg_client_window_level.h"

#include <rpg_engine_level.h>

#include <rpg_graphics_cursor_manager.h>
#include <rpg_graphics_surface.h>

#include <rpg_common_macros.h>

#include <ace/Log_Msg.h>

RPG_Client_Engine::RPG_Client_Engine()
//  : myQueue(RPG_ENGINE_MAX_QUEUE_SLOTS),
 : myCondition(myLock),
   myStop(false),
   myLevelState(NULL),
   myLevelWindow(NULL)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Engine::RPG_Client_Engine"));

  // use member message queue...
//   inherited::msg_queue(&myQueue);
}

RPG_Client_Engine::RPG_Client_Engine(RPG_Engine_Level* levelState_in,
                                     RPG_Graphics_IWindow* window_in)
//  : myQueue(RPG_ENGINE_MAX_QUEUE_SLOTS),
 : myCondition(myLock),
   myStop(false),
   myLevelState(levelState_in),
   myLevelWindow(window_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Engine::RPG_Client_Engine"));

  // sanity check(s)
  ACE_ASSERT(levelState_in);
  ACE_ASSERT(window_in);

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
      ACE_DEBUG((LM_CRITICAL,
                 ACE_TEXT("should never get here, returning\n")));

      ACE_ASSERT(false);

      // what else can we do ?
      break;
    }
    default:
    {
      ACE_DEBUG((LM_CRITICAL,
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
                            thread_ids);                 // thread id(s)
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

  // signal the (waiting) worker thread...
  myCondition.signal();

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

const bool
RPG_Client_Engine::isRunning()
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
RPG_Client_Engine::init()
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Engine::init"));

  // sanity check(s)
  if (myLevelWindow == NULL)
    return;

  RPG_Client_Action new_action;
  new_action.command = COMMAND_WINDOW_INIT;
  new_action.map_position = std::make_pair(0, 0);
  new_action.graphics_position = std::make_pair(0, 0);
  new_action.window = myLevelWindow;
  new_action.cursor = RPG_GRAPHICS_CURSOR_INVALID;

  action(new_action);
}

void
RPG_Client_Engine::redraw()
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Engine::redraw"));

  // sanity check(s)
  if (myLevelWindow == NULL)
    return;

  RPG_Client_Action new_action;
  new_action.command = COMMAND_WINDOW_DRAW;
  new_action.map_position = std::make_pair(0, 0);
  new_action.graphics_position = std::make_pair(0, 0);
  new_action.window = myLevelWindow;
  new_action.cursor = RPG_GRAPHICS_CURSOR_INVALID;

  action(new_action);
}

void
RPG_Client_Engine::toggleDoor(const RPG_Map_Position_t& position_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Engine::toggleDoor"));

  RPG_Client_Action new_action;
  new_action.command = COMMAND_TOGGLE_DOOR;
  new_action.map_position = position_in;
  new_action.graphics_position = std::make_pair(0, 0);
  new_action.window = myLevelWindow;
  new_action.cursor = RPG_GRAPHICS_CURSOR_INVALID;

  action(new_action);
}

void
RPG_Client_Engine::center(const RPG_Map_Position_t& position_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Engine::center"));

  // sanity check(s)
  if (myLevelWindow == NULL)
    return;

  RPG_Client_Action new_action;
  new_action.command = COMMAND_SET_VIEW;
  new_action.map_position = position_in;
  new_action.graphics_position = std::make_pair(0, 0);
  new_action.window = myLevelWindow;
  new_action.cursor = RPG_GRAPHICS_CURSOR_INVALID;

  action(new_action);
}

void
RPG_Client_Engine::init(RPG_Engine_Level* levelState_in,
                        RPG_Graphics_IWindow* window_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Engine::init"));

  // sanity check(s)
  ACE_ASSERT(levelState_in);
  ACE_ASSERT(window_in);

  myLevelState = levelState_in;
  myLevelWindow = window_in;
}

void
RPG_Client_Engine::action(const RPG_Client_Action& action_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Engine::action"));

  ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

  myActions.push_back(action_in);

  // wake up the (waiting) worker thread
  myCondition.signal();
}

void
RPG_Client_Engine::handleActions()
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Engine::handleActions"));

  bool refresh_window = false;
  SDL_Rect dirtyRegion;
  for (RPG_Client_ActionsIterator_t iterator = myActions.begin();
       iterator != myActions.end();
       iterator++)
  {
    refresh_window = false;

    switch ((*iterator).command)
    {
      case COMMAND_CURSOR_DRAW:
      {
        RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->put((*iterator).graphics_position.first,
                                                               (*iterator).graphics_position.second,
                                                               (*iterator).window->getScreen(),
                                                               dirtyRegion);
        RPG_Graphics_Surface::update(dirtyRegion,
                                     (*iterator).window->getScreen());

        break;
      }
      case COMMAND_CURSOR_SET:
      {
        RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->set((*iterator).cursor);

        break;
      }
      case COMMAND_CURSOR_INVALIDATE_BG:
      {
        RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->invalidateBG();

        break;
      }
      case COMMAND_CURSOR_RESTORE_BG:
      {
        RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->restoreBG((*iterator).window->getScreen(),
                                                                     dirtyRegion);
        RPG_Graphics_Surface::update(dirtyRegion,
                                     (*iterator).window->getScreen());

        break;
      }
      case COMMAND_SET_VIEW:
      {
        // fiddling with the view invalidates the cursor BG
        RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->invalidateBG();

        RPG_Client_WindowLevel* window = dynamic_cast<RPG_Client_WindowLevel*>((*iterator).window);
        ACE_ASSERT(window);
        try
        {
          window->setView((*iterator).map_position);
          window->draw();
        }
        catch (...)
        {
          ACE_DEBUG((LM_CRITICAL,
                     ACE_TEXT("caught exception in [%@]: RPG_Client_WindowLevel::setView([%u,%u])/draw(), aborting\n"),
                     window,
                     (*iterator).map_position.first,
                     (*iterator).map_position.second));

          return;
        }

        refresh_window = true;

        break;
      }
      case COMMAND_TILE_HIGHLIGHT_DRAW:
      {
        RPG_Client_WindowLevel* window = dynamic_cast<RPG_Client_WindowLevel*>((*iterator).window);
        ACE_ASSERT(window);
        try
        {
          window->drawHighlight((*iterator).graphics_position);
        }
        catch (...)
        {
          ACE_DEBUG((LM_CRITICAL,
                     ACE_TEXT("caught exception in [%@]: RPG_Client_WindowLevel::drawHighlight(), aborting\n"),
                     window));

          return;
        }

        break;
      }
      case COMMAND_TILE_HIGHLIGHT_RESTORE_BG:
      {
        RPG_Client_WindowLevel* window = dynamic_cast<RPG_Client_WindowLevel*>((*iterator).window);
        ACE_ASSERT(window);
        try
        {
          window->restoreHighlightBG();
        }
        catch (...)
        {
          ACE_DEBUG((LM_CRITICAL,
                     ACE_TEXT("caught exception in [%@]: RPG_Client_WindowLevel::restoreHighlightBG(), aborting\n"),
                     window));

          return;
        }

        break;
      }
      case COMMAND_TILE_HIGHLIGHT_STORE_BG:
      {
        RPG_Client_WindowLevel* window = dynamic_cast<RPG_Client_WindowLevel*>((*iterator).window);
        ACE_ASSERT(window);
        try
        {
          window->storeHighlightBG((*iterator).map_position,
                                   (*iterator).graphics_position);
        }
        catch (...)
        {
          ACE_DEBUG((LM_CRITICAL,
                     ACE_TEXT("caught exception in [%@]: RPG_Client_WindowLevel::storeHighlightBG(), aborting\n"),
                     window));

          return;
        }

        break;
      }
      case COMMAND_WINDOW_BORDER_DRAW:
      {
        RPG_Client_WindowMain* window = dynamic_cast<RPG_Client_WindowMain*>((*iterator).window);
        ACE_ASSERT(window);
        try
        {
          window->drawBorder((*iterator).window->getScreen(),
                             0,
                             0);
        }
        catch (...)
        {
          ACE_DEBUG((LM_CRITICAL,
                     ACE_TEXT("caught exception in [%@]: RPG_Client_WindowMain::drawBorder(), aborting\n"),
                     window));

          return;
        }

        refresh_window = true;

        break;
      }
      case COMMAND_WINDOW_DRAW:
      {
        try
        {
          (*iterator).window->draw();
        }
        catch (...)
        {
          ACE_DEBUG((LM_CRITICAL,
                     ACE_TEXT("caught exception in [%@]: RPG_Graphics_IWindow::draw(), aborting\n"),
                     (*iterator).window));

          return;
        }

        refresh_window = true;

        break;
      }
      case COMMAND_WINDOW_INIT:
      {
        // fiddling with the view invalidates the cursor BG
        RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->invalidateBG();

        // step1: init/redraw level map
        RPG_Client_WindowLevel* window = dynamic_cast<RPG_Client_WindowLevel*>((*iterator).window);
        ACE_ASSERT(window);
        try
        {
          window->init();
          window->draw();
        }
        catch (...)
        {
          ACE_DEBUG((LM_CRITICAL,
                     ACE_TEXT("caught exception in [%@]: RPG_Client_WindowMain::init/draw(), aborting\n"),
                     window));

          return;
        }

        // step2: (re)set level window title caption/iconify
        std::string caption = RPG_CLIENT_DEF_GRAPHICS_MAINWINDOW_TITLE;
        if (!myLevelState->getName().empty())
        {
          caption = ACE_TEXT_ALWAYS_CHAR("* ");
          caption += myLevelState->getName();
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
        SDL_WM_SetCaption(caption.c_str(),  // window caption
                          caption.c_str()); // icon caption

        refresh_window = true;

        break;
      }
      case COMMAND_WINDOW_REFRESH:
      {
        try
        {
          (*iterator).window->refresh();
        }
        catch (...)
        {
          ACE_DEBUG((LM_CRITICAL,
                     ACE_TEXT("caught exception in [%@]: RPG_Graphics_IWindow::refresh(), aborting\n"),
                     (*iterator).window));

          return;
        }

        break;
      }
      case COMMAND_TOGGLE_DOOR:
      {
        RPG_Client_WindowLevel* window = dynamic_cast<RPG_Client_WindowLevel*>((*iterator).window);
        ACE_ASSERT(window);
        try
        {
          window->toggleDoor((*iterator).map_position);
          window->draw();
        }
        catch (...)
        {
          ACE_DEBUG((LM_CRITICAL,
                     ACE_TEXT("caught exception in [%@]: RPG_Client_WindowLevel::toggleDoor/draw(), aborting\n"),
                     window));

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