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

#include "rpg_client_window_level_stub.h"

#include <sstream>

#include "ace/Log_Msg.h"

#include "common_file_tools.h"

#include "rpg_common_macros.h"

#include "rpg_map_common_tools.h"
#include "rpg_map_pathfinding_tools.h"

#include "rpg_graphics_defines.h"
#include "rpg_graphics_surface.h"
#include "rpg_graphics_cursor_manager.h"
#include "rpg_graphics_common_tools.h"
#include "rpg_graphics_SDL_tools.h"

#include "rpg_engine.h"
#include "rpg_engine_command.h"
#include "rpg_engine_common_tools.h"

#include "rpg_client_defines.h"
#include "rpg_client_engine.h"
#include "rpg_client_common_tools.h"
#include "rpg_client_entity_manager.h"
#include "rpg_client_window_minimap.h"
#include "rpg_client_window_message.h"

RPG_Client_Window_Level_Stub::RPG_Client_Window_Level_Stub (const RPG_Graphics_SDLWindowBase& parent_in)
 : inherited (WINDOW_MAP,                 // type
              parent_in,                  // parent
              std::make_pair (0, 0),      // offset
              ACE_TEXT_ALWAYS_CHAR ("")), // title
   myEngine (NULL),
   myClient (NULL),
   myClientAction (),
   myDrawMinimap (RPG_CLIENT_MINIMAP_DEF_ISON),
#if defined (_DEBUG)
   myShowCoordinates (false),
#endif // _DEBUG
   myShowMessages (RPG_CLIENT_MESSAGE_DEF_ISON),
   myLock (NULL, NULL),
   myView (std::make_pair (std::numeric_limits<unsigned int>::max (),
                           std::numeric_limits<unsigned int>::max ()))
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Window_Level_Stub::RPG_Client_Window_Level_Stub"));

}

RPG_Client_Window_Level_Stub::~RPG_Client_Window_Level_Stub ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Window_Level_Stub::~RPG_Client_Window_Level_Stub"));

}

void
RPG_Client_Window_Level_Stub::setView (const RPG_Map_Position_t& view_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Window_Level_Stub::setView"));

  ACE_GUARD (ACE_Thread_Mutex, aGuard, myLock);

  myView = view_in;
}

void
RPG_Client_Window_Level_Stub::setView (int offsetX_in,
                                       int offsetY_in,
                                       bool lockedAccess_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Window_Level_Stub::setView"));

  RPG_Map_Size_t size = myEngine->getSize (lockedAccess_in);

  ACE_GUARD (ACE_Thread_Mutex, aGuard, myLock);

  // handle over-/underruns
  if ((offsetX_in < 0) &&
      (static_cast<unsigned int> (-offsetX_in) > myView.first))
    myView.first = 0;
  else
    myView.first += offsetX_in;

  if ((offsetY_in < 0) &&
      (static_cast<unsigned int> (-offsetY_in) > myView.second))
    myView.second = 0;
  else
    myView.second += offsetY_in;

  if (myView.first >= size.first)
    myView.first = (size.first - 1);
  if (myView.second >= size.second)
    myView.second = (size.second - 1);
}

RPG_Graphics_Position_t
RPG_Client_Window_Level_Stub::getView () const
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Window_Level_Stub::getView"));

  RPG_Graphics_Position_t result =
    std::make_pair (std::numeric_limits<unsigned int>::max (),
                    std::numeric_limits<unsigned int>::max ());

  { ACE_GUARD_RETURN (ACE_Thread_Mutex, aGuard, myLock, result);
    result = myView;
  } // end lock scope

  return result;
}

void
RPG_Client_Window_Level_Stub::toggleMiniMap ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Window_Level_Stub::toggleMiniMap"));

  ACE_GUARD (ACE_Thread_Mutex, aGuard, myLock);

  myDrawMinimap = !myDrawMinimap;

  // retrieve map window handle
  RPG_Graphics_WindowsConstIterator_t iterator = children_.begin ();
  for (;
       iterator != children_.end ();
       iterator++)
  {
    if ((*iterator)->getType () == WINDOW_MINIMAP)
      break;
  } // end FOR
  ACE_ASSERT ((*iterator)->getType () == WINDOW_MINIMAP);

  // hide/draw
  myClientAction.command = (myDrawMinimap ? COMMAND_WINDOW_DRAW
                                          : COMMAND_WINDOW_HIDE);
  myClientAction.window = *iterator;
  myClient->action (myClientAction);

  // reset window
  myClientAction.window = this;
}

bool
RPG_Client_Window_Level_Stub::showMiniMap () const
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Window_Level_Stub::showMiniMap"));

  bool result = false;

  { ACE_GUARD_RETURN (ACE_Thread_Mutex, aGuard, myLock, result); // *TODO*: avoid false negative
    result = myDrawMinimap;
  } // end lock scope

  return result;
}

void
RPG_Client_Window_Level_Stub::toggleMessages ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Window_Level_Stub::toggleMessages"));

  ACE_GUARD (ACE_Thread_Mutex, aGuard, myLock);

  myShowMessages = !myShowMessages;
}

bool
RPG_Client_Window_Level_Stub::showMessages () const
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Window_Level_Stub::showMessages"));

  bool result = false;

  { ACE_GUARD_RETURN (ACE_Thread_Mutex, aGuard, myLock, result); // *TODO*: avoid false negative
    result = myShowMessages;
  } // end lock scope

  return result;
}

void
RPG_Client_Window_Level_Stub::toggleDoor (const RPG_Map_Position_t& position_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Window_Level_Stub::toggleDoor"));

}

bool
RPG_Client_Window_Level_Stub::initialize (RPG_Client_Engine* clientEngine_in,
                                          RPG_Engine* engine_in,
                                          bool debug_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Window_Level_Stub::initialize"));

  // sanity checks
  ACE_ASSERT (clientEngine_in);
  ACE_ASSERT (engine_in);

#if defined (_DEBUG)
  myDebug = debug_in;
#endif // _DEBUG
  myClient = clientEngine_in;
  myEngine = engine_in;

  // initialize edge, wall, door tiles
  initialize (myClient->getStyle ());

  // initialize minimap/message windows
  if (!initMiniMap (debug_in) ||
      !initMessageWindow ())
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to initialize sub-windows, aborting\n")));
    return false;
  } // end IF

  return true;
}

void
RPG_Client_Window_Level_Stub::drawChild (enum RPG_Graphics_WindowType child_in,
                                         SDL_Surface* targetSurface_in,
                                         unsigned int offsetX_in,
                                         unsigned int offsetY_in,
                                         bool refresh_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Window_Level_Stub::drawChild"));

  // sanity check(s)
  ACE_ASSERT (inherited::screen_);
#if defined (SDL_USE)
  SDL_Surface* surface_p = inherited::screen_;
#elif defined (SDL2_USE) || defined (SDL3_USE)
  SDL_Surface* surface_p = SDL_GetWindowSurface (inherited::screen_);
#endif // SDL_USE || SDL2_USE || SDL3_USE
  ACE_ASSERT (surface_p);
  // sanity check(s)
  SDL_Surface* target_surface =
    (targetSurface_in ? targetSurface_in : surface_p);
  ACE_ASSERT (target_surface);
  ACE_ASSERT (child_in != RPG_GRAPHICS_WINDOWTYPE_INVALID);

  ACE_GUARD (ACE_Thread_Mutex, aGuard, myLock);

  // draw any child(ren) of a specific type
  for (RPG_Graphics_WindowsIterator_t iterator = inherited::children_.begin ();
       iterator != inherited::children_.end ();
       iterator++)
  {
    if (((*iterator)->getType () != child_in)             ||
        ((child_in == WINDOW_MINIMAP) && !myDrawMinimap)  || // minimap switched off...
        ((child_in == WINDOW_MESSAGE) && !myShowMessages))   // message window switched off...
      continue;

    myClientAction.command = COMMAND_WINDOW_DRAW;
    myClientAction.window = *iterator;
    myClient->action (myClientAction);

    if (refresh_in)
    {
      myClientAction.command = COMMAND_WINDOW_REFRESH;
      myClient->action (myClientAction);
    } // end IF
  } // end FOR

  // reset window
  myClientAction.window = this;
}

void
RPG_Client_Window_Level_Stub::initialize (const RPG_Graphics_Style& style_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Window_Level_Stub::initialize"));

  ACE_GUARD (ACE_Thread_Mutex, aGuard, myLock);

  // initialize style
  struct RPG_Graphics_StyleUnion style;
  style.discriminator = RPG_Graphics_StyleUnion::FLOORSTYLE;
  style.floorstyle = style_in.floor;
  if (!setStyle (style))
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Client_Window_Level_Stub::setStyle(FLOORSTYLE), continuing\n")));
  style.discriminator = RPG_Graphics_StyleUnion::EDGESTYLE;
  style.edgestyle = style_in.edge;
  if (!setStyle (style))
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Client_Window_Level_Stub::setStyle(EDGESTYLE), continuing\n")));
  style.discriminator = RPG_Graphics_StyleUnion::WALLSTYLE;
  style.wallstyle = style_in.wall;
  if (!setStyle (style))
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Client_Window_Level_Stub::setStyle(WALLSTYLE), continuing\n")));
  style.discriminator = RPG_Graphics_StyleUnion::DOORSTYLE;
  style.doorstyle = style_in.door;
  if (!setStyle (style))
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Client_Window_Level_Stub::setStyle(DOORSTYLE), continuing\n")));

  // initialize tiles / position
  // RPG_Client_Common_Tools::initFloorEdges (*myEngine,
  //                                          myCurrentFloorEdgeSet,
  //                                          myFloorEdgeTiles);
  // RPG_Client_Common_Tools::initWalls (*myEngine,
  //                                     myCurrentWallSet,
  //                                     myWallTiles);
  // RPG_Client_Common_Tools::initDoors (*myEngine,
  //                                     myCurrentDoorSet,
  //                                     myDoorTiles);
}

void
RPG_Client_Window_Level_Stub::setBlendRadius (ACE_UINT8 radius_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Window_Level_Stub::setBlendRadius"));

}

void
RPG_Client_Window_Level_Stub::updateMinimap ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Window_Level_Stub::updateMinimap"));

  // sanity check
  if (!showMiniMap ())
    return;

  drawChild (WINDOW_MINIMAP,
             NULL,
             0, 0,
             true); // refresh ?
}

void
RPG_Client_Window_Level_Stub::updateMessageWindow (const std::string& message_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Window_Level_Stub::updateMessageWindow"));

  RPG_Graphics_IWindowBase* child = inherited::child (WINDOW_MESSAGE);
  if (!child)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to RPG_Graphics_SDLWindowBase::child(WINDOW_MESSAGE), returning\n")));
    return;
  } // end IF
  RPG_Client_Window_Message* message_window =
    dynamic_cast<RPG_Client_Window_Message*> (child);
  if (!message_window)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to dynamic_cast<RPG_Client_Window_Message*>(%@), returning\n"),
                child));
    return;
  } // end IF
  message_window->push (message_in);

  // sanity check
  if (!showMessages ())
    return;

  drawChild (WINDOW_MESSAGE,
             NULL,
             0, 0,
             true); // refresh ?
}

void
RPG_Client_Window_Level_Stub::draw (SDL_Surface* targetSurface_in,
                                    unsigned int offsetX_in,
                                    unsigned int offsetY_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Window_Level_Stub::draw"));

  // sanity check(s)
  ACE_ASSERT (inherited::screen_);
#if defined (SDL_USE)
  SDL_Surface* surface_p = inherited::screen_;
#elif defined (SDL2_USE) || defined (SDL3_USE)
  SDL_Surface* surface_p = SDL_GetWindowSurface (inherited::screen_);
#endif // SDL_USE || SDL2_USE || SDL3_USE
  ACE_ASSERT (surface_p);
  SDL_Surface* target_surface =
    (targetSurface_in ? targetSurface_in : surface_p);
  ACE_ASSERT (target_surface);
  ACE_ASSERT (myEngine);
  ACE_ASSERT (static_cast<int> (offsetX_in) <= target_surface->w);
  ACE_ASSERT (static_cast<int> (offsetY_in) <= target_surface->h);

  // realize any children
  for (RPG_Graphics_WindowsIterator_t iterator = children_.begin ();
       iterator != children_.end ();
       iterator++)
  {
    if ((((*iterator)->getType () == WINDOW_MINIMAP) && !myDrawMinimap) ||
        (((*iterator)->getType () == WINDOW_MESSAGE) && !myShowMessages))
      continue;

    myClientAction.command = COMMAND_WINDOW_DRAW;
    myClientAction.window = *iterator;
    myClient->action (myClientAction);

    // reset window
    myClientAction.window = this;
  } // end FOR

  // whole viewport needs a refresh...
  struct SDL_Rect dirty_region;
#if defined (SDL_USE) || defined (SDL2_USE)
  SDL_GetClipRect (target_surface, &dirty_region);
#elif defined (SDL3_USE)
  SDL_GetSurfaceClipRect (target_surface, &dirty_region);
#endif // SDL_USE || SDL2_USE || SDL3_USE
  inherited::invalidate (dirty_region);

  // reset clipping area
  inherited::unclip (target_surface);

  // remember position of last realization
  lastAbsolutePosition_ = std::make_pair (inherited::clipRectangle_.x,
                                          inherited::clipRectangle_.y);
}

void
RPG_Client_Window_Level_Stub::handleEvent (const union SDL_Event& event_in,
                                           RPG_Graphics_IWindowBase* window_in,
                                           struct SDL_Rect& dirtyRegion_out)
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Window_Level_Stub::handleEvent"));

  // initialize return value(s)
  ACE_OS::memset (&dirtyRegion_out, 0, sizeof (struct SDL_Rect));

  // sanity check(s)
  ACE_ASSERT (myClient);
  ACE_ASSERT (myEngine);

  bool delegate_to_parent = false;
  switch (event_in.type)
  {
    // *** keyboard ***
#if defined (SDL_USE) || defined (SDL2_USE)
    case SDL_KEYDOWN:
#elif defined (SDL3_USE)
    case SDL_EVENT_KEY_DOWN:
#endif // SDL_USE || SDL2_USE || SDL3_USE
      {
//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("%s key\n%s\n"),
//                  ((event_in.type == SDL_KEYDOWN) ? ACE_TEXT("pressed") : ACE_TEXT("released")),
//                  ACE_TEXT(RPG_Graphics_SDL_Tools::keyToString(event_in.key.keysym).c_str())));

      switch (event_in.key.keysym.sym)
      {
        case SDLK_a:
        {
          myClient->centerOnActive (!myClient->getCenterOnActive ());

          // adjust view ?
          myEngine->lock ();
          RPG_Engine_EntityID_t entity_id = myEngine->getActive (false); // locked access ?
          if (entity_id &&
              myClient->getCenterOnActive ())
          {
            myClientAction.position = myEngine->getPosition (entity_id,
                                                             false); // locked access ?
            myClient->setView (myClientAction.position,
                               true); // refresh ?
          } // end IF
          myEngine->unlock ();

          break;
        }
        // implement keypad navigation
        case SDLK_c:
        {
          myEngine->lock ();
          myClientAction.entity_id = myEngine->getActive (false); // locked access ?
          if ((myClientAction.entity_id == 0)        ||
#if defined (SDL_USE) || defined (SDL2_USE)
              (event_in.key.keysym.mod & KMOD_SHIFT))
#elif defined (SDL3_USE)
              (event_in.key.keysym.mod & SDL_KMOD_SHIFT))
#endif // SDL_USE || SDL2_USE || SDL3_USE
          {
            // center view
            myClientAction.position = myEngine->getSize (false); // locked access ?
            myClientAction.position.first  >>= 1;
            myClientAction.position.second >>= 1;
          } // end IF
          else
            myClientAction.position =
                myEngine->getPosition (myClientAction.entity_id,
                                       false); // locked access ?
          myEngine->unlock ();
          myClient->setView (myClientAction.position,
                             true); // refresh ?

          break;
        }
        case SDLK_m:
        {
          toggleMiniMap ();

          break;
        }
        case SDLK_r:
        {
#if defined (SDL_USE) || defined (SDL2_USE)
          if (event_in.key.keysym.mod & KMOD_SHIFT)
#elif defined (SDL3_USE)
          if (event_in.key.keysym.mod & SDL_KMOD_SHIFT)
#endif // SDL_USE || SDL2_USE || SDL3_USE
          {
            // (manual) refresh
            myClientAction.command = COMMAND_WINDOW_DRAW;
            myClientAction.window = this;
            myClient->action (myClientAction);
          } // end IF
          else
          {
            myEngine->lock ();
            RPG_Engine_EntityID_t entity_id = myEngine->getActive (false); // locked access ?
            if (entity_id)
            {
              struct RPG_Engine_Action player_action;
              player_action.command = COMMAND_RUN;
              player_action.position =
                  std::make_pair (std::numeric_limits<unsigned int>::max (),
                                  std::numeric_limits<unsigned int>::max ());
              player_action.target = 0;

              myEngine->action (entity_id,
                                player_action,
                                false); // locked access ?
            } // end IF
            myEngine->unlock ();
          } // end ELSE

          break;
        }
        case SDLK_s:
        {
#if defined (SDL_USE) || defined (SDL2_USE)
          if (event_in.key.keysym.mod & KMOD_SHIFT)
#elif defined (SDL3_USE)
          if (event_in.key.keysym.mod & SDL_KMOD_SHIFT)
#endif // SDL_USE || SDL2_USE || SDL3_USE
          {
#if defined (_DEBUG)
            toggleShowCoordinates ();

            // --> need a redraw
            myClient->redraw (true); // refresh ?
#endif // _DEBUG
          } // end IF
          else
          {
            myEngine->lock ();
            RPG_Engine_EntityID_t entity_id = myEngine->getActive (false); // locked access ?
            if (entity_id)
            {
              struct RPG_Engine_Action player_action;
              player_action.command = COMMAND_SEARCH;
              player_action.position =
                  std::make_pair (std::numeric_limits<unsigned int>::max (),
                                  std::numeric_limits<unsigned int>::max ());

              myEngine->action (entity_id,
                                player_action,
                                false); // locked access ?
            } // end IF
            myEngine->unlock ();
          } // end ELSE

          break;
        }
        case SDLK_t:
        case SDLK_x:
        {
          // find pointed-to map square
          RPG_Graphics_Position_t cursor_position =
              RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance ()->position (false); // highlight- ?
          struct SDL_Rect window_area;
          getArea (window_area,
                   true); // toplevel- ?
          myEngine->lock ();
          myClientAction.position =
            RPG_Graphics_Common_Tools::screenToMap (cursor_position,
                                                    myEngine->getSize (false), // locked access ?
                                                    std::make_pair (window_area.w,
                                                                    window_area.h),
                                                    myView);
          bool push_mousemove_event = myEngine->isValid (myClientAction.position,
                                                         false); // locked access ?

          myClientAction.entity_id = myEngine->getActive (false); // locked access ?
          // toggle path selection mode
          switch (myClient->mode ())
          {
            case SELECTIONMODE_AIM_CIRCLE:
            case SELECTIONMODE_AIM_SQUARE:
            {
              // --> switch off aim selection

              myClientAction.command = COMMAND_TILE_HIGHLIGHT_RESTORE_BG;
              myClient->action (myClientAction);
              myClientAction.command = COMMAND_CURSOR_RESTORE_BG;
              myClient->action (myClientAction);

              // clear cached positions
              myClientAction.source =
                  std::make_pair (std::numeric_limits<unsigned int>::max (),
                                  std::numeric_limits<unsigned int>::max ());
              myClientAction.positions.clear ();

              myClient->mode (SELECTIONMODE_NORMAL);

              if (event_in.key.keysym.sym != SDLK_t)
                break;

              // *WARNING*: falls through !
            }
            case SELECTIONMODE_PATH:
            {
              myClientAction.command = COMMAND_TILE_HIGHLIGHT_RESTORE_BG;
              myClient->action (myClientAction);
              myClientAction.command = COMMAND_CURSOR_RESTORE_BG;
              myClient->action (myClientAction);

              // clear any cached positions
              myClientAction.path.clear ();

              myClient->mode (SELECTIONMODE_NORMAL);

              break;
            }
            case SELECTIONMODE_NORMAL:
            {
              if (myClientAction.entity_id == 0)
                push_mousemove_event = false;

              if (event_in.key.keysym.sym == SDLK_t)
                myClient->mode (SELECTIONMODE_PATH);	// --> switch on path selection
              else
              {
                // --> switch on aim selection

                // retain source position
                myClientAction.source = myClientAction.position;

                // initial radius == 0
                myClientAction.positions.insert (myClientAction.position);

#if defined (SDL_USE) || defined (SDL2_USE)
                myClient->mode (((event_in.key.keysym.mod & KMOD_SHIFT) ? SELECTIONMODE_AIM_SQUARE
                                                                        : SELECTIONMODE_AIM_CIRCLE));
#elif defined (SDL3_USE)
                myClient->mode (((event_in.key.keysym.mod & SDL_KMOD_SHIFT) ? SELECTIONMODE_AIM_SQUARE
                                                                            : SELECTIONMODE_AIM_CIRCLE));
#endif // SDL_USE || SDL2_USE || SDL3_USE
              } // end ELSE

              break;
            }
            default:
            {
              ACE_DEBUG ((LM_ERROR,
                          ACE_TEXT ("invalid selection mode (was: %d), continuing\n"),
                          myClient->mode ()));
              break;
            }
          } // end SWITCH
          myEngine->unlock ();

          // on the map ?
          if (push_mousemove_event)
          {
            // redraw highlight / cursor --> push fake mouse-move event
            SDL_Event sdl_event;
#if defined (SDL_USE) || defined (SDL2_USE)
            sdl_event.type = SDL_MOUSEMOTION;
#elif defined (SDL3_USE)
            sdl_event.type = SDL_EVENT_MOUSE_MOTION;
#endif // SDL_USE || SDL2_USE || SDL3_USE
            sdl_event.motion.x = cursor_position.first;
            sdl_event.motion.y = cursor_position.second;
            if (SDL_PushEvent (&sdl_event) < 0)
              ACE_DEBUG ((LM_ERROR,
                          ACE_TEXT ("failed to SDL_PushEvent(): \"%s\", continuing\n"),
                          ACE_TEXT (SDL_GetError ())));

            // *NOTE*: the cursor remains on the same map tile, so the highlight
            //         will not get redrawn --> do so manually
            myClientAction.command = COMMAND_TILE_HIGHLIGHT_DRAW;
            myClientAction.window = this;
            myClient->action (myClientAction);
          } // end IF

          break;
        }
        // implement keypad navigation
        case SDLK_UP:
        case SDLK_DOWN:
        case SDLK_LEFT:
        case SDLK_RIGHT:
        {
          enum RPG_Map_Direction direction = RPG_MAP_DIRECTION_INVALID;
          myClientAction.position = myView;
          switch (event_in.key.keysym.sym)
          {
            case SDLK_UP:
            {
#if defined (SDL_USE) || defined (SDL2_USE)
              if (event_in.key.keysym.mod & KMOD_SHIFT)
#elif defined (SDL3_USE)
              if (event_in.key.keysym.mod & SDL_KMOD_SHIFT)
#endif // SDL_USE || SDL2_USE || SDL3_USE
              {
                myClientAction.position.first--;
                myClientAction.position.second--;
              } // end IF
              else
                direction = DIRECTION_UP;

              break;
            }
            case SDLK_DOWN:
            {
#if defined (SDL_USE) || defined (SDL2_USE)
              if (event_in.key.keysym.mod & KMOD_SHIFT)
#elif defined (SDL3_USE)
              if (event_in.key.keysym.mod & SDL_KMOD_SHIFT)
#endif // SDL_USE || SDL2_USE || SDL3_USE
              {
                myClientAction.position.first++;
                myClientAction.position.second++;
              } // end IF
              else
                direction = DIRECTION_DOWN;

              break;
            }
            case SDLK_LEFT:
            {
#if defined (SDL_USE) || defined (SDL2_USE)
              if (event_in.key.keysym.mod & KMOD_SHIFT)
#elif defined (SDL3_USE)
              if (event_in.key.keysym.mod & SDL_KMOD_SHIFT)
#endif // SDL_USE || SDL2_USE || SDL3_USE
              {
                myClientAction.position.first--;
                myClientAction.position.second++;
              } // end IF
              else
                direction = DIRECTION_LEFT;

              break;
            }
            case SDLK_RIGHT:
            {
#if defined (SDL_USE) || defined (SDL2_USE)
              if (event_in.key.keysym.mod & KMOD_SHIFT)
#elif defined (SDL3_USE)
              if (event_in.key.keysym.mod & SDL_KMOD_SHIFT)
#endif // SDL_USE || SDL2_USE || SDL3_USE
              {
                myClientAction.position.first++;
                myClientAction.position.second--;
              } // end IF
              else
                direction = DIRECTION_RIGHT;

              break;
            }
            default:
            {
              ACE_DEBUG ((LM_ERROR,
                          ACE_TEXT ("invalid key (was: %u), aborting\n"),
                          event_in.key.keysym.sym));
              break;
            }
          } // end SWITCH

#if defined (SDL_USE) || defined (SDL2_USE)
          if (!(event_in.key.keysym.mod & KMOD_SHIFT))
#elif defined (SDL3_USE)
          if (!(event_in.key.keysym.mod & SDL_KMOD_SHIFT))
#endif // SDL_USE || SDL2_USE || SDL3_USE
          {
            myEngine->lock ();
            myClientAction.entity_id = myEngine->getActive (false); // locked access ?
            if (myClientAction.entity_id == 0)
            {
              // clean up
              myEngine->unlock ();

              break; // nothing to do...
            } // end IF

            struct RPG_Engine_Action player_action;
            player_action.command = COMMAND_TRAVEL;
            // compute target position
            myClientAction.position =
                myEngine->getPosition (myClientAction.entity_id,
                                       false); // locked access ?
            player_action.position = myClientAction.position;
            switch (direction)
            {
              case DIRECTION_UP:
                player_action.position.second--; break;
              case DIRECTION_DOWN:
                player_action.position.second++; break;
              case DIRECTION_LEFT:
                player_action.position.first--; break;
              case DIRECTION_RIGHT:
                player_action.position.first++; break;
              default:
              {
                ACE_DEBUG ((LM_ERROR,
                            ACE_TEXT ("invalid direction (was: \"%s\"), returning\n"),
                            ACE_TEXT (RPG_Map_DirectionHelper::RPG_Map_DirectionToString (direction).c_str ())));
                break;
              }
            } // end SWITCH
            player_action.path.clear ();
            player_action.target = 0;

            if (myEngine->isValid (player_action.position,
                                   false)) // locked access ?
              myEngine->action (myClientAction.entity_id,
                                player_action,
                                false); // locked access ?
            myEngine->unlock ();
          } // end IF
          else
            myClient->setView (myClientAction.position,
                               true); // refresh ?

          break;
        }
        default:
        {
          // delegate all other keypresses to the parent...
          delegate_to_parent = true;

          break;
        }
      } // end SWITCH

      break;
    }
    // *** mouse ***
#if defined (SDL_USE) || defined (SDL2_USE)
    case SDL_MOUSEMOTION:
#elif defined (SDL3_USE)
    case SDL_EVENT_MOUSE_MOTION:
#endif // SDL_USE || SDL2_USE || SDL3_USE
    {
      // find map square
      struct SDL_Rect window_area;
      getArea (window_area,
               true); // toplevel- ?
      myEngine->lock ();
      myClientAction.position =
        RPG_Graphics_Common_Tools::screenToMap (std::make_pair (event_in.motion.x,
                                                                event_in.motion.y),
                                                myEngine->getSize (false), // locked access ?
                                                std::make_pair (window_area.w,
                                                                window_area.h),
                                                myView);
//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("mouse position [%u,%u] --> [%u,%u]\n"),
//                  event_in.button.x,
//                  event_in.button.y,
//                  myClientAction.position.first,
//                  myClientAction.position.second));

      myClientAction.entity_id = myEngine->getActive (false); // locked access ?
      bool has_seen =
        (myClientAction.entity_id ? myEngine->hasSeen (myClientAction.entity_id,
                                                       myClientAction.position,
                                                       false) // locked access ?
                                  : false);

      // change "active" tile ?
      enum RPG_Client_SelectionMode current_mode = myClient->mode ();
      if (myClientAction.position != myClientAction.previous)
      {
        // step1: restore/clear old tile highlight background
        myClientAction.command = COMMAND_TILE_HIGHLIGHT_RESTORE_BG;
        myClientAction.window = this;
        myClient->action (myClientAction);

        bool is_valid = myEngine->isValid (myClientAction.position,
                                           false); // locked access ?
        switch (current_mode)
        {
          case SELECTIONMODE_AIM_CIRCLE:
          {
            unsigned int selection_radius =
                RPG_Map_Common_Tools::distanceMax (myClientAction.source,
                                                   myClientAction.position);
            if (selection_radius > RPG_MAP_CIRCLE_MAX_RADIUS)
              selection_radius = RPG_MAP_CIRCLE_MAX_RADIUS;

            RPG_Map_Common_Tools::buildCircle (myClientAction.source,
                                               myEngine->getSize (false), // locked access ?
                                               selection_radius,
                                               false, // don't fill
                                               myClientAction.positions);

            // *WARNING*: falls through !
          }
          case SELECTIONMODE_AIM_SQUARE:
          {
            if (current_mode == SELECTIONMODE_AIM_SQUARE)
            {
              unsigned int selection_radius =
                  RPG_Map_Common_Tools::distanceMax (myClientAction.source,
                                                     myClientAction.position);
              RPG_Map_Common_Tools::buildSquare (myClientAction.source,
                                                 myEngine->getSize (false), // locked access ?
                                                 selection_radius,
                                                 false, // don't fill
                                                 myClientAction.positions);
            } // end IF

            // step2: remove invalid positions
            RPG_Map_Positions_t obstacles = myEngine->getObstacles (false,  // include entities ?
                                                                    false); // locked access ?
            // *WARNING*: this works for associative containers ONLY
            for (RPG_Map_PositionsIterator_t iterator = myClientAction.positions.begin ();
                 iterator != myClientAction.positions.end ();
                 )
              if (RPG_Map_Common_Tools::hasLineOfSight (myClientAction.source,
                                                        *iterator,
                                                        obstacles,
                                                        false)) // allow target is obstacle ?
                iterator++;
              else
                myClientAction.positions.erase (iterator++);

            break;
          }
          case SELECTIONMODE_NORMAL:
            break;
          case SELECTIONMODE_PATH:
          {
            if (myClientAction.entity_id > 0 &&
                is_valid                     &&
                has_seen)
            {
              RPG_Map_Position_t current_position =
                  myEngine->getPosition (myClientAction.entity_id,
                                         false); // locked access ?
              if (current_position != myClientAction.position)
              {
                if (!myEngine->findPath (current_position,
                                         myClientAction.position,
                                         myClientAction.path,
                                         false)) // locked access ?
                {
                  //ACE_DEBUG((LM_DEBUG,
                  //           ACE_TEXT("could not find a path [%u,%u] --> [%u,%u], aborting\n"),
                  //           current_position.first,
                  //           current_position.second,
                  //           myClientAction.position.first,
                  //           myClientAction.position.second));

                  // pointing at an invalid (==unreachable) position (still on the map)
                  // --> erase cached path (and tile highlights)
                  myClientAction.path.clear ();
                } // end IF
              } // end IF
            } // end IF
            else
            {
              // pointing at an invalid (==unreachable) position (still on the map)
              // --> erase cached path (and tile highlights)
              myClientAction.path.clear ();
            } // end ELSE

            break;
          }
          default:
          {
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("invalid/unknown selection mode (was: %d), continuing\n"),
                        current_mode));
            break;
          }
        } // end SWITCH

        // draw tile highlight(s) ?
        if (has_seen &&
            is_valid)
        {
          myClientAction.command = COMMAND_TILE_HIGHLIGHT_DRAW;
          myClientAction.window = this;
          myClient->action (myClientAction);
        } // end IF
      } // end IF

      // set an appropriate cursor
      myClientAction.cursor =
          RPG_Client_Common_Tools::getCursor (myClientAction.position,
                                              myClientAction.entity_id,
                                              has_seen,
                                              current_mode,
                                              *myEngine,
                                              false); // locked access ?
      myEngine->unlock ();

      if (myClientAction.cursor != RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance ()->type ())
      {
        myClientAction.command = COMMAND_CURSOR_SET;
        myClient->action (myClientAction);
      } // end IF

      if (myClientAction.position != myClientAction.previous)
        myClientAction.previous = myClientAction.position;

      break;
    }
#if defined (SDL_USE) || defined (SDL2_USE)
    case SDL_MOUSEBUTTONDOWN:
#elif defined (SDL3_USE)
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
#endif // SDL_USE || SDL2_USE || SDL3_USE
    {
//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("mouse button [%u,%u] pressed\n"),
//                  static_cast<unsigned int>(event_in.button.which),
//                  static_cast<unsigned int>(event_in.button.button)));

      struct SDL_Rect window_area;
      getArea (window_area,
               true); // toplevel- ?
      if (event_in.button.button == 1) // left-click
      {
        myEngine->lock ();
        RPG_Graphics_Position_t map_position =
            RPG_Graphics_Common_Tools::screenToMap (std::make_pair (event_in.button.x,
                                                                    event_in.button.y),
                                                    myEngine->getSize (false), // locked access ?
                                                    std::make_pair (window_area.w,
                                                                    window_area.h),
                                                    myView);
        //ACE_DEBUG((LM_DEBUG,
        //           ACE_TEXT("mouse position [%u,%u] --> map position [%u,%u]\n"),
        //           event_in.button.x,
        //           event_in.button.y,
        //           map_position.first,
        //           map_position.second));

        myClientAction.entity_id = myEngine->getActive (false); // locked access ?
        bool has_seen =
          (myClientAction.entity_id ? myEngine->hasSeen (myClientAction.entity_id,
                                                         map_position,
                                                         false) // locked access ?
                                    : false);
        if ((myClientAction.entity_id == 0) ||
            !has_seen)
        {
          myEngine->unlock ();
          break; // --> no player/vision, no action...
        } // end IF

        myClientAction.position =
          myEngine->getPosition (myClientAction.entity_id,
                                 false); // locked access ?

        struct RPG_Engine_Action player_action;
        player_action.command = RPG_ENGINE_COMMAND_INVALID;
        player_action.position = map_position;
        //player_action.path.clear();
        player_action.target = myEngine->hasEntity (map_position,
                                                    false); // locked access ?
        // self ?
        if (player_action.target == myClientAction.entity_id)
        {
          myEngine->unlock ();
          break;
        } // end IF

        // clicked on monster ?
        if (player_action.target                        &&
            myEngine->canSee (myClientAction.entity_id,
                              map_position,
                              false)                    && // locked access ?
            myEngine->isMonster (player_action.target,
                                 false)) // locked access ?
        {
          // attack/pursue selected monster
          player_action.command = COMMAND_ATTACK;

          // reuse existing path ?
          if (!myEngine->canReach (myClientAction.entity_id,
                                   map_position,
                                   false)                    &&  // locked access ?
              (myClient->mode () == SELECTIONMODE_PATH)      &&
              !myClientAction.path.empty ())
          { ACE_ASSERT (myClientAction.path.front ().first == myClientAction.position);
            ACE_ASSERT (myClientAction.path.back ().first == player_action.position);

            // path exists --> reuse it
            player_action.path = myClientAction.path;
            player_action.path.pop_front ();

            myClient->mode (SELECTIONMODE_NORMAL);
            myClientAction.path.clear ();
            // restore/clear old tile highlight background
            myClientAction.command = COMMAND_TILE_HIGHLIGHT_RESTORE_BG;
            myClientAction.window = this;
            myClient->action (myClientAction);
            myClientAction.command = COMMAND_TILE_HIGHLIGHT_DRAW;
            myClient->action (myClientAction);
          } // end IF
          myEngine->action (myClientAction.entity_id,
                            player_action,
                            false); // locked access ?

          myEngine->unlock ();
          break;
        } // end IF
        player_action.target = 0;

        // player standing next to door ?
        enum RPG_Map_Element map_element = myEngine->getElement (map_position,
                                                                 false); // locked access ?
        switch (map_element)
        {
          case MAPELEMENT_DOOR:
          {
            enum RPG_Map_DoorState door_state = myEngine->state (map_position,
                                                                 false); // locked access ?

            // (try to) handle door ?
            if (RPG_Map_Common_Tools::isAdjacent (myClientAction.position,
                                                  map_position))
            {
              bool ignore_door = false;
              switch (door_state)
              {
                case DOORSTATE_OPEN:
                  player_action.command = COMMAND_DOOR_CLOSE;
                  break;
                case DOORSTATE_CLOSED:
                case DOORSTATE_LOCKED:
                  player_action.command = COMMAND_DOOR_OPEN;
                  break;
                case DOORSTATE_BROKEN:
                  ignore_door = true;
                  break;
                default:
                {
                  ACE_DEBUG ((LM_ERROR,
                              ACE_TEXT ("[%u,%u]: invalid door state (was: \"%s\"), returning\n"),
                              map_position.first, map_position.second,
                              ACE_TEXT (RPG_Map_DoorStateHelper::RPG_Map_DoorStateToString (door_state).c_str ())));
                  myEngine->unlock ();
                  return;
                }
              } // end SWITCH
              player_action.position = map_position;

              if (!ignore_door)
                myEngine->action (myClientAction.entity_id,
                                  player_action,
                                  false); // locked access ?
            } // end IF
            else if ((door_state == DOORSTATE_CLOSED) ||
                     (door_state == DOORSTATE_LOCKED))
            {
              // cannot travel there --> done
              break;
            } // end ELSEIF

            // done ?
            if (player_action.command == COMMAND_DOOR_CLOSE)
            {
              break;
            } // end IF

            // *WARNING*: falls through !
          }
          case MAPELEMENT_FLOOR:
          {
            // floor / (open/broken) door --> (try to) travel to this position

            player_action.command = COMMAND_TRAVEL;

            // reuse existing path ?
            if ((myClient->mode () == SELECTIONMODE_PATH) &&
                !myClientAction.path.empty ())
            { ACE_ASSERT (myClientAction.path.front ().first == myClientAction.position);
              ACE_ASSERT (myClientAction.path.back ().first == player_action.position);

              // path exists --> reuse it
              player_action.path = myClientAction.path;
              player_action.path.pop_front ();

              myClient->mode (SELECTIONMODE_NORMAL);
              myClientAction.path.clear ();
              // restore/clear old tile highlight background
              myClientAction.command = COMMAND_TILE_HIGHLIGHT_RESTORE_BG;
              myClientAction.window = this;
              myClient->action (myClientAction);
              myClientAction.command = COMMAND_TILE_HIGHLIGHT_DRAW;
              myClient->action (myClientAction);
            } // end IF

            myEngine->action (myClientAction.entity_id,
                              player_action,
                              false); // locked access ?

            break;
          }
          default:
            break;
        } // end SWITCH
        myEngine->unlock ();
      } // end IF (event_in.button.button == 1)

      break;
    }
    case RPG_GRAPHICS_SDL_HOVEREVENT:
    {
//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("RPG_GRAPHICS_SDL_HOVEREVENT event...\n")));

      break;
    }
    case RPG_GRAPHICS_SDL_MOUSEMOVEOUT:
    {
//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("RPG_GRAPHICS_SDL_MOUSEMOVEOUT event...\n")));

      // restore/clear tile highlight BG
      //myClientAction.command = COMMAND_TILE_HIGHLIGHT_RESTORE_BG;
      //myClient->action (myClientAction);

      //myClientAction.command = COMMAND_TILE_HIGHLIGHT_INVALIDATE_BG;
      //myClient->action (myClientAction);

      break;
    }
    default:
    {
      // delegate these to the parent...
      delegate_to_parent = true;

      break;
    }
  } // end SWITCH

  if (delegate_to_parent)
    getParent ()->handleEvent (event_in,
                               window_in,
                               dirtyRegion_out);
}

bool
RPG_Client_Window_Level_Stub::setStyle (const struct RPG_Graphics_StyleUnion& style_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Window_Level_Stub::setStyle"));

  switch (style_in.discriminator)
  {
    case RPG_Graphics_StyleUnion::EDGESTYLE:
    case RPG_Graphics_StyleUnion::FLOORSTYLE:
    case RPG_Graphics_StyleUnion::WALLSTYLE:
    case RPG_Graphics_StyleUnion::DOORSTYLE:
      break;
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid style (was: %d), aborting\n"),
                  style_in.discriminator));
      return false;
    }
  } // end SWITCH

  return true;
}

bool
RPG_Client_Window_Level_Stub::initMiniMap (bool debug_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Window_Level_Stub::initMiniMap"));

  // sanity check(s)
  ACE_ASSERT (myClient);
  ACE_ASSERT (myEngine);
  ACE_ASSERT (inherited::screen_);

  RPG_Graphics_Offset_t offset =
      std::make_pair (std::numeric_limits<int>::max (),
                      std::numeric_limits<int>::max ());
  RPG_Client_Window_MiniMap* minimap_window = NULL;
  ACE_NEW_NORETURN (minimap_window,
                    RPG_Client_Window_MiniMap (*this,
                                               offset,
                                               debug_in));
  if (!minimap_window)
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory(%u): %m, aborting\n"),
                sizeof (RPG_Client_Window_MiniMap)));
    return false;
  } // end IF
  minimap_window->initialize (myClient,
                              myEngine);
  minimap_window->initializeSDL (inherited::renderer_,
                                 inherited::screen_,
                                 inherited::GLContext_);

  return true;
}

bool
RPG_Client_Window_Level_Stub::initMessageWindow ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Window_Level_Stub::initMessageWindow"));

  // sanity check(s)
  ACE_ASSERT (myClient);
  ACE_ASSERT (inherited::screen_);

  RPG_Client_Window_Message* message_window = NULL;
  ACE_NEW_NORETURN (message_window,
                    RPG_Client_Window_Message (*this));
  if (!message_window)
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory(%u): %m, aborting\n"),
                sizeof (RPG_Client_Window_Message)));
    return false;
  } // end IF
  message_window->initialize (inherited::screenLock_, // screen lock handle
                              RPG_CLIENT_MESSAGE_FONT,
                              RPG_CLIENT_MESSAGE_DEF_NUM_LINES);
  message_window->initializeSDL (inherited::renderer_,
                                 inherited::screen_,
                                 inherited::GLContext_);

  return true;
}
