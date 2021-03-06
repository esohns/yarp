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
#include "stdafx.h"

#include "rpg_client_window_level.h"

#include "rpg_client_defines.h"
#include "rpg_client_engine.h"
#include "rpg_client_common_tools.h"
#include "rpg_client_entity_manager.h"
#include "rpg_client_window_minimap.h"
#include "rpg_client_window_message.h"

#include "rpg_engine.h"
#include "rpg_engine_command.h"
#include "rpg_engine_common_tools.h"

#include "rpg_graphics_defines.h"
#include "rpg_graphics_surface.h"
#include "rpg_graphics_cursor_manager.h"
#include "rpg_graphics_common_tools.h"
#include "rpg_graphics_SDL_tools.h"

#include "rpg_map_common_tools.h"
#include "rpg_map_pathfinding_tools.h"

#include "rpg_common_macros.h"

#include "ace/Log_Msg.h"

#include <sstream>

RPG_Client_Window_Level::RPG_Client_Window_Level(const RPG_Graphics_SDLWindowBase& parent_in)
 : inherited(WINDOW_MAP,           // type
             parent_in,            // parent
             std::make_pair(0, 0), // offset
             std::string()),       // title
//             NULL),                // background
   myEngine(NULL),
   myClient(NULL),
//   myClientAction(),
   myDrawMinimap(RPG_CLIENT_MINIMAP_DEF_ISON),
#ifdef _DEBUG
   myShowCoordinates(false),
#endif
   myShowMessages(RPG_CLIENT_MESSAGE_DEF_ISON),
//   myCurrentMapStyle(mapStyle_in),
//   myCurrentFloorSet(),
//   myCurrentFloorEdgeSet(),
//   myCurrentWallSet(),
   myCeilingTile(NULL),
//   myCurrentDoorSet(),
   myOffMapTile(NULL),
   myInvisibleTile(NULL),
   myVisionBlendTile(NULL),
   myVisionTempTile(NULL),
//   myFloorEdgeTiles(),
//   myWallTiles(),
//   myDoorTiles(),
   myWallBlend(NULL),
//   myLightingCache(),
   myView(std::make_pair(std::numeric_limits<unsigned int>::max(),
                         std::numeric_limits<unsigned int>::max()))
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Window_Level::RPG_Client_Window_Level"));

  // init client action
  myClientAction.command = RPG_CLIENT_COMMAND_INVALID;
  myClientAction.previous =
      std::make_pair(std::numeric_limits<unsigned int>::max(),
                     std::numeric_limits<unsigned int>::max());
  myClientAction.position =
      std::make_pair(std::numeric_limits<unsigned int>::max(),
                     std::numeric_limits<unsigned int>::max());
  myClientAction.window = this;
  myClientAction.cursor = RPG_GRAPHICS_CURSOR_INVALID;
  myClientAction.entity_id = 0;
  myClientAction.path.clear();
  myClientAction.source =
      std::make_pair(std::numeric_limits<unsigned int>::max(),
                     std::numeric_limits<unsigned int>::max());
  myClientAction.positions.clear();

  ACE_OS::memset(&myCurrentFloorEdgeSet,
                 0,
                 sizeof(myCurrentFloorEdgeSet));
  ACE_OS::memset(&myCurrentWallSet,
                 0,
                 sizeof(myCurrentWallSet));
  ACE_OS::memset(&myCurrentDoorSet,
                 0,
                 sizeof(myCurrentDoorSet));
//   RPG_Client_Common_Tools::initFloorEdges(myEngine->getFloorPlan(),
//                                           myCurrentFloorEdgeSet,
//                                           myFloorEdgeTiles);
//   RPG_Client_Common_Tools::initWalls(myEngine->getFloorPlan(),
//                                      myCurrentWallSet,
//                                      myWallTiles);
//   RPG_Client_Common_Tools::initDoors(myEngine->getFloorPlan(),
//                                      *myEngine,
//                                      myCurrentDoorSet,
//                                      myDoorTiles);

//   initWallBlend(false);

  // init ceiling tile
  initCeiling();

  // load tile for unmapped areas
  RPG_Graphics_GraphicTypeUnion type;
  type.discriminator = RPG_Graphics_GraphicTypeUnion::TILEGRAPHIC;
  type.tilegraphic = TILE_OFF_MAP;
  myOffMapTile =
      RPG_Graphics_Common_Tools::loadGraphic(type,   // tile
                                             true,   // convert to display format
                                             false); // don't cache
  if (!myOffMapTile)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Graphics_Common_Tools::loadGraphic(\"%s\"), continuing\n"),
               ACE_TEXT(RPG_Graphics_Common_Tools::toString(type).c_str())));

  // load tile for invisible areas
  type.tilegraphic = TILE_FLOOR_INVISIBLE;
  myInvisibleTile =
      RPG_Graphics_Common_Tools::loadGraphic(type,   // tile
                                             true,   // convert to display format
                                             false); // don't cache
  if (!myInvisibleTile)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Graphics_Common_Tools::loadGraphic(\"%s\"), continuing\n"),
               ACE_TEXT(RPG_Graphics_Common_Tools::toString(type).c_str())));

  // create tile for vision blending/temp
  myVisionBlendTile = RPG_Graphics_Surface::copy(*myOffMapTile);
  if (!myVisionBlendTile)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Graphics_Surface::copy(%@), aborting\n"),
               myOffMapTile));

    return;
  } // end IF
  RPG_Graphics_Surface::alpha(static_cast<Uint8>((RPG_GRAPHICS_TILE_PREVSEEN_DEF_OPACITY * SDL_ALPHA_OPAQUE)),
                              *myVisionBlendTile);
  myVisionTempTile =
      RPG_Graphics_Surface::create(RPG_GRAPHICS_TILE_FLOOR_WIDTH,
                                   RPG_GRAPHICS_TILE_FLOOR_HEIGHT);
  if (!myVisionTempTile)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Graphics_Surface::create(%u,%u), aborting\n"),
               RPG_GRAPHICS_TILE_FLOOR_WIDTH,
               RPG_GRAPHICS_TILE_FLOOR_HEIGHT));

    return;
  } // end IF
}

RPG_Client_Window_Level::~RPG_Client_Window_Level()
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Window_Level::~RPG_Client_Window_Level"));

  // clean up
  for (RPG_Graphics_FloorTilesConstIterator_t iterator = myCurrentFloorSet.tiles.begin();
       iterator != myCurrentFloorSet.tiles.end();
       iterator++)
    SDL_FreeSurface((*iterator).surface);

  if (myCurrentFloorEdgeSet.east.surface)
    SDL_FreeSurface(myCurrentFloorEdgeSet.east.surface);
  if (myCurrentFloorEdgeSet.west.surface)
    SDL_FreeSurface(myCurrentFloorEdgeSet.west.surface);
  if (myCurrentFloorEdgeSet.north.surface)
    SDL_FreeSurface(myCurrentFloorEdgeSet.north.surface);
  if (myCurrentFloorEdgeSet.south.surface)
    SDL_FreeSurface(myCurrentFloorEdgeSet.south.surface);
  if (myCurrentFloorEdgeSet.south_east.surface)
    SDL_FreeSurface(myCurrentFloorEdgeSet.south_east.surface);
  if (myCurrentFloorEdgeSet.south_west.surface)
    SDL_FreeSurface(myCurrentFloorEdgeSet.south_west.surface);
  if (myCurrentFloorEdgeSet.north_east.surface)
    SDL_FreeSurface(myCurrentFloorEdgeSet.north_east.surface);
  if (myCurrentFloorEdgeSet.north_west.surface)
    SDL_FreeSurface(myCurrentFloorEdgeSet.north_west.surface);
  if (myCurrentFloorEdgeSet.top.surface)
    SDL_FreeSurface(myCurrentFloorEdgeSet.top.surface);
  if (myCurrentFloorEdgeSet.right.surface)
    SDL_FreeSurface(myCurrentFloorEdgeSet.right.surface);
  if (myCurrentFloorEdgeSet.left.surface)
    SDL_FreeSurface(myCurrentFloorEdgeSet.left.surface);
  if (myCurrentFloorEdgeSet.bottom.surface)
    SDL_FreeSurface(myCurrentFloorEdgeSet.bottom.surface);

  if (myCurrentWallSet.east.surface)
    SDL_FreeSurface(myCurrentWallSet.east.surface);
  if (myCurrentWallSet.west.surface)
    SDL_FreeSurface(myCurrentWallSet.west.surface);
  if (myCurrentWallSet.north.surface)
    SDL_FreeSurface(myCurrentWallSet.north.surface);
  if (myCurrentWallSet.south.surface)
    SDL_FreeSurface(myCurrentWallSet.south.surface);

  if (myWallBlend)
    SDL_FreeSurface(myWallBlend);

  for (RPG_Client_BlendingMaskCacheIterator_t iterator = myLightingCache.begin();
       iterator != myLightingCache.end();
       iterator++)
    SDL_FreeSurface(*iterator);

  if (myCurrentDoorSet.horizontal_open.surface)
    SDL_FreeSurface(myCurrentDoorSet.horizontal_open.surface);
  if (myCurrentDoorSet.vertical_open.surface)
    SDL_FreeSurface(myCurrentDoorSet.vertical_open.surface);
  if (myCurrentDoorSet.horizontal_closed.surface)
    SDL_FreeSurface(myCurrentDoorSet.horizontal_closed.surface);
  if (myCurrentDoorSet.vertical_closed.surface)
    SDL_FreeSurface(myCurrentDoorSet.vertical_closed.surface);
  if (myCurrentDoorSet.broken.surface)
    SDL_FreeSurface(myCurrentDoorSet.broken.surface);

  if (myCeilingTile)
    SDL_FreeSurface(myCeilingTile);

  if (myOffMapTile)
    SDL_FreeSurface(myOffMapTile);

  if (myInvisibleTile)
    SDL_FreeSurface(myInvisibleTile);

  if (myVisionBlendTile)
    SDL_FreeSurface(myVisionBlendTile);
  if (myVisionTempTile)
    SDL_FreeSurface(myVisionTempTile);
}

void
RPG_Client_Window_Level::setView(const RPG_Map_Position_t& view_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Window_Level::setView"));

  ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

  myView = view_in;
}

void
RPG_Client_Window_Level::setView (int offsetX_in,
                                  int offsetY_in,
                                  bool lockedAccess_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Window_Level::setView"));

  RPG_Map_Size_t size = myEngine->getSize(lockedAccess_in);

  ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

  // handle over-/underruns
  if ((offsetX_in < 0) &&
      (static_cast<unsigned int>(-offsetX_in) > myView.first))
    myView.first = 0;
  else
    myView.first += offsetX_in;

  if ((offsetY_in < 0) &&
      (static_cast<unsigned int>(-offsetY_in) > myView.second))
    myView.second = 0;
  else
    myView.second += offsetY_in;

  if (myView.first >= size.first)
    myView.first = (size.first - 1);
  if (myView.second >= size.second)
    myView.second = (size.second - 1);
}

RPG_Graphics_Position_t
RPG_Client_Window_Level::getView() const
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Window_Level::getView"));

  RPG_Graphics_Position_t result;

  {
    ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

    result = myView;
  } // end lock scope

  return result;
}

void
RPG_Client_Window_Level::toggleMiniMap()
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Window_Level::toggleMiniMap"));

  ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

  myDrawMinimap = !myDrawMinimap;

  // retrieve map window handle
  RPG_Graphics_WindowsConstIterator_t iterator = children_.begin();
  for (;
       iterator != children_.end();
       iterator++)
  {
    if ((*iterator)->getType() == WINDOW_MINIMAP)
      break;
  } // end FOR
  ACE_ASSERT((*iterator)->getType() == WINDOW_MINIMAP);

  // hide/draw
  myClientAction.command = (myDrawMinimap ? COMMAND_WINDOW_DRAW
                                          : COMMAND_WINDOW_HIDE);
  myClientAction.window = *iterator;
  myClient->action(myClientAction);

  // reset window
   myClientAction.window = this;
}

bool
RPG_Client_Window_Level::showMiniMap() const
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Window_Level::showMiniMap"));

  ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

  return myDrawMinimap;
}

void
RPG_Client_Window_Level::toggleMessages()
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Window_Level::toggleMessages"));

  ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

  myShowMessages = !myShowMessages;
}

bool
RPG_Client_Window_Level::showMessages() const
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Window_Level::showMessages"));

  ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

  return myShowMessages;
}

void
RPG_Client_Window_Level::toggleDoor(const RPG_Map_Position_t& position_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Window_Level::toggleDoor"));

  ACE_ASSERT(myEngine);

//  myEngine->lock();
  bool is_open = (myEngine->state(position_in,
                                  false) == DOORSTATE_OPEN);

  // change tile accordingly
  RPG_Graphics_Orientation orientation = RPG_GRAPHICS_ORIENTATION_INVALID;
  orientation = RPG_Client_Common_Tools::getDoorOrientation(position_in,
                                                            *myEngine,
                                                            false);
//  myEngine->unlock();

  ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

  switch (orientation)
  {
    case ORIENTATION_HORIZONTAL:
    {
      myDoorTiles[position_in] =
          (is_open ? myCurrentDoorSet.horizontal_open
                   : myCurrentDoorSet.horizontal_closed);
      break;
    }
    case ORIENTATION_VERTICAL:
    {
      myDoorTiles[position_in] = (is_open ? myCurrentDoorSet.vertical_open
                                          : myCurrentDoorSet.vertical_closed);

      break;
    }
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid door orientation \"%s\", aborting\n"),
                 ACE_TEXT(RPG_Graphics_OrientationHelper::RPG_Graphics_OrientationToString(orientation).c_str())));

      return;
    }
  } // end SWITCH
}

#ifdef _DEBUG
void
RPG_Client_Window_Level::toggleShowCoordinates()
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Window_Level::toggleShowCoordinates"));

  myShowCoordinates = !myShowCoordinates;
}
#endif

bool
RPG_Client_Window_Level::initialize (RPG_Client_Engine* clientEngine_in,
                                     RPG_Engine* engine_in,
                                     bool debug_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Window_Level::initialize"));

  // sanity checks
  ACE_ASSERT (clientEngine_in);
  ACE_ASSERT (engine_in);

  myClient = clientEngine_in;
  myEngine = engine_in;

  // init edge, wall, door tiles
  initialize (myClient->getStyle ());

  // init minimap/message windows
  if (!initMiniMap (debug_in)/* ||
      !initMessageWindow ()*/)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to initialize sub-windows, aborting\n")));
    return false;
  } // end IF

  return true;
}

void
RPG_Client_Window_Level::drawBorder (SDL_Surface* targetSurface_in,
                                     unsigned int offsetX_in,
                                     unsigned int offsetY_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Window_Level::drawBorder"));

  // *NOTE*: should NEVER be reached !
  ACE_ASSERT (false);

#if defined (_MSC_VER)
  return;
#else
  ACE_NOTREACHED(return;)
#endif
}

void
RPG_Client_Window_Level::drawChild (const RPG_Graphics_WindowType& child_in,
                                    SDL_Surface* targetSurface_in,
                                    unsigned int offsetX_in,
                                    unsigned int offsetY_in,
                                    bool refresh_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Window_Level::drawChild"));

  ACE_Guard<ACE_Thread_Mutex> aGuard (myLock);

  // sanity check(s)
  ACE_ASSERT(child_in != RPG_GRAPHICS_WINDOWTYPE_INVALID);
  // set target surface
  SDL_Surface* target_surface = (targetSurface_in ? targetSurface_in
                                                  : inherited::screen_);
  ACE_ASSERT(target_surface);

  // draw any child(ren) of a specific type
  for (RPG_Graphics_WindowsIterator_t iterator = inherited::children_.begin ();
       iterator != inherited::children_.end ();
       iterator++)
  {
    if (((*iterator)->getType () != child_in) ||
        ((child_in == WINDOW_MINIMAP) && !myDrawMinimap)  || // minimap switched off...
        ((child_in == WINDOW_MESSAGE) && !myShowMessages))   // message window switched off...
      continue;

    try
    {
      (*iterator)->draw (target_surface,
                         offsetX_in,
                         offsetY_in);
    }
    catch (...)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("caught exception in RPG_Graphics_IWindow::draw(), continuing\n")));
    }

    if (refresh_in)
    {
      try
      {
        (*iterator)->update (target_surface);
      }
      catch (...)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("caught exception in RPG_Graphics_IWindow::update(), continuing\n")));
      }
    } // end IF
  } // end FOR
}

void
RPG_Client_Window_Level::initialize (const RPG_Graphics_Style& style_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Window_Level::initialize"));

  ACE_Guard<ACE_Thread_Mutex> aGuard (myLock);

  // init style
  RPG_Graphics_StyleUnion style;
  style.discriminator = RPG_Graphics_StyleUnion::FLOORSTYLE;
  style.floorstyle = style_in.floor;
  if (!setStyle (style))
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Client_Window_Level::setStyle(FLOORSTYLE), continuing\n")));
  style.discriminator = RPG_Graphics_StyleUnion::EDGESTYLE;
  style.edgestyle = style_in.edge;
  if (!setStyle (style))
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Client_Window_Level::setStyle(EDGESTYLE), continuing\n")));
  style.discriminator = RPG_Graphics_StyleUnion::WALLSTYLE;
  style.wallstyle = style_in.wall;
  if (!setStyle (style))
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Client_Window_Level::setStyle(WALLSTYLE), continuing\n")));
  style.discriminator = RPG_Graphics_StyleUnion::DOORSTYLE;
  style.doorstyle = style_in.door;
  if (!setStyle (style))
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Client_Window_Level::setStyle(DOORSTYLE), continuing\n")));

  // init tiles / position
  RPG_Client_Common_Tools::initFloorEdges (*myEngine,
                                           myCurrentFloorEdgeSet,
                                           myFloorEdgeTiles);
  RPG_Client_Common_Tools::initWalls (*myEngine,
                                      myCurrentWallSet,
                                      myWallTiles);
  RPG_Client_Common_Tools::initDoors (*myEngine,
                                      myCurrentDoorSet,
                                      myDoorTiles);
}

void
RPG_Client_Window_Level::setBlendRadius (unsigned char radius_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Window_Level::setBlendRadius"));

  ACE_Guard<ACE_Thread_Mutex> aGuard (myLock);

  // sanity check
  if (radius_in == 0)
  {
    for (RPG_Client_BlendingMaskCacheIterator_t iterator = myLightingCache.begin ();
         iterator != myLightingCache.end ();
         iterator++)
      SDL_FreeSurface (*iterator);
    myLightingCache.clear ();

    return;
  } // end IF

  // grow/shrink cache as necessary
  int delta = myLightingCache.size () - radius_in;
  if (delta > 0)
  {
    for (int i = delta;
         i > 0;
         i--)
    {
      SDL_FreeSurface (myLightingCache.back ());
      myLightingCache.pop_back ();
    } // end FOR
  } // end IF
  else if (delta < 0)
  {
    SDL_Surface* new_entry = NULL;
    for (int i = -delta;
         i > 0;
         i--)
    {
      new_entry = RPG_Graphics_Surface::create (RPG_GRAPHICS_TILE_FLOOR_WIDTH,
                                                RPG_GRAPHICS_TILE_FLOOR_HEIGHT);
      if (!new_entry)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to RPG_Graphics_Surface::create(%u,%u), returning\n"),
                    RPG_GRAPHICS_TILE_FLOOR_WIDTH,
                    RPG_GRAPHICS_TILE_FLOOR_HEIGHT));
        return;
      } // end IF
      myLightingCache.push_back(new_entry);
    } // end IF
  } // end ELSEIF
  ACE_ASSERT(myLightingCache.size() == radius_in);

  // *NOTE*: quantum == (SDL_ALPHA_OPAQUE / (visible_radius + 1));
  Uint8 quantum = static_cast<Uint8>(SDL_ALPHA_OPAQUE / (radius_in + 1));
  RPG_Client_BlendingMaskCacheIterator_t iterator = myLightingCache.begin ();
  for (unsigned int i = 1;
       i <= radius_in;
       i++, iterator++)
  {
    RPG_Graphics_Surface::copy (*myOffMapTile,
                                **iterator);
    RPG_Graphics_Surface::alpha ((i * quantum),
                                 **iterator);
  } // end FOR
}

void
RPG_Client_Window_Level::updateMinimap ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Window_Level::updateMinimap"));

  // sanity check
  if (!showMiniMap ())
    return;

  drawChild (WINDOW_MINIMAP,
             NULL,
             0, 0,
             true);
}

void
RPG_Client_Window_Level::updateMessageWindow (const std::string& message_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Window_Level::updateMessageWindow"));

  RPG_Graphics_IWindowBase* child = inherited::child (WINDOW_MESSAGE);
  if (!child)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to RPG_Graphics_SDLWindowBase::child(WINDOW_MESSAGE), returning\n")));
    return;
  } // end IF
  RPG_Client_Window_Message* message_window =
    dynamic_cast<RPG_Client_Window_Message*>(child);
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
             true);
}

void
RPG_Client_Window_Level::draw (SDL_Surface* targetSurface_in,
                               unsigned int offsetX_in,
                               unsigned int offsetY_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Window_Level::draw"));

//  ACE_Guard<ACE_Thread_Mutex> aGuard (myLock);

  // sanity check(s)
  ACE_ASSERT (myEngine);
  ACE_ASSERT (myCeilingTile);
  ACE_ASSERT (myOffMapTile);
  ACE_ASSERT (myInvisibleTile);
  ACE_ASSERT (myVisionBlendTile);
  SDL_Surface* target_surface = (targetSurface_in ? targetSurface_in
                                                  : inherited::screen_);
  ACE_ASSERT (target_surface);
  ACE_ASSERT (static_cast<int> (offsetX_in) <= target_surface->w);
  ACE_ASSERT (static_cast<int> (offsetY_in) <= target_surface->h);

  // init clipping
  inherited::clip (target_surface,
                   offsetX_in,
                   offsetY_in);

  // *NOTE*: mapping tile coordinates to world-, and world- to screen coordinates basically
  // works as follows (DImetric projection):
  // 1. the "tile" coordinates are represented by:
  //    - RPG_GRAPHICS_TILE_HEIGHT_MOD:
  //      (== sin(a==26.565°)*e == 0.4472*e, e == length of a tile edge)
  //    - RPG_GRAPHICS_TILE_WIDTH_MOD:
  //      (== cos(a==26.565°)*e == 0.8944*e)
  // 2. world (== map-) coordinates of a (checkerboard) set of tiles are represented by:
  //    - Xworld+1 = Xscreen + cos(a)*e, Yscreen + sin(a)*e
  //    - Yworld+1 = Xscreen - cos(a)*e, Yscreen + sin(a)*e
  //    --> Xscreen = (Xworld - Yworld)*cos(a)*e (== (Xworld - Yworld)*RPG_GRAPHICS_TILE_WIDTH_MOD)
  //    --> Yscreen = (Xworld + Yworld)*sin(a)*e (== (Xworld + Yworld)*RPG_GRAPHICS_TILE_HEIGHT_MOD)
  // 3. screen coordinates derive from the screen (== target surface T) center
  //    - Xcenter = (Twidth / 2), Ycenter = (Theight / 2)
  //    ...and the world (== map) focus (== current tile). Then, the "origin" (== center of a tile
  //    relative to the tile at [0,0]) derives to something like:
  //    - Xorigin = (Xfocus - Yfocus)*cos(a)*e
  //    - Yorigin = (Xworld + Yworld + 1)*sin(a)*e
  // 4. given these reference points, map-to-screen translation derives to something like:
  //    - Xscreen = (Xworld - Yworld)*cos(a)*e - Xorigin + Xcenter
  //    - Yscreen = (Xworld + Yworld)*sin(a)*e - Yorigin + Ycenter

  // position of the top right corner
  RPG_Graphics_Position_t top_right = std::make_pair (0, 0);
//   top_right.first = (((-RPG_GRAPHICS_TILE_HEIGHT_MOD * ((targetSurface->w / 2) + 50)) +
//                       (RPG_GRAPHICS_TILE_WIDTH_MOD * ((targetSurface->h / 2) + 50)) +
//                       (RPG_GRAPHICS_TILE_WIDTH_MOD * RPG_GRAPHICS_TILE_HEIGHT_MOD)) /
//                      (2 * RPG_GRAPHICS_TILE_WIDTH_MOD * RPG_GRAPHICS_TILE_HEIGHT_MOD));
//   top_right.second = (((RPG_GRAPHICS_TILE_HEIGHT_MOD * ((targetSurface->w / 2) + 50)) +
//                        (RPG_GRAPHICS_TILE_WIDTH_MOD * ((targetSurface->h / 2) + 50)) +
//                        (RPG_GRAPHICS_TILE_WIDTH_MOD * RPG_GRAPHICS_TILE_HEIGHT_MOD)) /
//                       (2 * RPG_GRAPHICS_TILE_WIDTH_MOD * RPG_GRAPHICS_TILE_HEIGHT_MOD));
  top_right.first =
      (((-RPG_GRAPHICS_TILE_HEIGHT_MOD * ((target_surface->w / 2))) +
        (RPG_GRAPHICS_TILE_WIDTH_MOD * ((target_surface->h / 2))) +
        (RPG_GRAPHICS_TILE_WIDTH_MOD * RPG_GRAPHICS_TILE_HEIGHT_MOD)) /
       (2 * RPG_GRAPHICS_TILE_WIDTH_MOD * RPG_GRAPHICS_TILE_HEIGHT_MOD));
  top_right.second =
      (((RPG_GRAPHICS_TILE_HEIGHT_MOD * ((target_surface->w / 2))) +
        (RPG_GRAPHICS_TILE_WIDTH_MOD * ((target_surface->h / 2))) +
        (RPG_GRAPHICS_TILE_WIDTH_MOD * RPG_GRAPHICS_TILE_HEIGHT_MOD)) /
       (2 * RPG_GRAPHICS_TILE_WIDTH_MOD * RPG_GRAPHICS_TILE_HEIGHT_MOD));

  // *NOTE*: without the "+-1" small corners within the viewport are not drawn
  int diff = top_right.first - top_right.second - 1;
  int sum  = top_right.first + top_right.second + 1;

  // *NOTE*: draw (visible !) tiles order according to the painter's algorithm
  // --> "back-to-front"

  // lock engine
//  myEngine->lock();

  RPG_Engine_EntityID_t active_entity_id = myEngine->getActive(false);
  RPG_Map_Position_t active_position =
      std::make_pair(std::numeric_limits<unsigned int>::max(),
                     std::numeric_limits<unsigned int>::max());
  RPG_Map_Positions_t visible_positions;
  if (active_entity_id)
  {
    active_position = myEngine->getPosition(active_entity_id,
                                            false);
    myEngine->getVisiblePositions(active_entity_id,
                                  visible_positions,
                                  false);
  } // end IF

  // pass 1:
  //   1. off-map & unmapped areas
  //   2. floor
  //   3. floor edges
  //   [4. cursor highlight]
  //
  // pass 2:
  //   1. north & west walls
  //   2. furniture
  //   3. traps
  //   4. objects
  //   5. creatures
  //   6. effects
  //   7. south & east walls
  //   8. ceiling
  //
  // pass 3:
  //   1. blend (currently) visible tiles ("light-source")
  //   2. blend (previously) seen tiles ("memory")

  SDL_Rect dirty_region = {0, 0, 0, 0};
  SDL_Rect window_area;
  getArea(window_area, true);

  int i, j;
  RPG_Client_SignedPosition_t current_map_position =
      std::make_pair(std::numeric_limits<int>::max(),
                     std::numeric_limits<int>::max());
  RPG_Graphics_FloorTilesConstIterator_t floor_iterator =
      myCurrentFloorSet.tiles.begin();
  RPG_Graphics_FloorTilesConstIterator_t begin_row =
      myCurrentFloorSet.tiles.begin();
  //unsigned int floor_column_index = 0;
  RPG_Graphics_Position_t screen_position =
      std::make_pair(std::numeric_limits<unsigned int>::max(),
                     std::numeric_limits<unsigned int>::max());
  RPG_Map_Size_t map_size = myEngine->getSize(false);
  RPG_Map_Element current_element = MAPELEMENT_INVALID;
  bool is_visible, has_been_seen;
  RPG_Client_BlendingMaskCacheIterator_t blendmask_iterator =
      myLightingCache.end();
  RPG_Graphics_FloorEdgeTileMapIterator_t floor_edge_iterator =
      myFloorEdgeTiles.end();

  //SDL_Rect rect = {0, 0, 0, 0};
  std::ostringstream converter;
  std::string tile_text;
  RPG_Graphics_TextSize_t tile_text_size;
//   RPG_Map_Position_t map_position = std::make_pair(std::numeric_limits<unsigned int>::max(),
//                                                    std::numeric_limits<unsigned int>::max());

  // pass 1
//  myClient->lock();
  for (i = -static_cast<int>(top_right.second);
       i <= static_cast<int>(top_right.second);
       i++)
  {
    current_map_position.second = myView.second + i;
    // step0: off the map ? --> continue
    if ((current_map_position.second < 0) ||
        (current_map_position.second >= static_cast<int>(map_size.second)))
      continue;

    // step1: floor tile rotation
    begin_row = myCurrentFloorSet.tiles.begin();
    //std::advance(begin_row, (current_map_position.second % myCurrentFloorSet.rows) * myCurrentFloorSet.columns);
    std::advance(begin_row,
                 (current_map_position.second %
                  (myCurrentFloorSet.tiles.size() / myCurrentFloorSet.columns)));

    for (j = diff + i;
         (j + i) <= sum;
         j++)
    {
      current_map_position.first = myView.first + j;

      // step0: off the map ? --> continue
      if ((current_map_position.first < 0) ||
          (current_map_position.first >= static_cast<int>(map_size.first)))
        continue;

      is_visible =
          (visible_positions.find(current_map_position) != visible_positions.end());
      has_been_seen =
        (active_entity_id ? myEngine->hasSeen (active_entity_id,
                                               current_map_position,
                                               false)
                          : false);
      current_element = myEngine->getElement(current_map_position,
                                             false);
      ACE_ASSERT(current_element != MAPELEMENT_INVALID);

      // map --> screen coordinates
//       x = (targetSurface->w / 2) + (RPG_GRAPHICS_TILE_WIDTH_MOD * (j - i));
//       y = (targetSurface->h / 2) + (RPG_GRAPHICS_TILE_HEIGHT_MOD * (j + i));
      screen_position =
          RPG_Graphics_Common_Tools::mapToScreen (current_map_position,
                                                  std::make_pair (window_area.w,
                                                                  window_area.h),
                                                  myView);

      // step1: unmapped areas
      if ((current_element == MAPELEMENT_UNMAPPED) ||
          // *NOTE*: walls are drawn together with the floor...
          (current_element == MAPELEMENT_WALL))
      {
        RPG_Graphics_Surface::put(screen_position,
                                  ((is_visible ||
                                    has_been_seen) ? *myOffMapTile
                                                   : *myInvisibleTile),
                                  target_surface,
                                  dirty_region);

        // advance floor iterator
        //std::advance(floor_iterator, myCurrentFloorSet.rows);

        continue;
      } // end IF

      // step2: floor
      // floor tile rotation
      floor_iterator = begin_row;
      //std::advance(floor_iterator, current_map_position.first % myCurrentFloorSet.columns);
      std::advance(floor_iterator,
                   (myCurrentFloorSet.rows *
                    (current_map_position.first % myCurrentFloorSet.columns)));

      if ((current_element == MAPELEMENT_FLOOR) ||
          (current_element == MAPELEMENT_DOOR))
      {
        // blend tile ? 
        if (is_visible)
        {
          if ((static_cast<unsigned int>(current_map_position.first) !=
               active_position.first)                                    ||
              (static_cast<unsigned int>(current_map_position.second) !=
               active_position.second))
          {
            // step0: find blend mask
            blendmask_iterator = myLightingCache.begin();
            std::advance(blendmask_iterator,
                          RPG_Map_Common_Tools::distanceMax(active_position,
                                                            current_map_position) - 1);

            // step1: get background
            RPG_Graphics_Surface::copy(*(*floor_iterator).surface,
                                        *myVisionTempTile);

            // step2: blend tiles
            if (SDL_BlitSurface(*blendmask_iterator, // source
                                NULL,                // aspect (--> everything)
                                myVisionTempTile,    // target
                                &dirty_region))      // aspect
              ACE_DEBUG((LM_ERROR,
                         ACE_TEXT("failed to SDL_BlitSurface(): %s, continuing\n"),
                         ACE_TEXT(SDL_GetError())));
          } // end IF
        } // end IF
        else if (has_been_seen)
        {
          // step1: get background
          RPG_Graphics_Surface::copy(*(*floor_iterator).surface,
                                      *myVisionTempTile);

          // step2: blend tiles
          if (SDL_BlitSurface(myVisionBlendTile, // source
                              NULL,              // aspect (--> everything)
                              myVisionTempTile,  // target
                              &dirty_region))    // aspect
            ACE_DEBUG((LM_ERROR,
                       ACE_TEXT("failed to SDL_BlitSurface(): %s, continuing\n"),
                       ACE_TEXT(SDL_GetError())));
        } // end IF
        RPG_Graphics_Surface::put(screen_position,
                                  (is_visible ? (((static_cast<unsigned int>(current_map_position.first)  == active_position.first) &&
                                                  (static_cast<unsigned int>(current_map_position.second) == active_position.second)) ? *(*floor_iterator).surface
                                                                                                                                      : *myVisionTempTile)
                                              : (has_been_seen ? *myVisionTempTile
                                                               : *myInvisibleTile)),
                                  target_surface,
                                  dirty_region);

#ifdef _DEBUG
        if (myShowCoordinates)
        {
          SDL_Rect rect;
          rect.x = screen_position.first;
          rect.y = screen_position.second;
          rect.w = (*floor_iterator).surface->w;
          rect.h = (*floor_iterator).surface->h;
          //RPG_Graphics_Surface::putRect(rect,                              // rectangle
          //                              RPG_GRAPHICS_WINDOW_HOTSPOT_COLOR, // color
          //                              targetSurface);                    // target surface
          converter.str(ACE_TEXT(""));
          converter.clear();
          tile_text = ACE_TEXT_ALWAYS_CHAR("[");
          converter << current_map_position.first;
          tile_text += converter.str();
          tile_text += ACE_TEXT_ALWAYS_CHAR(",");
          converter.str(ACE_TEXT(""));
          converter.clear();
          converter << current_map_position.second;
          tile_text += converter.str();
          tile_text += ACE_TEXT_ALWAYS_CHAR("]");
          tile_text_size = RPG_Graphics_Common_Tools::textSize(FONT_MAIN_SMALL,
                                                               tile_text);
          RPG_Graphics_Surface::putText(FONT_MAIN_SMALL,
                                        tile_text,
                                        RPG_Graphics_SDL_Tools::colorToSDLColor(RPG_GRAPHICS_FONT_DEF_COLOR,
                                                                                *target_surface),
                                        true, // add shade
                                        RPG_Graphics_SDL_Tools::colorToSDLColor(RPG_GRAPHICS_FONT_DEF_SHADECOLOR,
                                                                                *target_surface),
                                        std::make_pair((rect.x + ((rect.w - tile_text_size.first) / 2)),
                                                       (rect.y + ((rect.h - tile_text_size.second) / 2))),
                                        target_surface,
                                        dirty_region);
        } // end IF
#endif

        // step3: floor edges
        floor_edge_iterator = myFloorEdgeTiles.find(current_map_position);
        if ((floor_edge_iterator != myFloorEdgeTiles.end()) &&
            is_visible)
        {
          // straight edges
          if ((*floor_edge_iterator).second.west.surface)
            RPG_Graphics_Surface::put(std::make_pair((screen_position.first +
                                                      (*floor_edge_iterator).second.west.offset_x),
                                                     (screen_position.second -
                                                      (*floor_edge_iterator).second.west.surface->h +
                                                      (RPG_GRAPHICS_TILE_FLOOR_HEIGHT / 2) +
                                                      (*floor_edge_iterator).second.west.offset_y)),
                                      *(myCurrentFloorEdgeSet.west.surface),
                                      target_surface,
                                      dirty_region);
          if ((*floor_edge_iterator).second.north.surface)
            RPG_Graphics_Surface::put(std::make_pair((screen_position.first +
                                                      (RPG_GRAPHICS_TILE_FLOOR_WIDTH / 2) +
                                                      (*floor_edge_iterator).second.north.offset_x),
                                                     (screen_position.second -
                                                      (*floor_edge_iterator).second.north.surface->h +
                                                      (RPG_GRAPHICS_TILE_FLOOR_HEIGHT / 2) +
                                                      (*floor_edge_iterator).second.north.offset_y)),
                                      *(myCurrentFloorEdgeSet.north.surface),
                                      target_surface,
                                      dirty_region);
          if ((*floor_edge_iterator).second.east.surface)
            RPG_Graphics_Surface::put(std::make_pair((screen_position.first +
                                                      (RPG_GRAPHICS_TILE_FLOOR_WIDTH / 2) +
                                                      (*floor_edge_iterator).second.east.offset_x),
                                                     (screen_position.second -
                                                      (*floor_edge_iterator).second.east.surface->h +
                                                      (RPG_GRAPHICS_TILE_FLOOR_HEIGHT / 2) +
                                                      (*floor_edge_iterator).second.east.offset_y)),
                                      *(myCurrentFloorEdgeSet.east.surface),
                                      target_surface,
                                      dirty_region);
          if ((*floor_edge_iterator).second.south.surface)
            RPG_Graphics_Surface::put(std::make_pair((screen_position.first +
                                                      (*floor_edge_iterator).second.south.offset_x),
                                                     (screen_position.second -
                                                      (*floor_edge_iterator).second.south.surface->h +
                                                      (RPG_GRAPHICS_TILE_FLOOR_HEIGHT / 2) +
                                                      (*floor_edge_iterator).second.south.offset_y)),
                                      *(myCurrentFloorEdgeSet.south.surface),
                                      target_surface,
                                      dirty_region);
          // corners
          if ((*floor_edge_iterator).second.south_west.surface)
            RPG_Graphics_Surface::put(std::make_pair((screen_position.first +
                                                      (*floor_edge_iterator).second.south_west.offset_x),
                                                     (screen_position.second -
                                                      (*floor_edge_iterator).second.south_west.surface->h +
                                                      (RPG_GRAPHICS_TILE_FLOOR_HEIGHT / 2) +
                                                      (*floor_edge_iterator).second.south_west.offset_y)),
                                      *(myCurrentFloorEdgeSet.south_west.surface),
                                      target_surface,
                                      dirty_region);
          if ((*floor_edge_iterator).second.north_west.surface)
            RPG_Graphics_Surface::put(std::make_pair((screen_position.first +
                                                      (RPG_GRAPHICS_TILE_FLOOR_WIDTH / 2) +
                                                      (*floor_edge_iterator).second.north_west.offset_x),
                                                     (screen_position.second -
                                                      (*floor_edge_iterator).second.north_west.surface->h +
                                                      (RPG_GRAPHICS_TILE_FLOOR_HEIGHT / 2) +
                                                      (*floor_edge_iterator).second.north_west.offset_y)),
                                      *(myCurrentFloorEdgeSet.north_west.surface),
                                      target_surface,
                                      dirty_region);
          if ((*floor_edge_iterator).second.south_east.surface)
            RPG_Graphics_Surface::put(std::make_pair((screen_position.first +
                                                      (RPG_GRAPHICS_TILE_FLOOR_WIDTH / 2) +
                                                      (*floor_edge_iterator).second.south_east.offset_x),
                                                     (screen_position.second -
                                                      (*floor_edge_iterator).second.south_east.surface->h +
                                                      (RPG_GRAPHICS_TILE_FLOOR_HEIGHT / 2) +
                                                      (*floor_edge_iterator).second.south_east.offset_y)),
                                      *(myCurrentFloorEdgeSet.south_east.surface),
                                      target_surface,
                                      dirty_region);
          if ((*floor_edge_iterator).second.north_east.surface)
            RPG_Graphics_Surface::put(std::make_pair((screen_position.first +
                                                      (RPG_GRAPHICS_TILE_FLOOR_WIDTH / 2) +
                                                      (*floor_edge_iterator).second.north_east.offset_x),
                                                     (screen_position.second -
                                                      (*floor_edge_iterator).second.north_east.surface->h +
                                                      (RPG_GRAPHICS_TILE_FLOOR_HEIGHT / 2) +
                                                      (*floor_edge_iterator).second.north_east.offset_y)),
                                      *(myCurrentFloorEdgeSet.north_east.surface),
                                      target_surface,
                                      dirty_region);
          // (square) corners
          if ((*floor_edge_iterator).second.top.surface)
            RPG_Graphics_Surface::put(std::make_pair((screen_position.first +
                                                      (*floor_edge_iterator).second.top.offset_x),
                                                     (screen_position.second -
                                                      (*floor_edge_iterator).second.top.surface->h +
                                                      (RPG_GRAPHICS_TILE_FLOOR_HEIGHT / 2) +
                                                      (*floor_edge_iterator).second.top.offset_y)),
                                      *(myCurrentFloorEdgeSet.top.surface),
                                      target_surface,
                                      dirty_region);
          if ((*floor_edge_iterator).second.right.surface)
            RPG_Graphics_Surface::put(std::make_pair((screen_position.first +
                                                      (RPG_GRAPHICS_TILE_FLOOR_WIDTH / 2) +
                                                      (*floor_edge_iterator).second.right.offset_x),
                                                     (screen_position.second -
                                                      (*floor_edge_iterator).second.right.surface->h +
                                                      (RPG_GRAPHICS_TILE_FLOOR_HEIGHT / 2) +
                                                      (*floor_edge_iterator).second.right.offset_y)),
                                      *(myCurrentFloorEdgeSet.right.surface),
                                      target_surface,
                                      dirty_region);
          if ((*floor_edge_iterator).second.left.surface)
            RPG_Graphics_Surface::put(std::make_pair((screen_position.first +
                                                      (*floor_edge_iterator).second.left.offset_x),
                                                     (screen_position.second -
                                                      (*floor_edge_iterator).second.left.surface->h +
                                                      (RPG_GRAPHICS_TILE_FLOOR_HEIGHT / 2) +
                                                      (*floor_edge_iterator).second.left.offset_y)),
                                      *(myCurrentFloorEdgeSet.left.surface),
                                      target_surface,
                                      dirty_region);
          if ((*floor_edge_iterator).second.bottom.surface)
            RPG_Graphics_Surface::put(std::make_pair((screen_position.first +
                                                      (RPG_GRAPHICS_TILE_FLOOR_WIDTH / 2) +
                                                      (*floor_edge_iterator).second.bottom.offset_x),
                                                     (screen_position.second -
                                                      (*floor_edge_iterator).second.bottom.surface->h +
                                                      (RPG_GRAPHICS_TILE_FLOOR_HEIGHT / 2) +
                                                      (*floor_edge_iterator).second.bottom.offset_y)),
                                      *(myCurrentFloorEdgeSet.bottom.surface),
                                      target_surface,
                                      dirty_region);
        } // end IF
      } // end IF

      // advance floor iterator
      //std::advance(floor_iterator, myCurrentFloorSet.rows);
    } // end FOR
  } // end FOR

  // pass 2
  RPG_Graphics_WallTileMapIterator_t wall_iterator = myWallTiles.end();
  RPG_Graphics_DoorTileMapIterator_t door_iterator = myDoorTiles.end();
  RPG_Engine_EntityID_t entity_id = 0;
  for (i = -static_cast<int>(top_right.second);
       i <= static_cast<int>(top_right.second);
       i++)
  {
    current_map_position.second = myView.second + i;

    // step0: off the map ? --> continue
    if ((current_map_position.second < 0) ||
        (current_map_position.second >= static_cast<int>(map_size.second)))
      continue;

    for (j = diff + i;
          (j + i) <= sum;
          j++)
    {
      current_map_position.first = myView.first + j;

      // step0: off the map ? --> continue
      if ((current_map_position.first < 0) ||
          (current_map_position.first >= static_cast<int>(map_size.first)))
        continue;

      is_visible =
          (visible_positions.find(current_map_position) != visible_positions.end());

      // transform map coordinates into screen coordinates
//       x = (targetSurface->w / 2) + (RPG_GRAPHICS_TILE_WIDTH_MOD * (j - i));
//       y = (targetSurface->h / 2) + (RPG_GRAPHICS_TILE_HEIGHT_MOD * (j + i));
      screen_position =
          RPG_Graphics_Common_Tools::mapToScreen (current_map_position,
                                                  std::make_pair (window_area.w,
                                                                  window_area.h),
                                                  myView);

      wall_iterator = myWallTiles.find(current_map_position);
      door_iterator = myDoorTiles.find(current_map_position);

      // step1: walls (west & north)
      if ((wall_iterator != myWallTiles.end()) &&
          is_visible)
      {
        if ((*wall_iterator).second.west.surface)
          RPG_Graphics_Surface::put(std::make_pair(screen_position.first,
                                                   (screen_position.second -
                                                    (*wall_iterator).second.west.surface->h +
                                                    (RPG_GRAPHICS_TILE_FLOOR_HEIGHT / 2))),
                                    *(myCurrentWallSet.west.surface),
                                    target_surface,
                                    dirty_region);
        if ((*wall_iterator).second.north.surface)
          RPG_Graphics_Surface::put(std::make_pair((screen_position.first +
                                                    (RPG_GRAPHICS_TILE_FLOOR_WIDTH / 2)),
                                                   (screen_position.second -
                                                    (*wall_iterator).second.north.surface->h +
                                                    (RPG_GRAPHICS_TILE_FLOOR_HEIGHT / 2))),
                                    *(myCurrentWallSet.north.surface),
                                    target_surface,
                                    dirty_region);
      } // end IF

      // step2: doors
      if ((door_iterator != myDoorTiles.end()) &&
          is_visible)
      {
        // *NOTE*: doors are drawn in the "middle" of the floor tile
        RPG_Graphics_Surface::put(std::make_pair((screen_position.first +
                                                  (*door_iterator).second.offset_x +
                                                  (RPG_GRAPHICS_TILE_FLOOR_WIDTH / 4)),
                                                 (screen_position.second +
                                                  (*door_iterator).second.offset_y -
                                                  (*door_iterator).second.surface->h +
                                                  (RPG_GRAPHICS_TILE_WALL_HEIGHT / 2) -
                                                  (RPG_GRAPHICS_TILE_FLOOR_HEIGHT / 4))),
                                  *(*door_iterator).second.surface,
                                  target_surface,
                                  dirty_region);
      } // end IF

      // step3: furniture & traps

      // step4: objects

      // step5: creatures
      entity_id = myEngine->hasEntity(current_map_position,
                                      false);
      if (entity_id && is_visible)
      {
        // invalidate bg
        RPG_CLIENT_ENTITY_MANAGER_SINGLETON::instance()->invalidateBG(entity_id);

        // draw creature
        RPG_CLIENT_ENTITY_MANAGER_SINGLETON::instance()->put(entity_id,
                                                             screen_position,
                                                             dirty_region,
                                                             false);
      } // end IF

      // step6: effects

      // step7: walls (south & east)
      if ((wall_iterator != myWallTiles.end()) &&
          is_visible)
      {
        if ((*wall_iterator).second.south.surface)
          RPG_Graphics_Surface::put(std::make_pair(screen_position.first,
                                                   (screen_position.second -
                                                    (*wall_iterator).second.south.surface->h +
                                                    RPG_GRAPHICS_TILE_FLOOR_HEIGHT)),
                                    *(*wall_iterator).second.south.surface,
                                    target_surface,
                                    dirty_region);
        if ((*wall_iterator).second.east.surface)
          RPG_Graphics_Surface::put(std::make_pair((screen_position.first +
                                                    (RPG_GRAPHICS_TILE_FLOOR_WIDTH / 2)),
                                                   (screen_position.second -
                                                    (*wall_iterator).second.east.surface->h +
                                                    RPG_GRAPHICS_TILE_FLOOR_HEIGHT)),
                                    *(*wall_iterator).second.east.surface,
                                    target_surface,
                                    dirty_region);
      } // end IF

      //// step8: ceiling
      //// *TODO*: this is static information: compute once / level and use a lookup-table here...
      //if (RPG_Client_Common_Tools::hasCeiling(current_map_position,
      //                                        *myEngine,
      //                                        false) &&
      //    is_visible)
      //{
      //  RPG_Graphics_Surface::put(screen_position.first,
      //                            (screen_position.second -
      //                              (myCurrentMapStyle.half_height_walls ? (RPG_GRAPHICS_TILE_WALL_HEIGHT / 2)
      //                                                                  : RPG_GRAPHICS_TILE_WALL_HEIGHT) +
      //                              (RPG_GRAPHICS_TILE_FLOOR_HEIGHT / (myCurrentMapStyle.half_height_walls ? 8 : 2))),
      //                            *myCeilingTile,
      //                            targetSurface);
      //} // end IF
    } // end FOR
  } // end FOR

  // pass 3
  // - vision blending
  //if (active_entity_id &&
  //    myDoVisionBlend)
  //{
  //  unsigned int visible_radius = myEngine->getVisibleRadius(active_entity_id);
  //  RPG_Map_Position_t entity_position = myEngine->getPosition(active_entity_id);
  //  RPG_Client_BlendingMaskCacheIterator_t blendmask_iterator = myLightingCache.begin();
  //  for (RPG_Map_PositionsConstIterator_t iterator = visible_positions.begin();
  //       iterator != visible_positions.end();
  //       iterator++)
  //  {
  //    if ((*iterator == entity_position) ||
  //        !myEngine->isValid(*iterator))
  //      continue; // done

  //    // step0: find blend mask
  //    blendmask_iterator = myLightingCache.begin();
  //    std::advance(blendmask_iterator,
  //                 RPG_Map_Common_Tools::distanceMax(entity_position,
  //                                                   *iterator) - 1);

  //    // blend visible tile
  //    // step1: get background
  //    screen_position = RPG_Graphics_Common_Tools::map2Screen(*iterator,
  //                                                            mySize,
  //                                                            myView);
  //    RPG_Graphics_Surface::get(screen_position.first,
  //                              screen_position.second,
  //                              true, // use (fast) blitting method
  //                              *targetSurface,
  //                              *myVisionTempTile);

  //    // step2: merge tiles
  //    if (SDL_BlitSurface(*blendmask_iterator, // source
  //                        NULL,                // aspect (--> everything)
  //                        myVisionTempTile,    // target
  //                        &dirty_region))      // aspect
  //    {
  //      ACE_DEBUG((LM_ERROR,
  //                 ACE_TEXT("failed to SDL_BlitSurface(): %s, continuing"),
  //                 SDL_GetError()));
  //    } // end IF

  //    // step3: write result
  //    RPG_Graphics_Surface::put(screen_position.first,
  //                              screen_position.second,
  //                              *myVisionTempTile,
  //                              target_surface);
  //  } // end FOR
  //} // end IF
  myClient->unlock();

  // unlock engine
//  myEngine->unlock();

  // realize any children
  for (RPG_Graphics_WindowsIterator_t iterator = children_.begin();
       iterator != children_.end();
       iterator++)
  {
    if ((((*iterator)->getType() == WINDOW_MINIMAP) && !myDrawMinimap) ||
        (((*iterator)->getType() == WINDOW_MESSAGE) && !myShowMessages))
      continue;

    try
    {
      (*iterator)->draw(target_surface,
                        offsetX_in,
                        offsetY_in);
    }
    catch (...)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("caught exception in RPG_Graphics_IWindow::draw(), continuing\n")));
    }
  } // end FOR

  // whole viewport needs a refresh...
  SDL_GetClipRect(target_surface, &dirty_region);
  invalidate(dirty_region);

  // reset clipping area
  unclip(target_surface);

  // remember position of last realization
  lastAbsolutePosition_ = std::make_pair (inherited::clipRectangle_.x,
                                          inherited::clipRectangle_.y);
}

void
RPG_Client_Window_Level::handleEvent(const SDL_Event& event_in,
                                     RPG_Graphics_IWindowBase* window_in,
                                     SDL_Rect& dirtyRegion_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Window_Level::handleEvent"));

  // init return value(s)
  ACE_OS::memset(&dirtyRegion_out, 0, sizeof(dirtyRegion_out));

  // sanity check(s)
  ACE_ASSERT(myClient);
  ACE_ASSERT(myEngine);

  bool delegate_to_parent = false;
  switch (event_in.type)
  {
    // *** keyboard ***
    case SDL_KEYDOWN:
    {
//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("%s key\n%s\n"),
//                  ((event_in.type == SDL_KEYDOWN) ? ACE_TEXT("pressed") : ACE_TEXT("released")),
//                  ACE_TEXT(RPG_Graphics_SDL_Tools::keyToString(event_in.key.keysym).c_str())));

      switch (event_in.key.keysym.sym)
      {
        case SDLK_a:
        {
          myClient->centerOnActive(!myClient->getCenterOnActive());

          // adjust view ?
//          myEngine->lock();
          RPG_Engine_EntityID_t entity_id = myEngine->getActive(false);
          if (entity_id &&
              myClient->getCenterOnActive())
          {
            myClientAction.command = COMMAND_SET_VIEW;
            myClientAction.position = myEngine->getPosition(entity_id, false);
            myClient->action(myClientAction);
          } // end IF
//          myEngine->unlock();

          break;
        }
        // implement keypad navigation
        case SDLK_c:
        {
          // step1: set view
          myClientAction.command = COMMAND_SET_VIEW;

//          myEngine->lock();
          myClientAction.entity_id = myEngine->getActive(false);
          if ((myClientAction.entity_id == 0)        ||
              (event_in.key.keysym.mod & KMOD_SHIFT))
          {
            // center view
            myClientAction.position = myEngine->getSize(false);
            myClientAction.position.first  >>= 1;
            myClientAction.position.second >>= 1;
          } // end IF
          else
          {
            myClientAction.position =
                myEngine->getPosition(myClientAction.entity_id, false);
          }
//          myEngine->unlock();

          myClient->action(myClientAction);

          break;
        }
        case SDLK_m:
        {
          toggleMiniMap();

          break;
        }
        case SDLK_r:
        {
          if (event_in.key.keysym.mod & KMOD_SHIFT)
          {
            // (manual) refresh
            myClientAction.command = COMMAND_WINDOW_DRAW;
            myClientAction.window = this;
            myClient->action(myClientAction);
          } // end IF
          else
          {
//            myEngine->lock();
            RPG_Engine_EntityID_t entity_id = myEngine->getActive(false);
            if (entity_id)
            {
              struct RPG_Engine_Action player_action;
              player_action.command = COMMAND_RUN;
              player_action.position =
                  std::make_pair(std::numeric_limits<unsigned int>::max(),
                                 std::numeric_limits<unsigned int>::max());
              //player_action.path.clear();
              player_action.target = 0;

              myEngine->action(entity_id,
                               player_action,
                               false);
            } // end IF
//            myEngine->unlock();
          } // end ELSE

          break;
        }
        case SDLK_s:
        {
          if (event_in.key.keysym.mod & KMOD_SHIFT)
          {
#ifdef _DEBUG
            toggleShowCoordinates();

            // --> need a redraw
            myClientAction.command = COMMAND_WINDOW_DRAW;
            myClientAction.window = this;
            myClient->action(myClientAction);
#endif
          } // end IF
          else
          {
//            myEngine->lock();
            RPG_Engine_EntityID_t entity_id = myEngine->getActive(false);
            if (entity_id)
            {
              struct RPG_Engine_Action player_action;
              player_action.command = COMMAND_SEARCH;
              player_action.position =
                  std::make_pair(std::numeric_limits<unsigned int>::max(),
                                 std::numeric_limits<unsigned int>::max());
              //player_action.path.clear();
              player_action.target = 0;

              myEngine->action(entity_id,
                               player_action,
                               false);
            } // end IF
//            myEngine->unlock();
          } // end ELSE

          break;
        }
        case SDLK_t:
        case SDLK_x:
        {
          myClientAction.command = COMMAND_TILE_HIGHLIGHT_INVALIDATE_BG;
          myClient->action(myClientAction);

          // find pointed-to map square
          RPG_Graphics_Position_t cursor_position =
              RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->position();
          RPG_Graphics_Position_t curent_view = getView();
          SDL_Rect window_area;
          getArea(window_area, true);
//          myEngine->lock();
          myClientAction.position =
            RPG_Graphics_Common_Tools::screenToMap (cursor_position,
                                                    myEngine->getSize(false),
                                                    std::make_pair (window_area.w,
                                                                    window_area.h),
                                                    curent_view);
          bool push_mousemove_event = myEngine->isValid(myClientAction.position,
                                                        false);

          myClientAction.entity_id = myEngine->getActive(false);
          // toggle path selection mode
          switch (myClient->mode())
          {
            case SELECTIONMODE_AIM_CIRCLE:
            case SELECTIONMODE_AIM_SQUARE:
            {
              // --> switch off aim selection

              // clear cached positions
              myClientAction.source =
                  std::make_pair(std::numeric_limits<unsigned int>::max(),
                                 std::numeric_limits<unsigned int>::max());
              myClientAction.positions.clear();

              myClient->mode(SELECTIONMODE_NORMAL);
              push_mousemove_event = false;

              if (event_in.key.keysym.sym != SDLK_t)
                break;

              // *WARNING*: falls through !
            }
            case SELECTIONMODE_PATH:
            {
              // clear any cached positions
              myClientAction.path.clear();

              // *WARNING*: falls through !
            }
            case SELECTIONMODE_NORMAL:
            {
              if (myClientAction.entity_id == 0)
                push_mousemove_event = false;

              if (event_in.key.keysym.sym == SDLK_t)
                myClient->mode(SELECTIONMODE_PATH);	// --> switch on path selection
              else
              {
                // --> switch on aim selection

                // retain source position
                myClientAction.source = myClientAction.position;

                // initial radius == 0
                myClientAction.positions.insert(myClientAction.position);

                myClient->mode(((event_in.key.keysym.mod & KMOD_SHIFT) ? SELECTIONMODE_AIM_SQUARE
                                                                       : SELECTIONMODE_AIM_CIRCLE));
              } // end ELSE

              break;
            }
            default:
            {
              ACE_DEBUG((LM_ERROR,
                         ACE_TEXT("invalid selection mode (was: %u), continuing\n"),
                         myClient->mode()));

              break;
            }
          } // end SWITCH

          // on the map ?
          if (push_mousemove_event)
          {
            // redraw highlight / cursor --> push fake mouse-move event
            SDL_Event sdl_event;
            sdl_event.type = SDL_MOUSEMOTION;
            sdl_event.motion.x = cursor_position.first;
            sdl_event.motion.y = cursor_position.second;
            if (SDL_PushEvent(&sdl_event))
              ACE_DEBUG((LM_ERROR,
                         ACE_TEXT("failed to SDL_PushEvent(): \"%s\", continuing\n"),
                         ACE_TEXT(SDL_GetError())));

//            // step2: draw tile highlight
//            myClientAction.command = COMMAND_TILE_HIGHLIGHT_DRAW;
//            myClient->action(myClientAction);
//            // *NOTE*: this MAY also invalidate the current cursor bg...
//            myClientAction.command = COMMAND_CURSOR_INVALIDATE_BG;
//            myClient->action(myClientAction);
          } // end IF
//          // step3: set/draw an appropriate cursor
//          RPG_Graphics_Cursor cursor_type =
//              RPG_Client_Common_Tools::getCursor(myClientAction.position,
//                                                 myClientAction.entity_id,
//                                                 myClient->hasSeen(myClientAction.entity_id,
//                                                                   myClientAction.position,
//                                                                   true),
//                                                 myClient->mode(),
//                                                 *myEngine,
//                                                 false);
//          myEngine->unlock();
//          if (cursor_type !=
//              RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->type())
//          {
//            myClientAction.command = COMMAND_CURSOR_SET;
//            myClientAction.cursor = cursor_type;
//            myClient->action(myClientAction);
//          } // end IF
//          myClientAction.command = COMMAND_CURSOR_DRAW;
//          myClientAction.position = cursor_position;
//          myClient->action(myClientAction);

          break;
        }
        // implement keypad navigation
        case SDLK_UP:
        case SDLK_DOWN:
        case SDLK_LEFT:
        case SDLK_RIGHT:
        {
          RPG_Map_Direction direction = RPG_MAP_DIRECTION_INVALID;
          myClientAction.position = getView();
          switch (event_in.key.keysym.sym)
          {
//             case SDLK_UP:
//               mapWindow->setView(0,
//                                  -RPG_GRAPHICS_WINDOW_SCROLL_OFFSET); break;
//             case SDLK_DOWN:
//               mapWindow->setView(0,
//                                  RPG_GRAPHICS_WINDOW_SCROLL_OFFSET); break;
//             case SDLK_LEFT:
//               mapWindow->setView(-RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
//                                  0); break;
//             case SDLK_RIGHT:
//               mapWindow->setView(RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
//                                  0); break;
            case SDLK_UP:
            {
              if (event_in.key.keysym.mod & KMOD_SHIFT)
              {
                myClientAction.position.first--;
                myClientAction.position.second--;
                //setView(-RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
                //        -RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);
              } // end IF
              else
                direction = DIRECTION_UP;

              break;
            }
            case SDLK_DOWN:
            {
              if (event_in.key.keysym.mod & KMOD_SHIFT)
              {
                myClientAction.position.first++;
                myClientAction.position.second++;
                //setView(RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
                //        RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);
              } // end IF
              else
                direction = DIRECTION_DOWN;

              break;
            }
            case SDLK_LEFT:
            {
              if (event_in.key.keysym.mod & KMOD_SHIFT)
              {
                myClientAction.position.first--;
                myClientAction.position.second++;
                //setView(-RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
                //        RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);
              } // end IF
              else
                direction = DIRECTION_LEFT;

              break;
            }
            case SDLK_RIGHT:
            {
              if (event_in.key.keysym.mod & KMOD_SHIFT)
              {
                myClientAction.position.first++;
                myClientAction.position.second--;
                //setView(RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
                //        -RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);
              } // end IF
              else
                direction = DIRECTION_RIGHT;

              break;
            }
            default:
            {
              ACE_DEBUG((LM_ERROR,
                         ACE_TEXT("invalid key (was: %u), aborting\n"),
                         event_in.key.keysym.sym));

              break;
            }
          } // end SWITCH

          if (!(event_in.key.keysym.mod & KMOD_SHIFT))
          {
//            myEngine->lock();
            myClientAction.entity_id = myEngine->getActive(false);
            if (myClientAction.entity_id == 0)
            {
              // clean up
//              myEngine->unlock();

              break; // nothing to do...
            } // end IF

            struct RPG_Engine_Action player_action;
            player_action.command = COMMAND_TRAVEL;
            // compute target position
            myClientAction.position =
                myEngine->getPosition(myClientAction.entity_id, false);
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
                ACE_DEBUG((LM_ERROR,
                           ACE_TEXT("invalid direction (was: \"%s\"), aborting\n"),
                           ACE_TEXT(RPG_Map_DirectionHelper::RPG_Map_DirectionToString(direction).c_str())));
                break;
              }
            } // end SWITCH
            player_action.path.clear();
            player_action.target = 0;

            if (myEngine->isValid(player_action.position, false))
            {
              myEngine->action(myClientAction.entity_id,
                               player_action,
                               false);

//              // adjust view ?
//              if (myClient->getCenterOnActive())
//              {
//                myClientAction.command = COMMAND_SET_VIEW;
//                myClientAction.position = player_action.position;
//                myClient->action(myClientAction);
//              } // end IF
            } // end IF
//            myEngine->unlock();
          } // end IF
          else
          {
            myClientAction.command = COMMAND_SET_VIEW;
            myClient->action(myClientAction);
          } // end ELSE

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
    case SDL_MOUSEMOTION:
    {
      // find map square
      RPG_Graphics_Position_t curent_view = getView();
//      myEngine->lock();
      myClientAction.position =
        RPG_Graphics_Common_Tools::screenToMap (std::make_pair (event_in.motion.x,
                                                                event_in.motion.y),
                                                myEngine->getSize(false),
                                                std::make_pair (inherited::clipRectangle_.w,
                                                                inherited::clipRectangle_.h),
                                                curent_view);
//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("mouse position [%u,%u] --> [%u,%u]\n"),
//                  event_in.button.x,
//                  event_in.button.y,
//                  myClientAction.position.first,
//                  myClientAction.position.second));

      myClientAction.entity_id = myEngine->getActive (false);
      bool is_valid = myEngine->isValid (myClientAction.position, false);
//      myEngine->unlock();
      bool has_seen =
        (myClientAction.entity_id ? myEngine->hasSeen (myClientAction.entity_id,
                                                       myClientAction.position,
                                                       true)
                                  : false);
//      myEngine->lock();

      // change "active" tile ?
      if (myClientAction.position != myClientAction.previous)
      {
        // step1: restore/clear old tile highlight background
        myClientAction.command = COMMAND_TILE_HIGHLIGHT_RESTORE_BG;
        myClient->action(myClientAction);

        // step2: invalidate tile highlight background
        myClientAction.command = COMMAND_TILE_HIGHLIGHT_INVALIDATE_BG;
        myClient->action(myClientAction);
        // *NOTE*: this MAY also invalidate the current cursor bg. As the cursor
        // gets redrawn anyway, (safely) assume it to be so...
        myClientAction.command = COMMAND_CURSOR_INVALIDATE_BG;
        myClient->action(myClientAction);

        RPG_Client_SelectionMode current_mode = myClient->mode();
        switch (current_mode)
        {
          case SELECTIONMODE_AIM_CIRCLE:
          {
            unsigned int selection_radius =
                RPG_Map_Common_Tools::distanceMax(myClientAction.source,
                                                  myClientAction.position);
            if (selection_radius > RPG_MAP_CIRCLE_MAX_RADIUS)
              selection_radius = RPG_MAP_CIRCLE_MAX_RADIUS;

            RPG_Map_Common_Tools::buildCircle(myClientAction.source,
                                              myEngine->getSize(false),
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
                  RPG_Map_Common_Tools::distanceMax(myClientAction.source,
                                                    myClientAction.position);
              RPG_Map_Common_Tools::buildSquare(myClientAction.source,
                                                myEngine->getSize(false),
                                                selection_radius,
                                                false, // don't fill
                                                myClientAction.positions);
            } // end IF

            // step2: remove invalid positions
            RPG_Map_Positions_t obstacles = myEngine->getObstacles(false,
                                                                   false);
            // *WARNING*: this works for associative containers ONLY
            for (RPG_Map_PositionsIterator_t iterator = myClientAction.positions.begin();
                 iterator != myClientAction.positions.end();
                 )
              if (RPG_Map_Common_Tools::hasLineOfSight(myClientAction.source,
                                                       *iterator,
                                                       obstacles,
                                                       false))
                iterator++;
              else
                myClientAction.positions.erase(iterator++);

            break;
          }
          case SELECTIONMODE_NORMAL:
            break;
          case SELECTIONMODE_PATH:
          {
            if (myClientAction.entity_id &&
                is_valid                 &&
                has_seen)
            {
              RPG_Map_Position_t current_position =
                  myEngine->getPosition(myClientAction.entity_id, false);
              if (current_position != myClientAction.position)
              {
                if (!myEngine->findPath(current_position,
                                        myClientAction.position,
                                        myClientAction.path,
                                        false))
                {
                  //ACE_DEBUG((LM_DEBUG,
                  //           ACE_TEXT("could not find a path [%u,%u] --> [%u,%u], aborting\n"),
                  //           current_position.first,
                  //           current_position.second,
                  //           myClientAction.position.first,
                  //           myClientAction.position.second));

                  // pointing at an invalid (==unreachable) position (still on the map)
                  // --> erase cached path (and tile highlights)
                  myClientAction.path.clear();
                } // end IF
              } // end IF
            } // end IF
            else
            {
              // pointing at an invalid (==unreachable) position (still on the map)
              // --> erase cached path (and tile highlights)
              myClientAction.path.clear();
            } // end ELSE

            break;
          }
          default:
          {
            ACE_DEBUG((LM_ERROR,
                       ACE_TEXT("invalid selection mode (was: %d), aborting\n"),
                       current_mode));

            // clean up
//            myEngine->unlock();

            return;
          }
        } // end SWITCH

        // draw tile highlight(s)
        myClientAction.command = COMMAND_TILE_HIGHLIGHT_DRAW;
        myClient->action(myClientAction);
      } // end IF

      // set an appropriate cursor
      RPG_Graphics_Cursor cursor_type =
          RPG_Client_Common_Tools::getCursor(myClientAction.position,
                                             myClientAction.entity_id,
                                             has_seen,
                                             myClient->mode(),
                                             *myEngine,
                                             false);
//      myEngine->unlock();
      if (cursor_type !=
          RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->type())
      {
        myClientAction.command = COMMAND_CURSOR_SET;
        myClientAction.cursor = cursor_type;
        myClient->action(myClientAction);
      } // end IF

      if (myClientAction.position != myClientAction.previous)
        myClientAction.previous = myClientAction.position;

      break;
    }
    case SDL_MOUSEBUTTONDOWN:
    {
//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("mouse button [%u,%u] pressed\n"),
//                  static_cast<unsigned int>(event_in.button.which),
//                  static_cast<unsigned int>(event_in.button.button)));

      RPG_Graphics_Position_t curent_view = getView();
//      myEngine->lock();
      if (event_in.button.button == 1) // left-click
      {
        RPG_Graphics_Position_t map_position =
            RPG_Graphics_Common_Tools::screenToMap (std::make_pair (event_in.button.x,
                                                                    event_in.button.y),
                                                    myEngine->getSize(false),
                                                    std::make_pair (inherited::clipRectangle_.w,
                                                                    inherited::clipRectangle_.h),
                                                    curent_view);
        //ACE_DEBUG((LM_DEBUG,
        //           ACE_TEXT("mouse position [%u,%u] --> map position [%u,%u]\n"),
        //           event_in.button.x,
        //           event_in.button.y,
        //           map_position.first,
        //           map_position.second));

        myClientAction.entity_id = myEngine->getActive(false);
//        myEngine->unlock();
        bool has_seen =
          (myClientAction.entity_id ? myEngine->hasSeen (myClientAction.entity_id,
                                                         map_position,
                                                         true)
                                    : false);
        if ((myClientAction.entity_id == 0) ||
            !has_seen)
          break; // --> no player/vision, no action...

//				myEngine->lock();
        myClientAction.position =
          myEngine->getPosition(myClientAction.entity_id, false);

        struct RPG_Engine_Action player_action;
        player_action.command = RPG_ENGINE_COMMAND_INVALID;
        player_action.position = map_position;
        //player_action.path.clear();
        player_action.target = myEngine->hasEntity(map_position, false);
        // self ?
        if (player_action.target == myClientAction.entity_id)
        {
          // clean up
//          myEngine->unlock();

          break;
        } // end IF

        // clicked on monster ?
        if (player_action.target                       &&
            myEngine->canSee(myClientAction.entity_id,
                             map_position,
                             false)                    &&
            myEngine->isMonster(player_action.target,
                                false))
        {
          // attack/pursue selected monster
          player_action.command = COMMAND_ATTACK;

          // reuse existing path ?
          if (!myEngine->canReach(myClientAction.entity_id,
                                  map_position,
                                  false)                   &&
              (myClient->mode() == SELECTIONMODE_PATH)     &&
              !myClientAction.path.empty())
          {
            // sanity checks
            ACE_ASSERT(myClientAction.path.front().first == myClientAction.position);
            ACE_ASSERT(myClientAction.path.back().first == player_action.position);

            // path exists --> reuse it
            player_action.path = myClientAction.path;
            player_action.path.pop_front();
            //player_action.path.pop_back();

            myClient->mode(SELECTIONMODE_NORMAL);
            myClientAction.path.clear();
            // restore/clear old tile highlight background
            myClientAction.command = COMMAND_TILE_HIGHLIGHT_RESTORE_BG;
            myClient->action(myClientAction);
            myClientAction.command = COMMAND_TILE_HIGHLIGHT_DRAW;
            myClient->action(myClientAction);
          } // end IF
          myEngine->action(myClientAction.entity_id,
                           player_action,
                           false);

          // clean up
//          myEngine->unlock();

          break;
        } // end IF
        player_action.target = 0;

        // player standing next to door ?
        RPG_Map_Element map_element = myEngine->getElement(map_position, false);
        switch (map_element)
        {
          case MAPELEMENT_DOOR:
          {
            RPG_Map_DoorState door_state = myEngine->state(map_position, false);

            // (try to) handle door ?
            if (RPG_Map_Common_Tools::isAdjacent(myClientAction.position,
                                                 map_position))
            {
              bool ignore_door = false;
              switch (door_state)
              {
                case DOORSTATE_OPEN:
                  player_action.command = COMMAND_DOOR_CLOSE; break;
                case DOORSTATE_CLOSED:
                case DOORSTATE_LOCKED:
                  player_action.command = COMMAND_DOOR_OPEN; break;
                case DOORSTATE_BROKEN:
                  ignore_door = true; break;
                default:
                {
                  ACE_DEBUG((LM_ERROR,
                             ACE_TEXT("[%u,%u]: invalid door state (was: \"%s\"), aborting\n"),
                             map_position.first, map_position.second,
                             ACE_TEXT(RPG_Map_DoorStateHelper::RPG_Map_DoorStateToString(door_state).c_str())));

                  // clean up
//                  myEngine->unlock();

                  return;
                }
              } // end SWITCH
              player_action.position = map_position;

              if (!ignore_door)
                myEngine->action(myClientAction.entity_id,
                                 player_action,
                                 false);
            } // end IF
            else if ((door_state == DOORSTATE_CLOSED) ||
                     (door_state == DOORSTATE_LOCKED))
            {
              // cannot travel there --> done

              // clean up
//              myEngine->unlock();

              break;
            } // end ELSEIF

            // done ?
            if (player_action.command == COMMAND_DOOR_CLOSE)
            {
              // clean up
//              myEngine->unlock();

              break;
            } // end IF

            // *WARNING*: falls through !
          }
          case MAPELEMENT_FLOOR:
          {
            // floor / (open/broken) door --> (try to) travel to this position

            player_action.command = COMMAND_TRAVEL;

            // reuse existing path ?
            if ((myClient->mode() == SELECTIONMODE_PATH) &&
                !myClientAction.path.empty())
            {
              // sanity checks
              ACE_ASSERT(myClientAction.path.front().first == myClientAction.position);
              ACE_ASSERT(myClientAction.path.back().first == player_action.position);

              // path exists --> reuse it
              player_action.path = myClientAction.path;
              player_action.path.pop_front();

              myClient->mode(SELECTIONMODE_NORMAL);
              myClientAction.path.clear();
              // restore/clear old tile highlight background
              myClientAction.command = COMMAND_TILE_HIGHLIGHT_RESTORE_BG;
              myClient->action(myClientAction);
              myClientAction.command = COMMAND_TILE_HIGHLIGHT_DRAW;
              myClient->action(myClientAction);
            } // end IF

            myEngine->action(myClientAction.entity_id,
                             player_action,
                             false);

            break;
          }
          default:
            break;
        } // end SWITCH
      } // end IF
//      myEngine->unlock();

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
      myClientAction.command = COMMAND_TILE_HIGHLIGHT_RESTORE_BG;
      myClient->action(myClientAction);

      myClientAction.command = COMMAND_TILE_HIGHLIGHT_INVALIDATE_BG;
      myClient->action(myClientAction);

      break;
    }
    case SDL_ACTIVEEVENT:
    case SDL_KEYUP:
    case SDL_MOUSEBUTTONUP:
    case SDL_JOYAXISMOTION:
    case SDL_JOYBALLMOTION:
    case SDL_JOYHATMOTION:
    case SDL_JOYBUTTONDOWN:
    case SDL_JOYBUTTONUP:
    case SDL_QUIT:
    case SDL_SYSWMEVENT:
    case SDL_VIDEORESIZE:
    case SDL_VIDEOEXPOSE:
    default:
    {
      // delegate these to the parent...
      delegate_to_parent = true;

      break;
    }
  } // end SWITCH

  if (delegate_to_parent)
    getParent()->handleEvent(event_in,
                             window_in,
                             dirtyRegion_out);
}

bool
RPG_Client_Window_Level::setStyle(const RPG_Graphics_StyleUnion& style_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Window_Level::setStyle"));

  switch (style_in.discriminator)
  {
    case RPG_Graphics_StyleUnion::EDGESTYLE:
    {
      RPG_Graphics_Common_Tools::loadFloorEdgeTileSet(style_in.edgestyle,
                                                      myCurrentFloorEdgeSet);
      // sanity check
      if ((myCurrentFloorEdgeSet.west.surface == NULL) &&
          (myCurrentFloorEdgeSet.north.surface == NULL) &&
          (myCurrentFloorEdgeSet.east.surface == NULL) &&
          (myCurrentFloorEdgeSet.south.surface == NULL) &&
          (myCurrentFloorEdgeSet.south_west.surface == NULL) &&
          (myCurrentFloorEdgeSet.south_east.surface == NULL) &&
          (myCurrentFloorEdgeSet.north_west.surface == NULL) &&
          (myCurrentFloorEdgeSet.north_east.surface == NULL) &&
          (myCurrentFloorEdgeSet.top.surface == NULL) &&
          (myCurrentFloorEdgeSet.right.surface == NULL) &&
          (myCurrentFloorEdgeSet.left.surface == NULL) &&
          (myCurrentFloorEdgeSet.bottom.surface == NULL))
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("edge-style \"%s\" has no tiles, continuing\n"),
                   ACE_TEXT(RPG_Graphics_EdgeStyleHelper::RPG_Graphics_EdgeStyleToString(style_in.edgestyle).c_str())));
      } // end IF

      // update floor edge tiles / position
      RPG_Client_Common_Tools::updateFloorEdges(myCurrentFloorEdgeSet,
                                                myFloorEdgeTiles);

      break;
    }
    case RPG_Graphics_StyleUnion::FLOORSTYLE:
    {
      RPG_Graphics_Common_Tools::loadFloorTileSet(style_in.floorstyle,
                                                  myCurrentFloorSet);
      // sanity check
      if (myCurrentFloorSet.tiles.empty())
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("floor-style \"%s\" has no tiles, continuing\n"),
                   ACE_TEXT(RPG_Graphics_FloorStyleHelper::RPG_Graphics_FloorStyleToString(style_in.floorstyle).c_str())));
      } // end IF

      break;
    }
    case RPG_Graphics_StyleUnion::WALLSTYLE:
    {
      RPG_Graphics_Common_Tools::loadWallTileSet(style_in.wallstyle,
                                                 myClient->getStyle().half_height_walls,
                                                 myCurrentWallSet);
      // sanity check
      if ((myCurrentWallSet.east.surface == NULL) ||
          (myCurrentWallSet.west.surface == NULL) ||
          (myCurrentWallSet.north.surface == NULL) ||
          (myCurrentWallSet.south.surface == NULL))
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("wall-style \"%s\" is incomplete, aborting\n"),
                   ACE_TEXT(RPG_Graphics_WallStyleHelper::RPG_Graphics_WallStyleToString(style_in.wallstyle).c_str())));

        return false;
      } // end IF

      initWallBlend(myClient->getStyle().half_height_walls);

      // *NOTE*: west is just a "darkened" version of east...
      SDL_Surface* copy = NULL;
      copy = RPG_Graphics_Surface::copy(*myCurrentWallSet.east.surface);
      if (!copy)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to RPG_Graphics_Surface::copy(), aborting\n")));

        return false;
      } // end IF
      if (SDL_BlitSurface(myWallBlend,
                          NULL,
                          copy,
                          NULL))
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to SDL_BlitSurface(): %s, aborting\n"),
                   ACE_TEXT(SDL_GetError())));

        // clean up
        SDL_FreeSurface(copy);

        return false;
      } // end IF
      SDL_FreeSurface(myCurrentWallSet.west.surface);
      myCurrentWallSet.west.surface = copy;

      // *NOTE*: north is just a "darkened" version of south...
      copy = RPG_Graphics_Surface::copy(*myCurrentWallSet.south.surface);
      if (!copy)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to RPG_Graphics_Surface::copy(), aborting\n")));

        return false;
      } // end IF
      if (SDL_BlitSurface(myWallBlend,
                          NULL,
                          copy,
                          NULL))
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to SDL_BlitSurface(): %s, aborting\n"),
                   ACE_TEXT(SDL_GetError())));

        // clean up
        SDL_FreeSurface(copy);

        return false;
      } // end IF
      SDL_FreeSurface(myCurrentWallSet.north.surface);
      myCurrentWallSet.north.surface = copy;

      // set wall opacity
      SDL_Surface* shaded_wall = NULL;
      shaded_wall =
          RPG_Graphics_Surface::alpha(*myCurrentWallSet.east.surface,
                                      static_cast<Uint8>((RPG_GRAPHICS_TILE_WALL_DEF_SE_OPACITY * SDL_ALPHA_OPAQUE)));
      if (!shaded_wall)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to RPG_Graphics_Surface::alpha(%u), aborting\n"),
                   static_cast<Uint8>((RPG_GRAPHICS_TILE_WALL_DEF_SE_OPACITY * SDL_ALPHA_OPAQUE))));

        return false;
      } // end IF
      SDL_FreeSurface(myCurrentWallSet.east.surface);
      myCurrentWallSet.east.surface = shaded_wall;

      shaded_wall =
          RPG_Graphics_Surface::alpha(*myCurrentWallSet.west.surface,
                                      static_cast<Uint8>((RPG_GRAPHICS_TILE_WALL_DEF_NW_OPACITY * SDL_ALPHA_OPAQUE)));
      if (!shaded_wall)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to RPG_Graphics_Surface::alpha(%u), aborting\n"),
                   static_cast<Uint8>((RPG_GRAPHICS_TILE_WALL_DEF_NW_OPACITY * SDL_ALPHA_OPAQUE))));

        return false;
      } // end IF
      SDL_FreeSurface(myCurrentWallSet.west.surface);
      myCurrentWallSet.west.surface = shaded_wall;

      shaded_wall =
          RPG_Graphics_Surface::alpha(*myCurrentWallSet.south.surface,
                                      static_cast<Uint8>((RPG_GRAPHICS_TILE_WALL_DEF_SE_OPACITY * SDL_ALPHA_OPAQUE)));
      if (!shaded_wall)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to RPG_Graphics_Surface::alpha(%u), aborting\n"),
                   static_cast<Uint8>((RPG_GRAPHICS_TILE_WALL_DEF_SE_OPACITY * SDL_ALPHA_OPAQUE))));

        return false;
      } // end IF
      SDL_FreeSurface(myCurrentWallSet.south.surface);
      myCurrentWallSet.south.surface = shaded_wall;

      shaded_wall =
          RPG_Graphics_Surface::alpha(*myCurrentWallSet.north.surface,
                                      static_cast<Uint8>((RPG_GRAPHICS_TILE_WALL_DEF_NW_OPACITY * SDL_ALPHA_OPAQUE)));
      if (!shaded_wall)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to RPG_Graphics_Surface::alpha(%u), aborting\n"),
                   static_cast<Uint8>((RPG_GRAPHICS_TILE_WALL_DEF_NW_OPACITY * SDL_ALPHA_OPAQUE))));

        return false;
      } // end IF
      SDL_FreeSurface(myCurrentWallSet.north.surface);
      myCurrentWallSet.north.surface = shaded_wall;

      // init wall tiles / position
      RPG_Client_Common_Tools::updateWalls(myCurrentWallSet,
                                           myWallTiles);

      break;
    }
    case RPG_Graphics_StyleUnion::DOORSTYLE:
    {
      RPG_Graphics_Common_Tools::loadDoorTileSet(style_in.doorstyle,
                                                 myCurrentDoorSet);
      // sanity check
      if ((myCurrentDoorSet.horizontal_open.surface == NULL)   ||
          (myCurrentDoorSet.vertical_open.surface == NULL)     ||
          (myCurrentDoorSet.horizontal_closed.surface == NULL) ||
          (myCurrentDoorSet.vertical_closed.surface == NULL)   ||
          (myCurrentDoorSet.broken.surface == NULL))
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("door-style \"%s\" is incomplete, aborting\n"),
                   ACE_TEXT(RPG_Graphics_DoorStyleHelper::RPG_Graphics_DoorStyleToString(style_in.doorstyle).c_str())));

        return false;
      } // end IF

      // init door tiles / position
      RPG_Client_Common_Tools::updateDoors(myCurrentDoorSet,
                                           *myEngine,
                                           myDoorTiles);

      break;
    }
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid style (was: %d), aborting\n"),
                 style_in.discriminator));

      return false;
    }
  } // end SWITCH

  return true;
}

void
RPG_Client_Window_Level::initCeiling()
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Window_Level::initCeiling"));

  // sanity check
  if (myCeilingTile)
  {
    SDL_FreeSurface(myCeilingTile);
    myCeilingTile = NULL;
  } // end IF

  // load tile for ceiling
  RPG_Graphics_GraphicTypeUnion type;
  type.discriminator = RPG_Graphics_GraphicTypeUnion::TILEGRAPHIC;
  type.tilegraphic = TILE_CEILING;
  myCeilingTile = RPG_Graphics_Common_Tools::loadGraphic(type,   // tile
                                                         true,   // convert to display format
                                                         false); // don't cache
  if (!myCeilingTile)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Graphics_Common_Tools::loadGraphic(\"%s\"), aborting\n"),
               ACE_TEXT(RPG_Graphics_Common_Tools::toString(type).c_str())));

    return;
  } // end IF

  SDL_Surface* shaded_ceiling = NULL;
  shaded_ceiling =
      RPG_Graphics_Surface::alpha(*myCeilingTile,
                                  static_cast<Uint8>((RPG_GRAPHICS_TILE_WALL_DEF_NW_OPACITY * SDL_ALPHA_OPAQUE)));
  if (!shaded_ceiling)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Graphics_Surface::alpha(%u), aborting\n"),
               static_cast<Uint8>((RPG_GRAPHICS_TILE_WALL_DEF_NW_OPACITY * SDL_ALPHA_OPAQUE))));

    // clean up
    SDL_FreeSurface(myCeilingTile);
    myCeilingTile = NULL;

    return;
  } // end IF

  SDL_FreeSurface(myCeilingTile);
  myCeilingTile = shaded_ceiling;
}

void
RPG_Client_Window_Level::initWallBlend (bool halfHeightWalls_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Window_Level::initWallBlend"));

  // sanity check
  if (myWallBlend)
  {
    SDL_FreeSurface (myWallBlend);
    myWallBlend = NULL;
  } // end IF

  myWallBlend =
    RPG_Graphics_Surface::create (RPG_GRAPHICS_TILE_WALL_WIDTH,
                                  (halfHeightWalls_in ? RPG_GRAPHICS_TILE_WALL_HEIGHT_HALF
                                                      : RPG_GRAPHICS_TILE_WALL_HEIGHT));
  if (!myWallBlend)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to RPG_Graphics_Surface::create(%u,%u), returning\n"),
                RPG_GRAPHICS_TILE_WALL_WIDTH,
                (halfHeightWalls_in ? RPG_GRAPHICS_TILE_WALL_HEIGHT_HALF
                                    : RPG_GRAPHICS_TILE_WALL_HEIGHT)));
    return;
  } // end IF

  if (SDL_FillRect (myWallBlend,
                    NULL,
                    RPG_Graphics_SDL_Tools::getColor (COLOR_BLACK_A10,
                                                      *myWallBlend)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_FillRect(): %s, returning\n"),
                ACE_TEXT (SDL_GetError ())));

    // clean up
    SDL_FreeSurface(myWallBlend);
    myWallBlend = NULL;

    return;
  } // end IF
}

bool
RPG_Client_Window_Level::initMiniMap (bool debug_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Window_Level::initMiniMap"));

  // sanity check(s)
  ACE_ASSERT (myClient);
  ACE_ASSERT (myEngine);
  ACE_ASSERT (inherited::screen_);

  RPG_Graphics_Offset_t offset =
      std::make_pair(std::numeric_limits<int>::max(),
                     std::numeric_limits<int>::max());
  RPG_Client_Window_MiniMap* minimap_window = NULL;
  ACE_NEW_NORETURN (minimap_window,
                    RPG_Client_Window_MiniMap (*this,
                                               offset,
                                               debug_in));
  if (!minimap_window)
  {
    ACE_DEBUG((LM_CRITICAL,
               ACE_TEXT("failed to allocate memory(%u): %m, aborting\n"),
               sizeof(RPG_Client_Window_MiniMap)));
    return false;
  } // end IF

  minimap_window->initialize (myClient, myEngine);
  minimap_window->setScreen (inherited::screen_);

  return true;
}

bool
RPG_Client_Window_Level::initMessageWindow ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Window_Level::initMessageWindow"));

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

  message_window->initialize (myClient,
                              RPG_CLIENT_MESSAGE_FONT,
                              RPG_CLIENT_MESSAGE_DEF_NUM_LINES);
  message_window->setScreen (inherited::screen_);

  return true;
}
