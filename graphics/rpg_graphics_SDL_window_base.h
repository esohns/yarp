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

#include "rpg_graphics_exports.h"
#include "rpg_graphics_iwindow.h"
#include "rpg_graphics_common.h"
#include "rpg_graphics_colorname.h"

#include "rpg_common_ilock.h"

#include <SDL.h>

#include <ace/Global_Macros.h>

#include <string>

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Graphics_Export RPG_Graphics_SDLWindowBase
 : public RPG_Graphics_IWindow
{
 public:
  virtual ~RPG_Graphics_SDLWindowBase();

  // implement (part of) RPG_Graphics_IWindow
  virtual void show(SDL_Rect&); // return value: "dirty" region
  virtual void hide(SDL_Rect&); // return value: "dirty" region
  virtual void clear(const RPG_Graphics_ColorName& = COLOR_BLACK,
                     const bool& = true);

  virtual RPG_Graphics_IWindowBase* child(const RPG_Graphics_WindowType&); // type

  virtual void notify(const RPG_Graphics_Cursor&) const;

  // implement (part of) RPG_Graphics_IWindowBase
  virtual void init(RPG_Common_ILock* = NULL, // screen lock interface handle
                    const bool& = false);     // double-buffered screen ?

  virtual void clip(SDL_Surface* = NULL,      // target surface (default: screen)
                    const unsigned int& = 0,  // offset x (top-left = [0,0])
                    const unsigned int& = 0); // offset y (top-left = [0,0]));
  virtual void unclip(SDL_Surface* = NULL); // target surface (default: screen)

  virtual void update(SDL_Surface* = NULL); // target surface (default: screen)
  virtual void invalidate(const SDL_Rect&); // "dirty" area

  virtual void setScreen(SDL_Surface*);   // (default) screen
  virtual SDL_Surface* getScreen() const; // (default) screen

  virtual RPG_Graphics_WindowType getType() const;

  virtual void getArea(SDL_Rect&,                  // return value: window area
		                   const bool& = false) const; // toplevel ?

  virtual void handleEvent(const SDL_Event&,          // event
                           RPG_Graphics_IWindowBase*, // target window (NULL: this)
                           SDL_Rect&);                // return value: "dirty" region

  virtual RPG_Graphics_IWindowBase* getWindow(const RPG_Graphics_Position_t&) const; // position (e.g. mouse-)

  virtual void getBorders(unsigned int&,             // return value: size (top)
                          unsigned int&,             // return value: size (bottom)
                          unsigned int&,             // return value: size (left)
                          unsigned int&,             // return value: size (right)
                          const bool& = true) const; // recursive ?

 protected:
  // *NOTE*: window assumes responsibility for its background surface
  RPG_Graphics_SDLWindowBase(const RPG_Graphics_WindowType&, // type
		                         const RPG_Graphics_Size_t&,     // size
                             const std::string&);            // title
//                              SDL_Surface* = NULL);             // background
  // embedded ("child") window(s)
  // *NOTE*: window assumes responsibility for its background surface
  RPG_Graphics_SDLWindowBase(const RPG_Graphics_WindowType&,    // type
                             const RPG_Graphics_SDLWindowBase&, // parent
                             // *NOTE*: offset doesn't include any border(s) !
                             const RPG_Graphics_Offset_t&,      // offset
                             const std::string&);               // title
//                              SDL_Surface* = NULL);              // background

  virtual SDL_Rect getDirty() const; // "dirty" area
  void clean();

  // default screen
  SDL_Surface*                     myScreen;
  RPG_Common_ILock*                myScreenLock;

  // border sizes
  unsigned int                     myBorderTop;
  unsigned int                     myBorderBottom;
  unsigned int                     myBorderLeft;
  unsigned int                     myBorderRight;

//   // title sizes
//   unsigned int                    myTitleWidth;
//   unsigned int                    myTitleHeight;

  // helper types
  typedef std::vector<SDL_Rect> RPG_Graphics_DirtyRegions_t;
  typedef RPG_Graphics_DirtyRegions_t::const_iterator RPG_Graphics_DirtyRegionsConstIterator_t;
  typedef std::vector<RPG_Graphics_IWindowBase*> RPG_Graphics_Windows_t;
  typedef RPG_Graphics_Windows_t::const_iterator RPG_Graphics_WindowsConstIterator_t;
  typedef RPG_Graphics_Windows_t::iterator RPG_Graphics_WindowsIterator_t;
  typedef RPG_Graphics_Windows_t::const_reverse_iterator RPG_Graphics_WindowsRIterator_t;

  // helper methods
  virtual RPG_Graphics_IWindowBase* getParent() const;

  std::string                      myTitle;
//   SDL_Surface*                     myBackGround;

  RPG_Graphics_IWindowBase*        myParent;
  RPG_Graphics_Windows_t           myChildren;

  RPG_Graphics_Position_t          myLastAbsolutePosition;
  SDL_Rect                         myClipRect;

 private:
  typedef RPG_Graphics_IWindow inherited;

  // helper types
  typedef std::vector<SDL_Rect> RPG_Graphics_InvalidRegions_t;
  typedef RPG_Graphics_InvalidRegions_t::const_iterator RPG_Graphics_InvalidRegionsConstIterator_t;

  ACE_UNIMPLEMENTED_FUNC(RPG_Graphics_SDLWindowBase());
  ACE_UNIMPLEMENTED_FUNC(RPG_Graphics_SDLWindowBase(const RPG_Graphics_SDLWindowBase&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Graphics_SDLWindowBase& operator=(const RPG_Graphics_SDLWindowBase&));

  // helper methods
  virtual void addChild(RPG_Graphics_IWindowBase*); // window handle
  virtual void removeChild(RPG_Graphics_IWindowBase*); // window handle

  // "dirty" region(s)
  RPG_Graphics_InvalidRegions_t    myInvalidRegions;
  bool                             myFlip;

  RPG_Graphics_WindowType          myType;
};

#endif
