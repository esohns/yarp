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

// *NOTE*: workaround quirky MSVC...
#define NOMINMAX

#include "rpg_graphics_cursor_manager.h"

#include "rpg_graphics_defines.h"
#include "rpg_graphics_iwindow.h"
#include "rpg_graphics_surface.h"
#include "rpg_graphics_dictionary.h"
#include "rpg_graphics_common_tools.h"
#include "rpg_graphics_SDL_tools.h"

#include "rpg_common_macros.h"

#include <ace/OS.h>
#include <ace/Log_Msg.h>

#include <limits>

RPG_Graphics_Cursor_Manager::RPG_Graphics_Cursor_Manager()
 : myCurrentType(CURSOR_NORMAL),
   myCurrentGraphic(NULL),
//   myCache(),
   myBGPosition(std::make_pair(std::numeric_limits<unsigned int>::max(),
                               std::numeric_limits<unsigned int>::max())),
   myBG(NULL),
   myHighlightWindow(NULL),
//   myHighlightBGCache(),
   myHighlightTile(NULL),
   myScreenLock(NULL)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Cursor_Manager::RPG_Graphics_Cursor_Manager"));

  // init cursor highlighting
  SDL_Surface* tile_highlight_bg = RPG_Graphics_Surface::create(RPG_GRAPHICS_TILE_FLOOR_WIDTH,
                                                                RPG_GRAPHICS_TILE_FLOOR_HEIGHT);
  if (!tile_highlight_bg)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Graphics_Surface::create(%u,%u), continuing\n"),
               RPG_GRAPHICS_TILE_FLOOR_WIDTH,
               RPG_GRAPHICS_TILE_FLOOR_HEIGHT));
  myHighlightBGCache.push_back(std::make_pair(std::make_pair(std::numeric_limits<unsigned int>::max(),
                                                             std::numeric_limits<unsigned int>::max()),
                                              tile_highlight_bg));

  RPG_Graphics_GraphicTypeUnion type;
  type.discriminator = RPG_Graphics_GraphicTypeUnion::TILEGRAPHIC;
  type.tilegraphic = TILE_CURSOR_HIGHLIGHT;
  myHighlightTile = RPG_Graphics_Common_Tools::loadGraphic(type,   // tile
                                                           true,   // convert to display format
                                                           false); // don't cache
  if (!myHighlightTile)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Graphics_Common_Tools::loadGraphic(\"%s\"), continuing\n"),
               RPG_Graphics_Common_Tools::typeToString(type).c_str()));

}

RPG_Graphics_Cursor_Manager::~RPG_Graphics_Cursor_Manager()
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Cursor_Manager::~RPG_Graphics_Cursor_Manager"));

  // clean up
  if (myBG)
    SDL_FreeSurface(myBG);

  for (RPG_Graphics_Cursor_CacheConstIterator_t iterator = myCache.begin();
       iterator != myCache.end();
       iterator++)
    SDL_FreeSurface((*iterator).second);

  for (RPG_Graphics_TileCacheConstIterator_t iterator = myHighlightBGCache.begin();
       iterator != myHighlightBGCache.end();
       iterator++)
    SDL_FreeSurface((*iterator).second);

  if (myHighlightTile)
    SDL_FreeSurface(myHighlightTile);
}

void
RPG_Graphics_Cursor_Manager::init(RPG_Common_ILock* screenLock_in,
                                  RPG_Graphics_IWindow* window_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Cursor_Manager::init"));

  ACE_ASSERT(window_in);

  myHighlightWindow = window_in;
  myScreenLock = screenLock_in;
}

void
RPG_Graphics_Cursor_Manager::reset()
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Cursor_Manager::reset"));

  invalidateBG();
  resetHighlightBG(std::make_pair(std::numeric_limits<unsigned int>::max(),
                                  std::numeric_limits<unsigned int>::max()));
}

RPG_Graphics_Cursor
RPG_Graphics_Cursor_Manager::type() const
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Cursor_Manager::type"));

  return myCurrentType;
}

RPG_Graphics_Position_t
RPG_Graphics_Cursor_Manager::position() const
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Cursor_Manager::type"));

  int x, y;
  SDL_GetMouseState(&x, &y);

  RPG_Graphics_Position_t result = std::make_pair(static_cast<unsigned int>(x),
                                                  static_cast<unsigned int>(y));
  return result;
}

void
RPG_Graphics_Cursor_Manager::set(const RPG_Graphics_Cursor& type_in,
                                 SDL_Rect& dirtyRegion_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Cursor_Manager::set"));

  // step0: init return value(s)
  ACE_OS::memset(&dirtyRegion_out, 0, sizeof(dirtyRegion_out));

  // step1: load graphic (check cache first)
  RPG_Graphics_Cursor_CacheConstIterator_t iterator = myCache.find(type_in);
  if (iterator != myCache.end())
    myCurrentGraphic = (*iterator).second;
  else
  {
    // not in cache --> (try to) load graphic
    RPG_Graphics_GraphicTypeUnion type;
    type.discriminator = RPG_Graphics_GraphicTypeUnion::CURSOR;
    type.cursor = type_in;
    RPG_Graphics_t graphic;
    graphic.category = RPG_GRAPHICS_CATEGORY_INVALID;
    graphic.type.discriminator = RPG_Graphics_GraphicTypeUnion::INVALID;
    // retrieve properties from the dictionary
    graphic = RPG_GRAPHICS_DICTIONARY_SINGLETON::instance()->get(type);
    ACE_ASSERT((graphic.type.cursor == type_in) &&
               (graphic.type.discriminator == RPG_Graphics_GraphicTypeUnion::CURSOR));
    // sanity check
    if (graphic.category != CATEGORY_CURSOR)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid category (was: \"%s\"): \"%s\" not a cursor type, aborting\n"),
                 RPG_Graphics_CategoryHelper::RPG_Graphics_CategoryToString(graphic.category).c_str(),
                 RPG_Graphics_CursorHelper::RPG_Graphics_CursorToString(type_in).c_str()));

      return;
    } // end IF

    // assemble path
    std::string filename;
    RPG_Graphics_Common_Tools::graphicToFile(graphic, filename);
    ACE_ASSERT(!filename.empty());

    // load file
    myCurrentGraphic = NULL;
    myCurrentGraphic = RPG_Graphics_Surface::load(filename, // file
                                                  true);    // convert to display format
    if (!myCurrentGraphic)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to RPG_Graphics_Surface::load(\"%s\"), aborting\n"),
                 filename.c_str()));

      return;
    } // end IF

    // update cache
    myCache.insert(std::make_pair(type_in, myCurrentGraphic));

    iterator = myCache.find(type_in);
    ACE_ASSERT(iterator != myCache.end());
  } // end ELSE

  // step1: restore old background
  if (myScreenLock)
    myScreenLock->lock();
  restoreBG(dirtyRegion_out);
  if (myScreenLock)
    myScreenLock->unlock();

  // step2: create background surface
  if (myBG)
  {
    SDL_FreeSurface(myBG);
    myBG = NULL;
  } // end IF
  myBG = RPG_Graphics_Surface::create((*iterator).second->w,
                                      (*iterator).second->h);
  if (!myBG)
  {
    ACE_DEBUG((LM_ERROR,
                ACE_TEXT("failed to RPG_Graphics_Surface::create(%u,%u), aborting\n"),
                (*iterator).second->w,
                (*iterator).second->h));

    return;
  } // end IF
  myBGPosition = std::make_pair(std::numeric_limits<unsigned int>::max(),
                                std::numeric_limits<unsigned int>::max());

//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("set cursor to: \"%s\"\n"),
//                RPG_Graphics_TypeHelper::RPG_Graphics_TypeToString(type_in).c_str()));

  myCurrentType = type_in;
}

void
RPG_Graphics_Cursor_Manager::put(const unsigned int& offsetX_in,
                                 const unsigned int& offsetY_in,
                                 SDL_Rect& dirtyRegion_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Cursor_Manager::put"));

  // step0: init return value(s)
  ACE_OS::memset(&dirtyRegion_out, 0, sizeof(dirtyRegion_out));

  // sanity check(s)
  ACE_ASSERT(myHighlightWindow);
  if ((offsetX_in >= static_cast<unsigned int>(myHighlightWindow->getScreen()->w)) ||
      (offsetY_in >= static_cast<unsigned int>(myHighlightWindow->getScreen()->h)))
    return; // nothing to do
  ACE_ASSERT(myCurrentGraphic);

  // step1: restore old background
  SDL_Rect bgRect;
  if (myScreenLock)
    myScreenLock->lock();
  ACE_OS::memset(&bgRect, 0, sizeof(bgRect));
  restoreBG(bgRect);

  // step2: get new background
  RPG_Graphics_Surface::get(offsetX_in,
                            offsetY_in,
                            true, // use (fast) blitting method
                            *myHighlightWindow->getScreen(),
                            *myBG);
  myBGPosition.first = offsetX_in;
  myBGPosition.second = offsetY_in;

  // step3: place cursor

  // compute bounding box
  dirtyRegion_out.x = static_cast<int16_t>(offsetX_in);
  dirtyRegion_out.y = static_cast<int16_t>(offsetY_in);
  dirtyRegion_out.w = static_cast<uint16_t>(myCurrentGraphic->w);
  dirtyRegion_out.h = static_cast<uint16_t>(myCurrentGraphic->h);
  // handle clipping
  if ((dirtyRegion_out.x + dirtyRegion_out.w) > myHighlightWindow->getScreen()->w)
    dirtyRegion_out.w -= ((dirtyRegion_out.x + dirtyRegion_out.w) - myHighlightWindow->getScreen()->w);
  if ((dirtyRegion_out.y + dirtyRegion_out.h) > myHighlightWindow->getScreen()->h)
    dirtyRegion_out.h -= ((dirtyRegion_out.y + dirtyRegion_out.h) - myHighlightWindow->getScreen()->h);

  SDL_Rect clipRect;
  SDL_GetClipRect(myHighlightWindow->getScreen(), &clipRect);
  SDL_SetClipRect(myHighlightWindow->getScreen(), NULL); // disable window-specific clipping here...
  if (SDL_BlitSurface(myCurrentGraphic,               // source
                      NULL,                           // aspect (--> everything)
                      myHighlightWindow->getScreen(), // target
                      &dirtyRegion_out))              // aspect
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_BlitSurface(): %s, aborting\n"),
               SDL_GetError()));

    // clean up
    if (myScreenLock)
      myScreenLock->unlock();
    SDL_SetClipRect(myHighlightWindow->getScreen(), &clipRect);
    if ((bgRect.w != 0) &&
        (bgRect.h != 0))
      dirtyRegion_out = bgRect;
    else
      ACE_OS::memset(&dirtyRegion_out, 0, sizeof(dirtyRegion_out));

    return;
  } // end IF
  if (myScreenLock)
    myScreenLock->unlock();
  SDL_SetClipRect(myHighlightWindow->getScreen(), &clipRect);

  // *HACK*: somehow, SDL_BlitSurface zeroes dirtyRegion_out.w, dirtyRegion_out.h...
  // --> reset them
  dirtyRegion_out.w = myCurrentGraphic->w;
  dirtyRegion_out.h = myCurrentGraphic->h;

  // if necessary, adjust dirty region
  if ((bgRect.w != 0) &&
      (bgRect.h != 0))
    dirtyRegion_out = RPG_Graphics_SDL_Tools::boundingBox(bgRect,
                                                          dirtyRegion_out);
}

void
RPG_Graphics_Cursor_Manager::restoreBG(SDL_Rect& dirtyRegion_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Cursor_Manager::restoreBG"));

  // step0: init return value(s)
  ACE_OS::memset(&dirtyRegion_out, 0, sizeof(dirtyRegion_out));

  // sanity check(s)
  if (!myBG)
    return; // nothing to do
  ACE_ASSERT(myHighlightWindow);
  if ((myBGPosition.first  == std::numeric_limits<unsigned int>::max()) &&
      (myBGPosition.second == std::numeric_limits<unsigned int>::max()))
    return; // nothing to do

  dirtyRegion_out.x = static_cast<int16_t>(myBGPosition.first);
  dirtyRegion_out.y = static_cast<int16_t>(myBGPosition.second);
  dirtyRegion_out.w = static_cast<uint16_t>(myBG->w);
  dirtyRegion_out.h = static_cast<uint16_t>(myBG->h);
  // handle clipping
  if ((dirtyRegion_out.x + dirtyRegion_out.w) > myHighlightWindow->getScreen()->w)
    dirtyRegion_out.w -= ((dirtyRegion_out.x + dirtyRegion_out.w) - myHighlightWindow->getScreen()->w);
  if ((dirtyRegion_out.y + dirtyRegion_out.h) > myHighlightWindow->getScreen()->h)
    dirtyRegion_out.h -= ((dirtyRegion_out.y + dirtyRegion_out.h) - myHighlightWindow->getScreen()->h);

  // restore/clear background
  SDL_Rect clipRect;
  SDL_GetClipRect(myHighlightWindow->getScreen(), &clipRect);
  SDL_SetClipRect(myHighlightWindow->getScreen(), NULL); // disable window-specific clipping here...
  if (SDL_BlitSurface(myBG,                           // source
                      NULL,                           // aspect (--> everything)
                      myHighlightWindow->getScreen(), // target
                      &dirtyRegion_out))              // aspect
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_BlitSurface(): %s, aborting\n"),
               SDL_GetError()));

    // clean up
    SDL_SetClipRect(myHighlightWindow->getScreen(), &clipRect);

    return;
  } // end IF
  SDL_SetClipRect(myHighlightWindow->getScreen(), &clipRect);

  // safety...
  invalidateBG();

  // *HACK*: somehow, SDL_BlitSurface zeroes dirtyRegion_out.w, dirtyRegion_out.h...
  // --> reset them
  dirtyRegion_out.w = myBG->w;
  dirtyRegion_out.h = myBG->h;
}

void
RPG_Graphics_Cursor_Manager::updateBG()
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Cursor_Manager::updateBG"));

  // sanity check(s)
  ACE_ASSERT(myBG);
  ACE_ASSERT(myHighlightWindow);

  // the map window has changed...
  // --> update ONLY that bit of our background (if any)

  // step1: retrieve clip area of the map (==highlight) window
  SDL_Rect map_clip_rect;
  myHighlightWindow->clip();
  SDL_GetClipRect(myHighlightWindow->getScreen(), &map_clip_rect);
  myHighlightWindow->unclip();

  // step2: intersect with cached bg
  SDL_Rect bg_rect;
  bg_rect.x = static_cast<int16_t>(myBGPosition.first);
  bg_rect.y = static_cast<int16_t>(myBGPosition.second);
  bg_rect.w = myBG->w;
  bg_rect.h = myBG->h;
  SDL_Rect map_bit_rect;
  ACE_OS::memset(&map_bit_rect, 0, sizeof(SDL_Rect));
  map_bit_rect = RPG_Graphics_SDL_Tools::intersect(map_clip_rect, bg_rect);
  if ((map_bit_rect.w == 0) || // cached cursor bg fully outside of map ?
      (map_bit_rect.h == 0))
    return; // --> nothing to do...
  else if ((map_bit_rect.w == bg_rect.w) && (map_bit_rect.h == bg_rect.h))
  {
    // cached cursor bg fully inside of map
    // --> just get a fresh copy
    RPG_Graphics_Surface::get(myBGPosition.first,
                              myBGPosition.second,
                              true, // use (fast) blitting method
                              *myHighlightWindow->getScreen(),
                              *myBG);

    return;
  } // end ELSEIF

  // step3: adjust intersection coordinates (relative to cached bg surface)
  map_bit_rect.x -= bg_rect.x;
  map_bit_rect.y -= bg_rect.y;

  // step4: get a fresh copy from that part of the map
  SDL_Surface* new_bg = RPG_Graphics_Surface::create(static_cast<unsigned int>(myBG->w),
                                                     static_cast<unsigned int>(myBG->h));
  ACE_ASSERT(new_bg);
  RPG_Graphics_Surface::get(myBGPosition.first,
                            myBGPosition.second,
                            true, // use (fast) blitting method
                            *myHighlightWindow->getScreen(),
                            *new_bg);

  // step5: mask the map bit from the cached bg
  if (SDL_FillRect(myBG,
                   &map_bit_rect,
                   RPG_Graphics_SDL_Tools::CLR32_BLACK_A0)) // "transparency"
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_FillRect(): %s, aborting\n"),
               SDL_GetError()));

    // clean up
    SDL_FreeSurface(new_bg);

    return;
  } // end IF

  // step6: blit the cached bg onto the fresh copy
  if (SDL_BlitSurface(myBG,   // source
                      NULL,   // aspect (--> everything)
                      new_bg, // target
                      NULL))  // aspect (--> everything)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_BlitSurface(): %s, aborting\n"),
               SDL_GetError()));

    // clean up
    SDL_FreeSurface(new_bg);

    return;
  } // end IF

  // clean up
  SDL_FreeSurface(myBG);

  myBG = new_bg;
}

void
RPG_Graphics_Cursor_Manager::invalidateBG()
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Cursor_Manager::invalidateBG"));

  // sanity check
  ACE_ASSERT(myBG);

  RPG_Graphics_Surface::clear(myBG);
}

RPG_Graphics_Position_t
RPG_Graphics_Cursor_Manager::getHighlightBGPosition(const unsigned int& index_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Cursor_Manager::getHighlightBGPosition"));

  // sanity checks
  if (myHighlightBGCache.empty() ||
      ((index_in + 1) > myHighlightBGCache.size()))
    return std::make_pair(std::numeric_limits<unsigned int>::max(),
                          std::numeric_limits<unsigned int>::max());

  if (index_in == std::numeric_limits<unsigned int>::max())
    return myHighlightBGCache.back().first; // return LAST element

  return myHighlightBGCache[index_in].first;
}

void
RPG_Graphics_Cursor_Manager::putHighlight(const RPG_Map_Position_t& mapPosition_in,
                                          const RPG_Graphics_Position_t& graphicsPosition_in,
                                          const RPG_Graphics_Position_t& viewPort_in,
                                          SDL_Rect& dirtyRegion_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Cursor_Manager::putHighlight"));

  // step0: init return value(s)
  ACE_OS::memset(&dirtyRegion_out, 0, sizeof(dirtyRegion_out));

  // sanity check
  ACE_ASSERT(myHighlightTile);
  ACE_ASSERT(myHighlightWindow);
  ACE_ASSERT(myScreenLock);

  // step1: restore old background
  if (myScreenLock)
    myScreenLock->lock();
  restoreHighlightBG(viewPort_in,
                     dirtyRegion_out);
  if (myScreenLock)
    myScreenLock->unlock();

  // step2: get new background
  storeHighlightBG(mapPosition_in,
                   graphicsPosition_in);

  // step3: place highlight

  myHighlightWindow->clip();
  myScreenLock->lock();
  RPG_Graphics_Surface::put(graphicsPosition_in.first,
                            graphicsPosition_in.second,
                            *myHighlightTile,
                            myHighlightWindow->getScreen());
  myScreenLock->unlock();
  myHighlightWindow->unclip();

  // step4: compute dirty region
  SDL_Rect dirty_region;
  dirty_region.x = static_cast<int16_t>(graphicsPosition_in.first);
  dirty_region.y = static_cast<int16_t>(graphicsPosition_in.second);
  dirty_region.w = static_cast<uint16_t>(myHighlightTile->w);
  dirty_region.h = static_cast<uint16_t>(myHighlightTile->h);
  dirtyRegion_out = RPG_Graphics_SDL_Tools::boundingBox(dirty_region,
                                                        dirtyRegion_out);
}

void
RPG_Graphics_Cursor_Manager::putHighlights(const RPG_Map_PositionList_t& mapPositions_in,
                                           const RPG_Graphics_Positions_t& graphicsPositions_in,
                                           const RPG_Graphics_Position_t& viewPort_in,
                                           SDL_Rect& dirtyRegion_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Cursor_Manager::drawHighlight"));

  // step0: init return value(s)
  ACE_OS::memset(&dirtyRegion_out, 0, sizeof(dirtyRegion_out));

  // sanity check
  ACE_ASSERT(myHighlightTile);
  ACE_ASSERT(myHighlightWindow);
  ACE_ASSERT(myScreenLock);

  // step1: restore old backgrounds
  if (myScreenLock)
    myScreenLock->lock();
  restoreHighlightBG(viewPort_in,
                     dirtyRegion_out);
  if (myScreenLock)
    myScreenLock->unlock();

  // step2: get new backgrounds
  storeHighlightBG(mapPositions_in,
                   graphicsPositions_in);

  // step3: place highlights

  SDL_Rect current_rect;
  current_rect.w = static_cast<uint16_t>(myHighlightTile->w);
  current_rect.h = static_cast<uint16_t>(myHighlightTile->h);
  myHighlightWindow->clip();
  myScreenLock->lock();
  for (RPG_Graphics_PositionsConstIterator_t iterator = graphicsPositions_in.begin();
       iterator != graphicsPositions_in.end();
       iterator++)
  {
    RPG_Graphics_Surface::put((*iterator).first,
                              (*iterator).second,
                              *myHighlightTile,
                              myHighlightWindow->getScreen());

    // compute dirty region
    current_rect.x = static_cast<int16_t>((*iterator).first);
    current_rect.y = static_cast<int16_t>((*iterator).second);
    if ((dirtyRegion_out.w == 0) && (dirtyRegion_out.h == 0))
      dirtyRegion_out = current_rect;
    else
      dirtyRegion_out = RPG_Graphics_SDL_Tools::boundingBox(current_rect,
                                                            dirtyRegion_out);
  } // end FOR
  myScreenLock->unlock();
  myHighlightWindow->unclip();
}

void
RPG_Graphics_Cursor_Manager::storeHighlightBG(const RPG_Map_Position_t& mapPosition_in,
                                              const RPG_Graphics_Position_t& graphicsPosition_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Cursor_Manager::storeHighlightBG"));

  // sanity check
  ACE_ASSERT(!myHighlightBGCache.empty());
  ACE_ASSERT(myHighlightWindow);

  // clean up cache ?
  if (myHighlightBGCache.size() > 1)
    resetHighlightBG(std::make_pair(std::numeric_limits<unsigned int>::max(),
                                    std::numeric_limits<unsigned int>::max()));

  RPG_Graphics_Surface::clear(myHighlightBGCache.front().second);

  myHighlightWindow->clip();
  RPG_Graphics_Surface::get(graphicsPosition_in.first,
                            graphicsPosition_in.second,
                            true, // use (fast) blitting method
                            *myHighlightWindow->getScreen(),
                            *myHighlightBGCache.front().second);
  myHighlightWindow->unclip();

  myHighlightBGCache.front().first = mapPosition_in;
}

void
RPG_Graphics_Cursor_Manager::storeHighlightBG(const RPG_Map_PositionList_t& mapPositions_in,
                                              const RPG_Graphics_Positions_t& graphicsPositions_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Cursor_Manager::storeHighlightBG"));

  // sanity check
  ACE_ASSERT(myHighlightWindow);
  ACE_ASSERT(!mapPositions_in.empty());
  ACE_ASSERT(mapPositions_in.size() == graphicsPositions_in.size());

  // grow/shrink cache as necessary
  int delta = myHighlightBGCache.size() - mapPositions_in.size();
  if (delta > 0)
  {
    for (int i = delta;
         i > 0;
         i--)
    {
      SDL_FreeSurface(myHighlightBGCache.back().second);
      myHighlightBGCache.pop_back();
    } // end FOR
  } // end IF
  else if (delta < 0)
  {
    SDL_Surface* new_entry = NULL;
    for (int i = -delta;
         i > 0;
         i--)
    {
      new_entry = RPG_Graphics_Surface::create(RPG_GRAPHICS_TILE_FLOOR_WIDTH,
                                               RPG_GRAPHICS_TILE_FLOOR_HEIGHT);
      if (!new_entry)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to RPG_Graphics_Surface::create(%u,%u), aborting\n"),
                   RPG_GRAPHICS_TILE_FLOOR_WIDTH,
                   RPG_GRAPHICS_TILE_FLOOR_HEIGHT));

        return;
      } // end IF
      myHighlightBGCache.push_back(std::make_pair(std::make_pair(std::numeric_limits<unsigned int>::max(),
                                                                 std::numeric_limits<unsigned int>::max()),
                                                  new_entry));
    } // end IF
  } // end ELSEIF
  ACE_ASSERT(myHighlightBGCache.size() == mapPositions_in.size());

  RPG_Graphics_PositionsConstIterator_t graphics_position_iterator = graphicsPositions_in.begin();;
  RPG_Graphics_TileCacheIterator_t cache_iterator = myHighlightBGCache.begin();
  myHighlightWindow->clip();
  for (RPG_Map_PositionListConstIterator_t map_position_iterator = mapPositions_in.begin();
       map_position_iterator != mapPositions_in.end();
       map_position_iterator++, graphics_position_iterator++, cache_iterator++)
  {
    RPG_Graphics_Surface::clear((*cache_iterator).second);
    RPG_Graphics_Surface::get((*graphics_position_iterator).first,
                              (*graphics_position_iterator).second,
                              true, // use (fast) blitting method
                              *myHighlightWindow->getScreen(),
                              *(*cache_iterator).second);
    (*cache_iterator).first = *map_position_iterator;
  } // end FOR
  myHighlightWindow->unclip();
}

void
RPG_Graphics_Cursor_Manager::restoreHighlightBG(const RPG_Graphics_Position_t& viewPort_in,
                                                SDL_Rect& dirtyRegion_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Cursor_Manager::restoreHighlightBG"));

  // step0: init return value(s)
  ACE_OS::memset(&dirtyRegion_out, 0, sizeof(dirtyRegion_out));

  // sanity check
  ACE_ASSERT(myHighlightWindow);
  ACE_ASSERT(!myHighlightBGCache.empty());
  ACE_ASSERT(myScreenLock);

  RPG_Graphics_Position_t screen_position;
  RPG_Graphics_Size_t size = myHighlightWindow->getSize();
  SDL_Rect current_rect;
  current_rect.w = static_cast<uint16_t>(myHighlightBGCache.front().second->w);
  current_rect.h = static_cast<uint16_t>(myHighlightBGCache.front().second->h);
  myHighlightWindow->clip();
  myScreenLock->lock();
  for (RPG_Graphics_TileCacheConstIterator_t iterator = myHighlightBGCache.begin();
       iterator != myHighlightBGCache.end();
       iterator++)
  {
    screen_position = RPG_Graphics_Common_Tools::map2Screen((*iterator).first,
                                                            size,
                                                            viewPort_in);
    RPG_Graphics_Surface::put(screen_position.first,
                              screen_position.second,
                              *(*iterator).second,
                              myHighlightWindow->getScreen());

    // compute dirty region
    current_rect.x = static_cast<int16_t>(screen_position.first);
    current_rect.y = static_cast<int16_t>(screen_position.second);
    if ((dirtyRegion_out.w == 0) && (dirtyRegion_out.h == 0))
      dirtyRegion_out = current_rect;
    else
      dirtyRegion_out = RPG_Graphics_SDL_Tools::boundingBox(current_rect,
                                                            dirtyRegion_out);
  } // end FOR
  myScreenLock->unlock();
  myHighlightWindow->unclip();
}

void
RPG_Graphics_Cursor_Manager::resetHighlightBG(const RPG_Graphics_Position_t& position_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Cursor_Manager::resetHighlightBG"));

  // init/clear highlight BG cache
  while (myHighlightBGCache.size() > 1)
  {
    SDL_FreeSurface(myHighlightBGCache.back().second);
    myHighlightBGCache.pop_back();
  } // end WHILE
  myHighlightBGCache.front().first = position_in;
  RPG_Graphics_Surface::clear(myHighlightBGCache.front().second);
}
