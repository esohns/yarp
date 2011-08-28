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

#include "rpg_client_window_level.h"

#include "rpg_client_defines.h"
#include "rpg_client_engine.h"
#include "rpg_client_common_tools.h"
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

#include <rpg_common_macros.h>

#include <ace/Log_Msg.h>

#include <sstream>

RPG_Client_WindowLevel::RPG_Client_WindowLevel(const RPG_Graphics_SDLWindowBase& parent_in)
 : inherited(WINDOW_MAP,           // type
             parent_in,            // parent
             std::make_pair(0, 0), // offset
             std::string()),       // title
//              NULL),                // background
   myLevelState(NULL),
   myEngine(NULL),
   myDrawMinimap(RPG_CLIENT_DEF_MINIMAP_ISON),
//    myCurrentMapStyle(mapStyle_in),
//    myCurrentFloorSet(),
//    myCurrentFloorEdgeSet(),
//    myCurrentWallSet(),
   myCurrentCeilingTile(NULL),
//    myCurrentDoorSet(),
   myCurrentOffMapTile(NULL),
//    myFloorEdgeTiles(),
//    myWallTiles(),
//    myDoorTiles(),
   myWallBlend(NULL),
   myView(std::make_pair(0, 0)),
   myHighlightBGPosition(std::make_pair(0, 0)),
   myHighlightBG(NULL),
   myHighlightTile(NULL)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_WindowLevel::RPG_Client_WindowLevel"));

  ACE_OS::memset(&myCurrentFloorEdgeSet,
                 0,
                 sizeof(myCurrentFloorEdgeSet));
  ACE_OS::memset(&myCurrentWallSet,
                 0,
                 sizeof(myCurrentWallSet));
  ACE_OS::memset(&myCurrentDoorSet,
                 0,
                 sizeof(myCurrentDoorSet));
//   RPG_Client_Common_Tools::initFloorEdges(myLevelState->getFloorPlan(),
//                                           myCurrentFloorEdgeSet,
//                                           myFloorEdgeTiles);
//   RPG_Client_Common_Tools::initWalls(myLevelState->getFloorPlan(),
//                                      myCurrentWallSet,
//                                      myWallTiles);
//   RPG_Client_Common_Tools::initDoors(myLevelState->getFloorPlan(),
//                                      *myLevelState,
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

  // init cursor highlighting
  myHighlightBG = RPG_Graphics_Surface::create(RPG_GRAPHICS_TILE_FLOOR_WIDTH,
                                               RPG_GRAPHICS_TILE_FLOOR_HEIGHT);
  if (!myHighlightBG)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Graphics_Surface::create(%u,%u), continuing\n"),
               RPG_GRAPHICS_TILE_FLOOR_WIDTH,
               RPG_GRAPHICS_TILE_FLOOR_HEIGHT));
  type.tilegraphic = TILE_CURSOR_HIGHLIGHT;
  myHighlightTile = RPG_Graphics_Common_Tools::loadGraphic(type,   // tile
                                                           true,   // convert to display format
                                                           false); // don't cache
  if (!myHighlightTile)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Graphics_Common_Tools::loadGraphic(\"%s\"), continuing\n"),
               RPG_Graphics_Common_Tools::typeToString(type).c_str()));

  // init style
  myCurrentMapStyle.door_style = RPG_CLIENT_DEF_GRAPHICS_DOORSTYLE;
  myCurrentMapStyle.edge_style = RPG_CLIENT_DEF_GRAPHICS_EDGESTYLE;
  myCurrentMapStyle.floor_style = RPG_CLIENT_DEF_GRAPHICS_FLOORSTYLE;
  myCurrentMapStyle.half_height_walls = RPG_CLIENT_DEF_GRAPHICS_WALLSTYLE_HALF;
  myCurrentMapStyle.wall_style = RPG_CLIENT_DEF_GRAPHICS_WALLSTYLE;
  RPG_Graphics_Common_Tools::loadDoorTileSet(myCurrentMapStyle.door_style,
                                             myCurrentDoorSet);
  RPG_Graphics_Common_Tools::loadFloorEdgeTileSet(myCurrentMapStyle.edge_style,
                                                  myCurrentFloorEdgeSet);
  RPG_Graphics_Common_Tools::loadFloorTileSet(myCurrentMapStyle.floor_style,
                                              myCurrentFloorSet);
  RPG_Graphics_Common_Tools::loadWallTileSet(myCurrentMapStyle.wall_style,
                                             myCurrentMapStyle.half_height_walls,
                                             myCurrentWallSet);
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

  if (myHighlightBG)
    SDL_FreeSurface(myHighlightBG);
  if (myHighlightTile)
    SDL_FreeSurface(myHighlightTile);
}

void
RPG_Client_WindowLevel::setView(const RPG_Map_Position_t& view_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_WindowLevel::setView"));

  myView = view_in;
}

void
RPG_Client_WindowLevel::setView(const int& offsetX_in,
                                const int& offsetY_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_WindowLevel::setView"));

  RPG_Map_Dimensions_t dimensions = myLevelState->getDimensions();

  // handle over-/underruns
  if ((offsetX_in < 0) &&
      (static_cast<unsigned long>(-offsetX_in) > myView.first))
    myView.first = 0;
  else
    myView.first += offsetX_in;

  if ((offsetY_in < 0) &&
      (static_cast<unsigned long>(-offsetY_in) > myView.second))
    myView.second = 0;
  else
    myView.second += offsetY_in;

  if (myView.first >= dimensions.first)
    myView.first = (dimensions.first - 1);
  if (myView.second >= dimensions.second)
    myView.second = (dimensions.second - 1);
}

void
RPG_Client_WindowLevel::toggleMiniMap()
{
  RPG_TRACE(ACE_TEXT("RPG_Client_WindowLevel::toggleMiniMap"));

  myDrawMinimap = !myDrawMinimap;
}

void
RPG_Client_WindowLevel::toggleDoor(const RPG_Map_Position_t& position_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_WindowLevel::toggleDoor"));

  RPG_Map_Door_t door = myLevelState->getDoor(position_in);
  ACE_ASSERT(door.position == position_in);

  // change tile accordingly
  RPG_Graphics_Orientation orientation = RPG_GRAPHICS_ORIENTATION_INVALID;
  orientation = RPG_Engine_Common_Tools::getDoorOrientation(*myLevelState,
                                                            position_in);
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

  myEngine = engine_in;
  myLevelState = levelState_in;

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

  // *NOTE*: fiddling with the view invalidates the cursor BG !
  RPG_Client_Action client_action;
  client_action.command = COMMAND_CURSOR_INVALIDATE_BG;
  client_action.map_position = std::make_pair(0, 0);
  client_action.graphics_position = std::make_pair(0, 0);
  client_action.window = this;
  client_action.cursor = RPG_GRAPHICS_CURSOR_INVALID;
  myEngine->action(client_action);

  // clear highlight BG
  RPG_Graphics_Surface::clear(myHighlightBG);

  // init cursor highlighting
  RPG_Map_Dimensions_t dimensions = myLevelState->getDimensions();
  myHighlightBGPosition = std::make_pair(dimensions.first / 2,
                                         dimensions.second / 2);

  // init minimap
  initMiniMap();
}

void
RPG_Client_WindowLevel::init()
{
  RPG_TRACE(ACE_TEXT("RPG_Client_WindowLevel::init"));

  // clean up
  myFloorEdgeTiles.clear();
  myWallTiles.clear();
  myDoorTiles.clear();

  // init tiles / position
  RPG_Client_Common_Tools::initFloorEdges(myLevelState->getFloorPlan(),
                                          myCurrentFloorEdgeSet,
                                          myFloorEdgeTiles);
  RPG_Client_Common_Tools::initWalls(myLevelState->getFloorPlan(),
                                     myCurrentWallSet,
                                     myWallTiles);
  RPG_Client_Common_Tools::initDoors(myLevelState->getFloorPlan(),
                                     *myLevelState,
                                     myCurrentDoorSet,
                                     myDoorTiles);

  // init view
  RPG_Map_Dimensions_t dimensions = myLevelState->getDimensions();
  RPG_Map_Position_t center_position = std::make_pair(dimensions.first / 2,
                                                      dimensions.second / 2);
  setView(center_position);
}

void
RPG_Client_WindowLevel::draw(SDL_Surface* targetSurface_in,
                             const unsigned long& offsetX_in,
                             const unsigned long& offsetY_in)
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

  int i, j;
  RPG_Client_Position_t current_map_position = std::make_pair(0, 0);
  RPG_Graphics_FloorTilesConstIterator_t floor_iterator = myCurrentFloorSet.tiles.begin();
  RPG_Graphics_FloorTilesConstIterator_t begin_row = myCurrentFloorSet.tiles.begin();
  unsigned long floor_row = 0;
  unsigned long floor_index = 0;
//   unsigned long x, y;
  RPG_Client_Position_t screen_position = std::make_pair(0, 0);
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
  for (i = -top_right.second;
       i <= static_cast<int>(top_right.second);
       i++)
  {
    current_map_position.second = myView.second + i;

    // floor tile rotation
    floor_row = (current_map_position.second % (myCurrentFloorSet.tiles.size() / myCurrentFloorSet.rows));
    begin_row = myCurrentFloorSet.tiles.begin();
    std::advance(begin_row, floor_row);

    for (j = diff + i;
         (j + i) <= sum;
         j++)
    {
      current_map_position.first = myView.first + j;

      // off-map ?
      if ((current_map_position.second < 0) ||
          (current_map_position.second >= static_cast<int>(myLevelState->getDimensions().second)) ||
          (current_map_position.first < 0) ||
          (current_map_position.first >= static_cast<int>(myLevelState->getDimensions().first)))
        continue;

      // floor tile rotation
      floor_index = (current_map_position.first % myCurrentFloorSet.rows);
      floor_iterator = begin_row;
      std::advance(floor_iterator, (myCurrentFloorSet.rows * floor_index));

      // map --> screen coordinates
//       x = (targetSurface->w / 2) + (RPG_GRAPHICS_TILE_WIDTH_MOD * (j - i));
//       y = (targetSurface->h / 2) + (RPG_GRAPHICS_TILE_HEIGHT_MOD * (j + i));
      screen_position = RPG_Client_Common_Tools::map2Screen(current_map_position,
                                                            mySize,
                                                            myView);

      // step1: unmapped areas
      if ((myLevelState->getElement(current_map_position) == MAPELEMENT_UNMAPPED) ||
          // *NOTE*: walls are drawn together with the floor...
          (myLevelState->getElement(current_map_position) == MAPELEMENT_WALL))
      {
        RPG_Graphics_Surface::put(screen_position.first,
                                  screen_position.second,
                                  *myCurrentOffMapTile,
                                  targetSurface);

//         // off the map ? --> continue
//         if ((current_map_position.second < 0) ||
//             (current_map_position.second >= myLevelState.getDimensions().second) ||
//             (current_map_position.first < 0) ||
//             (current_map_position.first >= myLevelState.getDimensions().first))

        // advance floor iterator
        std::advance(floor_iterator, myCurrentFloorSet.rows);

        continue;
      } // end IF

      // step2: floor
      if ((myLevelState->getElement(current_map_position) == MAPELEMENT_FLOOR) ||
          (myLevelState->getElement(current_map_position) == MAPELEMENT_DOOR))
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
      std::advance(floor_iterator, myCurrentFloorSet.rows);
    } // end FOR
  } // end FOR

  // pass 2
  RPG_Graphics_WallTileMapIterator_t wall_iterator = myWallTiles.end();
  RPG_Graphics_DoorTileMapIterator_t door_iterator = myDoorTiles.end();
  RPG_Engine_EntityGraphics_t entity_graphics = myLevelState->getGraphics();
  RPG_Engine_EntityGraphicsConstIterator_t creature_iterator;
  for (i = -top_right.second;
       i <= static_cast<int>(top_right.second);
       i++)
  {
    current_map_position.second = myView.second + i;
    // off the map ? --> continue
    if ((current_map_position.second < 0) ||
        (current_map_position.second >= static_cast<int>(myLevelState->getDimensions().second)))
      continue;

    for (j = diff + i;
         (j + i) <= sum;
         j++)
    {
      current_map_position.first = myView.first + j;
      // off the map ? --> continue
      if ((current_map_position.first < 0) ||
          (current_map_position.first >= static_cast<int>(myLevelState->getDimensions().first)))
        continue;

      // transform map coordinates into screen coordinates
//       x = (targetSurface->w / 2) + (RPG_GRAPHICS_TILE_WIDTH_MOD * (j - i));
//       y = (targetSurface->h / 2) + (RPG_GRAPHICS_TILE_HEIGHT_MOD * (j + i));
      screen_position = RPG_Client_Common_Tools::map2Screen(current_map_position,
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
      creature_iterator = entity_graphics.find(current_map_position);
      if (creature_iterator != entity_graphics.end())
      {
        // sanity check
        ACE_ASSERT((*creature_iterator).second);

        // *NOTE*: creatures are drawn in the "middle" of the floor tile
        RPG_Graphics_Surface::put((screen_position.first +
                                   ((RPG_GRAPHICS_TILE_FLOOR_WIDTH - (*creature_iterator).second->w) / 2)),
                                  (screen_position.second +
                                   (RPG_GRAPHICS_TILE_FLOOR_HEIGHT / 2) -
                                   (*creature_iterator).second->h),
                                  *((*creature_iterator).second),
                                  targetSurface);
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
                                              *myLevelState))
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

  // refresh cursor highlight
  screen_position = RPG_Client_Common_Tools::map2Screen(myHighlightBGPosition,
                                                        inherited::mySize,
                                                        myView);
  // grab BG
  // sanity check for underruns
  if ((screen_position.first < targetSurface->w) &&
      (screen_position.second < targetSurface->h))
    RPG_Graphics_Surface::get(screen_position.first,
                              screen_position.second,
                              true, // use (fast) blitting method
                              *targetSurface,
                              *myHighlightBG);

  RPG_Graphics_Surface::put(screen_position.first,
                            screen_position.second,
                            *myHighlightTile,
                            targetSurface);

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
                                          offsetY_in + myBorderTop + myOffset.second);
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
  ACE_ASSERT(myEngine);

  RPG_Engine_Action player_action;
  player_action.command = RPG_ENGINE_COMMAND_INVALID;
  player_action.direction = DIRECTION_INVALID;
  player_action.position = std::make_pair(0, 0);
  RPG_Client_Action client_action;
  client_action.command = RPG_CLIENT_COMMAND_INVALID;
  client_action.map_position = std::make_pair(0, 0);
  client_action.graphics_position = std::make_pair(0, 0);
  client_action.window = this;
  client_action.cursor = RPG_GRAPHICS_CURSOR_INVALID;
  bool do_player_action = false;
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
        // implement keypad navigation
        case SDLK_c:
        {
          RPG_Map_Position_t position;
          if ((event_in.key.keysym.mod & KMOD_SHIFT) ||
              (myLevelState->getActive() == 0))
          {
            // center view
            RPG_Map_Dimensions_t dimensions = myLevelState->getDimensions();
            position.first = dimensions.first / 2;
            position.second = dimensions.second / 2;
          } // end IF
          else
            position = myLevelState->getPosition(myLevelState->getActive());

          setView(position);

          // *NOTE*: fiddling with the view invalidates the cursor BG !
          client_action.command = COMMAND_CURSOR_INVALIDATE_BG;
          myEngine->action(client_action);

          // clear highlight BG
          RPG_Graphics_Surface::clear(myHighlightBG);

          // need a redraw
          redraw_out = true;

          break;
        }
        case SDLK_m:
        {
          toggleMiniMap();

          // need a redraw
          redraw_out = true;

          break;
        }
        // implement (manual) refresh
        case SDLK_r:
        {
          // need a redraw
          redraw_out = true;

          break;
        }
        case SDLK_UP:
        case SDLK_DOWN:
        case SDLK_LEFT:
        case SDLK_RIGHT:
        {
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
                setView(-RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
                        -RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);
              else
                player_action.direction = DIRECTION_UP;

              break;
            }
            case SDLK_DOWN:
            {
              if (event_in.key.keysym.mod & KMOD_SHIFT)
                setView(RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
                        RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);
              else
                player_action.direction = DIRECTION_DOWN;

              break;
            }
            case SDLK_LEFT:
            {
              if (event_in.key.keysym.mod & KMOD_SHIFT)
                setView(-RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
                        RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);
              else
                player_action.direction = DIRECTION_LEFT;

              break;
            }
            case SDLK_RIGHT:
            {
              if (event_in.key.keysym.mod & KMOD_SHIFT)
                setView(RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
                        -RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);
              else
                player_action.direction = DIRECTION_RIGHT;

              break;
            }
            default:
              break;
          } // end SWITCH

          if (!(event_in.key.keysym.mod & KMOD_SHIFT) &&
              myLevelState->getActive())
          {
            player_action.command = COMMAND_STEP;
            do_player_action = true;
          } // end IF
          else
          {
            // *NOTE*: fiddling with the view invalidates the cursor BG !
            client_action.command = COMMAND_CURSOR_INVALIDATE_BG;
            myEngine->action(client_action);

            // clear highlight BG
            RPG_Graphics_Surface::clear(myHighlightBG);

            // need a redraw
            redraw_out = true;
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
      RPG_Graphics_Position_t map_position = RPG_Client_Common_Tools::screen2Map(std::make_pair(event_in.motion.x,
                                                                                                event_in.motion.y),
                                                                                 myLevelState->getDimensions(),
                                                                                 mySize,
                                                                                 myView);
//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("mouse position [%u,%u] --> [%u,%u]\n"),
//                  event_in.button.x,
//                  event_in.button.y,
//                  map_position.first,
//                  map_position.second));

      // (re-)draw "active" tile highlight ?

      // inside map ?
      if (map_position == std::make_pair(std::numeric_limits<unsigned long>::max(),
                                         std::numeric_limits<unsigned long>::max()))
      {
        // off the map --> remove "active" tile highlight
        client_action.command = COMMAND_TILE_HIGHLIGHT_RESTORE_BG;
        myEngine->action(client_action);

        break;
      } // end IF

      // change "active" tile ?
      if (map_position != myHighlightBGPosition)
      {
        // *NOTE*: restore cursor BG first
        client_action.command = COMMAND_CURSOR_RESTORE_BG;
        myEngine->action(client_action);

        // step1: restore/clear old tile highlight background
        client_action.command = COMMAND_TILE_HIGHLIGHT_RESTORE_BG;
        myEngine->action(client_action);

        // step2: store current background
        RPG_Graphics_Position_t tile_position = RPG_Client_Common_Tools::map2Screen(map_position,
                                                            mySize,
                                                            myView);
        // sanity check for underruns
        if ((tile_position.first < static_cast<unsigned long>(myScreen->w)) &&
            (tile_position.second < static_cast<unsigned long>(myScreen->h)))
        {
          client_action.command = COMMAND_TILE_HIGHLIGHT_STORE_BG;
          client_action.graphics_position = tile_position;
          client_action.map_position = map_position;
          myEngine->action(client_action);

          // step3: draw highlight
//           ACE_DEBUG((LM_DEBUG,
//                      ACE_TEXT("highlight @ (%u,%u) --> (%u,%u)\n"),
//                      map_position.first,
//                      map_position.second,
//                      tile_position.first,
//                      tile_position.second));

          client_action.command = COMMAND_TILE_HIGHLIGHT_DRAW;
          myEngine->action(client_action);
        } // end IF
      } // end IF

      // set an appropriate cursor
      RPG_Graphics_Cursor cursor_type = RPG_Engine_Common_Tools::getCursor(map_position,
                                                                           *myLevelState);
      if (cursor_type != RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->type())
      {
        client_action.command = COMMAND_CURSOR_RESTORE_BG;
        myEngine->action(client_action);

        client_action.command = COMMAND_CURSOR_SET;
        client_action.cursor = cursor_type;
        myEngine->action(client_action);
      } // end IF

      break;
    }
    case SDL_MOUSEBUTTONDOWN:
    {
//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("mouse button [%u,%u] pressed\n"),
//                  static_cast<unsigned long> (event_in.button.which),
//                  static_cast<unsigned long> (event_in.button.button)));

      if (event_in.button.button == 1) // left-click
      {
        RPG_Graphics_Position_t map_position = RPG_Client_Common_Tools::screen2Map(std::make_pair(event_in.button.x,
                                                                                                  event_in.button.y),
                                                                                   myLevelState->getDimensions(),
                                                                                   mySize,
                                                                                   myView);

        ACE_DEBUG((LM_DEBUG,
                   ACE_TEXT("mouse position [%u,%u] --> map position [%u,%u]\n"),
                   event_in.button.x,
                   event_in.button.y,
                   map_position.first,
                   map_position.second));

        if (myLevelState->getActive() == 0)
          break;

        // player standing next to door ?
        switch (myLevelState->getElement(map_position))
        {
          case MAPELEMENT_DOOR:
          {
            RPG_Map_Door_t door = myLevelState->getDoor(map_position);

            // closed --> (try to) open it
            if ((!door.is_open) &&
                (RPG_Map_Common_Tools::dist2Positions(myLevelState->getPosition(myLevelState->getActive()),
                                                      map_position) == 1))
            {
              player_action.command = (door.is_open ? COMMAND_DOOR_CLOSE
                                                    : COMMAND_DOOR_OPEN);
              player_action.position = map_position;
              do_player_action = true;

              break;
            } // end IF

            // *WARNING*: falls through !
          }
          case MAPELEMENT_FLOOR:
          {
            // (try to) travel to this position
            player_action.command = COMMAND_TRAVEL;
            player_action.position = map_position;
            do_player_action = true;

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

      // restore/clear highlight BG
      // *CONSIDER*: is this really useful/necessary ?
      client_action.command = COMMAND_TILE_HIGHLIGHT_RESTORE_BG;
      myEngine->action(client_action);

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

  if (do_player_action)
    myLevelState->action(myLevelState->getActive(), player_action);

  if (delegate_to_parent)
    getParent()->handleEvent(event_in,
                             window_in,
                             redraw_out);
}

void
RPG_Client_WindowLevel::clear()
{
  RPG_TRACE(ACE_TEXT("RPG_Client_WindowLevel::clear"));

//   // init clipping
//   clip();

  SDL_Rect dstRect = {0, 0, 0, 0};
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

//   // reset clipping
//   unclip();
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
                                           *myLevelState,
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
  ACE_ASSERT(myLevelState);
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

  minimap_window->init(myLevelState);
  minimap_window->setScreen(inherited::myScreen);
}

void
RPG_Client_WindowLevel::drawHighlight(const RPG_Graphics_Position_t& graphicsPosition_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_WindowLevel::drawHighlight"));

  // sanity check
  ACE_ASSERT(myHighlightTile);
  ACE_ASSERT(myScreen);

  clip();
  RPG_Graphics_Surface::put(graphicsPosition_in.first,
                            graphicsPosition_in.second,
                            *myHighlightTile,
                            myScreen);
  unclip();

  SDL_Rect dirtyRegion, clipRect;
  dirtyRegion.x = graphicsPosition_in.first;
  dirtyRegion.y = graphicsPosition_in.second;
  dirtyRegion.w = myHighlightTile->w;
  dirtyRegion.h = myHighlightTile->h;
  SDL_GetClipRect(myScreen, &clipRect);

//   invalidate(dirtyRegion);
  // *NOTE*: updating straight away reduces ugly smears...
  RPG_Graphics_Surface::update(RPG_Graphics_SDL_Tools::intersect(clipRect, dirtyRegion),
                               myScreen);
}

void
RPG_Client_WindowLevel::storeHighlightBG(const RPG_Map_Position_t& mapPosition_in,
                                         const RPG_Graphics_Position_t& graphicsPosition_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_WindowLevel::storeHighlightBG"));

  // sanity check
  ACE_ASSERT(myScreen);
  ACE_ASSERT(myHighlightBG);

  RPG_Graphics_Surface::get(graphicsPosition_in.first,
                            graphicsPosition_in.second,
                            true, // use (fast) blitting method
                            *myScreen,
                            *myHighlightBG);
  myHighlightBGPosition = mapPosition_in;
}

void
RPG_Client_WindowLevel::restoreHighlightBG()
{
  RPG_TRACE(ACE_TEXT("RPG_Client_WindowLevel::restoreHighlightBG"));

  // sanity check
  ACE_ASSERT(myHighlightBG);
  ACE_ASSERT(myScreen);

  SDL_Rect dirtyRegion, clipRect;
  RPG_Graphics_Position_t tile_position = RPG_Client_Common_Tools::map2Screen(myHighlightBGPosition,
                                                                              mySize,
                                                                              myView);
  // sanity check for underruns
  if ((tile_position.first < static_cast<unsigned long>(myScreen->w)) &&
      (tile_position.second < static_cast<unsigned long>(myScreen->h)))
  {
    RPG_Graphics_Surface::put(tile_position.first,
                              tile_position.second,
                              *myHighlightBG,
                              myScreen);

    dirtyRegion.x = tile_position.first;
    dirtyRegion.y = tile_position.second;
    dirtyRegion.w = myHighlightBG->w;
    dirtyRegion.h = myHighlightBG->h;
    SDL_GetClipRect(myScreen, &clipRect);
//       invalidate(dirtyRegion);
    // *NOTE*: updating straight away reduces ugly smears...
    RPG_Graphics_Surface::update(RPG_Graphics_SDL_Tools::intersect(clipRect, dirtyRegion),
                                 myScreen);
  } // end IF

  // clear highlight BG
  RPG_Graphics_Surface::clear(myHighlightBG);
}
