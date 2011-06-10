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
#ifndef RPG_GRAPHICS_CURSOR_MANAGER_H
#define RPG_GRAPHICS_CURSOR_MANAGER_H

#include "rpg_graphics_common.h"
#include "rpg_graphics_cursor.h"

#include <SDL/SDL.h>

#include <ace/Global_Macros.h>
#include <ace/Singleton.h>
#include <ace/Synch.h>

#include <map>

/**
  @author Erik Sohns <erik.sohns@web.de>
 */
class RPG_Graphics_Cursor_Manager
{
  // singleton requires access to the ctor/dtor
  friend class ACE_Singleton<RPG_Graphics_Cursor_Manager, ACE_Thread_Mutex>;

 public:
  const RPG_Graphics_Cursor type() const;

  void set(const RPG_Graphics_Cursor&); // cursor type
//   SDL_Surface* get() const;

  // draw the cursor
  void put(const unsigned long&, // offset x (top left == 0,0)
           const unsigned long&, // offset y (top left == 0,0)
           SDL_Surface*,         // target surface (e.g. screen)
           SDL_Rect&);           // return value: "dirty" region

  // restore the BG
  void restoreBG(SDL_Surface*, // target surface (e.g. screen)
                 SDL_Rect&);   // return value: "dirty" region
  // clear the BG
  void invalidateBG();

 private:
  // safety measures
  virtual ~RPG_Graphics_Cursor_Manager();
  ACE_UNIMPLEMENTED_FUNC(RPG_Graphics_Cursor_Manager());
  ACE_UNIMPLEMENTED_FUNC(RPG_Graphics_Cursor_Manager(const RPG_Graphics_Cursor_Manager&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Graphics_Cursor_Manager& operator=(const RPG_Graphics_Cursor_Manager&));

  // helper types
  typedef std::map<RPG_Graphics_Cursor, SDL_Surface*> RPG_Graphics_Cursor_Cache_t;
  typedef RPG_Graphics_Cursor_Cache_t::const_iterator RPG_Graphics_Cursor_CacheConstIterator_t;

  RPG_Graphics_Cursor         myCurrentType;
  SDL_Surface*                myCurrentGraphic;

  // fast(er) updates
  RPG_Graphics_Position_t     myBGPosition;
  SDL_Surface*                myBG;

  RPG_Graphics_Cursor_Cache_t myCache;
};

typedef ACE_Singleton<RPG_Graphics_Cursor_Manager,
                      ACE_Thread_Mutex> RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON;

#endif
