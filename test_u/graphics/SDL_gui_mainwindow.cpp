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

#include <rpg_client_defines.h>

#include <rpg_engine_level.h>

#include <rpg_graphics_defines.h>
#include <rpg_graphics_surface.h>
#include <rpg_graphics_cursor_manager.h>
#include <rpg_graphics_hotspot.h>
#include <rpg_graphics_common_tools.h>
#include <rpg_graphics_SDL_tools.h>

#include <rpg_common_macros.h>
#include <rpg_common_defines.h>

#include <ace/Log_Msg.h>

#include <sstream>

SDL_GUI_MainWindow::SDL_GUI_MainWindow(const RPG_Graphics_Size_t& size_in,
                                       const RPG_Graphics_GraphicTypeUnion& elementType_in,
                                       const std::string& title_in,
                                       const RPG_Graphics_Font& fontType_in)
 : inherited(size_in,        // size
             elementType_in, // element type
             title_in),      // title
//              NULL),          // background
   myScreenshotIndex(1),
   myLastHoverTime(0),
   myHaveMouseFocus(true), // *NOTE*: enforced with SDL_WarpMouse()
   myTitleFont(fontType_in)
{
  RPG_TRACE(ACE_TEXT("SDL_GUI_MainWindow::SDL_GUI_MainWindow"));

}

SDL_GUI_MainWindow::~SDL_GUI_MainWindow()
{
  RPG_TRACE(ACE_TEXT("SDL_GUI_MainWindow::~SDL_GUI_MainWindow"));

}

void
SDL_GUI_MainWindow::init(RPG_Engine_Level* levelState_in,
                         const RPG_Graphics_MapStyle_t& style_in,
                         const bool& debugMode_in)
{
  RPG_TRACE(ACE_TEXT("SDL_GUI_MainWindow::init"));

  // init scroll margins
  initScrollSpots();

  // init map
  initMap(levelState_in,
          style_in,
          debugMode_in);
}

void
SDL_GUI_MainWindow::draw(SDL_Surface* targetSurface_in,
                         const unsigned int& offsetX_in,
                         const unsigned int& offsetY_in)
{
  RPG_TRACE(ACE_TEXT("SDL_GUI_MainWindow::draw"));

  // set target surface
  SDL_Surface* targetSurface = (targetSurface_in ? targetSurface_in : myScreen);

  // sanity check(s)
  ACE_ASSERT(targetSurface);
  ACE_ASSERT(static_cast<int>(offsetX_in) <= targetSurface->w);
  ACE_ASSERT(static_cast<int>(offsetY_in) <= targetSurface->h);

  // step1: draw borders
  drawBorder(targetSurface,
             offsetX_in,
             offsetY_in);

  // step2: draw title
  drawTitle(myTitleFont,
            myTitle,
            myScreen);

  // step3: fill central area
  SDL_Rect clipRect;
  clipRect.x = static_cast<Sint16>(offsetX_in + myBorderLeft);
  clipRect.y = static_cast<Sint16>(offsetY_in + myBorderTop);
  clipRect.w = static_cast<Uint16>(targetSurface->w - offsetX_in - (myBorderLeft + myBorderRight));
  clipRect.h = static_cast<Uint16>(targetSurface->h - offsetY_in - (myBorderTop + myBorderBottom));
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
  for (unsigned long i = (offsetY_in + myBorderTop);
       i < (targetSurface->h - myBorderBottom);
       i += (*iterator).second->h)
    for (unsigned long j = (offsetX_in + myBorderLeft);
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

  // init clipping
  clip(targetSurface,
       offsetX_in,
       offsetY_in);

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

  // restore previous clipping area
  unclip(targetSurface);

//   // whole window needs a refresh...
//   SDL_Rect dirtyRegion;
//   dirtyRegion.x = offsetX_in + myBorderLeft + myOffset.first;
//   dirtyRegion.y = offsetY_in + myBorderTop + myOffset.second;
//   dirtyRegion.w = (targetSurface->w - offsetX_in - (myBorderLeft + myBorderRight) - myOffset.first);
//   dirtyRegion.h = (targetSurface->h - offsetY_in - (myBorderTop + myBorderBottom) - myOffset.second);
//   invalidate(dirtyRegion);

  // remember position of last realization
  myLastAbsolutePosition = std::make_pair(offsetX_in,
                                          offsetY_in);
}

void
SDL_GUI_MainWindow::handleEvent(const SDL_Event& event_in,
                                RPG_Graphics_IWindow* window_in,
                                bool& redraw_out)
{
  RPG_TRACE(ACE_TEXT("SDL_GUI_MainWindow::handleEvent"));

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

          // --> restore background
          SDL_Rect dirtyRegion;
          RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->restoreBG(myScreen,
                                                                       dirtyRegion);
          //             invalidate(dirtyRegion);
          // *NOTE*: updating straight away reduces ugly smears...
          RPG_Graphics_Surface::update(dirtyRegion,
                                       myScreen);

          // *HACK*: prevents MOST "mouse trails" (NW borders)...
          drawBorder(myScreen,
                     0,
                     0);
          drawTitle(myTitleFont,
                    myTitle,
                    myScreen);
          refresh();

          myHaveMouseFocus = true;
        } // end IF
        else
        {
//           ACE_DEBUG((LM_DEBUG,
//                      ACE_TEXT("lost mouse coverage...\n")));

          // *HACK*: prevents MOST "mouse trails" (NW borders)...
          drawBorder(myScreen,
                     0,
                     0);
          drawTitle(myTitleFont,
                    myTitle,
                    myScreen);
          refresh();

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
          dump_path += ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_DEF_SCREENSHOT_PREFIX);
          dump_path += ACE_TEXT_ALWAYS_CHAR("_");
          dump_path += converter.str();
          dump_path += ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_DEF_SCREENSHOT_EXT);
          RPG_Graphics_Surface::savePNG(*SDL_GetVideoSurface(), // image
                                        dump_path,              // file
                                        false);                 // no alpha

          break;
        }
        default:
          break;
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

//       // (re-)draw the cursor
//       SDL_Rect dirtyRegion;
//       RPG_GRAPHICS_CURSOR_SINGLETON::instance()->put(event_in.motion.x,
//                                                      event_in.motion.y,
//                                                      myScreen,
//                                                      dirtyRegion);
//       myDirtyRegions.push_back(dirtyRegion);
//
//       // show changes
//       refresh();

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
          if ((*iterator)->getType() == WINDOW_MAP)
            break;
        } // end FOR
        ACE_ASSERT((*iterator)->getType() == WINDOW_MAP);
        SDL_GUI_LevelWindow* levelWindow = NULL;
        levelWindow = dynamic_cast<SDL_GUI_LevelWindow*>(*iterator);
        if (!levelWindow)
        {
          ACE_DEBUG((LM_ERROR,
                    ACE_TEXT("dynamic downcast failed, aborting\n")));

          break;
        } // end IF

        switch (hotspot->getCursorType())
        {
          case CURSOR_SCROLL_UL:
          {
            levelWindow->setView(-RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
                                 -RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);
            levelWindow->setView(-RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
                                 RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);

            break;
          }
          case CURSOR_SCROLL_U:
          {
            levelWindow->setView(-RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
                                 -RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);

            break;
          }
          case CURSOR_SCROLL_UR:
          {
            levelWindow->setView(-RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
                                 -RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);
            levelWindow->setView(RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
                                 -RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);

            break;
          }
          case CURSOR_SCROLL_L:
          {
            levelWindow->setView(-RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
                                 RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);

            break;
          }
          case CURSOR_SCROLL_R:
          {
            levelWindow->setView(RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
                                 -RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);

            break;
          }
          case CURSOR_SCROLL_DL:
          {
            levelWindow->setView(RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
                                 RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);
            levelWindow->setView(-RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
                                 RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);

            break;
          }
          case CURSOR_SCROLL_D:
          {
            levelWindow->setView(RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
                                 RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);

            break;
          }
          case CURSOR_SCROLL_DR:
          {
            levelWindow->setView(RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
                                 RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);
            levelWindow->setView(RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
                                 -RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);

            break;
          }
          default:
          {
            ACE_DEBUG((LM_DEBUG,
                      ACE_TEXT("invalid/unknown cursor type (was: \"%s\"), aborting\n"),
                      RPG_Graphics_CursorHelper::RPG_Graphics_CursorToString(hotspot->getCursorType()).c_str()));

            return;
          }
        } // end SWITCH

//         // *NOTE*: fiddling with the view invalidates the cursor BG !
//         RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->invalidateBG();

        // need a redraw
        redraw_out = true;

  //       ACE_DEBUG((LM_DEBUG,
  //                  ACE_TEXT("scrolled map (%s)...\n"),
  //                  RPG_Graphics_TypeHelper::RPG_Graphics_TypeToString(hotspot->getHotSpotType()).c_str()));
      } // end IF

      break;
    }
    case SDL_MOUSEBUTTONUP:
    {
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("mouse button [%u,%u] released...\n"),
                 static_cast<unsigned long>(event_in.button.which),
                 static_cast<unsigned long>(event_in.button.button)));

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
    case RPG_GRAPHICS_SDL_HOVEREVENT:
    {
//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("RPG_GRAPHICS_SDL_HOVEREVENT event (%d ms)...\n"),
//                  event_in.user.code));

      // don't hover if we've lost the mouse in the meantime...
      if (!myHaveMouseFocus)
        break;

      // hovering on a hotspot (edge) area triggers a scroll of the viewport
      if (window_in == this)
        break; // not a hotspot

      // sanity check
      ACE_ASSERT(window_in->getType() == WINDOW_HOTSPOT);

      // retrieve hotspot window handle
      RPG_Graphics_HotSpot* hotspot = NULL;
      hotspot = dynamic_cast<RPG_Graphics_HotSpot*>(window_in);
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
        if ((*iterator)->getType() == WINDOW_MAP)
          break;
      } // end FOR
      ACE_ASSERT((*iterator)->getType() == WINDOW_MAP);
      SDL_GUI_LevelWindow* levelWindow = NULL;
      levelWindow = dynamic_cast<SDL_GUI_LevelWindow*>(*iterator);
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

      switch (hotspot->getCursorType())
      {
        case CURSOR_SCROLL_UL:
        {
          levelWindow->setView(-RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
                               -RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);
          levelWindow->setView(-RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
                               RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);

          break;
        }
        case CURSOR_SCROLL_U:
        {
          levelWindow->setView(-RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
                               -RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);

          break;
        }
        case CURSOR_SCROLL_UR:
        {
          levelWindow->setView(-RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
                               -RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);
          levelWindow->setView(RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
                               -RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);

          break;
        }
        case CURSOR_SCROLL_L:
        {
          levelWindow->setView(-RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
                               RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);

          break;
        }
        case CURSOR_SCROLL_R:
        {
          levelWindow->setView(RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
                               -RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);

          break;
        }
        case CURSOR_SCROLL_DL:
        {
          levelWindow->setView(RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
                               RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);
          levelWindow->setView(-RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
                               RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);

          break;
        }
        case CURSOR_SCROLL_D:
        {
          levelWindow->setView(RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
                               RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);

          break;
        }
        case CURSOR_SCROLL_DR:
        {
          levelWindow->setView(RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
                               RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);
          levelWindow->setView(RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
                               -RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);

          break;
        }
        default:
        {
          ACE_DEBUG((LM_DEBUG,
                     ACE_TEXT("invalid/unknown cursor type (was: \"%s\"), aborting\n"),
                     RPG_Graphics_CursorHelper::RPG_Graphics_CursorToString(hotspot->getCursorType()).c_str()));

          return;
        }
      } // end SWITCH

//       // *NOTE*: fiddling with the view invalidates the cursor BG !
//       RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->invalidateBG();

      // need a redraw
      redraw_out = true;

//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("scrolled map (%s)...\n"),
//                  RPG_Graphics_TypeHelper::RPG_Graphics_TypeToString(hotspot->getHotSpotType()).c_str()));

      break;
    }
    case RPG_GRAPHICS_SDL_MOUSEMOVEOUT:
    {
//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("RPG_GRAPHICS_SDL_MOUSEMOVEOUT event...\n")));

//       // --> restore background
//       SDL_Rect dirtyRegion;
//       RPG_GRAPHICS_CURSOR_SINGLETON::instance()->restore(myScreen,
//                                               dirtyRegion);
// //           RPG_Graphics_Surface::update(dirtyRegion,
// //                                        myScreen);

      break;
    }
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("received unknown event (was: %u)...\n"),
                 static_cast<unsigned long>(event_in.type)));

      break;
    }
  } // end SWITCH

  // if necessary, reset last hover time
  if (event_in.type != RPG_GRAPHICS_SDL_HOVEREVENT)
    myLastHoverTime = 0;
}

void
SDL_GUI_MainWindow::notify(const RPG_Graphics_Cursor& cursor_in) const
{
  RPG_TRACE(ACE_TEXT("SDL_GUI_MainWindow::notify"));

  switch (cursor_in)
  {
    case RPG_GRAPHICS_CURSOR_INVALID:
    {
      SDL_Rect dirtyRegion;
      RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->restoreBG(myScreen, dirtyRegion);
      // *NOTE*: updating straight away reduces ugly smears...
      RPG_Graphics_Surface::update(dirtyRegion, myScreen);

      return;
    }
    default:
      RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->set(cursor_in);
  } // end SWITCH
}

void
SDL_GUI_MainWindow::initScrollSpots()
{
  RPG_TRACE(ACE_TEXT("SDL_GUI_MainWindow::initScrollSpots"));

  // upper left
  RPG_Graphics_HotSpot::init(*this,             // parent
                             std::make_pair(RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN,
                                            RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN), // size
                             std::make_pair(0,
                                            0), // offset
                             CURSOR_SCROLL_UL); // (hover) cursor graphic
  // up
  RPG_Graphics_HotSpot::init(*this,             // parent
                             std::make_pair(mySize.first - (2 * RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN),
                                            RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN), // size
                             std::make_pair(RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN,
                                            0), // offset
                             CURSOR_SCROLL_U);  // (hover) cursor graphic
  // upper right
  RPG_Graphics_HotSpot::init(*this,             // parent
                             std::make_pair(RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN,
                                            RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN), // size
                             std::make_pair(mySize.first - RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN,
                                            0), // offset
                             CURSOR_SCROLL_UR); // (hover) cursor graphic
  // left
  RPG_Graphics_HotSpot::init(*this,            // parent
                             std::make_pair(RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN,
                                            mySize.second - (2 * RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN)), // size
                             std::make_pair(0,
                                            RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN), // offset
                             CURSOR_SCROLL_L); // (hover) cursor graphic
  // right
  RPG_Graphics_HotSpot::init(*this,            // parent
                             std::make_pair(RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN,
                                            mySize.second - (2 * RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN)), // size
                             std::make_pair(mySize.first - RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN,
                                            RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN), // offset
                             CURSOR_SCROLL_R); // (hover) cursor graphic
  // lower left
  RPG_Graphics_HotSpot::init(*this,             // parent
                             std::make_pair(RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN,
                                            RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN), // size
                             std::make_pair(0,
                                            mySize.second - RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN), // offset
                             CURSOR_SCROLL_DL); // (hover) cursor graphic
  // down
  RPG_Graphics_HotSpot::init(*this,            // parent
                             std::make_pair(mySize.first - (2 * RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN),
                                            RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN), // size
                             std::make_pair(RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN,
                                            mySize.second - RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN), // offset
                             CURSOR_SCROLL_D); // (hover) cursor graphic
  // lower right
  RPG_Graphics_HotSpot::init(*this,             // parent
                             std::make_pair(RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN,
                                            RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN), // size
                             std::make_pair(mySize.first - RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN,
                                            mySize.second - RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN), // offset
                             CURSOR_SCROLL_DR); // (hover) cursor graphic
}

void
SDL_GUI_MainWindow::initMap(RPG_Engine_Level* levelState_in,
                            const RPG_Graphics_MapStyle_t& style_in,
                            const bool& debug_in)
{
  RPG_TRACE(ACE_TEXT("SDL_GUI_MainWindow::initMap"));

  SDL_GUI_LevelWindow* map_window = NULL;
  try
  {
    map_window = new SDL_GUI_LevelWindow(*this,
                                         levelState_in);
  }
  catch (...)
  {
    ACE_DEBUG((LM_CRITICAL,
               ACE_TEXT("failed to allocate memory(%u): %m, aborting\n"),
               sizeof(SDL_GUI_LevelWindow)));

    return;
  }
  if (!map_window)
  {
    ACE_DEBUG((LM_CRITICAL,
               ACE_TEXT("failed to allocate memory(%u): %m, aborting\n"),
               sizeof(SDL_GUI_LevelWindow)));

    return;
  } // end IF

  // init window
  map_window->init(style_in, debug_in);
  map_window->setScreen(myScreen);
}

void
SDL_GUI_MainWindow::drawBorder(SDL_Surface* targetSurface_in,
                               const unsigned int& offsetX_in,
                               const unsigned int& offsetY_in)
{
  RPG_TRACE(ACE_TEXT("SDL_GUI_MainWindow::drawBorder"));

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
  clipRect.x = static_cast<Sint16>(offsetX_in + myBorderLeft);
  clipRect.y = static_cast<Sint16>(offsetY_in);
  clipRect.w = static_cast<Uint16>(targetSurface->w - (myBorderLeft + myBorderRight));
  clipRect.h = static_cast<Uint16>(myBorderTop);
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

  clipRect.x = static_cast<Sint16>(offsetX_in);
  clipRect.y = static_cast<Sint16>(offsetY_in + myBorderTop);
  clipRect.w = static_cast<Uint16>(myBorderLeft);
  clipRect.h = static_cast<Uint16>(targetSurface->h - (myBorderTop + myBorderBottom));
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

  clipRect.x = static_cast<Sint16>(targetSurface->w - myBorderRight);
  clipRect.y = static_cast<Sint16>(offsetY_in + myBorderTop);
  clipRect.w = static_cast<Uint16>(myBorderRight);
  clipRect.h = static_cast<Uint16>(targetSurface->h - (myBorderTop + myBorderBottom));
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
       i < (static_cast<unsigned int>(targetSurface->h) - myBorderBottom);
       i += (*iterator).second->h)
    RPG_Graphics_Surface::put((targetSurface->w - myBorderRight),
                               i,
                               *(*iterator).second,
                               targetSurface);
  invalidate(clipRect);

  clipRect.x = static_cast<Sint16>(offsetX_in + myBorderLeft);
  clipRect.y = static_cast<Sint16>(targetSurface->h - myBorderBottom);
  clipRect.w = static_cast<Uint16>(targetSurface->w - (myBorderLeft + myBorderRight));
  clipRect.h = static_cast<Uint16>(myBorderBottom);
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
  clipRect.x = static_cast<Sint16>(offsetX_in);
  clipRect.y = static_cast<Sint16>(offsetY_in);
  iterator = myElementGraphics.find(INTERFACEELEMENT_BORDER_TOP_LEFT);
  ACE_ASSERT(iterator != myElementGraphics.end());
  clipRect.w = static_cast<Uint16>((*iterator).second->w);
  clipRect.h = static_cast<Uint16>((*iterator).second->h);
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
  clipRect.y = static_cast<Sint16>(offsetY_in);
  iterator = myElementGraphics.find(INTERFACEELEMENT_BORDER_TOP_RIGHT);
  ACE_ASSERT(iterator != myElementGraphics.end());
  clipRect.x = static_cast<Sint16>(targetSurface->w - (*iterator).second->w);
  clipRect.w = static_cast<Uint16>((*iterator).second->w);
  clipRect.h = static_cast<Uint16>((*iterator).second->h);
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
  clipRect.x = static_cast<Sint16>(offsetX_in);
  iterator = myElementGraphics.find(INTERFACEELEMENT_BORDER_BOTTOM_LEFT);
  ACE_ASSERT(iterator != myElementGraphics.end());
  clipRect.y = static_cast<Sint16>(targetSurface->h - (*iterator).second->h);
  clipRect.w = static_cast<Uint16>((*iterator).second->w);
  clipRect.h = static_cast<Uint16>((*iterator).second->h);
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
  clipRect.x = (targetSurface->w - (*iterator).second->w);
  clipRect.y = (targetSurface->h - (*iterator).second->h);
  clipRect.w = (*iterator).second->w;
  clipRect.h = (*iterator).second->h;
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

void
SDL_GUI_MainWindow::drawTitle(const RPG_Graphics_Font& font_in,
                              const std::string& text_in,
                              SDL_Surface* targetSurface_in)
{
  RPG_TRACE(ACE_TEXT("SDL_GUI_MainWindow::drawTitle"));

  if (text_in.empty())
    return;

  // sanity check
  SDL_Surface* targetSurface = (targetSurface_in ? targetSurface_in
                                                 : myScreen);
  ACE_ASSERT(targetSurface);

  RPG_Graphics_TextSize_t title_size = RPG_Graphics_Common_Tools::textSize(font_in,
                                                                           text_in);

  SDL_Rect clipRect;
  clipRect.x = static_cast<Sint16>(myBorderLeft);
  clipRect.y = static_cast<Sint16>((myBorderTop - title_size.second) / 2);
  clipRect.w = static_cast<Uint16>(title_size.first);
  clipRect.h = static_cast<Uint16>(title_size.second);
  if (!SDL_SetClipRect(targetSurface, &clipRect))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_SetClipRect(): %s, aborting\n"),
               SDL_GetError()));

    return;
  } // end IF

  RPG_Graphics_Surface::putText(font_in,
                                text_in,
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
}
