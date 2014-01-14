/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2011  <copyright holder> <email>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "stdafx.h"

#include "SDL_gui_minimapwindow.h"

#include <rpg_client_common.h>
#include <rpg_client_defines.h>
#include <rpg_client_engine.h>

#include <rpg_engine.h>

#include <rpg_graphics_common.h>
#include <rpg_graphics_defines.h>
#include <rpg_graphics_surface.h>
#include <rpg_graphics_common_tools.h>
#include <rpg_graphics_SDL_tools.h>

#include <rpg_map_common.h>

#include <rpg_common_macros.h>
#include <rpg_common_defines.h>

SDL_GUI_MinimapWindow::SDL_GUI_MinimapWindow(const RPG_Graphics_SDLWindowBase& parent_in,
                                             // *NOTE*: offset doesn't include any border(s) !
                                             const RPG_Graphics_Offset_t& offset_in,
                                             RPG_Engine* engine_in)
 : inherited(WINDOW_MINIMAP, // type
             parent_in,      // parent
             offset_in,      // offset
             std::string()), // title
//              NULL),          // background
   myEngine(engine_in),
   myBG(NULL),
   mySurface(NULL)
{
  RPG_TRACE(ACE_TEXT("SDL_GUI_MinimapWindow::SDL_GUI_MinimapWindow"));

  // load interface image
  RPG_Graphics_GraphicTypeUnion type;
  type.discriminator = RPG_Graphics_GraphicTypeUnion::IMAGE;
  type.image = IMAGE_INTERFACE_MINIMAP;
  myBG = RPG_Graphics_Common_Tools::loadGraphic(type,
                                                true,   // convert to display format
                                                false); // don't cache this one
  ACE_ASSERT(myBG);

  // adjust size / clip rect
  inherited::mySize.first = myBG->w;
  inherited::mySize.second = myBG->h;
//   inherited::myClipRect.x = (myScreen->w -
//                              (myBorderLeft + myBorderRight) -
//                              (myBG->w + myOffset.first));
//   inherited::myClipRect.y = myBorderTop + myOffset.second;
//   inherited::myClipRect.w = myBG->w;
//   inherited::myClipRect.h = myBG->h;

  mySurface = RPG_Graphics_Surface::copy(*myBG);
  ACE_ASSERT(mySurface);
}

SDL_GUI_MinimapWindow::~SDL_GUI_MinimapWindow()
{
  RPG_TRACE(ACE_TEXT("SDL_GUI_MinimapWindow::~SDL_GUI_MinimapWindow"));

  // clean up
  SDL_FreeSurface(myBG);
  SDL_FreeSurface(mySurface);
}

RPG_Graphics_Position_t
SDL_GUI_MinimapWindow::getView() const
{
  RPG_TRACE(ACE_TEXT("SDL_GUI_MinimapWindow::getView"));

  ACE_ASSERT(false);

  return std::make_pair(0, 0);
}

void
SDL_GUI_MinimapWindow::handleEvent(const SDL_Event& event_in,
                                   RPG_Graphics_IWindow* window_in,
                                   bool& redraw_out)
{
  RPG_TRACE(ACE_TEXT("SDL_GUI_MinimapWindow::handleEvent"));

  // init return value(s)
  redraw_out = false;

  //   ACE_DEBUG((LM_DEBUG,
  //              ACE_TEXT("SDL_GUI_MinimapWindow::handleEvent(%s)\n"),
  //              RPG_Graphics_TypeHelper::RPG_Graphics_TypeToString(myType).c_str()));

  switch (event_in.type)
  {
    // *** mouse ***
    case RPG_GRAPHICS_SDL_MOUSEMOVEOUT:
    {

      break;
    }
    case SDL_MOUSEMOTION:
    {

      // *WARNING*: falls through !
    }
    case SDL_ACTIVEEVENT:
    case SDL_KEYDOWN:
    case SDL_KEYUP:
    case SDL_MOUSEBUTTONDOWN:
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
    case SDL_USEREVENT:
    case RPG_GRAPHICS_SDL_HOVEREVENT:
    default:
    {
      // delegate these to the parent...
      getParent()->handleEvent(event_in,
                               window_in,
                               redraw_out);

      break;
    }
    //     default:
    //     {
      //       ACE_DEBUG((LM_ERROR,
      //                  ACE_TEXT("received unknown event (was: %u)...\n"),
      //                  static_cast<unsigned long> (event_in.type)));
      //
      //       break;
      //     }
  } // end SWITCH
}

void
SDL_GUI_MinimapWindow::draw(SDL_Surface* targetSurface_in,
                            const unsigned int& offsetX_in,
                            const unsigned int& offsetY_in)
{
  RPG_TRACE(ACE_TEXT("SDL_GUI_MinimapWindow::draw"));

  // sanity check(s)
  ACE_ASSERT(targetSurface_in);
  ACE_UNUSED_ARG(offsetX_in);
  ACE_UNUSED_ARG(offsetY_in);
  ACE_ASSERT(myEngine);
  ACE_ASSERT(mySurface);

  // save BG ?
  // *CONSIDER*: this doesn't really make sense...
  if (!inherited::myBGHasBeenSaved)
    inherited::saveBG(RPG_Graphics_Size_t(0, 0)); // save size of "this"

  // init clipping
  SDL_GetClipRect(targetSurface_in, &(inherited::myClipRect));
  SDL_Rect clipRect;
  clipRect.x = static_cast<Sint16>(myBorderLeft +
                                   (myScreen->w -
                                    (myBorderLeft + myBorderRight) -
                                   (inherited::mySize.first + inherited::myOffset.first)));
  clipRect.y = static_cast<Sint16>(myBorderTop + inherited::myOffset.second);
  clipRect.w = static_cast<Uint16>(inherited::mySize.first);
  clipRect.h = static_cast<Uint16>(inherited::mySize.second);
  if (!SDL_SetClipRect(targetSurface_in, &clipRect))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_SetClipRect(): %s, aborting\n"),
               SDL_GetError()));

    return;
  } // end IF

  // init surface
  RPG_Graphics_Surface::put(0,
                            0,
                            *myBG,
                            mySurface);

  // lock surface during pixel access
  if (SDL_MUSTLOCK((mySurface)))
    if (SDL_LockSurface(mySurface))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to SDL_LockSurface(): %s, aborting\n"),
                 SDL_GetError()));

      return;
    } // end IF

  RPG_Map_Size_t size = myEngine->getSize();
  RPG_Map_Position_t map_position = std::make_pair(0, 0);
  RPG_Client_MiniMapTile tile = RPG_CLIENT_MINIMAPTILE_INVALID;
  Uint32 color = 0;
  SDL_Rect destrect = {0, 0, 3, 2};
  Uint32* pixels = NULL;
  RPG_Engine_EntityID_t entity_id = 0;
  for (unsigned int y = 0;
       y < size.second;
       y++)
    for (unsigned int x = 0;
         x < size.first;
         x++)
    {
      // step1: retrieve appropriate symbol
      map_position = std::make_pair(x, y);
      tile = RPG_CLIENT_MINIMAPTILE_INVALID;
      entity_id = myEngine->hasEntity(map_position);
      
      if (entity_id)
      {
        if (entity_id == myEngine->getActive())
          tile = MINIMAPTILE_PLAYER_ACTIVE;
        else if (!myEngine->isMonster(entity_id))
          tile = MINIMAPTILE_PLAYER;
        else
          tile = MINIMAPTILE_MONSTER;
      } // end IF
      if (tile == RPG_CLIENT_MINIMAPTILE_INVALID)
      {
        switch (myEngine->getElement(map_position))
        {
          case MAPELEMENT_UNMAPPED:
          case MAPELEMENT_WALL:
            tile = MINIMAPTILE_NONE; break;
          case MAPELEMENT_FLOOR:
            tile = MINIMAPTILE_FLOOR; break;
          case MAPELEMENT_STAIRS:
            tile = MINIMAPTILE_STAIRS; break;
          case MAPELEMENT_DOOR:
            tile = MINIMAPTILE_DOOR; break;
          default:
          {
            ACE_DEBUG((LM_ERROR,
                       ACE_TEXT("invalid map element ([%u,%u] was: %d), aborting\n"),
                       x,
                       y,
                       myEngine->getElement(map_position)));

            return;
          }
        } // end SWITCH
      } // end IF

      // step2: map symbol to color
      color = 0;
      switch (tile)
      {
        case MINIMAPTILE_NONE:
          color = RPG_CLIENT_DEF_MINIMAPCOLOR_WALL; break;
        case MINIMAPTILE_DOOR:
          color = RPG_CLIENT_DEF_MINIMAPCOLOR_DOOR; break;
        case MINIMAPTILE_FLOOR:
          color = RPG_CLIENT_DEF_MINIMAPCOLOR_FLOOR; break;
        case MINIMAPTILE_MONSTER:
          color = RPG_CLIENT_DEF_MINIMAPCOLOR_MONSTER; break;
        case MINIMAPTILE_PLAYER:
          color = RPG_CLIENT_DEF_MINIMAPCOLOR_PLAYER; break;
        case MINIMAPTILE_STAIRS:
          color = RPG_CLIENT_DEF_MINIMAPCOLOR_STAIRS; break;
        default:
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("invalid minimap tile type (was: %d), aborting\n"),
                     tile));

          break;
        }
      } // end SWITCH

      // step3: draw tile onto surface
      // *NOTE*: a minimap symbol has this shape: _ C _
      //                                          C C C
      // where "_" is transparent and "C" is a colored pixel
      ACE_ASSERT(mySurface->format->BytesPerPixel == 4);

      // step3a: row 1
      destrect.x = 40 + (2 * x) - (2 * y);
      destrect.y = x + y;
      pixels = reinterpret_cast<Uint32*>(static_cast<char*>(mySurface->pixels) +
                                         (mySurface->pitch * (destrect.y + 6)) +
                                         ((destrect.x + 6) * 4));
//       pixels[0] = transparent -> dont write
      pixels[1] = color;
//       pixels[2] = transparent -> dont write
      // step3b: row 2
      pixels = reinterpret_cast<Uint32*>(static_cast<char*>(mySurface->pixels) +
                                         (mySurface->pitch * (destrect.y + 7)) +
                                         ((destrect.x + 6) * 4));
      pixels[0] = color;
      pixels[1] = color;
      pixels[2] = color;
    } // end FOR

  if (SDL_MUSTLOCK(mySurface))
    SDL_UnlockSurface(mySurface);

  // step4: paint surface
  RPG_Graphics_Surface::put((myBorderLeft +
                             (myScreen->w -
                              (myBorderLeft + myBorderRight) -
                              (inherited::mySize.first + inherited::myOffset.first))),
                            (myBorderTop +
                             inherited::myOffset.second),
                            *mySurface,
                            targetSurface_in);

//   // save image
//   std::string path = ACE_TEXT_ALWAYS_CHAR(RPG_COMMON_DEF_DUMP_DIR);
//   path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
//   path += ACE_TEXT_ALWAYS_CHAR("minimap.png");
//   RPG_Graphics_Surface::savePNG(*mySurface,
//                                 path,
//                                 true);

  // reset clipping
  if (!SDL_SetClipRect(targetSurface_in, &(inherited::myClipRect)))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_SetClipRect(): %s, aborting\n"),
               SDL_GetError()));

    return;
  } // end IF

  // remember position of last realization
  inherited::myLastAbsolutePosition = std::make_pair((myBorderLeft +
                                                      (myScreen->w -
                                                       (myBorderLeft + myBorderRight) -
                                                       (inherited::mySize.first + inherited::myOffset.first))),
                                                     (myBorderTop +
                                                      inherited::myOffset.second));
}
