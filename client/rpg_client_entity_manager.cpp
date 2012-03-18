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

// *NOTE*: workaround quirky MSVC...
#define NOMINMAX

#include "rpg_client_entity_manager.h"

#include <rpg_graphics_defines.h>
#include <rpg_graphics_surface.h>
#include <rpg_graphics_SDL_tools.h>

#include <rpg_common_macros.h>

#include <ace/OS.h>
#include <ace/Log_Msg.h>

#include <limits>

RPG_Client_Entity_Manager::RPG_Client_Entity_Manager()
 : myWindow(NULL)//,
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
    SDL_FreeSurface((*iterator).second.bg);

  myCache.clear();
}

void
RPG_Client_Entity_Manager::init(RPG_Graphics_SDLWindowBase* window_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Entity_Manager::init"));

  ACE_ASSERT(window_in);

  myWindow = window_in;
}

void
RPG_Client_Entity_Manager::add(const RPG_Engine_EntityID_t& id_in,
                               const SDL_Surface* surface_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Entity_Manager::add"));

  // sanity checks
  ACE_ASSERT(surface_in);
  if (myCache.find(id_in) != myCache.end())
    return; // nothing to do...

  RPG_Client_EntityCacheEntry_t new_entry;
  new_entry.graphic = surface_in;
  new_entry.bg_position = std::make_pair(std::numeric_limits<unsigned int>::max(),
                                         std::numeric_limits<unsigned int>::max());
  new_entry.bg = RPG_Graphics_Surface::create(surface_in->w,
                                              surface_in->h);
  if (!new_entry.bg)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Graphics_Surface::create(%u,%u), aborting\n"),
               surface_in->w,
               surface_in->h));

    return;
  };

  myCache[id_in] = new_entry;
}

void
RPG_Client_Entity_Manager::remove(const RPG_Engine_EntityID_t& id_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Entity_Manager::remove"));

  RPG_Client_EntityCacheConstIterator_t iterator = myCache.find(id_in);
  if (iterator == myCache.end())
    return; // nothing to do...

  // clean up
  SDL_FreeSurface((*iterator).second.bg);

  myCache.erase(iterator);
}

const bool
RPG_Client_Entity_Manager::cached(const RPG_Engine_EntityID_t& id_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Entity_Manager::cached"));

  RPG_Client_EntityCacheConstIterator_t iterator = myCache.find(id_in);

  return (iterator != myCache.end());
}

void
RPG_Client_Entity_Manager::put(const RPG_Engine_EntityID_t& id_in,
                               const RPG_Graphics_Position_t& tileCoordinates_in,
                               SDL_Surface* targetSurface_in,
                               SDL_Rect& dirtyRegion_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Entity_Manager::put"));

  RPG_Client_EntityCacheIterator_t iterator = myCache.find(id_in);
  // sanity check(s)
  ACE_ASSERT(iterator != myCache.end());
  ACE_ASSERT(myWindow);
  ACE_ASSERT(targetSurface_in);
//   ACE_ASSERT(offsetX_in < static_cast<unsigned int>(targetSurface_in->w));
//   ACE_ASSERT(offsetY_in < static_cast<unsigned int>(targetSurface_in->h));

  // step0: init return value(s)
  ACE_OS::memset(&dirtyRegion_out,
                 0,
                 sizeof(dirtyRegion_out));

  // step1: restore old background
  SDL_Rect bgRect;
  ACE_OS::memset(&bgRect,
                 0,
                 sizeof(bgRect));
  if (((*iterator).second.bg_position.first  != std::numeric_limits<unsigned int>::max()) ||
      ((*iterator).second.bg_position.second != std::numeric_limits<unsigned int>::max()))
    restoreBG(id_in,
              targetSurface_in,
              bgRect);

  // step2: get new background

  // *NOTE*: creatures are drawn in the "middle" of the (floor) tile
  RPG_Graphics_Position_t screen_coordinates = std::make_pair((tileCoordinates_in.first +
                                                               ((RPG_GRAPHICS_TILE_FLOOR_WIDTH - (*iterator).second.graphic->w) / 2)),
                                                              (tileCoordinates_in.second +
                                                               (RPG_GRAPHICS_TILE_FLOOR_HEIGHT / 2) -
                                                               (*iterator).second.graphic->h));

  RPG_Graphics_Surface::get(screen_coordinates.first,
                            screen_coordinates.second,
                            true, // use (fast) blitting method
                            *targetSurface_in,
                            *(*iterator).second.bg);
  (*iterator).second.bg_position = screen_coordinates;

  // compute bounding box
  dirtyRegion_out.x = static_cast<int16_t>(screen_coordinates.first);
  dirtyRegion_out.y = static_cast<int16_t>(screen_coordinates.second);
  dirtyRegion_out.w = static_cast<uint16_t>((*iterator).second.graphic->w);
  dirtyRegion_out.h = static_cast<uint16_t>((*iterator).second.graphic->h);
  // handle clipping
  if ((dirtyRegion_out.x + dirtyRegion_out.w) > targetSurface_in->w)
    dirtyRegion_out.w -= ((dirtyRegion_out.x + dirtyRegion_out.w) - targetSurface_in->w);
  if ((dirtyRegion_out.y + dirtyRegion_out.h) > targetSurface_in->h)
    dirtyRegion_out.h -= ((dirtyRegion_out.y + dirtyRegion_out.h) - targetSurface_in->h);

  // place graphic
  myWindow->clip();
  if (SDL_BlitSurface(const_cast<SDL_Surface*>((*iterator).second.graphic), // source
                      NULL,                                                 // aspect (--> everything)
                      targetSurface_in,                                     // target
                      &dirtyRegion_out))                                    // aspect
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_BlitSurface(): %s, aborting\n"),
               SDL_GetError()));

    // clean up
    myWindow->unclip();
    if ((bgRect.w != 0) &&
        (bgRect.h != 0))
      dirtyRegion_out = bgRect;
    else
    {
      dirtyRegion_out.x = 0;
      dirtyRegion_out.y = 0;
      dirtyRegion_out.w = 0;
      dirtyRegion_out.h = 0;
    } // end ELSE

    return;
  } // end IF
  myWindow->unclip();

  // *HACK*: somehow, SDL_BlitSurface zeroes dirtyRegion_out.w, dirtyRegion_out.h...
  // --> reset them
  dirtyRegion_out.w = (*iterator).second.graphic->w;
  dirtyRegion_out.h = (*iterator).second.graphic->h;

  // if necessary, adjust dirty region
  if ((bgRect.w != 0) &&
      (bgRect.h != 0))
    dirtyRegion_out = RPG_Graphics_SDL_Tools::boundingBox(bgRect,
                                                          dirtyRegion_out);
}

void
RPG_Client_Entity_Manager::restoreBG(const RPG_Engine_EntityID_t& id_in,
                                     SDL_Surface* targetSurface_in,
                                     SDL_Rect& dirtyRegion_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Entity_Manager::restoreBG"));

  RPG_Client_EntityCacheConstIterator_t iterator = myCache.find(id_in);
  // sanity check(s)
  ACE_ASSERT(iterator != myCache.end());
  ACE_ASSERT(myWindow);
  ACE_ASSERT(targetSurface_in);
//   ACE_ASSERT((*iterator).bg_position.first != 0);
//   ACE_ASSERT((*iterator).bg_position.second != 0);

  dirtyRegion_out.x = static_cast<int16_t>((*iterator).second.bg_position.first);
  dirtyRegion_out.y = static_cast<int16_t>((*iterator).second.bg_position.second);
  dirtyRegion_out.w = static_cast<uint16_t>((*iterator).second.bg->w);
  dirtyRegion_out.h = static_cast<uint16_t>((*iterator).second.bg->h);
  // handle clipping
  if ((dirtyRegion_out.x + dirtyRegion_out.w) > targetSurface_in->w)
    dirtyRegion_out.w -= ((dirtyRegion_out.x + dirtyRegion_out.w) - targetSurface_in->w);
  if ((dirtyRegion_out.y + dirtyRegion_out.h) > targetSurface_in->h)
    dirtyRegion_out.h -= ((dirtyRegion_out.y + dirtyRegion_out.h) - targetSurface_in->h);

  // restore/clear background
  myWindow->clip();
  if (SDL_BlitSurface((*iterator).second.bg, // source
                      NULL,                  // aspect (--> everything)
                      targetSurface_in,      // target
                      &dirtyRegion_out))     // aspect
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_BlitSurface(): %s, aborting\n"),
               SDL_GetError()));

    // clean up
    myWindow->unclip();

    return;
  } // end IF
  myWindow->unclip();

  // *HACK*: somehow, SDL_BlitSurface zeroes dirtyRegion_out.w, dirtyRegion_out.h...
  // --> reset them
  dirtyRegion_out.w = (*iterator).second.bg->w;
  dirtyRegion_out.h = (*iterator).second.bg->h;

//  RPG_Graphics_Surface::clear((*iterator).bg);
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
