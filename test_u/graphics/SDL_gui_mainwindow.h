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
#include <rpg_graphics_cursor.h>
#include <rpg_graphics_font.h>
#include <rpg_graphics_image.h>
#include <rpg_graphics_tilegraphic.h>
#include <rpg_graphics_tilesetgraphic.h>
#include <rpg_graphics_graphictypeunion.h>
#include <rpg_graphics_toplevel.h>

#include <rpg_map_common.h>

#include <SDL/SDL.h>

#include <ace/Global_Macros.h>

#include <string>

// forward declarations
class RPG_Engine_Level;

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class SDL_GUI_MainWindow
 : public RPG_Graphics_TopLevel
{
 public:
  SDL_GUI_MainWindow(const RPG_Graphics_Size_t&,                  // size
                     const RPG_Graphics_GraphicTypeUnion&,        // (element) type
                     const std::string&,                          // title
                     const RPG_Graphics_Font& = FONT_MAIN_LARGE); // title font
  virtual ~SDL_GUI_MainWindow();

  // initialize different hotspots/sub-windows
  // *WARNING*: call this AFTER setScreen() !
  void init(RPG_Engine_Level*,               // level state handle
            const RPG_Graphics_MapStyle_t&); // map style

  // implement (part of) RPG_Graphics_IWindow
  virtual void draw(SDL_Surface* = NULL,      // target surface (default: screen)
                    const unsigned int& = 0,  // offset x (top-left = [0,0])
                    const unsigned int& = 0); // offset y (top-left = [0,0])
  virtual void handleEvent(const SDL_Event&,      // event
                           RPG_Graphics_IWindow*, // target window (NULL: this)
                           bool&);                // return value: redraw ?
  virtual void notify(const RPG_Graphics_Cursor&) const;

 private:
  typedef RPG_Graphics_TopLevel inherited;

  // safety measures
  ACE_UNIMPLEMENTED_FUNC(SDL_GUI_MainWindow());
  ACE_UNIMPLEMENTED_FUNC(SDL_GUI_MainWindow(const SDL_GUI_MainWindow&));
  ACE_UNIMPLEMENTED_FUNC(SDL_GUI_MainWindow& operator=(const SDL_GUI_MainWindow&));

  // helper methods
  void initScrollSpots();
  void initMap(RPG_Engine_Level*,               // level state handle
               const RPG_Graphics_MapStyle_t&); // map style
  void drawBorder(SDL_Surface* = NULL,      // target surface (default: screen)
                  const unsigned int& = 0,  // offset x (top-left = [0,0])
                  const unsigned int& = 0); // offset y (top-left = [0,0])
  void drawTitle(const RPG_Graphics_Font&,  // font
                 const std::string&,        // text
                 SDL_Surface* = NULL);      // target surface (default: screen)

  // counter
  unsigned int      myScreenshotIndex;

  unsigned int      myLastHoverTime;
  bool              myHaveMouseFocus;

  RPG_Graphics_Font myTitleFont;
};

#endif
