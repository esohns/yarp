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
#ifndef RPG_GRAPHICS_SDLWINDOW_H
#define RPG_GRAPHICS_SDLWINDOW_H

#include "rpg_graphics_iwindow.h"
#include "rpg_graphics_common.h"
#include "rpg_graphics_type.h"

#include <SDL/SDL.h>

#include <ace/Global_Macros.h>

#include <string>

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Graphics_SDLWindow
 : public RPG_Graphics_IWindow
{
 public:
  virtual ~RPG_Graphics_SDLWindow();

  // implement (part of) RPG_Graphics_IWindow
  virtual void refresh(SDL_Surface*); // target surface (screen !)
  virtual void handleEvent(const SDL_Event&, // event
                           bool&);           // return value: redraw ?

  const RPG_Graphics_WindowSize_t getSize(const bool& = false) const; // top-level ?
  RPG_Graphics_IWindow* getWindow(const RPG_Graphics_Position_t&); // position (e.g. mouse-)

 protected:
  RPG_Graphics_SDLWindow(const RPG_Graphics_WindowSize_t&,                 // size
                         const RPG_Graphics_WindowType&,                   // type
                         const std::string&,                               // title
                         const RPG_Graphics_Type& = TYPE_FONT_MAIN_LARGE); // title font
  // embedded ("child") window(s)
  RPG_Graphics_SDLWindow(const RPG_Graphics_WindowType&, // type
                         const RPG_Graphics_SDLWindow&,  // parent
                         // *NOTE*: offset doesn't include any border(s) !
                         const RPG_Graphics_Offset_t&);  // offset

  // absolute size
  RPG_Graphics_WindowSize_t        mySize;

  // border sizes
  unsigned long                    myBorderTop;
  unsigned long                    myBorderBottom;
  unsigned long                    myBorderLeft;
  unsigned long                    myBorderRight;

//   // title sizes
//   unsigned long                    myTitleWidth;
//   unsigned long                    myTitleHeight;

  // helper types
  typedef std::vector<SDL_Rect> RPG_Graphics_DirtyRegions_t;
  typedef RPG_Graphics_DirtyRegions_t::const_iterator RPG_Graphics_DirtyRegionsConstIterator_t;
  typedef std::vector<RPG_Graphics_SDLWindow*> RPG_Graphics_Windows_t;
  typedef RPG_Graphics_Windows_t::const_iterator RPG_Graphics_WindowsConstIterator_t;
  typedef RPG_Graphics_Windows_t::iterator RPG_Graphics_WindowsIterator_t;
  typedef RPG_Graphics_Windows_t::const_reverse_iterator RPG_Graphics_WindowsRIterator_t;

  // helper methods
  void getBorders(unsigned long&,        // size (top)
                  unsigned long&,        // size (bottom)
                  unsigned long&,        // size (left)
                  unsigned long&) const; // size (right)
  const RPG_Graphics_SDLWindow* getParent() const;

  // "dirty" region(s)
  RPG_Graphics_DirtyRegions_t      myDirtyRegions;

  // window title
  std::string                      myTitle;
  RPG_Graphics_Type                myTitleFont;

  RPG_Graphics_Offset_t            myOffset; // offset to parent
  RPG_Graphics_Windows_t           myChildren;

  RPG_Graphics_Position_t          myLastAbsolutePosition;

 private:
  typedef RPG_Graphics_IWindow inherited;

  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Graphics_SDLWindow());
  ACE_UNIMPLEMENTED_FUNC(RPG_Graphics_SDLWindow(const RPG_Graphics_SDLWindow&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Graphics_SDLWindow& operator=(const RPG_Graphics_SDLWindow&));

  // helper methods
  void addChild(RPG_Graphics_SDLWindow*);
  void removeChild(RPG_Graphics_SDLWindow*);

  RPG_Graphics_SDLWindow*          myParent;
  RPG_Graphics_WindowType          myType;
};

#endif
