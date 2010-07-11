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
#include "rpg_graphics_hotspot.h"

#include "rpg_graphics_defines.h"
#include "rpg_graphics_dictionary.h"
#include "rpg_graphics_surface.h"
#include "rpg_graphics_cursor.h"
#include "rpg_graphics_common_tools.h"
#include "rpg_graphics_SDL_tools.h"

RPG_Graphics_HotSpot::RPG_Graphics_HotSpot(const RPG_Graphics_SDLWindowBase& parent_in,
                                           const RPG_Graphics_WindowSize_t& size_in,
                                           // *NOTE*: offset doesn't include any border(s) !
                                           const RPG_Graphics_Offset_t& offset_in,
                                           const RPG_Graphics_Type& graphicsType_in)
 : inherited(WINDOWTYPE_HOTSPOT, // type
             parent_in,          // parent
             offset_in),         // offset
   myCursorType(graphicsType_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_HotSpot::RPG_Graphics_HotSpot"));

  // *NOTE*: hotspots don't have borders
  // --> overwrite size
  mySize = size_in;
}

RPG_Graphics_HotSpot::~RPG_Graphics_HotSpot()
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_HotSpot::~RPG_Graphics_HotSpot"));

}

const RPG_Graphics_Type
RPG_Graphics_HotSpot::getCursorType() const
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_HotSpot::getCursorType"));

  return myCursorType;
}

void
RPG_Graphics_HotSpot::handleEvent(const SDL_Event& event_in,
                                  RPG_Graphics_IWindow* window_in,
                                  bool& redraw_out)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_HotSpot::handleEvent"));

  // init return value(s)
  redraw_out = false;

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("RPG_Graphics_HotSpot::handleEvent(%s)\n"),
//              RPG_Graphics_TypeHelper::RPG_Graphics_TypeToString(myType).c_str()));

  switch (event_in.type)
  {
    // *** mouse ***
    case SDL_MOUSEMOTION:
    {
      // set appropriate cursor
      RPG_GRAPHICS_CURSOR_SINGLETON::instance()->set(myCursorType);

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
    default:
    {
      // delegate these to the parent...
      return getParent()->handleEvent(event_in,
                                      window_in,
                                      redraw_out);

      break;
    }
//     default:
//     {
//       ACE_DEBUG((LM_ERROR,
//                  ACE_TEXT("received unknown event (was: %u)...\n"),
//                  ACE_static_cast(unsigned long, event_in.type)));
//
//       break;
//     }
  } // end SWITCH
}

void
RPG_Graphics_HotSpot::draw(SDL_Surface* targetSurface_in,
                           const unsigned long& offsetX_in,
                           const unsigned long& offsetY_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_HotSpot::draw"));

  // sanity check(s)
  ACE_ASSERT(targetSurface_in);
  ACE_ASSERT(ACE_static_cast(int, offsetX_in) <= targetSurface_in->w);
  ACE_ASSERT(ACE_static_cast(int, offsetY_in) <= targetSurface_in->h);

  // init clipping
  SDL_Rect clipRect;
  clipRect.x = offsetX_in + myOffset.first;
  clipRect.y = offsetY_in + myOffset.second;
  clipRect.w = mySize.first;
  clipRect.h = mySize.second;
  if (!SDL_SetClipRect(targetSurface_in, &clipRect))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_SetClipRect(): %s, aborting\n"),
               SDL_GetError()));

    return;
  } // end IF

//   // debug info
//   RPG_Graphics_Surface::putRect(clipRect,                          // rectangle
//                                 RPG_GRAPHICS_WINDOW_HOTSPOT_COLOR, // color
//                                 targetSurface_in);                 // target surface

  // remember position of last realization
  myLastAbsolutePosition = std::make_pair(offsetX_in + myOffset.first,
                                          offsetY_in + myOffset.second);
}

void
RPG_Graphics_HotSpot::init(const RPG_Graphics_SDLWindowBase& parent_in,
                           const RPG_Graphics_WindowSize_t& size_in,
                           const RPG_Graphics_Offset_t& offset_in,
                           const RPG_Graphics_Type& type_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_HotSpot::init"));

  RPG_Graphics_HotSpot* hotspot = NULL;
  //*NOTE*: hotspot registers automagically
  try
  {
    hotspot = new RPG_Graphics_HotSpot(parent_in,
                                       size_in,
                                       offset_in,
                                       type_in);
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to allocate memory(%u): %m, aborting\n"),
               sizeof(RPG_Graphics_HotSpot)));

    return;
  }
  if (!hotspot)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to allocate memory(%u): %m, aborting\n"),
               sizeof(RPG_Graphics_HotSpot)));

    return;
  } // end IF
}
