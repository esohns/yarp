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
#include "stdafx.h"

#include "rpg_client_entity_manager.h"

// *NOTE*: work around quirky MSVC...
#if defined (_MSC_VER)
#define NOMINMAX
#endif // _MSC_VER

#include <limits>

#include "ace/OS.h"
#include "ace/Log_Msg.h"

#include "common_macros.h"

#include "rpg_common_macros.h"

#include "rpg_graphics_defines.h"
#include "rpg_graphics_surface.h"
#include "rpg_graphics_cursor_manager.h"
#include "rpg_graphics_SDL_tools.h"

#include "rpg_client_iwindow_level.h"

RPG_Client_Entity_Manager::RPG_Client_Entity_Manager ()
 : myScreenLock (NULL),
   myWindow (NULL),
   myCache ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Entity_Manager::RPG_Client_Entity_Manager"));

}

RPG_Client_Entity_Manager::~RPG_Client_Entity_Manager ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Entity_Manager::~RPG_Client_Entity_Manager"));

  // clean up
  for (RPG_Client_EntityCacheConstIterator_t iterator = myCache.begin ();
       iterator != myCache.end ();
       iterator++)
  {
    if ((*iterator).second.free_on_remove)
#if defined (SDL_USE) || defined (SDL2_USE)
      SDL_FreeSurface ((*iterator).second.graphic);
#elif defined (SDL3_USE)
      SDL_DestroySurface ((*iterator).second.graphic);
#endif // SDL_USE || SDL2_USE || SDL3_USE
#if defined (SDL_USE) || defined (SDL2_USE)
    SDL_FreeSurface ((*iterator).second.bg);
#elif defined (SDL3_USE)
    SDL_DestroySurface ((*iterator).second.bg);
#endif // SDL_USE || SDL2_USE || SDL3_USE
  } // end FOR
}

void
RPG_Client_Entity_Manager::initialize (Common_ILock* screenLock_in,
                                       RPG_Graphics_IWindowBase* window_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Entity_Manager::initialize"));

  ACE_ASSERT (window_in);

  myScreenLock = screenLock_in;
  myWindow = window_in;
}

void
RPG_Client_Entity_Manager::add (RPG_Engine_EntityID_t id_in,
                                SDL_Surface* surface_in,
                                bool free_on_remove_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Entity_Manager::add"));

  // sanity checks
  ACE_ASSERT (surface_in);
  RPG_Client_EntityCacheConstIterator_t iterator = myCache.find (id_in);
  ACE_ASSERT (iterator == myCache.end ());
  //if (iterator != myCache.end ())
  //{
  //  ACE_DEBUG ((LM_ERROR,
  //              ACE_TEXT ("entity ID %u already cached, returning\n"),
  //              id_in));

  //  // clean up
  //  if (free_on_remove_in)
  //    SDL_FreeSurface (surface_in);

  //  return;
  //} // end IF

  struct RPG_Client_EntityCacheEntry new_entry;
  new_entry.graphic = surface_in;
  new_entry.free_on_remove = free_on_remove_in;
  new_entry.bg =
    RPG_Graphics_Surface::create (static_cast<unsigned int> (surface_in->w),
                                  static_cast<unsigned int> (surface_in->h));
  if (!new_entry.bg)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to RPG_Graphics_Surface::create(%d,%d), returning\n"),
                surface_in->w, surface_in->h));

    // clean up
    if (free_on_remove_in)
#if defined (SDL_USE) || defined (SDL2_USE)
      SDL_FreeSurface (surface_in);
#elif defined (SDL3_USE)
      SDL_DestroySurface (surface_in);
#endif // SDL_USE || SDL2_USE || SDL3_USE

    return;
  } // end IF
  new_entry.bg_position =
    std::make_pair (std::numeric_limits<unsigned int>::max (),
                    std::numeric_limits<unsigned int>::max ());

  myCache[id_in] = new_entry;
}

void
RPG_Client_Entity_Manager::remove (RPG_Engine_EntityID_t id_in,
                                   struct SDL_Rect& dirtyRegion_out,
                                   bool lockedAccess_in,
                                   bool debug_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Entity_Manager::remove"));

  // initialize return value(s)
  ACE_OS::memset (&dirtyRegion_out, 0, sizeof (struct SDL_Rect));

  // sanity check(s)
  RPG_Client_EntityCacheConstIterator_t iterator = myCache.find (id_in);
  ACE_ASSERT (iterator != myCache.end ());
  //if (iterator == myCache.end ())
  //{
  //  ACE_DEBUG ((LM_ERROR,
  //              ACE_TEXT ("invalid entity ID (was: %u), returning\n"),
  //              id_in));
  //  return;
  //} // end IF

  // step0: restore old background
  if (myScreenLock && lockedAccess_in)
    myScreenLock->lock ();
  restoreBG (id_in,
             dirtyRegion_out,
             true, // clip window ?
             false, // locked access ? (screen lock)
             debug_in); // debug ?
  if (myScreenLock && lockedAccess_in)
    myScreenLock->unlock ();

  // clean up
  if ((*iterator).second.free_on_remove)
#if defined (SDL_USE) || defined (SDL2_USE)
    SDL_FreeSurface ((*iterator).second.graphic);
#elif defined (SDL3_USE)
    SDL_DestroySurface ((*iterator).second.graphic);
#endif // SDL_USE || SDL2_USE || SDL3_USE
#if defined (SDL_USE) || defined (SDL2_USE)
  SDL_FreeSurface ((*iterator).second.bg);
#elif defined (SDL3_USE)
  SDL_DestroySurface ((*iterator).second.bg);
#endif // SDL_USE || SDL2_USE || SDL3_USE

  myCache.erase ((*iterator).first);
}

bool
RPG_Client_Entity_Manager::cached (RPG_Engine_EntityID_t id_in) const
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Entity_Manager::cached"));

  RPG_Client_EntityCacheConstIterator_t iterator = myCache.find (id_in);

  return (iterator != myCache.end ());
}

void
RPG_Client_Entity_Manager::put (RPG_Engine_EntityID_t id_in,
                                const RPG_Graphics_Position_t& position_in,
                                struct SDL_Rect& dirtyRegion_out,
                                bool clipWindow_in,
                                bool lockedAccess_in,
                                bool debug_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Entity_Manager::put"));

  // initialize return value(s)
  ACE_OS::memset (&dirtyRegion_out, 0, sizeof (struct SDL_Rect));

  // sanity check(s)
  ACE_ASSERT (myWindow);
#if defined (SDL_USE)
  SDL_Surface* target_surface = myWindow->getScreen ();
#elif defined (SDL2_USE) || defined (SDL3_USE)
  SDL_Surface* target_surface = SDL_GetWindowSurface (myWindow->getScreen ());
#endif // SDL_USE || SDL2_USE || SDL3_USE
  ACE_ASSERT (target_surface);
  RPG_Client_EntityCacheIterator_t iterator = myCache.find (id_in);
  if (iterator == myCache.end ())
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid entity ID (was: %u), returning\n"),
                id_in));
    return;
  } // end IF

  // step1: restore old background
  if (myScreenLock && lockedAccess_in)
    myScreenLock->lock ();
  restoreBG (id_in,
             dirtyRegion_out,
             clipWindow_in,
             false, // locked access ?
             debug_in);

  // step2: get new background
  // step2a: restore cursor / highlight bg first
  struct SDL_Rect clip_rectangle, dirty_region;
  // *NOTE*: entities are drawn "centered" on the floor tile
  clip_rectangle.x = (position_in.first +
                      ((RPG_GRAPHICS_TILE_FLOOR_WIDTH -
                      (*iterator).second.graphic->w) / 2));
  clip_rectangle.y = (position_in.second +
                      (RPG_GRAPHICS_TILE_FLOOR_HEIGHT / 2) -
                      (*iterator).second.graphic->h);
  clip_rectangle.w = (*iterator).second.graphic->w;
  clip_rectangle.h = (*iterator).second.graphic->h;
  RPG_Client_IWindowLevel* window = NULL;
  try {
    window = dynamic_cast<RPG_Client_IWindowLevel*> (myWindow);
  } catch (...) {
    window = NULL;
  }
  if (!window)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to dynamic_cast<RPG_Client_IWindowLevel*>(%@), returning\n"),
                myWindow));
    if (myScreenLock && lockedAccess_in)
      myScreenLock->unlock ();
    return;
  } // end IF

  // step2b: load bg image
  RPG_Graphics_Position_t screen_coordinates = std::make_pair (clip_rectangle.x,
                                                               clip_rectangle.y);

  RPG_Graphics_Surface::get (screen_coordinates,
                             *target_surface,
                             *(*iterator).second.bg);
  (*iterator).second.bg_position = screen_coordinates;

  // step3: paint sprite graphic

  // compute dirty area
  dirty_region = clip_rectangle;
//  // *NOTE*: as it is (see implementation of clip()), this wouldn't work, as
  // the target surface (myWindow) is already clipped to the SDL window area...
//  myWindow->getArea(clip_rectangle, false);
  clip_rectangle = target_surface->clip_rect;
  dirty_region = RPG_Graphics_SDL_Tools::intersect (dirty_region,
                                                    clip_rectangle);

  // place graphic
  if (clipWindow_in)
    myWindow->clip (target_surface, 0, 0);
  RPG_Graphics_Surface::put (screen_coordinates,
                             *(*iterator).second.graphic,
                             target_surface,
                             dirty_region);
  if (lockedAccess_in && myScreenLock)
    myScreenLock->unlock ();
  if (clipWindow_in)
    myWindow->unclip (target_surface);
  dirtyRegion_out = RPG_Graphics_SDL_Tools::boundingBox (dirty_region,
                                                         dirtyRegion_out);

  // update cursor / highlight(s)
  RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance ()->updateBG (dirty_region,
                                                                &dirtyRegion_out,
                                                                lockedAccess_in,
                                                                debug_in);
  dirtyRegion_out = RPG_Graphics_SDL_Tools::boundingBox (dirty_region,
                                                         dirtyRegion_out);

  RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance ()->updateHighlightBG (window->getView (),
                                                                         dirty_region,
                                                                         &dirtyRegion_out,
                                                                         lockedAccess_in,
                                                                         debug_in);
  dirtyRegion_out = RPG_Graphics_SDL_Tools::boundingBox (dirty_region,
                                                         dirtyRegion_out);

  if (debug_in)
  {
    // show entity bg in bottom right corner
    ACE_ASSERT (target_surface->w >= (*iterator).second.bg->w);
    ACE_ASSERT (target_surface->h >= (*iterator).second.bg->h);

#if defined (SDL_USE)
    RPG_Graphics_Surface::unclip ();
#elif defined (SDL2_USE) || defined (SDL3_USE)
    RPG_Graphics_Surface::unclip (myWindow->getScreen ());
#endif // SDL_USE || SDL2_USE || SDL3_USE
    RPG_Graphics_Surface::put (std::make_pair ((target_surface->w - (*iterator).second.bg->w),
                                               (target_surface->h - (*iterator).second.bg->h)),
                               *(*iterator).second.bg,
                               target_surface,
                               dirty_region);
#if defined (SDL_USE)
    RPG_Graphics_Surface::clip ();
#elif defined (SDL2_USE) || defined (SDL3_USE)
    RPG_Graphics_Surface::clip (myWindow->getScreen ());
#endif // SDL_USE || SDL2_USE || SDL3_USE
    dirtyRegion_out = RPG_Graphics_SDL_Tools::boundingBox (dirty_region,
                                                           dirtyRegion_out);
  } // end IF
}

void
RPG_Client_Entity_Manager::restoreBG (RPG_Engine_EntityID_t id_in,
                                      struct SDL_Rect& dirtyRegion_out,
                                      bool clipWindow_in,
                                      bool lockedAccess_in,
                                      bool debug_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Entity_Manager::restoreBG"));

  // sanity check(s)
  ACE_ASSERT (myWindow);
#if defined (SDL_USE)
  SDL_Surface* target_surface = myWindow->getScreen();
#elif defined (SDL2_USE) || defined (SDL3_USE)
  SDL_Surface* target_surface = SDL_GetWindowSurface (myWindow->getScreen ());
#endif // SDL_USE || SDL2_USE || SDL3_USE
  ACE_ASSERT (target_surface);
  RPG_Client_EntityCacheConstIterator_t iterator = myCache.find (id_in);
  if (iterator == myCache.end ())
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid entity ID (was: %u), returning\n"),
                id_in));
    return;
  } // end IF
  if ((*iterator).second.bg_position ==
      std::make_pair (std::numeric_limits<unsigned int>::max (),
                      std::numeric_limits<unsigned int>::max ()))
    return; // nothing to do

  dirtyRegion_out.x =
      static_cast<int16_t> ((*iterator).second.bg_position.first);
  dirtyRegion_out.y =
      static_cast<int16_t> ((*iterator).second.bg_position.second);
  dirtyRegion_out.w = static_cast<uint16_t> ((*iterator).second.bg->w);
  dirtyRegion_out.h = static_cast<uint16_t> ((*iterator).second.bg->h);
  // restore/clear background
  if (clipWindow_in)
    myWindow->clip ();
  if (lockedAccess_in && myScreenLock)
    myScreenLock->lock ();
  if (SDL_BlitSurface ((*iterator).second.bg, // source
                       NULL,                  // aspect (--> everything)
                       target_surface,        // target
                       &dirtyRegion_out))     // position, result
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_BlitSurface(): \"%s\", returning\n"),
                ACE_TEXT (SDL_GetError ())));

    // clean up
    if (lockedAccess_in && myScreenLock)
      myScreenLock->unlock ();
    if (clipWindow_in)
      myWindow->unclip ();

    return;
  } // end IF
  if (lockedAccess_in && myScreenLock)
    myScreenLock->unlock ();
  if (clipWindow_in)
    myWindow->unclip ();

  // update cursor / highlight(s) ?
  // *TODO*: is this really necessary ?
  struct SDL_Rect dirty_region;
  RPG_Client_IWindowLevel* window;
  try {
    window = dynamic_cast<RPG_Client_IWindowLevel*> (myWindow);
  } catch (...) {
    window = NULL;
  }
  if (!window)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to dynamic_cast<RPG_Client_IWindowLevel*>(%@), returning\n"),
                myWindow));
    return;
  } // end IF
  RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance ()->updateHighlightBG (window->getView (),
                                                                         dirty_region,
                                                                         &dirtyRegion_out,
                                                                         lockedAccess_in,
                                                                         debug_in);
  dirtyRegion_out = RPG_Graphics_SDL_Tools::boundingBox (dirty_region,
                                                         dirtyRegion_out);

  RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance ()->updateBG (dirty_region,
                                                                &dirtyRegion_out,
                                                                lockedAccess_in,
                                                                debug_in);
  dirtyRegion_out = RPG_Graphics_SDL_Tools::boundingBox (dirty_region,
                                                         dirtyRegion_out);
}

void
RPG_Client_Entity_Manager::invalidateBG (RPG_Engine_EntityID_t id_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Entity_Manager::invalidateBG"));

  RPG_Client_EntityCacheConstIterator_t iterator = myCache.find (id_in);
  ACE_ASSERT (iterator != myCache.end ());
  ACE_ASSERT ((*iterator).second.bg);
  RPG_Graphics_Surface::clear ((*iterator).second.bg,
                                NULL);
}
