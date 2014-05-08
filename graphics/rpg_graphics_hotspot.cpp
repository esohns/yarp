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

#include "rpg_graphics_hotspot.h"

#include "rpg_graphics_defines.h"
#include "rpg_graphics_dictionary.h"
#include "rpg_graphics_surface.h"
#include "rpg_graphics_common_tools.h"
#include "rpg_graphics_SDL_tools.h"

#include "rpg_common_macros.h"

RPG_Graphics_HotSpot::RPG_Graphics_HotSpot(const RPG_Graphics_SDLWindowBase& parent_in,
                                           const RPG_Graphics_Size_t& size_in,
                                           // *NOTE*: offset doesn't include any border(s) !
                                           const RPG_Graphics_Offset_t& offset_in,
                                           const RPG_Graphics_Cursor& cursor_in,
                                           const bool& debug_in)
 : inherited(WINDOW_HOTSPOT, // type
             parent_in,      // parent
             offset_in,      // offset
             std::string()), // title
//              NULL),          // background
   myCursorType(cursor_in),
   myCursorHasBeenSet(false),
   myDebug(debug_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_HotSpot::RPG_Graphics_HotSpot"));

  // *NOTE*: hotspots don't have borders
  // --> overwrite size
  myClipRect.w = static_cast<uint16_t>(size_in.first);
  myClipRect.h = static_cast<uint16_t>(size_in.second);
}

RPG_Graphics_HotSpot::~RPG_Graphics_HotSpot()
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_HotSpot::~RPG_Graphics_HotSpot"));

}

RPG_Graphics_Cursor
RPG_Graphics_HotSpot::getCursorType() const
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_HotSpot::getCursorType"));

  return myCursorType;
}

RPG_Graphics_Position_t
RPG_Graphics_HotSpot::getView() const
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_HotSpot::getView"));

  ACE_ASSERT(false);

  return std::make_pair(0, 0);
}

void
RPG_Graphics_HotSpot::handleEvent(const SDL_Event& event_in,
                                  RPG_Graphics_IWindowBase* window_in,
                                  SDL_Rect& dirtyRegion_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_HotSpot::handleEvent"));

  // init return value(s)
  ACE_OS::memset(&dirtyRegion_out, 0, sizeof(dirtyRegion_out));

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("RPG_Graphics_HotSpot::handleEvent(%s)\n"),
//              RPG_Graphics_TypeHelper::RPG_Graphics_TypeToString(myType).c_str()));

  switch (event_in.type)
  {
    // *** mouse ***
    case RPG_GRAPHICS_SDL_MOUSEMOVEOUT:
    {
      // reset cursor
      getParent()->notify(CURSOR_NORMAL);

      myCursorHasBeenSet = false;

      break;
    }
    case SDL_MOUSEMOTION:
    {
      // upon entry, set appropriate cursor
      if (!myCursorHasBeenSet)
      {
        getParent()->notify(myCursorType);

        myCursorHasBeenSet = true;
      } // end IF

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
                               dirtyRegion_out);

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
RPG_Graphics_HotSpot::draw(SDL_Surface* targetSurface_in,
                           const unsigned int& offsetX_in,
                           const unsigned int& offsetY_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_HotSpot::draw"));

  // sanity check(s)
  ACE_ASSERT(targetSurface_in);
  ACE_UNUSED_ARG(offsetX_in);
  ACE_UNUSED_ARG(offsetY_in);

//   // init clipping
//   SDL_Rect clipRect;
//   clipRect.x = offsetX_in + myOffset.first;
//   clipRect.y = offsetY_in + myOffset.second;
//   clipRect.w = mySize.first;
//   clipRect.h = mySize.second;
//   if (!SDL_SetClipRect(targetSurface_in, &clipRect))
//   {
//     ACE_DEBUG((LM_ERROR,
//                ACE_TEXT("failed to SDL_SetClipRect(): %s, aborting\n"),
//                SDL_GetError()));
//
//     return;
//   } // end IF

   // debug info
  if (myDebug)
    RPG_Graphics_Surface::putRect(myClipRect,                            // rectangle
                                  RPG_GRAPHICS_WINDOW_HOTSPOT_DEF_COLOR, // color
                                  targetSurface_in);                     // target surface

  // remember position of last realization
  myLastAbsolutePosition = std::make_pair(myClipRect.x,
                                          myClipRect.y);
}

void
RPG_Graphics_HotSpot::init(const RPG_Graphics_SDLWindowBase& parent_in,
                           const RPG_Graphics_Size_t& size_in,
                           const RPG_Graphics_Offset_t& offset_in,
                           const RPG_Graphics_Cursor& cursor_in,
                           const bool& debug_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_HotSpot::init"));

  //*NOTE*: hotspot registers automagically
  RPG_Graphics_HotSpot* hotspot = NULL;
  ACE_NEW_NORETURN(hotspot,
                   RPG_Graphics_HotSpot(parent_in,
                                        size_in,
                                        offset_in,
                                        cursor_in,
                                        debug_in));
  if (!hotspot)
  {
    ACE_DEBUG((LM_CRITICAL,
               ACE_TEXT("failed to allocate memory(%u): %m, aborting\n"),
               sizeof(RPG_Graphics_HotSpot)));

    return;
  } // end IF
}
