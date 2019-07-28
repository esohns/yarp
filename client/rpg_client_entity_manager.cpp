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

// *NOTE*: work around quirky MSVC...
#ifdef _MSC_VER
#define NOMINMAX
#endif

#include "rpg_client_entity_manager.h"

#include "rpg_client_iwindow_level.h"

#include "rpg_graphics_defines.h"
#include "rpg_graphics_surface.h"
#include "rpg_graphics_cursor_manager.h"
#include "rpg_graphics_SDL_tools.h"

#include "rpg_common_macros.h"

#include "ace/OS.h"
#include "ace/Log_Msg.h"

#include <limits>

RPG_Client_Entity_Manager::RPG_Client_Entity_Manager()
 : myScreenLock(NULL),
   myWindow(NULL)//,
//   myCache()
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Entity_Manager::RPG_Client_Entity_Manager"));

}

RPG_Client_Entity_Manager::~RPG_Client_Entity_Manager()
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Entity_Manager::~RPG_Client_Entity_Manager"));

  // clean up
  for (RPG_Client_EntityCacheConstIterator_t iterator = myCache.begin();
       iterator != myCache.end();
       iterator++)
  {
    if ((*iterator).second.free_on_remove)
      SDL_FreeSurface((*iterator).second.graphic);
    SDL_FreeSurface((*iterator).second.bg);
  } // end FOR
}

void
RPG_Client_Entity_Manager::initialize(Common_ILock* screenLock_in,
                                      RPG_Graphics_IWindowBase* window_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Entity_Manager::init"));

  ACE_ASSERT(window_in);

  myScreenLock = screenLock_in;
  myWindow = window_in;
}

void
RPG_Client_Entity_Manager::add(const RPG_Engine_EntityID_t& id_in,
                               SDL_Surface* surface_in,
                               const bool& free_on_remove_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Entity_Manager::add"));

  // sanity checks
  ACE_ASSERT(surface_in);
	RPG_Client_EntityCacheConstIterator_t iterator = myCache.find(id_in);
	if (iterator != myCache.end())
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("entity ID %u already cached, returning\n"),
               id_in));

    // clean up
    if (free_on_remove_in)
      SDL_FreeSurface(surface_in);

    return;
  } // end IF

  struct RPG_Client_EntityCacheEntry new_entry;
  new_entry.graphic = surface_in;
  new_entry.free_on_remove = free_on_remove_in;
  new_entry.bg = RPG_Graphics_Surface::create(surface_in->w,
                                              surface_in->h);
  if (!new_entry.bg)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Graphics_Surface::create(%u,%u), returning\n"),
               surface_in->w, surface_in->h));

    // clean up
    if (free_on_remove_in)
      SDL_FreeSurface(surface_in);

    return;
  } // end IF
  new_entry.bg_position =
      std::make_pair(std::numeric_limits<unsigned int>::max(),
                     std::numeric_limits<unsigned int>::max());

  myCache[id_in] = new_entry;
}

void
RPG_Client_Entity_Manager::remove(const RPG_Engine_EntityID_t& id_in,
                                  SDL_Rect& dirtyRegion_out,
                                  const bool& lockedAccess_in,
                                  const bool& debug_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Entity_Manager::remove"));

  // init return value(s)
  ACE_OS::memset(&dirtyRegion_out, 0, sizeof(dirtyRegion_out));

  // sanity check(s)
  RPG_Client_EntityCacheConstIterator_t iterator = myCache.find(id_in);
  if (iterator == myCache.end())
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid entity ID (was: %u), returning\n"),
               id_in));

    return;
  } // end IF

  // step0: restore old background
  if (myScreenLock && lockedAccess_in)
    myScreenLock->lock();
  restoreBG(id_in,
            dirtyRegion_out,
            true,
            false,
            debug_in);
  if (myScreenLock && lockedAccess_in)
    myScreenLock->unlock();

  // clean up
  if ((*iterator).second.free_on_remove)
    SDL_FreeSurface((*iterator).second.graphic);
  SDL_FreeSurface((*iterator).second.bg);

  myCache.erase((*iterator).first);
}

bool
RPG_Client_Entity_Manager::cached(const RPG_Engine_EntityID_t& id_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Entity_Manager::cached"));

  RPG_Client_EntityCacheConstIterator_t iterator = myCache.find(id_in);

  return (iterator != myCache.end());
}

void
RPG_Client_Entity_Manager::put(const RPG_Engine_EntityID_t& id_in,
                               const RPG_Graphics_Position_t& position_in,
                               SDL_Rect& dirtyRegion_out,
                               const bool& clipWindow_in,
                               const bool& lockedAccess_in,
                               const bool& debug_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Entity_Manager::put"));

  // step0: init return value(s)
  ACE_OS::memset(&dirtyRegion_out, 0, sizeof(dirtyRegion_out));

  // sanity check(s)
  ACE_ASSERT(myWindow);
  SDL_Surface* target_surface = myWindow->getScreen();
  ACE_ASSERT(target_surface);
  RPG_Client_EntityCacheIterator_t iterator = myCache.find(id_in);
  if (iterator == myCache.end())
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid entity ID (was: %u), returning\n"),
               id_in));

    return;
  } // end IF

  // step1: restore old background
//	if (myScreenLock && lockedAccess_in)
//		myScreenLock->lock();
  restoreBG(id_in,
            dirtyRegion_out,
            clipWindow_in,
            false,
            debug_in);

  // step2: get new background
	// step2a: restore cursor / highlight bg first
	SDL_Rect clip_rectangle, dirty_region;
	// *NOTE*: entities are drawn "centered" on the floor tile
	clip_rectangle.x = (position_in.first +
											((RPG_GRAPHICS_TILE_FLOOR_WIDTH -
											(*iterator).second.graphic->w) / 2));
	clip_rectangle.y = (position_in.second +
											(RPG_GRAPHICS_TILE_FLOOR_HEIGHT / 2) -
											(*iterator).second.graphic->h);
	clip_rectangle.w = (*iterator).second.graphic->w;
	clip_rectangle.h = (*iterator).second.graphic->h;
	RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->restoreBG(dirty_region,
																															 &clip_rectangle,
																															 false);
	dirtyRegion_out = RPG_Graphics_SDL_Tools::boundingBox(dirty_region,
																												dirtyRegion_out);
	RPG_Client_IWindowLevel* window = NULL;
	try
	{
		window = dynamic_cast<RPG_Client_IWindowLevel*>(myWindow);
	}
	catch (...)
	{
		window = NULL;
	}
	if (!window)
	{
		ACE_DEBUG((LM_ERROR,
							 ACE_TEXT("failed to dynamic_cast<RPG_Client_IWindowLevel*>(%@), returning\n"),
							 myWindow));

		// clean up
		if (myScreenLock && lockedAccess_in)
			myScreenLock->unlock();

		return;
	} // end IF
	RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->restoreHighlightBG(window->getView(),
																																				dirty_region,
																																				&clip_rectangle,
																																				false,
																																				debug_in);
	dirtyRegion_out = RPG_Graphics_SDL_Tools::boundingBox(dirty_region,
																												dirtyRegion_out);
	// step2b: load bg image
  RPG_Graphics_Position_t screen_coordinates =
		std::make_pair(clip_rectangle.x, clip_rectangle.y);
  RPG_Graphics_Surface::get(screen_coordinates,
                            true, // use (fast) blitting method
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
  dirty_region = RPG_Graphics_SDL_Tools::intersect(dirty_region,
												   clip_rectangle);

  // place graphic
  if (clipWindow_in)
    myWindow->clip();
  RPG_Graphics_Surface::put(screen_coordinates,
                            *(*iterator).second.graphic,
                            target_surface,
                            dirty_region);
  if (lockedAccess_in && myScreenLock)
    myScreenLock->unlock();
  if (clipWindow_in)
    myWindow->unclip();
  dirtyRegion_out = RPG_Graphics_SDL_Tools::boundingBox(dirty_region,
														dirtyRegion_out);

  // update cursor / highlight(s)
  RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->updateBG(dirty_region,
															  &dirtyRegion_out,
															  lockedAccess_in,
															  debug_in);
  dirtyRegion_out = RPG_Graphics_SDL_Tools::boundingBox(dirty_region,
														dirtyRegion_out);
  RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->updateHighlightBG(window->getView(),
																	   dirty_region,
																	   &dirtyRegion_out,
																	   lockedAccess_in,
																	   debug_in);
  dirtyRegion_out = RPG_Graphics_SDL_Tools::boundingBox(dirty_region,
	                                                    dirtyRegion_out);

  if (debug_in)
  {
    // show entity bg in bottom right corner
    ACE_ASSERT(target_surface->w >= (*iterator).second.bg->w);
    ACE_ASSERT(target_surface->h >= (*iterator).second.bg->h);

    RPG_Graphics_Surface::unclip();
    RPG_Graphics_Surface::put(std::make_pair((target_surface->w - (*iterator).second.bg->w),
                                             (target_surface->h - (*iterator).second.bg->h)),
                              *(*iterator).second.bg,
                              target_surface,
                              dirty_region);
    RPG_Graphics_Surface::clip();

    dirtyRegion_out = RPG_Graphics_SDL_Tools::boundingBox(dirty_region,
                                                          dirtyRegion_out);
  } // end IF
}

void
RPG_Client_Entity_Manager::restoreBG(const RPG_Engine_EntityID_t& id_in,
                                     SDL_Rect& dirtyRegion_out,
                                     const bool& clipWindow_in,
                                     const bool& lockedAccess_in,
																		 const bool& debug_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Entity_Manager::restoreBG"));

  // sanity check(s)
  ACE_ASSERT(myWindow);
  SDL_Surface* target_surface = myWindow->getScreen();
  ACE_ASSERT(target_surface);
  RPG_Client_EntityCacheConstIterator_t iterator = myCache.find(id_in);
  if (iterator == myCache.end())
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid entity ID (was: %u), returning\n"),
               id_in));

    return;
  } // end IF
  if ((*iterator).second.bg_position ==
      std::make_pair(std::numeric_limits<unsigned int>::max(),
                     std::numeric_limits<unsigned int>::max()))
    return; // nothing to do

  dirtyRegion_out.x =
      static_cast<int16_t>((*iterator).second.bg_position.first);
  dirtyRegion_out.y =
      static_cast<int16_t>((*iterator).second.bg_position.second);
//  dirtyRegion_out.w = static_cast<uint16_t>((*iterator).second.bg->w);
//  dirtyRegion_out.h = static_cast<uint16_t>((*iterator).second.bg->h);
//	SDL_Rect clip_rectangle;
//	myWindow->getArea(clip_rectangle, false);
//	dirtyRegion_out = RPG_Graphics_SDL_Tools::intersect(dirtyRegion_out,
//																											clip_rectangle);

  // restore/clear background
  if (clipWindow_in)
    myWindow->clip();
//  if (lockedAccess_in && myScreenLock)
//    myScreenLock->lock();
  if (SDL_BlitSurface((*iterator).second.bg, // source
                      NULL,                  // aspect (--> everything)
                      target_surface,        // target
                      &dirtyRegion_out))     // position, result
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_BlitSurface(): \"%s\", returning\n"),
               ACE_TEXT(SDL_GetError())));

    // clean up
//    if (lockedAccess_in && myScreenLock)
//      myScreenLock->unlock();
    if (clipWindow_in)
      myWindow->unclip();

    return;
  } // end IF
//  if (lockedAccess_in && myScreenLock)
//    myScreenLock->unlock();
  if (clipWindow_in)
    myWindow->unclip();

	// update cursor / highlight(s) ?
	SDL_Rect dirty_region;
	RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->updateBG(dirty_region,
                                                                &dirtyRegion_out,
                                                                lockedAccess_in,
                                                                debug_in);
	dirtyRegion_out = RPG_Graphics_SDL_Tools::boundingBox(dirty_region,
                                                            dirtyRegion_out);
	RPG_Client_IWindowLevel* window = NULL;
	try
	{
		window = dynamic_cast<RPG_Client_IWindowLevel*>(myWindow);
	}
	catch (...)
	{
		window = NULL;
	}
	if (!window)
	{
		ACE_DEBUG((LM_ERROR,
							 ACE_TEXT("failed to dynamic_cast<RPG_Client_IWindowLevel*>(%@), returning\n"),
			         myWindow));

		return;
	} // end IF
	RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->updateHighlightBG(window->getView(),
		                                                                   dirty_region,
																																			 &dirtyRegion_out,
																																			 lockedAccess_in,
																																			 debug_in);
	dirtyRegion_out = RPG_Graphics_SDL_Tools::boundingBox(dirty_region,
																												dirtyRegion_out);
}

void
RPG_Client_Entity_Manager::invalidateBG(const RPG_Engine_EntityID_t& id_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Entity_Manager::invalidateBG"));

  RPG_Client_EntityCacheConstIterator_t iterator = myCache.find(id_in);
  // sanity check(s)
  ACE_ASSERT(iterator != myCache.end());

  RPG_Graphics_Surface::clear((*iterator).second.bg);
}
