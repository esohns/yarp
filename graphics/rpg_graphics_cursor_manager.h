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

#include <map>
#include <limits>

#define _SDL_main_h
#define SDL_main_h_
#include "SDL.h"

#include "ace/Global_Macros.h"
#include "ace/Singleton.h"
#include "ace/Synch_Traits.h"

#include "common_ilock.h"

#include "rpg_map_common.h"

#include "rpg_graphics_common.h"
#include "rpg_graphics_cursor.h"
#include "rpg_graphics_iwindow.h"

class RPG_Graphics_Cursor_Manager
{
  // singleton requires access to the ctor/dtor
  friend class ACE_Singleton<RPG_Graphics_Cursor_Manager,
                             ACE_SYNCH_MUTEX>;

 public:
  // init (clipping of highlight tile)
  void initialize (Common_ILock*,              // screen lock interface handle
                   RPG_Graphics_IWindowBase*); // target window handle
  void reset (bool = false,  // update cursor BG ? : clear
              bool = true,   // locked access ? (debug only)
              bool = false); // debug ?

  inline enum RPG_Graphics_Cursor type () const { return myCurrentType; }
  SDL_Rect area (const RPG_Graphics_Position_t&) const; // viewport (map coords !)
  RPG_Graphics_Position_t position (bool = false) const; // highlight ?

  // draw cursor (and highlight)
  void put (const RPG_Graphics_Position_t&, // cursor position
            const RPG_Graphics_Position_t&, // viewport (map coords !)
            const RPG_Map_Size_t&,          // (current) map size
            SDL_Rect&,                      // return value: "dirty" region
            bool = true,                    // draw highlight ?
            bool = true,                    // locked access ?
            bool = false);                  // debug ?

  void setCursor (const RPG_Graphics_Cursor&, // cursor type
                  SDL_Rect&,                  // return value: "dirty" region
                  bool = true);               // locked access ?
  // draw the cursor
  void putCursor (const RPG_Graphics_Offset_t&, // offset (top left == 0,0)
                  SDL_Rect&,                    // return value: "dirty" region
                  bool = true,                  // locked access ?
                  bool = false);                // debug ?
  // restore/update/clear the BG
  void restoreBG (SDL_Rect&,              // return value: "dirty" region
                  const SDL_Rect* = NULL, // clip area (default: don't clip)
                  bool = true);           // locked access ?
  void updateBG (SDL_Rect&,              // return value: "dirty" region (debug only)
                 const SDL_Rect* = NULL, // clip area (default: clip to map window)
                 bool = true,            // locked access ?
                 bool = false);          // debug ?
  void invalidateBG (const SDL_Rect* = NULL); // clip area (default: don't clip)

  RPG_Graphics_Position_t getHighlightBGPosition(const unsigned int& = std::numeric_limits<unsigned int>::max()) const;
  void putHighlights(const RPG_Map_PositionList_t&,   // positions (map coords !)
                     const RPG_Graphics_Offsets_t&,   // position(s) (screen coords !)
                     const RPG_Graphics_Position_t&,  // viewport (map coords !)
                     SDL_Rect&,                       // return value: "dirty" region
                     bool = true,                     // locked access ?
                     bool = false);                   // debug ?
  void restoreHighlightBG (const RPG_Graphics_Position_t&, // viewport (map coords !)
                           SDL_Rect&,                      // return value: "dirty" region
                           const SDL_Rect* = NULL,         // clip area (default: don't clip)
                           bool = true,                    // locked access ?
                           bool = false);                  // debug ?
  void updateHighlightBG (const RPG_Graphics_Position_t&, // viewport (map coords !)
                          SDL_Rect&,                      // return value: "dirty" region (debug only)
                          const SDL_Rect* = NULL,         // clip area (default: clip to map window)
                          bool = true,                    // locked access ?
                          bool = false);                  // debug ?
  void resetHighlightBG (const RPG_Graphics_Position_t&); // reset to (single) initial position (map coords !)

 private:
  virtual ~RPG_Graphics_Cursor_Manager ();
  RPG_Graphics_Cursor_Manager ();
  ACE_UNIMPLEMENTED_FUNC (RPG_Graphics_Cursor_Manager (const RPG_Graphics_Cursor_Manager&))
  ACE_UNIMPLEMENTED_FUNC (RPG_Graphics_Cursor_Manager& operator= (const RPG_Graphics_Cursor_Manager&))

  // helper types
  typedef std::map<RPG_Graphics_Cursor, SDL_Surface*> RPG_Graphics_Cursor_Cache_t;
  typedef RPG_Graphics_Cursor_Cache_t::const_iterator RPG_Graphics_Cursor_CacheConstIterator_t;

  // helper methods
  //void storeHighlightBG(const RPG_Map_Position_t&,    // position (map coords !)
  //                      const RPG_Graphics_Offset_t&, // position (screen coords !)
  //                      SDL_Rect&,                    // return value: "dirty" region
  //                      const bool& = true);          // locked access ?
  void storeHighlightBG (const RPG_Map_PositionList_t&, // positions (map coords !)
                         const RPG_Graphics_Offsets_t&, // positions (screen coords !)
                         SDL_Rect&,                     // return value: "dirty" region
                         bool = true);                  // locked access ?

  RPG_Graphics_Cursor         myCurrentType;
  SDL_Surface*                myCurrentGraphic;
  RPG_Graphics_Cursor_Cache_t myCache;
  // fast(er) updates
  RPG_Graphics_Position_t     myBGPosition;
  SDL_Surface*                myBG;

  // tile highlight
  RPG_Graphics_IWindowBase*   myHighlightWindow;
  RPG_Graphics_TileCache_t    myHighlightBGCache;
  SDL_Surface*                myHighlightTile;

  Common_ILock*               myScreenLock;
};

typedef ACE_Singleton<RPG_Graphics_Cursor_Manager,
                      ACE_SYNCH_MUTEX> RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON;
//RPG_GRAPHICS_SINGLETON_DECLARE(ACE_Singleton,
//                               RPG_Graphics_Cursor_Manager,
//                               ACE_Recursive_Thread_Mutex);
#endif
