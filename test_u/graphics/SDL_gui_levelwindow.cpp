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

#include "SDL_gui_levelwindow.h"

#include "SDL_gui_defines.h"
#include "SDL_gui_minimapwindow.h"

#include <rpg_client_defines.h>
#include <rpg_client_entity_manager.h>
#include <rpg_client_common_tools.h>

#include <rpg_engine.h>
#include <rpg_engine_common_tools.h>

#include <rpg_graphics_defines.h>
#include <rpg_graphics_surface.h>
#include <rpg_graphics_cursor_manager.h>
#include <rpg_graphics_common_tools.h>
#include <rpg_graphics_SDL_tools.h>

#include <rpg_map_common_tools.h>

#include <rpg_common_macros.h>
#include <rpg_common_defines.h>

#include <ace/Log_Msg.h>

#include <sstream>

SDL_GUI_LevelWindow::SDL_GUI_LevelWindow(const RPG_Graphics_SDLWindowBase& parent_in,
                                         RPG_Engine* engine_in)
 : inherited(WINDOW_MAP,           // type
             parent_in,            // parent
             std::make_pair(0, 0), // offset
             std::string()),       // title
//              NULL),                // background
   myEngine(engine_in),
   myDebug(SDL_GUI_DEF_DEBUG),
//    myCurrentMapStyle(),
//    myCurrentFloorSet(),
//    myCurrentWallSet(),
   myCurrentCeilingTile(NULL),
//    myCurrentDoorSet(),
   myCurrentOffMapTile(NULL),
//    myFloorEdgeTiles(),
//    myWallTiles(),
   myHideFloor(false),
   myHideWalls(false),
   myWallBlend(NULL),
   myView(std::make_pair(myEngine->getSize().first / 2,
                         myEngine->getSize().second / 2)),
   myHighlightBGPosition(std::make_pair(myEngine->getSize().first / 2,
                                        myEngine->getSize().second / 2)),
   myHighlightBG(NULL),
   myHighlightTile(NULL),
   myMinimapIsOn(SDL_GUI_DEF_GRAPHICS_MINIMAP_ISON)
{
  RPG_TRACE(ACE_TEXT("SDL_GUI_LevelWindow::SDL_GUI_LevelWindow"));

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

  // init tiles / position
  ACE_OS::memset(&myCurrentFloorEdgeSet,
                 0,
                 sizeof(myCurrentFloorEdgeSet));
  ACE_OS::memset(&myCurrentWallSet,
                 0,
                 sizeof(myCurrentWallSet));
  ACE_OS::memset(&myCurrentDoorSet,
                 0,
                 sizeof(myCurrentDoorSet));

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

  // init minimap
  initMiniMap(myEngine);
}

SDL_GUI_LevelWindow::~SDL_GUI_LevelWindow()
{
  RPG_TRACE(ACE_TEXT("SDL_GUI_LevelWindow::~SDL_GUI_LevelWindow"));

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
SDL_GUI_LevelWindow::setView(const int& offsetX_in,
                             const int& offsetY_in)
{
  RPG_TRACE(ACE_TEXT("SDL_GUI_LevelWindow::setView"));

  RPG_Map_Size_t size = myEngine->getSize();

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
SDL_GUI_LevelWindow::getView() const
{
  RPG_TRACE(ACE_TEXT("SDL_GUI_LevelWindow::getView"));

  return myView;
}

void
SDL_GUI_LevelWindow::init(const RPG_Graphics_MapStyle_t& mapStyle_in,
                          const bool& debug_in)
{
  RPG_TRACE(ACE_TEXT("SDL_GUI_LevelWindow::init"));

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

  myDebug = debug_in;

  // init edge, floor, door tiles
  init();

  // init cursor highlighting
  RPG_Map_Size_t size = myEngine->getSize();
  myHighlightBGPosition = std::make_pair(size.first / 2,
                                         size.second / 2);
}

void
SDL_GUI_LevelWindow::draw(SDL_Surface* targetSurface_in,
                          const unsigned int& offsetX_in,
                          const unsigned int& offsetY_in)
{
  RPG_TRACE(ACE_TEXT("SDL_GUI_LevelWindow::draw"));

  // set target surface
  SDL_Surface* targetSurface = (targetSurface_in ? targetSurface_in : myScreen);

  // sanity check(s)
  ACE_ASSERT(myCurrentOffMapTile);
//   ACE_ASSERT(myCurrentCeilingTile);
  ACE_ASSERT(targetSurface);
  ACE_ASSERT(static_cast<int>(offsetX_in) <= targetSurface->w);
  ACE_ASSERT(static_cast<int>(offsetY_in) <= targetSurface->h);

  // init clipping
  clip(targetSurface,
       offsetX_in,
       offsetY_in);

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
  //   2. doors & furniture
  //   3. traps
  //   4. objects
  //   5. creatures
  //   6. effects
  //   7. south & east walls
  //   8. ceiling

  int i, j = 0;
  RPG_Position_t current_map_position = std::make_pair(0, 0);
  RPG_Graphics_FloorTilesConstIterator_t floor_iterator = myCurrentFloorSet.tiles.begin();
  RPG_Graphics_FloorTilesConstIterator_t begin_row = myCurrentFloorSet.tiles.begin();
  unsigned int floor_row = 0;
  unsigned int floor_index = 0;
  RPG_Position_t screen_position = std::make_pair(0, 0);
  // debug info
  SDL_Rect rect;
  std::ostringstream converter;
  std::string tile_text;
  RPG_Graphics_TextSize_t tile_text_size;
//   RPG_Position_t map_position = std::make_pair(0, 0);
  RPG_Graphics_FloorEdgeTileMapIterator_t floor_edge_iterator = myFloorEdgeTiles.end();
  RPG_Engine_EntityID_t entity_id = 0;

  // "clear" map "window"
  clear();

  // pass 1
  RPG_Map_Size_t size = myEngine->getSize();
  for (i = -static_cast<int>(top_right.second);
       i <= static_cast<int>(top_right.second);
       i++)
  {
    current_map_position.second = myView.second + i;

    // floor tile rotation
    floor_row = (current_map_position.second % (myCurrentFloorSet.tiles.size() / myCurrentFloorSet.columns));
    begin_row = myCurrentFloorSet.tiles.begin();
    std::advance(begin_row, floor_row);

    for (j = diff + i;
         (j + i) <= sum;
         j++)
    {
      current_map_position.first = myView.first + j;

      // off-map ?
      if ((current_map_position.second < 0) ||
          (current_map_position.second >= static_cast<int>(size.second)) ||
          (current_map_position.first < 0) ||
          (current_map_position.first >= static_cast<int>(size.first)))
        continue;

      // floor tile rotation
      floor_index = (current_map_position.first % myCurrentFloorSet.columns);
      floor_iterator = begin_row;
      std::advance(floor_iterator, (myCurrentFloorSet.rows * floor_index));

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
//             (current_map_position.second >= myEngine->getDimensions().second) ||
//             (current_map_position.first < 0) ||
//             (current_map_position.first >= myEngine->getDimensions().first))

//         // advance floor iterator
//         std::advance(floor_iterator, myCurrentFloorSet.rows);

        continue;
      } // end IF

      // step2: floor
      if ((myEngine->getElement(current_map_position) == MAPELEMENT_FLOOR) ||
          (myEngine->getElement(current_map_position) == MAPELEMENT_DOOR))
      {
        if (!myHideFloor)
          RPG_Graphics_Surface::put(screen_position.first,
                                    screen_position.second,
                                    *(*floor_iterator).surface,
                                    targetSurface);

        if (myDebug)
        {
          // highlight floor tile indexes
          rect.x = screen_position.first;
          rect.y = screen_position.second;
          rect.w = (*floor_iterator).surface->w;
          rect.h = (*floor_iterator).surface->h;
          RPG_Graphics_Surface::putRect(rect,                         // rectangle
                                        SDL_GUI_DEF_TILE_FRAME_COLOR, // color
                                        targetSurface);               // target surface
          converter.str(ACE_TEXT(""));
          converter.clear();
          tile_text = ACE_TEXT("[");
          converter << current_map_position.first;
          tile_text += converter.str();
          tile_text += ACE_TEXT(",");
          converter.str(ACE_TEXT(""));
          converter.clear();
          converter << current_map_position.second;
          tile_text += converter.str();
          tile_text += ACE_TEXT("]");
          tile_text_size = RPG_Graphics_Common_Tools::textSize(FONT_MAIN_NORMAL,
                                                               tile_text);
          RPG_Graphics_Surface::putText(FONT_MAIN_NORMAL,
                                        tile_text,
                                        RPG_Graphics_SDL_Tools::colorToSDLColor(SDL_GUI_DEF_TILE_INDEX_COLOR,
                                                                                *targetSurface),
                                        true, // add shade
                                        RPG_Graphics_SDL_Tools::colorToSDLColor(RPG_GRAPHICS_FONT_DEF_SHADECOLOR,
                                                                                *targetSurface),
                                        (rect.x + ((rect.w - tile_text_size.first) / 2)),
                                        (rect.y + ((rect.h - tile_text_size.second) / 2)),
                                        targetSurface);
        } // end IF

        // step3: floor edges
        floor_edge_iterator = myFloorEdgeTiles.find(current_map_position);
        if ((floor_edge_iterator != myFloorEdgeTiles.end()) &&
            !myHideFloor)
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

//       // advance floor iterator
//       std::advance(floor_iterator, myCurrentFloorSet.rows);
    } // end FOR
  } // end FOR

  // pass 2
  RPG_Graphics_WallTileMapIterator_t wall_iterator = myWallTiles.end();
  RPG_Graphics_DoorTileMapIterator_t door_iterator = myDoorTiles.end();
//  RPG_Engine_EntityGraphics_t entity_graphics = myEngine->getGraphics();
  RPG_Engine_EntityGraphicsConstIterator_t creature_iterator;
  for (i = -static_cast<int>(top_right.second);
       i <= static_cast<int>(top_right.second);
       i++)
  {
    current_map_position.second = myView.second + i;
    // off the map ? --> continue
    if ((current_map_position.second < 0) ||
        (current_map_position.second >= static_cast<int>(size.second)))
      continue;

    for (j = diff + i;
         (j + i) <= sum;
         j++)
    {
      current_map_position.first = myView.first + j;
      // off the map ? --> continue
      if ((current_map_position.first < 0) ||
          (current_map_position.first >= static_cast<int>(size.first)))
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
      if ((wall_iterator != myWallTiles.end()) &&
          !myHideWalls)
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

      // step2: doors & furniture
      if (door_iterator != myDoorTiles.end())
      {
        // *NOTE*: door are drawn in the "middle" of the floor tile
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

      // step3: traps

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

      //creature_iterator = entity_graphics.find(current_map_position);
      //if (creature_iterator != entity_graphics.end())
      //{
      //  // sanity check
      //  ACE_ASSERT((*creature_iterator).second);

      //  // *NOTE*: creatures are drawn in the "middle" of the floor tile
      //  RPG_Graphics_Surface::put((screen_position.first +
      //                             ((RPG_GRAPHICS_TILE_FLOOR_WIDTH - (*creature_iterator).second->w) / 2)),
      //                            (screen_position.second +
      //                             (RPG_GRAPHICS_TILE_FLOOR_HEIGHT / 2) -
      //                             (*creature_iterator).second->h),
      //                            *((*creature_iterator).second),
      //                            targetSurface);
      //} // end IF

      // step6: effects

      // step7: walls (south & east)
      if ((wall_iterator != myWallTiles.end()) &&
          !myHideWalls)
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
      if (RPG_Client_Common_Tools::hasCeiling(current_map_position, *myEngine) &&
          !myHideWalls)
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
  screen_position = RPG_Graphics_Common_Tools::map2Screen(myHighlightBGPosition,
                                                          inherited::mySize,
                                                          myView);
  // grab BG
  // sanity check for underruns
  if ((screen_position.first  < targetSurface->w) &&
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

  // realize any sub-windows
  for (RPG_Graphics_WindowsIterator_t iterator = inherited::myChildren.begin();
       iterator != inherited::myChildren.end();
       iterator++)
  {
    // draw minimap ?
    if (((*iterator)->getType() == WINDOW_MINIMAP) && !myMinimapIsOn)
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

  // reset clipping area
  unclip(targetSurface);

  // whole viewport needs a refresh...
  SDL_Rect dirtyRegion;
  dirtyRegion.x = static_cast<Sint16>(offsetX_in + myBorderLeft + myOffset.first);
  dirtyRegion.y = static_cast<Sint16>(offsetY_in + myBorderTop + myOffset.second);
  dirtyRegion.w = static_cast<Uint16>(targetSurface->w - offsetX_in - (myBorderLeft + myBorderRight) - myOffset.first);
  dirtyRegion.h = static_cast<Uint16>(targetSurface->h - offsetY_in - (myBorderTop + myBorderBottom) - myOffset.second);
  invalidate(dirtyRegion);

  // remember position of last realization
  myLastAbsolutePosition = std::make_pair(offsetX_in + myBorderLeft + myOffset.first,
                                          offsetY_in + myBorderTop + myOffset.second);
}

void
SDL_GUI_LevelWindow::handleEvent(const SDL_Event& event_in,
                                 RPG_Graphics_IWindow* window_in,
                                 bool& redraw_out)
{
  RPG_TRACE(ACE_TEXT("SDL_GUI_LevelWindow::handleEvent"));

  // init return value(s)
  redraw_out = false;

  RPG_Engine_EntityID_t entity_id = 0;
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
        case SDLK_UP:
        case SDLK_DOWN:
        case SDLK_LEFT:
        case SDLK_RIGHT:
        {
          RPG_Map_Direction direction = DIRECTION_INVALID;
          entity_id = myEngine->getActive();
          switch (event_in.key.keysym.sym)
          {
            case SDLK_c:
            {
              if (myEngine->getActive())
                setView(myEngine->getPosition(entity_id));
              else
              {
                RPG_Map_Size_t size = myEngine->getSize();
                setView((size.first  / 2),
                        (size.second / 2));
              } // end ELSE

              break;
            }
            case SDLK_UP:
            {
              if (event_in.key.keysym.mod & KMOD_SHIFT)
                setView(-RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
                        -RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);
              else
                direction = DIRECTION_UP;

              break;
            }
            case SDLK_DOWN:
            {
              if (event_in.key.keysym.mod & KMOD_SHIFT)
                setView(RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
                        RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);
              else
                direction = DIRECTION_DOWN;

              break;
            }
            case SDLK_LEFT:
            {
              if (event_in.key.keysym.mod & KMOD_SHIFT)
                setView(-RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
                        RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);
              else
                direction = DIRECTION_LEFT;

              break;
            }
            case SDLK_RIGHT:
            {
              if (event_in.key.keysym.mod & KMOD_SHIFT)
                setView(RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
                        -RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);
              else
                direction = DIRECTION_RIGHT;

              break;
            }
            default:
              break;
          } // end SWITCH

          if (!(event_in.key.keysym.mod & KMOD_SHIFT))
          {
            if (entity_id == 0)
              break; // nothing to do...

            RPG_Engine_Action player_action;
            player_action.command = COMMAND_TRAVEL;
            // compute target position
            player_action.position = myEngine->getPosition(entity_id);
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
            // position valid ?
            RPG_Map_Element element = myEngine->getElement(player_action.position);
            if ((element == MAPELEMENT_FLOOR) ||
                (element == MAPELEMENT_DOOR))
            {
              if (element == MAPELEMENT_DOOR)
              {
                RPG_Map_DoorState door_state = myEngine->state(player_action.position, true);
                if ((door_state == DOORSTATE_CLOSED) ||
                    (door_state == DOORSTATE_LOCKED))
                  break;
              } // end IF

              myEngine->action(entity_id, player_action);

              setView(myEngine->getPosition(entity_id));
            } // end IF
          } // end IF

          // *NOTE*: fiddling with the view invalidates the cursor BG !
          RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->invalidateBG();
          // clear highlight BG
          RPG_Graphics_Surface::clear(myHighlightBG);

          // need a redraw
          redraw_out = true;

          // done with this event
          return;
        }
        case SDLK_d:
        {
          myDebug = !myDebug;

          // need a redraw
          redraw_out = true;

          // done with this event
          return;
        }
        case SDLK_f:
        {
          if (event_in.key.keysym.mod & KMOD_SHIFT)
          {
            // toggle setting
            myHideFloor = !myHideFloor;

            // *NOTE*: fiddling with the style invalidates the cursor BG !
            RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->invalidateBG();
            // clear highlight BG
            RPG_Graphics_Surface::clear(myHighlightBG);

            // need a redraw
            redraw_out = true;

            // done with this event
            return;
          } // end IF

          myCurrentMapStyle.floor_style = static_cast<RPG_Graphics_FloorStyle>(myCurrentMapStyle.floor_style + 1);
          if (myCurrentMapStyle.floor_style == RPG_GRAPHICS_FLOORSTYLE_MAX)
            myCurrentMapStyle.floor_style = static_cast<RPG_Graphics_FloorStyle>(0);
          RPG_Graphics_StyleUnion style;
          style.discriminator = RPG_Graphics_StyleUnion::FLOORSTYLE;
          style.floorstyle = myCurrentMapStyle.floor_style;
          setStyle(style);

          // *NOTE*: fiddling with the style invalidates the cursor BG !
          RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->invalidateBG();
          // clear highlight BG
          RPG_Graphics_Surface::clear(myHighlightBG);

          // need a redraw
          redraw_out = true;

          // done with this event
          return;
        }
        case SDLK_h:
        {
          // toggle setting
          myCurrentMapStyle.half_height_walls = !myCurrentMapStyle.half_height_walls;

          RPG_Graphics_StyleUnion new_style;
          new_style.discriminator = RPG_Graphics_StyleUnion::WALLSTYLE;
          new_style.wallstyle = myCurrentMapStyle.wall_style;
          setStyle(new_style);

          // *NOTE*: fiddling with the style invalidates the cursor BG !
          RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->invalidateBG();
          // clear highlight BG
          RPG_Graphics_Surface::clear(myHighlightBG);

          // need a redraw
          redraw_out = true;

          // done with this event
          return;
        }
        case SDLK_m:
        {
          myMinimapIsOn = !myMinimapIsOn;

          // *NOTE*: fiddling with the view invalidates the cursor BG !
          RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->invalidateBG();
          // clear highlight BG
          RPG_Graphics_Surface::clear(myHighlightBG);

          // need a redraw
          redraw_out = true;

          // done with this event
          return;
        }
        case SDLK_w:
        {
          if (event_in.key.keysym.mod & KMOD_SHIFT)
          {
            // toggle setting
            myHideWalls = !myHideWalls;

            // *NOTE*: fiddling with the style invalidates the cursor BG !
            RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->invalidateBG();
            // clear highlight BG
            RPG_Graphics_Surface::clear(myHighlightBG);

            // need a redraw
            redraw_out = true;

            // done with this event
            return;
          } // end IF

          myCurrentMapStyle.wall_style = static_cast<RPG_Graphics_WallStyle>(myCurrentMapStyle.wall_style + 1);;
          if (myCurrentMapStyle.wall_style == RPG_GRAPHICS_WALLSTYLE_MAX)
            myCurrentMapStyle.wall_style = static_cast<RPG_Graphics_WallStyle>(0);
          RPG_Graphics_StyleUnion style;
          style.discriminator = RPG_Graphics_StyleUnion::WALLSTYLE;
          style.wallstyle = myCurrentMapStyle.wall_style;
          setStyle(style);

          // *NOTE*: fiddling with the style invalidates the cursor BG !
          RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->invalidateBG();
          // clear highlight BG
          RPG_Graphics_Surface::clear(myHighlightBG);

          // need a redraw
          redraw_out = true;

          // done with this event
          return;
        }
        default:
          break;
      } // end SWITCH

      // delegate all other keypresses to the parent...
      return getParent()->handleEvent(event_in,
                                      window_in,
                                      redraw_out);
    }
    // *** mouse ***
    case SDL_MOUSEMOTION:
    {
      // find map square
      RPG_Graphics_Position_t map_position = RPG_Graphics_Common_Tools::screen2Map(std::make_pair(event_in.motion.x,
                                                                                                  event_in.motion.y),
                                                                                   myEngine->getSize(),
                                                                                   mySize,
                                                                                   myView);
//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("mouse position [%u,%u] --> [%u,%u]\n"),
//                  event_in.button.x,
//                  event_in.button.y,
//                  map_position.first,
//                  map_position.second));

      // (re-)draw "active" tile highlight ?
      SDL_Rect clipRect, dirtyRegion;
      RPG_Graphics_Position_t tile_position;

      // inside map ?
      if (map_position == std::make_pair(std::numeric_limits<unsigned int>::max(),
                                         std::numeric_limits<unsigned int>::max()))
      {
        // off the map --> remove "active" tile highlight
        restoreBG();

        break;
      } // end IF

      // change "active" tile ?
      if (map_position != myHighlightBGPosition)
      {
        // *NOTE*: restore cursor BG first
        RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->restoreBG(myScreen,
                                                                     dirtyRegion);
//         invalidate(dirtyRegion);
        // *NOTE*: updating straight away reduces ugly smears...
        RPG_Graphics_Surface::update(dirtyRegion,
                                     myScreen);

        // step1: restore/clear old background
        restoreBG();

        // step2: store current background
        tile_position = RPG_Graphics_Common_Tools::map2Screen(map_position,
                                                              mySize,
                                                              myView);
        // sanity check for underruns
        if ((tile_position.first  < static_cast<unsigned int>(myScreen->w)) &&
            (tile_position.second < static_cast<unsigned int>(myScreen->h)))
        {
          clip();
          RPG_Graphics_Surface::get(tile_position.first,
                                    tile_position.second,
                                    true, // use (fast) blitting method
                                    *myScreen,
                                    *myHighlightBG);
          myHighlightBGPosition = map_position;

          // step3: draw highlight
  //         ACE_DEBUG((LM_DEBUG,
  //                    ACE_TEXT("highlight @ (%u,%u) --> (%u,%u)\n"),
  //                    map_position.first,
  //                    map_position.second,
  //                    tile_position.first,
  //                    tile_position.second));

          RPG_Graphics_Surface::put(tile_position.first,
                                    tile_position.second,
                                    *myHighlightTile,
                                    myScreen);

          dirtyRegion.x = static_cast<Sint16>(tile_position.first);
          dirtyRegion.y = static_cast<Sint16>(tile_position.second);
          dirtyRegion.w = static_cast<Uint16>(myHighlightTile->w);
          dirtyRegion.h = static_cast<Uint16>(myHighlightTile->h);
          SDL_GetClipRect(myScreen, &clipRect);
          unclip();
//           invalidate(dirtyRegion);
          // *NOTE*: updating straight away reduces ugly smears...
          RPG_Graphics_Surface::update(RPG_Graphics_SDL_Tools::intersect(clipRect, dirtyRegion),
                                       myScreen);
        } // end IF
      } // end IF

      // set an appropriate cursor
      RPG_Engine_EntityID_t entity_id = myEngine->getActive(true);
      RPG_Graphics_Cursor cursor_type = RPG_Client_Common_Tools::getCursor(map_position,
                                                                           entity_id,
                                                                           true,
                                                                           SELECTIONMODE_NORMAL,
                                                                           *myEngine,
                                                                           true);
      if (cursor_type != RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->type())
      {
        // *NOTE*: restore cursor BG first
        RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->restoreBG(myScreen, dirtyRegion);
//         invalidate(dirtyRegion);
        // *NOTE*: updating straight away reduces ugly smears...
        RPG_Graphics_Surface::update(dirtyRegion, myScreen);

        RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->set(cursor_type);
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
        RPG_Graphics_Position_t map_position = RPG_Graphics_Common_Tools::screen2Map(std::make_pair(event_in.button.x,
                                                                                                    event_in.button.y),
                                                                                     myEngine->getSize(),
                                                                                     mySize,
                                                                                     myView);

        ACE_DEBUG((LM_DEBUG,
                   ACE_TEXT("mouse position [%u,%u] --> [%u,%u]\n"),
                   event_in.button.x,
                   event_in.button.y,
                   map_position.first,
                   map_position.second));

        // closed door ? --> (try to) open it
        if (myEngine->getElement(map_position) == MAPELEMENT_DOOR)
        {
          RPG_Map_DoorState door_state = myEngine->state(map_position, true);

          RPG_Engine_Action action;
          action.command = ((door_state == DOORSTATE_OPEN) ? COMMAND_DOOR_CLOSE
                                                           : COMMAND_DOOR_OPEN);
          action.position = map_position;
          myEngine->action(myEngine->getActive(), action);
        } // end IF
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
      restoreBG();

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
      return getParent()->handleEvent(event_in,
                                      window_in,
                                      redraw_out);

      break;
    }
  } // end SWITCH
}

void
SDL_GUI_LevelWindow::init()
{
  RPG_TRACE(ACE_TEXT("SDL_GUI_LevelWindow::init"));

  // clean up
  myFloorEdgeTiles.clear();
  myWallTiles.clear();
  myDoorTiles.clear();

  // init tiles / position
  RPG_Client_Common_Tools::initFloorEdges(*myEngine,
                                          myCurrentFloorEdgeSet,
                                          myFloorEdgeTiles);
  RPG_Client_Common_Tools::initWalls(*myEngine,
                                     myCurrentWallSet,
                                     myWallTiles);
  RPG_Client_Common_Tools::initDoors(*myEngine,
                                     myCurrentDoorSet,
                                     myDoorTiles);

  // init view
  RPG_Map_Size_t size = myEngine->getSize(true);
  setView((size.first  / 2),
          (size.second / 2));

  // *NOTE*: fiddling with the view invalidates the cursor BG !
  RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->invalidateBG();
  // clear highlight BG
  RPG_Graphics_Surface::clear(myHighlightBG);
}

//void
//SDL_GUI_LevelWindow::redraw()
//{
//  RPG_TRACE(ACE_TEXT("SDL_GUI_LevelWindow::redraw"));
//
////   draw();
////   refresh();
//}

void
SDL_GUI_LevelWindow::setView(const RPG_Map_Position_t& position_in)
{
  RPG_TRACE(ACE_TEXT("SDL_GUI_LevelWindow::setView"));

  myView = position_in;
}

//void
//SDL_GUI_LevelWindow::toggleDoor(const RPG_Map_Position_t& position_in)
//{
//  RPG_TRACE(ACE_TEXT("SDL_GUI_LevelWindow::toggleDoor"));
//
//  RPG_Map_Door_t door = myEngine->getDoor(position_in);
//  ACE_ASSERT(door.position == position_in);
//
//  // change tile accordingly
//  RPG_Graphics_Orientation orientation = RPG_GRAPHICS_ORIENTATION_INVALID;
//  orientation = RPG_Engine_Common_Tools::getDoorOrientation(*myEngine,
//                                                            position_in);
//  switch (orientation)
//  {
//    case ORIENTATION_HORIZONTAL:
//      myDoorTiles[position_in] = (door.is_open ? myCurrentDoorSet.horizontal_open
//      : myCurrentDoorSet.horizontal_closed); break;
//    case ORIENTATION_VERTICAL:
//      myDoorTiles[position_in] = (door.is_open ? myCurrentDoorSet.vertical_open
//      : myCurrentDoorSet.vertical_closed); break;
//    default:
//    {
//      ACE_DEBUG((LM_ERROR,
//                 ACE_TEXT("invalid door orientation \"%s\", aborting\n"),
//                 RPG_Graphics_OrientationHelper::RPG_Graphics_OrientationToString(orientation).c_str()));
//
//      return;
//    }
//  } // end SWITCH
//}
//
//void
//SDL_GUI_LevelWindow::updateEntity(const RPG_Engine_EntityID_t& id_in)
//{
//  RPG_TRACE(ACE_TEXT("SDL_GUI_LevelWindow::updateEntity"));
//
////   draw();
////   refresh();
//}

void
SDL_GUI_LevelWindow::notify(const RPG_Engine_Command& command_in,
                            const RPG_Engine_ClientParameters_t& parameters_in)
{
  RPG_TRACE(ACE_TEXT("SDL_GUI_LevelWindow::notify"));

  switch (command_in)
  {
    case COMMAND_ATTACK:
      return;
    case COMMAND_DOOR_CLOSE:
    case COMMAND_DOOR_OPEN:
    {
      ACE_ASSERT(parameters_in.size() == 1);

      RPG_Map_Position_t position = *static_cast<RPG_Map_Position_t*>(parameters_in.front());
      RPG_Map_DoorState door_state = myEngine->state(position, true);

      // change tile accordingly
      RPG_Graphics_Orientation orientation = RPG_GRAPHICS_ORIENTATION_INVALID;
      orientation = RPG_Client_Common_Tools::getDoorOrientation(*myEngine,
                                                                position);
      switch (orientation)
      {
        case ORIENTATION_HORIZONTAL:
          myDoorTiles[position] = ((door_state == DOORSTATE_OPEN) ? myCurrentDoorSet.horizontal_open
                                                                  : myCurrentDoorSet.horizontal_closed); break;
        case ORIENTATION_VERTICAL:
          myDoorTiles[position] = ((door_state == DOORSTATE_OPEN) ? myCurrentDoorSet.vertical_open
                                                                  : myCurrentDoorSet.vertical_closed); break;
        default:
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("invalid door orientation \"%s\", aborting\n"),
                     RPG_Graphics_OrientationHelper::RPG_Graphics_OrientationToString(orientation).c_str()));

          return;
        }
      } // end SWITCH

      break;
    }
    case COMMAND_SEARCH:
    case COMMAND_STOP:
    case COMMAND_TRAVEL:
      return;
    case COMMAND_E2C_ENTITY_ADD:
    {
      ACE_ASSERT(parameters_in.size() == 2);
      RPG_CLIENT_ENTITY_MANAGER_SINGLETON::instance()->add(*static_cast<RPG_Engine_EntityID_t*>(parameters_in.front()),
                                                           static_cast<SDL_Surface*>(parameters_in.back()));

      return;
    }
    case COMMAND_E2C_ENTITY_HIT:
    case COMMAND_E2C_ENTITY_MISS:
      return;
    case COMMAND_E2C_ENTITY_POSITION:
    {
      ACE_ASSERT(parameters_in.size() == 2);

      SDL_Rect dirtyRegion = {0, 0, 0, 0};
      RPG_CLIENT_ENTITY_MANAGER_SINGLETON::instance()->put(*static_cast<RPG_Engine_EntityID_t*>(parameters_in.front()),
                                                           RPG_Graphics_Common_Tools::map2Screen(*static_cast<RPG_Map_Position_t*>(parameters_in.back()),
                                                                                                 getSize(false),
                                                                                                 getView()),
                                                           getScreen(),
                                                           dirtyRegion);

      RPG_Graphics_Surface::update(dirtyRegion,
                                   getScreen());

      break;
    }
    case COMMAND_E2C_ENTITY_REMOVE:
    {
      ACE_ASSERT(parameters_in.size() == 1);
      RPG_CLIENT_ENTITY_MANAGER_SINGLETON::instance()->remove(*static_cast<RPG_Engine_EntityID_t*>(parameters_in.front()));
      
      return;
    }
    case COMMAND_E2C_ENTITY_VISION:
    {
      ACE_ASSERT(parameters_in.size() == 2);

      return;
    }
    case COMMAND_E2C_QUIT:
    {
      ACE_ASSERT(parameters_in.empty());

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
}

void
SDL_GUI_LevelWindow::clear()
{
  RPG_TRACE(ACE_TEXT("SDL_GUI_LevelWindow::clear"));

  // init clipping
  SDL_Rect old_clip_rect;
  SDL_GetClipRect(myScreen, &old_clip_rect);

  SDL_Rect clipRect;
  clipRect.x = static_cast<Sint16>(myBorderLeft + myOffset.first);
  clipRect.y = static_cast<Sint16>(myBorderTop + myOffset.second);
  clipRect.w = static_cast<Uint16>(myScreen->w - (myBorderLeft + myBorderRight) - myOffset.first);
  clipRect.h = static_cast<Uint16>(myScreen->h - (myBorderTop + myBorderBottom) - myOffset.second);
  if (!SDL_SetClipRect(myScreen, &clipRect))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_SetClipRect(): %s, aborting\n"),
               SDL_GetError()));

    return;
  } // end IF

  if (SDL_FillRect(myScreen,                             // target surface
                   &clipRect,                            // rectangle
                   RPG_Graphics_SDL_Tools::CLR32_BLACK)) // color
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_FillRect(): %s, aborting\n"),
               SDL_GetError()));

    return;
  } // end IF

  // reset clipping
  if (!SDL_SetClipRect(myScreen, &old_clip_rect))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_SetClipRect(): %s, aborting\n"),
               SDL_GetError()));

    return;
  } // end IF
}

void
SDL_GUI_LevelWindow::setStyle(const RPG_Graphics_StyleUnion& style_in)
{
  RPG_TRACE(ACE_TEXT("SDL_GUI_LevelWindow::setStyle"));

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

      // debug info
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
      std::string dump_path_base = ACE_TEXT_ALWAYS_CHAR(RPG_COMMON_DUMP_DIR);
#else
      std::string dump_path_base = ACE_OS::getenv(ACE_TEXT_ALWAYS_CHAR(RPG_COMMON_DUMP_DIR));
#endif
      dump_path_base += ACE_DIRECTORY_SEPARATOR_CHAR_A;
      std::string dump_path;

//       dump_path = dump_path_base;
//       dump_path += ACE_TEXT("wall_n.png");
//       RPG_Graphics_Surface::savePNG(*myCurrentWallSet.north.surface, // image
//                                     dump_path,                       // file
//                                     true);                           // WITH alpha
//       dump_path = dump_path_base;
//       dump_path += ACE_TEXT("wall_s.png");
//       RPG_Graphics_Surface::savePNG(*myCurrentWallSet.south.surface, // image
//                                     dump_path,                       // file
//                                     true);                           // WITH alpha
//       dump_path = dump_path_base;
//       dump_path += ACE_TEXT("wall_w.png");
//       RPG_Graphics_Surface::savePNG(*myCurrentWallSet.west.surface, // image
//                                     dump_path,                      // file
//                                     true);                          // WITH alpha
//       dump_path = dump_path_base;
//       dump_path += ACE_TEXT("wall_e.png");
//       RPG_Graphics_Surface::savePNG(*myCurrentWallSet.east.surface, // image
//                                     dump_path,                      // file
//                                     true);                          // WITH alpha

      // *NOTE*: WEST is just a "darkened" version of EAST...
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

      // *NOTE*: NORTH is just a "darkened" version of SOUTH...
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

//       dump_path = dump_path_base;
//       dump_path += ACE_TEXT("wall_n_blended.png");
//       RPG_Graphics_Surface::savePNG(*myCurrentWallSet.north.surface, // image
//                                     dump_path,                       // file
//                                     true);                           // WITH alpha
//       dump_path = dump_path_base;
//       dump_path += ACE_TEXT("wall_s_blended.png");
//       RPG_Graphics_Surface::savePNG(*myCurrentWallSet.south.surface, // image
//                                     dump_path,                       // file
//                                     true);                           // WITH alpha
//       dump_path = dump_path_base;
//       dump_path += ACE_TEXT("wall_w_blended.png");
//       RPG_Graphics_Surface::savePNG(*myCurrentWallSet.west.surface, // image
//                                     dump_path,                      // file
//                                     true);                          // WITH alpha
//       dump_path = dump_path_base;
//       dump_path += ACE_TEXT("wall_e_blended.png");
//       RPG_Graphics_Surface::savePNG(*myCurrentWallSet.east.surface, // image
//                                     dump_path,                      // file
//                                     true);                          // WITH alpha

      // adjust EAST wall opacity
      SDL_Surface* shaded_wall = NULL;
      shaded_wall = RPG_Graphics_Surface::alpha(*myCurrentWallSet.east.surface,
                                                static_cast<Uint8>((RPG_GRAPHICS_TILE_DEF_WALL_SE_OPACITY * SDL_ALPHA_OPAQUE)));
      if (!shaded_wall)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to RPG_Graphics_Surface::alpha(%u), aborting\n"),
                   static_cast<Uint8>((RPG_GRAPHICS_TILE_DEF_WALL_SE_OPACITY * SDL_ALPHA_OPAQUE))));

        return;
      } // end IF
      SDL_FreeSurface(myCurrentWallSet.east.surface);
      myCurrentWallSet.east.surface = shaded_wall;

      // adjust WEST wall opacity
      shaded_wall = RPG_Graphics_Surface::alpha(*myCurrentWallSet.west.surface,
                                                static_cast<Uint8>((RPG_GRAPHICS_TILE_DEF_WALL_NW_OPACITY * SDL_ALPHA_OPAQUE)));
      if (!shaded_wall)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to RPG_Graphics_Surface::alpha(%u), aborting\n"),
                   static_cast<Uint8>((RPG_GRAPHICS_TILE_DEF_WALL_NW_OPACITY * SDL_ALPHA_OPAQUE))));

        return;
      } // end IF
      SDL_FreeSurface(myCurrentWallSet.west.surface);
      myCurrentWallSet.west.surface = shaded_wall;

      // adjust SOUTH wall opacity
      shaded_wall = RPG_Graphics_Surface::alpha(*myCurrentWallSet.south.surface,
                                                static_cast<Uint8>((RPG_GRAPHICS_TILE_DEF_WALL_SE_OPACITY * SDL_ALPHA_OPAQUE)));
      if (!shaded_wall)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to RPG_Graphics_Surface::alpha(%u), aborting\n"),
                   static_cast<Uint8>((RPG_GRAPHICS_TILE_DEF_WALL_SE_OPACITY * SDL_ALPHA_OPAQUE))));

        return;
      } // end IF
      SDL_FreeSurface(myCurrentWallSet.south.surface);
      myCurrentWallSet.south.surface = shaded_wall;

      // adjust NORTH wall opacity
      shaded_wall = RPG_Graphics_Surface::alpha(*myCurrentWallSet.north.surface,
                                                static_cast<Uint8>((RPG_GRAPHICS_TILE_DEF_WALL_NW_OPACITY * SDL_ALPHA_OPAQUE)));
      if (!shaded_wall)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to RPG_Graphics_Surface::alpha(%u), aborting\n"),
                   static_cast<Uint8>((RPG_GRAPHICS_TILE_DEF_WALL_NW_OPACITY * SDL_ALPHA_OPAQUE))));

        return;
      } // end IF
      SDL_FreeSurface(myCurrentWallSet.north.surface);
      myCurrentWallSet.north.surface = shaded_wall;

//       dump_path = dump_path_base;
//       dump_path += ACE_TEXT("wall_n_shaded.png");
//       RPG_Graphics_Surface::savePNG(*myCurrentWallSet.north.surface, // image
//                                     dump_path,                       // file
//                                     true);                           // WITH alpha
//       dump_path = dump_path_base;
//       dump_path += ACE_TEXT("wall_s_shaded.png");
//       RPG_Graphics_Surface::savePNG(*myCurrentWallSet.south.surface, // image
//                                     dump_path,                       // file
//                                     true);                           // WITH alpha
//       dump_path = dump_path_base;
//       dump_path += ACE_TEXT("wall_w_shaded.png");
//       RPG_Graphics_Surface::savePNG(*myCurrentWallSet.west.surface, // image
//                                     dump_path,                      // file
//                                     true);                          // WITH alpha
//       dump_path = dump_path_base;
//       dump_path += ACE_TEXT("wall_e_shaded.png");
//       RPG_Graphics_Surface::savePNG(*myCurrentWallSet.east.surface, // image
//                                     dump_path,                      // file
//                                     true);                          // WITH alpha

      // update wall tiles / position
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
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("door-style \"%s\" is incomplete, continuing\n"),
                   RPG_Graphics_DoorStyleHelper::RPG_Graphics_DoorStyleToString(style_in.doorstyle).c_str()));

      // update door tiles / position
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
SDL_GUI_LevelWindow::initCeiling()
{
  RPG_TRACE(ACE_TEXT("SDL_GUI_LevelWindow::initCeiling"));

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
  shaded_ceiling = RPG_Graphics_Surface::alpha(*myCurrentCeilingTile,
                                               static_cast<Uint8>((RPG_GRAPHICS_TILE_DEF_WALL_NW_OPACITY * SDL_ALPHA_OPAQUE)));
  if (!shaded_ceiling)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Graphics_Surface::alpha(%u), aborting\n"),
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
SDL_GUI_LevelWindow::initWallBlend(const bool& halfHeightWalls_in)
{
  RPG_TRACE(ACE_TEXT("SDL_GUI_LevelWindow::initWallBlend"));

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

//   // debug info
//   std::string dump_path_base = ACE_TEXT_ALWAYS_CHAR(RPG_COMMON_DUMP_DIR);
//   dump_path_base += ACE_DIRECTORY_SEPARATOR_CHAR_A;
//   std::string dump_path = dump_path_base;
//   dump_path += ACE_TEXT("wall_blend.png");
//   RPG_Graphics_Surface::savePNG(*myWallBlend, // image
//                                 dump_path,    // file
//                                 true);        // WITH alpha
}

void
SDL_GUI_LevelWindow::initMiniMap(RPG_Engine* engine_in)
{
  RPG_TRACE(ACE_TEXT("SDL_GUI_LevelWindow::initMiniMap"));

  RPG_Graphics_Offset_t offset;
  offset.first = SDL_GUI_DEF_GRAPHICS_MINIMAP_OFFSET_X;
  offset.second = SDL_GUI_DEF_GRAPHICS_MINIMAP_OFFSET_Y;

  SDL_GUI_MinimapWindow* minimap_window = NULL;
  try
  {
    minimap_window = new SDL_GUI_MinimapWindow(*this,
                                               offset,
                                               engine_in);
  }
  catch (...)
  {
    ACE_DEBUG((LM_CRITICAL,
               ACE_TEXT("failed to allocate memory(%u): %m, aborting\n"),
               sizeof(SDL_GUI_MinimapWindow)));

    return;
  }
  if (!minimap_window)
  {
    ACE_DEBUG((LM_CRITICAL,
               ACE_TEXT("failed to allocate memory(%u): %m, aborting\n"),
               sizeof(SDL_GUI_MinimapWindow)));

    return;
  } // end IF
}

void
SDL_GUI_LevelWindow::restoreBG()
{
  RPG_TRACE(ACE_TEXT("SDL_GUI_LevelWindow::restoreBG"));

  SDL_Rect dirtyRegion, clipRect;
  RPG_Graphics_Position_t tile_position;

  if (myHighlightBG)
  {
    tile_position = RPG_Graphics_Common_Tools::map2Screen(myHighlightBGPosition,
                                                          mySize,
                                                          myView);
    // sanity check for underruns
    if ((tile_position.first  < static_cast<unsigned int>(myScreen->w)) &&
        (tile_position.second < static_cast<unsigned int>(myScreen->h)))
    {
      clip();
      RPG_Graphics_Surface::put(tile_position.first,
                                tile_position.second,
                                *myHighlightBG,
                                myScreen);

      dirtyRegion.x = static_cast<Sint16>(tile_position.first);
      dirtyRegion.y = static_cast<Sint16>(tile_position.second);
      dirtyRegion.w = static_cast<Uint16>(myHighlightBG->w);
      dirtyRegion.h = static_cast<Uint16>(myHighlightBG->h);
      SDL_GetClipRect(myScreen, &clipRect);
      unclip();
//             invalidate(dirtyRegion);
      // *NOTE*: updating straight away reduces ugly smears...
      RPG_Graphics_Surface::update(RPG_Graphics_SDL_Tools::intersect(clipRect, dirtyRegion),
                                   myScreen);

      // clear highlight BG
      RPG_Graphics_Surface::clear(myHighlightBG);
    } // end IF
  } // end IF
}
