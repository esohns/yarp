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

#include "ace/OS.h"

#include "rpg_common_macros.h"

#include "rpg_graphics_common_tools.h"
#include "rpg_graphics_defines.h"
#include "rpg_graphics_dictionary.h"
#include "rpg_graphics_surface.h"
#include "rpg_graphics_SDL_window_sub.h"
#include "rpg_graphics_SDL_tools.h"

RPG_Graphics_SDLWindowBase::RPG_Graphics_SDLWindowBase (enum RPG_Graphics_WindowType type_in,
                                                        const RPG_Graphics_Size_t& size_in,
                                                        const std::string& title_in)
#if defined (SDL2_USE)
  : renderer_ (NULL)
  , screen_ (NULL)
#elif defined (SDL_USE)
  : screen_ (NULL)
#endif // SDL2_USE || SDL_USE
 , screenLock_ (NULL)
 , borderTop_ (0)
 , borderBottom_ (0)
 , borderLeft_ (0)
 , borderRight_ (0)
 , title_ (title_in)
 , parent_ (NULL)
 , children_ ()
 , lastAbsolutePosition_ (std::make_pair (std::numeric_limits<unsigned int>::max (),
                                          std::numeric_limits<unsigned int>::max()))
 , clipRectangle_ ()
 , invalidRegions_ ()
 , flip_ (false)
 , type_ (type_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_SDLWindowBase::RPG_Graphics_SDLWindowBase"));

  ACE_OS::memset (&clipRectangle_, 0, sizeof (clipRectangle_));
  clipRectangle_.w = static_cast<Uint16>(size_in.first);
  clipRectangle_.h = static_cast<Uint16>(size_in.second);
}

RPG_Graphics_SDLWindowBase::RPG_Graphics_SDLWindowBase (enum RPG_Graphics_WindowType type_in,
                                                        const RPG_Graphics_SDLWindowBase& parent_in,
                                                        const RPG_Graphics_Offset_t& offset_in,
                                                        const std::string& title_in)
#if defined (SDL2_USE)
 : renderer_ (parent_in.renderer_)
 , screen_ (parent_in.screen_)
#elif defined (SDL_USE)
 : screen_ (parent_in.screen_)
#endif // SDL2_USE || SDL_USE
 , screenLock_ (NULL)
 , borderTop_ (0)
 , borderBottom_ (0)
 , borderLeft_ (0)
 , borderRight_ (0)
 , title_ (title_in)
 , parent_ (&const_cast<RPG_Graphics_SDLWindowBase&> (parent_in))
 , children_ ()
 , lastAbsolutePosition_ (std::make_pair (std::numeric_limits<unsigned int>::max (),
                                          std::numeric_limits<unsigned int>::max ()))
 , clipRectangle_ ()
 , invalidRegions_ ()
 , flip_ (false)
 , type_ (type_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_SDLWindowBase::RPG_Graphics_SDLWindowBase"));

  // register with parent
  parent_->addChild (this);

  // init clip rect
  parent_->getBorders (borderTop_,
                       borderBottom_,
                       borderLeft_,
                       borderRight_,
                       false);
  SDL_Rect clip_rectangle;
  // get parent window clip area (if any)
  parent_->getArea (clip_rectangle,
                    false);
  clipRectangle_ = clip_rectangle;
  if (offset_in.first)
    clipRectangle_.x = static_cast<int16_t> (offset_in.first +
                                             borderLeft_);
  else
    clipRectangle_.x += static_cast<int16_t> (borderLeft_);
  if (offset_in.second)
    clipRectangle_.y = static_cast<int16_t> (offset_in.second +
                                             borderTop_);
  else
    clipRectangle_.y += static_cast<int16_t> (borderTop_);
  clipRectangle_.w = (clip_rectangle.w -
                      offset_in.first -
                      (borderLeft_ + borderRight_));
  clipRectangle_.h = (clip_rectangle.h -
                      offset_in.second -
                      (borderTop_ + borderBottom_));

  // clean up
  borderTop_ = borderBottom_ = borderLeft_ = borderRight_ = 0;
}

RPG_Graphics_SDLWindowBase::~RPG_Graphics_SDLWindowBase ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_SDLWindowBase::~RPG_Graphics_SDLWindowBase"));

  // de-register with parent
  if (parent_)
    parent_->removeChild (this);

  // free any children (front-to-back)
  RPG_Graphics_SDLWindowBase* window_p = NULL;
  while (!children_.empty ())
  {
    try
    {
      window_p = dynamic_cast<RPG_Graphics_SDLWindowBase*> (children_.back ());
    }
    catch (...)
    {
      window_p = NULL;
    }
    if (!window_p)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to dynamic_cast<RPG_Graphics_SDLWindowBase*>(%@), continuing\n"),
                  children_.back ()));
      continue;
    } // end IF

    delete window_p; // *NOTE*: this will invoke removeChild() on us (see above !)
  } // end WHILE

//   // free surface
//   if (myBackGround)
//     SDL_FreeSurface(myBackGround);
}

SDL_Rect
RPG_Graphics_SDLWindowBase::getDirty () const
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_SDLWindowBase::getDirty"));

  SDL_Rect result = {0, 0, 0, 0};
  for (RPG_Graphics_InvalidRegionsConstIterator_t iterator = invalidRegions_.begin ();
       iterator != invalidRegions_.end ();
       iterator++)
    result = RPG_Graphics_SDL_Tools::boundingBox (result,
                                                  *iterator);

  return result;
}

void
RPG_Graphics_SDLWindowBase::clean ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_SDLWindowBase::clean"));

  invalidRegions_.clear ();

  // recurse into any children
  RPG_Graphics_SDLWindowBase* window_p = NULL;
  for (RPG_Graphics_WindowsIterator_t iterator = children_.begin ();
       iterator != children_.end ();
       iterator++)
  {
    window_p = NULL;
    try
    {
      window_p = dynamic_cast<RPG_Graphics_SDLWindowBase*> (*iterator);
    }
    catch (...)
    {
      window_p = NULL;
    }
    if (!window_p)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to dynamic_cast<RPG_Graphics_SDLWindowBase*>(%@), continuing\n"),
                  *iterator));
      continue;
    } // end IF
    window_p->clean ();
  } // end FOR
}

void
RPG_Graphics_SDLWindowBase::refresh (SDL_Surface* targetSurface_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_SDLWindowBase::refresh"));

  // delegate (recurse into any children)
  for (RPG_Graphics_WindowsIterator_t iterator = children_.begin ();
       iterator != children_.end ();
       iterator++)
  {
    try
    {
      (*iterator)->refresh (targetSurface_in);
    }
    catch (...)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%@: caught exception in RPG_Graphics_IWindowBase::refresh(), continuing\n"),
                  *iterator));
      continue;
    } // end IF
  } // end FOR
}

void
RPG_Graphics_SDLWindowBase::initialize (Common_ILock* screenLock_in,
                                        bool flip_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_SDLWindowBase::init"));

  screenLock_ = screenLock_in;
  flip_ = flip_in;

  // recurse into any children
  for (RPG_Graphics_WindowsIterator_t iterator = children_.begin ();
       iterator != children_.end ();
       iterator++)
    (*iterator)->initialize (screenLock_in,
                             flip_in);
}

void
RPG_Graphics_SDLWindowBase::getBorders (unsigned int& borderTop_out,
                                        unsigned int& borderBottom_out,
                                        unsigned int& borderLeft_out,
                                        unsigned int& borderRight_out,
                                        bool recursive_in) const
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_SDLWindowBase::getBorders"));

  // init return value(s)
  borderTop_out = borderTop_;
  borderBottom_out = borderBottom_;
  borderLeft_out = borderLeft_;
  borderRight_out = borderRight_;

  // iterate over all parent(s) ?
  if (recursive_in)
  {
    unsigned int borderTop;
    unsigned int borderBottom;
    unsigned int borderLeft;
    unsigned int borderRight;
    for (const RPG_Graphics_IWindowBase* current = parent_;
         current != NULL;
         current = current->getParent ())
    {
      current->getBorders (borderTop,
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

RPG_Graphics_IWindowBase*
RPG_Graphics_SDLWindowBase::getParent () const
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_SDLWindowBase::getParent"));

  return parent_;
}

void
RPG_Graphics_SDLWindowBase::invalidate (const SDL_Rect& rect_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_SDLWindowBase::invalidate"));

//  // sanity check(s)
//  ACE_ASSERT((rect_in.x >= 0) &&
//             (rect_in.x < screen_->w) &&
//             (rect_in.y >= 0) &&
//             (rect_in.y < screen_->h) &&
//             (rect_in.w >= 0) &&
////              ((rect_in.x + rect_in.w) <= screen_->w) &&
//             (rect_in.h >= 0)/* &&
//             ((rect_in.y + rect_in.h) <= screen_->h)*/);

  if ((rect_in.x == 0) &&
      (rect_in.y == 0) &&
      (rect_in.w == 0) &&
      (rect_in.h == 0))
    invalidRegions_.push_back (clipRectangle_);
  else
  { //ACE_ASSERT (rect_in.x >= 0);
    invalidRegions_.push_back (rect_in);
  } // end ELSE
}

void
RPG_Graphics_SDLWindowBase::addChild (RPG_Graphics_IWindowBase* child_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_SDLWindowBase::addChild"));

  children_.push_back (child_in);
}

void
RPG_Graphics_SDLWindowBase::removeChild (RPG_Graphics_IWindowBase* child_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_SDLWindowBase::removeChild"));

  RPG_Graphics_WindowsIterator_t iterator;
  for (iterator = children_.begin ();
       iterator != children_.end ();
       iterator++)
    if (*iterator == child_in)
      break;

  ACE_ASSERT (iterator != children_.end ());
  if (iterator != children_.end ())
    children_.erase (iterator);
}

void
RPG_Graphics_SDLWindowBase::show (SDL_Rect& dirtyRegion_out)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_SDLWindowBase::show"));

  draw ();

  dirtyRegion_out = clipRectangle_;
}

void
RPG_Graphics_SDLWindowBase::hide (SDL_Rect& dirtyRegion_out)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_SDLWindowBase::hide"));

  clear ();

  dirtyRegion_out = clipRectangle_;
}

bool
RPG_Graphics_SDLWindowBase::visible () const
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_SDLWindowBase::visible"));

  return true;
}

void
RPG_Graphics_SDLWindowBase::clear (const RPG_Graphics_ColorName& color_in,
                                   bool clip_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_SDLWindowBase::clear"));

  // sanity check(s)
  ACE_ASSERT (screen_);
#if defined (SDL_USE)
  SDL_Surface* surface_p = screen_;
#elif defined (SDL2_USE)
  SDL_Surface* surface_p = SDL_GetWindowSurface (screen_);
#endif // SDL_USE || SDL2_USE
  ACE_ASSERT (surface_p);

  // *NOTE*: SDL_FillRect may modify the dstrect argument --> save it first
  SDL_Rect dstrect = clipRectangle_;

  if (clip_in) clip ();

  if (screenLock_)
    screenLock_->lock ();
  if (SDL_FillRect (surface_p,                                    // target surface
                    &dstrect,                                     // fill area
                    RPG_Graphics_SDL_Tools::getColor (color_in,
                                                      *surface_p->format,
                                                      1.0F)))     // color
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_FillRect(): \"%s\", returning\n"),
                ACE_TEXT (SDL_GetError ())));
    if (screenLock_)
      screenLock_->unlock ();
    return;
  } // end IF
  if (screenLock_)
    screenLock_->unlock ();

  if (clip_in) unclip ();

  invalidate (dstrect);
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
//  SDL_Surface* target_surface = (targetSurface_in ? targetSurface_in : screen_);
//
//  // realize any child(ren) of a specific type
//  for (RPG_Graphics_WindowsIterator_t iterator = children_.begin();
//       iterator != children_.end();
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
RPG_Graphics_SDLWindowBase::update (SDL_Surface* targetSurface_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_SDLWindowBase::update"));

  // sanity check(s)
  ACE_ASSERT (screen_);
#if defined (SDL_USE)
  SDL_Surface* surface_p = screen_;
#elif defined (SDL2_USE)
  SDL_Surface* surface_p = SDL_GetWindowSurface (screen_);
#endif // SDL_USE || SDL2_USE
  ACE_ASSERT (surface_p);
  SDL_Surface* target_surface_p = (targetSurface_in ? targetSurface_in
                                                    : surface_p);
  ACE_ASSERT (target_surface_p);

  // compute bounding box of dirty areas
  SDL_Rect dirty_region = getDirty ();
  // recurse into any children
  for (RPG_Graphics_WindowsIterator_t iterator = children_.begin ();
       iterator != children_.end ();
       iterator++)
    dirty_region = RPG_Graphics_SDL_Tools::boundingBox (dirty_region,
                                                        (*iterator)->getDirty ());
  if (!dirty_region.w || !dirty_region.h)
    return; // nothing to do...

  //ACE_DEBUG((LM_DEBUG,
  //           ACE_TEXT("refreshing bbox [[%d,%d][%d,%d]]...\n"),
  //           dirty_region.x,
  //           dirty_region.y,
  //           dirty_region.w,
  //           dirty_region.h));

  if (screenLock_)
    screenLock_->lock ();
  if (flip_)
  {
#if defined (SDL_USE)
    if (SDL_Flip (target_surface_p) == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to SDL_Flip(%@): \"%s\", continuing\n"),
                  target_surface_p,
                  ACE_TEXT (SDL_GetError ())));
#elif defined (SDL2_USE)
    if (SDL_UpdateWindowSurface (screen_) < 0)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to SDL_UpdateWindowSurface(%@): \"%s\", continuing\n"),
                  screen_,
                  ACE_TEXT (SDL_GetError ())));
#endif // SDL_USE || SDL2_USE
  } // end IF
  else
  {
#if defined (SDL_USE)
    SDL_UpdateRect (target_surface_p,
                    dirty_region.x,
                    dirty_region.y,
                    dirty_region.w,
                    dirty_region.h);
#elif defined (SDL2_USE)
    if (SDL_UpdateWindowSurfaceRects (screen_,
                                      &dirty_region,
                                      1) < 0)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to SDL_UpdateWindowSurfaceRects(%@): \"%s\", continuing\n"),
                  screen_,
                  ACE_TEXT (SDL_GetError ())));
#endif // SDL_USE || SDL2_USE
  } // end ELSE
  if (screenLock_)
    screenLock_->unlock ();

  clean ();
}

void
RPG_Graphics_SDLWindowBase::handleEvent (const SDL_Event& event_in,
                                         RPG_Graphics_IWindowBase* window_in,
                                         SDL_Rect& dirtyRegion_out)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_SDLWindowBase::handleEvent"));

  ACE_UNUSED_ARG (window_in);

  // init return value(s)
  ACE_OS::memset (&dirtyRegion_out, 0, sizeof (dirtyRegion_out));

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
      active_position = std::make_pair (event_in.motion.x,
                                        event_in.motion.y); break;
    case SDL_MOUSEBUTTONDOWN:
      active_position = std::make_pair (event_in.button.x,
                                        event_in.button.y); break;
    default:
    {
      int x, y;
      SDL_GetMouseState (&x, &y);
      active_position = std::make_pair (x, y);

      break;
    }
  } // end SWITCH

  // recurse into any children
  RPG_Graphics_SDLWindowSub* sub_window_p = NULL;
  SDL_Rect window_area;
  ACE_OS::memset (&window_area, 0, sizeof (window_area));
  SDL_Rect dirty_region;
  ACE_OS::memset (&dirty_region, 0, sizeof (dirty_region));
  // *NOTE*: check in reverse order, because "newer", overlapping children are
  // considered to be "on-top"
  for (RPG_Graphics_WindowsRIterator_t iterator = children_.rbegin ();
       iterator != children_.rend ();
       iterator++)
  {
    // visible ?
    sub_window_p = NULL;
    try
    {
      sub_window_p = dynamic_cast<RPG_Graphics_SDLWindowSub*> (*iterator);
    }
    catch (...)
    {
//      ACE_DEBUG((LM_ERROR,
//                 ACE_TEXT("failed to dynamic_cast<RPG_Graphics_SDLWindowSub*>(%@), continuing\n"),
//                 *iterator));
    }
    if (sub_window_p && !sub_window_p->visible ())
      continue;

    // covered ?
    try
    {
      (*iterator)->getArea (window_area);
    }
    catch (...)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("caught exception in RPG_Graphics_IWindow::getArea(), continuing\n")));
    }
    if ((active_position.first  <  static_cast<unsigned int> (window_area.x))   ||
        (active_position.second <  static_cast<unsigned int> (window_area.y))   ||
        (active_position.first  >= (static_cast<unsigned int> (window_area.x) +
                                    static_cast<unsigned int> (window_area.w))) ||
        (active_position.second >= (static_cast<unsigned int> (window_area.y) +
                                    static_cast<unsigned int> (window_area.h))))
      continue;

    ACE_OS::memset (&dirty_region, 0, sizeof (dirty_region));
    try
    {
      (*iterator)->handleEvent (event_in,
                                window_in,
                                dirty_region);
    }
    catch (...)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("caught exception in RPG_Graphics_IWindow::handleEvent(), continuing\n")));
    }
    dirtyRegion_out = RPG_Graphics_SDL_Tools::boundingBox (dirty_region,
                                                           dirtyRegion_out);
  } // end FOR
}

void
RPG_Graphics_SDLWindowBase::notify (const RPG_Graphics_Cursor& cursor_in) const
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_SDLWindowBase::notify"));

  ACE_UNUSED_ARG (cursor_in);

  ACE_ASSERT (false);
  ACE_NOTREACHED (return;)
}

RPG_Graphics_WindowType
RPG_Graphics_SDLWindowBase::getType () const
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_SDLWindowBase::getType"));

  return type_;
}

//RPG_Graphics_Size_t
//RPG_Graphics_SDLWindowBase::getSize(const bool& topLevel_in) const
//{
//  RPG_TRACE(ACE_TEXT("RPG_Graphics_SDLWindowBase::getSize"));
//
//  if (topLevel_in && parent_)
//    return parent_->getSize(true);
//
//  return std::make_pair(clipRectangle_.w, clipRectangle_.h);
//}

RPG_Graphics_IWindowBase*
RPG_Graphics_SDLWindowBase::getWindow (const RPG_Graphics_Position_t& position_in) const
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_SDLWindowBase::getWindow"));

  if ((position_in.first  < static_cast<unsigned int> (clipRectangle_.x))                       ||
      (position_in.second < static_cast<unsigned int> (clipRectangle_.y))                       ||
      (position_in.first  >= (static_cast<unsigned int> (clipRectangle_.x) + clipRectangle_.w)) ||
      (position_in.second >= (static_cast<unsigned int> (clipRectangle_.y) + clipRectangle_.h)))
    return NULL;

  // OK "this" currently "owns" position_in
  // --> check any children

  // *NOTE*: check in reverse order, because "newer", overlapping children are
  // considered to be "on-top"
  RPG_Graphics_IWindowBase* child = NULL;
  for (RPG_Graphics_WindowsRIterator_t iterator = children_.rbegin ();
       iterator != children_.rend ();
       iterator++)
  {
    // *NOTE*: also check child's children
    child = (*iterator)->getWindow (position_in);
    if (!child)
      continue; // try next child

    return child;
  } // end FOR

  return const_cast<RPG_Graphics_SDLWindowBase*> (this);
}

RPG_Graphics_IWindowBase*
RPG_Graphics_SDLWindowBase::child (const RPG_Graphics_WindowType& type_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_SDLWindowBase::child"));

  for (RPG_Graphics_WindowsIterator_t iterator = children_.begin ();
       iterator != children_.end ();
       iterator++)
  {
    // *CONSIDER*: also check child's children ?
    if ((*iterator)->getType () == type_in)
      return *iterator;
  } // end FOR

  // not found !
  return NULL;
}

void
RPG_Graphics_SDLWindowBase::clip (SDL_Surface* targetSurface_in,
                                  unsigned int offsetX_in,
                                  unsigned int offsetY_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_SDLWindowBase::clip"));

  // sanity check(s)
  ACE_ASSERT (screen_);
#if defined (SDL_USE)
  SDL_Surface* surface_p = screen_;
#elif defined (SDL2_USE)
  SDL_Surface* surface_p = SDL_GetWindowSurface (screen_);
#endif // SDL_USE || SDL2_USE
  ACE_ASSERT (surface_p);
  SDL_Surface* target_surface_p = (targetSurface_in ? targetSurface_in
                                                    : surface_p);
  ACE_ASSERT (target_surface_p);

	SDL_Rect clip_rectangle = clipRectangle_;

  // retain previous clip rectangle
  SDL_GetClipRect (target_surface_p, &clipRectangle_);

  SDL_SetClipRect (target_surface_p, &clip_rectangle);
}

void
RPG_Graphics_SDLWindowBase::unclip (SDL_Surface* targetSurface_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_SDLWindowBase::unclip"));

  // sanity check(s)
  ACE_ASSERT (screen_);
#if defined (SDL_USE)
  SDL_Surface* surface_p = screen_;
#elif defined (SDL2_USE)
  SDL_Surface* surface_p = SDL_GetWindowSurface (screen_);
#endif // SDL_USE || SDL2_USE
  ACE_ASSERT (surface_p);
  SDL_Surface* target_surface_p = (targetSurface_in ? targetSurface_in
                                                    : surface_p);
  ACE_ASSERT (target_surface_p);

  // restore previous clip rectangle
	SDL_Rect clip_rectangle = clipRectangle_;
  SDL_GetClipRect (target_surface_p, &clipRectangle_);

  SDL_SetClipRect (target_surface_p, &clip_rectangle);
}

void
RPG_Graphics_SDLWindowBase::getArea (SDL_Rect& area_out,
                                     bool topLevel_in) const
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_SDLWindowBase::getArea"));

  if (topLevel_in && parent_)
    return parent_->getArea (area_out,
                             true);

  area_out = clipRectangle_;
}
