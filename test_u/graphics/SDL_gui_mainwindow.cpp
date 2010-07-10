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
#include "SDL_gui_mainwindow.h"

#include "SDL_gui_defines.h"
#include "SDL_gui_levelwindow.h"

#include <rpg_graphics_defines.h>
#include <rpg_graphics_surface.h>
#include <rpg_graphics_hotspot.h>
#include <rpg_graphics_common_tools.h>
#include <rpg_graphics_SDL_tools.h>

#include <ace/Log_Msg.h>

#include <sstream>

// init statics
unsigned long SDL_GUI_MainWindow::screenshot_index = 0;

SDL_GUI_MainWindow::SDL_GUI_MainWindow(const RPG_Graphics_WindowSize_t& size_in,
                                       const RPG_Graphics_Type& graphicType_in,
                                       const std::string& title_in,
                                       const RPG_Graphics_Type& fontType_in)
 : inherited(size_in,
             graphicType_in,
             title_in,
             fontType_in),
   myLastHoverTime(0),
   myHaveMouseFocus(false)
{
  ACE_TRACE(ACE_TEXT("SDL_GUI_MainWindow::SDL_GUI_MainWindow"));

  // init scroll margins
  initScrollSpots();
}

SDL_GUI_MainWindow::~SDL_GUI_MainWindow()
{
  ACE_TRACE(ACE_TEXT("SDL_GUI_MainWindow::~SDL_GUI_MainWindow"));

}

void
SDL_GUI_MainWindow::draw(SDL_Surface* targetSurface_in,
                         const RPG_Graphics_Offset_t& offset_in)
{
  ACE_TRACE(ACE_TEXT("SDL_GUI_MainWindow::draw"));

  // sanity check(s)
  ACE_ASSERT(targetSurface_in);
  ACE_ASSERT(ACE_static_cast(int, offset_in.first) <= targetSurface_in->w);
  ACE_ASSERT(ACE_static_cast(int, offset_in.second) <= targetSurface_in->h);

  RPG_Graphics_InterfaceElementsConstIterator_t iterator;
//   unsigned long border = 0;
  SDL_Rect clipRect;
  unsigned long i = 0;

  // step1: draw borders
  clipRect.x = offset_in.first + myBorderLeft;
  clipRect.y = offset_in.second;
  clipRect.w = (targetSurface_in->w - (myBorderLeft + myBorderRight));
  clipRect.h = myBorderTop;
  if (!SDL_SetClipRect(targetSurface_in, &clipRect))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_SetClipRect(): %s, aborting\n"),
               SDL_GetError()));

    return;
  } // end IF
  iterator = myElementGraphics.find(INTERFACEELEMENT_BORDER_TOP);
  ACE_ASSERT(iterator != myElementGraphics.end());
  for (i = offset_in.first + myBorderLeft;
       i < (ACE_static_cast(unsigned long, targetSurface_in->w) - myBorderRight);
       i += (*iterator).second->w)
    RPG_Graphics_Surface::put(i,
                              offset_in.second,
                              *(*iterator).second,
                              targetSurface_in);

  clipRect.x = offset_in.first;
  clipRect.y = (offset_in.second + myBorderTop);
  clipRect.w = myBorderLeft;
  clipRect.h = (targetSurface_in->h - (myBorderTop + myBorderBottom));
  if (!SDL_SetClipRect(targetSurface_in, &clipRect))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_SetClipRect(): %s, aborting\n"),
               SDL_GetError()));

    return;
  } // end IF
  iterator = myElementGraphics.find(INTERFACEELEMENT_BORDER_LEFT);
  ACE_ASSERT(iterator != myElementGraphics.end());
  for (i = (offset_in.second + myBorderTop);
       i < (ACE_static_cast(unsigned long, targetSurface_in->h) - myBorderBottom);
       i += (*iterator).second->h)
    RPG_Graphics_Surface::put(offset_in.first,
                              i,
                              *(*iterator).second,
                              targetSurface_in);

  clipRect.x = (targetSurface_in->w - myBorderRight);
  clipRect.y = offset_in.second + myBorderTop;
  clipRect.w = myBorderRight;
  clipRect.h = (targetSurface_in->h - (myBorderTop + myBorderBottom));
  if (!SDL_SetClipRect(targetSurface_in, &clipRect))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_SetClipRect(): %s, aborting\n"),
               SDL_GetError()));

    return;
  } // end IF
  iterator = myElementGraphics.find(INTERFACEELEMENT_BORDER_RIGHT);
  ACE_ASSERT(iterator != myElementGraphics.end());
  for (i = (offset_in.second + myBorderTop);
       i < (ACE_static_cast(unsigned long, targetSurface_in->h) - myBorderBottom);
       i += (*iterator).second->h)
    RPG_Graphics_Surface::put((targetSurface_in->w - myBorderRight),
                              i,
                              *(*iterator).second,
                              targetSurface_in);

  clipRect.x = offset_in.first + myBorderLeft;
  clipRect.y = (targetSurface_in->h - myBorderBottom);
  clipRect.w = (targetSurface_in->w - (myBorderLeft + myBorderRight));
  clipRect.h = myBorderBottom;
  if (!SDL_SetClipRect(targetSurface_in, &clipRect))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_SetClipRect(): %s, aborting\n"),
               SDL_GetError()));

    return;
  } // end IF
  iterator = myElementGraphics.find(INTERFACEELEMENT_BORDER_BOTTOM);
  ACE_ASSERT(iterator != myElementGraphics.end());
  for (i = (offset_in.first + myBorderLeft);
       i < (targetSurface_in->w - myBorderRight);
       i += (*iterator).second->w)
    RPG_Graphics_Surface::put(i,
                              (targetSurface_in->h - myBorderBottom),
                              *(*iterator).second,
                              targetSurface_in);

  // reset clipping area
  clipRect.x = 0;
  clipRect.y = 0;
  clipRect.w = targetSurface_in->w;
  clipRect.h = targetSurface_in->h;
  if (!SDL_SetClipRect(targetSurface_in, &clipRect))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_SetClipRect(): %s, aborting\n"),
               SDL_GetError()));

    return;
  } // end IF

  // step2: draw corners
  iterator = myElementGraphics.find(INTERFACEELEMENT_CORNER_TL);
  ACE_ASSERT(iterator != myElementGraphics.end());
  RPG_Graphics_Surface::put(offset_in.first,
                            offset_in.second,
                            *(*iterator).second,
                            targetSurface_in);

  iterator = myElementGraphics.find(INTERFACEELEMENT_CORNER_TR);
  ACE_ASSERT(iterator != myElementGraphics.end());
  RPG_Graphics_Surface::put((targetSurface_in->w - (*iterator).second->w),
                            offset_in.second,
                            *(*iterator).second,
                            targetSurface_in);

  iterator = myElementGraphics.find(INTERFACEELEMENT_CORNER_BL);
  ACE_ASSERT(iterator != myElementGraphics.end());
  RPG_Graphics_Surface::put(offset_in.first,
                            (targetSurface_in->h - (*iterator).second->h),
                            *(*iterator).second,
                            targetSurface_in);

  iterator = myElementGraphics.find(INTERFACEELEMENT_CORNER_BR);
  ACE_ASSERT(iterator != myElementGraphics.end());
  RPG_Graphics_Surface::put((targetSurface_in->w - (*iterator).second->w),
                            (targetSurface_in->h - (*iterator).second->h),
                            *(*iterator).second,
                            targetSurface_in);

  // step3: fill central area
  iterator = myElementGraphics.find(INTERFACEELEMENT_CENTER);
  ACE_ASSERT(iterator != myElementGraphics.end());
  clipRect.x = offset_in.first + myBorderLeft;
  clipRect.y = offset_in.second + myBorderTop;
  clipRect.w = (targetSurface_in->w - (myBorderLeft + myBorderRight));
  clipRect.h = (targetSurface_in->h - (myBorderTop + myBorderBottom));
  if (!SDL_SetClipRect(targetSurface_in, &clipRect))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_SetClipRect(): %s, aborting\n"),
               SDL_GetError()));

    return;
  } // end IF
  for (i = (offset_in.second + myBorderTop);
       i < (targetSurface_in->h - myBorderBottom);
       i += (*iterator).second->h)
    for (unsigned long j = (offset_in.first + myBorderLeft);
         j < (targetSurface_in->w - myBorderRight);
         j += (*iterator).second->w)
      RPG_Graphics_Surface::put(j,
                                i,
                                *(*iterator).second,
                                targetSurface_in);

  // step4: draw title (if any)
  if (!myTitle.empty())
  {
    clipRect.x = myBorderLeft;
    clipRect.y = 0;
    clipRect.w = targetSurface_in->w - (myBorderLeft + myBorderRight);
    clipRect.h = myBorderTop;
    if (!SDL_SetClipRect(targetSurface_in, &clipRect))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to SDL_SetClipRect(): %s, aborting\n"),
                 SDL_GetError()));

      return;
    } // end IF
    RPG_Graphics_TextSize_t title_size = RPG_Graphics_Common_Tools::textSize(myTitleFont,
                                                                             myTitle);
    RPG_Graphics_Surface::putText(myTitleFont,
                                  myTitle,
                                  RPG_Graphics_SDL_Tools::colorToSDLColor(RPG_GRAPHICS_FONT_DEF_COLOR,
                                                                          *targetSurface_in),
                                  true, // add shade
                                  RPG_Graphics_SDL_Tools::colorToSDLColor(RPG_GRAPHICS_FONT_DEF_SHADECOLOR,
                                                                          *targetSurface_in),
                                  myBorderLeft, // top left
                                  ((myBorderTop - title_size.second) / 2), // center of top border
                                  targetSurface_in);
  } // end IF

  // step5: realize hotspots (and any other children)
  for (RPG_Graphics_WindowsIterator_t iterator = myChildren.begin();
       iterator != myChildren.end();
       iterator++)
  {
    try
    {
      (*iterator)->draw(targetSurface_in,
                        offset_in);
    }
    catch (...)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("caught exception in RPG_Graphics_IWindow::draw(), continuing\n")));
    }
  } // end FOR

  // reset clipping area
  clipRect.x = 0;
  clipRect.y = 0;
  clipRect.w = targetSurface_in->w;
  clipRect.h = targetSurface_in->h;
  if (!SDL_SetClipRect(targetSurface_in, &clipRect))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_SetClipRect(): %s, aborting\n"),
               SDL_GetError()));

    return;
  } // end IF

  // whole window needs a refresh...
  myDirtyRegions.push_back(clipRect);

  // remember position of last realization
  myLastAbsolutePosition = offset_in;
}

void
SDL_GUI_MainWindow::handleEvent(const SDL_Event& event_in,
                                RPG_Graphics_IWindow* window_in,
                                bool& redraw_out)
{
  ACE_TRACE(ACE_TEXT("SDL_GUI_MainWindow::handleEvent"));

  // init return value(s)
  redraw_out = false;

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("SDL_GUI_MainWindow::handleEvent\n")));

  switch (event_in.type)
  {
    // *** visibility ***
    case SDL_ACTIVEEVENT:
    {
      if (event_in.active.state & SDL_APPMOUSEFOCUS)
      {
        if (event_in.active.gain & SDL_APPMOUSEFOCUS)
        {
//           ACE_DEBUG((LM_DEBUG,
//                      ACE_TEXT("gained mouse coverage...\n")));

          myHaveMouseFocus = true;
        } // end IF
        else
        {
//           ACE_DEBUG((LM_DEBUG,
//                      ACE_TEXT("lost mouse coverage...\n")));

          myHaveMouseFocus = false;
        } // end ELSE
      } // end IF
      if (event_in.active.state & SDL_APPINPUTFOCUS)
      {
        if (event_in.active.gain & SDL_APPINPUTFOCUS)
        {
//           ACE_DEBUG((LM_DEBUG,
//                      ACE_TEXT("gained input focus...\n")));
        } // end IF
        else
        {
//           ACE_DEBUG((LM_DEBUG,
//                      ACE_TEXT("lost input focus...\n")));
        } // end ELSE
      } // end IF
      if (event_in.active.state & SDL_APPACTIVE)
      {
        if (event_in.active.gain & SDL_APPACTIVE)
        {
//           ACE_DEBUG((LM_DEBUG,
//                      ACE_TEXT("activated...\n")));
        } // end IF
        else
        {
//           ACE_DEBUG((LM_DEBUG,
//                      ACE_TEXT("iconified...\n")));
        } // end ELSE
      } // end IF

      break;
    }
    // *** keyboard ***
    case SDL_KEYDOWN:
    {
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("%s key\n%s\n"),
                 ((event_in.type == SDL_KEYDOWN) ? ACE_TEXT("pressed") : ACE_TEXT("released")),
                 RPG_Graphics_SDL_Tools::keyToString(event_in.key.keysym).c_str()));

      switch (event_in.key.keysym.sym)
      {
        case SDLK_s:
        {
          std::ostringstream converter;
          converter << screenshot_index++;
          std::string dump_path = RPG_GRAPHICS_DUMP_DIR;
          dump_path += ACE_DIRECTORY_SEPARATOR_STR;
          dump_path += ACE_TEXT("screenshot_");
          dump_path += converter.str();
          dump_path += ACE_TEXT(".png");
          RPG_Graphics_Surface::savePNG(*SDL_GetVideoSurface(), // image
                                        dump_path,              // file
                                        false);                 // no alpha

          break;
        }
        default:
        {

          break;
        }
      } // end SWITCH

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
//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("mouse motion...\n")));

      break;
    }
    case SDL_MOUSEBUTTONDOWN:
    {
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("mouse button [%u,%u] pressed\n"),
                 ACE_static_cast(unsigned long, event_in.button.which),
                 ACE_static_cast(unsigned long, event_in.button.button)));

      break;
    }
    case SDL_MOUSEBUTTONUP:
    {
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("mouse button [%u,%u] released...\n"),
                 ACE_static_cast(unsigned long, event_in.button.which),
                 ACE_static_cast(unsigned long, event_in.button.button)));

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
    case SDL_GUI_SDL_HOVEREVENT:
    {
//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("SDL_GUI_SDL_HOVEREVENT event (%d ms)...\n"),
//                  event_in.user.code));

      // don't hover if we've lost the mouse
      if (!myHaveMouseFocus)
        break;

      // hovering on a hotspot (edge) area triggers a scroll of the viewport
      if (window_in == this)
        break; // not a hotspot

      // sanity check
      ACE_ASSERT(window_in->getType() == WINDOWTYPE_HOTSPOT);

      // retrieve hotspot window handle
      RPG_Graphics_HotSpot* hotspot = NULL;
      hotspot = ACE_dynamic_cast(RPG_Graphics_HotSpot*, window_in);
      if (!hotspot)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("dynamic downcast failed, aborting\n")));

        break;
      } // end IF

      // retrieve map window handle
      RPG_Graphics_WindowsConstIterator_t iterator = myChildren.begin();
      for (;
           iterator != myChildren.end();
           iterator++)
      {
        if ((*iterator)->getType() == WINDOWTYPE_MAP)
          break;
      } // end FOR
      ACE_ASSERT((*iterator)->getType() == WINDOWTYPE_MAP);
      SDL_GUI_LevelWindow* levelWindow = NULL;
      levelWindow = ACE_dynamic_cast(SDL_GUI_LevelWindow*, *iterator);
      if (!levelWindow)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("dynamic downcast failed, aborting\n")));

        break;
      } // end IF

      if (myLastHoverTime)
      {
        // throttle scrolling
        if ((event_in.user.code - myLastHoverTime) < RPG_GRAPHICS_WINDOW_HOTSPOT_HOVER_SCROLL_DELAY)
          break; // don't scroll this time
      } // end ELSE
      myLastHoverTime = event_in.user.code;

      switch (hotspot->getHotSpotType())
      {
        case TYPE_CURSOR_SCROLL_UL:
        {
          levelWindow->setView(-RPG_GRAPHICS_WINDOW_SCROLL_KEYPRESS_OFFSET,
                               -RPG_GRAPHICS_WINDOW_SCROLL_KEYPRESS_OFFSET);
          levelWindow->setView(-RPG_GRAPHICS_WINDOW_SCROLL_KEYPRESS_OFFSET,
                               RPG_GRAPHICS_WINDOW_SCROLL_KEYPRESS_OFFSET);

          break;
        }
        case TYPE_CURSOR_SCROLL_U:
        {
          levelWindow->setView(-RPG_GRAPHICS_WINDOW_SCROLL_KEYPRESS_OFFSET,
                               -RPG_GRAPHICS_WINDOW_SCROLL_KEYPRESS_OFFSET);

          break;
        }
        case TYPE_CURSOR_SCROLL_UR:
        {
          levelWindow->setView(-RPG_GRAPHICS_WINDOW_SCROLL_KEYPRESS_OFFSET,
                               -RPG_GRAPHICS_WINDOW_SCROLL_KEYPRESS_OFFSET);
          levelWindow->setView(RPG_GRAPHICS_WINDOW_SCROLL_KEYPRESS_OFFSET,
                               -RPG_GRAPHICS_WINDOW_SCROLL_KEYPRESS_OFFSET);

          break;
        }
        case TYPE_CURSOR_SCROLL_L:
        {
          levelWindow->setView(-RPG_GRAPHICS_WINDOW_SCROLL_KEYPRESS_OFFSET,
                               RPG_GRAPHICS_WINDOW_SCROLL_KEYPRESS_OFFSET);

          break;
        }
        case TYPE_CURSOR_SCROLL_R:
        {
          levelWindow->setView(RPG_GRAPHICS_WINDOW_SCROLL_KEYPRESS_OFFSET,
                               -RPG_GRAPHICS_WINDOW_SCROLL_KEYPRESS_OFFSET);

          break;
        }
        case TYPE_CURSOR_SCROLL_DL:
        {
          levelWindow->setView(RPG_GRAPHICS_WINDOW_SCROLL_KEYPRESS_OFFSET,
                               RPG_GRAPHICS_WINDOW_SCROLL_KEYPRESS_OFFSET);
          levelWindow->setView(-RPG_GRAPHICS_WINDOW_SCROLL_KEYPRESS_OFFSET,
                               RPG_GRAPHICS_WINDOW_SCROLL_KEYPRESS_OFFSET);

          break;
        }
        case TYPE_CURSOR_SCROLL_D:
        {
          levelWindow->setView(RPG_GRAPHICS_WINDOW_SCROLL_KEYPRESS_OFFSET,
                               RPG_GRAPHICS_WINDOW_SCROLL_KEYPRESS_OFFSET);

          break;
        }
        case TYPE_CURSOR_SCROLL_DR:
        {
          levelWindow->setView(RPG_GRAPHICS_WINDOW_SCROLL_KEYPRESS_OFFSET,
                               RPG_GRAPHICS_WINDOW_SCROLL_KEYPRESS_OFFSET);
          levelWindow->setView(RPG_GRAPHICS_WINDOW_SCROLL_KEYPRESS_OFFSET,
                               -RPG_GRAPHICS_WINDOW_SCROLL_KEYPRESS_OFFSET);

          break;
        }
        default:
        {
          ACE_DEBUG((LM_DEBUG,
                     ACE_TEXT("invalid/unknown hotspot type (was: %s), aborting\n"),
                     RPG_Graphics_TypeHelper::RPG_Graphics_TypeToString(hotspot->getHotSpotType()).c_str()));

          return;
        }
      } // end SWITCH

      // need a redraw
      redraw_out = true;

//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("scrolled map (%s)...\n"),
//                  RPG_Graphics_TypeHelper::RPG_Graphics_TypeToString(hotspot->getHotSpotType()).c_str()));

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

  // if necessary, reset last hover time
  if (event_in.type != SDL_GUI_SDL_HOVEREVENT)
    myLastHoverTime = 0;
}

void
SDL_GUI_MainWindow::initScrollSpots()
{
  ACE_TRACE(ACE_TEXT("SDL_GUI_MainWindow::initScrollSpots"));

  // upper left
  RPG_Graphics_HotSpot::init(*this,                  // parent
                             std::make_pair(RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN,
                                            RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN), // size
                             std::make_pair(0,
                                            0),      // offset
                             TYPE_CURSOR_SCROLL_UL); // (hover) cursor graphic
  // up
  RPG_Graphics_HotSpot::init(*this,                 // parent
                             std::make_pair(mySize.first - (2 * RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN),
                                            RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN), // size
                             std::make_pair(RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN,
                                            0),     // offset
                             TYPE_CURSOR_SCROLL_U); // (hover) cursor graphic
  // upper right
  RPG_Graphics_HotSpot::init(*this,                  // parent
                             std::make_pair(RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN,
                                            RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN), // size
                             std::make_pair(mySize.first - RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN,
                                            0),      // offset
                             TYPE_CURSOR_SCROLL_UR); // (hover) cursor graphic
  // left
  RPG_Graphics_HotSpot::init(*this,                 // parent
                             std::make_pair(RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN,
                                            mySize.second - (2 * RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN)), // size
                             std::make_pair(0,
                                            RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN), // offset
                             TYPE_CURSOR_SCROLL_L); // (hover) cursor graphic
  // right
  RPG_Graphics_HotSpot::init(*this,                 // parent
                             std::make_pair(RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN,
                                            mySize.second - (2 * RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN)), // size
                             std::make_pair(mySize.first - RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN,
                                            RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN), // offset
                             TYPE_CURSOR_SCROLL_R); // (hover) cursor graphic
  // lower left
  RPG_Graphics_HotSpot::init(*this,                  // parent
                             std::make_pair(RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN,
                                            RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN), // size
                             std::make_pair(0,
                                            mySize.second - RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN), // offset
                             TYPE_CURSOR_SCROLL_DL); // (hover) cursor graphic
  // down
  RPG_Graphics_HotSpot::init(*this,                 // parent
                             std::make_pair(mySize.first - (2 * RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN),
                                            RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN), // size
                             std::make_pair(RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN,
                                            mySize.second - RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN), // offset
                             TYPE_CURSOR_SCROLL_D); // (hover) cursor graphic
  // lower right
  RPG_Graphics_HotSpot::init(*this,                  // parent
                             std::make_pair(RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN,
                                            RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN), // size
                             std::make_pair(mySize.first - RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN,
                                            mySize.second - RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN), // offset
                             TYPE_CURSOR_SCROLL_DR); // (hover) cursor graphic
}
