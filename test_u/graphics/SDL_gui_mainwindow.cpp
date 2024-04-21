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

#include "SDL_gui_mainwindow.h"

#include <sstream>

#include "ace/Log_Msg.h"

#include "common_file_tools.h"

#include "rpg_common_defines.h"
#include "rpg_common_macros.h"

#include "rpg_engine.h"

#include "rpg_graphics_common_tools.h"
#include "rpg_graphics_cursor_manager.h"
#include "rpg_graphics_defines.h"
#include "rpg_graphics_hotspot.h"
#include "rpg_graphics_surface.h"
#include "rpg_graphics_SDL_tools.h"

#include "rpg_client_defines.h"

#include "SDL_gui_defines.h"
#include "SDL_gui_levelwindow_3d.h"
#include "SDL_gui_levelwindow_isometric.h"

SDL_GUI_MainWindow::SDL_GUI_MainWindow (const RPG_Graphics_Size_t& size_in,
                                        const struct RPG_Graphics_GraphicTypeUnion& elementType_in,
                                        const std::string& title_in,
                                        enum RPG_Graphics_Font fontType_in)
 : inherited (size_in,        // size
              elementType_in, // element type
              title_in)       // title
 , myScreenshotIndex (1)
 , myLastHoverTime (0)
 , myHaveMouseFocus (true) // *NOTE*: enforced with SDL_WarpMouse()
 , myTitleFont (fontType_in)
{
  RPG_TRACE (ACE_TEXT ("SDL_GUI_MainWindow::SDL_GUI_MainWindow"));

}

void
SDL_GUI_MainWindow::initialize (state_t* state_in,
                                RPG_Engine* engine_in,
                                enum RPG_Client_GraphicsMode mode_in)
{
  RPG_TRACE (ACE_TEXT ("SDL_GUI_MainWindow::initialize"));

  // init scroll margins
  initScrollSpots ();

  // init map
  initMap (state_in,
           engine_in,
           mode_in);
}

void
SDL_GUI_MainWindow::draw (SDL_Surface* targetSurface_in,
                          unsigned int offsetX_in,
                          unsigned int offsetY_in)
{
  RPG_TRACE (ACE_TEXT ("SDL_GUI_MainWindow::draw"));

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
  ACE_ASSERT (static_cast<int> (offsetX_in) <= target_surface->w);
  ACE_ASSERT (static_cast<int> (offsetY_in) <= target_surface->h);

  // *NOTE*: drawBorder() does its own locking
  // step1: draw borders
  drawBorder (target_surface,
              offsetX_in,
              offsetY_in);

  lock (true); // block ?

  // step2: draw title
  drawTitle (myTitleFont,
             title_,
             target_surface);

  // step3: fill central area
  struct SDL_Rect prev_clip_rect, clip_rect, dirty_region;
  SDL_GetClipRect (target_surface, &prev_clip_rect);
  clip_rect.x = static_cast<Sint16> (offsetX_in + borderLeft_);
  clip_rect.y = static_cast<Sint16> (offsetY_in + borderTop_);
  clip_rect.w = static_cast<Uint16> (target_surface->w               -
                                     offsetX_in                      -
                                     (borderLeft_ + borderRight_));
  clip_rect.h = static_cast<Uint16> (target_surface->h               -
                                     offsetY_in                      -
                                     (borderTop_ + borderBottom_));
  if (!SDL_SetClipRect (target_surface, &clip_rect))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_SetClipRect(): %s, aborting\n"),
                ACE_TEXT (SDL_GetError ())));

    // clean up
    unlock (false);

    return;
  } // end IF
  RPG_Graphics_InterfaceElementsConstIterator_t iterator;
  iterator = myElementGraphics.find (INTERFACEELEMENT_BORDER_CENTER);
  ACE_ASSERT(iterator != myElementGraphics.end ());
  for (unsigned int i = (offsetY_in + borderTop_);
       i < (target_surface->h - borderBottom_);
       i += (*iterator).second->h)
    for (unsigned int j = (offsetX_in + borderLeft_);
         j < (target_surface->w - borderRight_);
         j += (*iterator).second->w)
    {
      RPG_Graphics_Surface::put (std::make_pair(j, i),
                                 *(*iterator).second,
                                 target_surface,
                                 dirty_region);
      inherited::invalidate (clip_rect);
    } // end FOR
  if (!SDL_SetClipRect (target_surface, &prev_clip_rect))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_SetClipRect(): %s, aborting\n"),
                ACE_TEXT (SDL_GetError ())));

    // clean up
    unlock (false);

    return;
  } // end IF
  unlock (false);

  // initialize clipping
  clip (target_surface,
        offsetX_in,
        offsetY_in);

  // step4: realize hotspots (and any other children)
  for (RPG_Graphics_WindowsIterator_t iterator = children_.begin ();
       iterator != children_.end ();
       iterator++)
  {
    try {
      (*iterator)->draw(target_surface,
                        offsetX_in,
                        offsetY_in);
    } catch (...) {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("caught exception in RPG_Graphics_IWindow::draw(), continuing\n")));
    }
  } // end FOR

  // restore previous clipping area
  unclip (target_surface);

  // remember position of last realization
  lastAbsolutePosition_ = std::make_pair (offsetX_in,
                                          offsetY_in);
}

void
SDL_GUI_MainWindow::handleEvent (const SDL_Event& event_in,
                                 RPG_Graphics_IWindowBase* window_in,
                                 struct SDL_Rect& dirtyRegion_out)
{
  RPG_TRACE (ACE_TEXT ("SDL_GUI_MainWindow::handleEvent"));

  // initialize return value(s)
  ACE_OS::memset (&dirtyRegion_out, 0, sizeof (struct SDL_Rect));

//   ACE_DEBUG ((LM_DEBUG,
//               ACE_TEXT ("SDL_GUI_MainWindow::handleEvent\n")));

  switch (event_in.type)
  {
    // *** visibility ***
#if defined (SDL_USE)
    case SDL_ACTIVEEVENT:
#elif defined (SDL2_USE)
    case SDL_WINDOWEVENT_SHOWN:
#endif // SDL_USE || SDL2_USE
    {
#if defined (SDL_USE)
      if (event_in.active.state & SDL_APPMOUSEFOCUS)
      {
        if (event_in.active.gain & SDL_APPMOUSEFOCUS)
        {
//           ACE_DEBUG((LM_DEBUG,
//                      ACE_TEXT("gained mouse coverage...\n")));

//          // *HACK*: prevents MOST "mouse trails" (NW borders)...
//          drawBorder(inherited::screen_,
//                     0,
//                     0);
//          drawTitle(myTitleFont,
//                    myTitle,
//                    inherited::screen_);
//          refresh();

          myHaveMouseFocus = true;
        } // end IF
        else
        {
//           ACE_DEBUG((LM_DEBUG,
//                      ACE_TEXT("lost mouse coverage...\n")));

//          // *HACK*: prevents MOST "mouse trails" (NW borders)...
//          drawBorder(inherited::screen_,
//                     0,
//                     0);
//          drawTitle(myTitleFont,
//                    myTitle,
//                    inherited::screen_);
//          refresh();

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
#endif // SDL_USE || SDL2_USE

      break;
    }
    // *** keyboard ***
    case SDL_KEYDOWN:
    {
//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("%s key\n%s\n"),
//                  ((event_in.type == SDL_KEYDOWN) ? ACE_TEXT("pressed") : ACE_TEXT("released")),
//                  ACE_TEXT(RPG_Graphics_SDL_Tools::keyToString(event_in.key.keysym).c_str())));

      switch (event_in.key.keysym.sym)
      {
        case SDLK_s:
        {
          // sanity check(s)
          ACE_ASSERT (inherited::screen_);
#if defined (SDL_USE)
          SDL_Surface* surface_p = inherited::screen_;
#elif defined (SDL2_USE)
          SDL_Surface* surface_p = SDL_GetWindowSurface (inherited::screen_);
#endif // SDL_USE || SDL2_USE
          ACE_ASSERT (surface_p);

          std::ostringstream converter;
          converter << myScreenshotIndex++;
          std::string dump_path = Common_File_Tools::getTempDirectory ();
          dump_path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
          dump_path += ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_SCREENSHOT_DEF_PREFIX);
          dump_path += ACE_TEXT_ALWAYS_CHAR ("_");
          dump_path += converter.str ();
          dump_path += ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_SCREENSHOT_DEF_EXT);
          RPG_Graphics_Surface::savePNG (*surface_p, // image
                                         dump_path,  // file
                                         false);     // no alpha

          break;
        }
        default:
          break;
      } // end SWITCH

      break;
    }
    case SDL_KEYUP:
    {
      //ACE_DEBUG ((LM_DEBUG,
      //            ACE_TEXT ("key released...\n")));

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
//                  static_cast<unsigned int>(event_in.button.which),
//                  static_cast<unsigned int>(event_in.button.button)));

      // (left-)clicking on a hotspot (edge) area triggers a scroll of the viewport
      if ((window_in->getType () == WINDOW_HOTSPOT) &&
          (event_in.button.button == 1))
      {
        // retrieve hotspot window handle
        RPG_Graphics_HotSpot* hotspot = NULL;
        try {
          hotspot = dynamic_cast<RPG_Graphics_HotSpot*>(window_in);
        } catch (...) {
          hotspot = NULL;
        }
        if (!hotspot)
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to dynamic_cast<RPG_Graphics_HotSpot*>(%@), returning\n"),
                      window_in));
          break;
        } // end IF

        // retrieve map window handle
        RPG_Graphics_WindowsConstIterator_t iterator = children_.begin ();
        for (;
             iterator != children_.end ();
             iterator++)
        {
          if ((*iterator)->getType () == WINDOW_MAP)
            break;
        } // end FOR
        ACE_ASSERT ((*iterator)->getType () == WINDOW_MAP);
        RPG_Client_IWindowLevel* level_window = NULL;
        try {
          level_window = dynamic_cast<RPG_Client_IWindowLevel*>(*iterator);
        } catch (...) {
          level_window = NULL;
        }
        if (!level_window)
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to dynamic_cast<RPG_Client_IWindowLevel*>(%@), returning\n"),
                      *iterator));
          break;
        } // end IF

        switch (hotspot->getCursorType ())
        {
          case CURSOR_SCROLL_UL:
          {
            level_window->setView (-RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
                                   -RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);
            level_window->setView (-RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
                                   RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);

            break;
          }
          case CURSOR_SCROLL_U:
          {
            level_window->setView (-RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
                                   -RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);

            break;
          }
          case CURSOR_SCROLL_UR:
          {
            level_window->setView (-RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
                                   -RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);
            level_window->setView (RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
                                   -RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);

            break;
          }
          case CURSOR_SCROLL_L:
          {
            level_window->setView (-RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
                                   RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);

            break;
          }
          case CURSOR_SCROLL_R:
          {
            level_window->setView (RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
                                   -RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);

            break;
          }
          case CURSOR_SCROLL_DL:
          {
            level_window->setView (RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
                                   RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);
            level_window->setView (-RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
                                   RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);

            break;
          }
          case CURSOR_SCROLL_D:
          {
            level_window->setView (RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
                                   RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);

            break;
          }
          case CURSOR_SCROLL_DR:
          {
            level_window->setView (RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
                                   RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);
            level_window->setView (RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
                                   -RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);

            break;
          }
          default:
          {
            ACE_DEBUG ((LM_DEBUG,
                        ACE_TEXT ("invalid/unknown cursor type (was: \"%s\"), returning\n"),
                        ACE_TEXT (RPG_Graphics_CursorHelper::RPG_Graphics_CursorToString (hotspot->getCursorType ()).c_str ())));
            return;
          }
        } // end SWITCH

        // redraw
        try {
          level_window->draw ();
          level_window->getArea (dirtyRegion_out);
        } catch (...) {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("caught exception in RPG_Graphics_IWindowBase::draw/getArea, continuing\n")));
        }
        RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance ()->reset (true,   // update cursor bg ? : clear
                                                                   true,   // locked access ?
                                                                   false); // debug ?

  //       ACE_DEBUG((LM_DEBUG,
  //                  ACE_TEXT("scrolled map (%s)...\n"),
  //                  ACE_TEXT(RPG_Graphics_TypeHelper::RPG_Graphics_TypeToString(hotspot->getHotSpotType()).c_str())));
      } // end IF

      break;
    }
    case SDL_MOUSEBUTTONUP:
    {
      //ACE_DEBUG ((LM_DEBUG,
      //            ACE_TEXT ("mouse button [%u,%u] released...\n"),
      //            static_cast<unsigned int> (event_in.button.which),
      //            static_cast<unsigned int> (event_in.button.button)));

      break;
    }
    // *** joystick ***
    case SDL_JOYAXISMOTION:
    case SDL_JOYBALLMOTION:
    case SDL_JOYHATMOTION:
    case SDL_JOYBUTTONDOWN:
    case SDL_JOYBUTTONUP:
    {
      //ACE_DEBUG ((LM_DEBUG,
      //            ACE_TEXT ("joystick activity...\n")));

      break;
    }
    case SDL_QUIT:
    {
      //ACE_DEBUG ((LM_DEBUG,
      //            ACE_TEXT ("SDL_QUIT event...\n")));

      break;
    }
    case SDL_SYSWMEVENT:
    {
      //ACE_DEBUG ((LM_DEBUG,
      //            ACE_TEXT ("SDL_SYSWMEVENT event...\n")));

      break;
    }
#if defined (SDL_USE)
    case SDL_VIDEORESIZE:
    {
      //ACE_DEBUG ((LM_DEBUG,
      //            ACE_TEXT ("SDL_VIDEORESIZE event...\n")));
      break;
    }
    case SDL_VIDEOEXPOSE:
    {
      //ACE_DEBUG ((LM_DEBUG,
      //            ACE_TEXT ("SDL_VIDEOEXPOSE event...\n")));
      break;
    }
#elif defined (SDL2_USE)
    case SDL_WINDOWEVENT_RESIZED:
    {
      //ACE_DEBUG ((LM_DEBUG,
      //            ACE_TEXT ("SDL_WINDOWEVENT_RESIZED event...\n")));
      break;
    }
    case SDL_WINDOWEVENT_EXPOSED:
    {
      //ACE_DEBUG ((LM_DEBUG,
      //            ACE_TEXT ("SDL_WINDOWEVENT_EXPOSED event...\n")));
      break;
    }
#endif // SDL_USE || SDL2_USE
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
      ACE_ASSERT (window_in->getType () == WINDOW_HOTSPOT);

      // retrieve hotspot window handle
      RPG_Graphics_HotSpot* hotspot = NULL;
      try {
        hotspot = dynamic_cast<RPG_Graphics_HotSpot*>(window_in);
      } catch (...) {
        hotspot = NULL;
      }
      if (!hotspot)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to dynamic_cast<RPG_Graphics_HotSpot*>(%@), returning\n"),
                    window_in));
        break;
      } // end IF

      // retrieve map window handle
      RPG_Graphics_WindowsConstIterator_t iterator = children_.begin ();
      for (;
           iterator != children_.end ();
           iterator++)
      {
        if ((*iterator)->getType () == WINDOW_MAP)
          break;
      } // end FOR
      ACE_ASSERT ((iterator != children_.end ()) && (*iterator)->getType () == WINDOW_MAP);
      RPG_Client_IWindowLevel* level_window = NULL;
      try {
        level_window = dynamic_cast<RPG_Client_IWindowLevel*>(*iterator);
      } catch (...) {
        level_window = NULL;
      }
      if (!level_window)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to dynamic_cast<RPG_Client_IWindowLevel*>(%@), returning\n"),
                    *iterator));

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
          level_window->setView (-RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
                                 -RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);
          level_window->setView (-RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
                                 RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);

          break;
        }
        case CURSOR_SCROLL_U:
        {
          level_window->setView (-RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
                                 -RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);

          break;
        }
        case CURSOR_SCROLL_UR:
        {
          level_window->setView (-RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
                                 -RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);
          level_window->setView (RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
                                 -RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);

          break;
        }
        case CURSOR_SCROLL_L:
        {
          level_window->setView (-RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
                                 RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);

          break;
        }
        case CURSOR_SCROLL_R:
        {
          level_window->setView (RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
                                 -RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);

          break;
        }
        case CURSOR_SCROLL_DL:
        {
          level_window->setView (RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
                                 RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);
          level_window->setView (-RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
                                 RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);

          break;
        }
        case CURSOR_SCROLL_D:
        {
          level_window->setView (RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
                                 RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);

          break;
        }
        case CURSOR_SCROLL_DR:
        {
          level_window->setView (RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
                                 RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);
          level_window->setView (RPG_GRAPHICS_WINDOW_SCROLL_OFFSET,
                                 -RPG_GRAPHICS_WINDOW_SCROLL_OFFSET);

          break;
        }
        default:
        {
          ACE_DEBUG ((LM_DEBUG,
                      ACE_TEXT ("invalid/unknown cursor type (was: \"%s\"), returning\n"),
                      ACE_TEXT (RPG_Graphics_CursorHelper::RPG_Graphics_CursorToString (hotspot->getCursorType ()).c_str ())));

          return;
        }
      } // end SWITCH

      // redraw
      try {
        level_window->draw ();
        level_window->getArea (dirtyRegion_out);
      } catch (...) {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("caught exception in RPG_Graphics_IWindowBase::draw/getArea, continuing\n")));
      }
      RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance ()->reset (true,   // update cursor bg : clear ?
                                                                 true,   // locked access ?
                                                                 false); // debug ?

//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("scrolled map (%s)...\n"),
//                  ACE_TEXT(RPG_Graphics_TypeHelper::RPG_Graphics_TypeToString(hotspot->getHotSpotType()).c_str())));

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
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("received unknown event (was: %u)...\n"),
                  static_cast<unsigned int> (event_in.type)));

      break;
    }
  } // end SWITCH

  // if necessary, reset last hover time
  if (event_in.type != RPG_GRAPHICS_SDL_HOVEREVENT)
    myLastHoverTime = 0;

  // pass events to any children ?
  if (window_in == this)
  {
    struct SDL_Rect dirty_region;
    ACE_OS::memset (&dirty_region, 0, sizeof (struct SDL_Rect));
    inherited::handleEvent (event_in,
                            window_in,
                            dirty_region);
    dirtyRegion_out = RPG_Graphics_SDL_Tools::boundingBox (dirty_region,
                                                           dirtyRegion_out);
  } // end IF
}

void
SDL_GUI_MainWindow::notify (enum RPG_Graphics_Cursor cursor_in) const
{
  RPG_TRACE(ACE_TEXT("SDL_GUI_MainWindow::notify"));

  switch (cursor_in)
  {
    case RPG_GRAPHICS_CURSOR_INVALID:
    {
      RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance ()->invalidateBG ();

      break;
    }
    default:
    {
      struct SDL_Rect dirty_region;
      ACE_OS::memset (&dirty_region, 0, sizeof (struct SDL_Rect));
      RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance ()->setCursor (cursor_in,
                                                                     dirty_region,
                                                                     true); // locked access ?
      if ((dirty_region.w != 0) || (dirty_region.h != 0))
        const_cast<SDL_GUI_MainWindow*> (this)->invalidate (dirty_region);

      break;
    }
  } // end SWITCH
}

bool
SDL_GUI_MainWindow::lock (bool block_in)
{
  RPG_TRACE(ACE_TEXT("SDL_GUI_MainWindow::lock"));

  ACE_UNUSED_ARG (block_in);

  if (myScreenLock.acquire () == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Thread_Mutex::acquire(): \"%m\", continuing\n")));

  return true;
}

int
SDL_GUI_MainWindow::unlock (bool unlock_in)
{
  RPG_TRACE(ACE_TEXT("SDL_GUI_MainWindow::unlock"));

  ACE_UNUSED_ARG (unlock_in);

  if (myScreenLock.release () == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Thread_Mutex::release(): \"%m\", continuing\n")));

  return 0;
}

void
SDL_GUI_MainWindow::initScrollSpots ()
{
  RPG_TRACE (ACE_TEXT ("SDL_GUI_MainWindow::initScrollSpots"));

  // upper left
  RPG_Graphics_HotSpot::initialize (*this,             // parent
                                    std::make_pair (RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN,
                                                    RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN), // size
                                    std::make_pair (0,
                                                    0), // offset
                                    CURSOR_SCROLL_UL); // (hover) cursor graphic
  // up
  RPG_Graphics_HotSpot::initialize (*this,             // parent
                                    std::make_pair ((inherited::clipRectangle_.w -
                                                     (2 * RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN)),
                                                    RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN), // size
                                    std::make_pair (RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN,
                                                    0), // offset
                                    CURSOR_SCROLL_U);  // (hover) cursor graphic
  // upper right
  RPG_Graphics_HotSpot::initialize (*this,             // parent
                                    std::make_pair (RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN,
                                                    RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN), // size
                                    std::make_pair ((clipRectangle_.w -
                                                     RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN),
                                                    0), // offset
                                    CURSOR_SCROLL_UR); // (hover) cursor graphic
  // left
  RPG_Graphics_HotSpot::initialize (*this,            // parent
                                    std::make_pair (RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN,
                                                    (clipRectangle_.h -
                                                     (2 * RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN))), // size
                                    std::make_pair (0,
                                                    RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN), // offset
                                    CURSOR_SCROLL_L); // (hover) cursor graphic
  // right
  RPG_Graphics_HotSpot::initialize (*this,            // parent
                                    std::make_pair (RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN,
                                                    (clipRectangle_.h -
                                                     (2 * RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN))), // size
                                    std::make_pair ((clipRectangle_.w -
                                                     RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN),
                                                    RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN), // offset
                                    CURSOR_SCROLL_R); // (hover) cursor graphic
  // lower left
  RPG_Graphics_HotSpot::initialize (*this,             // parent
                                    std::make_pair (RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN,
                                                    RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN), // size
                                    std::make_pair (0,
                                                    (clipRectangle_.h -
                                                     RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN)), // offset
                                    CURSOR_SCROLL_DL); // (hover) cursor graphic
  // down
  RPG_Graphics_HotSpot::initialize (*this,            // parent
                                    std::make_pair (clipRectangle_.w -
                                                    (2 * RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN),
                                                    RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN), // size
                                    std::make_pair (RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN,
                                                    (clipRectangle_.h -
                                                     RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN)), // offset
                                    CURSOR_SCROLL_D); // (hover) cursor graphic
  // lower right
  RPG_Graphics_HotSpot::initialize (*this,             // parent
                                    std::make_pair (RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN,
                                                    RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN), // size
                                    std::make_pair ((clipRectangle_.w -
                                                     RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN),
                                                    (clipRectangle_.h -
                                                     RPG_GRAPHICS_WINDOW_HOTSPOT_SCROLL_MARGIN)), // offset
                                    CURSOR_SCROLL_DR); // (hover) cursor graphic
}

bool
SDL_GUI_MainWindow::initMap (state_t* state_in,
                             RPG_Engine* engine_in,
                             enum RPG_Client_GraphicsMode mode_in)
{
  RPG_TRACE (ACE_TEXT ("SDL_GUI_MainWindow::initMap"));

  RPG_Graphics_IWindowBase* window_base = NULL;
  switch (mode_in)
  {
    case GRAPHICSMODE_2D_ISOMETRIC:
    {
      ACE_NEW_NORETURN (window_base,
                        SDL_GUI_LevelWindow_Isometric (*this,
                                                       engine_in));
      ACE_ASSERT (window_base);
      // initialize window
      SDL_GUI_LevelWindow_Isometric* map_window =
          dynamic_cast<SDL_GUI_LevelWindow_Isometric*> (window_base);
      ACE_ASSERT (map_window);
      map_window->initialize (state_in,
                              this);

      break;
    }
    case GRAPHICSMODE_2D_OPENGL:
    {
      ACE_NEW_NORETURN (window_base,
                        SDL_GUI_LevelWindow_3D (*this,
                                                engine_in));
      ACE_ASSERT (window_base);
      // initialize window
      SDL_GUI_LevelWindow_3D* map_window =
          dynamic_cast<SDL_GUI_LevelWindow_3D*> (window_base);
      ACE_ASSERT (map_window);
      map_window->initialize (state_in,
                              this);

      break;
    }
    case GRAPHICSMODE_3D: // *TODO*
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid graphics mode (was: \"%s\"), aborting\n"),
                  ACE_TEXT (RPG_Client_GraphicsModeHelper::RPG_Client_GraphicsModeToString (mode_in).c_str ())));
     return false;
    }
  } // end SWITCH
  if (!window_base)
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory: %m, aborting\n")));
    return false;
  } // end IF

  // initialize window
  window_base->setScreen (inherited::screen_);

  return true;
}

void
SDL_GUI_MainWindow::drawBorder (SDL_Surface* targetSurface_in,
                                unsigned int offsetX_in,
                                unsigned int offsetY_in)
{
  RPG_TRACE (ACE_TEXT ("SDL_GUI_MainWindow::drawBorder"));

  // sanity check(s)
  ACE_ASSERT (inherited::screen_);
#if defined (SDL_USE)
  SDL_Surface* surface_p = inherited::screen_;
#elif defined (SDL2_USE)
  SDL_Surface* surface_p = SDL_GetWindowSurface (inherited::screen_);
#endif // SDL_USE || SDL2_USE
  ACE_ASSERT (surface_p);
  // sanity check(s)
  SDL_Surface* target_surface =
    (targetSurface_in ? targetSurface_in : surface_p);
  ACE_ASSERT (target_surface);
  ACE_ASSERT (static_cast<int> (offsetX_in) <= target_surface->w);
  ACE_ASSERT (static_cast<int> (offsetY_in) <= target_surface->h);

  lock (true);

  RPG_Graphics_InterfaceElementsConstIterator_t iterator;
  struct SDL_Rect dirty_region, prev_clip_rect, clip_rect;
  unsigned int i = 0;

  // step0: retain previous clip rect
  SDL_GetClipRect (target_surface, &prev_clip_rect);

  // step1: draw border elements
  clip_rect.x = static_cast<Sint16> (offsetX_in + borderLeft_);
  clip_rect.y = static_cast<Sint16> (offsetY_in);
  clip_rect.w = static_cast<Uint16> (clipRectangle_.w -
                                     (borderLeft_ + borderRight_));
  clip_rect.h = static_cast<Uint16> (borderTop_);
  if (!SDL_SetClipRect (target_surface, &clip_rect))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_SetClipRect(): \"%s\", returning\n"),
                ACE_TEXT (SDL_GetError ())));
    return;
  } // end IF
  iterator = myElementGraphics.find (INTERFACEELEMENT_BORDER_TOP);
  ACE_ASSERT (iterator != myElementGraphics.end ());
  for (i = offsetX_in + borderLeft_;
       i < (static_cast<unsigned int> (clipRectangle_.w) - borderRight_);
       i += (*iterator).second->w)
  {
    RPG_Graphics_Surface::put (std::make_pair (i, offsetY_in),
                               *(*iterator).second,
                               target_surface,
                               dirty_region);
    inherited::invalidate (dirty_region);
  } // end FOR
  if (!SDL_SetClipRect (target_surface, &prev_clip_rect))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_SetClipRect(): \"%s\", returning\n"),
                ACE_TEXT (SDL_GetError ())));
    return;
  } // end IF

  iterator = myElementGraphics.find (INTERFACEELEMENT_BORDER_LEFT);
  ACE_ASSERT (iterator != myElementGraphics.end ());
  for (i = (offsetY_in + borderTop_);
       i < (static_cast<unsigned int>(clipRectangle_.h) - borderBottom_);
       i += (*iterator).second->h)
  {
    RPG_Graphics_Surface::put (std::make_pair (offsetX_in, i),
                               *(*iterator).second,
                               target_surface,
                               dirty_region);
    inherited::invalidate (dirty_region);
  } // end FOR

  iterator = myElementGraphics.find (INTERFACEELEMENT_BORDER_RIGHT);
  ACE_ASSERT (iterator != myElementGraphics.end ());
  for (i = (offsetY_in + borderTop_);
       i < (static_cast<unsigned int> (clipRectangle_.h) - borderBottom_);
       i += (*iterator).second->h)
  {
    RPG_Graphics_Surface::put (std::make_pair ((clipRectangle_.w - borderRight_), i),
                               *(*iterator).second,
                               target_surface,
                               dirty_region);
    inherited::invalidate (dirty_region);
  } // end FOR

  clip_rect.x = static_cast<Sint16> (offsetX_in + borderLeft_);
  clip_rect.y = static_cast<Sint16> (offsetY_in + clipRectangle_.h - borderBottom_);
  clip_rect.w = static_cast<Uint16> (clipRectangle_.w -
                                     (borderLeft_ + borderRight_));
  clip_rect.h = static_cast<Uint16> (borderBottom_);
  if (!SDL_SetClipRect (target_surface, &clip_rect))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_SetClipRect(): \"%s\", returning\n"),
                ACE_TEXT (SDL_GetError ())));
    return;
  } // end IF
  iterator = myElementGraphics.find (INTERFACEELEMENT_BORDER_BOTTOM);
  ACE_ASSERT (iterator != myElementGraphics.end ());
  for (i = (offsetX_in + borderLeft_);
       i < (clipRectangle_.w - borderRight_);
       i += (*iterator).second->w)
  {
    RPG_Graphics_Surface::put (std::make_pair (i, (offsetY_in + (clipRectangle_.h - borderBottom_))),
                               *(*iterator).second,
                               target_surface,
                               dirty_region);
    inherited::invalidate (dirty_region);
  } // end FOR
  if (!SDL_SetClipRect (target_surface, &prev_clip_rect))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_SetClipRect(): \"%s\", returning\n"),
                ACE_TEXT (SDL_GetError ())));
    return;
  } // end IF

  // step2: draw corners
  // NW
  iterator = myElementGraphics.find (INTERFACEELEMENT_BORDER_TOP_LEFT);
  ACE_ASSERT (iterator != myElementGraphics.end ());
  RPG_Graphics_Surface::put (std::make_pair (offsetX_in, offsetY_in),
                             *(*iterator).second,
                             target_surface,
                             dirty_region);
  inherited::invalidate (dirty_region);

  // NE
  iterator = myElementGraphics.find (INTERFACEELEMENT_BORDER_TOP_RIGHT);
  ACE_ASSERT (iterator != myElementGraphics.end ());
  RPG_Graphics_Surface::put (std::make_pair ((clipRectangle_.w - (*iterator).second->w), offsetY_in),
                             *(*iterator).second,
                             target_surface,
                             dirty_region);
  inherited::invalidate (dirty_region);

  // SW
  iterator = myElementGraphics.find (INTERFACEELEMENT_BORDER_BOTTOM_LEFT);
  ACE_ASSERT (iterator != myElementGraphics.end ());
  RPG_Graphics_Surface::put (std::make_pair (offsetX_in, (clipRectangle_.h - (*iterator).second->h)),
                             *(*iterator).second,
                             target_surface,
                             dirty_region);
  inherited::invalidate (dirty_region);

  // SE
  iterator = myElementGraphics.find (INTERFACEELEMENT_BORDER_BOTTOM_RIGHT);
  ACE_ASSERT (iterator != myElementGraphics.end ());
  RPG_Graphics_Surface::put (std::make_pair ((clipRectangle_.w - (*iterator).second->w), (clipRectangle_.h - (*iterator).second->h)),
                             *(*iterator).second,
                             target_surface,
                             dirty_region);
  inherited::invalidate (dirty_region);

  unlock (false);
}

void
SDL_GUI_MainWindow::drawTitle (enum RPG_Graphics_Font font_in,
                               const std::string& text_in,
                               SDL_Surface* targetSurface_in)
{
  RPG_TRACE (ACE_TEXT ("SDL_GUI_MainWindow::drawTitle"));

  // sanity check(s)
  if (text_in.empty ())
    return;
  ACE_ASSERT (inherited::screen_);
#if defined (SDL_USE)
  SDL_Surface* surface_p = inherited::screen_;
#elif defined (SDL2_USE)
  SDL_Surface* surface_p = SDL_GetWindowSurface (inherited::screen_);
#endif // SDL_USE || SDL2_USE
  ACE_ASSERT (surface_p);
  // sanity check(s)
  SDL_Surface* target_surface = (targetSurface_in ? targetSurface_in
                                                  : surface_p);
  ACE_ASSERT (target_surface);

  RPG_Graphics_TextSize_t title_size =
      RPG_Graphics_Common_Tools::textSize (font_in,
                                           text_in);

  struct SDL_Rect clip_rect, dirty_region;
  clip_rect.x = static_cast<Sint16> (borderLeft_);
  clip_rect.y = static_cast<Sint16> ((borderTop_ - title_size.second) / 2);
  clip_rect.w = static_cast<Uint16> (title_size.first);
  clip_rect.h = static_cast<Uint16> (title_size.second);
  if (!SDL_SetClipRect (target_surface, &clip_rect))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_SetClipRect(): \"%s\", returning\n"),
                ACE_TEXT (SDL_GetError ())));
    return;
  } // end IF

  RPG_Graphics_Surface::putText (font_in,
                                 text_in,
                                 RPG_Graphics_SDL_Tools::colorToSDLColor (RPG_Graphics_SDL_Tools::getColor (RPG_GRAPHICS_FONT_DEF_COLOR,
                                                                                                            *target_surface->format,
                                                                                                            1.0F),
                                                                          *target_surface->format),
                                 true, // add shade
                                 RPG_Graphics_SDL_Tools::colorToSDLColor (RPG_Graphics_SDL_Tools::getColor (RPG_GRAPHICS_FONT_DEF_SHADECOLOR,
                                                                                                            *target_surface->format,
                                                                                                            1.0F),
                                                                          *target_surface->format),
                                 std::make_pair (borderLeft_,                             // top left
                                                 ((borderTop_ - title_size.second) / 2)), // center of top border
                                 target_surface,
                                 dirty_region);
  inherited::invalidate (dirty_region);
  if (!SDL_SetClipRect (target_surface, NULL))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_SetClipRect(): \"%s\", returning\n"),
                ACE_TEXT (SDL_GetError ())));
    return;
  } // end IF
}
