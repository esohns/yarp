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
#ifndef SDL_GUI_MAINWINDOW_H
#define SDL_GUI_MAINWINDOW_H

#include <rpg_graphics_common.h>
#include <rpg_graphics_type.h>
#include <rpg_graphics_toplevel.h>

#include <SDL/SDL.h>

#include <ace/Global_Macros.h>

#include <string>

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class SDL_GUI_MainWindow
 : public RPG_Graphics_TopLevel
{
 public:
  SDL_GUI_MainWindow(const RPG_Graphics_WindowSize_t&,                 // size
                     const RPG_Graphics_Type&,                         // style
                     const std::string&,                               // title
                     const RPG_Graphics_Type& = TYPE_FONT_MAIN_LARGE); // title font
  virtual ~SDL_GUI_MainWindow();

  virtual void draw(SDL_Surface*,                  // target surface (screen !)
                    const RPG_Graphics_Offset_t&); // offset
  virtual void handleEvent(const SDL_Event&,      // event
                           RPG_Graphics_IWindow*, // target window (NULL: this)
                           bool&);                // return value: redraw ?

 private:
  typedef RPG_Graphics_TopLevel inherited;

  // safety measures
  ACE_UNIMPLEMENTED_FUNC(SDL_GUI_MainWindow());
  ACE_UNIMPLEMENTED_FUNC(SDL_GUI_MainWindow(const SDL_GUI_MainWindow&));
  ACE_UNIMPLEMENTED_FUNC(SDL_GUI_MainWindow& operator=(const SDL_GUI_MainWindow&));

  // helper methods
  void initScrollSpots();

  // counter
  static unsigned long screenshot_index;

  unsigned long myLastHoverTime;
  bool          myHaveMouseFocus;
};

#endif
