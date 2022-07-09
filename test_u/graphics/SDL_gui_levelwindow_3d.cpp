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

#define _USE_MATH_DEFINES
#include <cmath>
#include <sstream>

#include "ace/config-lite.h"
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "gl/GL.h"
#include "gl/GLU.h"
#else
#include "GL/gl.h"
#endif // ACE_WIN32 || ACE_WIN64

#if defined (SDL_USE)
#include "SDL_opengl.h"
#endif // SDL_USE

#include "ace/Log_Msg.h"

#include "rpg_common_defines.h"
#include "rpg_common_macros.h"

#include "rpg_map_common_tools.h"

#include "rpg_engine.h"
#include "rpg_engine_common_tools.h"

#include "rpg_graphics_common_tools.h"
#include "rpg_graphics_cursor_manager.h"
#include "rpg_graphics_defines.h"
#include "rpg_graphics_surface.h"
#include "rpg_graphics_SDL_tools.h"

#include "rpg_client_common_tools.h"
#include "rpg_client_defines.h"
#include "rpg_client_entity_manager.h"

#include "SDL_gui_defines.h"
#include "SDL_gui_minimapwindow.h"

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
   myMinimapIsOn (RPG_CLIENT_MINIMAP_DEF_ISON)
{
  RPG_TRACE (ACE_TEXT ("SDL_GUI_LevelWindow_3D::SDL_GUI_LevelWindow_3D"));

  myEngine->initialize (this);

//  myEngine->lock ();
  RPG_Map_Size_t map_size = myEngine->getSize (true);
//  myEngine->unlock ();
  myView = std::make_pair (map_size.first / 2,
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
               RPG_Graphics_Common_Tools::toString(type).c_str()));

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
  RPG_TRACE(ACE_TEXT("SDL_GUI_LevelWindow_3D::~SDL_GUI_LevelWindow_3D"));

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
SDL_GUI_LevelWindow_3D::setView(int offsetX_in,
                                int offsetY_in,
                                bool lockedAccess_in)
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

  RPG_Map_Size_t map_size = myEngine->getSize(lockedAccess_in);
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
SDL_GUI_LevelWindow_3D::initialize (state_t* state_in,
                                    Common_ILock* screenLock_in)
{
  RPG_TRACE (ACE_TEXT ("SDL_GUI_LevelWindow_3D::init"));

  // sanity check(s)
  ACE_ASSERT (state_in);
  ACE_ASSERT (state_in->screen);

  myState = state_in;
  inherited::initialize (screenLock_in,
#if defined (SDL_USE)
                         (state_in->screen->flags & SDL_DOUBLEBUF));
#elif defined (SDL2_USE)
                         true);
#endif // SDL_USE || SDL2_USE

  GLfloat ratio = (static_cast<float>(clipRectangle_.w) /
                   static_cast<float>(clipRectangle_.h));

  GLenum gl_error = GL_NO_ERROR;
  /* Our shading model--Gouraud (smooth). */
  glShadeModel(GL_SMOOTH);
  gl_error = glGetError();
  if (gl_error != GL_NO_ERROR)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to glShadeModel: \"%s\", continuing\n"),
               ACE_TEXT(gluErrorString(gl_error))));

  /* Culling. */
  glCullFace(GL_BACK);
  gl_error = glGetError();
  if (gl_error != GL_NO_ERROR)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to glCullFace: \"%s\", continuing\n"),
               ACE_TEXT(gluErrorString(gl_error))));
  glFrontFace(GL_CCW);
  gl_error = glGetError();
  if (gl_error != GL_NO_ERROR)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to glFrontFace: \"%s\", continuing\n"),
               ACE_TEXT(gluErrorString(gl_error))));
  glEnable(GL_CULL_FACE);
  gl_error = glGetError();
  if (gl_error != GL_NO_ERROR)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to glEnable: \"%s\", continuing\n"),
               ACE_TEXT(gluErrorString(gl_error))));

  /* Set the clear color. */
  glClearColor(0, 0, 0, 0);
  gl_error = glGetError();
  if (gl_error != GL_NO_ERROR)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to glClearColor: \"%s\", continuing\n"),
               ACE_TEXT(gluErrorString(gl_error))));

  /* Setup our viewport. */
//  ACE_ASSERT(myParent);
//  SDL_Rect parent_area;
//  myParent->getArea(parent_area,
//                    false);
//  myParent->getBorders(myBorderTop,
//                       myBorderBottom,
//                       myBorderLeft,
//                       myBorderRight,
//                       false);
//  glViewport(clipRectangle_.x,
//             (parent_area.w - myBorderBottom),
//             clipRectangle_.w,
//             clipRectangle_.h);
//  myBorderTop = myBorderBottom = myBorderLeft = myBorderRight = 0;
  glViewport(clipRectangle_.x, clipRectangle_.y, clipRectangle_.w, clipRectangle_.h);
  gl_error = glGetError();
  if (gl_error != GL_NO_ERROR)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to glViewport: \"%s\", continuing\n"),
               ACE_TEXT(gluErrorString(gl_error))));

  /*
   * Change to the projection matrix and set
   * our viewing volume.
   */
  glMatrixMode(GL_PROJECTION);
  gl_error = glGetError();
  if (gl_error != GL_NO_ERROR)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to glMatrixMode: \"%s\", continuing\n"),
               ACE_TEXT(gluErrorString(gl_error))));
  glLoadIdentity();
  gl_error = glGetError();
  if (gl_error != GL_NO_ERROR)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to glLoadIdentity: \"%s\", continuing\n"),
               ACE_TEXT(gluErrorString(gl_error))));

  /*
   * EXERCISE:
   * Replace this with a call to glFrustum.
   */
  GLdouble fovY = 60.0F;
  GLdouble zNear = 1.0F;
  GLdouble zFar = 1024.0F;
  //  gluPerspective(fovY, ratio, zNear, zFar);
  GLdouble fH = ::tan(fovY / 360.0F * M_PI) * zNear;
  GLdouble fW = fH * ratio;
  glFrustum(-fW, fW, -fH, fH, zNear, zFar);
  gl_error = glGetError();
  if (gl_error != GL_NO_ERROR)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to glFrustum: \"%s\", continuing\n"),
               ACE_TEXT(gluErrorString(gl_error))));

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
                                   unsigned int offsetX_in,
                                   unsigned int offsetY_in)
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
SDL_GUI_LevelWindow_3D::draw (SDL_Surface* targetSurface_in,
                              unsigned int offsetX_in,
                              unsigned int offsetY_in)
{
  RPG_TRACE (ACE_TEXT ("SDL_GUI_LevelWindow_3D::draw"));

  // sanity check(s)
  ACE_ASSERT (inherited::screen_);
#if defined (SDL_USE)
  SDL_Surface* surface_p = inherited::screen_;
#elif defined (SDL2_USE)
  SDL_Surface* surface_p = SDL_GetWindowSurface (inherited::screen_);
#endif // SDL_USE || SDL2_USE
  ACE_ASSERT (surface_p);
  SDL_Surface* target_surface = (targetSurface_in ? targetSurface_in
                                                  : surface_p);
  ACE_ASSERT (target_surface);
  ACE_ASSERT (myState);
//  ACE_ASSERT(myCurrentOffMapTile);
//  ACE_ASSERT(myCurrentCeilingTile);
  ACE_ASSERT (static_cast<int> (offsetX_in) <= target_surface->w);
  ACE_ASSERT (static_cast<int> (offsetY_in) <= target_surface->h);

  // init clipping
  clip (target_surface,
        offsetX_in,
        offsetY_in);

  if (inherited::screenLock_)
    inherited::screenLock_->lock();

  /* Our angle of rotation. */
//  static GLfloat angle = 0.0F;

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
  static GLfloat v0[] = {-1.0F, -1.0F,  1.0F};
  static GLfloat v1[] = { 1.0F, -1.0F,  1.0F};
  static GLfloat v2[] = { 1.0F,  1.0F,  1.0F};
  static GLfloat v3[] = {-1.0F,  1.0F,  1.0F};
  static GLfloat v4[] = {-1.0F, -1.0F, -1.0F};
  static GLfloat v5[] = { 1.0F, -1.0F, -1.0F};
  static GLfloat v6[] = { 1.0F,  1.0F, -1.0F};
  static GLfloat v7[] = {-1.0F,  1.0F, -1.0F};
  static GLubyte red[]    = {255,   0,   0, 255};
  static GLubyte green[]  = {  0, 255,   0, 255};
  static GLubyte blue[]   = {  0,   0, 255, 255};
  static GLubyte white[]  = {255, 255, 255, 255};
  static GLubyte yellow[] = {  0, 255, 255, 255};
  static GLubyte black[]  = {  0,   0,   0, 255};
  static GLubyte orange[] = {255, 255,   0, 255};
  static GLubyte purple[] = {255,   0, 255,   0};

  /* Clear the color and depth buffers. */
  glClear(GL_COLOR_BUFFER_BIT |
          GL_DEPTH_BUFFER_BIT);
  GLenum gl_error = glGetError();
  if (gl_error != GL_NO_ERROR)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to glClear: \"%s\", continuing\n"),
               ACE_TEXT(gluErrorString(gl_error))));

  /* We don't want to modify the projection matrix. */
  glMatrixMode(GL_MODELVIEW);
  gl_error = glGetError();
  if (gl_error != GL_NO_ERROR)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to glMatrixMode: \"%s\", continuing\n"),
               ACE_TEXT(gluErrorString(gl_error))));
  glLoadIdentity();
  gl_error = glGetError();
  if (gl_error != GL_NO_ERROR)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to glLoadIdentity: \"%s\", continuing\n"),
               ACE_TEXT(gluErrorString(gl_error))));

  /* Move down the z-axis. */
  glTranslatef(0.0, 0.0, -5.0);
  gl_error = glGetError();
  if (gl_error != GL_NO_ERROR)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to glTranslatef: \"%s\", continuing\n"),
               ACE_TEXT(gluErrorString(gl_error))));

  /* Rotate. */
  glRotatef(myState->angle, 0.0, 1.0, 0.0);
  gl_error = glGetError();
  if (gl_error != GL_NO_ERROR)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to glRotatef: \"%s\", continuing\n"),
               ACE_TEXT(gluErrorString(gl_error))));

//  static GLboolean should_rotate = GL_TRUE;
//  if (should_rotate &&
//      (++(myState->angle) > 360.0F))
//    myState->angle = 0.0F;

  /* Send our triangle data to the pipeline. */
  glBegin(GL_TRIANGLES);
  gl_error = glGetError();
  if (gl_error != GL_NO_ERROR)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to glBegin: \"%s\", continuing\n"),
               ACE_TEXT(gluErrorString(gl_error))));

  glColor4ubv(red);
  glVertex3fv(v0);
  glColor4ubv(green);
  glVertex3fv(v1);
  glColor4ubv(blue);
  glVertex3fv(v2);

  glColor4ubv(red);
  glVertex3fv(v0);
  glColor4ubv(blue);
  glVertex3fv(v2);
  glColor4ubv(white);
  glVertex3fv(v3);

  glColor4ubv(green);
  glVertex3fv(v1);
  glColor4ubv(black);
  glVertex3fv(v5);
  glColor4ubv(orange);
  glVertex3fv(v6);

  glColor4ubv(green);
  glVertex3fv(v1);
  glColor4ubv(orange);
  glVertex3fv(v6);
  glColor4ubv(blue);
  glVertex3fv(v2);

  glColor4ubv(black);
  glVertex3fv(v5);
  glColor4ubv(yellow);
  glVertex3fv(v4);
  glColor4ubv(purple);
  glVertex3fv(v7);

  glColor4ubv(black);
  glVertex3fv(v5);
  glColor4ubv(purple);
  glVertex3fv(v7);
  glColor4ubv(orange);
  glVertex3fv(v6);

  glColor4ubv(yellow);
  glVertex3fv(v4);
  glColor4ubv(red);
  glVertex3fv(v0);
  glColor4ubv(white);
  glVertex3fv(v3);

  glColor4ubv(yellow);
  glVertex3fv(v4);
  glColor4ubv(white);
  glVertex3fv(v3);
  glColor4ubv(purple);
  glVertex3fv(v7);

  glColor4ubv(white);
  glVertex3fv(v3);
  glColor4ubv(blue);
  glVertex3fv(v2);
  glColor4ubv(orange);
  glVertex3fv(v6);

  glColor4ubv(white);
  glVertex3fv(v3);
  glColor4ubv(orange);
  glVertex3fv(v6);
  glColor4ubv(purple);
  glVertex3fv(v7);

  glColor4ubv(green);
  glVertex3fv(v1);
  glColor4ubv(red);
  glVertex3fv(v0);
  glColor4ubv(yellow);
  glVertex3fv(v4);

  glColor4ubv(green);
  glVertex3fv(v1);
  glColor4ubv(yellow);
  glVertex3fv(v4);
  glColor4ubv(black);
  glVertex3fv(v5);

  glEnd();
  gl_error = glGetError();
//  if (gl_error != GL_NO_ERROR)
//    ACE_DEBUG((LM_ERROR,
//               ACE_TEXT("failed to glEnd: \"%s\", continuing\n"),
//               ACE_TEXT(gluErrorString(gl_error))));

  /*
   * EXERCISE:
   * Draw text telling the user that 'Spc'
   * pauses the rotation and 'Esc' quits.
   * Do it using vetors and textured quads.
   */

  if (inherited::screenLock_)
    inherited::screenLock_->unlock();

  // realize any sub-windows
  for (RPG_Graphics_WindowsIterator_t iterator = inherited::children_.begin();
       iterator != inherited::children_.end();
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

  // remember position of last realization
  lastAbsolutePosition_ = std::make_pair(clipRectangle_.x,
                                         clipRectangle_.y);
}

void
SDL_GUI_LevelWindow_3D::update (SDL_Surface* targetSurface_in)
{
  RPG_TRACE(ACE_TEXT("SDL_GUI_LevelWindow_3D::update"));

  ACE_UNUSED_ARG (targetSurface_in);

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
#if defined (SDL_USE)
  SDL_GL_SwapBuffers ();
#elif defined (SDL2_USE)
  SDL_GL_SwapWindow (inherited::screen_);
#endif // SDL_USE
}

void
SDL_GUI_LevelWindow_3D::handleEvent(const SDL_Event& event_in,
                                    RPG_Graphics_IWindowBase* window_in,
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
          RPG_Map_Direction direction = RPG_MAP_DIRECTION_INVALID;
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

            struct RPG_Engine_Action player_action;
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
              case RPG_MAP_DIRECTION_INVALID: // fell-through case
                break;
              default:
              {
                ACE_DEBUG((LM_ERROR,
                           ACE_TEXT("invalid direction (was: \"%s\"), aborting\n"),
                           ACE_TEXT(RPG_Map_DirectionHelper::RPG_Map_DirectionToString(direction).c_str())));

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
            RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->reset();

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
      SDL_Rect window_area;
      getArea (window_area, true);
//      myEngine->lock ();
      RPG_Graphics_Position_t map_position =
        RPG_Graphics_Common_Tools::screenToMap (std::make_pair (event_in.motion.x,
                                                                event_in.motion.y),
                                                myEngine->getSize (false),
                                                std::make_pair (window_area.w,
                                                                window_area.h),
                                                myView);
      //       ACE_DEBUG((LM_DEBUG,
      //                  ACE_TEXT("mouse position [%u,%u] --> [%u,%u]\n"),
      //                  event_in.button.x, event_in.button.y,
      //                  map_position.first, map_position.second));

      // inside map ?
      if (map_position ==
          std::make_pair (std::numeric_limits<unsigned int>::max (),
                          std::numeric_limits<unsigned int>::max ()))
      {
        // clean up
//        myEngine->unlock ();

        break; // off-map
      } // end IF
      // (re-)draw "active" tile highlight(s) ?
      if ((map_position ==
           RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance ()->getHighlightBGPosition ()) &&
          (event_in.motion.which != std::numeric_limits<unsigned char>::max ())) // fake event ?
      {
        // clean up
//        myEngine->unlock ();

        break; // same map square/not initiating pathing --> nothing to do...
      } // end IF

      // *NOTE*: --> (re-)draw/remove tile highlight(s)...

      bool toggle_on = myEngine->isValid (map_position, false);
      RPG_Map_Positions_t positions;
      RPG_Graphics_Offsets_t screen_positions;
      // unmapped area/invalid position ?
      if (toggle_on)
      {
        // *NOTE*: essentially, this means:
        // - the cursor bg is dirty, needs an update
        // - the highlight needs to be redrawn
        // - the cursor needs to be redrawn
        // --> handled by the cursor manager

        // step1: determine the highlighted area
        switch (myState->selection_mode)
        {
          case SELECTIONMODE_AIM_CIRCLE:
          {
            // step1: build circle
            unsigned int selection_radius =
                RPG_Map_Common_Tools::distanceMax(myState->source,
                                                  map_position);
            if (selection_radius > RPG_MAP_CIRCLE_MAX_RADIUS)
              selection_radius = RPG_MAP_CIRCLE_MAX_RADIUS;

            RPG_Map_Common_Tools::buildCircle(myState->source,
                                              myEngine->getSize(false),
                                              selection_radius,
                                              false, // don't fill
                                              positions);

            // *WARNING*: falls through !
          }
          case SELECTIONMODE_AIM_SQUARE:
          {
            if (myState->selection_mode == SELECTIONMODE_AIM_SQUARE)
            {
              // step1: build square
              unsigned int selection_radius =
                  RPG_Map_Common_Tools::distanceMax(myState->source,
                                                    map_position);
              RPG_Map_Common_Tools::buildSquare(myState->source,
                                                myEngine->getSize(false),
                                                selection_radius,
                                                false, // don't fill
                                                positions);
            } // end IF

            // step2: remove invalid positions
            RPG_Map_Positions_t obstacles = myEngine->getObstacles(false,
                                                                   false);
            // *WARNING*: this works for associative containers ONLY
            for (RPG_Map_PositionsIterator_t iterator = positions.begin();
                 iterator != positions.end();
                 )
              if (RPG_Map_Common_Tools::hasLineOfSight(myState->source,
                                                       *iterator,
                                                       obstacles,
                                                       false))
                iterator++;
              else
                positions.erase(iterator++);
            myState->positions.insert(myState->positions.begin(),
                                      positions.begin(),
                                      positions.end());

            // *WARNING*: falls through !
          }
          case SELECTIONMODE_PATH:
          {
            if (myState->selection_mode == SELECTIONMODE_PATH)
            {
              //// step1: build path ?
              //RPG_Engine_EntityID_t entity_id = myEngine->getActive(false);
              //if (!entity_id ||
              //		!hasSeen(entity_id,
              //		map_position))
              //{
              //	toggle_on = false; // --> remove highlights

              //	break;
              //} // end IF

              RPG_Map_Position_t current_position =
                  myEngine->getPosition(entity_id, false);
              if (current_position != map_position)
              {
                if (!myEngine->findPath(current_position,
                                        map_position,
                                        myState->path,
                                        false))
                {
                  ACE_DEBUG((LM_DEBUG,
                             ACE_TEXT("could not find a path [%u,%u] --> [%u,%u], continuing\n"),
                             current_position.first, current_position.second,
                             map_position.first, map_position.second));

                  // pointing at an invalid (i.e. unreachable) position (still on the map though)
                  // --> erase cached path (and tile highlights)
                  //	RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->resetHighlightBG(map_position);
                  myState->path.clear();
                } // end IF
              } // end IF
              else
              {
                // pointing at curent position
                // --> erase cached path (and tile highlights)
                //RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->resetHighlightBG(map_position);
                myState->path.clear();
              } // end ELSE

              // step2: compute (screen) positions
              myState->positions.clear();
              for (RPG_Map_PathConstIterator_t iterator = myState->path.begin();
                   iterator != myState->path.end();
                   iterator++)
              {
                myState->positions.push_back ((*iterator).first);
                screen_positions.push_back (RPG_Graphics_Common_Tools::mapToScreen ((*iterator).first,
                                                                                    std::make_pair (window_area.w,
                                                                                                    window_area.h),
                                                                                    myView));
              } // end FOR
            } // end IF
            else
            {
              // step3: compute screen positions
              for (RPG_Map_PositionListIterator_t iterator = myState->positions.begin();
                   iterator != myState->positions.end();
                   iterator++)
                screen_positions.push_back (RPG_Graphics_Common_Tools::mapToScreen (*iterator,
                                                                                    std::make_pair (window_area.w,
                                                                                                    window_area.h),
                                                                                    myView));
            } // end ELSE

            break;
          }
          case SELECTIONMODE_NORMAL:
          {
            // step2: compute screen position
            screen_positions.push_back (RPG_Graphics_Common_Tools::mapToScreen (map_position,
                                                                                std::make_pair (window_area.w,
                                                                                                window_area.h),
                                                                                myView));
            break;
          }
          default:
          {
            ACE_DEBUG((LM_ERROR,
                       ACE_TEXT("invalid selection mode (was: %d), aborting\n"),
                       myState->selection_mode));

            // clean up
//            myEngine->unlock ();

            break;
          }
        } // end SWITCH
      } // end IF

      // step2: draw/remove highlight(s)
      screenLock_->lock();
      if (toggle_on)
        RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->putHighlights (myState->positions,
                                                                          screen_positions,
                                                                          myView,
                                                                          dirtyRegion_out,
                                                                          false,
                                                                          myState->debug);
      else // --> moved into an "invalid" map position (i.e. unmapped, closed door, wall, ...)
        RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->restoreHighlightBG (myView,
                                                                               dirtyRegion_out,
                                                                               NULL,
                                                                               false,
                                                                               myState->debug);
      screenLock_->unlock ();
      invalidate (dirtyRegion_out);

      // step3: set an appropriate cursor
      entity_id = myEngine->getActive(false);
      RPG_Graphics_Cursor cursor_type =
          RPG_Client_Common_Tools::getCursor (map_position,
                                              entity_id,
                                              true,
                                              myState->selection_mode,
                                              *myEngine,
                                              false);
//      myEngine->unlock();
      if (cursor_type !=
          RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance ()->type ())
      {
        ACE_OS::memset (&dirty_region, 0, sizeof (SDL_Rect));
        RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->setCursor (cursor_type,
                                                                      dirty_region,
                                                                      true);
        dirtyRegion_out =
            RPG_Graphics_SDL_Tools::boundingBox (dirty_region,
                                                 dirtyRegion_out);
        invalidate (dirty_region);
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
            RPG_Graphics_Common_Tools::screenToMap (std::make_pair (event_in.button.x,
                                                                    event_in.button.y),
                                                    myEngine->getSize (),
                                                    std::make_pair (clipRectangle_.w,
                                                                    clipRectangle_.h),
                                                    myView);

        ACE_DEBUG ((LM_DEBUG,
                    ACE_TEXT ("mouse position [%u,%u] --> [%u,%u]\n"),
                    event_in.button.x, event_in.button.y,
                    map_position.first, map_position.second));

        // closed door ? --> (try to) open it
        if (myEngine->getElement(map_position) == MAPELEMENT_DOOR)
        {
          RPG_Map_DoorState door_state = myEngine->state(map_position, true);

          struct RPG_Engine_Action action;
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
    case SDL_KEYUP:
    case SDL_MOUSEBUTTONUP:
    case SDL_JOYAXISMOTION:
    case SDL_JOYBALLMOTION:
    case SDL_JOYHATMOTION:
    case SDL_JOYBUTTONDOWN:
    case SDL_JOYBUTTONUP:
    case SDL_QUIT:
    case SDL_SYSWMEVENT:
#if defined (SDL_USE)
    case SDL_ACTIVEEVENT:
    case SDL_VIDEORESIZE:
    case SDL_VIDEOEXPOSE:
#elif defined (SDL2_USE)
    case SDL_WINDOWEVENT_SHOWN:
    case SDL_WINDOWEVENT_RESIZED:
    case SDL_WINDOWEVENT_EXPOSED:
#endif // SDL_USE || SDL2_USE
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
SDL_GUI_LevelWindow_3D::initialize(const RPG_Graphics_Style& style_in)
{
  RPG_TRACE(ACE_TEXT("SDL_GUI_LevelWindow_3D::initialize"));

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
SDL_GUI_LevelWindow_3D::notify(enum RPG_Engine_Command command_in,
                               const struct RPG_Engine_ClientNotificationParameters& parameters_in)
{
  RPG_TRACE(ACE_TEXT("SDL_GUI_LevelWindow_3D::notify"));

  // sanity check(s)
  ACE_ASSERT(myEngine);

  switch (command_in)
  {
    case COMMAND_ATTACK:
      return;
    case COMMAND_DOOR_CLOSE:
    case COMMAND_DOOR_OPEN:
    {
      RPG_Map_Position_t position = *parameters_in.positions.begin();
//      myEngine->lock();
      RPG_Map_DoorState door_state = myEngine->state(position, false);

      // change tile accordingly
      RPG_Graphics_Orientation orientation =
          RPG_Client_Common_Tools::getDoorOrientation(position,
                                                      *myEngine,
                                                      false);
//      myEngine->unlock();
      switch (orientation)
      {
        case ORIENTATION_HORIZONTAL:
        {
          myDoorTiles[position] =
              ((door_state == DOORSTATE_OPEN) ? myCurrentDoorSet.horizontal_open
                                              : myCurrentDoorSet.horizontal_closed);
          break;
        }
        case ORIENTATION_VERTICAL:
        {
          myDoorTiles[position] =
              ((door_state == DOORSTATE_OPEN) ? myCurrentDoorSet.vertical_open
                                              : myCurrentDoorSet.vertical_closed);
          break;
        }
        default:
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("invalid door orientation \"%s\", returning\n"),
                     ACE_TEXT(RPG_Graphics_OrientationHelper::RPG_Graphics_OrientationToString(orientation).c_str())));

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
//      myEngine->lock();
      type.sprite =
          (myEngine->isMonster(parameters_in.entity_id, false) ? RPG_Client_Common_Tools::classToSprite(myEngine->getClass(parameters_in.entity_id,
                                                                                                                          false))
                                                               : RPG_Client_Common_Tools::monsterToSprite(myEngine->getName(parameters_in.entity_id,
                                                                                                                           false)));
//      myEngine->unlock();
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
                                                           RPG_Graphics_Common_Tools::mapToScreen (*parameters_in.positions.begin (),
                                                                                                   std::make_pair (clipRectangle_.w,
                                                                                                                   clipRectangle_.h),
                                                                                                   getView ()),
                                                           dirty_region,
                                                           true,
                                                           true,
                                                           myState->debug);

      RPG_Graphics_Surface::update (dirty_region,
                                    getScreen ());
      break;
    }
    case COMMAND_E2C_ENTITY_REMOVE:
    {
      SDL_Rect dirty_region;
      ACE_OS::memset (&dirty_region, 0, sizeof (SDL_Rect));
      RPG_CLIENT_ENTITY_MANAGER_SINGLETON::instance()->remove (parameters_in.entity_id,
                                                               dirty_region,
                                                               true,
                                                               myState->debug);
      RPG_Graphics_Surface::update (dirty_region,
                                    getScreen ());

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
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid command (was: \"%s\", aborting\n"),
                  ACE_TEXT (RPG_Engine_CommandHelper::RPG_Engine_CommandToString (command_in).c_str ())));
      return;
    }
  } // end SWITCH
}

void
SDL_GUI_LevelWindow_3D::toggleDoor(const RPG_Map_Position_t& position_in)
{
  RPG_TRACE(ACE_TEXT("SDL_GUI_LevelWindow_3D::toggleDoor"));

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
      myDoorTiles[position_in] =
          (is_open ? myCurrentDoorSet.vertical_open
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

void
SDL_GUI_LevelWindow_3D::setBlendRadius(unsigned char radius_in)
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
  RPG_TRACE (ACE_TEXT ("SDL_GUI_LevelWindow_3D::clear"));

  // sanity check(s)
  ACE_ASSERT (inherited::screen_);
#if defined (SDL_USE)
  SDL_Surface* surface_p = inherited::screen_;
#elif defined (SDL2_USE)
  SDL_Surface* surface_p = SDL_GetWindowSurface (inherited::screen_);
#endif // SDL_USE || SDL2_USE
  ACE_ASSERT (surface_p);

  // initialize clipping
  SDL_Rect old_clip_rect;
  SDL_GetClipRect (surface_p, &old_clip_rect);

  if (!SDL_SetClipRect (surface_p, &clipRectangle_))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_SetClipRect(): \"%s\", returning\n"),
                ACE_TEXT (SDL_GetError ())));
    return;
  } // end IF

  if (SDL_FillRect (surface_p,                                      // target surface
                    &clipRectangle_,                                // rectangle
                    RPG_Graphics_SDL_Tools::getColor (COLOR_BLACK,
                                                      *surface_p->format,
                                                      1.0F)))       // color
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_FillRect(): \"%s\", returning\n"),
                ACE_TEXT (SDL_GetError ())));
    return;
  } // end IF

  // reset clipping
  if (!SDL_SetClipRect (surface_p, &old_clip_rect))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_SetClipRect(): \"%s\", returning\n"),
                ACE_TEXT (SDL_GetError ())));
    return;
  } // end IF

  invalidate (clipRectangle_);
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

      // adjust WEST wall opacity
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

      // adjust SOUTH wall opacity
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

      // adjust NORTH wall opacity
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
               RPG_Graphics_Common_Tools::toString(type).c_str()));

    return;
  } // end IF

  SDL_Surface* shaded_ceiling = NULL;
  Uint8 opacity = static_cast<Uint8>(RPG_GRAPHICS_TILE_WALL_DEF_NW_OPACITY *
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

  if (SDL_FillRect (myWallBlend,
                    NULL,
                    RPG_Graphics_SDL_Tools::getColor (COLOR_BLACK_A10,
                                                      *myWallBlend->format,
                                                      1.0F)))
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

  RPG_Graphics_Offset_t offset =
      std::make_pair(std::numeric_limits<int>::max(),
                     std::numeric_limits<int>::max());
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
//  minimap_window->init(inherited::screenLock_);
}
