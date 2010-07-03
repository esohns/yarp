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
  RPG_Graphics_SDLWindow(const RPG_Graphics_InterfaceWindow_t&,            // type
                         const RPG_Graphics_Type&,                         // style
                         const std::string&,                               // title
                         const RPG_Graphics_Type& = TYPE_FONT_MAIN_LARGE); // title font
  // embedded ("child") window(s)
  RPG_Graphics_SDLWindow(const RPG_Graphics_SDLWindow&,          // parent
                         const RPG_Graphics_InterfaceWindow_t&); // type
  virtual ~RPG_Graphics_SDLWindow();

  // implement (part of) RPG_Graphics_IWindow
  virtual void refresh(SDL_Surface*); // target surface (screen !)

 protected:
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

  // helper methods
  void getBorders(unsigned long&,        // size (top)
                  unsigned long&,        // size (bottom)
                  unsigned long&,        // size (left)
                  unsigned long&) const; // size (right)
  const RPG_Graphics_SDLWindow* getParent() const;

  // "dirty" region(s)
  RPG_Graphics_DirtyRegions_t      myDirtyRegions;

  // window element graphic(s)
  RPG_Graphics_InterfaceElements_t myElementGraphics;

  // window title
  std::string                      myTitle;
  RPG_Graphics_Type                myTitleFont;

  bool                             myInitialized;

 private:
  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Graphics_SDLWindow());
  ACE_UNIMPLEMENTED_FUNC(RPG_Graphics_SDLWindow(const RPG_Graphics_SDLWindow&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Graphics_SDLWindow& operator=(const RPG_Graphics_SDLWindow&));

  // helper methods
  const bool loadGraphics(const RPG_Graphics_Type&); // style

  const RPG_Graphics_SDLWindow*    myParent;
  RPG_Graphics_InterfaceWindow_t   myType;
  RPG_Graphics_Type                myElementGraphicsType;
};

#endif
