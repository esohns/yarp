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

#include "rpg_graphics_dictionary.h"
#include "rpg_graphics_surface.h"
#include "rpg_graphics_common_tools.h"

RPG_Graphics_HotSpot::RPG_Graphics_HotSpot(const RPG_Graphics_SDLWindow& parent_in,
                                           const RPG_Graphics_WindowSize_t& size_in,
                                           // *NOTE*: offset doesn't include any border(s) !
                                           const RPG_Graphics_Offset_t& offset_in,
                                           const RPG_Graphics_Type& graphicsType_in)
 : inherited(WINDOWTYPE_HOTSPOT,
             parent_in,
             offset_in),
   myCursor(NULL),
   myInitialized(false)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_HotSpot::RPG_Graphics_HotSpot"));

  // *NOTE*: hotspots don't have borders
  // --> overwrite size
  mySize = size_in;

  // (try to) load cursor graphic
  myInitialized = loadGraphics(graphicsType_in);
}

RPG_Graphics_HotSpot::~RPG_Graphics_HotSpot()
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_HotSpot::~RPG_Graphics_HotSpot"));

  // clean up
  if (myInitialized)
  {
    if (myCursor)
      SDL_FreeSurface(myCursor);
  } // end IF
}

void
RPG_Graphics_HotSpot::handleEvent(const SDL_Event& event_in,
                                  bool& redraw_out)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_HotSpot::handleEvent"));

  // init return value(s)
  redraw_out = false;

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("RPG_Graphics_HotSpot::handleEvent\n")));

  switch (event_in.type)
  {
    // *** visibility ***
    case SDL_ACTIVEEVENT:
    {
      if (event_in.active.state & SDL_APPACTIVE)
      {
        if (event_in.active.gain)
        {
          ACE_DEBUG((LM_DEBUG,
                     ACE_TEXT("activated...\n")));
        } // end IF
        else
        {
          ACE_DEBUG((LM_DEBUG,
                     ACE_TEXT("iconified...\n")));
        } // end ELSE
      } // end IF
      if (event_in.active.state & SDL_APPMOUSEFOCUS)
      {
        if (event_in.active.gain)
        {
          ACE_DEBUG((LM_DEBUG,
                     ACE_TEXT("mouse focus...\n")));
        } // end IF
        else
        {
          ACE_DEBUG((LM_DEBUG,
                     ACE_TEXT("lost mouse focus...\n")));
        } // end ELSE
      } // end IF

      break;
    }
    // *** keyboard ***
    case SDL_KEYDOWN:
    {
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("key pressed...\n")));

      break;
    }
    case SDL_KEYUP:
    {
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("key released...\n")));

      break;
    }
    // *** mouse ***
    case SDL_MOUSEMOTION:
    {
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("mouse motion...\n")));

      break;
    }
    case SDL_MOUSEBUTTONDOWN:
    {
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("mouse button pressed...\n")));

      break;
    }
    case SDL_MOUSEBUTTONUP:
    {
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("mouse button released...\n")));

      break;
    }
    // *** joystick ***
    case SDL_JOYAXISMOTION:
    case SDL_JOYBALLMOTION:
    case SDL_JOYHATMOTION:
    case SDL_JOYBUTTONDOWN:
    case SDL_JOYBUTTONUP:
    {
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("joystick activity...\n")));

      break;
    }
    case SDL_QUIT:
    {
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("SDL_QUIT event...\n")));

      break;
    }
    case SDL_SYSWMEVENT:
    {
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("SDL_SYSWMEVENT event...\n")));

      break;
    }
    case SDL_VIDEORESIZE:
    {
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("SDL_VIDEORESIZE event...\n")));

      break;
    }
    case SDL_VIDEOEXPOSE:
    {
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("SDL_VIDEOEXPOSE event...\n")));

      break;
    }
    case SDL_USEREVENT:
    {
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("SDL_USEREVENT event...\n")));

      break;
    }
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("received unknown event (was: %u)...\n"),
                 ACE_static_cast(unsigned long, event_in.type)));

      break;
    }
  } // end SWITCH
}

void
RPG_Graphics_HotSpot::draw(SDL_Surface* targetSurface_in,
                           const RPG_Graphics_Offset_t& offset_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_HotSpot::draw"));

  ACE_UNUSED_ARG(targetSurface_in);
  ACE_UNUSED_ARG(offset_in);

  // nothing to do here...
}

void
RPG_Graphics_HotSpot::init(const RPG_Graphics_SDLWindow& parent_in,
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

const bool
RPG_Graphics_HotSpot::loadGraphics(const RPG_Graphics_Type& graphicType_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_HotSpot::loadGraphics"));

  // sanity check
  if (myCursor)
  {
    SDL_FreeSurface(myCursor);
    myCursor = NULL;
  } // end IF

  // load cursor graphic
  RPG_Graphics_t graphic;
  graphic.type = graphicType_in;
  // retrieve properties from the dictionary
  graphic = RPG_GRAPHICS_DICTIONARY_SINGLETON::instance()->getGraphic(graphicType_in);
  if (graphic.type != graphicType_in)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Graphics_Dictionary::getGraphic(\"%s\"), aborting\n"),
               RPG_Graphics_TypeHelper::RPG_Graphics_TypeToString(graphicType_in).c_str()));

    return false;
  } // end IF

  myCursor = RPG_Graphics_Common_Tools::loadGraphic(graphicType_in,
                                                    false); // don't cache this one
  if (!myCursor)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Graphics_Common_Tools::loadGraphic(\"%s\"), aborting\n"),
               RPG_Graphics_TypeHelper::RPG_Graphics_TypeToString(graphicType_in).c_str()));

    return false;
  } // end IF

  return true;
}
