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

#ifndef RPG_GRAPHICS_SDL_WINDOW_BASE_H
#define RPG_GRAPHICS_SDL_WINDOW_BASE_H

#include <string>

#define _SDL_main_h
#define SDL_main_h_
#include "SDL.h"

#include "ace/Global_Macros.h"

#include "common_ilock.h"

#include "rpg_graphics_common.h"
#include "rpg_graphics_colorname.h"
#include "rpg_graphics_iwindow.h"

class RPG_Graphics_SDLWindowBase
 : public RPG_Graphics_IWindow
{
 public:
  virtual ~RPG_Graphics_SDLWindowBase ();

  // implement (part of) RPG_Graphics_IWindow
  virtual void show (struct SDL_Rect&); // return value: "dirty" region
  virtual void hide (struct SDL_Rect&); // return value: "dirty" region
  inline virtual bool visible () const { return true; }

  virtual void clear (enum RPG_Graphics_ColorName = COLOR_BLACK, // color
                      bool = true);                              // clip ?

  virtual RPG_Graphics_IWindowBase* child (enum RPG_Graphics_WindowType); // type

  inline virtual void notify (enum RPG_Graphics_Cursor) const {ACE_ASSERT (false); ACE_NOTREACHED (return;) }

  // implement (part of) RPG_Graphics_IWindowBase
  virtual void initialize (Common_ILock* = NULL, // screen lock interface handle
                           bool = false);        // double-buffered screen ?

  virtual void clip (SDL_Surface* = NULL, // target surface (default: screen)
                     unsigned int = 0,    // offset x (top-left = [0,0])
                     unsigned int = 0);   // offset y (top-left = [0,0]));
  virtual void unclip (SDL_Surface* = NULL); // target surface (default: screen)

  virtual void update (SDL_Surface* = NULL); // target surface (default: screen)
  virtual void invalidate (const struct SDL_Rect&); // "dirty" area

#if defined (SDL_USE)
  inline virtual void setScreen (SDL_Surface* screen_in) { ACE_ASSERT (screen_in); screen_ = screen_in; }
  inline virtual SDL_Surface* getScreen () const { return (screen_ ? screen_ : SDL_GetVideoSurface ()); }
#elif defined (SDL2_USE)
  inline virtual void setRenderer (SDL_Renderer* renderer_in) { ACE_ASSERT (renderer_in); renderer_ = renderer_in; }
  inline virtual void setScreen (SDL_Window* window_in) { ACE_ASSERT (window_in); screen_ = window_in; }
  inline virtual SDL_Renderer* getRenderer () const { return renderer_; }
  inline virtual SDL_Window* getScreen () const { return screen_; }
#endif // SDL_USE || SDL2_USE

  inline virtual enum RPG_Graphics_WindowType getType () const { return type_; }

  virtual void getArea (struct SDL_Rect&,    // return value: window area
                        bool = false) const; // toplevel ?

  virtual void handleEvent (const SDL_Event&,          // event
                            RPG_Graphics_IWindowBase*, // target window (NULL: this)
                            struct SDL_Rect&);         // return value: "dirty" region

  virtual RPG_Graphics_IWindowBase* getWindow (const RPG_Graphics_Position_t&) const; // position (e.g. mouse-)

  virtual void getBorders (unsigned int&,      // return value: size (top)
                           unsigned int&,      // return value: size (bottom)
                           unsigned int&,      // return value: size (left)
                           unsigned int&,      // return value: size (right)
                           bool = true) const; // recursive ?

 protected:
  // *NOTE*: window assumes responsibility for its background surface
  RPG_Graphics_SDLWindowBase (enum RPG_Graphics_WindowType, // type
                              const RPG_Graphics_Size_t&,   // size
                              const std::string&);          // title
  // embedded ("child") window(s)
  // *NOTE*: window assumes responsibility for its background surface
  RPG_Graphics_SDLWindowBase (enum RPG_Graphics_WindowType,      // type
                              const RPG_Graphics_SDLWindowBase&, // parent
                              // *NOTE*: offset doesn't include any border(s) !
                              const RPG_Graphics_Offset_t&,      // offset
                              const std::string&);               // title

  virtual SDL_Rect getDirty () const; // "dirty" area
  void clean ();
  // implement (part of) RPG_Graphics_IWindowBase
  virtual void refresh (SDL_Surface* = NULL); // target surface (default: screen)

  // default screen
#if defined (SDL_USE)
  SDL_Surface*                  screen_;
#elif defined (SDL2_USE)
  SDL_Renderer*                 renderer_;
  SDL_Window*                   screen_;
#endif // SDL_USE || SDL2_USE
  Common_ILock*                 screenLock_;

  // border sizes
  unsigned int                  borderTop_;
  unsigned int                  borderBottom_;
  unsigned int                  borderLeft_;
  unsigned int                  borderRight_;

  // helper types
  typedef std::vector<struct SDL_Rect> RPG_Graphics_DirtyRegions_t;
  typedef RPG_Graphics_DirtyRegions_t::const_iterator RPG_Graphics_DirtyRegionsConstIterator_t;
  typedef std::vector<RPG_Graphics_IWindowBase*> RPG_Graphics_Windows_t;
  typedef RPG_Graphics_Windows_t::const_iterator RPG_Graphics_WindowsConstIterator_t;
  typedef RPG_Graphics_Windows_t::iterator RPG_Graphics_WindowsIterator_t;
  typedef RPG_Graphics_Windows_t::const_reverse_iterator RPG_Graphics_WindowsRIterator_t;

  // helper methods
  inline virtual RPG_Graphics_IWindowBase* getParent () const { return parent_; }

  std::string                   title_;

  RPG_Graphics_IWindowBase*     parent_;
  RPG_Graphics_Windows_t        children_;

  RPG_Graphics_Position_t       lastAbsolutePosition_;
  struct SDL_Rect               clipRectangle_;

 private:
  // helper types
  typedef std::vector<struct SDL_Rect> RPG_Graphics_InvalidRegions_t;
  typedef RPG_Graphics_InvalidRegions_t::const_iterator RPG_Graphics_InvalidRegionsConstIterator_t;

  ACE_UNIMPLEMENTED_FUNC (RPG_Graphics_SDLWindowBase ())
  ACE_UNIMPLEMENTED_FUNC (RPG_Graphics_SDLWindowBase (const RPG_Graphics_SDLWindowBase&))
  ACE_UNIMPLEMENTED_FUNC (RPG_Graphics_SDLWindowBase& operator= (const RPG_Graphics_SDLWindowBase&))

  // helper methods
  virtual void addChild (RPG_Graphics_IWindowBase*); // window handle
  virtual void removeChild (RPG_Graphics_IWindowBase*); // window handle

  // "dirty" region(s)
  RPG_Graphics_InvalidRegions_t invalidRegions_;
  bool                          flip_;

  enum RPG_Graphics_WindowType  type_;
};

#endif
