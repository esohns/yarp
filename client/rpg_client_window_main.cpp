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

#include "rpg_client_window_main.h"

#include "rpg_client_defines.h"
#include "rpg_client_iwindow_level.h"
#include "rpg_client_engine.h"
#include "rpg_client_window_level.h"

#include <rpg_graphics_defines.h>
#include <rpg_graphics_surface.h>
#include <rpg_graphics_cursor_manager.h>
#include <rpg_graphics_hotspot.h>
#include <rpg_graphics_common_tools.h>
#include <rpg_graphics_SDL_tools.h>

#include <rpg_engine.h>

#include <rpg_common_macros.h>
#include <rpg_common_defines.h>

#include <ace/Log_Msg.h>

#include <sstream>

RPG_Client_WindowMain::RPG_Client_WindowMain(const RPG_Graphics_Size_t& size_in,
                                             const RPG_Graphics_GraphicTypeUnion& elementType_in,
                                             const std::string& title_in,
                                             const RPG_Graphics_Font& font_in)
 : inherited(size_in,        // size
             elementType_in, // element type
             title_in),      // title
//              NULL),          // background
   myEngine(NULL),
   myScreenshotIndex(1),
   myLastHoverTime(0),
   myAutoEdgeScroll(RPG_CLIENT_DEF_WINDOW_EDGE_AUTOSCROLL),
   myTitleFont(font_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_WindowMain::RPG_Client_WindowMain"));

}

RPG_Client_WindowMain::~RPG_Client_WindowMain()
{
  RPG_TRACE(ACE_TEXT("RPG_Client_WindowMain::~RPG_Client_WindowMain"));

}

void
RPG_Client_WindowMain::init(RPG_Client_Engine* clientEngine_in,
                            const bool& doAutoEdgeScroll_in,
                            RPG_Engine* engine_in,
                            const RPG_Graphics_MapStyle_t& style_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_WindowMain::init"));

  // sanity checks
  ACE_ASSERT(clientEngine_in);

  myEngine = clientEngine_in;
  myAutoEdgeScroll = doAutoEdgeScroll_in;

  // init scroll margins
  initScrollSpots();

  // init map
  initMap(clientEngine_in,
          engine_in,
          style_in);
}

void
RPG_Client_WindowMain::draw(SDL_Surface* targetSurface_in,
                            const unsigned int& offsetX_in,
                            const unsigned int& offsetY_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_WindowMain::draw"));

  // set target surface
  SDL_Surface* targetSurface = (targetSurface_in ? targetSurface_in : myScreen);

  // sanity check(s)
  ACE_ASSERT(targetSurface);
  ACE_ASSERT(static_cast<int>(offsetX_in) <= targetSurface->w);
  ACE_ASSERT(static_cast<int>(offsetY_in) <= targetSurface->h);

//   // init clipping
//   clip(targetSurface,
//        offsetX_in,
//        offsetY_in);

  // step1: draw borders
  drawBorder(targetSurface,
             offsetX_in,
             offsetY_in);

  // step2: fill central area
  SDL_Rect clipRect;
  clipRect.x = static_cast<int16_t>(offsetX_in + myBorderLeft);
  clipRect.y = static_cast<int16_t>(offsetY_in + myBorderTop);
  clipRect.w = static_cast<uint16_t>(targetSurface->w -
	                                   offsetX_in -
									                   (myBorderLeft + myBorderRight));
  clipRect.h = static_cast<uint16_t>(targetSurface->h -
	                                   offsetY_in -
								                  	 (myBorderTop + myBorderBottom));
  if (!SDL_SetClipRect(targetSurface, &clipRect))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_SetClipRect(): %s, aborting\n"),
               SDL_GetError()));

    return;
  } // end IF
  RPG_Graphics_InterfaceElementsConstIterator_t iterator;
  iterator = myElementGraphics.find(INTERFACEELEMENT_BORDER_CENTER);
  ACE_ASSERT(iterator != myElementGraphics.end());
  for (unsigned int i = (offsetY_in + myBorderTop);
       i < (targetSurface->h - myBorderBottom);
       i += (*iterator).second->h)
    for (unsigned int j = (offsetX_in + myBorderLeft);
         j < (targetSurface->w - myBorderRight);
         j += (*iterator).second->w)
      RPG_Graphics_Surface::put(j,
                                i,
                                *(*iterator).second,
                                targetSurface);
  invalidate(clipRect);
  if (!SDL_SetClipRect(targetSurface, NULL))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_SetClipRect(): %s, aborting\n"),
               SDL_GetError()));

    return;
  } // end IF

  // step3: draw title (if any)
  if (!myTitle.empty())
  {
    clipRect.x = static_cast<int16_t>(myBorderLeft);
    clipRect.y = 0;
    clipRect.w = static_cast<uint16_t>(targetSurface->w -
		                                   offsetX_in -
									                     (myBorderLeft + myBorderRight));
    clipRect.h = static_cast<uint16_t>(myBorderTop);
    if (!SDL_SetClipRect(targetSurface, &clipRect))
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
                                                                          *targetSurface),
                                  true, // add shade
                                  RPG_Graphics_SDL_Tools::colorToSDLColor(RPG_GRAPHICS_FONT_DEF_SHADECOLOR,
                                                                          *targetSurface),
                                  myBorderLeft, // top left
                                  ((myBorderTop - title_size.second) / 2), // center of top border
                                  targetSurface);
    invalidate(clipRect);
    if (!SDL_SetClipRect(targetSurface, NULL))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to SDL_SetClipRect(): %s, aborting\n"),
                 SDL_GetError()));

      return;
    } // end IF
  } // end IF

  // step4: realize hotspots (and any other children)
  for (RPG_Graphics_WindowsIterator_t iterator = myChildren.begin();
       iterator != myChildren.end();
       iterator++)
  {
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

  // whole window needs a refresh...
  SDL_Rect dirtyRegion;
  SDL_GetClipRect(targetSurface, &dirtyRegion);
  invalidate(dirtyRegion);

//   // restore previous clipping area
//   unclip(targetSurface);

  // remember position of last realization
  myLastAbsolutePosition = std::make_pair(offsetX_in,
                                          offsetY_in);
}

void
RPG_Client_WindowMain::handleEvent(const SDL_Event& event_in,
                                   RPG_Graphics_IWindow* window_in,
                                   bool& redraw_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_WindowMain::handleEvent"));

  // init return value(s)
  redraw_out = false;

  RPG_Client_Action client_action;
  client_action.command = RPG_CLIENT_COMMAND_INVALID;
  client_action.position = std::make_pair(std::numeric_limits<unsigned int>::max(),
                                          std::numeric_limits<unsigned int>::max());
  client_action.window = this;
  client_action.cursor = RPG_GRAPHICS_CURSOR_INVALID;
  client_action.entity_id = 0;
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

          //// *HACK*: prevents MOST "mouse trails" (NW borders)...
          //client_action.command = COMMAND_WINDOW_BORDER_DRAW;
          //myEngine->action(client_action);
          //client_action.command = COMMAND_WINDOW_REFRESH;
          //myEngine->action(client_action);

          //myHaveMouseFocus = true;
        } // end IF
        else
        {
//           ACE_DEBUG((LM_DEBUG,
//                      ACE_TEXT("lost mouse coverage...\n")));

          client_action.command = COMMAND_CURSOR_RESTORE_BG;
          client_action.window = this;
          myEngine->action(client_action);

          //// *HACK*: prevents MOST "mouse trails" (NW borders)...
          //client_action.command = COMMAND_WINDOW_BORDER_DRAW;
          //myEngine->action(client_action);
          //client_action.command = COMMAND_WINDOW_REFRESH;
          //myEngine->action(client_action);

          //myHaveMouseFocus = false;
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
//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("%s key\n%s\n"),
//                  ((event_in.type == SDL_KEYDOWN) ? ACE_TEXT("pressed") : ACE_TEXT("released")),
//                  RPG_Graphics_SDL_Tools::keyToString(event_in.key.keysym).c_str()));

      switch (event_in.key.keysym.sym)
      {
        case SDLK_s:
        {
          std::ostringstream converter;
          converter << myScreenshotIndex++;
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
          std::string dump_path = ACE_TEXT_ALWAYS_CHAR(RPG_COMMON_DUMP_DIR);
#else
          std::string dump_path = ACE_OS::getenv(ACE_TEXT_ALWAYS_CHAR(RPG_COMMON_DUMP_DIR));
#endif
          dump_path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
          dump_path += ACE_TEXT(RPG_CLIENT_DEF_SCREENSHOT_PREFIX);
          dump_path += converter.str();
          dump_path += ACE_TEXT(RPG_CLIENT_DEF_SCREENSHOT_EXT);
          RPG_Graphics_Surface::savePNG(*myScreen, // image
                                        dump_path, // file
                                        false);    // no alpha

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
//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("key released...\n")));

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
//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("mouse button [%u,%u] pressed\n"),
//                  static_cast<unsigned long> (event_in.button.which),
//                  static_cast<unsigned long> (event_in.button.button)));

      // (left-)clicking on a hotspot (edge) area triggers a scroll of the viewport
      if ((window_in->getType() == WINDOW_HOTSPOT) &&
          (event_in.button.button == 1))
      {
        // retrieve hotspot window handle
        RPG_Graphics_HotSpot* hotspot = NULL;
        hotspot = dynamic_cast<RPG_Graphics_HotSpot*>(window_in);
        ACE_ASSERT(hotspot);

        // retrieve map window handle
        client_action.command = COMMAND_SET_VIEW;
        client_action.window = child(WINDOW_MAP);
        RPG_Client_IWindowLevel* level_window = dynamic_cast<RPG_Client_IWindowLevel*>(client_action.window);
        ACE_ASSERT(level_window);
        client_action.position = level_window->getView();
        switch (hotspot->getCursorType())
        {
          case CURSOR_SCROLL_UL:
          {
            client_action.position.first--;
            client_action.position.second--;
            //level_window->setView(-RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
            //                      -RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);
            client_action.position.first--;
            client_action.position.second++;
            //level_window->setView(-RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
            //                      RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);

            break;
          }
          case CURSOR_SCROLL_U:
          {
            client_action.position.first--;
            client_action.position.second--;
            //level_window->setView(-RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
            //                      -RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);

            break;
          }
          case CURSOR_SCROLL_UR:
          {
            client_action.position.first--;
            client_action.position.second--;
            //level_window->setView(-RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
            //                      -RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);
            client_action.position.first++;
            client_action.position.second--;
            //level_window->setView(RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
            //                      -RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);

            break;
          }
          case CURSOR_SCROLL_L:
          {
            client_action.position.first--;
            client_action.position.second++;
            //level_window->setView(-RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
            //                      RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);

            break;
          }
          case CURSOR_SCROLL_R:
          {
            client_action.position.first++;
            client_action.position.second--;
            //level_window->setView(RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
            //                      -RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);

            break;
          }
          case CURSOR_SCROLL_DL:
          {
            client_action.position.first++;
            client_action.position.second++;
            //level_window->setView(RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
            //                      RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);
            client_action.position.first--;
            client_action.position.second++;
            //level_window->setView(-RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
            //                      RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);

            break;
          }
          case CURSOR_SCROLL_D:
          {
            client_action.position.first++;
            client_action.position.second++;
            //level_window->setView(RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
            //                      RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);

            break;
          }
          case CURSOR_SCROLL_DR:
          {
            client_action.position.first++;
            client_action.position.second++;
            //level_window->setView(RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
            //                      RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);
            client_action.position.first++;
            client_action.position.second--;
            //level_window->setView(RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
            //                      -RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);

            break;
          }
          default:
          {
            ACE_DEBUG((LM_DEBUG,
                       ACE_TEXT("invalid/unknown cursor type (was: %s), aborting\n"),
                       RPG_Graphics_CursorHelper::RPG_Graphics_CursorToString(hotspot->getCursorType()).c_str()));

            return;
          }
        } // end SWITCH
        myEngine->action(client_action);
      } // end IF

      break;
    }
    case SDL_MOUSEBUTTONUP:
    {
//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("mouse button [%u,%u] released...\n"),
//                  static_cast<unsigned long> (event_in.button.which),
//                  static_cast<unsigned long> (event_in.button.button)));

      break;
    }
    // *** joystick ***
    case SDL_JOYAXISMOTION:
    case SDL_JOYBALLMOTION:
    case SDL_JOYHATMOTION:
    case SDL_JOYBUTTONDOWN:
    case SDL_JOYBUTTONUP:
    {
//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("joystick activity...\n")));

      break;
    }
    case SDL_QUIT:
    {
//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("SDL_QUIT event...\n")));

      break;
    }
    case SDL_SYSWMEVENT:
    {
//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("SDL_SYSWMEVENT event...\n")));

      break;
    }
    case SDL_VIDEORESIZE:
    {
//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("SDL_VIDEORESIZE event...\n")));

      break;
    }
    case SDL_VIDEOEXPOSE:
    {
//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("SDL_VIDEOEXPOSE event...\n")));

      break;
    }
    case RPG_GRAPHICS_SDL_HOVEREVENT:
    {
//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("RPG_GRAPHICS_SDL_HOVEREVENT event (%d ms)...\n"),
//                  event_in.user.code));

      // hovering on a hotspot (edge) area may trigger a scroll of the viewport
      if (!myAutoEdgeScroll ||                      // function is disabled
          (window_in->getType() != WINDOW_HOTSPOT)) // not a hotspot
        break;

      if (myLastHoverTime)
      {
        // throttle scrolling
        if ((event_in.user.code - myLastHoverTime) < RPG_GRAPHICS_WINDOW_HOTSPOT_HOVER_SCROLL_DELAY)
          break; // don't scroll this time
      } // end ELSE
      myLastHoverTime = event_in.user.code;

      client_action.command = COMMAND_SET_VIEW;

      // retrieve map window handle
      client_action.window = child(WINDOW_MAP);
      RPG_Client_IWindowLevel* level_window = dynamic_cast<RPG_Client_IWindowLevel*>(client_action.window);
      ACE_ASSERT(level_window);

      // retrieve hotspot window handle
      RPG_Graphics_HotSpot* hotspot = NULL;
      hotspot = dynamic_cast<RPG_Graphics_HotSpot*>(window_in);
      ACE_ASSERT(hotspot);

      client_action.position = level_window->getView();
      switch (hotspot->getCursorType())
      {
        case CURSOR_SCROLL_UL:
        {
          client_action.position.first--;
          client_action.position.second--;
          //level_window->setView(-RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
          //                      -RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);
          client_action.position.first--;
          client_action.position.second++;
          //level_window->setView(-RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
          //                      RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);

          break;
        }
        case CURSOR_SCROLL_U:
        {
          client_action.position.first--;
          client_action.position.second--;
          //level_window->setView(-RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
          //                      -RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);

          break;
        }
        case CURSOR_SCROLL_UR:
        {
          client_action.position.first--;
          client_action.position.second--;
          //level_window->setView(-RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
          //                      -RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);
          client_action.position.first++;
          client_action.position.second--;
          //level_window->setView(RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
          //                      -RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);

          break;
        }
        case CURSOR_SCROLL_L:
        {
          client_action.position.first--;
          client_action.position.second++;
          //level_window->setView(-RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
          //                      RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);

          break;
        }
        case CURSOR_SCROLL_R:
        {
          client_action.position.first++;
          client_action.position.second--;
          //level_window->setView(RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
          //                      -RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);

          break;
        }
        case CURSOR_SCROLL_DL:
        {
          client_action.position.first++;
          client_action.position.second++;
          //level_window->setView(RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
          //                      RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);
          client_action.position.first--;
          client_action.position.second++;
          //level_window->setView(-RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
          //                      RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);

          break;
        }
        case CURSOR_SCROLL_D:
        {
          client_action.position.first++;
          client_action.position.second++;
          //level_window->setView(RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
          //                      RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);

          break;
        }
        case CURSOR_SCROLL_DR:
        {
          client_action.position.first++;
          client_action.position.second++;
          //level_window->setView(RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
          //                      RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);
          client_action.position.first++;
          client_action.position.second--;
          //level_window->setView(RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
          //                      -RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);

          break;
        }
        default:
        {
          ACE_DEBUG((LM_DEBUG,
                      ACE_TEXT("invalid/unknown cursor type (was: %s), aborting\n"),
                      RPG_Graphics_CursorHelper::RPG_Graphics_CursorToString(hotspot->getCursorType()).c_str()));

          return;
        }
      } // end SWITCH
      myEngine->action(client_action);

      break;
    }
    case RPG_GRAPHICS_SDL_MOUSEMOVEOUT:
    {
//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("RPG_GRAPHICS_SDL_MOUSEMOVEOUT event...\n")));

      break;
    }
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("received unknown event (was: %u)...\n"),
                 static_cast<unsigned int>(event_in.type)));

      break;
    }
  } // end SWITCH

  // if necessary, reset last hover time
  if (event_in.type != RPG_GRAPHICS_SDL_HOVEREVENT)
    myLastHoverTime = 0;
}

void
RPG_Client_WindowMain::notify(const RPG_Graphics_Cursor& cursor_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Client_WindowMain::notify"));

  RPG_Client_Action client_action;
  client_action.command = RPG_CLIENT_COMMAND_INVALID;
  client_action.position = std::make_pair(std::numeric_limits<unsigned int>::max(),
                                          std::numeric_limits<unsigned int>::max());
  client_action.window = const_cast<RPG_Client_WindowMain*>(this);
  client_action.cursor = cursor_in;
  client_action.entity_id = 0;
  if (cursor_in == RPG_GRAPHICS_CURSOR_INVALID)
  {
    client_action.command = COMMAND_CURSOR_RESTORE_BG;
    myEngine->action(client_action);

    return;
  } // end IF

  client_action.command = COMMAND_CURSOR_SET;
  myEngine->action(client_action);
}

void
RPG_Client_WindowMain::initScrollSpots()
{
  RPG_TRACE(ACE_TEXT("RPG_Client_WindowMain::initScrollSpots"));

  // upper left
  RPG_Graphics_HotSpot::init(*this,                  // parent
                             std::make_pair(RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN,
                                            RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN), // size
                             std::make_pair(0,
                                            0),      // offset
                             CURSOR_SCROLL_UL); // (hover) cursor graphic
  // up
  RPG_Graphics_HotSpot::init(*this,                 // parent
                             std::make_pair(mySize.first - (2 * RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN),
                                            RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN), // size
                             std::make_pair(RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN,
                                            0),     // offset
                             CURSOR_SCROLL_U); // (hover) cursor graphic
  // upper right
  RPG_Graphics_HotSpot::init(*this,                  // parent
                             std::make_pair(RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN,
                                            RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN), // size
                             std::make_pair(mySize.first - RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN,
                                            0),      // offset
                             CURSOR_SCROLL_UR); // (hover) cursor graphic
  // left
  RPG_Graphics_HotSpot::init(*this,                 // parent
                             std::make_pair(RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN,
                                            mySize.second - (2 * RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN)), // size
                             std::make_pair(0,
                                            RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN), // offset
                             CURSOR_SCROLL_L); // (hover) cursor graphic
  // right
  RPG_Graphics_HotSpot::init(*this,                 // parent
                             std::make_pair(RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN,
                                            mySize.second - (2 * RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN)), // size
                             std::make_pair(mySize.first - RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN,
                                            RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN), // offset
                             CURSOR_SCROLL_R); // (hover) cursor graphic
  // lower left
  RPG_Graphics_HotSpot::init(*this,                  // parent
                             std::make_pair(RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN,
                                            RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN), // size
                             std::make_pair(0,
                                            mySize.second - RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN), // offset
                             CURSOR_SCROLL_DL); // (hover) cursor graphic
  // down
  RPG_Graphics_HotSpot::init(*this,                 // parent
                             std::make_pair(mySize.first - (2 * RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN),
                                            RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN), // size
                             std::make_pair(RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN,
                                            mySize.second - RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN), // offset
                             CURSOR_SCROLL_D); // (hover) cursor graphic
  // lower right
  RPG_Graphics_HotSpot::init(*this,                  // parent
                             std::make_pair(RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN,
                                            RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN), // size
                             std::make_pair(mySize.first - RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN,
                                            mySize.second - RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN), // offset
                             CURSOR_SCROLL_DR); // (hover) cursor graphic
}

void
RPG_Client_WindowMain::initMap(RPG_Client_Engine* clientEngine_in,
                               RPG_Engine* engine_in,
                               const RPG_Graphics_MapStyle_t& style_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_WindowMain::initMap"));

  RPG_Client_WindowLevel* map_window = NULL;
  map_window = new(std::nothrow) RPG_Client_WindowLevel(*this);
  if (!map_window)
  {
    ACE_DEBUG((LM_CRITICAL,
               ACE_TEXT("failed to allocate memory: %m, aborting\n")));

    return;
  } // end IF

  // init window
  map_window->init(clientEngine_in,
                   engine_in,
                   style_in);
  map_window->setScreen(myScreen);
}

void
RPG_Client_WindowMain::drawBorder(SDL_Surface* targetSurface_in,
                                  const unsigned int& offsetX_in,
                                  const unsigned int& offsetY_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_WindowMain::drawBorder"));

  // set target surface
  SDL_Surface* targetSurface = (targetSurface_in ? targetSurface_in : myScreen);

  // sanity check(s)
  ACE_ASSERT(targetSurface);
  ACE_ASSERT(static_cast<int>(offsetX_in) <= targetSurface->w);
  ACE_ASSERT(static_cast<int>(offsetY_in) <= targetSurface->h);

  RPG_Graphics_InterfaceElementsConstIterator_t iterator;
  SDL_Rect prev_clipRect, clipRect;
  unsigned int i = 0;

  // step0: save previous clipRect
  SDL_GetClipRect(targetSurface, &prev_clipRect);

  // step1: draw borders
  clipRect.x = static_cast<int16_t>(offsetX_in + myBorderLeft);
  clipRect.y = static_cast<int16_t>(offsetY_in);
  clipRect.w = static_cast<uint16_t>(targetSurface->w -
	                                 (myBorderLeft + myBorderRight));
  clipRect.h = static_cast<uint16_t>(myBorderTop);
  if (!SDL_SetClipRect(targetSurface, &clipRect))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_SetClipRect(): %s, aborting\n"),
               SDL_GetError()));

    return;
  } // end IF
  iterator = myElementGraphics.find(INTERFACEELEMENT_BORDER_TOP);
  ACE_ASSERT(iterator != myElementGraphics.end());
  for (i = offsetX_in + myBorderLeft;
       i < (static_cast<unsigned int>(targetSurface->w) - myBorderRight);
       i += (*iterator).second->w)
    RPG_Graphics_Surface::put(i,
                              offsetY_in,
                              *(*iterator).second,
                              targetSurface);
  invalidate(clipRect);

  clipRect.x = static_cast<int16_t>(offsetX_in);
  clipRect.y = static_cast<int16_t>(offsetY_in + myBorderTop);
  clipRect.w = static_cast<uint16_t>(myBorderLeft);
  clipRect.h = static_cast<uint16_t>(targetSurface->h -
	                                   (myBorderTop + myBorderBottom));
  if (!SDL_SetClipRect(targetSurface, &clipRect))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_SetClipRect(): %s, aborting\n"),
               SDL_GetError()));

    return;
  } // end IF
  iterator = myElementGraphics.find(INTERFACEELEMENT_BORDER_LEFT);
  ACE_ASSERT(iterator != myElementGraphics.end());
  for (i = (offsetY_in + myBorderTop);
       i < (static_cast<unsigned int>(targetSurface->h) - myBorderBottom);
       i += (*iterator).second->h)
    RPG_Graphics_Surface::put(offsetX_in,
                              i,
                              *(*iterator).second,
                              targetSurface);
  invalidate(clipRect);

  clipRect.x = static_cast<int16_t>(targetSurface->w - myBorderRight);
  clipRect.y = static_cast<int16_t>(offsetY_in + myBorderTop);
  clipRect.w = static_cast<uint16_t>(myBorderRight);
  clipRect.h = static_cast<uint16_t>(targetSurface->h -
	                                   (myBorderTop + myBorderBottom));
  if (!SDL_SetClipRect(targetSurface, &clipRect))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_SetClipRect(): %s, aborting\n"),
               SDL_GetError()));

    return;
  } // end IF
  iterator = myElementGraphics.find(INTERFACEELEMENT_BORDER_RIGHT);
  ACE_ASSERT(iterator != myElementGraphics.end());
  for (i = (offsetY_in + myBorderTop);
       i < (static_cast<unsigned int> (targetSurface->h) - myBorderBottom);
       i += (*iterator).second->h)
    RPG_Graphics_Surface::put((targetSurface->w - myBorderRight),
                               i,
                               *(*iterator).second,
                               targetSurface);
  invalidate(clipRect);

  clipRect.x = static_cast<int16_t>(offsetX_in + myBorderLeft);
  clipRect.y = static_cast<int16_t>(targetSurface->h - myBorderBottom);
  clipRect.w = static_cast<uint16_t>(targetSurface->w -
	                                   (myBorderLeft + myBorderRight));
  clipRect.h = static_cast<uint16_t>(myBorderBottom);
  if (!SDL_SetClipRect(targetSurface, &clipRect))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_SetClipRect(): %s, aborting\n"),
               SDL_GetError()));

    return;
  } // end IF
  iterator = myElementGraphics.find(INTERFACEELEMENT_BORDER_BOTTOM);
  ACE_ASSERT(iterator != myElementGraphics.end());
  for (i = (offsetX_in + myBorderLeft);
       i < (targetSurface->w - myBorderRight);
       i += (*iterator).second->w)
    RPG_Graphics_Surface::put(i,
                              (targetSurface->h - myBorderBottom),
                              *(*iterator).second,
                              targetSurface);
  invalidate(clipRect);

  // step2: draw corners

  // NW
  clipRect.x = static_cast<int16_t>(offsetX_in);
  clipRect.y = static_cast<int16_t>(offsetY_in);
  iterator = myElementGraphics.find(INTERFACEELEMENT_BORDER_TOP_LEFT);
  ACE_ASSERT(iterator != myElementGraphics.end());
  clipRect.w = static_cast<uint16_t>((*iterator).second->w);
  clipRect.h = static_cast<uint16_t>((*iterator).second->h);
  if (!SDL_SetClipRect(targetSurface, &clipRect))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_SetClipRect(): %s, aborting\n"),
               SDL_GetError()));

    return;
  } // end IF
  RPG_Graphics_Surface::put(offsetX_in,
                            offsetY_in,
                            *(*iterator).second,
                            targetSurface);
  invalidate(clipRect);

  // NE
  clipRect.y = static_cast<int16_t>(offsetY_in);
  iterator = myElementGraphics.find(INTERFACEELEMENT_BORDER_TOP_RIGHT);
  ACE_ASSERT(iterator != myElementGraphics.end());
  clipRect.x = static_cast<int16_t>(targetSurface->w - (*iterator).second->w);
  clipRect.w = static_cast<uint16_t>((*iterator).second->w);
  clipRect.h = static_cast<uint16_t>((*iterator).second->h);
  if (!SDL_SetClipRect(targetSurface, &clipRect))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_SetClipRect(): %s, aborting\n"),
               SDL_GetError()));

    return;
  } // end IF
  RPG_Graphics_Surface::put((targetSurface->w - (*iterator).second->w),
                            offsetY_in,
                            *(*iterator).second,
                            targetSurface);
  invalidate(clipRect);

  // SW
  clipRect.x = static_cast<int16_t>(offsetX_in);
  iterator = myElementGraphics.find(INTERFACEELEMENT_BORDER_BOTTOM_LEFT);
  ACE_ASSERT(iterator != myElementGraphics.end());
  clipRect.y = static_cast<int16_t>(targetSurface->h - (*iterator).second->h);
  clipRect.w = static_cast<uint16_t>((*iterator).second->w);
  clipRect.h = static_cast<uint16_t>((*iterator).second->h);
  if (!SDL_SetClipRect(targetSurface, &clipRect))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_SetClipRect(): %s, aborting\n"),
               SDL_GetError()));

    return;
  } // end IF
  RPG_Graphics_Surface::put(offsetX_in,
                            (targetSurface->h - (*iterator).second->h),
                            *(*iterator).second,
                            targetSurface);
  invalidate(clipRect);

  // SE
  iterator = myElementGraphics.find(INTERFACEELEMENT_BORDER_BOTTOM_RIGHT);
  ACE_ASSERT(iterator != myElementGraphics.end());
  clipRect.x = static_cast<int16_t>(targetSurface->w - (*iterator).second->w);
  clipRect.y = static_cast<int16_t>(targetSurface->h - (*iterator).second->h);
  clipRect.w = static_cast<uint16_t>((*iterator).second->w);
  clipRect.h = static_cast<uint16_t>((*iterator).second->h);
  if (!SDL_SetClipRect(targetSurface, &clipRect))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_SetClipRect(): %s, aborting\n"),
               SDL_GetError()));

    return;
  } // end IF
  RPG_Graphics_Surface::put((targetSurface->w - (*iterator).second->w),
                            (targetSurface->h - (*iterator).second->h),
                            *(*iterator).second,
                            targetSurface);
  invalidate(clipRect);

  // restore previous clipping area
  if (!SDL_SetClipRect(targetSurface, &prev_clipRect))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_SetClipRect(): %s, aborting\n"),
               SDL_GetError()));

    return;
  } // end IF
}
