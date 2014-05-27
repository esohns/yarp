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

#include "SDL_gui_levelwindow_3d.h"

#include "SDL_gui_defines.h"
#include "SDL_gui_minimapwindow.h"

#include "rpg_client_defines.h"
#include "rpg_client_entity_manager.h"
#include "rpg_client_common_tools.h"

#include "rpg_engine.h"
#include "rpg_engine_common_tools.h"

#include "rpg_graphics_defines.h"
#include "rpg_graphics_surface.h"
#include "rpg_graphics_cursor_manager.h"
#include "rpg_graphics_common_tools.h"
#include "rpg_graphics_SDL_tools.h"

#include "rpg_map_common_tools.h"

#include "rpg_common_macros.h"
#include "rpg_common_defines.h"

#include <SDL_opengl.h>

#include <ace/Log_Msg.h>

#include <sstream>

SDL_GUI_LevelWindow_3D::SDL_GUI_LevelWindow_3D(const RPG_Graphics_SDLWindowBase& parent_in,
                                               RPG_Engine* engine_in)
 : inherited(WINDOW_MAP,           // type
             parent_in,            // parent
             std::make_pair(0, 0), // offset
             std::string()),       // title
//              NULL),                // background
   myState(NULL),
   myEngine(engine_in),
//   myState->style(),
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
   myView(std::make_pair(std::numeric_limits<unsigned int>::max(),
                         std::numeric_limits<unsigned int>::max())),
//   myHighlightBGPosition(std::make_pair(myEngine->getSize().first / 2,
//                                        myEngine->getSize().second / 2)),
//   myHighlightBG(NULL),
//   myHighlightTile(NULL),
   myMinimapIsOn(SDL_GUI_DEF_GRAPHICS_MINIMAP_ISON)
{
  RPG_TRACE(ACE_TEXT("SDL_GUI_LevelWindow_3D::SDL_GUI_LevelWindow"));

	myEngine->init(this);

	myEngine->lock();
	RPG_Map_Size_t map_size = myEngine->getSize(false);
	myEngine->unlock();
	myView = std::make_pair(map_size.first / 2,
													map_size.second / 2);

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

  // init minimap
  initMiniMap(myEngine);
}

SDL_GUI_LevelWindow_3D::~SDL_GUI_LevelWindow_3D()
{
  RPG_TRACE(ACE_TEXT("SDL_GUI_LevelWindow_3D::~SDL_GUI_LevelWindow"));

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

//  if (myHighlightBG)
//    SDL_FreeSurface(myHighlightBG);
//  if (myHighlightTile)
//    SDL_FreeSurface(myHighlightTile);
}

void
SDL_GUI_LevelWindow_3D::setView(const int& offsetX_in,
                                const int& offsetY_in)
{
  RPG_TRACE(ACE_TEXT("SDL_GUI_LevelWindow_3D::setView"));

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

  RPG_Map_Size_t map_size = myEngine->getSize(true);
  if (myView.first >= map_size.first)
    myView.first = (map_size.first - 1);
  if (myView.second >= map_size.second)
    myView.second = (map_size.second - 1);
}

RPG_Graphics_Position_t
SDL_GUI_LevelWindow_3D::getView() const
{
  RPG_TRACE(ACE_TEXT("SDL_GUI_LevelWindow_3D::getView"));

  return myView;
}

void
SDL_GUI_LevelWindow_3D::init(state_t* state_in,
                             RPG_Common_ILock* screenLock_in)
{
  RPG_TRACE(ACE_TEXT("SDL_GUI_LevelWindow_3D::init"));

  // sanity check(s)
  ACE_ASSERT(state_in);

  myState = state_in;
  inherited::init(screenLock_in);

	float ratio = (static_cast<float>(myClipRect.w) /
								 static_cast<float>(myClipRect.h));

  /* Our shading model--Gouraud (smooth). */
  glShadeModel(GL_SMOOTH);

  /* Culling. */
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);
  glEnable(GL_CULL_FACE);

  /* Set the clear color. */
  glClearColor(0, 0, 0, 0);

  /* Setup our viewport. */
  glViewport(myClipRect.x, myClipRect.y, myClipRect.w, myClipRect.h);

  /*
   * Change to the projection matrix and set
   * our viewing volume.
   */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  /*
   * EXERCISE:
   * Replace this with a call to glFrustum.
   */
  gluPerspective(60.0, ratio, 1.0, 1024.0);

  // init style
  RPG_Graphics_StyleUnion style;
  style.discriminator = RPG_Graphics_StyleUnion::FLOORSTYLE;
  style.floorstyle = myState->style.floor;
  setStyle(style);
  style.discriminator = RPG_Graphics_StyleUnion::EDGESTYLE;
  style.edgestyle = myState->style.edge;
  setStyle(style);
  style.discriminator = RPG_Graphics_StyleUnion::WALLSTYLE;
  style.wallstyle = myState->style.wall;
  setStyle(style);
  style.discriminator = RPG_Graphics_StyleUnion::DOORSTYLE;
  style.doorstyle = myState->style.door;
  setStyle(style);
}

void
SDL_GUI_LevelWindow_3D::drawBorder(SDL_Surface* targetSurface_in,
                                   const unsigned int& offsetX_in,
                                   const unsigned int& offsetY_in)
{
  RPG_TRACE(ACE_TEXT("SDL_GUI_LevelWindow_3D::drawBorder"));

  // *NOTE*: should NEVER be reached !
  ACE_ASSERT(false);

#if defined (_MSC_VER)
  return;
#else
  ACE_NOTREACHED(return;)
#endif
}

void
SDL_GUI_LevelWindow_3D::draw(SDL_Surface* targetSurface_in,
                             const unsigned int& offsetX_in,
                             const unsigned int& offsetY_in)
{
  RPG_TRACE(ACE_TEXT("SDL_GUI_LevelWindow_3D::draw"));

  // sanity check(s)
  SDL_Surface* target_surface = (targetSurface_in ? targetSurface_in
                                                  : myScreen);
  ACE_ASSERT(target_surface);
  ACE_ASSERT(myCurrentOffMapTile);
//   ACE_ASSERT(myCurrentCeilingTile);
  ACE_ASSERT(static_cast<int>(offsetX_in) <= target_surface->w);
  ACE_ASSERT(static_cast<int>(offsetY_in) <= target_surface->h);

  // init clipping
  clip(target_surface,
       offsetX_in,
       offsetY_in);

  if (inherited::myScreenLock)
    inherited::myScreenLock->lock();
  // "clear" map "window"
//  inherited::clear();

  /* Our angle of rotation. */
  static float angle = 0.0f;

  /*
   * EXERCISE:
   * Replace this awful mess with vertex
   * arrays and a call to glDrawElements.
   *
   * EXERCISE:
   * After completing the above, change
   * it to use compiled vertex arrays.
   *
   * EXERCISE:
   * Verify my windings are correct here ;).
   */
  static GLfloat v0[] = { -1.0f, -1.0f,  1.0f };
  static GLfloat v1[] = {  1.0f, -1.0f,  1.0f };
  static GLfloat v2[] = {  1.0f,  1.0f,  1.0f };
  static GLfloat v3[] = { -1.0f,  1.0f,  1.0f };
  static GLfloat v4[] = { -1.0f, -1.0f, -1.0f };
  static GLfloat v5[] = {  1.0f, -1.0f, -1.0f };
  static GLfloat v6[] = {  1.0f,  1.0f, -1.0f };
  static GLfloat v7[] = { -1.0f,  1.0f, -1.0f };
  static GLubyte red[]    = { 255,   0,   0, 255 };
  static GLubyte green[]  = {   0, 255,   0, 255 };
  static GLubyte blue[]   = {   0,   0, 255, 255 };
  static GLubyte white[]  = { 255, 255, 255, 255 };
  static GLubyte yellow[] = {   0, 255, 255, 255 };
  static GLubyte black[]  = {   0,   0,   0, 255 };
  static GLubyte orange[] = { 255, 255,   0, 255 };
  static GLubyte purple[] = { 255,   0, 255,   0 };

  /* Clear the color and depth buffers. */
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  /* We don't want to modify the projection matrix. */
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity( );

  /* Move down the z-axis. */
  glTranslatef( 0.0, 0.0, -5.0 );

  /* Rotate. */
  glRotatef( angle, 0.0, 1.0, 0.0 );

  static GLboolean should_rotate = GL_TRUE;
  if( should_rotate ) {

      if( ++angle > 360.0f ) {
          angle = 0.0f;
      }

  }

  /* Send our triangle data to the pipeline. */
  glBegin( GL_TRIANGLES );

  glColor4ubv( red );
  glVertex3fv( v0 );
  glColor4ubv( green );
  glVertex3fv( v1 );
  glColor4ubv( blue );
  glVertex3fv( v2 );

  glColor4ubv( red );
  glVertex3fv( v0 );
  glColor4ubv( blue );
  glVertex3fv( v2 );
  glColor4ubv( white );
  glVertex3fv( v3 );

  glColor4ubv( green );
  glVertex3fv( v1 );
  glColor4ubv( black );
  glVertex3fv( v5 );
  glColor4ubv( orange );
  glVertex3fv( v6 );

  glColor4ubv( green );
  glVertex3fv( v1 );
  glColor4ubv( orange );
  glVertex3fv( v6 );
  glColor4ubv( blue );
  glVertex3fv( v2 );

  glColor4ubv( black );
  glVertex3fv( v5 );
  glColor4ubv( yellow );
  glVertex3fv( v4 );
  glColor4ubv( purple );
  glVertex3fv( v7 );

  glColor4ubv( black );
  glVertex3fv( v5 );
  glColor4ubv( purple );
  glVertex3fv( v7 );
  glColor4ubv( orange );
  glVertex3fv( v6 );

  glColor4ubv( yellow );
  glVertex3fv( v4 );
  glColor4ubv( red );
  glVertex3fv( v0 );
  glColor4ubv( white );
  glVertex3fv( v3 );

  glColor4ubv( yellow );
  glVertex3fv( v4 );
  glColor4ubv( white );
  glVertex3fv( v3 );
  glColor4ubv( purple );
  glVertex3fv( v7 );

  glColor4ubv( white );
  glVertex3fv( v3 );
  glColor4ubv( blue );
  glVertex3fv( v2 );
  glColor4ubv( orange );
  glVertex3fv( v6 );

  glColor4ubv( white );
  glVertex3fv( v3 );
  glColor4ubv( orange );
  glVertex3fv( v6 );
  glColor4ubv( purple );
  glVertex3fv( v7 );

  glColor4ubv( green );
  glVertex3fv( v1 );
  glColor4ubv( red );
  glVertex3fv( v0 );
  glColor4ubv( yellow );
  glVertex3fv( v4 );

  glColor4ubv( green );
  glVertex3fv( v1 );
  glColor4ubv( yellow );
  glVertex3fv( v4 );
  glColor4ubv( black );
  glVertex3fv( v5 );

  glEnd( );

  /*
   * EXERCISE:
   * Draw text telling the user that 'Spc'
   * pauses the rotation and 'Esc' quits.
   * Do it using vetors and textured quads.
   */

  /*
   * Swap the buffers. This this tells the driver to
   * render the next frame from the contents of the
   * back-buffer, and to set all rendering operations
   * to occur on what was the front-buffer.
   *
   * Double buffering prevents nasty visual tearing
   * from the application drawing on areas of the
   * screen that are being updated at the same time.
   */
  SDL_GL_SwapBuffers();

  if (inherited::myScreenLock)
    inherited::myScreenLock->unlock();

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

  // reset clipping area
  unclip(target_surface);

  // whole viewport needs a refresh...
  invalidate(myClipRect);

  // remember position of last realization
  myLastAbsolutePosition = std::make_pair(myClipRect.x,
                                          myClipRect.y);
}

void
SDL_GUI_LevelWindow_3D::handleEvent(const SDL_Event& event_in,
                                    RPG_Graphics_IWindow* window_in,
                                    SDL_Rect& dirtyRegion_out)
{
  RPG_TRACE(ACE_TEXT("SDL_GUI_LevelWindow_3D::handleEvent"));

  // init return value(s)
  ACE_OS::memset(&dirtyRegion_out, 0, sizeof(dirtyRegion_out));

  RPG_Engine_EntityID_t entity_id = 0;
  SDL_Rect dirty_region;
  switch (event_in.type)
  {
    // *** keyboard ***
    case SDL_KEYDOWN:
    {
      switch (event_in.key.keysym.sym)
      {
        // implement keypad navigation
        case SDLK_c:
        case SDLK_UP:
        case SDLK_DOWN:
        case SDLK_LEFT:
        case SDLK_RIGHT:
        {
          bool redraw = false;
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
              redraw = true;

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

            RPG_Engine_Action_t player_action;
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
              case DIRECTION_INVALID: // fell-through case
                break;
              default:
              {
                ACE_DEBUG((LM_ERROR,
                           ACE_TEXT("invalid direction (was: \"%s\"), aborting\n"),
                           ACE_TEXT(RPG_Map_Common_Tools::direction2String(direction).c_str())));

                break;
              }
            } // end SWITCH
            // position valid ?
            RPG_Map_Element element =
                myEngine->getElement(player_action.position);
            if ((element == MAPELEMENT_FLOOR) ||
                (element == MAPELEMENT_DOOR))
            {
              if (element == MAPELEMENT_DOOR)
              {
                RPG_Map_DoorState door_state =
                    myEngine->state(player_action.position, true);
                if ((door_state == DOORSTATE_CLOSED) ||
                    (door_state == DOORSTATE_LOCKED))
                  break;
              } // end IF

              myEngine->action(entity_id, player_action);

//              setView(myEngine->getPosition(entity_id));
            } // end IF
          } // end IF
          else
            redraw = true;

          if (redraw)
          {
            draw();
            getArea(dirtyRegion_out);
            RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->reset();
          } // end IF

          break;
        }
        case SDLK_d:
        {
          myState->debug = !myState->debug;

          // redraw
          draw();
          getArea(dirtyRegion_out);
          RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->reset();

          break;
        }
        case SDLK_f:
        {
          if (event_in.key.keysym.mod & KMOD_SHIFT)
          {
            // toggle setting
            myHideFloor = !myHideFloor;

            // redraw
            draw();
            getArea(dirtyRegion_out);
            RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->reset();

            break;
          } // end IF

          myState->style.floor =
              static_cast<RPG_Graphics_FloorStyle>(myState->style.floor + 1);
          if (myState->style.floor == RPG_GRAPHICS_FLOORSTYLE_MAX)
            myState->style.floor = static_cast<RPG_Graphics_FloorStyle>(0);
          RPG_Graphics_StyleUnion style;
          style.discriminator = RPG_Graphics_StyleUnion::FLOORSTYLE;
          style.floorstyle = myState->style.floor;
          setStyle(style);

          // redraw
          draw();
          getArea(dirtyRegion_out);
          RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->reset();

          break;
        }
        case SDLK_h:
        {
          // toggle setting
          myState->style.half_height_walls =
              !myState->style.half_height_walls;

          RPG_Graphics_StyleUnion new_style;
          new_style.discriminator = RPG_Graphics_StyleUnion::WALLSTYLE;
          new_style.wallstyle = myState->style.wall;
          setStyle(new_style);

          // redraw
          draw();
          getArea(dirtyRegion_out);
          RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->reset();

          break;
        }
        case SDLK_m:
        {
          myMinimapIsOn = !myMinimapIsOn;

          // redraw
          draw();
          getArea(dirtyRegion_out);

          break;
        }
        case SDLK_w:
        {
          if (event_in.key.keysym.mod & KMOD_SHIFT)
          {
            // toggle setting
            myHideWalls = !myHideWalls;

            // redraw
            draw();
            getArea(dirtyRegion_out);

            break;
          } // end IF

          myState->style.wall =
              static_cast<RPG_Graphics_WallStyle>(myState->style.wall + 1);
          if (myState->style.wall == RPG_GRAPHICS_WALLSTYLE_MAX)
            myState->style.wall = static_cast<RPG_Graphics_WallStyle>(0);
          RPG_Graphics_StyleUnion style;
          style.discriminator = RPG_Graphics_StyleUnion::WALLSTYLE;
          style.wallstyle = myState->style.wall;
          setStyle(style);

          // redraw
          draw();
          getArea(dirtyRegion_out);
          RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->reset();

          break;
        }
        default:
          break;
      } // end SWITCH

      break;
    }
    // *** mouse ***
    case SDL_MOUSEMOTION:
    {
      // find map square
      RPG_Graphics_Position_t map_position =
          RPG_Graphics_Common_Tools::screen2Map(std::make_pair(event_in.motion.x,
                                                               event_in.motion.y),
                                                myEngine->getSize(),
                                                std::make_pair(myClipRect.w,
                                                               myClipRect.h),
                                                myView);
      // inside map ?
      if (map_position ==
          std::make_pair(std::numeric_limits<unsigned int>::max(),
                         std::numeric_limits<unsigned int>::max()))
      {
        //// off the map --> remove "active" tile highlight
        //RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->restoreHighlightBG(myView,
        //                                                                      dirtyRegion_out);
        //invalidate(dirtyRegion_out);

        break;
      } // end IF

//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("mouse position [%u,%u] --> [%u,%u]\n"),
//                  event_in.button.x,
//                  event_in.button.y,
//                  map_position.first,
//                  map_position.second));

      // (re-)draw "active" tile highlight ?
      SDL_Rect dirty_region;
      ACE_OS::memset(&dirty_region, 0, sizeof(dirty_region));
      if (map_position !=
          RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->getHighlightBGPosition())
      {
        // unmapped area ?
        RPG_Map_Element map_element = myEngine->getElement(map_position);
  //      ACE_DEBUG((LM_DEBUG,
  //                 ACE_TEXT("map element [%u,%u]: \"%s\"\n"),
  //                 map_position.first, map_position.second,
  //                 ACE_TEXT(RPG_Map_Common_Tools::mapElement2String(map_element).c_str())));
        if ((map_element == MAPELEMENT_UNMAPPED) ||
            (map_element == MAPELEMENT_WALL))
        {
          RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->restoreHighlightBG(myView,
                                                                                dirty_region);
          if ((dirty_region.x != 0) ||
              (dirty_region.y != 0) ||
              (dirty_region.w != 0) ||
              (dirty_region.h != 0))
          {
            dirtyRegion_out =
                RPG_Graphics_SDL_Tools::boundingBox(dirty_region, dirtyRegion_out);
            invalidate(dirty_region);
          } // end IF
        } // end IF
        else
        {
          RPG_Graphics_Offset_t highlight_position =
            RPG_Graphics_Common_Tools::map2Screen(map_position,
                                                  std::make_pair(myClipRect.w,
                                                                 myClipRect.h),
                                                  myView);
          if (highlight_position !=
						  std::make_pair(std::numeric_limits<int>::max(),
							               std::numeric_limits<int>::max()))
          {
						RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->restoreBG(dirty_region,
							                                                           NULL);
            dirtyRegion_out =
                RPG_Graphics_SDL_Tools::boundingBox(dirty_region,
                                                    dirtyRegion_out);

						RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->putHighlight(map_position,
                                                                            highlight_position,
                                                                            myView,
                                                                            dirty_region,
                                                                            myState->debug);
            dirtyRegion_out =
                RPG_Graphics_SDL_Tools::boundingBox(dirty_region,
                                                    dirtyRegion_out);
            invalidate(dirty_region);

						// invalidate cursor bg
						RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->updateBG(NULL);
          } // end IF
        } // end IF
      } // end IF

      // set an appropriate cursor
      RPG_Engine_EntityID_t entity_id = myEngine->getActive(true);
      RPG_Graphics_Cursor cursor_type =
          RPG_Client_Common_Tools::getCursor(map_position,
                                             entity_id,
                                             true,
                                             SELECTIONMODE_NORMAL,
                                             *myEngine,
                                             true);
      if (cursor_type !=
            RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->type())
      {
        ACE_OS::memset(&dirty_region, 0, sizeof(dirty_region));
        RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->setCursor(cursor_type,
                                                                     dirty_region);
        if ((dirty_region.x != 0) ||
            (dirty_region.y != 0) ||
            (dirty_region.w != 0) ||
            (dirty_region.h != 0))
        {
          dirtyRegion_out =
						RPG_Graphics_SDL_Tools::boundingBox(dirty_region,
						                                    dirtyRegion_out);
          invalidate(dirty_region);
        } // end IF
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
        RPG_Graphics_Position_t map_position =
            RPG_Graphics_Common_Tools::screen2Map(std::make_pair(event_in.button.x,
                                                                 event_in.button.y),
                                                  myEngine->getSize(),
                                                  std::make_pair(myClipRect.w,
                                                                 myClipRect.h),
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

          RPG_Engine_Action_t action;
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

      // restore/clear (highlight) BG
			//RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->restoreBG(dirtyRegion_out);
      //RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->restoreHighlightBG(myView,
      //                                                                      dirtyRegion_out);
      //RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->resetHighlightBG(std::make_pair(std::numeric_limits<unsigned int>::max(),
      //                                                                                   std::numeric_limits<unsigned int>::max()));
      //invalidate(dirtyRegion_out);

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
      break;
  } // end SWITCH

  // pass events to any children
  ACE_OS::memset(&dirty_region, 0, sizeof(dirty_region));
  inherited::handleEvent(event_in,
                         window_in,
                         dirty_region);
  dirtyRegion_out = RPG_Graphics_SDL_Tools::boundingBox(dirty_region,
                                                        dirtyRegion_out);
}

void
SDL_GUI_LevelWindow_3D::initTiles()
{
	RPG_TRACE(ACE_TEXT("SDL_GUI_LevelWindow_3D::initTiles"));

	ACE_ASSERT(myEngine);

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
}

//void
//SDL_GUI_LevelWindow_3D::redraw()
//{
//  RPG_TRACE(ACE_TEXT("SDL_GUI_LevelWindow_3D::redraw"));
//
////   draw();
////   refresh();
//}

void
SDL_GUI_LevelWindow_3D::setView(const RPG_Map_Position_t& position_in)
{
  RPG_TRACE(ACE_TEXT("SDL_GUI_LevelWindow_3D::setView"));

  myView = position_in;
}

void
SDL_GUI_LevelWindow_3D::center()
{
	RPG_TRACE(ACE_TEXT("SDL_GUI_LevelWindow_3D::center"));

	ACE_ASSERT(myEngine);

  // init view
  RPG_Map_Size_t size = myEngine->getSize(true);
  setView((size.first  / 2),
          (size.second / 2));
}

void
SDL_GUI_LevelWindow_3D::init(const RPG_Graphics_Style& style_in)
{
  RPG_TRACE(ACE_TEXT("SDL_GUI_LevelWindow_3D::init"));

  // init style
  RPG_Graphics_StyleUnion style;
  style.discriminator = RPG_Graphics_StyleUnion::FLOORSTYLE;
  style.floorstyle = style_in.floor;
  if (!setStyle(style))
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_GUI_LevelWindow_Isometric::setStyle(FLOORSTYLE), continuing\n")));
  style.discriminator = RPG_Graphics_StyleUnion::EDGESTYLE;
  style.edgestyle = style_in.edge;
  if (!setStyle(style))
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_GUI_LevelWindow_Isometric::setStyle(EDGESTYLE), continuing\n")));
  style.discriminator = RPG_Graphics_StyleUnion::WALLSTYLE;
  style.wallstyle = style_in.wall;
  if (!setStyle(style))
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_GUI_LevelWindow_Isometric::setStyle(WALLSTYLE), continuing\n")));
  style.discriminator = RPG_Graphics_StyleUnion::DOORSTYLE;
  style.doorstyle = style_in.door;
  if (!setStyle(style))
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_GUI_LevelWindow_Isometric::setStyle(DOORSTYLE), continuing\n")));

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
}

void
SDL_GUI_LevelWindow_3D::notify(const RPG_Engine_Command& command_in,
                               const RPG_Engine_ClientNotificationParameters_t& parameters_in)
{
  RPG_TRACE(ACE_TEXT("SDL_GUI_LevelWindow_3D::notify"));

  switch (command_in)
  {
    case COMMAND_ATTACK:
      return;
    case COMMAND_DOOR_CLOSE:
    case COMMAND_DOOR_OPEN:
    {
      RPG_Map_DoorState door_state =
          myEngine->state(parameters_in.position, true);

      // change tile accordingly
      RPG_Graphics_Orientation orientation =
          RPG_Client_Common_Tools::getDoorOrientation(*myEngine,
                                                      parameters_in.position);
      switch (orientation)
      {
        case ORIENTATION_HORIZONTAL:
        {
          myDoorTiles[parameters_in.position] =
              ((door_state == DOORSTATE_OPEN) ? myCurrentDoorSet.horizontal_open
                                              : myCurrentDoorSet.horizontal_closed);
          break;
        }
        case ORIENTATION_VERTICAL:
        {
          myDoorTiles[parameters_in.position] =
              ((door_state == DOORSTATE_OPEN) ? myCurrentDoorSet.vertical_open
                                              : myCurrentDoorSet.vertical_closed);
          break;
        }
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
		case COMMAND_E2C_INIT:
		{
			initTiles();

			break;
		}
    case COMMAND_E2C_ENTITY_ADD:
    {
      SDL_Surface* sprite_graphic = NULL;
      RPG_Graphics_GraphicTypeUnion type;
      type.discriminator = RPG_Graphics_GraphicTypeUnion::SPRITE;
      if (!myEngine->isMonster(parameters_in.entity_id, true))
        type.sprite = parameters_in.sprite;
      else
      {
        type.sprite =
            RPG_Client_Common_Tools::monster2Sprite(myEngine->getName(parameters_in.entity_id,
                                                                      true));
      } // end ELSE
      sprite_graphic =
          RPG_Graphics_Common_Tools::loadGraphic(type,   // sprite
                                                 true,   // convert to display format
                                                 false); // don't cache
      ACE_ASSERT(sprite_graphic);
      RPG_CLIENT_ENTITY_MANAGER_SINGLETON::instance()->add(parameters_in.entity_id,
                                                           sprite_graphic);

      return;
    }
    case COMMAND_E2C_ENTITY_HIT:
    case COMMAND_E2C_ENTITY_MISS:
      return;
    case COMMAND_E2C_ENTITY_POSITION:
    {
      SDL_Rect dirty_region;
      ACE_OS::memset(&dirty_region, 0, sizeof(dirty_region));
      RPG_CLIENT_ENTITY_MANAGER_SINGLETON::instance()->put(parameters_in.entity_id,
                                                           RPG_Graphics_Common_Tools::map2Screen(parameters_in.position,
                                                                                                 std::make_pair(myClipRect.w,
																																																                myClipRect.h),
                                                                                                 getView()),
                                                           dirty_region);

      RPG_Graphics_Surface::update(dirty_region,
                                   getScreen());

      break;
    }
    case COMMAND_E2C_ENTITY_REMOVE:
    {
      SDL_Rect dirty_region;
      ACE_OS::memset(&dirty_region, 0, sizeof(dirty_region));
      RPG_CLIENT_ENTITY_MANAGER_SINGLETON::instance()->remove(parameters_in.entity_id,
                                                              dirty_region);
      RPG_Graphics_Surface::update(dirty_region,
                                   getScreen());

      return;
    }
    case COMMAND_E2C_ENTITY_VISION:
    {
      return;
    }
    case COMMAND_E2C_QUIT:
    {
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
SDL_GUI_LevelWindow_3D::toggleDoor(const RPG_Map_Position_t& position_in)
{
  RPG_TRACE(ACE_TEXT("SDL_GUI_LevelWindow_3D::toggleDoor"));

  ACE_ASSERT(myEngine);

  bool is_open = (myEngine->state(position_in) == DOORSTATE_OPEN);

  // change tile accordingly
  RPG_Graphics_Orientation orientation = RPG_GRAPHICS_ORIENTATION_INVALID;
  orientation = RPG_Client_Common_Tools::getDoorOrientation(*myEngine,
                                                            position_in);
  switch (orientation)
  {
    case ORIENTATION_HORIZONTAL:
      myDoorTiles[position_in] = (is_open ? myCurrentDoorSet.horizontal_open
                                          : myCurrentDoorSet.horizontal_closed); break;
    case ORIENTATION_VERTICAL:
      myDoorTiles[position_in] = (is_open ? myCurrentDoorSet.vertical_open
                                          : myCurrentDoorSet.vertical_closed); break;
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid door orientation \"%s\", aborting\n"),
                 ACE_TEXT(RPG_Graphics_OrientationHelper::RPG_Graphics_OrientationToString(orientation).c_str())));

      return;
    }
  } // end SWITCH
}

void
SDL_GUI_LevelWindow_3D::setBlendRadius(const unsigned char& radius_in)
{
  RPG_TRACE(ACE_TEXT("SDL_GUI_LevelWindow_3D::setBlendRadius"));

  // *NOTE*: should NEVER be reached !
  ACE_ASSERT(false);

#if defined (_MSC_VER)
  return;
#else
  ACE_NOTREACHED(return;)
#endif
}

void
SDL_GUI_LevelWindow_3D::updateMinimap()
{
  RPG_TRACE(ACE_TEXT("SDL_GUI_LevelWindow_3D::updateMinimap"));

  // *NOTE*: should NEVER be reached !
  ACE_ASSERT(false);

#if defined (_MSC_VER)
  return;
#else
  ACE_NOTREACHED(return;)
#endif
}

void
SDL_GUI_LevelWindow_3D::updateMessageWindow(const std::string& message_in)
{
  RPG_TRACE(ACE_TEXT("SDL_GUI_LevelWindow_3D::updateMessageWindow"));

  // *NOTE*: should NEVER be reached !
  ACE_ASSERT(false);

#if defined (_MSC_VER)
  return;
#else
  ACE_NOTREACHED(return;)
#endif
}

void
SDL_GUI_LevelWindow_3D::clear()
{
  RPG_TRACE(ACE_TEXT("SDL_GUI_LevelWindow_3D::clear"));

  // init clipping
  SDL_Rect old_clip_rect;
  SDL_GetClipRect(myScreen, &old_clip_rect);

  if (!SDL_SetClipRect(myScreen, &myClipRect))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_SetClipRect(): %s, aborting\n"),
               SDL_GetError()));

    return;
  } // end IF

  if (SDL_FillRect(myScreen,                                     // target surface
                   &myClipRect,                                  // rectangle
                   RPG_Graphics_SDL_Tools::getColor(COLOR_BLACK,
                                                    *myScreen))) // color
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

  invalidate(myClipRect);
}

bool
SDL_GUI_LevelWindow_3D::setStyle(const RPG_Graphics_StyleUnion& style_in)
{
  RPG_TRACE(ACE_TEXT("SDL_GUI_LevelWindow_3D::setStyle"));

  switch (style_in.discriminator)
  {
    case RPG_Graphics_StyleUnion::EDGESTYLE:
    {
      RPG_Graphics_Common_Tools::loadFloorEdgeTileSet(style_in.edgestyle,
                                                      myCurrentFloorEdgeSet);
      // sanity check
      if ((myCurrentFloorEdgeSet.west.surface == NULL)       &&
          (myCurrentFloorEdgeSet.north.surface == NULL)      &&
          (myCurrentFloorEdgeSet.east.surface == NULL)       &&
          (myCurrentFloorEdgeSet.south.surface == NULL)      &&
          (myCurrentFloorEdgeSet.south_west.surface == NULL) &&
          (myCurrentFloorEdgeSet.south_east.surface == NULL) &&
          (myCurrentFloorEdgeSet.north_west.surface == NULL) &&
          (myCurrentFloorEdgeSet.north_east.surface == NULL) &&
          (myCurrentFloorEdgeSet.top.surface == NULL)        &&
          (myCurrentFloorEdgeSet.right.surface == NULL)      &&
          (myCurrentFloorEdgeSet.left.surface == NULL)       &&
          (myCurrentFloorEdgeSet.bottom.surface == NULL))
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("edge-style \"%s\" has no tiles, continuing\n"),
                   ACE_TEXT(RPG_Graphics_EdgeStyleHelper::RPG_Graphics_EdgeStyleToString(style_in.edgestyle).c_str())));

        return false;
      } // end IF
      myState->style.edge = style_in.edgestyle;

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

        return false;
      } // end IF
      myState->style.floor = style_in.floorstyle;

      break;
    }
    case RPG_Graphics_StyleUnion::WALLSTYLE:
    {
      RPG_Graphics_Common_Tools::loadWallTileSet(style_in.wallstyle,
                                                 myState->style.half_height_walls,
                                                 myCurrentWallSet);
      // sanity check
      if ((myCurrentWallSet.east.surface == NULL)  ||
          (myCurrentWallSet.west.surface == NULL)  ||
          (myCurrentWallSet.north.surface == NULL) ||
          (myCurrentWallSet.south.surface == NULL))
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("wall-style \"%s\" is incomplete, aborting\n"),
                   ACE_TEXT(RPG_Graphics_WallStyleHelper::RPG_Graphics_WallStyleToString(style_in.wallstyle).c_str())));

        return false;
      } // end IF

      initWallBlend(myState->style.half_height_walls);

//#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
//      std::string dump_path_base = ACE_TEXT_ALWAYS_CHAR(RPG_COMMON_DEF_DUMP_DIR);
//#else
//      std::string dump_path_base = ACE_OS::getenv(ACE_TEXT_ALWAYS_CHAR(RPG_COMMON_DEF_DUMP_DIR));
//#endif
//      dump_path_base += ACE_DIRECTORY_SEPARATOR_CHAR_A;
//      std::string dump_path;
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

        return false;
      } // end IF
      if (SDL_BlitSurface(myWallBlend,
                          NULL,
                          copy,
                          NULL))
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to SDL_BlitSurface(): \"%s\", aborting\n"),
                   ACE_TEXT(SDL_GetError())));

        // clean up
        SDL_FreeSurface(copy);

        return false;
      } // end IF
      SDL_FreeSurface(myCurrentWallSet.west.surface);
      myCurrentWallSet.west.surface = copy;

      // *NOTE*: NORTH is just a "darkened" version of SOUTH...
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
                   ACE_TEXT("failed to SDL_BlitSurface(): \"%s\", aborting\n"),
                   ACE_TEXT(SDL_GetError())));

        // clean up
        SDL_FreeSurface(copy);

        return false;
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
      shaded_wall =
          RPG_Graphics_Surface::alpha(*myCurrentWallSet.east.surface,
                                      static_cast<Uint8>((RPG_GRAPHICS_TILE_DEF_WALL_SE_OPACITY * SDL_ALPHA_OPAQUE)));
      if (!shaded_wall)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to RPG_Graphics_Surface::alpha(%u), aborting\n"),
                   static_cast<Uint8>((RPG_GRAPHICS_TILE_DEF_WALL_SE_OPACITY * SDL_ALPHA_OPAQUE))));

        return false;
      } // end IF
      SDL_FreeSurface(myCurrentWallSet.east.surface);
      myCurrentWallSet.east.surface = shaded_wall;

      // adjust WEST wall opacity
      shaded_wall =
          RPG_Graphics_Surface::alpha(*myCurrentWallSet.west.surface,
                                      static_cast<Uint8>((RPG_GRAPHICS_TILE_DEF_WALL_NW_OPACITY * SDL_ALPHA_OPAQUE)));
      if (!shaded_wall)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to RPG_Graphics_Surface::alpha(%u), aborting\n"),
                   static_cast<Uint8>((RPG_GRAPHICS_TILE_DEF_WALL_NW_OPACITY * SDL_ALPHA_OPAQUE))));

        return false;
      } // end IF
      SDL_FreeSurface(myCurrentWallSet.west.surface);
      myCurrentWallSet.west.surface = shaded_wall;

      // adjust SOUTH wall opacity
      shaded_wall =
          RPG_Graphics_Surface::alpha(*myCurrentWallSet.south.surface,
                                      static_cast<Uint8>((RPG_GRAPHICS_TILE_DEF_WALL_SE_OPACITY * SDL_ALPHA_OPAQUE)));
      if (!shaded_wall)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to RPG_Graphics_Surface::alpha(%u), aborting\n"),
                   static_cast<Uint8>((RPG_GRAPHICS_TILE_DEF_WALL_SE_OPACITY * SDL_ALPHA_OPAQUE))));

        return false;
      } // end IF
      SDL_FreeSurface(myCurrentWallSet.south.surface);
      myCurrentWallSet.south.surface = shaded_wall;

      // adjust NORTH wall opacity
      shaded_wall =
          RPG_Graphics_Surface::alpha(*myCurrentWallSet.north.surface,
                                      static_cast<Uint8>((RPG_GRAPHICS_TILE_DEF_WALL_NW_OPACITY * SDL_ALPHA_OPAQUE)));
      if (!shaded_wall)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to RPG_Graphics_Surface::alpha(%u), aborting\n"),
                   static_cast<Uint8>((RPG_GRAPHICS_TILE_DEF_WALL_NW_OPACITY * SDL_ALPHA_OPAQUE))));

        return false;
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

      myState->style.wall = style_in.wallstyle;

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

      // update door tiles / position
      RPG_Client_Common_Tools::updateDoors(myCurrentDoorSet,
                                           *myEngine,
                                           myDoorTiles);

      myState->style.door = style_in.doorstyle;

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
SDL_GUI_LevelWindow_3D::initCeiling()
{
  RPG_TRACE(ACE_TEXT("SDL_GUI_LevelWindow_3D::initCeiling"));

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
  Uint8 opacity = static_cast<Uint8>(RPG_GRAPHICS_TILE_DEF_WALL_NW_OPACITY *
                                     SDL_ALPHA_OPAQUE);
  shaded_ceiling = RPG_Graphics_Surface::alpha(*myCurrentCeilingTile,
                                               opacity);
  if (!shaded_ceiling)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Graphics_Surface::alpha(%u), aborting\n"),
               opacity));

    // clean up
    SDL_FreeSurface(myCurrentCeilingTile);
    myCurrentCeilingTile = NULL;

    return;
  } // end IF

  SDL_FreeSurface(myCurrentCeilingTile);
  myCurrentCeilingTile = shaded_ceiling;
}

void
SDL_GUI_LevelWindow_3D::initWallBlend(const bool& halfHeightWalls_in)
{
  RPG_TRACE(ACE_TEXT("SDL_GUI_LevelWindow_3D::initWallBlend"));

  // sanity check
  if (myWallBlend)
  {
    SDL_FreeSurface(myWallBlend);
    myWallBlend = NULL;
  } // end IF

  myWallBlend =
      RPG_Graphics_Surface::create(RPG_GRAPHICS_TILE_WALL_WIDTH,
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
                   RPG_Graphics_SDL_Tools::getColor(COLOR_BLACK_A10,
                                                    *myWallBlend)))
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
//   std::string dump_path_base = ACE_TEXT_ALWAYS_CHAR(RPG_COMMON_DEF_DUMP_DIR);
//   dump_path_base += ACE_DIRECTORY_SEPARATOR_CHAR_A;
//   std::string dump_path = dump_path_base;
//   dump_path += ACE_TEXT("wall_blend.png");
//   RPG_Graphics_Surface::savePNG(*myWallBlend, // image
//                                 dump_path,    // file
//                                 true);        // WITH alpha
}

void
SDL_GUI_LevelWindow_3D::initMiniMap(RPG_Engine* engine_in)
{
  RPG_TRACE(ACE_TEXT("SDL_GUI_LevelWindow_3D::initMiniMap"));

  RPG_Graphics_Offset_t offset;
  offset.first = SDL_GUI_DEF_GRAPHICS_MINIMAP_OFFSET_X;
  offset.second = SDL_GUI_DEF_GRAPHICS_MINIMAP_OFFSET_Y;

  SDL_GUI_MinimapWindow* minimap_window = NULL;
  ACE_NEW_NORETURN(minimap_window,
                   SDL_GUI_MinimapWindow(*this,
                                         offset,
                                         engine_in));
  if (!minimap_window)
  {
    ACE_DEBUG((LM_CRITICAL,
               ACE_TEXT("failed to allocate memory(%u): %m, aborting\n"),
               sizeof(SDL_GUI_MinimapWindow)));

    return;
  } // end IF
  // *NOTE* cannot init screen lock (has not been set), do it later...
//  minimap_window->init(inherited::myScreenLock);
}
