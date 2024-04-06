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

#include "rpg_graphics_SDL_window_sub.h"

#include "ace/Log_Msg.h"
#include "ace/OS.h"

#include "rpg_common_macros.h"

#include "rpg_graphics_surface.h"
#include "rpg_graphics_SDL_tools.h"

RPG_Graphics_SDLWindowSub::RPG_Graphics_SDLWindowSub (enum RPG_Graphics_WindowType type_in,
                                                      const RPG_Graphics_SDLWindowBase& parent_in,
                                                      // *NOTE*: offset doesn't include any border(s) !
                                                      const RPG_Graphics_Offset_t& offset_in,
                                                      const std::string& title_in)
//                                                      SDL_Surface* backGround_in)
 : inherited (type_in,       // type
              parent_in,     // parent
              offset_in,     // offset
              title_in)      // title
              //backGround_in) // background
 , BGHasBeenSaved_ (false)
 , isVisible_ (false)
 , BG_ (NULL)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_SDLWindowSub::RPG_Graphics_SDLWindowSub"));

}

RPG_Graphics_SDLWindowSub::~RPG_Graphics_SDLWindowSub ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_SDLWindowSub::~RPG_Graphics_SDLWindowSub"));

  // clean up
  if (BG_)
    SDL_FreeSurface (BG_);
}

void
RPG_Graphics_SDLWindowSub::show (struct SDL_Rect& dirtyRegion_out)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_SDLWindowSub::show"));

  if (!BGHasBeenSaved_)
    saveBG (inherited::clipRectangle_);

  inherited::show (dirtyRegion_out);

  isVisible_ = true;
}

void
RPG_Graphics_SDLWindowSub::hide (struct SDL_Rect& dirtyRegion_out)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_SDLWindowSub::hide"));

  // step0: initialize return value(s)
  ACE_OS::memset (&dirtyRegion_out, 0, sizeof (struct SDL_Rect));

  // restore saved background
  if (BG_)
  {
    restoreBG (dirtyRegion_out);
    invalidate (dirtyRegion_out);
  } // end IF

  isVisible_ = false;
}

void
RPG_Graphics_SDLWindowSub::clip (SDL_Surface* targetSurface_in,
                                 unsigned int offsetX_in,
                                 unsigned int offsetY_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_SDLWindowSub::clip"));

  RPG_Graphics_IWindowBase* parent_p = getParent ();
  // sanity check(s)
  ACE_ASSERT (parent_p);

  parent_p->clip (targetSurface_in,
                  offsetX_in,
                  offsetY_in);
}

void
RPG_Graphics_SDLWindowSub::unclip (SDL_Surface* targetSurface_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_SDLWindowSub::unclip"));

  RPG_Graphics_IWindowBase* parent_p = getParent ();
  // sanity check(s)
  ACE_ASSERT (parent_p);

  parent_p->unclip (targetSurface_in);
}

RPG_Graphics_IWindowBase*
RPG_Graphics_SDLWindowSub::getWindow (const RPG_Graphics_Position_t& position_in) const
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_SDLWindowSub::getWindow"));

  if (!isVisible_)
    return NULL;

  return inherited::getWindow (position_in);
}

void
RPG_Graphics_SDLWindowSub::refresh (SDL_Surface* targetSurface_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_SDLWindowSub::refresh"));

  // sanity check(s)
  if (!isVisible_)
    return; // nothing to do...

  saveBG (inherited::clipRectangle_);
}

void
RPG_Graphics_SDLWindowSub::saveBG (const struct SDL_Rect& area_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_SDLWindowSub::saveBG"));

  // sanity check(s)
  ACE_ASSERT (inherited::screen_);
#if defined (SDL_USE)
  SDL_Surface* surface_p = inherited::screen_;
#elif defined (SDL2_USE)
  SDL_Surface* surface_p = SDL_GetWindowSurface (inherited::screen_);
#endif // SDL_USE || SDL2_USE
  ACE_ASSERT (surface_p);

  if (!BGHasBeenSaved_)
  {
    BGHasBeenSaved_ = true;

    saveBG (inherited::clipRectangle_);
  } // end IF

  if (!BG_)
  {
    BG_ = RPG_Graphics_Surface::create (area_in.w,
                                        area_in.h);
    if (!BG_)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to RPG_Graphics_Surface::create(%u,%u), returning\n"),
                  area_in.w, area_in.w));
      return;
    } // end IF
  } // end IF
  ACE_ASSERT (BG_);

  struct SDL_Rect area = inherited::clipRectangle_;
  area = RPG_Graphics_SDL_Tools::intersect (area, area_in);
  RPG_Graphics_Surface::get (std::make_pair (area.x, area.y),
                             true,
                             *surface_p,
                             *BG_);
}

void
RPG_Graphics_SDLWindowSub::restoreBG (struct SDL_Rect& dirtyRegion_out)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_SDLWindowSub::restoreBG"));

  // sanity check(s)
  ACE_ASSERT (inherited::screen_);
#if defined (SDL_USE)
  SDL_Surface* surface_p = inherited::screen_;
#elif defined (SDL2_USE)
  SDL_Surface* surface_p = SDL_GetWindowSurface (inherited::screen_);
#endif // SDL_USE || SDL2_USE
  ACE_ASSERT (surface_p);
  ACE_ASSERT (BG_);

  // step0: initialize return value(s)
  ACE_OS::memset (&dirtyRegion_out, 0, sizeof (struct SDL_Rect));

  if (inherited::screenLock_)
    screenLock_->lock ();
  RPG_Graphics_Surface::put (std::make_pair (clipRectangle_.x,
                                             clipRectangle_.y),
                             *BG_,
                             surface_p,
                             dirtyRegion_out);
  if (inherited::screenLock_)
    screenLock_->unlock ();
}
