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

#include "rpg_client_window_minimap.h"

#include "rpg_client_common.h"
#include "rpg_client_defines.h"
#include "rpg_client_engine.h"

#include <rpg_engine_level.h>

#include <rpg_graphics_common.h>
#include <rpg_graphics_defines.h>
#include <rpg_graphics_surface.h>
#include <rpg_graphics_common_tools.h>
#include <rpg_graphics_SDL_tools.h>

#include <rpg_map_common.h>

#include <rpg_common_macros.h>

RPG_Client_Window_MiniMap::RPG_Client_Window_MiniMap(const RPG_Graphics_SDLWindowBase& parent_in,
                                                     // *NOTE*: offset doesn't include any border(s) !
                                                     const RPG_Graphics_Offset_t& offset_in,
                                                     RPG_Client_Engine* engine_in,
                                                     RPG_Engine_Level* levelState_in)
 : inherited(WINDOW_MINIMAP, // type
             parent_in,      // parent
             offset_in,      // offset
             std::string(),  // title
             NULL),          // background
   myEngine(engine_in),
   myLevelState(levelState_in),
   mySurface(NULL)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Window_MiniMap::RPG_Client_Window_MiniMap"));

  // load interface image
  RPG_Graphics_GraphicTypeUnion type;
  type.discriminator = RPG_Graphics_GraphicTypeUnion::IMAGE;
  type.image = IMAGE_INTERFACE_MINIMAP;
  inherited::myBackGround = NULL;
  inherited::myBackGround = RPG_Graphics_Common_Tools::loadGraphic(type,
                                                                   false); // don't cache this one
  // sanity check(s)
  ACE_ASSERT(inherited::myBackGround);

  mySurface = RPG_Graphics_Surface::copy(*inherited::myBackGround);
  ACE_ASSERT(mySurface);
}

RPG_Client_Window_MiniMap::~RPG_Client_Window_MiniMap()
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Window_MiniMap::~RPG_Client_Window_MiniMap"));

  // clean up
  SDL_FreeSurface(mySurface);
}

void
RPG_Client_Window_MiniMap::handleEvent(const SDL_Event& event_in,
                                       RPG_Graphics_IWindow* window_in,
                                       bool& redraw_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Window_MiniMap::handleEvent"));

  // init return value(s)
  redraw_out = false;

  //   ACE_DEBUG((LM_DEBUG,
  //              ACE_TEXT("RPG_Client_Window_MiniMap::handleEvent(%s)\n"),
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
RPG_Client_Window_MiniMap::draw(SDL_Surface* targetSurface_in,
                                const unsigned long& offsetX_in,
                                const unsigned long& offsetY_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Window_MiniMap::draw"));

  // sanity check(s)
  ACE_ASSERT(targetSurface_in);
  ACE_ASSERT(static_cast<int>(offsetX_in) <= targetSurface_in->w);
  ACE_ASSERT(static_cast<int>(offsetY_in) <= targetSurface_in->h);
  ACE_ASSERT(myEngine);
  ACE_ASSERT(myLevelState);

  RPG_Map_Dimensions_t dimensions = myLevelState->getDimensions();

  // init clipping
  SDL_Rect clipRect;
  clipRect.x = myOffset.first + offsetX_in;
  clipRect.y = myOffset.second + offsetY_in;
  clipRect.w = mySurface->w;
  clipRect.h = mySurface->h;
  if (!SDL_SetClipRect(targetSurface_in, &clipRect))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_SetClipRect(): %s, aborting\n"),
               SDL_GetError()));

    return;
  } // end IF

  // init surface
  ACE_ASSERT((myBackGround->w == mySurface->w) &&
             (myBackGround->h == mySurface->h));
  RPG_Graphics_Surface::put(0,
                            0,
                            *myBackGround,
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

  RPG_Map_Position_t map_position = std::make_pair(0, 0);
  RPG_Map_Element element = MAPELEMENT_INVALID;
  RPG_Client_MiniMapTile tile = RPG_CLIENT_MINIMAPTILE_INVALID;
  Uint32 color = 0;
  SDL_Rect destrect = {0, 0, 3, 2};
  Uint32* pixels = NULL;
  for (unsigned int y = 0;
       y < dimensions.second;
       y++)
    for (unsigned int x = 1;
         x < dimensions.first;
         x++)
    {
      // step1: retrieve appropriate symbol
      element = MAPELEMENT_INVALID;
      tile = RPG_CLIENT_MINIMAPTILE_INVALID;

      if (myEngine->getPlayer())
      {
        map_position = myLevelState->getPosition(myEngine->getPlayer());
        if ((x == map_position.first) &&
            (y == map_position.second))
          tile = MINIMAPTILE_PLAYER;
      } // end IF
      else
      {
        map_position.first = x;
        map_position.second = y;
        if (myLevelState->hasMonster(map_position))
          tile = MINIMAPTILE_MONSTER;
        else
        {
          element = myLevelState->getElement(map_position);
          switch (element)
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
                         myLevelState->getElement(map_position)));

              return;
            }
          } // end SWITCH
        } // end ELSE
      } // end ELSE

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
      pixels = static_cast<Uint32*>(mySurface->pixels) + (mySurface->pitch * (destrect.y + 6))
                                                       + ((destrect.x + 6) * 4);
//       pixels[0] = transparent -> dont write
      pixels[1] = color;
//       pixels[2] = transparent -> dont write
      // step3b: row 2
      pixels = static_cast<Uint32*>(mySurface->pixels) + (mySurface->pitch * (destrect.y + 7))
                                                       + ((destrect.x + 6) * 4);
      pixels[0] = color;
      pixels[1] = color;
      pixels[2] = color;
    } // end FOR

  if (SDL_MUSTLOCK(mySurface))
    SDL_UnlockSurface(mySurface);

  // step4: paint surface
  RPG_Graphics_Surface::put(myOffset.first + offsetX_in,
                            myOffset.second,
                            *mySurface,
                            targetSurface_in);

  // reset clipping
  if (!SDL_SetClipRect(targetSurface_in, NULL))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_SetClipRect(): %s, aborting\n"),
               SDL_GetError()));

    return;
  } // end IF

  // remember position of last realization
  inherited::myLastAbsolutePosition = std::make_pair(offsetX_in + inherited::myOffset.first,
                                                     offsetY_in + inherited::myOffset.second);
}
