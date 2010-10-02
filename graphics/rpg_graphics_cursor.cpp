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
#include "rpg_graphics_cursor.h"

#include "rpg_graphics_dictionary.h"
#include "rpg_graphics_common_tools.h"
#include "rpg_graphics_SDL_tools.h"

#include <rpg_common_macros.h>

#include <ace/Log_Msg.h>

RPG_Graphics_Cursor::RPG_Graphics_Cursor()
 : inherited(),
   myCurrentType(TYPE_CURSOR_NORMAL),
   myBGPosition(std::make_pair(0, 0)),
   myBG(NULL)//,
//    myCache()
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Cursor::RPG_Graphics_Cursor"));

}

RPG_Graphics_Cursor::~RPG_Graphics_Cursor()
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Cursor::~RPG_Graphics_Cursor"));

  // clean up
  if (myBG)
    SDL_FreeSurface(myBG);

  for (RPG_Graphics_CursorCacheConstIterator_t iterator = myCache.begin();
       iterator != myCache.end();
       iterator++)
    SDL_FreeSurface((*iterator).second);
}

const RPG_Graphics_Type
RPG_Graphics_Cursor::type() const
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Cursor::type"));

  return myCurrentType;
}

void
RPG_Graphics_Cursor::set(const RPG_Graphics_Type& type_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Cursor::set"));

  // check cache first
  RPG_Graphics_CursorCacheConstIterator_t iterator = myCache.find(type_in);
  if (iterator != myCache.end())
  {
    init((*iterator).second, // image
         false);             // don't "own" it

    // create background surface
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
    myBGPosition = std::make_pair(0, 0);

//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("set cursor to: \"%s\"\n"),
//                RPG_Graphics_TypeHelper::RPG_Graphics_TypeToString(type_in).c_str()));

    myCurrentType = type_in;

    return;
  } // end IF

  // not in cache --> (try to) load graphic
  RPG_Graphics_t graphic;
  graphic.category = RPG_GRAPHICS_CATEGORY_INVALID;
  graphic.type = RPG_GRAPHICS_TYPE_INVALID;
  // retrieve properties from the dictionary
  graphic = RPG_GRAPHICS_DICTIONARY_SINGLETON::instance()->getGraphic(type_in);
  ACE_ASSERT(graphic.type == type_in);
  // sanity check
  if (graphic.category != CATEGORY_CURSOR)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid category (was: \"%s\"): \"%s\" not a cursor type, aborting\n"),
               RPG_Graphics_CategoryHelper::RPG_Graphics_CategoryToString(graphic.category).c_str(),
               RPG_Graphics_TypeHelper::RPG_Graphics_TypeToString(type_in).c_str()));

    return;
  } // end IF

  // assemble path
  std::string filename;
  RPG_Graphics_Common_Tools::graphicToFile(graphic, filename);
  ACE_ASSERT(!filename.empty());

  // load file
  SDL_Surface* surface = NULL;
  surface = RPG_Graphics_Surface::load(filename, // file
                                       true);    // convert to display format
  if (!surface)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Graphics_Surface::load(\"%s\"), aborting\n"),
               filename.c_str()));

    return;
  } // end IF

  // init base class
  init(surface, // image
       false);  // don't "own" it

  // update cache
  myCache.insert(std::make_pair(type_in, surface));

  // create background surface
  if (myBG)
  {
    SDL_FreeSurface(myBG);
    myBG = NULL;
  } // end IF
  myBG = RPG_Graphics_Surface::create(surface->w,
                                      surface->h);
  if (!myBG)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Graphics_Surface::create(%u,%u), aborting\n"),
               surface->w,
               surface->h));

    return;
  } // end IF
  myBGPosition = std::make_pair(0, 0);

  myCurrentType = type_in;

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("set cursor to: \"%s\"\n"),
//              RPG_Graphics_TypeHelper::RPG_Graphics_TypeToString(type_in).c_str()));
}

// SDL_Surface*
// RPG_Graphics_Cursor::get() const
// {
//   RPG_TRACE(ACE_TEXT("RPG_Graphics_Cursor::get"));
//
//   // sanity check(s)
//   ACE_ASSERT(mySurface);
//
//   return mySurface;
// }

void
RPG_Graphics_Cursor::put(const unsigned long& offsetX_in,
                         const unsigned long& offsetY_in,
                         SDL_Surface* targetSurface_in,
                         SDL_Rect& dirtyRegion_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Cursor::put"));

  // sanity check(s)
  ACE_ASSERT(mySurface);
  ACE_ASSERT(targetSurface_in);
//   ACE_ASSERT(offsetX_in < ACE_static_cast(unsigned long, targetSurface_in->w));
//   ACE_ASSERT(offsetY_in < ACE_static_cast(unsigned long, targetSurface_in->h));

  // step0: init return value(s)
  ACE_OS::memset(&dirtyRegion_out,
                 0,
                 sizeof(dirtyRegion_out));

  // step1: restore old background
  SDL_Rect bgRect;
  ACE_OS::memset(&bgRect,
                 0,
                 sizeof(bgRect));

  if ((myBGPosition.first != 0) &&
      (myBGPosition.second != 0))
  {
    // sanity check(s)
    ACE_ASSERT(myBG);

    bgRect.x = myBGPosition.first;
    bgRect.y = myBGPosition.second;
    bgRect.w = myBG->w;
    bgRect.h = myBG->h;
    // handle clipping
    if ((bgRect.x + bgRect.w) > targetSurface_in->w)
      bgRect.w -= ((bgRect.x + bgRect.w) - targetSurface_in->w);
    if ((bgRect.y + bgRect.h) > targetSurface_in->h)
      bgRect.h -= ((bgRect.y + bgRect.h) - targetSurface_in->h);

    // restore background
    if (SDL_BlitSurface(myBG,             // source
                        NULL,             // aspect (--> everything)
                        targetSurface_in, // target
                        &bgRect))         // aspect
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to SDL_BlitSurface(): %s, aborting\n"),
                 SDL_GetError()));

      return;
    } // end IF

    // *HACK*: somehow, SDL_BlitSurface zeroes bgRect.w, bgRect.h...
    // --> reset them
    bgRect.w = myBG->w;
    bgRect.h = myBG->h;
  } // end IF

  // step2: get new background
  RPG_Graphics_Surface::get(offsetX_in,
                            offsetY_in,
                            true, // use (fast) blitting method
                            *targetSurface_in,
                            *myBG);
  myBGPosition.first = offsetX_in;
  myBGPosition.second = offsetY_in;

  // compute bounding box
  dirtyRegion_out.x = offsetX_in;
  dirtyRegion_out.y = offsetY_in;
  dirtyRegion_out.w = mySurface->w;
  dirtyRegion_out.h = mySurface->h;
  // handle clipping
  if ((dirtyRegion_out.x + dirtyRegion_out.w) > targetSurface_in->w)
    dirtyRegion_out.w -= ((dirtyRegion_out.x + dirtyRegion_out.w) - targetSurface_in->w);
  if ((dirtyRegion_out.y + dirtyRegion_out.h) > targetSurface_in->h)
    dirtyRegion_out.h -= ((dirtyRegion_out.y + dirtyRegion_out.h) - targetSurface_in->h);

  // place cursor
  if (SDL_BlitSurface(mySurface,         // source
                      NULL,              // aspect (--> everything)
                      targetSurface_in,  // target
                      &dirtyRegion_out)) // aspect
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_BlitSurface(): %s, aborting\n"),
               SDL_GetError()));

    // clean up
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

  // *HACK*: somehow, SDL_BlitSurface zeroes dirtyRegion_out.w, dirtyRegion_out.h...
  // --> reset them
  dirtyRegion_out.w = mySurface->w;
  dirtyRegion_out.h = mySurface->h;

  // if necessary, adjust dirty region
  if ((bgRect.w != 0) &&
      (bgRect.h != 0))
    dirtyRegion_out = RPG_Graphics_SDL_Tools::boundingBox(bgRect,
                                                          dirtyRegion_out);
}

void
RPG_Graphics_Cursor::restoreBG(SDL_Surface* targetSurface_in,
                               SDL_Rect& dirtyRegion_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Cursor::restoreBG"));

  // sanity check(s)
  ACE_ASSERT(targetSurface_in);
  ACE_ASSERT(myBG);
//   ACE_ASSERT(myBGPosition.first != 0);
//   ACE_ASSERT(myBGPosition.second != 0);

  dirtyRegion_out.x = myBGPosition.first;
  dirtyRegion_out.y = myBGPosition.second;
  dirtyRegion_out.w = myBG->w;
  dirtyRegion_out.h = myBG->h;
  // handle clipping
  if ((dirtyRegion_out.x + dirtyRegion_out.w) > targetSurface_in->w)
    dirtyRegion_out.w -= ((dirtyRegion_out.x + dirtyRegion_out.w) - targetSurface_in->w);
  if ((dirtyRegion_out.y + dirtyRegion_out.h) > targetSurface_in->h)
    dirtyRegion_out.h -= ((dirtyRegion_out.y + dirtyRegion_out.h) - targetSurface_in->h);

  // restore/clear background
  if (SDL_BlitSurface(myBG,              // source
                      NULL,              // aspect (--> everything)
                      targetSurface_in,  // target
                      &dirtyRegion_out)) // aspect
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_BlitSurface(): %s, aborting\n"),
               SDL_GetError()));

    return;
  } // end IF

  // *HACK*: somehow, SDL_BlitSurface zeroes dirtyRegion_out.w, dirtyRegion_out.h...
  // --> reset them
  dirtyRegion_out.w = myBG->w;
  dirtyRegion_out.h = myBG->h;

  RPG_Graphics_Surface::clear(myBG);
}

void
RPG_Graphics_Cursor::invalidateBG()
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Cursor::invalidateBG"));

  // sanity check
  ACE_ASSERT(myBG);

  RPG_Graphics_Surface::clear(myBG);
}
