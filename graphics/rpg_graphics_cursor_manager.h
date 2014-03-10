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

#include "rpg_graphics_exports.h"
#include "rpg_graphics_common.h"
#include "rpg_graphics_cursor.h"
#include "rpg_graphics_iwindow.h"

#include "rpg_map_common.h"

#include "rpg_common_ilock.h"

#include <SDL.h>

#include <ace/Global_Macros.h>
#include <ace/Singleton.h>
#include <ace/Synch.h>

#include <map>

/**
  @author Erik Sohns <erik.sohns@web.de>
 */
class RPG_Graphics_Export RPG_Graphics_Cursor_Manager
{
  // singleton requires access to the ctor/dtor
  friend class ACE_Singleton<RPG_Graphics_Cursor_Manager,
                             ACE_Recursive_Thread_Mutex>;

 public:
  // init (clipping of highlight tile)
  void init(RPG_Common_ILock*,      // screen lock interface handle
            RPG_Graphics_IWindow*); // target window handle
  void reset();

  RPG_Graphics_Cursor type() const;
  RPG_Graphics_Position_t position() const;

  void put(const RPG_Graphics_Position_t&, // viewport (map coords !)
           const RPG_Map_Size_t&,          // (current) map size
           SDL_Rect&);                     // return value: "dirty" region

  void setCursor(const RPG_Graphics_Cursor&, // cursor type
                 SDL_Rect&);                 // return value: "dirty" region
  // draw the cursor
  void putCursor(const unsigned int&,  // offset x (top left == 0,0)
                 const unsigned int&,  // offset y (top left == 0,0)
                 SDL_Rect&,            // return value: "dirty" region
                 const bool& = false); // debug ?
  // update/clear the BG
  void updateBG(const SDL_Rect* = NULL);
  void invalidateBG(const SDL_Rect* = NULL); // clip area (default: don't clip)

  RPG_Graphics_Position_t getHighlightBGPosition(const unsigned int& = std::numeric_limits<unsigned int>::max()) const;
  void putHighlight(const RPG_Map_Position_t&,      // position (map coords !)
                    const RPG_Graphics_Position_t&, // position (screen coords !)
                    const RPG_Graphics_Position_t&, // viewport (map coords !)
                    SDL_Rect&,                      // return value: "dirty" region
                    const bool& = false);           // debug ?
  void putHighlights(const RPG_Map_PositionList_t&,   // positions (map coords !)
                     const RPG_Graphics_Positions_t&, // position(s) (screen coords !)
                     const RPG_Graphics_Position_t&,  // viewport (map coords !)
                     SDL_Rect&);                      // return value: "dirty" region
  void restoreHighlightBG(const RPG_Graphics_Position_t&, // viewport (map coords !)
                          SDL_Rect&);                     // return value: "dirty" region
  void resetHighlightBG(const RPG_Graphics_Position_t&); // reset to (single) initial position (map coords !)

 private:
  virtual ~RPG_Graphics_Cursor_Manager();
  RPG_Graphics_Cursor_Manager();
  ACE_UNIMPLEMENTED_FUNC(RPG_Graphics_Cursor_Manager(const RPG_Graphics_Cursor_Manager&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Graphics_Cursor_Manager& operator=(const RPG_Graphics_Cursor_Manager&));

  // helper types
  typedef std::map<RPG_Graphics_Cursor, SDL_Surface*> RPG_Graphics_Cursor_Cache_t;
  typedef RPG_Graphics_Cursor_Cache_t::const_iterator RPG_Graphics_Cursor_CacheConstIterator_t;

  // helper methods
  // restore the BG
  void restoreBG(SDL_Rect&,               // return value: "dirty" region
                 const SDL_Rect* = NULL); // default: disable window-specific clipping
  void storeHighlightBG(const RPG_Map_Position_t&,      // position (map coords !)
                        const RPG_Graphics_Position_t&, // position (screen coords !)
                        SDL_Rect&);                     // return value: "dirty" region
  void storeHighlightBG(const RPG_Map_PositionList_t&,    // positions (map coords !)
                        const RPG_Graphics_Positions_t&); // positions (screen coords !)

  RPG_Graphics_Cursor         myCurrentType;
  SDL_Surface*                myCurrentGraphic;
  RPG_Graphics_Cursor_Cache_t myCache;
  // fast(er) updates
  RPG_Graphics_Position_t     myBGPosition;
  SDL_Surface*                myBG;

  // tile highlight
  RPG_Graphics_IWindow*       myHighlightWindow;
  RPG_Graphics_TileCache_t    myHighlightBGCache;
  SDL_Surface*                myHighlightTile;

  RPG_Common_ILock*           myScreenLock;
};

typedef ACE_Singleton<RPG_Graphics_Cursor_Manager,
                      ACE_Recursive_Thread_Mutex> RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON;
RPG_GRAPHICS_SINGLETON_DECLARE(ACE_Singleton,
                               RPG_Graphics_Cursor_Manager,
                               ACE_Recursive_Thread_Mutex);
#endif
