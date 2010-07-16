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

#include <ace/Log_Msg.h>

RPG_Graphics_Cursor::RPG_Graphics_Cursor()
 : inherited(),
   myCursorBGPosition(std::make_pair(0, 0)),
   myCursorBG(NULL)//,
//    myCache()
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Cursor::RPG_Graphics_Cursor"));

}

RPG_Graphics_Cursor::~RPG_Graphics_Cursor()
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Cursor::~RPG_Graphics_Cursor"));

  // clean up
  if (myCursorBG)
    SDL_FreeSurface(myCursorBG);

  for (RPG_Graphics_CursorCacheConstIterator_t iterator = myCache.begin();
       iterator != myCache.end();
       iterator++)
    SDL_FreeSurface((*iterator).second);
}

void
RPG_Graphics_Cursor::set(const RPG_Graphics_Type& type_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Cursor::set"));

  // check cache first
  RPG_Graphics_CursorCacheConstIterator_t iterator = myCache.find(type_in);
  if (iterator != myCache.end())
  {
    init((*iterator).second, // image
         false);             // don't "own" it

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
               RPG_Graphics_TypeHelper::RPG_Graphics_TypeToString(graphic.type).c_str()));

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
}

SDL_Surface*
RPG_Graphics_Cursor::get() const
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Cursor::get"));

  // sanity check(s)
  ACE_ASSERT(mySurface);

  return mySurface;
}

void
RPG_Graphics_Cursor::put(const unsigned long& offsetX_in,
                         const unsigned long& offsetY_in,
                         SDL_Surface* targetSurface_in,
                         SDL_Rect& dirtyRegion_out)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Cursor::put"));

  // sanity check(s)
  ACE_ASSERT(mySurface);
  ACE_ASSERT(targetSurface_in);
//   ACE_ASSERT(offsetX_in < ACE_static_cast(unsigned long, targetSurface_in->w));
//   ACE_ASSERT(offsetY_in < ACE_static_cast(unsigned long, targetSurface_in->h));

  // step1: reset old, get new background
  SDL_Rect bgRect;
  bgRect.x = 0;
  bgRect.y = 0;
  bgRect.w = 0;
  bgRect.h = 0;
  if (myCursorBG)
  {
    bgRect.x = myCursorBGPosition.first;
    bgRect.y = myCursorBGPosition.second;
    bgRect.w = myCursorBG->w;
    bgRect.h = myCursorBG->h;

    // restore background
    if (SDL_BlitSurface(myCursorBG,       // source
                        NULL,             // aspect (--> everything)
                        targetSurface_in, // target
                        &bgRect))         // aspect
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to SDL_BlitSurface(): %s, aborting\n"),
                 SDL_GetError()));

      // clean up
      dirtyRegion_out.x = 0;
      dirtyRegion_out.y = 0;
      dirtyRegion_out.w = 0;
      dirtyRegion_out.h = 0;

      return;
    } // end IF

    // clean up
    myCursorBGPosition = std::make_pair(0, 0);
    SDL_FreeSurface(myCursorBG);
    myCursorBG = NULL;
  } // end IF
  myCursorBG = RPG_Graphics_Surface::get(offsetX_in,
                                         offsetY_in,
                                         mySurface->w,
                                         mySurface->h,
                                         *targetSurface_in);
  if (!myCursorBG)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Graphics_Surface::get(%u,%u,%d,%d,%@), continuing\n"),
               offsetX_in,
               offsetY_in,
               mySurface->w,
               mySurface->h,
               targetSurface_in));
  } // end IF
  myCursorBGPosition.first = offsetX_in;
  myCursorBGPosition.second = offsetY_in;

  // compute bounding box
  dirtyRegion_out.x = offsetX_in;
  dirtyRegion_out.y = offsetY_in;
  dirtyRegion_out.w = mySurface->w;
  dirtyRegion_out.h = mySurface->h;

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
    dirtyRegion_out.x = 0;
    dirtyRegion_out.y = 0;
    dirtyRegion_out.w = 0;
    dirtyRegion_out.h = 0;

    return;
  } // end IF

  // if necessary, adjust dirty region
  if (bgRect.w && bgRect.h)
    dirtyRegion_out = RPG_Graphics_SDL_Tools::boundingBox(bgRect,
                                                          dirtyRegion_out);
}