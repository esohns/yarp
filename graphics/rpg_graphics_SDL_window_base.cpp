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

RPG_Graphics_SDLWindowBase::RPG_Graphics_SDLWindowBase(const RPG_Graphics_WindowSize_t& size_in,
                                                       const RPG_Graphics_WindowType& type_in,
                                                       const std::string& title_in,
                                                       const RPG_Graphics_Type& fontType_in)
 : //inherited(),
   myScreen(NULL),
   mySize(size_in),
   myBorderTop(0),
   myBorderBottom(0),
   myBorderLeft(0),
   myBorderRight(0),
   myTitle(title_in),
   myTitleFont(fontType_in),
   myOffset(std::make_pair(0, 0)),
   myLastAbsolutePosition(std::make_pair(0, 0)),
   myParent(NULL),
   myType(type_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_SDLWindowBase::RPG_Graphics_SDLWindowBase"));

}

RPG_Graphics_SDLWindowBase::RPG_Graphics_SDLWindowBase(const RPG_Graphics_WindowType& type_in,
                                                       const RPG_Graphics_SDLWindowBase& parent_in,
                                                       const RPG_Graphics_Offset_t& offset_in)
  : //inherited(),
    myScreen(parent_in.myScreen),
    mySize(std::make_pair(0, 0)),
    myBorderTop(0),
    myBorderBottom(0),
    myBorderLeft(0),
    myBorderRight(0),
//     myTitle(),
    myTitleFont(RPG_GRAPHICS_TYPE_INVALID),
    myOffset(offset_in),
    myLastAbsolutePosition(std::make_pair(0, 0)),
    myParent(&ACE_const_cast(RPG_Graphics_SDLWindowBase&, parent_in)),
    myType(type_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_SDLWindowBase::RPG_Graphics_SDLWindowBase"));

  // compute parent's borders
  myParent->getBorders(myBorderTop,
                       myBorderBottom,
                       myBorderLeft,
                       myBorderRight);

  RPG_Graphics_WindowSize_t size_parent = myParent->getSize(true); // top-level
  mySize.first = size_parent.first - (myBorderLeft + myBorderRight);
  mySize.second = size_parent.second - (myBorderTop + myBorderBottom);

  // register with parent
  myParent->addChild(this);
}

RPG_Graphics_SDLWindowBase::~RPG_Graphics_SDLWindowBase()
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_SDLWindowBase::~RPG_Graphics_SDLWindowBase"));

  // de-register with parent
  if (myParent)
    myParent->removeChild(this);

  // free any children (front-to-back)
  for (RPG_Graphics_WindowsRIterator_t iterator = myChildren.rbegin();
       iterator != myChildren.rend();
       iterator++)
    delete *iterator; // *NOTE*: this will invoke removeChild() on us (see above !)
  ACE_ASSERT(myChildren.empty());
}

void
RPG_Graphics_SDLWindowBase::getBorders(unsigned long& borderTop_out,
                                       unsigned long& borderBottom_out,
                                       unsigned long& borderLeft_out,
                                       unsigned long& borderRight_out) const
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_SDLWindowBase::getBorders"));

  // init return value(s)
  borderTop_out = myBorderTop;
  borderBottom_out = myBorderBottom;
  borderLeft_out = myBorderLeft;
  borderRight_out = myBorderRight;

  // iterate over all parent(s)
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
}

RPG_Graphics_SDLWindowBase*
RPG_Graphics_SDLWindowBase::getParent() const
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_SDLWindowBase::getParent"));

  return myParent;
}

void
RPG_Graphics_SDLWindowBase::invalidate(const SDL_Rect& rect_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_SDLWindowBase::invalidate"));

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
  ACE_TRACE(ACE_TEXT("RPG_Graphics_SDLWindowBase::addChild"));

  myChildren.push_back(child_in);
}

void
RPG_Graphics_SDLWindowBase::removeChild(RPG_Graphics_SDLWindowBase* child_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_SDLWindowBase::removeChild"));

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
  ACE_TRACE(ACE_TEXT("RPG_Graphics_SDLWindowBase::setScreen"));

  // sanity check(s)
  ACE_ASSERT(screen_in);

  myScreen = screen_in;
}

void
RPG_Graphics_SDLWindowBase::refresh(SDL_Surface* targetSurface_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_SDLWindowBase::refresh"));

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
  ACE_TRACE(ACE_TEXT("RPG_Graphics_SDLWindowBase::handleEvent"));

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
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("received unknown event (was: %u)...\n"),
                 ACE_static_cast(unsigned long, event_in.type)));

      break;
    }
  } // end SWITCH

  // *NOTE*: should never get here...
  ACE_ASSERT(false);
}

const RPG_Graphics_WindowType
RPG_Graphics_SDLWindowBase::getType() const
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_SDLWindowBase::getType"));

  return myType;
}

const RPG_Graphics_WindowSize_t
RPG_Graphics_SDLWindowBase::getSize(const bool& topLevel_in) const
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_SDLWindowBase::getSize"));

  if (topLevel_in && myParent)
    return myParent->getSize(true);

  return mySize;
}

RPG_Graphics_IWindow*
RPG_Graphics_SDLWindowBase::getWindow(const RPG_Graphics_Position_t& position_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_SDLWindowBase::getWindow"));

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
