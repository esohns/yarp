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

#include "rpg_client_defines.h"
#include "rpg_client_engine.h"
#include "rpg_client_common_tools.h"
#include "rpg_client_entity_manager.h"
#include "rpg_client_window_minimap.h"

#include <rpg_engine_level.h>
#include <rpg_engine_command.h>
#include <rpg_engine_common_tools.h>

#include <rpg_graphics_defines.h>
#include <rpg_graphics_surface.h>
#include <rpg_graphics_cursor_manager.h>
#include <rpg_graphics_common_tools.h>
#include <rpg_graphics_SDL_tools.h>

#include <rpg_map_common_tools.h>
#include <rpg_map_pathfinding_tools.h>

#include <rpg_common_macros.h>

#include <ace/Log_Msg.h>

#include <sstream>

RPG_Client_WindowLevel::RPG_Client_WindowLevel(const RPG_Graphics_SDLWindowBase& parent_in)
 : inherited(WINDOW_MAP,           // type
             parent_in,            // parent
             std::make_pair(0, 0), // offset
             std::string()),       // title
//             NULL),                // background
   myEngine(NULL),
   myClient(NULL),
//   myClientAction(),
   myDrawMinimap(RPG_CLIENT_DEF_MINIMAP_ISON),
//   myCurrentMapStyle(mapStyle_in),
//   myCurrentFloorSet(),
//   myCurrentFloorEdgeSet(),
//   myCurrentWallSet(),
   myCurrentCeilingTile(NULL),
//   myCurrentDoorSet(),
   myCurrentOffMapTile(NULL),
//   myFloorEdgeTiles(),
//   myWallTiles(),
//   myDoorTiles(),
   myWallBlend(NULL),
   myView(std::make_pair(0, 0))
{
  RPG_TRACE(ACE_TEXT("RPG_Client_WindowLevel::RPG_Client_WindowLevel"));

  // init client action
  myClientAction.command = RPG_CLIENT_COMMAND_INVALID;
  myClientAction.position = std::make_pair(std::numeric_limits<unsigned int>::max(),
                                           std::numeric_limits<unsigned int>::max());
  myClientAction.window = this;
  myClientAction.cursor = RPG_GRAPHICS_CURSOR_INVALID;
  myClientAction.entity_id = 0;
  myClientAction.path.clear();

  // init style
  myCurrentMapStyle.door_style = RPG_GRAPHICS_DOORSTYLE_INVALID;
  myCurrentMapStyle.edge_style = RPG_GRAPHICS_EDGESTYLE_INVALID;
  myCurrentMapStyle.floor_style = RPG_GRAPHICS_FLOORSTYLE_INVALID;
  myCurrentMapStyle.half_height_walls = RPG_CLIENT_DEF_GRAPHICS_WALLSTYLE_HALF;
  myCurrentMapStyle.wall_style = RPG_GRAPHICS_WALLSTYLE_INVALID;
  //   RPG_Graphics_Common_Tools::loadDoorTileSet(myCurrentMapStyle.door_style,
  //                                              myCurrentDoorSet);
  //   RPG_Graphics_Common_Tools::loadFloorEdgeTileSet(myCurrentMapStyle.edge_style,
  //                                                   myCurrentFloorEdgeSet);
  //   RPG_Graphics_Common_Tools::loadFloorTileSet(myCurrentMapStyle.floor_style,
  //                                               myCurrentFloorSet);
  //   RPG_Graphics_Common_Tools::loadWallTileSet(myCurrentMapStyle.wall_style,
  //                                              myCurrentMapStyle.half_height_walls,
  //                                              myCurrentWallSet);

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
  myCurrentOffMapTile = RPG_Graphics_Common_Tools::loadGraphic(type,   // tile
                                                               true,   // convert to display format
                                                               false); // don't cache
  if (!myCurrentOffMapTile)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Graphics_Common_Tools::loadGraphic(\"%s\"), continuing\n"),
               RPG_Graphics_Common_Tools::typeToString(type).c_str()));
}

RPG_Client_WindowLevel::~RPG_Client_WindowLevel()
{
  RPG_TRACE(ACE_TEXT("RPG_Client_WindowLevel::~RPG_Client_WindowLevel"));

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

  if (myCurrentCeilingTile)
    SDL_FreeSurface(myCurrentCeilingTile);

  if (myCurrentOffMapTile)
    SDL_FreeSurface(myCurrentOffMapTile);
}

void
RPG_Client_WindowLevel::setView(const RPG_Map_Position_t& view_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_WindowLevel::setView"));

  ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

  myView = view_in;
}

void
RPG_Client_WindowLevel::setView(const int& offsetX_in,
                                const int& offsetY_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_WindowLevel::setView"));

  RPG_Map_Size_t size = myEngine->getSize();

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
RPG_Client_WindowLevel::getView() const
{
  RPG_TRACE(ACE_TEXT("RPG_Client_WindowLevel::getView"));

  RPG_Graphics_Position_t result;
  {
    ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

    result = myView;
  } // end lock scope

  return result;
}

void
RPG_Client_WindowLevel::toggleMiniMap()
{
  RPG_TRACE(ACE_TEXT("RPG_Client_WindowLevel::toggleMiniMap"));

  ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

  myDrawMinimap = !myDrawMinimap;
}

void
RPG_Client_WindowLevel::toggleDoor(const RPG_Map_Position_t& position_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_WindowLevel::toggleDoor"));

  ACE_ASSERT(myEngine);

  const RPG_Map_Door_t& door = myEngine->getDoor(position_in);
  ACE_ASSERT(door.position == position_in);

  // change tile accordingly
  RPG_Graphics_Orientation orientation = RPG_GRAPHICS_ORIENTATION_INVALID;
  orientation = RPG_Engine_Common_Tools::getDoorOrientation(*myEngine,
                                                            position_in);

  ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

  switch (orientation)
  {
    case ORIENTATION_HORIZONTAL:
      myDoorTiles[position_in] = (door.is_open ? myCurrentDoorSet.horizontal_open
                                               : myCurrentDoorSet.horizontal_closed); break;
    case ORIENTATION_VERTICAL:
      myDoorTiles[position_in] = (door.is_open ? myCurrentDoorSet.vertical_open
                                               : myCurrentDoorSet.vertical_closed); break;
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid door orientation \"%s\", aborting\n"),
                 RPG_Graphics_OrientationHelper::RPG_Graphics_OrientationToString(orientation).c_str()));

      return;
    }
  } // end SWITCH
}

void
RPG_Client_WindowLevel::init(RPG_Client_Engine* engine_in,
                             RPG_Engine_Level* levelState_in,
                             const RPG_Graphics_MapStyle_t& mapStyle_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_WindowLevel::init"));

  // sanity checks
  ACE_ASSERT(engine_in);
  ACE_ASSERT(levelState_in);

  myClient = engine_in;
  myEngine = levelState_in;

  // init style
  RPG_Graphics_StyleUnion style;
  style.discriminator = RPG_Graphics_StyleUnion::FLOORSTYLE;
  style.floorstyle = mapStyle_in.floor_style;
  setStyle(style);
  style.discriminator = RPG_Graphics_StyleUnion::EDGESTYLE;
  style.edgestyle = mapStyle_in.edge_style;
  setStyle(style);
  style.discriminator = RPG_Graphics_StyleUnion::WALLSTYLE;
  style.wallstyle = mapStyle_in.wall_style;
  setStyle(style);
  style.discriminator = RPG_Graphics_StyleUnion::DOORSTYLE;
  style.doorstyle = mapStyle_in.door_style;
  setStyle(style);

  // init edge, wall, door tiles
  init();

  // init minimap
  initMiniMap();
}

void
RPG_Client_WindowLevel::init()
{
  RPG_TRACE(ACE_TEXT("RPG_Client_WindowLevel::init"));

  ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

  // clean up
  myFloorEdgeTiles.clear();
  myWallTiles.clear();
  myDoorTiles.clear();

  // init tiles / position
  RPG_Client_Common_Tools::initFloorEdges(myEngine->getFloorPlan(),
                                          myCurrentFloorEdgeSet,
                                          myFloorEdgeTiles);
  RPG_Client_Common_Tools::initWalls(myEngine->getFloorPlan(),
                                     myCurrentWallSet,
                                     myWallTiles);
  RPG_Client_Common_Tools::initDoors(myEngine->getFloorPlan(),
                                     *myEngine,
                                     myCurrentDoorSet,
                                     myDoorTiles);

  //// init view (--> center)
  //RPG_Map_Size_t size = myEngine->getSize();
  //myView = myEngine->getSize();
  //myView.first >>= 1;
  //myView.second >>= 1;

  //// *NOTE*: fiddling with the view invalidates the cursor/highlight BG !
  //myClientAction.command = COMMAND_CURSOR_INVALIDATE_BG;
  //myClient->action(myClientAction);
  //myClientAction.command = COMMAND_TILE_HIGHLIGHT_INVALIDATE_BG;
  //myClientAction.position = std::make_pair(std::numeric_limits<unsigned int>::max(),
  //                                         std::numeric_limits<unsigned int>::max());
  //myClient->action(myClientAction);
}

void
RPG_Client_WindowLevel::draw(SDL_Surface* targetSurface_in,
                             const unsigned int& offsetX_in,
                             const unsigned int& offsetY_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_WindowLevel::draw"));

  // set target surface
  SDL_Surface* targetSurface = (targetSurface_in ? targetSurface_in : myScreen);

  // sanity check(s)
  ACE_ASSERT(myCurrentOffMapTile);
  ACE_ASSERT(myCurrentCeilingTile);
  ACE_ASSERT(targetSurface);
  ACE_ASSERT(static_cast<int>(offsetX_in) <= targetSurface->w);
  ACE_ASSERT(static_cast<int>(offsetY_in) <= targetSurface->h);

  // init clipping
  clip(targetSurface,
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
  RPG_Graphics_Position_t top_right = std::make_pair(0, 0);
//   top_right.first = (((-RPG_GRAPHICS_TILE_HEIGHT_MOD * ((targetSurface->w / 2) + 50)) +
//                       (RPG_GRAPHICS_TILE_WIDTH_MOD * ((targetSurface->h / 2) + 50)) +
//                       (RPG_GRAPHICS_TILE_WIDTH_MOD * RPG_GRAPHICS_TILE_HEIGHT_MOD)) /
//                      (2 * RPG_GRAPHICS_TILE_WIDTH_MOD * RPG_GRAPHICS_TILE_HEIGHT_MOD));
//   top_right.second = (((RPG_GRAPHICS_TILE_HEIGHT_MOD * ((targetSurface->w / 2) + 50)) +
//                        (RPG_GRAPHICS_TILE_WIDTH_MOD * ((targetSurface->h / 2) + 50)) +
//                        (RPG_GRAPHICS_TILE_WIDTH_MOD * RPG_GRAPHICS_TILE_HEIGHT_MOD)) /
//                       (2 * RPG_GRAPHICS_TILE_WIDTH_MOD * RPG_GRAPHICS_TILE_HEIGHT_MOD));
  top_right.first = (((-RPG_GRAPHICS_TILE_HEIGHT_MOD * ((targetSurface->w / 2))) +
                      (RPG_GRAPHICS_TILE_WIDTH_MOD * ((targetSurface->h / 2))) +
                      (RPG_GRAPHICS_TILE_WIDTH_MOD * RPG_GRAPHICS_TILE_HEIGHT_MOD)) /
                     (2 * RPG_GRAPHICS_TILE_WIDTH_MOD * RPG_GRAPHICS_TILE_HEIGHT_MOD));
  top_right.second = (((RPG_GRAPHICS_TILE_HEIGHT_MOD * ((targetSurface->w / 2))) +
                       (RPG_GRAPHICS_TILE_WIDTH_MOD * ((targetSurface->h / 2))) +
                       (RPG_GRAPHICS_TILE_WIDTH_MOD * RPG_GRAPHICS_TILE_HEIGHT_MOD)) /
                      (2 * RPG_GRAPHICS_TILE_WIDTH_MOD * RPG_GRAPHICS_TILE_HEIGHT_MOD));

  // *NOTE*: without the "+-1" small corners within the viewport are not drawn
  int diff = top_right.first - top_right.second - 1;
  int sum = top_right.first + top_right.second + 1;

  // draw tiles
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

  {
    ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

    int i, j;
    RPG_Client_SignedPosition_t current_map_position = std::make_pair(0, 0);
    RPG_Graphics_FloorTilesConstIterator_t floor_iterator = myCurrentFloorSet.tiles.begin();
    RPG_Graphics_FloorTilesConstIterator_t begin_row = myCurrentFloorSet.tiles.begin();
    unsigned int floor_column_index = 0;
    RPG_Graphics_Position_t screen_position = std::make_pair(0, 0);
    RPG_Map_Size_t map_size = myEngine->getSize();
  //   // debug info
  //   SDL_Rect rect;
  //   std::ostringstream converter;
  //   std::string tile_text;
  //   RPG_Graphics_TextSize_t tile_text_size;
  //   RPG_Position_t map_position = std::make_pair(0, 0);
    RPG_Graphics_FloorEdgeTileMapIterator_t floor_edge_iterator = myFloorEdgeTiles.end();

    // "clear" map "window"
    clear();

    // pass 1
    for (i = -static_cast<int>(top_right.second);
         i <= static_cast<int>(top_right.second);
         i++)
    {
      current_map_position.second = myView.second + i;
      // off the map ? --> continue
      if ((current_map_position.second < 0) ||
          (current_map_position.second >= static_cast<int>(map_size.second)))
        continue;

      // floor tile rotation
      begin_row = myCurrentFloorSet.tiles.begin();
      //std::advance(begin_row, (current_map_position.second % myCurrentFloorSet.rows) * myCurrentFloorSet.columns);
      std::advance(begin_row, current_map_position.second % (myCurrentFloorSet.tiles.size() / myCurrentFloorSet.columns));

      for (j = diff + i;
           (j + i) <= sum;
           j++)
      {
        current_map_position.first = myView.first + j;
        // off the map ? --> continue
        if ((current_map_position.first < 0) ||
            (current_map_position.first >= static_cast<int>(map_size.first)))
          continue;

        // floor tile rotation
        floor_iterator = begin_row;
        //std::advance(floor_iterator, current_map_position.first % myCurrentFloorSet.columns);
        std::advance(floor_iterator, (myCurrentFloorSet.rows * (current_map_position.first % myCurrentFloorSet.columns)));

        // map --> screen coordinates
  //       x = (targetSurface->w / 2) + (RPG_GRAPHICS_TILE_WIDTH_MOD * (j - i));
  //       y = (targetSurface->h / 2) + (RPG_GRAPHICS_TILE_HEIGHT_MOD * (j + i));
        screen_position = RPG_Graphics_Common_Tools::map2Screen(current_map_position,
                                                                mySize,
                                                                myView);

        // step1: unmapped areas
        if ((myEngine->getElement(current_map_position) == MAPELEMENT_UNMAPPED) ||
            // *NOTE*: walls are drawn together with the floor...
            (myEngine->getElement(current_map_position) == MAPELEMENT_WALL))
        {
          RPG_Graphics_Surface::put(screen_position.first,
                                    screen_position.second,
                                    *myCurrentOffMapTile,
                                    targetSurface);

  //         // off the map ? --> continue
  //         if ((current_map_position.second < 0) ||
  //             (current_map_position.second >= myEngine.getDimensions().second) ||
  //             (current_map_position.first < 0) ||
  //             (current_map_position.first >= myEngine.getDimensions().first))

          // advance floor iterator
          //std::advance(floor_iterator, myCurrentFloorSet.rows);

          continue;
        } // end IF

        // step2: floor
        if ((myEngine->getElement(current_map_position) == MAPELEMENT_FLOOR) ||
            (myEngine->getElement(current_map_position) == MAPELEMENT_DOOR))
        {
          RPG_Graphics_Surface::put(screen_position.first,
                                    screen_position.second,
                                    *(*floor_iterator).surface,
                                    targetSurface);

  //         // debug info
  //         rect.x = x;
  //         rect.y = y;
  //         rect.x = screen_position.first;
  //         rect.y = screen_position.second;
  //         rect.w = (*floor_iterator).surface->w;
  //         rect.h = (*floor_iterator).surface->h;
  //         RPG_Graphics_Surface::putRect(rect,                              // rectangle
  //                                       RPG_GRAPHICS_WINDOW_HOTSPOT_COLOR, // color
  //                                       targetSurface);                    // target surface
  //         converter.str(ACE_TEXT(""));
  //         converter.clear();
  //         tile_text = ACE_TEXT("[");
  //         converter << current_map_position.first;
  //         tile_text += converter.str();
  //         tile_text += ACE_TEXT(",");
  //         converter.str(ACE_TEXT(""));
  //         converter.clear();
  //         converter << current_map_position.second;
  //         tile_text += converter.str();
  //         tile_text += ACE_TEXT("]");
  //         tile_text_size = RPG_Graphics_Common_Tools::textSize(TYPE_FONT_MAIN_SMALL,
  //                                                              tile_text);
  //         RPG_Graphics_Surface::putText(TYPE_FONT_MAIN_SMALL,
  //                                       tile_text,
  //                                       RPG_Graphics_SDL_Tools::colorToSDLColor(RPG_GRAPHICS_FONT_DEF_COLOR,
  //                                                                               *targetSurface),
  //                                       true, // add shade
  //                                       RPG_Graphics_SDL_Tools::colorToSDLColor(RPG_GRAPHICS_FONT_DEF_SHADECOLOR,
  //                                                                               *targetSurface),
  //                                       (rect.x + ((rect.w - tile_text_size.first) / 2)),
  //                                       (rect.y + ((rect.h - tile_text_size.second) / 2)),
  //                                       targetSurface);

          // step3: floor edges
          floor_edge_iterator = myFloorEdgeTiles.find(current_map_position);
          if (floor_edge_iterator != myFloorEdgeTiles.end())
          {
            // straight edges
            if ((*floor_edge_iterator).second.west.surface)
              RPG_Graphics_Surface::put(screen_position.first +
                                        (*floor_edge_iterator).second.west.offset_x,
                                        (screen_position.second -
                                         (*floor_edge_iterator).second.west.surface->h +
                                         (RPG_GRAPHICS_TILE_FLOOR_HEIGHT / 2) +
                                         (*floor_edge_iterator).second.west.offset_y),
                                        *(myCurrentFloorEdgeSet.west.surface),
                                        targetSurface);
            if ((*floor_edge_iterator).second.north.surface)
              RPG_Graphics_Surface::put((screen_position.first +
                                         (RPG_GRAPHICS_TILE_FLOOR_WIDTH / 2) +
                                         (*floor_edge_iterator).second.north.offset_x),
                                        (screen_position.second -
                                         (*floor_edge_iterator).second.north.surface->h +
                                         (RPG_GRAPHICS_TILE_FLOOR_HEIGHT / 2) +
                                         (*floor_edge_iterator).second.north.offset_y),
                                        *(myCurrentFloorEdgeSet.north.surface),
                                        targetSurface);
            if ((*floor_edge_iterator).second.east.surface)
              RPG_Graphics_Surface::put(screen_position.first +
                                        (RPG_GRAPHICS_TILE_FLOOR_WIDTH / 2) +
                                        (*floor_edge_iterator).second.east.offset_x,
                                        (screen_position.second -
                                         (*floor_edge_iterator).second.east.surface->h +
                                         (RPG_GRAPHICS_TILE_FLOOR_HEIGHT / 2) +
                                         (*floor_edge_iterator).second.east.offset_y),
                                        *(myCurrentFloorEdgeSet.east.surface),
                                        targetSurface);
            if ((*floor_edge_iterator).second.south.surface)
              RPG_Graphics_Surface::put((screen_position.first +
                                        (*floor_edge_iterator).second.south.offset_x),
                                        (screen_position.second -
                                         (*floor_edge_iterator).second.south.surface->h +
                                          (RPG_GRAPHICS_TILE_FLOOR_HEIGHT / 2) +
                                          (*floor_edge_iterator).second.south.offset_y),
                                        *(myCurrentFloorEdgeSet.south.surface),
                                        targetSurface);
            // corners
            if ((*floor_edge_iterator).second.south_west.surface)
              RPG_Graphics_Surface::put((screen_position.first +
                                         (*floor_edge_iterator).second.south_west.offset_x),
                                        (screen_position.second -
                                         (*floor_edge_iterator).second.south_west.surface->h +
                                         (RPG_GRAPHICS_TILE_FLOOR_HEIGHT / 2) +
                                         (*floor_edge_iterator).second.south_west.offset_y),
                                        *(myCurrentFloorEdgeSet.south_west.surface),
                                        targetSurface);
            if ((*floor_edge_iterator).second.north_west.surface)
              RPG_Graphics_Surface::put((screen_position.first +
                                         (RPG_GRAPHICS_TILE_FLOOR_WIDTH / 2) +
                                         (*floor_edge_iterator).second.north_west.offset_x),
                                        (screen_position.second -
                                         (*floor_edge_iterator).second.north_west.surface->h +
                                         (RPG_GRAPHICS_TILE_FLOOR_HEIGHT / 2) +
                                         (*floor_edge_iterator).second.north_west.offset_y),
                                        *(myCurrentFloorEdgeSet.north_west.surface),
                                        targetSurface);
            if ((*floor_edge_iterator).second.south_east.surface)
              RPG_Graphics_Surface::put((screen_position.first +
                                         (RPG_GRAPHICS_TILE_FLOOR_WIDTH / 2) +
                                         (*floor_edge_iterator).second.south_east.offset_x),
                                        (screen_position.second -
                                         (*floor_edge_iterator).second.south_east.surface->h +
                                         (RPG_GRAPHICS_TILE_FLOOR_HEIGHT / 2) +
                                         (*floor_edge_iterator).second.south_east.offset_y),
                                        *(myCurrentFloorEdgeSet.south_east.surface),
                                        targetSurface);
            if ((*floor_edge_iterator).second.north_east.surface)
              RPG_Graphics_Surface::put((screen_position.first +
                                         (RPG_GRAPHICS_TILE_FLOOR_WIDTH / 2) +
                                         (*floor_edge_iterator).second.north_east.offset_x),
                                        (screen_position.second -
                                         (*floor_edge_iterator).second.north_east.surface->h +
                                         (RPG_GRAPHICS_TILE_FLOOR_HEIGHT / 2) +
                                         (*floor_edge_iterator).second.north_east.offset_y),
                                        *(myCurrentFloorEdgeSet.north_east.surface),
                                        targetSurface);
            // (square) corners
            if ((*floor_edge_iterator).second.top.surface)
              RPG_Graphics_Surface::put((screen_position.first +
                                         (*floor_edge_iterator).second.top.offset_x),
                                        (screen_position.second -
                                         (*floor_edge_iterator).second.top.surface->h +
                                         (RPG_GRAPHICS_TILE_FLOOR_HEIGHT / 2) +
                                        (*floor_edge_iterator).second.top.offset_y),
                                        *(myCurrentFloorEdgeSet.top.surface),
                                        targetSurface);
            if ((*floor_edge_iterator).second.right.surface)
              RPG_Graphics_Surface::put((screen_position.first +
                                         (RPG_GRAPHICS_TILE_FLOOR_WIDTH / 2) +
                                         (*floor_edge_iterator).second.right.offset_x),
                                        (screen_position.second -
                                         (*floor_edge_iterator).second.right.surface->h +
                                         (RPG_GRAPHICS_TILE_FLOOR_HEIGHT / 2) +
                                         (*floor_edge_iterator).second.right.offset_y),
                                        *(myCurrentFloorEdgeSet.right.surface),
                                        targetSurface);
            if ((*floor_edge_iterator).second.left.surface)
              RPG_Graphics_Surface::put((screen_position.first +
                                         (*floor_edge_iterator).second.left.offset_x),
                                        (screen_position.second -
                                         (*floor_edge_iterator).second.left.surface->h +
                                         (RPG_GRAPHICS_TILE_FLOOR_HEIGHT / 2) +
                                         (*floor_edge_iterator).second.left.offset_y),
                                        *(myCurrentFloorEdgeSet.left.surface),
                                        targetSurface);
            if ((*floor_edge_iterator).second.bottom.surface)
              RPG_Graphics_Surface::put((screen_position.first +
                                         (RPG_GRAPHICS_TILE_FLOOR_WIDTH / 2) +
                                         (*floor_edge_iterator).second.bottom.offset_x),
                                        (screen_position.second -
                                         (*floor_edge_iterator).second.bottom.surface->h +
                                         (RPG_GRAPHICS_TILE_FLOOR_HEIGHT / 2) +
                                         (*floor_edge_iterator).second.bottom.offset_y),
                                        *(myCurrentFloorEdgeSet.bottom.surface),
                                        targetSurface);
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
      // off the map ? --> continue
      if ((current_map_position.second < 0) ||
          (current_map_position.second >= static_cast<int>(map_size.second)))
        continue;

      for (j = diff + i;
           (j + i) <= sum;
           j++)
      {
        current_map_position.first = myView.first + j;
        // off the map ? --> continue
        if ((current_map_position.first < 0) ||
            (current_map_position.first >= static_cast<int>(map_size.first)))
          continue;

        // transform map coordinates into screen coordinates
  //       x = (targetSurface->w / 2) + (RPG_GRAPHICS_TILE_WIDTH_MOD * (j - i));
  //       y = (targetSurface->h / 2) + (RPG_GRAPHICS_TILE_HEIGHT_MOD * (j + i));
        screen_position = RPG_Graphics_Common_Tools::map2Screen(current_map_position,
                                                                mySize,
                                                                myView);

        wall_iterator = myWallTiles.find(current_map_position);
        door_iterator = myDoorTiles.find(current_map_position);

        // step1: walls (west & north)
        if (wall_iterator != myWallTiles.end())
        {
          if ((*wall_iterator).second.west.surface)
            RPG_Graphics_Surface::put(screen_position.first,
                                      (screen_position.second -
                                       (*wall_iterator).second.west.surface->h +
                                       (RPG_GRAPHICS_TILE_FLOOR_HEIGHT / 2)),
                                      *(myCurrentWallSet.west.surface),
                                      targetSurface);
          if ((*wall_iterator).second.north.surface)
            RPG_Graphics_Surface::put((screen_position.first +
                                       (RPG_GRAPHICS_TILE_FLOOR_WIDTH / 2)),
                                      (screen_position.second -
                                       (*wall_iterator).second.north.surface->h +
                                       (RPG_GRAPHICS_TILE_FLOOR_HEIGHT / 2)),
                                      *(myCurrentWallSet.north.surface),
                                      targetSurface);
        } // end IF

        // step2: doors
        if (door_iterator != myDoorTiles.end())
        {
          // *NOTE*: doors are drawn in the "middle" of the floor tile
          RPG_Graphics_Surface::put((screen_position.first +
                                     (*door_iterator).second.offset_x +
                                     (RPG_GRAPHICS_TILE_FLOOR_WIDTH / 4)),
                                    (screen_position.second +
                                     (*door_iterator).second.offset_y -
                                     (*door_iterator).second.surface->h +
                                     (RPG_GRAPHICS_TILE_WALL_HEIGHT / 2) -
                                     (RPG_GRAPHICS_TILE_FLOOR_HEIGHT / 4)),
                                    *(*door_iterator).second.surface,
                                    targetSurface);
        } // end IF

        // step3: furniture & traps

        // step4: objects

        // step5: creatures
        entity_id = myEngine->hasEntity(current_map_position);
        if (entity_id)
        {
          // invalidate bg
          RPG_CLIENT_ENTITY_MANAGER_SINGLETON::instance()->invalidateBG(entity_id);

          // draw creature
          SDL_Rect dirtyRegion;
          RPG_CLIENT_ENTITY_MANAGER_SINGLETON::instance()->put(entity_id,
                                                               screen_position,
                                                               targetSurface,
                                                               dirtyRegion);
        } // end IF

        // step6: effects

        // step7: walls (south & east)
        if (wall_iterator != myWallTiles.end())
        {
          if ((*wall_iterator).second.south.surface)
            RPG_Graphics_Surface::put(screen_position.first,
                                      (screen_position.second -
                                       (*wall_iterator).second.south.surface->h +
                                       RPG_GRAPHICS_TILE_FLOOR_HEIGHT),
                                      *(*wall_iterator).second.south.surface,
                                      targetSurface);
          if ((*wall_iterator).second.east.surface)
            RPG_Graphics_Surface::put((screen_position.first +
                                       (RPG_GRAPHICS_TILE_FLOOR_WIDTH / 2)),
                                      (screen_position.second -
                                       (*wall_iterator).second.east.surface->h +
                                       RPG_GRAPHICS_TILE_FLOOR_HEIGHT),
                                      *(*wall_iterator).second.east.surface,
                                      targetSurface);
        } // end IF

        // step8: ceiling
        if (RPG_Engine_Common_Tools::hasCeiling(current_map_position,
                                                *myEngine))
        {
          RPG_Graphics_Surface::put(screen_position.first,
                                    (screen_position.second -
                                     (myCurrentMapStyle.half_height_walls ? (RPG_GRAPHICS_TILE_WALL_HEIGHT / 2)
                                                                          : RPG_GRAPHICS_TILE_WALL_HEIGHT) +
                                     (RPG_GRAPHICS_TILE_FLOOR_HEIGHT / (myCurrentMapStyle.half_height_walls ? 8 : 2))),
                                    *myCurrentCeilingTile,
                                    targetSurface);
        } // end IF
      } // end FOR
    } // end FOR
  } // end lock scope

  // realize any children
  for (RPG_Graphics_WindowsIterator_t iterator = myChildren.begin();
       iterator != myChildren.end();
       iterator++)
  {
    if (((*iterator)->getType() == WINDOW_MINIMAP) &&
        (!myDrawMinimap))
      continue;

    try
    {
      (*iterator)->draw(targetSurface,
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
  SDL_Rect dirtyRegion;
  SDL_GetClipRect(targetSurface, &dirtyRegion);
  invalidate(dirtyRegion);

  // reset clipping area
  unclip(targetSurface);

  // remember position of last realization
  myLastAbsolutePosition = std::make_pair(offsetX_in + myBorderLeft + myOffset.first,
                                          offsetY_in + myBorderTop  + myOffset.second);
}

void
RPG_Client_WindowLevel::handleEvent(const SDL_Event& event_in,
                                    RPG_Graphics_IWindow* window_in,
                                    bool& redraw_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_WindowLevel::handleEvent"));

  // init return value(s)
  redraw_out = false;

  // sanity check(s)
  ACE_ASSERT(myClient);

  bool delegate_to_parent = false;
  switch (event_in.type)
  {
    // *** keyboard ***
    case SDL_KEYDOWN:
    {
//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("%s key\n%s\n"),
//                  ((event_in.type == SDL_KEYDOWN) ? ACE_TEXT("pressed") : ACE_TEXT("released")),
//                  RPG_Graphics_SDL_Tools::keyToString(event_in.key.keysym).c_str()));

      switch (event_in.key.keysym.sym)
      {
        case SDLK_a:
        {
          myClient->centerOnActive(!myClient->getCenterOnActive());

          // adjust view ?
          RPG_Engine_EntityID_t entity_id = myEngine->getActive();
          if (myClient->getCenterOnActive() &&
              entity_id)
          {
            myClientAction.command = COMMAND_SET_VIEW;
            myClientAction.position = myEngine->getPosition(entity_id);
            myClient->action(myClientAction);
          } // end IF

          break;
        }
        // implement keypad navigation
        case SDLK_c:
        {
          // step1: set view
          myClientAction.command = COMMAND_SET_VIEW;
          myClientAction.entity_id = myEngine->getActive();
          if ((event_in.key.keysym.mod & KMOD_SHIFT) ||
              (myClientAction.entity_id == 0))
          {
            // center view
            myClientAction.position = myEngine->getSize();
            myClientAction.position.first >>= 1;
            myClientAction.position.second >>= 1;
          } // end IF
          else
            myClientAction.position = myEngine->getPosition(myClientAction.entity_id);
          myClient->action(myClientAction);

          break;
        }
        case SDLK_m:
        {
          toggleMiniMap();

          // --> need a redraw
          redraw_out = true;

          break;
        }
        // implement (manual) refresh
        case SDLK_r:
        {
          // --> need a redraw
          redraw_out = true;

          break;
        }
        case SDLK_t:
        {
          // step0: restore/clear old tile highlight background
          myClientAction.command = COMMAND_TILE_HIGHLIGHT_RESTORE_BG;
          myClient->action(myClientAction);

          // *NOTE*: this MAY also invalidate the current cursor bg...
          myClientAction.command = COMMAND_CURSOR_INVALIDATE_BG;
          myClient->action(myClientAction);

          // step1: store current background tile(s)
          myClientAction.command = COMMAND_TILE_HIGHLIGHT_STORE_BG;
          RPG_Graphics_Position_t cursor_position = RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->position();
          myClientAction.position = RPG_Graphics_Common_Tools::screen2Map(cursor_position,
                                                                          myEngine->getSize(),
                                                                          getSize(),
                                                                          getView());

          // toggle path selection mode
          if (myClient->hasMode(SELECTIONMODE_PATH))
          {
            // --> switch off path selection

            // clear cached path
            myClientAction.path.clear();

            // on the map ?
            if ((myClientAction.position.first  != std::numeric_limits<unsigned int>::max()) &&
                (myClientAction.position.second != std::numeric_limits<unsigned int>::max()))
            {
              myClient->action(myClientAction);

              // step2: draw tile highlight
              myClientAction.command = COMMAND_TILE_HIGHLIGHT_DRAW;
              myClient->action(myClientAction);
            } // end IF

            myClient->mode(SELECTIONMODE_NORMAL);
          } // end IF
          else
          {
            // --> switch on path selection

            myClientAction.entity_id = myEngine->getActive();
            if (myClientAction.entity_id)
            {
              if (RPG_Engine_Common_Tools::isValid(myClientAction.position, *myEngine))
              {
                RPG_Graphics_Position_t current_position = myEngine->getPosition(myClientAction.entity_id);
                if (current_position != myClientAction.position)
                {
                  if (!myEngine->findPath(current_position,
                                          myClientAction.position,
                                          myClientAction.path))
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
                  //else
                  //{
                  //  ACE_ASSERT(myClientAction.path.front().first == current_position);
                  //  ACE_ASSERT(myClientAction.path.back().first  == myClientAction.position);
                  //}
                } // end IF
              } // end IF
              else
              {
                // pointing at an invalid (==unreachable) position (still on the map)
                // --> erase cached path (and tile highlights)
                myClientAction.path.clear();
              } // end ELSE
            } // end IF

            // on the map ?
            if ((myClientAction.position.first  != std::numeric_limits<unsigned int>::max()) &&
                (myClientAction.position.second != std::numeric_limits<unsigned int>::max()))
            {
              myClient->action(myClientAction);

              // step2: draw tile highlight(s)
              myClientAction.command = COMMAND_TILE_HIGHLIGHT_DRAW;
              myClient->action(myClientAction);
            } // end IF

            myClient->mode(SELECTIONMODE_PATH);
          } // end ELSE

          myClientAction.command = COMMAND_CURSOR_DRAW;
          myClientAction.position = cursor_position;
          myClient->action(myClientAction);

          break;
        }
        // implement keypad navigation
        case SDLK_UP:
        case SDLK_DOWN:
        case SDLK_LEFT:
        case SDLK_RIGHT:
        {
          RPG_Map_Direction direction = DIRECTION_INVALID;
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
            myClientAction.entity_id = myEngine->getActive();
            if (myClientAction.entity_id == 0)
              break; // nothing to do...

            RPG_Engine_Action player_action;
            player_action.command = COMMAND_TRAVEL;
            // compute target position
            player_action.position = myEngine->getPosition(myClientAction.entity_id);
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
                           RPG_Map_Common_Tools::direction2String(direction).c_str()));

                break;
              }
            } // end SWITCH
            player_action.path.clear();
            player_action.target = 0;

            if (RPG_Engine_Common_Tools::isValid(player_action.position, *myEngine))
            {
              myEngine->action(myClientAction.entity_id, player_action);

              // adjust view ?
              if (myClient->getCenterOnActive())
              {
                myClientAction.command = COMMAND_SET_VIEW;
                myClientAction.position = player_action.position;
                myClient->action(myClientAction);
              } // end IF
            } // end IF
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
      myClientAction.position = RPG_Graphics_Common_Tools::screen2Map(std::make_pair(event_in.motion.x,
                                                                                     event_in.motion.y),
                                                                      myEngine->getSize(),
                                                                      getSize(),
                                                                      getView());
//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("mouse position [%u,%u] --> [%u,%u]\n"),
//                  event_in.button.x,
//                  event_in.button.y,
//                  myClientAction.position.first,
//                  myClientAction.position.second));

      // inside map ?
      if ((myClientAction.position.first  == std::numeric_limits<unsigned int>::max()) &&
          (myClientAction.position.second == std::numeric_limits<unsigned int>::max()))
      {
        // --> off the map

        // step1: restore/clear old tile highlight background
        myClientAction.command = COMMAND_TILE_HIGHLIGHT_RESTORE_BG;
        myClient->action(myClientAction);

        // step2: invalidate tile highlight background
        myClientAction.command = COMMAND_TILE_HIGHLIGHT_INVALIDATE_BG;
        myClient->action(myClientAction);

        break;
      } // end IF

      // change "active" tile ?
      RPG_Map_Position_t current_position = myClientAction.position;
      if (myClientAction.position != RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->getHighlightBGPosition(std::numeric_limits<unsigned int>::max()))
      {
        // step1: restore/clear old tile highlight background
        myClientAction.command = COMMAND_TILE_HIGHLIGHT_RESTORE_BG;
        myClient->action(myClientAction);

        // step2: invalidate tile highlight background
        myClientAction.command = COMMAND_TILE_HIGHLIGHT_INVALIDATE_BG;
        myClient->action(myClientAction);

        // *NOTE*: this MAY also invalidate the current cursor bg. As the cursor gets redrawn anyway,
        // we can safely assume it to be so...
        myClientAction.command = COMMAND_CURSOR_INVALIDATE_BG;
        myClient->action(myClientAction);

        // step3: an entity may have moved to a previously active tile in the meantime
        // --> it may need to be redrawn
        for (unsigned int i = 0;
             ;
             i++)
        {
          current_position = RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->getHighlightBGPosition(i);
          if ((current_position.first  == std::numeric_limits<unsigned int>::max()) &&
              (current_position.second == std::numeric_limits<unsigned int>::max()))
            break; // done

          myClientAction.entity_id = myEngine->hasEntity(current_position);
          if (myClientAction.entity_id)
          {
            myClientAction.command = COMMAND_ENTITY_DRAW;
            myClientAction.position = current_position;
            myClient->action(myClientAction);
          } // end IF
        } // end FOR

        // step4: store current background tile(s)
        myClientAction.command = COMMAND_TILE_HIGHLIGHT_STORE_BG;
        myClientAction.entity_id = myEngine->getActive();
        if (myClientAction.entity_id &&
            myClient->hasMode(SELECTIONMODE_PATH))
        {
          if (RPG_Engine_Common_Tools::isValid(myClientAction.position, *myEngine))
          {
            current_position = myEngine->getPosition(myClientAction.entity_id);
            if (current_position != myClientAction.position)
            {
              if (!myEngine->findPath(current_position,
                                      myClientAction.position,
                                      myClientAction.path))
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
              //else
              //{
              //  ACE_ASSERT(myClientAction.path.front().first == current_position);
              //  ACE_ASSERT(myClientAction.path.back().first  == myClientAction.position);
              //}
            } // end IF
          } // end IF
          else
          {
            // pointing at an invalid (==unreachable) position (still on the map)
            // --> erase cached path (and tile highlights)
            myClientAction.path.clear();
          } // end ELSE
        } // end IF
        myClient->action(myClientAction);

        // step5: draw tile highlight(s)
        myClientAction.command = COMMAND_TILE_HIGHLIGHT_DRAW;
        myClient->action(myClientAction);
      } // end IF

      // set an appropriate cursor
      RPG_Graphics_Cursor cursor_type = RPG_Engine_Common_Tools::getCursor(myClientAction.position,
                                                                           *myEngine);
      if ((cursor_type == CURSOR_NORMAL) &&
          myClientAction.entity_id &&
          myClient->hasMode(SELECTIONMODE_PATH) &&
          (current_position != myClientAction.position))
        cursor_type = CURSOR_TRAVEL;
      if (cursor_type != RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->type())
      {
        myClientAction.command = COMMAND_CURSOR_SET;
        myClientAction.cursor = cursor_type;
        myClient->action(myClientAction);
      } // end IF

      break;
    }
    case SDL_MOUSEBUTTONDOWN:
    {
//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("mouse button [%u,%u] pressed\n"),
//                  static_cast<unsigned int>(event_in.button.which),
//                  static_cast<unsigned int>(event_in.button.button)));

      if (event_in.button.button == 1) // left-click
      {
        RPG_Graphics_Position_t map_position = RPG_Graphics_Common_Tools::screen2Map(std::make_pair(event_in.button.x,
                                                                                                    event_in.button.y),
                                                                                     myEngine->getSize(),
                                                                                     getSize(),
                                                                                     getView());

        //ACE_DEBUG((LM_DEBUG,
        //           ACE_TEXT("mouse position [%u,%u] --> map position [%u,%u]\n"),
        //           event_in.button.x,
        //           event_in.button.y,
        //           map_position.first,
        //           map_position.second));
        
        // player standing next to door ?
        myClientAction.entity_id = myEngine->getActive();
        RPG_Engine_Action player_action;
        player_action.command = RPG_ENGINE_COMMAND_INVALID;
        player_action.position = std::make_pair(std::numeric_limits<unsigned int>::max(),
                                                std::numeric_limits<unsigned int>::max());
        player_action.path.clear();
        player_action.target = 0;
        switch (myEngine->getElement(map_position))
        {
          case MAPELEMENT_DOOR:
          {
            const RPG_Map_Door_t& door = myEngine->getDoor(map_position);

            // closed --> (try to) open it
            if (myClientAction.entity_id &&
                (!door.is_open) &&
                RPG_Map_Common_Tools::isAdjacent(myEngine->getPosition(myClientAction.entity_id),
                                                 map_position))
            {
              player_action.command = (door.is_open ? COMMAND_DOOR_CLOSE
                                                    : COMMAND_DOOR_OPEN);
              player_action.position = map_position;
              myEngine->action(myClientAction.entity_id, player_action);

              break;
            } // end IF

            // *WARNING*: falls through !
          }
          case MAPELEMENT_FLOOR:
          {
            if (myClientAction.entity_id &&
                RPG_Engine_Common_Tools::isValid(map_position, *myEngine))
            {
              // monster ?
              player_action.target = myEngine->hasEntity(map_position);
              if (player_action.target &&
                  myEngine->isMonster(player_action.target) &&
                  RPG_Map_Common_Tools::isAdjacent(myEngine->getPosition(myClientAction.entity_id),
                                                   map_position))
              {
                // --> attack monster
                player_action.command = COMMAND_ATTACK;
                player_action.position = map_position;
                myEngine->action(myClientAction.entity_id, player_action);

                break;
              } // end IF

              // (try to) travel to this position
              player_action.command = COMMAND_TRAVEL;
              player_action.position = map_position;
              if (myClient->hasMode(SELECTIONMODE_PATH) &&
                  !myClientAction.path.empty())
              {
                // sanity checks
                ACE_ASSERT(myClientAction.path.front().first == myEngine->getPosition(myClientAction.entity_id));
                ACE_ASSERT(myClientAction.path.back().first == player_action.position);

                // path exists --> reuse it
                player_action.path = myClientAction.path;
                player_action.path.pop_front();
              } // end IF
              myEngine->action(myClientAction.entity_id, player_action);

              if (myClient->hasMode(SELECTIONMODE_PATH))
              {
                // restore/clear old tile highlight background
                myClientAction.command = COMMAND_TILE_HIGHLIGHT_RESTORE_BG;
                myClient->action(myClientAction);
              } // end IF
            } // end IF

            break;
          }
          default:
            break;
        } // end SWITCH
      } // end IF

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
                             redraw_out);
}

void
RPG_Client_WindowLevel::clear()
{
  RPG_TRACE(ACE_TEXT("RPG_Client_WindowLevel::clear"));

  SDL_Rect dstRect = {0, 0, 0, 0};
//  clip();
  SDL_GetClipRect(myScreen, &dstRect);
  if (SDL_FillRect(myScreen,                             // target surface
                   &dstRect,                             // fill area
                   RPG_Graphics_SDL_Tools::CLR32_BLACK)) // color
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_FillRect(): %s, aborting\n"),
               SDL_GetError()));

    return;
  } // end IF
//  unclip();
}

void
RPG_Client_WindowLevel::setStyle(const RPG_Graphics_StyleUnion& style_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_WindowLevel::setStyle"));

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
                   RPG_Graphics_EdgeStyleHelper::RPG_Graphics_EdgeStyleToString(style_in.edgestyle).c_str()));
      } // end IF
      myCurrentMapStyle.edge_style = style_in.edgestyle;

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
                   RPG_Graphics_FloorStyleHelper::RPG_Graphics_FloorStyleToString(style_in.floorstyle).c_str()));
      } // end IF
      myCurrentMapStyle.floor_style = style_in.floorstyle;

      break;
    }
    case RPG_Graphics_StyleUnion::WALLSTYLE:
    {
      RPG_Graphics_Common_Tools::loadWallTileSet(style_in.wallstyle,
                                                 myCurrentMapStyle.half_height_walls,
                                                 myCurrentWallSet);
      // sanity check
      if ((myCurrentWallSet.east.surface == NULL) ||
          (myCurrentWallSet.west.surface == NULL) ||
          (myCurrentWallSet.north.surface == NULL) ||
          (myCurrentWallSet.south.surface == NULL))
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("wall-style \"%s\" is incomplete, aborting\n"),
                   RPG_Graphics_WallStyleHelper::RPG_Graphics_WallStyleToString(style_in.wallstyle).c_str()));

        return;
      } // end IF

      initWallBlend(myCurrentMapStyle.half_height_walls);

      // *NOTE*: west is just a "darkened" version of east...
      SDL_Surface* copy = NULL;
      copy = RPG_Graphics_Surface::copy(*myCurrentWallSet.east.surface);
      if (!copy)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to RPG_Graphics_Surface::copy(), aborting\n")));

        return;
      } // end IF
      if (SDL_BlitSurface(myWallBlend,
                          NULL,
                          copy,
                          NULL))
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to SDL_BlitSurface(): %s, aborting\n"),
                   SDL_GetError()));

        // clean up
        SDL_FreeSurface(copy);

        return;
      } // end IF
      SDL_FreeSurface(myCurrentWallSet.west.surface);
      myCurrentWallSet.west.surface = copy;

      // *NOTE*: north is just a "darkened" version of south...
      copy = RPG_Graphics_Surface::copy(*myCurrentWallSet.south.surface);
      if (!copy)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to RPG_Graphics_Surface::copy(), aborting\n")));

        return;
      } // end IF
      if (SDL_BlitSurface(myWallBlend,
                          NULL,
                          copy,
                          NULL))
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to SDL_BlitSurface(): %s, aborting\n"),
                   SDL_GetError()));

        // clean up
        SDL_FreeSurface(copy);

        return;
      } // end IF
      SDL_FreeSurface(myCurrentWallSet.north.surface);
      myCurrentWallSet.north.surface = copy;

      // set wall opacity
      SDL_Surface* shaded_wall = NULL;
      shaded_wall = RPG_Graphics_Surface::shade(*myCurrentWallSet.east.surface,
                                                static_cast<Uint8>((RPG_GRAPHICS_TILE_DEF_WALL_SE_OPACITY * SDL_ALPHA_OPAQUE)));
      if (!shaded_wall)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to RPG_Graphics_Surface::shade(%u), aborting\n"),
                   static_cast<Uint8>((RPG_GRAPHICS_TILE_DEF_WALL_SE_OPACITY * SDL_ALPHA_OPAQUE))));

        return;
      } // end IF
      SDL_FreeSurface(myCurrentWallSet.east.surface);
      myCurrentWallSet.east.surface = shaded_wall;

      shaded_wall = RPG_Graphics_Surface::shade(*myCurrentWallSet.west.surface,
                                                static_cast<Uint8>((RPG_GRAPHICS_TILE_DEF_WALL_NW_OPACITY * SDL_ALPHA_OPAQUE)));
      if (!shaded_wall)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to RPG_Graphics_Surface::shade(%u), aborting\n"),
                   static_cast<Uint8>((RPG_GRAPHICS_TILE_DEF_WALL_NW_OPACITY * SDL_ALPHA_OPAQUE))));

        return;
      } // end IF
      SDL_FreeSurface(myCurrentWallSet.west.surface);
      myCurrentWallSet.west.surface = shaded_wall;

      shaded_wall = RPG_Graphics_Surface::shade(*myCurrentWallSet.south.surface,
                                                static_cast<Uint8>((RPG_GRAPHICS_TILE_DEF_WALL_SE_OPACITY * SDL_ALPHA_OPAQUE)));
      if (!shaded_wall)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to RPG_Graphics_Surface::shade(%u), aborting\n"),
                   static_cast<Uint8>((RPG_GRAPHICS_TILE_DEF_WALL_SE_OPACITY * SDL_ALPHA_OPAQUE))));

        return;
      } // end IF
      SDL_FreeSurface(myCurrentWallSet.south.surface);
      myCurrentWallSet.south.surface = shaded_wall;

      shaded_wall = RPG_Graphics_Surface::shade(*myCurrentWallSet.north.surface,
                                                static_cast<Uint8>((RPG_GRAPHICS_TILE_DEF_WALL_NW_OPACITY * SDL_ALPHA_OPAQUE)));
      if (!shaded_wall)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to RPG_Graphics_Surface::shade(%u), aborting\n"),
                   static_cast<Uint8>((RPG_GRAPHICS_TILE_DEF_WALL_NW_OPACITY * SDL_ALPHA_OPAQUE))));

        return;
      } // end IF
      SDL_FreeSurface(myCurrentWallSet.north.surface);
      myCurrentWallSet.north.surface = shaded_wall;

      // init wall tiles / position
      RPG_Client_Common_Tools::updateWalls(myCurrentWallSet,
                                           myWallTiles);

      myCurrentMapStyle.wall_style = style_in.wallstyle;

      break;
    }
    case RPG_Graphics_StyleUnion::DOORSTYLE:
    {
      RPG_Graphics_Common_Tools::loadDoorTileSet(style_in.doorstyle,
                                                 myCurrentDoorSet);
      // sanity check
      if ((myCurrentDoorSet.horizontal_open.surface == NULL) ||
          (myCurrentDoorSet.vertical_open.surface == NULL) ||
          (myCurrentDoorSet.horizontal_closed.surface == NULL) ||
          (myCurrentDoorSet.vertical_closed.surface == NULL) ||
          (myCurrentDoorSet.broken.surface == NULL))
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("door-style \"%s\" is incomplete, continuing\n"),
                   RPG_Graphics_DoorStyleHelper::RPG_Graphics_DoorStyleToString(style_in.doorstyle).c_str()));
      } // end IF

      // init door tiles / position
      RPG_Client_Common_Tools::updateDoors(myCurrentDoorSet,
                                           *myEngine,
                                           myDoorTiles);

      myCurrentMapStyle.door_style = style_in.doorstyle;

      break;
    }
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid style (was: %d), aborting\n"),
                 style_in.discriminator));

      return;
    }
  } // end SWITCH
}

void
RPG_Client_WindowLevel::initCeiling()
{
  RPG_TRACE(ACE_TEXT("RPG_Client_WindowLevel::initCeiling"));

  // sanity check
  if (myCurrentCeilingTile)
  {
    SDL_FreeSurface(myCurrentCeilingTile);
    myCurrentCeilingTile = NULL;
  } // end IF

  // load tile for ceiling
  RPG_Graphics_GraphicTypeUnion type;
  type.discriminator = RPG_Graphics_GraphicTypeUnion::TILEGRAPHIC;
  type.tilegraphic = TILE_CEILING;
  myCurrentCeilingTile = RPG_Graphics_Common_Tools::loadGraphic(type,   // tile
                                                                true,   // convert to display format
                                                                false); // don't cache
  if (!myCurrentCeilingTile)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Graphics_Common_Tools::loadGraphic(\"%s\"), aborting\n"),
               RPG_Graphics_Common_Tools::typeToString(type).c_str()));

    return;
  } // end IF

  SDL_Surface* shaded_ceiling = NULL;
  shaded_ceiling = RPG_Graphics_Surface::shade(*myCurrentCeilingTile,
                                               static_cast<Uint8>((RPG_GRAPHICS_TILE_DEF_WALL_NW_OPACITY * SDL_ALPHA_OPAQUE)));
  if (!shaded_ceiling)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Graphics_Surface::shade(%u), aborting\n"),
               static_cast<Uint8>((RPG_GRAPHICS_TILE_DEF_WALL_NW_OPACITY * SDL_ALPHA_OPAQUE))));

    // clean up
    SDL_FreeSurface(myCurrentCeilingTile);
    myCurrentCeilingTile = NULL;

    return;
  } // end IF

  SDL_FreeSurface(myCurrentCeilingTile);
  myCurrentCeilingTile = shaded_ceiling;
}

void
RPG_Client_WindowLevel::initWallBlend(const bool& halfHeightWalls_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_WindowLevel::initWallBlend"));

  // sanity check
  if (myWallBlend)
  {
    SDL_FreeSurface(myWallBlend);
    myWallBlend = NULL;
  } // end IF

  myWallBlend = RPG_Graphics_Surface::create(RPG_GRAPHICS_TILE_WALL_WIDTH,
                                             (halfHeightWalls_in ? RPG_GRAPHICS_TILE_WALL_HEIGHT_HALF
                                                                 : RPG_GRAPHICS_TILE_WALL_HEIGHT));
  if (!myWallBlend)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Graphics_Surface::create(%u,%u), aborting\n"),
               RPG_GRAPHICS_TILE_WALL_WIDTH,
               (halfHeightWalls_in ? RPG_GRAPHICS_TILE_WALL_HEIGHT_HALF
                                   : RPG_GRAPHICS_TILE_WALL_HEIGHT)));

    return;
  } // end IF

  if (SDL_FillRect(myWallBlend,
                   NULL,
                   RPG_Graphics_SDL_Tools::CLR32_BLACK_A10))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_FillRect(): %s, aborting\n"),
               SDL_GetError()));

    // clean up
    SDL_FreeSurface(myWallBlend);
    myWallBlend = NULL;

    return;
  } // end IF
}

void
RPG_Client_WindowLevel::initMiniMap()
{
  RPG_TRACE(ACE_TEXT("RPG_Client_WindowLevel::initMiniMap"));

  // sanity check(s)
  ACE_ASSERT(myEngine);
  ACE_ASSERT(inherited::myScreen);

  RPG_Graphics_Offset_t offset;
  offset.first = RPG_CLIENT_DEF_MINIMAP_OFFSET_X;
  offset.second = RPG_CLIENT_DEF_MINIMAP_OFFSET_Y;

  RPG_Client_Window_MiniMap* minimap_window = NULL;
  try
  {
    minimap_window = new RPG_Client_Window_MiniMap(*this,
                                                   offset);
  }
  catch (...)
  {
    ACE_DEBUG((LM_CRITICAL,
               ACE_TEXT("failed to allocate memory(%u): %m, aborting\n"),
               sizeof(RPG_Client_Window_MiniMap)));

    return;
  }
  if (!minimap_window)
  {
    ACE_DEBUG((LM_CRITICAL,
               ACE_TEXT("failed to allocate memory(%u): %m, aborting\n"),
               sizeof(RPG_Client_Window_MiniMap)));

    return;
  } // end IF

  minimap_window->init(myEngine);
  minimap_window->setScreen(inherited::myScreen);
}
