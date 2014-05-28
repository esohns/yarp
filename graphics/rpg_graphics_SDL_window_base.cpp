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

#include "rpg_graphics_SDL_window_base.h"

#include "rpg_graphics_defines.h"
#include "rpg_graphics_dictionary.h"
#include "rpg_graphics_surface.h"
#include "rpg_graphics_common_tools.h"
#include "rpg_graphics_SDL_window_sub.h"
#include "rpg_graphics_SDL_tools.h"

#include "rpg_common_macros.h"

#include <ace/OS.h>

RPG_Graphics_SDLWindowBase::RPG_Graphics_SDLWindowBase(const RPG_Graphics_WindowType& type_in,
	                                                     const RPG_Graphics_Size_t& size_in,
                                                       const std::string& title_in)
//                                                        SDL_Surface* backGround_in)
 : //inherited(),
   myScreen(NULL),
   myScreenLock(NULL),
   myBorderTop(0),
   myBorderBottom(0),
   myBorderLeft(0),
   myBorderRight(0),
   myTitle(title_in),
//    myBackGround(backGround_in),
   myParent(NULL),
//    myChildren(),
   myLastAbsolutePosition(std::make_pair(std::numeric_limits<unsigned int>::max(),
                                         std::numeric_limits<unsigned int>::max())),
//    myClipRect(),
//    myInvalidRegions(),
   myFlip(false),
   myType(type_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_SDLWindowBase::RPG_Graphics_SDLWindowBase"));

  ACE_OS::memset(&myClipRect, 0, sizeof(myClipRect));
  myClipRect.w = static_cast<Uint16>(size_in.first);
  myClipRect.h = static_cast<Uint16>(size_in.second);
}

RPG_Graphics_SDLWindowBase::RPG_Graphics_SDLWindowBase(const RPG_Graphics_WindowType& type_in,
                                                       const RPG_Graphics_SDLWindowBase& parent_in,
                                                       const RPG_Graphics_Offset_t& offset_in,
                                                       const std::string& title_in)
//                                                        SDL_Surface* backGround_in)
  : //inherited(),
    myScreen(parent_in.myScreen),
    myScreenLock(NULL),
    myBorderTop(0),
    myBorderBottom(0),
    myBorderLeft(0),
    myBorderRight(0),
    myTitle(title_in),
//     myBackGround(backGround_in),
    myParent(&const_cast<RPG_Graphics_SDLWindowBase&>(parent_in)),
//    myChildren(),
    myLastAbsolutePosition(std::make_pair(std::numeric_limits<unsigned int>::max(),
                                          std::numeric_limits<unsigned int>::max())),
//    myClipRect(),
//    myInvalidRegions(),
    myFlip(false),
    myType(type_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_SDLWindowBase::RPG_Graphics_SDLWindowBase"));

  // register with parent
  myParent->addChild(this);

  // init clip rect
	myParent->getBorders(myBorderTop,
		                   myBorderBottom,
											 myBorderLeft,
											 myBorderRight,
											 false);
	SDL_Rect clip_rectangle;
	// get parent window clip area (if any)
	myParent->getArea(clip_rectangle);
	myClipRect = clip_rectangle;
	if (offset_in.first)
		myClipRect.x = static_cast<int16_t>(offset_in.first + 
		                                    myBorderLeft);
	else
		myClipRect.x += static_cast<int16_t>(myBorderLeft);
	if (offset_in.second)
		myClipRect.y = static_cast<int16_t>(offset_in.second + 
		                                    myBorderTop);
	else
		myClipRect.y += static_cast<int16_t>(myBorderTop);
	myClipRect.w = (clip_rectangle.w                 -
		              offset_in.first                  -
		              (myBorderLeft + myBorderRight));
	myClipRect.h = (clip_rectangle.h                 -
		              offset_in.second                 -
		              (myBorderTop + myBorderBottom));

	// clean up
	myBorderTop = myBorderBottom = myBorderLeft = myBorderRight = 0;
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

SDL_Rect
RPG_Graphics_SDLWindowBase::getDirty() const
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_SDLWindowBase::getDirty"));

  SDL_Rect result = {0, 0, 0, 0};
  for (RPG_Graphics_DirtyRegionsConstIterator_t iterator = myInvalidRegions.begin();
       iterator != myInvalidRegions.end();
       iterator++)
    result = RPG_Graphics_SDL_Tools::boundingBox(result,
                                                 *iterator);

  return result;
}

void
RPG_Graphics_SDLWindowBase::clean()
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_SDLWindowBase::clean"));

  myInvalidRegions.clear();

  // recurse into any children
  for (RPG_Graphics_WindowsIterator_t iterator = myChildren.begin();
       iterator != myChildren.end();
       iterator++)
    (*iterator)->clean();
}

void
RPG_Graphics_SDLWindowBase::init(RPG_Common_ILock* screenLock_in,
                                 const bool& flip_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_SDLWindowBase::init"));

  myScreenLock = screenLock_in;

  // recurse into any children
	for (RPG_Graphics_WindowsIterator_t iterator = myChildren.begin();
		   iterator != myChildren.end();
			 iterator++)
	  (*iterator)->init(screenLock_in);

	myFlip = flip_in;
}

void
RPG_Graphics_SDLWindowBase::getBorders(unsigned int& borderTop_out,
                                       unsigned int& borderBottom_out,
                                       unsigned int& borderLeft_out,
                                       unsigned int& borderRight_out,
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
    unsigned int borderTop;
    unsigned int borderBottom;
    unsigned int borderLeft;
    unsigned int borderRight;
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

//  // sanity check(s)
//  ACE_ASSERT((rect_in.x >= 0) &&
//             (rect_in.x < myScreen->w) &&
//             (rect_in.y >= 0) &&
//             (rect_in.y < myScreen->h) &&
//             (rect_in.w >= 0) &&
////              ((rect_in.x + rect_in.w) <= myScreen->w) &&
//             (rect_in.h >= 0)/* &&
//             ((rect_in.y + rect_in.h) <= myScreen->h)*/);

  if ((rect_in.x == 0) &&
      (rect_in.y == 0) &&
      (rect_in.w == 0) &&
      (rect_in.h == 0))
    myInvalidRegions.push_back(myClipRect);
  else
  {
    if (rect_in.x < 0)
      ACE_ASSERT(false);
    myInvalidRegions.push_back(rect_in);
  } // end ELSE
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
RPG_Graphics_SDLWindowBase::getScreen() const
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_SDLWindowBase::getScreen"));

  return (myScreen ? myScreen
                   : SDL_GetVideoSurface());
}

void
RPG_Graphics_SDLWindowBase::clear(const RPG_Graphics_ColorName& color_in,
                                  const bool& clip_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_SDLWindowBase::clear"));

  // sanity check(s)
  ACE_ASSERT(myScreen);

	if (clip_in)
		clip();

  if (myScreenLock)
    myScreenLock->lock();
  if (SDL_FillRect(myScreen,                                     // target surface
                   &myClipRect,                                  // fill area
                   RPG_Graphics_SDL_Tools::getColor(color_in,
                                                    *myScreen))) // color
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_FillRect(): \"%s\", aborting\n"),
               ACE_TEXT(SDL_GetError())));

    // clean up
    if (myScreenLock)
      myScreenLock->unlock();

    return;
  } // end IF
  if (myScreenLock)
    myScreenLock->unlock();

	if (clip_in)
		unclip();

  invalidate(myClipRect);
}

//void
//RPG_Graphics_SDLWindowBase::drawChild(const RPG_Graphics_WindowType& child_in,
//                                      SDL_Surface* targetSurface_in,
//                                      const unsigned int& offsetX_in,
//                                      const unsigned int& offsetY_in)
//{
//  RPG_TRACE(ACE_TEXT("RPG_Graphics_SDLWindowBase::drawChild"));
//
//  // sanity check(s)
//  ACE_ASSERT(child_in != RPG_GRAPHICS_WINDOWTYPE_INVALID);
//
//  // set target surface
//  SDL_Surface* target_surface = (targetSurface_in ? targetSurface_in : myScreen);
//
//  // realize any child(ren) of a specific type
//  for (RPG_Graphics_WindowsIterator_t iterator = myChildren.begin();
//       iterator != myChildren.end();
//       iterator++)
//  {
//    if ((*iterator)->getType() != child_in)
//      continue;
//
//    try
//    {
//      (*iterator)->draw(target_surface,
//                        offsetX_in,
//                        offsetY_in);
//    }
//    catch (...)
//    {
//      ACE_DEBUG((LM_ERROR,
//                 ACE_TEXT("caught exception in RPG_Graphics_IWindow::draw(), continuing\n")));
//    }
//  } // end FOR
//}

void
RPG_Graphics_SDLWindowBase::update(SDL_Surface* targetSurface_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_SDLWindowBase::update"));

  // sanity check(s)
  SDL_Surface* target_surface = (targetSurface_in ? targetSurface_in
                                                  : myScreen);
  ACE_ASSERT(target_surface);

  // compute bounding box of dirty areas
  SDL_Rect dirty_region = getDirty();
  // recurse into any children
  for (RPG_Graphics_WindowsIterator_t iterator = myChildren.begin();
       iterator != myChildren.end();
       iterator++)
    dirty_region = RPG_Graphics_SDL_Tools::boundingBox(dirty_region,
                                                       (*iterator)->getDirty());

  //ACE_DEBUG((LM_DEBUG,
  //           ACE_TEXT("refreshing bbox [[%d,%d][%d,%d]]...\n"),
  //           dirty_region.x,
  //           dirty_region.y,
  //           dirty_region.w,
  //           dirty_region.h));

  if (myScreenLock)
    myScreenLock->lock();
  if (myFlip)
  {
    if (SDL_Flip(target_surface) == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to SDL_Flip(%@): \"%s\", continuing\n"),
                 target_surface,
                 ACE_TEXT(SDL_GetError())));
  } // end IF
  else
    SDL_UpdateRect(target_surface,
                   dirty_region.x,
                   dirty_region.y,
                   dirty_region.w,
                   dirty_region.h);
  if (myScreenLock)
    myScreenLock->unlock();

  clean();
}

void
RPG_Graphics_SDLWindowBase::handleEvent(const SDL_Event& event_in,
                                        RPG_Graphics_IWindowBase* window_in,
                                        SDL_Rect& dirtyRegion_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_SDLWindowBase::handleEvent"));

  ACE_UNUSED_ARG(window_in);

  // init return value(s)
  ACE_OS::memset(&dirtyRegion_out, 0, sizeof(dirtyRegion_out));

//  switch (event_in.type)
//  {
//    // *** visibility ***
//    case SDL_ACTIVEEVENT:
//    {
//      if (event_in.active.state & SDL_APPACTIVE)
//      {
//        if (event_in.active.gain)
//        {
//          ACE_DEBUG((LM_DEBUG,
//                     ACE_TEXT("activated...\n")));
//        } // end IF
//        else
//        {
//          ACE_DEBUG((LM_DEBUG,
//                     ACE_TEXT("iconified...\n")));
//        } // end ELSE
//      } // end IF
//      if (event_in.active.state & SDL_APPMOUSEFOCUS)
//      {
//        if (event_in.active.gain)
//        {
//          ACE_DEBUG((LM_DEBUG,
//                     ACE_TEXT("mouse focus...\n")));
//        } // end IF
//        else
//        {
//          ACE_DEBUG((LM_DEBUG,
//                     ACE_TEXT("lost mouse focus...\n")));
//        } // end ELSE
//      } // end IF

//      break;
//    }
//    // *** keyboard ***
//    case SDL_KEYDOWN:
//    {
//      ACE_DEBUG((LM_DEBUG,
//                 ACE_TEXT("key pressed...\n")));

//      break;
//    }
//    case SDL_KEYUP:
//    {
//      ACE_DEBUG((LM_DEBUG,
//                 ACE_TEXT("key released...\n")));

//      break;
//    }
//    // *** mouse ***
//    case SDL_MOUSEMOTION:
//    {
//      ACE_DEBUG((LM_DEBUG,
//                 ACE_TEXT("mouse motion...\n")));

//      break;
//    }
//    case SDL_MOUSEBUTTONDOWN:
//    {
//      ACE_DEBUG((LM_DEBUG,
//                 ACE_TEXT("mouse button pressed...\n")));

//      break;
//    }
//    case SDL_MOUSEBUTTONUP:
//    {
//      ACE_DEBUG((LM_DEBUG,
//                 ACE_TEXT("mouse button released...\n")));

//      break;
//    }
//    // *** joystick ***
//    case SDL_JOYAXISMOTION:
//    case SDL_JOYBALLMOTION:
//    case SDL_JOYHATMOTION:
//    case SDL_JOYBUTTONDOWN:
//    case SDL_JOYBUTTONUP:
//    {
//      ACE_DEBUG((LM_DEBUG,
//                 ACE_TEXT("joystick activity...\n")));

//      break;
//    }
//    case SDL_QUIT:
//    {
//      ACE_DEBUG((LM_DEBUG,
//                 ACE_TEXT("SDL_QUIT event...\n")));

//      break;
//    }
//    case SDL_SYSWMEVENT:
//    {
//      ACE_DEBUG((LM_DEBUG,
//                 ACE_TEXT("SDL_SYSWMEVENT event...\n")));

//      break;
//    }
//    case SDL_VIDEORESIZE:
//    {
//      ACE_DEBUG((LM_DEBUG,
//                 ACE_TEXT("SDL_VIDEORESIZE event...\n")));

//      break;
//    }
//    case SDL_VIDEOEXPOSE:
//    {
//      ACE_DEBUG((LM_DEBUG,
//                 ACE_TEXT("SDL_VIDEOEXPOSE event...\n")));

//      break;
//    }
//    case SDL_USEREVENT:
//    {
//      ACE_DEBUG((LM_DEBUG,
//                 ACE_TEXT("SDL_USEREVENT event...\n")));

//      break;
//    }
//    case RPG_GRAPHICS_SDL_HOVEREVENT:
//    {
//      ACE_DEBUG((LM_DEBUG,
//                 ACE_TEXT("RPG_GRAPHICS_SDL_HOVEREVENT event...\n")));

//      break;
//    }
//    case RPG_GRAPHICS_SDL_MOUSEMOVEOUT:
//    {
//      ACE_DEBUG((LM_DEBUG,
//                 ACE_TEXT("RPG_GRAPHICS_SDL_MOUSEMOVEOUT event...\n")));

//      break;
//    }
//    default:
//    {
//      ACE_DEBUG((LM_ERROR,
//                 ACE_TEXT("received unknown event (was: %u)...\n"),
//                 static_cast<unsigned int>(event_in.type)));

//      break;
//    }
//  } // end SWITCH

  // find active position
  RPG_Graphics_Position_t active_position;
  switch (event_in.type)
  {
    case SDL_MOUSEMOTION:
      active_position = std::make_pair(event_in.motion.x,
                                       event_in.motion.y); break;
    case SDL_MOUSEBUTTONDOWN:
      active_position = std::make_pair(event_in.button.x,
                                       event_in.button.y); break;
    default:
    {
      int x, y;
      SDL_GetMouseState(&x, &y);
      active_position = std::make_pair(x, y);

      break;
    }
  } // end SWITCH

  // recurse into any children
  RPG_Graphics_SDLWindowSub* sub_window = NULL;
  SDL_Rect window_area;
  ACE_OS::memset(&window_area, 0, sizeof(window_area));
  SDL_Rect dirty_region;
  ACE_OS::memset(&dirty_region, 0, sizeof(dirty_region));
  // *NOTE*: check in reverse order, because "newer", overlapping children are
  // considered to be "on-top"
  for (RPG_Graphics_WindowsRIterator_t iterator = myChildren.rbegin();
       iterator != myChildren.rend();
       iterator++)
  {
    // visible ?
    sub_window = NULL;
		try
		{
      sub_window = dynamic_cast<RPG_Graphics_SDLWindowSub*>(*iterator);
		}
		catch (...)
		{
//      ACE_DEBUG((LM_ERROR,
//                 ACE_TEXT("failed to dynamic_cast<RPG_Graphics_SDLWindowSub*>(%@), continuing\n"),
//                 *iterator));
    }
    if (sub_window &&
        !sub_window->visible())
      continue;

    // covered ?
    try
    {
      (*iterator)->getArea(window_area);
    }
    catch (...)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("caught exception in RPG_Graphics_IWindow::getArea(), continuing\n")));
    }
    if ((active_position.first  <  static_cast<unsigned int>(window_area.x))   ||
        (active_position.second <  static_cast<unsigned int>(window_area.y))   ||
        (active_position.first  >= (static_cast<unsigned int>(window_area.x) +
                                    static_cast<unsigned int>(window_area.w))) ||
        (active_position.second >= (static_cast<unsigned int>(window_area.y) +
                                    static_cast<unsigned int>(window_area.h))))
      continue;

    ACE_OS::memset(&dirty_region, 0, sizeof(dirty_region));
    try
    {
      (*iterator)->handleEvent(event_in,
                               window_in,
                               dirty_region);
    }
    catch (...)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("caught exception in RPG_Graphics_IWindow::handleEvent(), continuing\n")));
    }
    dirtyRegion_out = RPG_Graphics_SDL_Tools::boundingBox(dirty_region,
                                                          dirtyRegion_out);
  } // end FOR
}

void
RPG_Graphics_SDLWindowBase::notify(const RPG_Graphics_Cursor& cursor_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_SDLWindowBase::notify"));

  ACE_UNUSED_ARG(cursor_in);

  ACE_ASSERT(false);
  ACE_NOTREACHED(return;)
}

RPG_Graphics_WindowType
RPG_Graphics_SDLWindowBase::getType() const
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_SDLWindowBase::getType"));

  return myType;
}

//RPG_Graphics_Size_t
//RPG_Graphics_SDLWindowBase::getSize(const bool& topLevel_in) const
//{
//  RPG_TRACE(ACE_TEXT("RPG_Graphics_SDLWindowBase::getSize"));
//
//  if (topLevel_in && myParent)
//    return myParent->getSize(true);
//
//  return std::make_pair(myClipRect.w, myClipRect.h);
//}

RPG_Graphics_IWindowBase*
RPG_Graphics_SDLWindowBase::getWindow(const RPG_Graphics_Position_t& position_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_SDLWindowBase::getWindow"));

  if ((position_in.first  < myLastAbsolutePosition.first)                     ||
      (position_in.second < myLastAbsolutePosition.second)                    ||
      (position_in.first  >= (myLastAbsolutePosition.first  + myClipRect.w))  ||
      (position_in.second >= (myLastAbsolutePosition.second + myClipRect.h)))
    return NULL;

  // OK "this" currently "owns" position_in
  // *TODO*: ...unless it's minimized/hidden
  // --> check any children

  // *NOTE*: check in reverse order, because "newer", overlapping children are
  // considered to be "on-top"
  RPG_Graphics_IWindowBase* child = NULL;
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

  return const_cast<RPG_Graphics_SDLWindowBase*>(this);
}

RPG_Graphics_IWindowBase*
RPG_Graphics_SDLWindowBase::child(const RPG_Graphics_WindowType& type_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_SDLWindowBase::child"));

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
                                 const unsigned int& offsetX_in,
                                 const unsigned int& offsetY_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_SDLWindowBase::clip"));

  // set target surface
  SDL_Surface* target_surface = (targetSurface_in ? targetSurface_in
                                                  : myScreen);

	SDL_Rect clip_rectangle = myClipRect;

  // retain previous clip rect
  SDL_GetClipRect(target_surface, &myClipRect);

	if (!SDL_SetClipRect(target_surface, &clip_rectangle))
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_SetClipRect(): %s, continuing\n"),
               ACE_TEXT(SDL_GetError())));
}

void
RPG_Graphics_SDLWindowBase::unclip(SDL_Surface* targetSurface_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_SDLWindowBase::unclip"));

  // set target surface
  SDL_Surface* target_surface = (targetSurface_in ? targetSurface_in
                                                  : myScreen);

  // restore previous clip rect
	SDL_Rect clip_rectangle = myClipRect;
  SDL_GetClipRect(target_surface, &myClipRect);

  if (!SDL_SetClipRect(target_surface, &clip_rectangle))
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_SetClipRect(): %s, continuing\n"),
               ACE_TEXT(SDL_GetError())));
}

void
RPG_Graphics_SDLWindowBase::getArea(SDL_Rect& area_out,
                                    const bool& topLevel_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_SDLWindowBase::getArea"));

  if (topLevel_in && myParent)
    return myParent->getArea(area_out,
		                         true);

  area_out = myClipRect;
}
