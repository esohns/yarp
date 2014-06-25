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

#include "rpg_graphics_surface.h"

#include "rpg_common_macros.h"

#include "ace/Log_Msg.h"

RPG_Graphics_SDLWindowSub::RPG_Graphics_SDLWindowSub(const RPG_Graphics_WindowType& type_in,
                                                     const RPG_Graphics_SDLWindowBase& parent_in,
                                                     // *NOTE*: offset doesn't include any border(s) !
                                                     const RPG_Graphics_Offset_t& offset_in,
                                                     const std::string& title_in)
//                                                      SDL_Surface* backGround_in)
 : inherited(type_in,        // type
             parent_in,      // parent
             offset_in,      // offset
             title_in),      // title
//              backGround_in), // background
   myBGHasBeenSaved(false),
   myIsVisible(false),
   myBG(NULL)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_SDLWindowSub::RPG_Graphics_SDLWindowSub"));

}

RPG_Graphics_SDLWindowSub::~RPG_Graphics_SDLWindowSub()
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_SDLWindowSub::~RPG_Graphics_SDLWindowSub"));

  // clean up
  if (myBG)
    SDL_FreeSurface(myBG);
}

void
RPG_Graphics_SDLWindowSub::show(SDL_Rect& dirtyRegion_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_SDLWindowSub::show"));

	if (!myBGHasBeenSaved)
		saveBG(std::make_pair(inherited::myClipRect.w,
		                      inherited::myClipRect.h));
  myIsVisible = true;

  inherited::show(dirtyRegion_out);
}

void
RPG_Graphics_SDLWindowSub::hide(SDL_Rect& dirtyRegion_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_SDLWindowSub::hide"));

  myIsVisible = false;

  // step0: init return value(s)
  ACE_OS::memset(&dirtyRegion_out, 0, sizeof(dirtyRegion_out));

  // restore saved background
  if (myBG)
	{
    restoreBG(dirtyRegion_out);
    invalidate(dirtyRegion_out);
  } // end IF
}

bool
RPG_Graphics_SDLWindowSub::visible() const
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_SDLWindowSub::visible"));

  return myIsVisible;
}

RPG_Graphics_IWindowBase*
RPG_Graphics_SDLWindowSub::getWindow(const RPG_Graphics_Position_t& position_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_SDLWindowSub::getWindow"));

  if (!myIsVisible)
    return NULL;

  return inherited::getWindow(position_in);
}

void
RPG_Graphics_SDLWindowSub::refresh(SDL_Surface* targetSurface_in)
{
	RPG_TRACE(ACE_TEXT("RPG_Graphics_SDLWindowSub::refresh"));

	// sanity check(s)
	if (!myIsVisible)
		return; // nothing to do...
	ACE_ASSERT(myBGHasBeenSaved);

	saveBG();
}

void
RPG_Graphics_SDLWindowSub::saveBG(const RPG_Graphics_Size_t& size_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_SDLWindowSub::saveBG"));

  if (size_in.first && size_in.second)
  {
    // clean up ?
    if (myBG)
    {
      SDL_FreeSurface(myBG);
      myBG = NULL;
    } // end IF
    myBG = RPG_Graphics_Surface::create(size_in.first,
                                        size_in.second);
    if (!myBG)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to RPG_Graphics_Surface::create(%u,%u), returning\n"),
                 size_in.first, size_in.second));

      return;
    } // end IF
  } // end IF
	ACE_ASSERT(myBG);

  RPG_Graphics_Surface::get(std::make_pair(myClipRect.x,
                                           myClipRect.y),
                            true,
                            *inherited::myScreen,
                            *myBG);

	myBGHasBeenSaved = true;
}

void
RPG_Graphics_SDLWindowSub::restoreBG(SDL_Rect& dirtyRegion_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_SDLWindowSub::restoreBG"));

  // step0: init return value(s)
  ACE_OS::memset(&dirtyRegion_out, 0, sizeof(dirtyRegion_out));

  // sanity check(s)
  ACE_ASSERT(myBG);

	if (inherited::myScreenLock)
		myScreenLock->lock();
  RPG_Graphics_Surface::put(std::make_pair(myClipRect.x,
                                           myClipRect.y),
                            *myBG,
                            inherited::myScreen,
                            dirtyRegion_out);
	if (inherited::myScreenLock)
		myScreenLock->unlock();
}
