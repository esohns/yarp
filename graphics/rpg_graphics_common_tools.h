/***************************************************************************
 *   Copyright (C) 2009 by Erik Sohns   *
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
#ifndef RPG_GRAPHICS_COMMON_TOOLS_H
#define RPG_GRAPHICS_COMMON_TOOLS_H

#include "rpg_graphics_incl.h"
#include "rpg_graphics_common.h"

#include <png.h>

#include <ace/Global_Macros.h>
#include <ace/Synch.h>

#include <string>

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Graphics_Common_Tools
{
 public:
  // some colors
  static Uint32 CLR32_BLACK;
  static Uint32 CLR32_BLACK_A30;
  static Uint32 CLR32_BLACK_A50;
  static Uint32 CLR32_BLACK_A70;
  static Uint32 CLR32_GREEN;
  static Uint32 CLR32_YELLOW;
  static Uint32 CLR32_ORANGE;
  static Uint32 CLR32_RED;
  static Uint32 CLR32_GRAY20;
  static Uint32 CLR32_GRAY70;
  static Uint32 CLR32_GRAY77;
  static Uint32 CLR32_PURPLE44;
  static Uint32 CLR32_LIGHTPINK;
  static Uint32 CLR32_LIGHTGREEN;
  static Uint32 CLR32_BROWN;
  static Uint32 CLR32_WHITE;
  static Uint32 CLR32_BLESS_BLUE;
  static Uint32 CLR32_CURSE_RED;
  static Uint32 CLR32_GOLD_SHADE;

  // *WARNING*: this needs to be called AFTER SDL_SetVideoMode !
  static void init(const std::string&,      // graphics directory
                   const unsigned long&);   // cache size
  static void fini();
  static SDL_Surface* loadGraphic(const RPG_Graphics_Type&); // graphic
  static void putGraphic(const unsigned long&, // location x
                         const unsigned long&, // location y
                         SDL_Surface*,         // target surface (screen !)
                         const SDL_Surface*);  // graphic
  // *NOTE*: the target image must already be loaded into the framebuffer !
  static void fadeIn(const double&, // interval (seconds)
                     SDL_Surface*); // screen
  static void fadeOut(const double&, // interval (seconds)
                      const Uint32&, // target color
                      SDL_Surface*); // screen

 private:
  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Graphics_Common_Tools());
  ACE_UNIMPLEMENTED_FUNC(~RPG_Graphics_Common_Tools());
  ACE_UNIMPLEMENTED_FUNC(RPG_Graphics_Common_Tools(const RPG_Graphics_Common_Tools&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Graphics_Common_Tools& operator=(const RPG_Graphics_Common_Tools&));

  // helper methods
  // init string conversion (and any other) tables
  static void initStringConversionTables();
  static void initColors();
  static const bool loadPNG(const unsigned char*, // source buffer
                            SDL_Surface*&);       // return value: SDL surface
  static void fade(const double&, // interval (seconds)
                   SDL_Surface*,  // image
                   SDL_Surface*); // screen

  static std::string                  myGraphicsDirectory;
  static ACE_Thread_Mutex             myLock;
  static unsigned long                myOldestCacheEntry;
  static unsigned long                myCacheSize;
  static RPG_Graphics_GraphicsCache_t myGraphicsCache;
  static bool                         myInitialized;
};

#endif
