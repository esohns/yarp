﻿/*
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

#include "rpg_client_window_message.h"

#include "rpg_client_defines.h"

#include "rpg_graphics_defines.h"
#include "rpg_graphics_surface.h"
#include "rpg_graphics_common_tools.h"
#include "rpg_graphics_SDL_tools.h"

#include "rpg_common_macros.h"

RPG_Client_Window_Message::RPG_Client_Window_Message (const RPG_Graphics_SDLWindowBase& parent_in)
 : inherited (WINDOW_MESSAGE,            // type
              parent_in,                 // parent
              std::make_pair (0, 0),     // offset
              ACE_TEXT_ALWAYS_CHAR ("")) // title
 , BG_ (NULL)
 , client_ (NULL)
 , font_ (RPG_CLIENT_MESSAGE_FONT)
 , lock_ ()
 , messages_ ()
 , numLines_ (RPG_CLIENT_MESSAGE_DEF_NUM_LINES)
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Window_Message::RPG_Client_Window_Message"));

}

RPG_Client_Window_Message::~RPG_Client_Window_Message ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Window_Message::~RPG_Client_Window_Message"));

  if (BG_)
#if defined (SDL_USE) || defined (SDL2_USE)
    SDL_FreeSurface (BG_);
#elif defined (SDL3_USE)
    SDL_DestroySurface (BG_);
#endif // SDL_USE || SDL2_USE || SDL3_USE
}

void
RPG_Client_Window_Message::handleEvent (const SDL_Event& event_in,
                                        RPG_Graphics_IWindowBase* window_in,
                                        struct SDL_Rect& dirtyRegion_out)
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Window_Message::handleEvent"));

  // initialize return value(s)
  ACE_OS::memset (&dirtyRegion_out, 0, sizeof (dirtyRegion_out));

  //   ACE_DEBUG((LM_DEBUG,
  //              ACE_TEXT("RPG_Client_Window_Message::handleEvent(%s)\n"),
  //              RPG_Graphics_TypeHelper::RPG_Graphics_TypeToString(myType).c_str()));

  switch (event_in.type)
  {
    // *** mouse ***
    case RPG_GRAPHICS_SDL_MOUSEMOVEOUT:
      break;
    default:
    {
      // delegate these to the parent...
      inherited::getParent ()->handleEvent (event_in,
                                            window_in,
                                            dirtyRegion_out);

      break;
    }
    //     default:
    //     {
      //       ACE_DEBUG((LM_ERROR,
      //                  ACE_TEXT("received unknown event (was: %u)...\n"),
      //                  static_cast<unsigned int>(event_in.type)));
      //
      //       break;
      //     }
  } // end SWITCH
}

void
RPG_Client_Window_Message::draw (SDL_Surface* targetSurface_in,
                                 unsigned int offsetX_in,
                                 unsigned int offsetY_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Window_Message::draw"));

  // sanity check(s)
  ACE_ASSERT (inherited::parent_);
  ACE_ASSERT (inherited::screen_);
#if defined (SDL_USE)
  SDL_Surface* surface_p = inherited::screen_;
#elif defined (SDL2_USE) || defined (SDL3_USE)
  SDL_Surface* surface_p = SDL_GetWindowSurface (inherited::screen_);
#endif // SDL_USE || SDL2_USE || SDL3_USE
  ACE_ASSERT (surface_p);
  SDL_Surface* target_surface = (targetSurface_in ? targetSurface_in
                                                  : surface_p);
  ACE_ASSERT (target_surface);
  ACE_UNUSED_ARG (offsetX_in);
  ACE_UNUSED_ARG (offsetY_in);

  struct SDL_Rect dirty_region;
  ACE_OS::memset (&dirty_region, 0, sizeof (struct SDL_Rect));

  // step0: restore background
  if (inherited::screenLock_)
    inherited::screenLock_->lock ();
  //if (inherited::BG_HasBeenSaved)
  //  inherited::restoreBG(dirty_region);

  // step1: compute required size / offset
  ACE_OS::memset (&(inherited::clipRectangle_), 0, sizeof (struct SDL_Rect));
  { ACE_GUARD (ACE_Thread_Mutex, aGuard, lock_);
    RPG_Graphics_TextSize_t text_size = std::make_pair (0, 0);
    unsigned int index = 0;
    for (Common_Log_MessageQueueConstIterator_t iterator = messages_.begin ();
         ((iterator != messages_.end ()) && (index < numLines_));
         iterator++, index++)
    {
      text_size = RPG_Graphics_Common_Tools::textSize (font_, *iterator);
      inherited::clipRectangle_.h += text_size.second + 1;
      inherited::clipRectangle_.w =
          ((inherited::clipRectangle_.w < static_cast<int> (text_size.first)) ? static_cast<int> (text_size.first)
                                                                              : inherited::clipRectangle_.w);
    } // end FOR
    // add some padding
    inherited::clipRectangle_.w += 2;
    inherited::clipRectangle_.h += 1;
    struct SDL_Rect parent_area;
    inherited::parent_->getArea (parent_area,
                                 false); // toplevel- ?
    inherited::clipRectangle_.x = (parent_area.x +
                                   ((parent_area.w - inherited::clipRectangle_.w) / 2));
    inherited::clipRectangle_.y = (parent_area.y +
                                   parent_area.h -
                                   inherited::clipRectangle_.h);

    // save background
    inherited::saveBG (inherited::clipRectangle_); // --> save everything

    // shade text area
    if (BG_)
    {
#if defined (SDL_USE) || defined (SDL2_USE)
      SDL_FreeSurface (BG_);
#elif defined (SDL3_USE)
      SDL_DestroySurface (BG_);
#endif // SDL_USE || SDL2_USE || SDL3_USE
      BG_ = NULL;
    } // end IF
    BG_ = RPG_Graphics_Surface::create (inherited::clipRectangle_.w,
                                        inherited::clipRectangle_.h);
    if (!BG_)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to RPG_Graphics_Surface::create(%d,%d), returning\n"),
                  inherited::clipRectangle_.w, inherited::clipRectangle_.h));

      // clean up
      if (inherited::screenLock_)
        inherited::screenLock_->unlock ();

      return;
    } // end IF
    RPG_Graphics_Surface::fill (RPG_Graphics_SDL_Tools::getColor (COLOR_BLACK_A50,
                                                                  *BG_->format,
                                                                  1.0f),
                                BG_,
                                NULL);

    // initialize clipping
    //clip(targetSurface,
    //     offsetX_in,
    //     offsetY_in);
    struct SDL_Rect clip_rect_orig;
#if defined (SDL_USE) || defined (SDL2_USE)
    SDL_GetClipRect (target_surface, &clip_rect_orig);
    if (!SDL_SetClipRect (target_surface, &(inherited::clipRectangle_)))
#elif defined (SDL3_USE)
    SDL_GetSurfaceClipRect (target_surface, &clip_rect_orig);
    if (!SDL_SetSurfaceClipRect (target_surface, &(inherited::clipRectangle_)))
#endif // SDL_USE || SDL2_USE || SDL3_USE
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to SDL_SetClipRect(): %s, returning\n"),
                  ACE_TEXT (SDL_GetError ())));

      // clean up
      if (inherited::screenLock_)
        inherited::screenLock_->unlock ();

      return;
    } // end IF

    RPG_Graphics_Surface::put (std::make_pair (inherited::clipRectangle_.x,
                                               inherited::clipRectangle_.y),
                               *BG_,
                               target_surface,
                               dirty_region);

    // draw messages
    index = 0;
    ACE_UINT32 text_color_i, shade_color_i;
    text_color_i =
      RPG_Graphics_SDL_Tools::getColor (RPG_CLIENT_MESSAGE_COLOR,
                                        *target_surface->format,
                                        1.0F);
    shade_color_i = 
      RPG_Graphics_SDL_Tools::getColor (RPG_CLIENT_MESSAGE_SHADECOLOR,
                                        *target_surface->format,
                                        1.0F);
    for (Common_Log_MessageQueueConstIterator_t iterator = messages_.begin ();
         ((iterator != messages_.end ()) && (index < numLines_));
         iterator++, index++)
    {
      text_size = RPG_Graphics_Common_Tools::textSize (font_, *iterator);
      RPG_Graphics_Surface::putText (font_,
                                     *iterator,
                                     RPG_Graphics_SDL_Tools::colorToSDLColor (text_color_i,
                                                                              *target_surface->format),
                                     RPG_CLIENT_MESSAGE_SHADE_LINES,
                                     RPG_Graphics_SDL_Tools::colorToSDLColor (shade_color_i,
                                                                              *target_surface->format),
                                     std::make_pair (inherited::clipRectangle_.x,
                                                     inherited::clipRectangle_.y +
                                                     (inherited::clipRectangle_.h -
                                                      ((index + 1) * (text_size.second + 1)))),
                                     target_surface,
                                     dirty_region);
    } // end FOR

    // reset clipping
#if defined (SDL_USE) || defined (SDL2_USE)
    if (!SDL_SetClipRect (target_surface, &clip_rect_orig))
#elif defined (SDL3_USE)
    if (!SDL_SetSurfaceClipRect (target_surface, &clip_rect_orig))
#endif // SDL_USE || SDL2_USE || SDL3_USE
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to SDL_SetClipRect(): %s, returning\n"),
                  ACE_TEXT (SDL_GetError ())));

      // clean up
      if (inherited::screenLock_)
        inherited::screenLock_->unlock ();

      return;
    } // end IF
  } // end lock scope
  if (inherited::screenLock_)
    inherited::screenLock_->unlock ();

  // invalidate dirty region
  dirty_region =
      RPG_Graphics_SDL_Tools::boundingBox (dirty_region,
                                           inherited::clipRectangle_);
  inherited::invalidate (dirty_region);

  // remember position of last realization
  lastAbsolutePosition_ = std::make_pair (inherited::clipRectangle_.x,
                                          inherited::clipRectangle_.y);
}

void
RPG_Client_Window_Message::initialize (Common_ILock* screenLock_in,
                                       enum RPG_Graphics_Font font_in,
                                       unsigned int numLines_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Window_Message::initialize"));

  inherited::initialize (screenLock_in);

  // sanity check(s)
  ACE_ASSERT (numLines_in);

  font_     = font_in;
  numLines_ = numLines_in;
}

void
RPG_Client_Window_Message::push (const std::string& message_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Window_Message::push"));

  ACE_GUARD (ACE_Thread_Mutex, aGuard, lock_);

  // age cache ?
  if (messages_.size () == RPG_CLIENT_MESSAGE_CACHE_SIZE)
    messages_.pop_back ();
  messages_.push_front (message_in);
}
