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
#include "rpg_client_window_main.h"
#include "rpg_client_window_level.h"
#include "rpg_client_common_tools.h"
#include "rpg_client_ui_tools.h"
#include "rpg_client_entity_manager.h"

#include <rpg_engine_level.h>

#include <rpg_graphics_surface.h>
#include <rpg_graphics_cursor_manager.h>
#include <rpg_graphics_common_tools.h>

#include <rpg_common_macros.h>

#include <ace/Log_Msg.h>

RPG_Client_Engine::RPG_Client_Engine()
//  : myQueue(RPG_ENGINE_MAX_QUEUE_SLOTS),
 : myCondition(myLock),
   myStop(false),
   myEngine(NULL),
   myWindow(NULL),
   myWidgets(NULL),
//   myActions(),
   mySelectionMode(SELECTIONMODE_NORMAL),
   myCenterOnActivePlayer(RPG_CLIENT_DEF_CENTER_ON_ACTIVE_PLAYER)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Engine::RPG_Client_Engine"));

  // use member message queue...
//   inherited::msg_queue(&myQueue);
}

//RPG_Client_Engine::RPG_Client_Engine(RPG_Engine_Level* levelState_in,
//                                     RPG_Graphics_IWindow* window_in)
////  : myQueue(RPG_ENGINE_MAX_QUEUE_SLOTS),
// : myCondition(myLock),
//   myStop(false),
//   myEngine(levelState_in),
//   myWindow(window_in),
//   myWidgets(NULL),
////   myActions(),
//   mySelectionMode(SELECTIONMODE_NORMAL),
//   myCenterOnActivePlayer(RPG_CLIENT_DEF_CENTER_ON_ACTIVE_PLAYER)
//{
//  RPG_TRACE(ACE_TEXT("RPG_Client_Engine::RPG_Client_Engine"));
//
//  // sanity check(s)
//  ACE_ASSERT(levelState_in);
//  ACE_ASSERT(window_in);
//
//  // use member message queue...
////   inherited::msg_queue(&myQueue);
//}

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
  new_action.position = std::make_pair(0, 0);
  new_action.window = myWindow;
  new_action.cursor = RPG_GRAPHICS_CURSOR_INVALID;
  new_action.entity_id = 0;

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
  new_action.position = std::make_pair(0, 0);
  new_action.window = myWindow;
  new_action.cursor = RPG_GRAPHICS_CURSOR_INVALID;
  new_action.entity_id = 0;
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
                          const RPG_Engine_ClientParameters_t& parameters_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Engine::notify"));

  ACE_ASSERT(myEngine);

  RPG_Client_Action client_action;
  client_action.command = RPG_CLIENT_COMMAND_INVALID;
  client_action.position = std::make_pair(std::numeric_limits<unsigned int>::max(),
                                          std::numeric_limits<unsigned int>::max());
  client_action.window = NULL;
  client_action.cursor = RPG_GRAPHICS_CURSOR_INVALID;
  client_action.entity_id = 0;
  switch (command_in)
  {
    case COMMAND_ATTACK:
      return;
    case COMMAND_DOOR_CLOSE:
    case COMMAND_DOOR_OPEN:
    {
      ACE_ASSERT(parameters_in.size() == 1);
      client_action.command = COMMAND_TOGGLE_DOOR;
      client_action.position = *static_cast<RPG_Map_Position_t*>(parameters_in.front());
      client_action.window = myWindow;

      break;
    }
    case COMMAND_SEARCH:
    case COMMAND_STOP:
    case COMMAND_TRAVEL:
      return;
    case COMMAND_E2C_ENTITY_ADD:
    {
      ACE_ASSERT(parameters_in.size() == 2);
      RPG_Engine_EntityID_t entity_id = *static_cast<RPG_Engine_EntityID_t*>(parameters_in.front());
      RPG_CLIENT_ENTITY_MANAGER_SINGLETON::instance()->add(entity_id,
                                                           static_cast<SDL_Surface*>(parameters_in.back()),
                                                           myEngine->isMonster(entity_id));

      return;
    }
    case COMMAND_E2C_ENTITY_REMOVE:
    {
      ACE_ASSERT(parameters_in.size() == 1);
      RPG_CLIENT_ENTITY_MANAGER_SINGLETON::instance()->remove(*static_cast<RPG_Engine_EntityID_t*>(parameters_in.front()));

      return;
    }
    case COMMAND_E2C_ENTITY_UPDATE:
    {
      ACE_ASSERT(parameters_in.size() == 2);
      client_action.command = COMMAND_ENTITY_DRAW;
      client_action.position = *static_cast<RPG_Map_Position_t*>(parameters_in.back());
      client_action.window = myWindow;
      client_action.entity_id = *static_cast<RPG_Engine_EntityID_t*>(parameters_in.front());

      // adjust view ?
      if (getCenterOnActive() &&
          (client_action.entity_id == myEngine->getActive()))
      {
        action(client_action);

        client_action.command = COMMAND_SET_VIEW;
      } // end IF

      break;
    }
    case COMMAND_E2C_QUIT:
    {
      // sanity check
      ACE_ASSERT(parameters_in.empty());
      ACE_ASSERT(myWidgets);

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

      return;
    }
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid command (was: \"%s\", aborting\n"),
                 RPG_Engine_CommandHelper::RPG_Engine_CommandToString(command_in).c_str()));

      return;
    }
  } // end SWITCH

  action(client_action);
}

void
RPG_Client_Engine::init(RPG_Engine_Level* levelState_in,
                        RPG_Graphics_IWindow* window_in,
                        GladeXML* widgets_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Engine::init"));

  // sanity check(s)
  ACE_ASSERT(levelState_in);
  ACE_ASSERT(window_in);
  ACE_ASSERT(widgets_in);

  myEngine = levelState_in;
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

bool
RPG_Client_Engine::hasMode(const RPG_Client_SelectionMode& mode_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Engine::hasMode"));

  return (mySelectionMode == mode_in);
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
        ACE_ASSERT((*iterator).window);

        RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->put((*iterator).position.first,
                                                               (*iterator).position.second,
                                                               (*iterator).window->getScreen(),
                                                               dirtyRegion);
        RPG_Graphics_Surface::update(dirtyRegion,
                                     (*iterator).window->getScreen());

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
        ACE_ASSERT((*iterator).window);

        RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->restoreBG((*iterator).window->getScreen(),
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
      case COMMAND_ENTITY_DRAW:
      {
        // sanity check
        ACE_ASSERT((*iterator).window);

        RPG_CLIENT_ENTITY_MANAGER_SINGLETON::instance()->put((*iterator).entity_id,
                                                             RPG_Graphics_Common_Tools::map2Screen((*iterator).position,
                                                                                                   (*iterator).window->getSize(false),
                                                                                                   (*iterator).window->getView()),
                                                             (*iterator).window->getScreen(),
                                                             dirtyRegion);
        RPG_Graphics_Surface::update(dirtyRegion,
                                     (*iterator).window->getScreen());

        break;
      }
      case COMMAND_SET_VIEW:
      {
        // sanity check
        ACE_ASSERT((*iterator).window);

        RPG_Client_WindowLevel* window = dynamic_cast<RPG_Client_WindowLevel*>((*iterator).window);
        ACE_ASSERT(window);
        try
        {
          window->setView((*iterator).position);
          window->draw();
        }
        catch (...)
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("caught exception in [%@]: RPG_Client_WindowLevel::setView([%u,%u])/draw(), aborting\n"),
                     window,
                     (*iterator).position.first,
                     (*iterator).position.second));

          return;
        }

        // fiddling with the view invalidates the tile highlight BG
        RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->resetHighlightBG((*iterator).position);
        // --> store/draw the new tile highlight (BG)
        RPG_Map_Position_t cursor_position = RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->position();
        RPG_Map_Position_t map_position = RPG_Graphics_Common_Tools::screen2Map(cursor_position,
                                                                                myEngine->getSize(),
                                                                                (*iterator).window->getSize(),
                                                                                (*iterator).window->getView());
        RPG_Map_Position_t screen_position = RPG_Graphics_Common_Tools::map2Screen(map_position,
                                                                                   (*iterator).window->getSize(false),
                                                                                   (*iterator).window->getView());
        RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->storeHighlightBG(map_position,
                                                                            screen_position,
                                                                            (*iterator).window->getScreen());
        RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->drawHighlight(screen_position,
                                                                         (*iterator).window->getScreen());

        // fiddling with the view (probably) invalidates (part of) the cursor BG
        RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->updateBG((*iterator).window->getScreen());
        SDL_Rect dirty_region = {0, 0, 0, 0};
        RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->put(cursor_position.first,
                                                               cursor_position.second,
                                                               (*iterator).window->getScreen(),
                                                               dirty_region);
        //RPG_Graphics_Surface::update(dirtyRegion,
        //                             (*iterator).window->getScreen());

        refresh_window = true;

        break;
      }
      case COMMAND_TILE_HIGHLIGHT_DRAW:
      {
        // sanity check
        ACE_ASSERT((*iterator).window);

        RPG_Graphics_Size_t size = (*iterator).window->getSize(false);
        RPG_Graphics_Position_t view = (*iterator).window->getView();
        if ((*iterator).path.empty())
          RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->drawHighlight(RPG_Graphics_Common_Tools::map2Screen((*iterator).position,
                                                                                                                 size,
                                                                                                                 view),
                                                                           (*iterator).window->getScreen());
        else
        {
          RPG_Graphics_Positions_t graphics_positions;
          for (RPG_Map_PathConstIterator_t iterator2 = (*iterator).path.begin();
               iterator2 != (*iterator).path.end();
               iterator2++)
            graphics_positions.push_back(RPG_Graphics_Common_Tools::map2Screen((*iterator2).first,
                                                                               size,
                                                                               view));
          RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->drawHighlight(graphics_positions,
                                                                           (*iterator).window->getScreen());
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
        ACE_ASSERT((*iterator).window);

        RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->restoreHighlightBG((*iterator).window->getScreen());

        break;
      }
      case COMMAND_TILE_HIGHLIGHT_STORE_BG:
      {
        // sanity check
        ACE_ASSERT((*iterator).window);

        RPG_Graphics_Size_t size = (*iterator).window->getSize(false);
        RPG_Graphics_Position_t view = (*iterator).window->getView();
        if ((*iterator).path.empty())
          RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->storeHighlightBG((*iterator).position,
                                                                              RPG_Graphics_Common_Tools::map2Screen((*iterator).position,
                                                                                                                    size,
                                                                                                                    view),
                                                                              (*iterator).window->getScreen());
        else
        {
          RPG_Graphics_Positions_t map_positions, graphics_positions;
          for (RPG_Map_PathConstIterator_t iterator2 = (*iterator).path.begin();
               iterator2 != (*iterator).path.end();
               iterator2++)
          {
            map_positions.push_back((*iterator2).first);
            graphics_positions.push_back(RPG_Graphics_Common_Tools::map2Screen((*iterator2).first,
                                                                               size,
                                                                               view));
          }
          RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->storeHighlightBG(map_positions,
                                                                              graphics_positions,
                                                                              (*iterator).window->getScreen());
        } // end ELSE

        break;
      }
      case COMMAND_WINDOW_BORDER_DRAW:
      {
        // sanity check
        ACE_ASSERT((*iterator).window);

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
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("caught exception in [%@]: RPG_Client_WindowMain::drawBorder(), aborting\n"),
                     window));

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
        // sanity check
        ACE_ASSERT((*iterator).window);

        // fiddling with the view invalidates the cursor/tile highlight BG
        RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->invalidateBG();
        RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->resetHighlightBG((*iterator).position);

        RPG_Map_Position_t center = myEngine->getSize();
        center.first >>= 1;
        center.second >>= 1;

        // step1: init/redraw level map
        RPG_Client_WindowLevel* window = dynamic_cast<RPG_Client_WindowLevel*>((*iterator).window);
        ACE_ASSERT(window);
        try
        {
          window->init();
          window->setView(center);
          window->draw();
        }
        catch (...)
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("caught exception in [%@]: RPG_Client_WindowMain::init/setView/draw(), aborting\n"),
                     window));

          return;
        }

        // step2: (re)set level window title caption/iconify
        std::string caption = ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_DEF_GRAPHICS_MAINWINDOW_TITLE);
        if (!myEngine->getName().empty())
        {
          caption = ACE_TEXT_ALWAYS_CHAR("* ");
          caption += myEngine->getName();
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
        // sanity check
        ACE_ASSERT((*iterator).window);

        try
        {
          (*iterator).window->refresh();
        }
        catch (...)
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("caught exception in [%@]: RPG_Graphics_IWindow::refresh(), aborting\n"),
                     (*iterator).window));

          return;
        }

        break;
      }
      case COMMAND_TOGGLE_DOOR:
      {
        // sanity check
        ACE_ASSERT((*iterator).window);

        RPG_Client_WindowLevel* window = dynamic_cast<RPG_Client_WindowLevel*>((*iterator).window);
        ACE_ASSERT(window);
        try
        {
          window->toggleDoor((*iterator).position);
          window->draw();
        }
        catch (...)
        {
          ACE_DEBUG((LM_ERROR,
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
