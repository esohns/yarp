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

#include "rpg_client_window_level.h"

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



RPG_Client_Window_Level::RPG_Client_Window_Level (const RPG_Graphics_SDLWindowBase& parent_in)
 : inherited (WINDOW_MAP,                 // type
              parent_in,                  // parent
              std::make_pair (0, 0),      // offset
              ACE_TEXT_ALWAYS_CHAR ("")), // title
#if defined (_DEBUG)
   myDebug (false),
#endif // _DEBUG
   myEngine (NULL),
   myClient (NULL),
   myClientAction (),
   myDrawMinimap (RPG_CLIENT_MINIMAP_DEF_ISON),
#if defined (_DEBUG)
   myShowCoordinates (false),
#endif // _DEBUG
   myShowMessages (RPG_CLIENT_MESSAGE_DEF_ISON),
   myCurrentFloorSet (),
   myCurrentFloorEdgeSet (),
   myCurrentWallSet(),
   myCeilingTile (NULL),
   myCurrentDoorSet (),
   myOffMapTile (NULL),
   myInvisibleTile (NULL),
   myVisionBlendTile (NULL),
   myVisionTempTile (NULL),
   myFloorEdgeTiles (),
   myWallTiles (),
   myDoorTiles (),
   myWallBlend (NULL),
   myLightingCache (),
   myLock (NULL, NULL),
   myView (std::make_pair (std::numeric_limits<unsigned int>::max (),
                           std::numeric_limits<unsigned int>::max ()))
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Window_Level::RPG_Client_Window_Level"));

  // initialize ceiling tile
  initCeiling ();

  // load tile for unmapped areas
  struct RPG_Graphics_GraphicTypeUnion type;
  type.discriminator = RPG_Graphics_GraphicTypeUnion::TILEGRAPHIC;
  type.tilegraphic = TILE_OFF_MAP;
  myOffMapTile =
    RPG_Graphics_Common_Tools::loadGraphic (type,   // tile
                                            true,   // convert to display format
                                            false); // don't cache
  if (!myOffMapTile)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to RPG_Graphics_Common_Tools::loadGraphic(\"%s\"), continuing\n"),
                ACE_TEXT (RPG_Graphics_Common_Tools::toString (type).c_str ())));

  // load tile for invisible areas
  type.tilegraphic = TILE_FLOOR_INVISIBLE;
  myInvisibleTile =
    RPG_Graphics_Common_Tools::loadGraphic (type,   // tile
                                            true,   // convert to display format
                                            false); // don't cache
  if (!myInvisibleTile)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to RPG_Graphics_Common_Tools::loadGraphic(\"%s\"), continuing\n"),
                ACE_TEXT (RPG_Graphics_Common_Tools::toString (type).c_str ())));

  // create tile for vision blending/temp
  myVisionBlendTile = RPG_Graphics_Surface::copy (*myOffMapTile);
  if (!myVisionBlendTile)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to RPG_Graphics_Surface::copy(%@), returning\n"),
                myOffMapTile));
    return;
  } // end IF
  RPG_Graphics_Surface::alpha (static_cast<Uint8> ((RPG_GRAPHICS_TILE_PREVSEEN_DEF_OPACITY * SDL_ALPHA_OPAQUE)),
                               *myVisionBlendTile);

  myVisionTempTile =
      RPG_Graphics_Surface::create (RPG_GRAPHICS_TILE_FLOOR_WIDTH,
                                    RPG_GRAPHICS_TILE_FLOOR_HEIGHT);
  if (!myVisionTempTile)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to RPG_Graphics_Surface::create(%u,%u), returning\n"),
                RPG_GRAPHICS_TILE_FLOOR_WIDTH,
                RPG_GRAPHICS_TILE_FLOOR_HEIGHT));
    return;
  } // end IF
}

RPG_Client_Window_Level::~RPG_Client_Window_Level ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Window_Level::~RPG_Client_Window_Level"));

  // clean up
  for (RPG_Graphics_FloorTilesConstIterator_t iterator = myCurrentFloorSet.tiles.begin ();
       iterator != myCurrentFloorSet.tiles.end ();
       iterator++)
#if defined (SDL_USE) || defined (SDL2_USE)
    SDL_FreeSurface ((*iterator).surface);
#elif defined (SDL3_USE)
    SDL_DestroySurface ((*iterator).surface);
#endif // SDL_USE || SDL2_USE || SDL3_USE

#if defined (SDL_USE) || defined (SDL2_USE)
  SDL_FreeSurface (myCurrentFloorEdgeSet.east.surface);
  SDL_FreeSurface (myCurrentFloorEdgeSet.west.surface);
  SDL_FreeSurface (myCurrentFloorEdgeSet.north.surface);
  SDL_FreeSurface (myCurrentFloorEdgeSet.south.surface);
  SDL_FreeSurface (myCurrentFloorEdgeSet.south_east.surface);
  SDL_FreeSurface (myCurrentFloorEdgeSet.south_west.surface);
  SDL_FreeSurface (myCurrentFloorEdgeSet.north_east.surface);
  SDL_FreeSurface (myCurrentFloorEdgeSet.north_west.surface);

  SDL_FreeSurface (myCurrentFloorEdgeSet.top.surface);
  SDL_FreeSurface (myCurrentFloorEdgeSet.right.surface);
  SDL_FreeSurface (myCurrentFloorEdgeSet.left.surface);
  SDL_FreeSurface (myCurrentFloorEdgeSet.bottom.surface);

  SDL_FreeSurface (myCurrentWallSet.east.surface);
  SDL_FreeSurface (myCurrentWallSet.west.surface);
  SDL_FreeSurface (myCurrentWallSet.north.surface);
  SDL_FreeSurface (myCurrentWallSet.south.surface);

  SDL_FreeSurface (myWallBlend);

  for (RPG_Client_BlendingMaskCacheIterator_t iterator = myLightingCache.begin ();
       iterator != myLightingCache.end ();
       iterator++)
    SDL_FreeSurface (*iterator);

  SDL_FreeSurface (myCurrentDoorSet.horizontal_open.surface);
  SDL_FreeSurface (myCurrentDoorSet.vertical_open.surface);
  SDL_FreeSurface (myCurrentDoorSet.horizontal_closed.surface);
  SDL_FreeSurface (myCurrentDoorSet.vertical_closed.surface);
  SDL_FreeSurface (myCurrentDoorSet.broken.surface);

  SDL_FreeSurface (myCeilingTile);
  SDL_FreeSurface (myOffMapTile);
  SDL_FreeSurface (myInvisibleTile);
  SDL_FreeSurface (myVisionBlendTile);
  SDL_FreeSurface (myVisionTempTile);
#elif defined (SDL3_USE)
  SDL_DestroySurface (myCurrentFloorEdgeSet.east.surface);
  SDL_DestroySurface (myCurrentFloorEdgeSet.west.surface);
  SDL_DestroySurface (myCurrentFloorEdgeSet.north.surface);
  SDL_DestroySurface (myCurrentFloorEdgeSet.south.surface);
  SDL_DestroySurface (myCurrentFloorEdgeSet.south_east.surface);
  SDL_DestroySurface (myCurrentFloorEdgeSet.south_west.surface);
  SDL_DestroySurface (myCurrentFloorEdgeSet.north_east.surface);
  SDL_DestroySurface (myCurrentFloorEdgeSet.north_west.surface);

  SDL_DestroySurface (myCurrentFloorEdgeSet.top.surface);
  SDL_DestroySurface (myCurrentFloorEdgeSet.right.surface);
  SDL_DestroySurface (myCurrentFloorEdgeSet.left.surface);
  SDL_DestroySurface (myCurrentFloorEdgeSet.bottom.surface);

  SDL_DestroySurface (myCurrentWallSet.east.surface);
  SDL_DestroySurface (myCurrentWallSet.west.surface);
  SDL_DestroySurface (myCurrentWallSet.north.surface);
  SDL_DestroySurface (myCurrentWallSet.south.surface);

  SDL_DestroySurface (myWallBlend);

  for (RPG_Client_BlendingMaskCacheIterator_t iterator = myLightingCache.begin ();
       iterator != myLightingCache.end ();
       iterator++)
    SDL_DestroySurface (*iterator);

  SDL_DestroySurface (myCurrentDoorSet.horizontal_open.surface);
  SDL_DestroySurface (myCurrentDoorSet.vertical_open.surface);
  SDL_DestroySurface (myCurrentDoorSet.horizontal_closed.surface);
  SDL_DestroySurface (myCurrentDoorSet.vertical_closed.surface);
  SDL_DestroySurface (myCurrentDoorSet.broken.surface);

  SDL_DestroySurface (myCeilingTile);
  SDL_DestroySurface (myOffMapTile);
  SDL_DestroySurface (myInvisibleTile);
  SDL_DestroySurface (myVisionBlendTile);
  SDL_DestroySurface (myVisionTempTile);
#endif // SDL_USE || SDL2_USE || SDL3_USE
}

void
RPG_Client_Window_Level::setView (const RPG_Map_Position_t& view_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Window_Level::setView"));

  ACE_GUARD (ACE_Thread_Mutex, aGuard, myLock);

  myView = view_in;
}

void
RPG_Client_Window_Level::setView (int offsetX_in,
                                  int offsetY_in,
                                  bool lockedAccess_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Window_Level::setView"));

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
RPG_Client_Window_Level::getView () const
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Window_Level::getView"));

  RPG_Graphics_Position_t result =
    std::make_pair (std::numeric_limits<unsigned int>::max (),
                    std::numeric_limits<unsigned int>::max ());

  { ACE_GUARD_RETURN (ACE_Thread_Mutex, aGuard, myLock, result);
    result = myView;
  } // end lock scope

  return result;
}

void
RPG_Client_Window_Level::toggleMiniMap ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Window_Level::toggleMiniMap"));

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
RPG_Client_Window_Level::showMiniMap () const
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Window_Level::showMiniMap"));

  bool result = false;

  { ACE_GUARD_RETURN (ACE_Thread_Mutex, aGuard, myLock, false); // *TODO*: avoid false negative
    result = myDrawMinimap;
  } // end lock scope

  return result;
}

void
RPG_Client_Window_Level::toggleMessages ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Window_Level::toggleMessages"));

  ACE_GUARD (ACE_Thread_Mutex, aGuard, myLock);

  myShowMessages = !myShowMessages;
}

bool
RPG_Client_Window_Level::showMessages () const
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Window_Level::showMessages"));

  bool result = false;

  { ACE_GUARD_RETURN (ACE_Thread_Mutex, aGuard, myLock, result); // *TODO*: avoid false negative
    result = myShowMessages;
  } // end lock scope

  return result;
}

void
RPG_Client_Window_Level::toggleDoor (const RPG_Map_Position_t& position_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Window_Level::toggleDoor"));

  ACE_ASSERT (myEngine);

  myEngine->lock ();
  enum RPG_Graphics_Orientation orientation =
    RPG_Client_Common_Tools::getDoorOrientation (position_in,
                                                 *myEngine,
                                                 false); // locked access ?
  bool is_open =
    (myEngine->state (position_in, false) == DOORSTATE_OPEN); // locked access ?
  myEngine->unlock ();

  ACE_GUARD (ACE_Thread_Mutex, aGuard, myLock);

  switch (orientation)
  {
    case ORIENTATION_HORIZONTAL:
    {
      myDoorTiles[position_in] = (is_open ? myCurrentDoorSet.horizontal_open
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
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid door orientation \"%s\", returning\n"),
                  ACE_TEXT (RPG_Graphics_OrientationHelper::RPG_Graphics_OrientationToString (orientation).c_str ())));
      return;
    }
  } // end SWITCH
}

bool
RPG_Client_Window_Level::initialize (RPG_Client_Engine* clientEngine_in,
                                     RPG_Engine* engine_in,
                                     bool debug_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Window_Level::initialize"));

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
RPG_Client_Window_Level::drawChild (enum RPG_Graphics_WindowType child_in,
                                    SDL_Surface* targetSurface_in,
                                    unsigned int offsetX_in,
                                    unsigned int offsetY_in,
                                    bool refresh_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Window_Level::drawChild"));

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
RPG_Client_Window_Level::initialize (const RPG_Graphics_Style& style_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Window_Level::initialize"));

  ACE_GUARD (ACE_Thread_Mutex, aGuard, myLock);

  // initialize style
  struct RPG_Graphics_StyleUnion style;
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

  // initialize tiles / position
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
RPG_Client_Window_Level::setBlendRadius (ACE_UINT8 radius_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Window_Level::setBlendRadius"));

  ACE_GUARD (ACE_Thread_Mutex, aGuard, myLock);

  // sanity check
  if (radius_in == 0)
  {
    for (RPG_Client_BlendingMaskCacheIterator_t iterator = myLightingCache.begin ();
         iterator != myLightingCache.end ();
         iterator++)
#if defined (SDL_USE) || defined (SDL2_USE)
      SDL_FreeSurface (*iterator);
#elif defined (SDL3_USE)
      SDL_DestroySurface (*iterator);
#endif // SDL_USE || SDL2_USE || SDL3_USE
    myLightingCache.clear ();
    return;
  } // end IF

  // grow/shrink cache as necessary
  int delta = static_cast<int> (myLightingCache.size ()) - radius_in;
  if (delta > 0)
  {
    for (int i = delta;
         i > 0;
         i--)
    {
#if defined (SDL_USE) || defined (SDL2_USE)
      SDL_FreeSurface (myLightingCache.back ());
#elif defined (SDL3_USE)
      SDL_DestroySurface (myLightingCache.back ());
#endif // SDL_USE || SDL2_USE || SDL3_USE
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
      new_entry = RPG_Graphics_Surface::copy (*myOffMapTile);
      myLightingCache.push_back (new_entry);
    } // end IF
  } // end ELSEIF
  ACE_ASSERT (myLightingCache.size () == radius_in);

  // *NOTE*: quantum == (SDL_ALPHA_OPAQUE / (visible_radius + 1));
  Uint8 quantum =
    static_cast<Uint8> (SDL_ALPHA_OPAQUE / static_cast<float> (radius_in + 1));
  RPG_Client_BlendingMaskCacheIterator_t iterator = myLightingCache.begin ();
  for (unsigned int i = 1;
       i <= radius_in;
       i++, iterator++)
  {
    RPG_Graphics_Surface::alpha ((i * quantum), // opacity
                                 **iterator);

#if defined (_DEBUG)
    if (myDebug)
    {
      std::ostringstream converter;
      std::string dump_path = Common_File_Tools::getTempDirectory ();
      dump_path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
      dump_path += ACE_TEXT_ALWAYS_CHAR ("floor_blend_");
      converter.str (""); converter.clear ();
      converter << i;
      dump_path += converter.str ();
      dump_path += ACE_TEXT_ALWAYS_CHAR (".png");
      RPG_Graphics_Surface::savePNG (**iterator, // image
                                     dump_path,  // file
                                     true);      // WITH alpha
    } // end IF
#endif // _DEBUG
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
             true); // refresh ?
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
RPG_Client_Window_Level::draw (SDL_Surface* targetSurface_in,
                               unsigned int offsetX_in,
                               unsigned int offsetY_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Window_Level::draw"));

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
  ACE_ASSERT (myCeilingTile);
  ACE_ASSERT (myOffMapTile);
  ACE_ASSERT (myInvisibleTile);
  ACE_ASSERT (myVisionBlendTile);
  ACE_ASSERT (static_cast<int> (offsetX_in) <= target_surface->w);
  ACE_ASSERT (static_cast<int> (offsetY_in) <= target_surface->h);

  inherited::clear (COLOR_BLACK, // color
                    false);      // don't clip

  // initialize clipping
  inherited::clip (target_surface,
                   offsetX_in,
                   offsetY_in);

  // *NOTE*: mapping tile coordinates to world-, and world- to screen coordinates basically
  // works as follows (dimetric projection):
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
  myEngine->lock ();

  RPG_Engine_EntityID_t active_entity_id = myEngine->getActive (false); // locked access ?
  RPG_Map_Position_t active_position =
      std::make_pair (std::numeric_limits<unsigned int>::max (),
                      std::numeric_limits<unsigned int>::max ());
  RPG_Map_Positions_t visible_positions;
  ACE_UINT8 visible_radius_i = 0;
  if (active_entity_id)
  {
    active_position = myEngine->getPosition (active_entity_id,
                                             false); // locked access ?
    myEngine->getVisiblePositions (active_entity_id,
                                   visible_positions,
                                   false); // locked access ?
    visible_radius_i = myEngine->getVisibleRadius (active_entity_id,
                                                   false); // locked access ?
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

  struct SDL_Rect dirty_region = {0, 0, 0, 0};
  struct SDL_Rect window_area;
  getArea (window_area,
           true); // toplevel- ?

  int i, j;
  RPG_Client_SignedPosition_t current_map_position =
      std::make_pair (std::numeric_limits<int>::max (),
                      std::numeric_limits<int>::max ());
  RPG_Graphics_FloorTilesConstIterator_t floor_iterator =
      myCurrentFloorSet.tiles.begin ();
  RPG_Graphics_FloorTilesConstIterator_t begin_row =
      myCurrentFloorSet.tiles.begin ();
  RPG_Graphics_Position_t screen_position =
      std::make_pair (std::numeric_limits<unsigned int>::max (),
                      std::numeric_limits<unsigned int>::max ());
  RPG_Map_Size_t map_size = myEngine->getSize (false); // locked access ?
  RPG_Map_Element current_element = MAPELEMENT_INVALID;
  bool is_visible, has_been_seen, is_active_position;
  RPG_Client_BlendingMaskCacheIterator_t blendmask_iterator =
      myLightingCache.end ();
  RPG_Graphics_FloorEdgeTileMapIterator_t floor_edge_iterator =
      myFloorEdgeTiles.end ();

#if defined (_DEBUG)
  std::ostringstream converter;
  std::string tile_text;
  RPG_Graphics_TextSize_t tile_text_size;
#endif // _DEBUG

  // pass 1
  for (i = -static_cast<int> (top_right.second);
       i <= static_cast<int> (top_right.second);
       i++)
  {
    current_map_position.second = myView.second + i;

    // step0: off the map ? --> continue
    if ((current_map_position.second < 0) ||
        (current_map_position.second >= static_cast<int> (map_size.second)))
      continue;

    // step1: floor tile rotation
    begin_row = myCurrentFloorSet.tiles.begin ();
    std::advance (begin_row,
                  current_map_position.second % (myCurrentFloorSet.tiles.size () / myCurrentFloorSet.columns));

    for (j = diff + i;
         (j + i) <= sum;
         j++)
    {
      current_map_position.first = myView.first + j;

      // step0: off the map ? --> continue
      if ((current_map_position.first < 0) ||
          (current_map_position.first >= static_cast<int> (map_size.first)))
        continue;

      is_visible =
          (visible_positions.find (current_map_position) != visible_positions.end ());
      has_been_seen =
        (active_entity_id ? myEngine->hasSeen (active_entity_id,
                                               current_map_position,
                                               false) // locked access ?
                          : false);
      is_active_position =
        (static_cast<unsigned int> (current_map_position.first)  == active_position.first) &&
        (static_cast<unsigned int> (current_map_position.second) == active_position.second);
      current_element = myEngine->getElement (current_map_position,
                                              false); // locked access ?
      ACE_ASSERT (current_element != MAPELEMENT_INVALID);

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
        RPG_Graphics_Surface::put (screen_position,
                                   ((is_visible ||
                                     has_been_seen) ? *myOffMapTile
                                                    : *myInvisibleTile),
                                   target_surface,
                                   dirty_region);
        continue;
      } // end IF

      // step2: floor
      // floor tile rotation
      floor_iterator = begin_row;
      std::advance (floor_iterator,
                    (myCurrentFloorSet.rows *
                     (current_map_position.first % myCurrentFloorSet.columns)));

      if ((current_element == MAPELEMENT_FLOOR) ||
          (current_element == MAPELEMENT_DOOR))
      {
        // blend tile ?
        if (is_visible)
        {
          if (!is_active_position)
          {
            unsigned int distance_i =
              RPG_Map_Common_Tools::distanceMax (active_position,
                                                 current_map_position);
            if (distance_i <= visible_radius_i)
            { ACE_ASSERT (!myLightingCache.empty ());

              // step0: find blend mask
              blendmask_iterator = myLightingCache.begin ();
              std::advance (blendmask_iterator, distance_i - 1);

              // step1: get background
              RPG_Graphics_Surface::copy (*(*floor_iterator).surface,
                                          *myVisionTempTile);

              // step2: blend tiles
              if (SDL_BlitSurface (*blendmask_iterator, // source
                                   NULL,                // aspect (--> everything)
                                   myVisionTempTile,    // target
                                   NULL))               // aspect
                ACE_DEBUG ((LM_ERROR,
                            ACE_TEXT ("failed to SDL_BlitSurface(): %s, continuing\n"),
                            ACE_TEXT (SDL_GetError ())));
            } // end IF
            else
            {
              // step1: get background
              RPG_Graphics_Surface::copy (*(*floor_iterator).surface,
                                          *myVisionTempTile);

              // step2: blend tiles
              if (SDL_BlitSurface (myVisionBlendTile, // source
                                   NULL,              // aspect (--> everything)
                                   myVisionTempTile,  // target
                                   NULL))             // aspect
                ACE_DEBUG ((LM_ERROR,
                            ACE_TEXT ("failed to SDL_BlitSurface(): %s, continuing\n"),
                            ACE_TEXT (SDL_GetError ())));
            } // end ELSE
          } // end IF
        } // end IF
        else if (has_been_seen)
        {
          // step1: get background
          RPG_Graphics_Surface::copy (*(*floor_iterator).surface,
                                      *myVisionTempTile);

          // step2: blend tiles
          if (SDL_BlitSurface (myVisionBlendTile, // source
                               NULL,              // aspect (--> everything)
                               myVisionTempTile,  // target
                               NULL))             // aspect
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("failed to SDL_BlitSurface(): %s, continuing\n"),
                        ACE_TEXT (SDL_GetError ())));
        } // end IF
        RPG_Graphics_Surface::put (screen_position,
                                   (is_visible ? (is_active_position ? *(*floor_iterator).surface
                                                                     : *myVisionTempTile)
                                               : (has_been_seen ? *myVisionTempTile
                                                                : *myInvisibleTile)),
                                   target_surface,
                                   dirty_region);

#if defined (_DEBUG)
        if (myShowCoordinates)
        {
          struct SDL_Rect rect;
          rect.x = screen_position.first;
          rect.y = screen_position.second;
          rect.w = (*floor_iterator).surface->w;
          rect.h = (*floor_iterator).surface->h;
          //RPG_Graphics_Surface::putRect(rect,                              // rectangle
          //                              RPG_GRAPHICS_WINDOW_HOTSPOT_COLOR, // color
          //                              targetSurface);                    // target surface
          converter.str (ACE_TEXT (""));
          converter.clear ();
          tile_text = ACE_TEXT_ALWAYS_CHAR ("[");
          converter << current_map_position.first;
          tile_text += converter.str ();
          tile_text += ACE_TEXT_ALWAYS_CHAR (",");
          converter.str (ACE_TEXT (""));
          converter.clear ();
          converter << current_map_position.second;
          tile_text += converter.str ();
          tile_text += ACE_TEXT_ALWAYS_CHAR("]");
          tile_text_size = RPG_Graphics_Common_Tools::textSize (FONT_MAIN_SMALL,
                                                                tile_text);
          RPG_Graphics_Surface::putText (FONT_MAIN_SMALL,
                                         tile_text,
                                         RPG_Graphics_SDL_Tools::colorToSDLColor (RPG_Graphics_SDL_Tools::getColor (RPG_GRAPHICS_FONT_DEF_COLOR,
                                                                                                                    *target_surface->format,
                                                                                                                    1.0F),
                                                                                  *target_surface->format),
                                         true, // add shade
                                         RPG_Graphics_SDL_Tools::colorToSDLColor (RPG_Graphics_SDL_Tools::getColor (RPG_GRAPHICS_FONT_DEF_SHADECOLOR,
                                                                                                                    *target_surface->format,
                                                                                                                    1.0F),
                                                                                  *target_surface->format),
                                         std::make_pair ((rect.x + ((rect.w - tile_text_size.first)  / 2)),
                                                         (rect.y + ((rect.h - tile_text_size.second) / 2))),
                                         target_surface,
                                         dirty_region);
        } // end IF
#endif // _DEBUG

        // step3: floor edges
        floor_edge_iterator = myFloorEdgeTiles.find (current_map_position);
        if ((floor_edge_iterator != myFloorEdgeTiles.end ()) &&
            is_visible)
        {
          // straight edges
          if ((*floor_edge_iterator).second.west.surface)
            RPG_Graphics_Surface::put (std::make_pair ((screen_position.first +
                                                        (*floor_edge_iterator).second.west.offset_x),
                                                       (screen_position.second -
                                                        (*floor_edge_iterator).second.west.surface->h +
                                                        (RPG_GRAPHICS_TILE_FLOOR_HEIGHT / 2) +
                                                        (*floor_edge_iterator).second.west.offset_y)),
                                       *(myCurrentFloorEdgeSet.west.surface),
                                       target_surface,
                                       dirty_region);
          if ((*floor_edge_iterator).second.north.surface)
            RPG_Graphics_Surface::put (std::make_pair ((screen_position.first +
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
            RPG_Graphics_Surface::put (std::make_pair ((screen_position.first +
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
            RPG_Graphics_Surface::put (std::make_pair ((screen_position.first +
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
            RPG_Graphics_Surface::put (std::make_pair ((screen_position.first +
                                                        (*floor_edge_iterator).second.south_west.offset_x),
                                                       (screen_position.second -
                                                        (*floor_edge_iterator).second.south_west.surface->h +
                                                        (RPG_GRAPHICS_TILE_FLOOR_HEIGHT / 2) +
                                                        (*floor_edge_iterator).second.south_west.offset_y)),
                                       *(myCurrentFloorEdgeSet.south_west.surface),
                                       target_surface,
                                       dirty_region);
          if ((*floor_edge_iterator).second.north_west.surface)
            RPG_Graphics_Surface::put (std::make_pair ((screen_position.first +
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
            RPG_Graphics_Surface::put (std::make_pair ((screen_position.first +
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
            RPG_Graphics_Surface::put (std::make_pair ((screen_position.first +
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
            RPG_Graphics_Surface::put (std::make_pair ((screen_position.first +
                                                        (*floor_edge_iterator).second.top.offset_x),
                                                       (screen_position.second -
                                                        (*floor_edge_iterator).second.top.surface->h +
                                                        (RPG_GRAPHICS_TILE_FLOOR_HEIGHT / 2) +
                                                        (*floor_edge_iterator).second.top.offset_y)),
                                       *(myCurrentFloorEdgeSet.top.surface),
                                       target_surface,
                                       dirty_region);
          if ((*floor_edge_iterator).second.right.surface)
            RPG_Graphics_Surface::put (std::make_pair ((screen_position.first +
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
            RPG_Graphics_Surface::put (std::make_pair ((screen_position.first +
                                                        (*floor_edge_iterator).second.left.offset_x),
                                                       (screen_position.second -
                                                        (*floor_edge_iterator).second.left.surface->h +
                                                        (RPG_GRAPHICS_TILE_FLOOR_HEIGHT / 2) +
                                                        (*floor_edge_iterator).second.left.offset_y)),
                                       *(myCurrentFloorEdgeSet.left.surface),
                                       target_surface,
                                       dirty_region);
          if ((*floor_edge_iterator).second.bottom.surface)
            RPG_Graphics_Surface::put (std::make_pair ((screen_position.first +
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
  RPG_Graphics_WallTileMapIterator_t wall_iterator = myWallTiles.end ();
  RPG_Graphics_DoorTileMapIterator_t door_iterator = myDoorTiles.end ();
  RPG_Engine_EntityID_t entity_id = 0;
  struct RPG_Graphics_Style style_s = myClient->getStyle ();
  for (i = -static_cast<int> (top_right.second);
       i <= static_cast<int> (top_right.second);
       i++)
  {
    current_map_position.second = myView.second + i;

    // step0: off the map ? --> continue
    if ((current_map_position.second < 0) ||
        (current_map_position.second >= static_cast<int> (map_size.second)))
      continue;

    for (j = diff + i;
          (j + i) <= sum;
          j++)
    {
      current_map_position.first = myView.first + j;

      // step0: off the map ? --> continue
      if ((current_map_position.first < 0) ||
          (current_map_position.first >= static_cast<int> (map_size.first)))
        continue;

      is_visible =
          (visible_positions.find (current_map_position) != visible_positions.end ());

      // transform map coordinates into screen coordinates
//       x = (targetSurface->w / 2) + (RPG_GRAPHICS_TILE_WIDTH_MOD * (j - i));
//       y = (targetSurface->h / 2) + (RPG_GRAPHICS_TILE_HEIGHT_MOD * (j + i));
      screen_position =
          RPG_Graphics_Common_Tools::mapToScreen (current_map_position,
                                                  std::make_pair (window_area.w,
                                                                  window_area.h),
                                                  myView);

      wall_iterator = myWallTiles.find (current_map_position);
      door_iterator = myDoorTiles.find (current_map_position);

      // step1: walls (west & north)
      if ((wall_iterator != myWallTiles.end ()) &&
          is_visible)
      {
        if ((*wall_iterator).second.west.surface)
          RPG_Graphics_Surface::put (std::make_pair (screen_position.first,
                                                     (screen_position.second -
                                                      (*wall_iterator).second.west.surface->h +
                                                      (RPG_GRAPHICS_TILE_FLOOR_HEIGHT / 2))),
                                     *(myCurrentWallSet.west.surface),
                                     target_surface,
                                     dirty_region);
        if ((*wall_iterator).second.north.surface)
          RPG_Graphics_Surface::put (std::make_pair ((screen_position.first +
                                                      (RPG_GRAPHICS_TILE_FLOOR_WIDTH / 2)),
                                                     (screen_position.second -
                                                      (*wall_iterator).second.north.surface->h +
                                                      (RPG_GRAPHICS_TILE_FLOOR_HEIGHT / 2))),
                                     *(myCurrentWallSet.north.surface),
                                     target_surface,
                                     dirty_region);
      } // end IF

      // step2: doors
      if ((door_iterator != myDoorTiles.end ()) &&
          is_visible)
      {
        // *NOTE*: doors are drawn in the "middle" of the floor tile
        RPG_Graphics_Surface::put (std::make_pair ((screen_position.first +
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
      entity_id = myEngine->hasEntity (current_map_position,
                                       false); // locked access ?
      if (entity_id && is_visible)
      {
        // put() restore()s the current bg --> invalidate current bg
        RPG_CLIENT_ENTITY_MANAGER_SINGLETON::instance ()->invalidateBG (entity_id);

        // draw creature
        RPG_CLIENT_ENTITY_MANAGER_SINGLETON::instance ()->put (entity_id,
                                                               screen_position,
                                                               dirty_region,
                                                               false,    // clip window ? --> already clipped (see above)
                                                               true,     // locked access ?
#if defined (_DEBUG)
                                                               myDebug); // debug ?
#else
                                                               false);   // debug ?
#endif // _DEBUG
      } // end IF

      // step6: effects

      // step7: walls (south & east)
      if ((wall_iterator != myWallTiles.end ()) &&
          is_visible)
      {
        if ((*wall_iterator).second.south.surface)
          RPG_Graphics_Surface::put (std::make_pair (screen_position.first,
                                                     (screen_position.second -
                                                     (*wall_iterator).second.south.surface->h +
                                                     RPG_GRAPHICS_TILE_FLOOR_HEIGHT)),
                                     *(*wall_iterator).second.south.surface,
                                     target_surface,
                                     dirty_region);
        if ((*wall_iterator).second.east.surface)
          RPG_Graphics_Surface::put (std::make_pair ((screen_position.first +
                                                      (RPG_GRAPHICS_TILE_FLOOR_WIDTH / 2)),
                                                     (screen_position.second -
                                                      (*wall_iterator).second.east.surface->h +
                                                     RPG_GRAPHICS_TILE_FLOOR_HEIGHT)),
                                     *(*wall_iterator).second.east.surface,
                                     target_surface,
                                     dirty_region);
      } // end IF

      // step8: ceiling
      // *TODO*: this is static information: compute once / level and use a lookup-table here...
      // *TODO*: does not work quite right yet
      if (RPG_Client_Common_Tools::hasCeiling (current_map_position,
                                               *myEngine,
                                               false) && // locked access ?
          is_visible)
        RPG_Graphics_Surface::put (std::make_pair (screen_position.first,
                                                   (screen_position.second -
                                                    (style_s.half_height_walls ? (RPG_GRAPHICS_TILE_WALL_HEIGHT / 2)
                                                                               : RPG_GRAPHICS_TILE_WALL_HEIGHT) +
                                                    (RPG_GRAPHICS_TILE_FLOOR_HEIGHT / (style_s.half_height_walls ? 8 : 2)))),
                                   *myCeilingTile,
                                   target_surface,
                                   dirty_region);
    } // end FOR
  } // end FOR

  // unlock engine
  myEngine->unlock ();

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
RPG_Client_Window_Level::handleEvent (const union SDL_Event& event_in,
                                      RPG_Graphics_IWindowBase* window_in,
                                      struct SDL_Rect& dirtyRegion_out)
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Window_Level::handleEvent"));

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
            if (SDL_PushEvent (&sdl_event))
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
                  player_action.command = COMMAND_DOOR_CLOSE; break;
                case DOORSTATE_CLOSED:
                case DOORSTATE_LOCKED:
                  player_action.command = COMMAND_DOOR_OPEN; break;
                case DOORSTATE_BROKEN:
                  ignore_door = true; break;
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
RPG_Client_Window_Level::setStyle (const struct RPG_Graphics_StyleUnion& style_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Window_Level::setStyle"));

  switch (style_in.discriminator)
  {
    case RPG_Graphics_StyleUnion::EDGESTYLE:
    {
      RPG_Graphics_Common_Tools::loadFloorEdgeTileSet (style_in.edgestyle,
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
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("edge-style \"%s\" has no tiles, continuing\n"),
                    ACE_TEXT (RPG_Graphics_EdgeStyleHelper::RPG_Graphics_EdgeStyleToString (style_in.edgestyle).c_str ())));
      } // end IF

      // update floor edge tiles / position
      RPG_Client_Common_Tools::updateFloorEdges (myCurrentFloorEdgeSet,
                                                 myFloorEdgeTiles);

      break;
    }
    case RPG_Graphics_StyleUnion::FLOORSTYLE:
    {
      RPG_Graphics_Common_Tools::loadFloorTileSet (style_in.floorstyle,
                                                   myCurrentFloorSet);
      // sanity check
      if (myCurrentFloorSet.tiles.empty ())
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("floor-style \"%s\" has no tiles, continuing\n"),
                    ACE_TEXT (RPG_Graphics_FloorStyleHelper::RPG_Graphics_FloorStyleToString (style_in.floorstyle).c_str ())));
      } // end IF

      break;
    }
    case RPG_Graphics_StyleUnion::WALLSTYLE:
    {
      RPG_Graphics_Common_Tools::loadWallTileSet (style_in.wallstyle,
                                                  myClient->getStyle ().half_height_walls,
                                                  myCurrentWallSet);
      // sanity check
      if ((myCurrentWallSet.east.surface == NULL)  ||
          (myCurrentWallSet.west.surface == NULL)  ||
          (myCurrentWallSet.north.surface == NULL) ||
          (myCurrentWallSet.south.surface == NULL))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("wall-style \"%s\" is incomplete, aborting\n"),
                    ACE_TEXT (RPG_Graphics_WallStyleHelper::RPG_Graphics_WallStyleToString (style_in.wallstyle).c_str ())));
        return false;
      } // end IF

      initWallBlend (myClient->getStyle ().half_height_walls);

      // *NOTE*: west is just a "darkened" version of east...
      SDL_Surface* copy = NULL;
      copy = RPG_Graphics_Surface::copy (*myCurrentWallSet.east.surface);
      if (!copy)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to RPG_Graphics_Surface::copy(), aborting\n")));

        return false;
      } // end IF
      if (SDL_BlitSurface (myCurrentWallSet.west.surface,
                           NULL,
                           copy,
                           NULL))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to SDL_BlitSurface(): %s, aborting\n"),
                    ACE_TEXT (SDL_GetError ())));
#if defined (SDL_USE) || defined (SDL2_USE)
        SDL_FreeSurface (copy);
#elif defined (SDL3_USE)
        SDL_DestroySurface (copy);
#endif // SDL_USE || SDL2_USE || SDL3_USE
        return false;
      } // end IF
#if defined (SDL_USE) || defined (SDL2_USE)
      SDL_FreeSurface (myCurrentWallSet.west.surface);
#elif defined (SDL3_USE)
      SDL_DestroySurface (myCurrentWallSet.west.surface);
#endif // SDL_USE || SDL2_USE || SDL3_USE
      myCurrentWallSet.west.surface = copy;

      // *NOTE*: north is just a "darkened" version of south...
      copy = RPG_Graphics_Surface::copy (*myCurrentWallSet.south.surface);
      if (!copy)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to RPG_Graphics_Surface::copy(), aborting\n")));
        return false;
      } // end IF
      if (SDL_BlitSurface (myCurrentWallSet.north.surface,
                           NULL,
                           copy,
                           NULL))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to SDL_BlitSurface(): %s, aborting\n"),
                    ACE_TEXT (SDL_GetError ())));
#if defined (SDL_USE) || defined (SDL2_USE)
        SDL_FreeSurface (copy);
#elif defined (SDL3_USE)
        SDL_DestroySurface (copy);
#endif // SDL_USE || SDL2_USE || SDL3_USE
        return false;
      } // end IF
#if defined (SDL_USE) || defined (SDL2_USE)
      SDL_FreeSurface (myCurrentWallSet.north.surface);
#elif defined (SDL3_USE)
      SDL_DestroySurface (myCurrentWallSet.north.surface);
#endif // SDL_USE || SDL2_USE || SDL3_USE
      myCurrentWallSet.north.surface = copy;

      // set wall opacity
      SDL_Surface* shaded_wall = NULL;
      shaded_wall =
          RPG_Graphics_Surface::alpha (*myCurrentWallSet.east.surface,
                                       static_cast<Uint8> ((RPG_GRAPHICS_TILE_WALL_DEF_SE_OPACITY * SDL_ALPHA_OPAQUE)));
      if (!shaded_wall)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to RPG_Graphics_Surface::alpha(%u), aborting\n"),
                    static_cast<Uint8> ((RPG_GRAPHICS_TILE_WALL_DEF_SE_OPACITY * SDL_ALPHA_OPAQUE))));
        return false;
      } // end IF
#if defined (SDL_USE) || defined (SDL2_USE)
      SDL_FreeSurface (myCurrentWallSet.east.surface);
#elif defined (SDL3_USE)
      SDL_DestroySurface (myCurrentWallSet.east.surface);
#endif // SDL_USE || SDL2_USE || SDL3_USE
      myCurrentWallSet.east.surface = shaded_wall;

      shaded_wall =
          RPG_Graphics_Surface::alpha (*myCurrentWallSet.west.surface,
                                       static_cast<Uint8>((RPG_GRAPHICS_TILE_WALL_DEF_NW_OPACITY * SDL_ALPHA_OPAQUE)));
      if (!shaded_wall)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to RPG_Graphics_Surface::alpha(%u), aborting\n"),
                    static_cast<Uint8> ((RPG_GRAPHICS_TILE_WALL_DEF_NW_OPACITY * SDL_ALPHA_OPAQUE))));
        return false;
      } // end IF
#if defined (SDL_USE) || defined (SDL2_USE)
      SDL_FreeSurface (myCurrentWallSet.west.surface);
#elif defined (SDL3_USE)
      SDL_DestroySurface (myCurrentWallSet.west.surface);
#endif // SDL_USE || SDL2_USE || SDL3_USE
      myCurrentWallSet.west.surface = shaded_wall;

      shaded_wall =
          RPG_Graphics_Surface::alpha (*myCurrentWallSet.south.surface,
                                       static_cast<Uint8> ((RPG_GRAPHICS_TILE_WALL_DEF_SE_OPACITY * SDL_ALPHA_OPAQUE)));
      if (!shaded_wall)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to RPG_Graphics_Surface::alpha(%u), aborting\n"),
                    static_cast<Uint8> ((RPG_GRAPHICS_TILE_WALL_DEF_SE_OPACITY * SDL_ALPHA_OPAQUE))));
        return false;
      } // end IF
#if defined (SDL_USE) || defined (SDL2_USE)
      SDL_FreeSurface (myCurrentWallSet.south.surface);
#elif defined (SDL3_USE)
      SDL_DestroySurface (myCurrentWallSet.south.surface);
#endif // SDL_USE || SDL2_USE || SDL3_USE
      myCurrentWallSet.south.surface = shaded_wall;

      shaded_wall =
          RPG_Graphics_Surface::alpha (*myCurrentWallSet.north.surface,
                                       static_cast<Uint8> ((RPG_GRAPHICS_TILE_WALL_DEF_NW_OPACITY * SDL_ALPHA_OPAQUE)));
      if (!shaded_wall)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to RPG_Graphics_Surface::alpha(%u), aborting\n"),
                    static_cast<Uint8> ((RPG_GRAPHICS_TILE_WALL_DEF_NW_OPACITY * SDL_ALPHA_OPAQUE))));
        return false;
      } // end IF
#if defined (SDL_USE) || defined (SDL2_USE)
      SDL_FreeSurface (myCurrentWallSet.north.surface);
#elif defined (SDL3_USE)
      SDL_DestroySurface (myCurrentWallSet.north.surface);
#endif // SDL_USE || SDL2_USE || SDL3_USE
      myCurrentWallSet.north.surface = shaded_wall;

      // init wall tiles / position
      RPG_Client_Common_Tools::updateWalls (myCurrentWallSet,
                                            myWallTiles);

      break;
    }
    case RPG_Graphics_StyleUnion::DOORSTYLE:
    {
      RPG_Graphics_Common_Tools::loadDoorTileSet (style_in.doorstyle,
                                                  myCurrentDoorSet);
      // sanity check
      if ((myCurrentDoorSet.horizontal_open.surface == NULL)   ||
          (myCurrentDoorSet.vertical_open.surface == NULL)     ||
          (myCurrentDoorSet.horizontal_closed.surface == NULL) ||
          (myCurrentDoorSet.vertical_closed.surface == NULL)   ||
          (myCurrentDoorSet.broken.surface == NULL))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("door-style \"%s\" is incomplete, aborting\n"),
                    ACE_TEXT (RPG_Graphics_DoorStyleHelper::RPG_Graphics_DoorStyleToString (style_in.doorstyle).c_str ())));
        return false;
      } // end IF

      // initialize door tiles / position
      myDoorTiles.clear ();
      struct RPG_Graphics_TileElement current_tile;
      RPG_Map_Positions_t doors = myEngine->getDoors (true); // locked access ?
      for (RPG_Map_PositionsConstIterator_t iterator = doors.begin ();
           iterator != doors.end ();
           iterator++)
        myDoorTiles.insert (std::make_pair (*iterator, current_tile));

      RPG_Client_Common_Tools::updateDoors (myCurrentDoorSet,
                                            *myEngine,
                                            myDoorTiles);

      break;
    }
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

void
RPG_Client_Window_Level::initCeiling()
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Window_Level::initCeiling"));

  // sanity check
  if (myCeilingTile)
  {
#if defined (SDL_USE) || defined (SDL2_USE)
    SDL_FreeSurface (myCeilingTile);
#elif defined (SDL3_USE)
    SDL_DestroySurface (myCeilingTile);
#endif // SDL_USE || SDL2_USE || SDL3_USE
    myCeilingTile = NULL;
  } // end IF

  // load tile for ceiling
  RPG_Graphics_GraphicTypeUnion type;
  type.discriminator = RPG_Graphics_GraphicTypeUnion::TILEGRAPHIC;
  type.tilegraphic = TILE_CEILING;
  myCeilingTile = RPG_Graphics_Common_Tools::loadGraphic (type,   // tile
                                                          true,   // convert to display format
                                                          false); // don't cache
  if (!myCeilingTile)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to RPG_Graphics_Common_Tools::loadGraphic(\"%s\"), aborting\n"),
                ACE_TEXT (RPG_Graphics_Common_Tools::toString (type).c_str ())));
    return;
  } // end IF

  SDL_Surface* shaded_ceiling = NULL;
  shaded_ceiling =
      RPG_Graphics_Surface::alpha (*myCeilingTile,
                                   static_cast<Uint8> ((RPG_GRAPHICS_TILE_WALL_DEF_NW_OPACITY * SDL_ALPHA_OPAQUE)));
  if (!shaded_ceiling)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to RPG_Graphics_Surface::alpha(%u), aborting\n"),
                static_cast<Uint8> ((RPG_GRAPHICS_TILE_WALL_DEF_NW_OPACITY * SDL_ALPHA_OPAQUE))));
#if defined (SDL_USE) || defined (SDL2_USE)
    SDL_FreeSurface (myCeilingTile);
#elif defined (SDL3_USE)
    SDL_DestroySurface (myCeilingTile);
#endif // SDL_USE || SDL2_USE || SDL3_USE
    myCeilingTile = NULL;
    return;
  } // end IF

#if defined (SDL_USE) || defined (SDL2_USE)
  SDL_FreeSurface (myCeilingTile);
#elif defined (SDL3_USE)
  SDL_DestroySurface (myCeilingTile);
#endif // SDL_USE || SDL2_USE || SDL3_USE
  myCeilingTile = shaded_ceiling;
}

void
RPG_Client_Window_Level::initWallBlend (bool halfHeightWalls_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Window_Level::initWallBlend"));

  // sanity check
  if (myWallBlend)
  {
#if defined (SDL_USE) || defined (SDL2_USE)
    SDL_FreeSurface (myWallBlend);
#elif defined (SDL3_USE)
    SDL_DestroySurface (myWallBlend);
#endif // SDL_USE || SDL2_USE || SDL3_USE
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

#if defined (SDL_USE) || defined (SDL2_USE)
  if (SDL_FillRect (myWallBlend,
                    NULL,
                    RPG_Graphics_SDL_Tools::getColor (COLOR_BLACK_A10,
                                                      *myWallBlend->format,
                                                      1.0f)))
#elif defined (SDL3_USE)
  if (SDL_FillSurfaceRect (myWallBlend,
                           NULL,
                           RPG_Graphics_SDL_Tools::getColor (COLOR_BLACK_A10,
                                                             *myWallBlend->format,
                                                             1.0f)))
#endif // SDL_USE || SDL2_USE || SDL3_USE
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_FillRect(): %s, returning\n"),
                ACE_TEXT (SDL_GetError ())));
#if defined (SDL_USE) || defined (SDL2_USE)
    SDL_FreeSurface (myWallBlend);
#elif defined (SDL3_USE)
    SDL_DestroySurface (myWallBlend);
#endif // SDL_USE || SDL2_USE || SDL3_USE
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
  message_window->initialize (inherited::screenLock_, // screen lock handle
                              RPG_CLIENT_MESSAGE_FONT,
                              RPG_CLIENT_MESSAGE_DEF_NUM_LINES);
  message_window->initializeSDL (inherited::renderer_,
                                 inherited::screen_,
                                 inherited::GLContext_);

  return true;
}
