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

#ifndef RPG_CLIENT_ENTITY_MANAGER_H
#define RPG_CLIENT_ENTITY_MANAGER_H

#include <map>

#define _SDL_main_h
#define SDL_main_h_
#include "SDL.h"

#include "ace/Global_Macros.h"
#include "ace/Singleton.h"
#include "ace/Synch_Traits.h"

#include "common_ilock.h"

#include "rpg_graphics_common.h"
#include "rpg_graphics_iwindow.h"

#include "rpg_engine_common.h"

class RPG_Client_Entity_Manager
{
  // singleton requires access to the ctor/dtor
  friend class ACE_Singleton<RPG_Client_Entity_Manager,
                             ACE_SYNCH_MUTEX>;

 public:
  // init (clipping)
  void initialize (Common_ILock*,              // screen lock interface handle
                   RPG_Graphics_IWindowBase*); // (target) window handle

  // manage entities
  void add (RPG_Engine_EntityID_t, // id
            SDL_Surface*,          // graphic handle
            bool = false);         // free on remove() ?
  void remove (RPG_Engine_EntityID_t, // id
               struct SDL_Rect&,      // return value: "dirty" region
               bool = true,           // locked access ?
               bool = false);         // debug ?
  bool cached (RPG_Engine_EntityID_t) const; // id

  // draw the entity
  void put (RPG_Engine_EntityID_t,          // id
            const RPG_Graphics_Position_t&, // position (screen coordinates !)
            struct SDL_Rect&,               // return value: "dirty" region
            bool,                           // clip window ?
            bool = true,                    // locked access ?
            bool = false);                  // debug ?

  // clear the stored BG
  void invalidateBG (RPG_Engine_EntityID_t); // id

 private:
  virtual ~RPG_Client_Entity_Manager ();
  RPG_Client_Entity_Manager ();
  ACE_UNIMPLEMENTED_FUNC (RPG_Client_Entity_Manager (const RPG_Client_Entity_Manager&))
  ACE_UNIMPLEMENTED_FUNC (RPG_Client_Entity_Manager& operator= (const RPG_Client_Entity_Manager&))

  // helper types
  struct RPG_Client_EntityCacheEntry
  {
    SDL_Surface*            graphic;
    bool                    free_on_remove;
    SDL_Surface*            bg;
    RPG_Graphics_Position_t bg_position;
  };
  typedef std::map<RPG_Engine_EntityID_t, struct RPG_Client_EntityCacheEntry> RPG_Client_EntityCache_t;
  typedef RPG_Client_EntityCache_t::iterator RPG_Client_EntityCacheIterator_t;
  typedef RPG_Client_EntityCache_t::const_iterator RPG_Client_EntityCacheConstIterator_t;

  // helper methods
  // restore the BG
  void restoreBG (RPG_Engine_EntityID_t, // id
                  struct SDL_Rect&,      // return value: "dirty" region
                  bool,                  // clip window ?
                  bool = true,           // locked access ?
                  bool = false);         // debug ?

  Common_ILock*             myScreenLock;
  RPG_Graphics_IWindowBase* myWindow;
  RPG_Client_EntityCache_t  myCache;
};

typedef ACE_Singleton<RPG_Client_Entity_Manager,
                      ACE_SYNCH_MUTEX> RPG_CLIENT_ENTITY_MANAGER_SINGLETON;
//RPG_CLIENT_SINGLETON_DECLARE(ACE_Singleton,
//                             RPG_Client_Entity_Manager,
//                             ACE_Thread_Mutex);
#endif
