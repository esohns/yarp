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

#include "rpg_graphics_SDL_window_base.h"

#include "rpg_graphics_defines.h"
#include "rpg_graphics_dictionary.h"
#include "rpg_graphics_surface.h"
#include "rpg_graphics_common_tools.h"

#include <rpg_common_macros.h>

#include <ace/OS.h>

RPG_Graphics_SDLWindowBase::RPG_Graphics_SDLWindowBase(const RPG_Graphics_WindowSize_t& size_in,
                                                       const RPG_Graphics_WindowType& type_in,
                                                       const std::string& title_in)
//                                                        SDL_Surface* backGround_in)
 : //inherited(),
   myScreen(NULL),
   mySize(size_in),
   myBorderTop(0),
   myBorderBottom(0),
   myBorderLeft(0),
   myBorderRight(0),
   myTitle(title_in),
//    myBackGround(backGround_in),
   myOffset(std::make_pair(0, 0)),
   myLastAbsolutePosition(std::make_pair(0, 0)),
   myParent(NULL),
   myType(type_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_SDLWindowBase::RPG_Graphics_SDLWindowBase"));

  ACE_OS::memset(&myClipRect,
                 0,
                 sizeof(myClipRect));
}

RPG_Graphics_SDLWindowBase::RPG_Graphics_SDLWindowBase(const RPG_Graphics_WindowType& type_in,
                                                       const RPG_Graphics_SDLWindowBase& parent_in,
                                                       const RPG_Graphics_Offset_t& offset_in,
                                                       const std::string& title_in)
//                                                        SDL_Surface* backGround_in)
  : //inherited(),
    myScreen(parent_in.myScreen),
    mySize(std::make_pair(0, 0)),
    myBorderTop(0),
    myBorderBottom(0),
    myBorderLeft(0),
    myBorderRight(0),
    myTitle(title_in),
//     myBackGround(backGround_in),
    myOffset(offset_in),
    myLastAbsolutePosition(std::make_pair(0, 0)),
    myParent(&const_cast<RPG_Graphics_SDLWindowBase&>(parent_in)),
    myType(type_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_SDLWindowBase::RPG_Graphics_SDLWindowBase"));

  // compute parent's borders
  // *NOTE*: yields absolute values (screen surface coordinates) !
  myParent->getBorders(myBorderTop,
                       myBorderBottom,
                       myBorderLeft,
                       myBorderRight,
                       false); // DON'T recurse

  RPG_Graphics_WindowSize_t size_parent = myParent->getSize(true); // top-level
  mySize.first = size_parent.first - myOffset.first - (myBorderLeft + myBorderRight);
  mySize.second = size_parent.second - myOffset.second - (myBorderTop + myBorderBottom);

  // register with parent
  myParent->addChild(this);

  // init clip rect
  myClipRect.x = static_cast<int16_t>(myBorderLeft + myOffset.first);
  myClipRect.y = static_cast<int16_t>(myBorderTop + myOffset.second);
  myClipRect.w = static_cast<uint16_t>(size_parent.first - (myBorderLeft + myBorderRight) - myOffset.first);
  myClipRect.h = static_cast<uint16_t>(size_parent.second - (myBorderTop + myBorderBottom) - myOffset.second);
}

RPG_Graphics_SDLWindowBase::~RPG_Graphics_SDLWindowBase()
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_SDLWindowBase::~RPG_Graphics_SDLWindowBase"));

  // de-register with parent
  if (myParent)
    myParent->removeChild(this);

  // free any children (front-to-back)
  while (!myChildren.empty())
    delete myChildren.back(); // *NOTE*: this will invoke removeChild() on us (see above !)

//   // free surface
//   if (myBackGround)
//     SDL_FreeSurface(myBackGround);
}

void
RPG_Graphics_SDLWindowBase::getBorders(unsigned long& borderTop_out,
                                       unsigned long& borderBottom_out,
                                       unsigned long& borderLeft_out,
                                       unsigned long& borderRight_out,
                                       const bool& recursive_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_SDLWindowBase::getBorders"));

  // init return value(s)
  borderTop_out = myBorderTop;
  borderBottom_out = myBorderBottom;
  borderLeft_out = myBorderLeft;
  borderRight_out = myBorderRight;

  // iterate over all parent(s) ?
  if (recursive_in)
  {
    unsigned long borderTop;
    unsigned long borderBottom;
    unsigned long borderLeft;
    unsigned long borderRight;
    for (const RPG_Graphics_SDLWindowBase* current = myParent;
         current != NULL;
         current = current->getParent())
    {
      current->getBorders(borderTop,
                          borderBottom,
                          borderLeft,
                          borderRight);
      borderTop_out += borderTop;
      borderBottom_out += borderBottom;
      borderLeft_out += borderLeft;
      borderRight_out += borderRight;
    } // end FOR
  } // end IF
}

RPG_Graphics_SDLWindowBase*
RPG_Graphics_SDLWindowBase::getParent() const
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_SDLWindowBase::getParent"));

  return myParent;
}

void
RPG_Graphics_SDLWindowBase::invalidate(const SDL_Rect& rect_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_SDLWindowBase::invalidate"));

  // sanity check(s)
  ACE_ASSERT((rect_in.x >= 0) &&
             (rect_in.x < myScreen->w) &&
             (rect_in.y >= 0) &&
             (rect_in.y < myScreen->h) &&
             (rect_in.w >= 0) &&
//              ((rect_in.x + rect_in.w) <= myScreen->w) &&
             (rect_in.h >= 0)/* &&
             ((rect_in.y + rect_in.h) <= myScreen->h)*/);

  myInvalidRegions.push_back(rect_in);
}

void
RPG_Graphics_SDLWindowBase::addChild(RPG_Graphics_SDLWindowBase* child_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_SDLWindowBase::addChild"));

  myChildren.push_back(child_in);
}

void
RPG_Graphics_SDLWindowBase::removeChild(RPG_Graphics_SDLWindowBase* child_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_SDLWindowBase::removeChild"));

  RPG_Graphics_WindowsIterator_t iterator;
  for (iterator = myChildren.begin();
       iterator != myChildren.end();
       iterator++)
    if (*iterator == child_in)
      break;

  ACE_ASSERT(iterator != myChildren.end());
  if (iterator != myChildren.end())
    myChildren.erase(iterator);
}

void
RPG_Graphics_SDLWindowBase::setScreen(SDL_Surface* screen_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_SDLWindowBase::setScreen"));

  // sanity check(s)
  ACE_ASSERT(screen_in);

  myScreen = screen_in;
}

SDL_Surface*
RPG_Graphics_SDLWindowBase::getScreen()
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_SDLWindowBase::getScreen"));

  return (myScreen ? myScreen : SDL_GetVideoSurface());
}

void
RPG_Graphics_SDLWindowBase::refresh(SDL_Surface* targetSurface_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_SDLWindowBase::refresh"));

  // set target surface
  SDL_Surface* targetSurface = (targetSurface_in ? targetSurface_in : myScreen);

  // sanity check(s)
  ACE_ASSERT(targetSurface);

  if (myInvalidRegions.empty())
    return; // nothing to do !

  Sint32 topLeftX = (*myInvalidRegions.begin()).x;
  Sint32 topLeftY = (*myInvalidRegions.begin()).y;
  Sint32 width = (*myInvalidRegions.begin()).w;
  Sint32 height = (*myInvalidRegions.begin()).h;

  // find bounding box of dirty areas
  unsigned long index = 0;
  for (RPG_Graphics_InvalidRegionsConstIterator_t iterator = myInvalidRegions.begin();
       iterator != myInvalidRegions.end();
       iterator++, index++)
  {
//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("dirty[%u]: [[%d,%d][%d,%d]]...\n"),
//                index,
//                (*iterator).x,
//                (*iterator).y,
//                (*iterator).x + (*iterator).w - 1,
//                (*iterator).y + (*iterator).h - 1));

    if (((*iterator).x < topLeftX) ||
        ((*iterator).y < topLeftY))
    {
      if ((*iterator).x < topLeftX)
      {
        width += topLeftX - (*iterator).x;
        topLeftX = (*iterator).x;
      } // end IF
      if ((*iterator).y < topLeftY)
      {
        height += topLeftY - (*iterator).y;
        topLeftY = (*iterator).y;
      } // end IF
    } // end IF
    else
    {
      int overlap = ((*iterator).x + (*iterator).w - 1) - (topLeftX + width - 1);
      if (overlap > 0)
        width += overlap;
      overlap = ((*iterator).y + (*iterator).h - 1) - (topLeftY + height - 1);
      if (overlap > 0)
        height += overlap;
    } // end ELSE
  } // end FOR

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("refreshing bbox [[%d,%d][%d,%d]]...\n"),
//              topLeftX,
//              topLeftY,
//              topLeftX + width - 1,
//              topLeftY + height - 1));

  SDL_UpdateRect(targetSurface,
                 topLeftX,
                 topLeftY,
                 width,
                 height);

  // all fresh & shiny !
  myInvalidRegions.clear();
}

void
RPG_Graphics_SDLWindowBase::handleEvent(const SDL_Event& event_in,
                                        RPG_Graphics_IWindow* window_in,
                                        bool& redraw_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_SDLWindowBase::handleEvent"));

  ACE_UNUSED_ARG(window_in);

  // init return value(s)
  redraw_out = false;

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("RPG_Graphics_SDLWindowBase::handleEvent\n")));

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
    case RPG_GRAPHICS_SDL_HOVEREVENT:
    {
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("RPG_GRAPHICS_SDL_HOVEREVENT event...\n")));

      break;
    }
    case RPG_GRAPHICS_SDL_MOUSEMOVEOUT:
    {
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("RPG_GRAPHICS_SDL_MOUSEMOVEOUT event...\n")));

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

  // *NOTE*: should never get here...
  ACE_ASSERT(false);
}

void
RPG_Graphics_SDLWindowBase::notify(const RPG_Graphics_Cursor& cursor_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_SDLWindowBase::notify"));

  ACE_UNUSED_ARG(cursor_in);

  ACE_ASSERT(false);
}

const RPG_Graphics_WindowType
RPG_Graphics_SDLWindowBase::getType() const
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_SDLWindowBase::getType"));

  return myType;
}

const RPG_Graphics_WindowSize_t
RPG_Graphics_SDLWindowBase::getSize(const bool& topLevel_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_SDLWindowBase::getSize"));

  if (topLevel_in && myParent)
    return myParent->getSize(true);

  return mySize;
}

RPG_Graphics_IWindow*
RPG_Graphics_SDLWindowBase::getWindow(const RPG_Graphics_Position_t& position_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_SDLWindowBase::getWindow"));

  if ((position_in.first < myLastAbsolutePosition.first) ||
      (position_in.second < myLastAbsolutePosition.second) ||
      (position_in.first >= (myLastAbsolutePosition.first + mySize.first)) ||
      (position_in.second >= (myLastAbsolutePosition.second + mySize.second)))
    return NULL;

  // OK "this" currently "owns" position_in
  // *TODO*: ...unless it's minimized/hidden
  // --> check any children

  // *NOTE*: check in reverse order, because "newer", overlapping children are
  // considered to be "on-top"
  RPG_Graphics_IWindow* child = NULL;
  for (RPG_Graphics_WindowsRIterator_t iterator = myChildren.rbegin();
       iterator != myChildren.rend();
       iterator++)
  {
    // *NOTE*: also check child's children
    child = (*iterator)->getWindow(position_in);
    if (!child)
      continue; // try next child

    return child;
  } // end FOR

  return this;
}

RPG_Graphics_IWindow*
RPG_Graphics_SDLWindowBase::getChild(const RPG_Graphics_WindowType& type_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_SDLWindowBase::getChild"));

  for (RPG_Graphics_WindowsIterator_t iterator = myChildren.begin();
       iterator != myChildren.end();
       iterator++)
  {
    // *CONSIDER*: also check child's children ?
    if ((*iterator)->getType() == type_in)
      return *iterator;
  } // end FOR

  // not found !
  return NULL;
}

void
RPG_Graphics_SDLWindowBase::clip(SDL_Surface* targetSurface_in,
                                 const unsigned long& offsetX_in,
                                 const unsigned long& offsetY_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_SDLWindowBase::clip"));

  // set target surface
  SDL_Surface* targetSurface = (targetSurface_in ? targetSurface_in : myScreen);

  // save previous clip rect
  SDL_GetClipRect(targetSurface, &myClipRect);

  SDL_Rect clipRect;
  clipRect.x = static_cast<int16_t>(offsetX_in + myBorderLeft + myOffset.first);
  clipRect.y = static_cast<int16_t>(offsetY_in + myBorderTop + myOffset.second);
  clipRect.w = static_cast<uint16_t>(targetSurface->w - offsetX_in - (myBorderLeft + myBorderRight) - myOffset.first);
  clipRect.h = static_cast<uint16_t>(targetSurface->h - offsetY_in - (myBorderTop + myBorderBottom) - myOffset.second);
  if (!SDL_SetClipRect(targetSurface, &clipRect))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_SetClipRect(): %s, aborting\n"),
               SDL_GetError()));

    return;
  } // end IF
}

void
RPG_Graphics_SDLWindowBase::unclip(SDL_Surface* targetSurface_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_SDLWindowBase::unclip"));

  // set target surface
  SDL_Surface* targetSurface = (targetSurface_in ? targetSurface_in : myScreen);

  if (!SDL_SetClipRect(targetSurface, &myClipRect))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_SetClipRect(): %s, aborting\n"),
               SDL_GetError()));

    return;
  } // end IF
}
