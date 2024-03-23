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

#include "rpg_graphics_cursor_manager.h"

#include <limits>
#include <sstream>

#include "ace/OS.h"
#include "ace/Log_Msg.h"

#include "rpg_common_macros.h"

#include "rpg_graphics_defines.h"
#include "rpg_graphics_iwindow.h"
#include "rpg_graphics_surface.h"
#include "rpg_graphics_dictionary.h"
#include "rpg_graphics_common_tools.h"
#include "rpg_graphics_SDL_tools.h"

RPG_Graphics_Cursor_Manager::RPG_Graphics_Cursor_Manager ()
 : myCurrentType (CURSOR_NORMAL),
   myCurrentGraphic (NULL),
//   myCache (),
   myBGPosition (std::make_pair (std::numeric_limits<unsigned int>::max (),
                                 std::numeric_limits<unsigned int>::max ())),
   myBG (NULL),
   myHighlightWindow (NULL),
//   myHighlightBGCache (),
   myHighlightTile (NULL),
   myScreenLock (NULL)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Cursor_Manager::RPG_Graphics_Cursor_Manager"));

  // initialize cursor highlighting
  SDL_Surface* tile_highlight_bg =
      RPG_Graphics_Surface::create (RPG_GRAPHICS_TILE_FLOOR_WIDTH,
                                    RPG_GRAPHICS_TILE_FLOOR_HEIGHT);
  if (!tile_highlight_bg)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to RPG_Graphics_Surface::create(%u,%u), continuing\n"),
                RPG_GRAPHICS_TILE_FLOOR_WIDTH,
                RPG_GRAPHICS_TILE_FLOOR_HEIGHT));
  myHighlightBGCache.push_back (std::make_pair (std::make_pair (std::numeric_limits<unsigned int>::max (),
                                                                std::numeric_limits<unsigned int>::max ()),
                                                tile_highlight_bg));

  struct RPG_Graphics_GraphicTypeUnion type;
  type.discriminator = RPG_Graphics_GraphicTypeUnion::TILEGRAPHIC;
  type.tilegraphic = TILE_CURSOR_HIGHLIGHT;
  myHighlightTile = RPG_Graphics_Common_Tools::loadGraphic (type,   // tile
                                                            true,   // convert to display format
                                                            false); // don't cache
  if (!myHighlightTile)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to RPG_Graphics_Common_Tools::loadGraphic(\"%s\"), continuing\n"),
                ACE_TEXT (RPG_Graphics_Common_Tools::toString (type).c_str ())));
}

RPG_Graphics_Cursor_Manager::~RPG_Graphics_Cursor_Manager ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Cursor_Manager::~RPG_Graphics_Cursor_Manager"));

  // clean up
  if (myBG)
    SDL_FreeSurface (myBG);

  for (RPG_Graphics_Cursor_CacheConstIterator_t iterator = myCache.begin ();
       iterator != myCache.end ();
       iterator++)
    SDL_FreeSurface ((*iterator).second);

  for (RPG_Graphics_TileCacheConstIterator_t iterator = myHighlightBGCache.begin ();
       iterator != myHighlightBGCache.end ();
       iterator++)
    SDL_FreeSurface ((*iterator).second);

  if (myHighlightTile)
    SDL_FreeSurface (myHighlightTile);
}

void
RPG_Graphics_Cursor_Manager::initialize (Common_ILock* screenLock_in,
                                         RPG_Graphics_IWindowBase* window_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Cursor_Manager::init"));

  ACE_ASSERT (window_in);

  myHighlightWindow = window_in;
  myScreenLock = screenLock_in;
}

void
RPG_Graphics_Cursor_Manager::reset (bool update_in,
                                    bool lockedAccess_in,
                                    bool debug_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Cursor_Manager::reset"));

  if (update_in)
  {
    // sanity check(s)
    ACE_ASSERT (myHighlightWindow);

    struct SDL_Rect dirty_region = {0, 0, 0, 0};
    updateBG (dirty_region,
              NULL,
              lockedAccess_in,
              debug_in);

    myHighlightWindow->invalidate (dirty_region);
  } // end IF
  else
    invalidateBG ();
  resetHighlightBG (std::make_pair (std::numeric_limits<unsigned int>::max (),
                                    std::numeric_limits<unsigned int>::max ()));
}

SDL_Rect
RPG_Graphics_Cursor_Manager::area (const RPG_Graphics_Position_t& viewport_in) const
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Cursor_Manager::area"));

  // sanity check(s)
  ACE_ASSERT (myHighlightWindow);

  struct SDL_Rect result = {0, 0, 0, 0};

  // step1: cursor background ?
  if (myBG)
  {
    struct SDL_Rect bg_area;
    bg_area.x = static_cast<Sint16> (myBGPosition.first);
    bg_area.y = static_cast<Sint16> (myBGPosition.second);
    bg_area.w = static_cast<Uint16> (myBG->w);
    bg_area.h = static_cast<Uint16> (myBG->h);
    result = RPG_Graphics_SDL_Tools::boundingBox (result,
                                                  bg_area);
  } // end IF

  // step2: highlight background(s)
  struct SDL_Rect highlight_area = {0, 0, 0, 0};
  RPG_Graphics_Position_t screen_position;

#if defined (SDL_USE)
  SDL_Surface* screen = myHighlightWindow->getScreen ();
#elif defined (SDL2_USE)
  SDL_Surface* screen = SDL_GetWindowSurface (myHighlightWindow->getScreen ());
#endif // SDL_USE || SDL2_USE
  ACE_ASSERT (screen);
  for (RPG_Graphics_TileCacheConstIterator_t iterator = myHighlightBGCache.begin ();
       iterator != myHighlightBGCache.end ();
       iterator++)
  {
    screen_position =
        RPG_Graphics_Common_Tools::mapToScreen ((*iterator).first,
                                                std::make_pair (screen->w,
                                                                screen->h),
                                                viewport_in);
    highlight_area.x = static_cast<Sint16> (screen_position.first);
    highlight_area.y = static_cast<Sint16> (screen_position.second);
    highlight_area.w = static_cast<Uint16> ((*iterator).second->w);
    highlight_area.h = static_cast<Uint16> ((*iterator).second->h);
    result = RPG_Graphics_SDL_Tools::boundingBox (result,
                                                  highlight_area);
  } // end FOR

  return result;
}

RPG_Graphics_Position_t
RPG_Graphics_Cursor_Manager::position (bool highlight_in) const
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Cursor_Manager::type"));

  if (highlight_in)
    return myHighlightBGCache.front().first;

  int x, y;
  Uint8 button_state = SDL_GetMouseState (&x, &y);
  ACE_UNUSED_ARG (button_state);

  return std::make_pair (static_cast<unsigned int> (x),
                         static_cast<unsigned int> (y));
}

void
RPG_Graphics_Cursor_Manager::put (const RPG_Graphics_Position_t& position_in,
                                  const RPG_Graphics_Position_t& view_in,
                                  const RPG_Map_Size_t& mapSize_in,
                                  struct SDL_Rect& dirtyRegion_out,
                                  bool drawHighlight_in,
                                  bool lockedAccess_in,
                                  bool debug_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Cursor_Manager::put"));

  // sanity check(s)
  ACE_ASSERT (myHighlightWindow);

  // initialize return value
  ACE_OS::memset (&dirtyRegion_out, 0, sizeof (struct SDL_Rect));

  RPG_Graphics_Position_t input_position =
    ((position_in != std::make_pair (std::numeric_limits<unsigned int>::max (),
                                     std::numeric_limits<unsigned int>::max ())) ? position_in
                                                                                 : position (false));

  // step1: draw highlight ?
  if (drawHighlight_in)
  {
    struct SDL_Rect window_area;
    myHighlightWindow->getArea (window_area, true);
    RPG_Map_Position_t map_position =
      RPG_Graphics_Common_Tools::screenToMap (input_position,
                                              mapSize_in,
                                              std::make_pair (window_area.w,
                                                              window_area.h),
                                              view_in);
    RPG_Map_PositionList_t positions;
    positions.push_back (map_position);
    RPG_Graphics_Offsets_t screen_positions;
    screen_positions.push_back (RPG_Graphics_Common_Tools::mapToScreen (map_position,
                                                                        std::make_pair (window_area.w,
                                                                                        window_area.h),
                                                                        view_in));
    putHighlights (positions,
                   screen_positions,
                   view_in,
                   dirtyRegion_out,
                   lockedAccess_in,
                   debug_in);
  } // end IF

  // step2: draw cursor
  struct SDL_Rect dirty_region;
  putCursor (input_position,
             dirty_region,
             lockedAccess_in,
             debug_in);

  dirtyRegion_out = RPG_Graphics_SDL_Tools::boundingBox (dirtyRegion_out,
                                                         dirty_region);
}

void
RPG_Graphics_Cursor_Manager::setCursor (const RPG_Graphics_Cursor& type_in,
                                        struct SDL_Rect& dirtyRegion_out,
                                        bool lockedAccess_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Cursor_Manager::setCursor"));

  // step0: initialize return value(s)
  ACE_OS::memset (&dirtyRegion_out, 0, sizeof (struct SDL_Rect));

  // step1: load graphic (check cache first)
  RPG_Graphics_Cursor_CacheConstIterator_t iterator = myCache.find (type_in);
  if (iterator != myCache.end ())
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
    graphic = RPG_GRAPHICS_DICTIONARY_SINGLETON::instance ()->get (type);
    ACE_ASSERT((graphic.type.cursor == type_in) &&
               (graphic.type.discriminator == RPG_Graphics_GraphicTypeUnion::CURSOR));
    // sanity check
    if (graphic.category != CATEGORY_CURSOR)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid category (was: \"%s\"): \"%s\" not a cursor type, returning\n"),
                  ACE_TEXT (RPG_Graphics_CategoryHelper::RPG_Graphics_CategoryToString (graphic.category).c_str ()),
                  ACE_TEXT (RPG_Graphics_CursorHelper::RPG_Graphics_CursorToString (type_in).c_str ())));
      return;
    } // end IF

    // assemble path
    std::string filename;
    RPG_Graphics_Common_Tools::graphicToFile (graphic, filename);
    ACE_ASSERT (!filename.empty ());

    // load file
    myCurrentGraphic = NULL;
    myCurrentGraphic = RPG_Graphics_Surface::load (filename, // file
                                                   true);    // convert to display format
    if (!myCurrentGraphic)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to RPG_Graphics_Surface::load(\"%s\"), returning\n"),
                  ACE_TEXT (filename.c_str ())));
      return;
    } // end IF

    // update cache
    myCache.insert (std::make_pair (type_in, myCurrentGraphic));

    iterator = myCache.find (type_in);
    ACE_ASSERT(iterator != myCache.end ());
  } // end ELSE

  // step1: restore old background
  if (myScreenLock && lockedAccess_in)
    myScreenLock->lock ();
  restoreBG (dirtyRegion_out,
             NULL,
             false);
  if (myScreenLock && lockedAccess_in)
    myScreenLock->unlock ();

  // step2: create background surface
  if (myBG)
  {
    SDL_FreeSurface (myBG);
    myBG = NULL;
  } // end IF
  myBG = RPG_Graphics_Surface::create ((*iterator).second->w,
                                       (*iterator).second->h);
  if (!myBG)
  {
    ACE_DEBUG ((LM_ERROR,
                 ACE_TEXT ("failed to RPG_Graphics_Surface::create(%u,%u), returning\n"),
                 (*iterator).second->w, (*iterator).second->h));
    return;
  } // end IF
  myBGPosition = std::make_pair (std::numeric_limits<unsigned int>::max (),
                                 std::numeric_limits<unsigned int>::max ());

//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("set cursor to: \"%s\"\n"),
//                ACE_TEXT(RPG_Graphics_TypeHelper::RPG_Graphics_TypeToString(type_in).c_str())));

  myCurrentType = type_in;
}

void
RPG_Graphics_Cursor_Manager::putCursor (const RPG_Graphics_Offset_t& offset_in,
                                        struct SDL_Rect& dirtyRegion_out,
                                        bool lockedAccess_in,
                                        bool debug_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Cursor_Manager::putCursor"));

  // step0: initialize return value(s)
  ACE_OS::memset (&dirtyRegion_out, 0, sizeof (struct SDL_Rect));

  // sanity check(s)
  ACE_ASSERT(myHighlightWindow);
#if defined (SDL_USE)
  SDL_Surface* target_surface = myHighlightWindow->getScreen ();
#elif defined (SDL2_USE)
  SDL_Surface* target_surface =
    SDL_GetWindowSurface (myHighlightWindow->getScreen ());
#endif // SDL_USE || SDL2_USE
  ACE_ASSERT (target_surface);
  if ((offset_in.first  >= target_surface->w) ||
      (offset_in.second >= target_surface->h))
    return; // nothing to do
  ACE_ASSERT(myCurrentGraphic);

  // step1: restore old background
  if (myScreenLock && lockedAccess_in)
    myScreenLock->lock ();
  restoreBG(dirtyRegion_out,
            NULL,
            false);
  if (myScreenLock && lockedAccess_in)
    myScreenLock->unlock ();

  // step2: get new background
  RPG_Graphics_Surface::get (offset_in,
                             true, // use (fast) blitting method
                             *target_surface,
                             *myBG);

  myBGPosition = offset_in;
  SDL_Rect dirty_region;
  if (debug_in)
  {
    // step1: show coordinates in top border
    std::ostringstream converter;
    std::string text = ACE_TEXT_ALWAYS_CHAR ("[");
    converter << offset_in.first;
    text += converter.str ();
    text += ACE_TEXT_ALWAYS_CHAR (",");
    converter.str (ACE_TEXT_ALWAYS_CHAR (""));
    converter.clear ();
    converter << offset_in.second;
    text += converter.str ();
    text += ACE_TEXT_ALWAYS_CHAR ("]");
    RPG_Graphics_TextSize_t text_size =
        RPG_Graphics_Common_Tools::textSize (FONT_MAIN_SMALL,
                                             text);
    RPG_Graphics_IWindowBase* parent = myHighlightWindow->getParent ();
    unsigned int border_top, dummy;
    parent->getBorders (border_top,
                        dummy,
                        dummy,
                        dummy);
    struct SDL_Rect window_area;
    myHighlightWindow->getArea (window_area,
                                true); // toplevel- ?
    static RPG_Graphics_TextSize_t text_size_bg = std::make_pair (0, 0);
    static SDL_Surface* coordinates_bg = NULL;
    if (!coordinates_bg)
    {
      text_size_bg =
          RPG_Graphics_Common_Tools::textSize (FONT_MAIN_SMALL,
                                               ACE_TEXT_ALWAYS_CHAR ("[0000,0000]"));
      coordinates_bg = RPG_Graphics_Surface::create (text_size_bg.first,
                                                     text_size_bg.second);
      RPG_Graphics_Surface::get (std::make_pair ((((window_area.w / 2) -
                                                   text_size_bg.first) / 2),
                                                 ((border_top - text_size_bg.second) / 2)),
                                 true,
                                 *target_surface,
                                 *coordinates_bg);
    } // end IF
    else
    {
      RPG_Graphics_Surface::put (std::make_pair((((window_area.w / 2) -
                                                  text_size_bg.first) / 2),
                                                ((border_top - text_size_bg.second) / 2)),
                                 *coordinates_bg,
                                 target_surface,
                                 dirty_region);
      dirtyRegion_out = RPG_Graphics_SDL_Tools::boundingBox (dirty_region,
                                                             dirtyRegion_out);
    } // end ELSE
//    RPG_Graphics_Surface::unclip();
    if (myScreenLock && lockedAccess_in)
      myScreenLock->lock ();
    RPG_Graphics_Surface::putText (FONT_MAIN_SMALL,
                                   text,
                                   RPG_Graphics_SDL_Tools::colorToSDLColor (RPG_Graphics_SDL_Tools::getColor (RPG_GRAPHICS_FONT_DEF_COLOR,
                                                                                                              *target_surface->format,
                                                                                                              1.0f),
                                                                            *target_surface),
                                   true, // add shade
                                   RPG_Graphics_SDL_Tools::colorToSDLColor (RPG_Graphics_SDL_Tools::getColor (RPG_GRAPHICS_FONT_DEF_SHADECOLOR,
                                                                                                              *target_surface->format,
                                                                                                              1.0f),
                                                                            *target_surface),
                                   std::make_pair ((((window_area.w / 2) - text_size.first) / 2),
                                                   ((border_top - text_size.second) / 2)),
                                   target_surface,
                                   dirty_region);

    // step2: show bg surface in left upper corner
    ACE_ASSERT (target_surface->w >= myBG->w);
    ACE_ASSERT (target_surface->h >= myBG->h);

    // compute bounding box
    dirty_region.x = 0;
    dirty_region.y = 0;
    dirty_region.w = myBG->w;
    dirty_region.h = myBG->h;

    RPG_Graphics_Surface::put (std::make_pair (dirty_region.x,
                                               dirty_region.y),
                               *myBG,
                               target_surface,
                               dirty_region);
    if (myScreenLock && lockedAccess_in)
      myScreenLock->unlock ();
//    RPG_Graphics_Surface::clip();

    dirtyRegion_out = RPG_Graphics_SDL_Tools::boundingBox (dirtyRegion_out,
                                                           dirty_region);
  } // end IF

  // step3: place cursor
//	RPG_Graphics_Surface::unclip();
  if (myScreenLock && lockedAccess_in)
    myScreenLock->lock ();
  RPG_Graphics_Surface::put (offset_in,
                             *myCurrentGraphic,
                             target_surface,
                             dirty_region);
  if (myScreenLock && lockedAccess_in)
    myScreenLock->unlock ();
//	RPG_Graphics_Surface::clip();

  dirtyRegion_out = RPG_Graphics_SDL_Tools::boundingBox (dirtyRegion_out,
                                                         dirty_region);
}

void
RPG_Graphics_Cursor_Manager::restoreBG (struct SDL_Rect& dirtyRegion_out,
                                        const struct SDL_Rect* clipRectangle_in,
                                        bool lockedAccess_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Cursor_Manager::restoreBG"));

  // step0: initialize return value(s)
  ACE_OS::memset (&dirtyRegion_out, 0, sizeof (struct SDL_Rect));

  // sanity check(s)
  if (!myBG                                                      ||
      (myBGPosition ==
       std::make_pair (std::numeric_limits<unsigned int>::max (),
                       std::numeric_limits<unsigned int>::max ())))
    return; // nothing to do
  ACE_ASSERT (myHighlightWindow);
#if defined (SDL_USE)
  SDL_Surface* target_surface = myHighlightWindow->getScreen ();
#elif defined (SDL2_USE)
  SDL_Surface* target_surface =
    SDL_GetWindowSurface (myHighlightWindow->getScreen ());
#endif // SDL_USE || SDL2_USE
  ACE_ASSERT (target_surface);

  // init dirty region
  dirtyRegion_out.x = static_cast<int16_t> (myBGPosition.first);
  dirtyRegion_out.y = static_cast<int16_t> (myBGPosition.second);
  dirtyRegion_out.w = static_cast<uint16_t> (myBG->w);
  dirtyRegion_out.h = static_cast<uint16_t> (myBG->h);
  SDL_Rect clip_rectangle;
  myHighlightWindow->getArea (clip_rectangle, true);
  dirtyRegion_out = RPG_Graphics_SDL_Tools::intersect (dirtyRegion_out,
                                                        clip_rectangle);
  clip_rectangle = dirtyRegion_out;
  SDL_Rect source_clip_rectangle = {0, 0, dirtyRegion_out.w, dirtyRegion_out.h};
  if (clipRectangle_in)
  {
    clip_rectangle = RPG_Graphics_SDL_Tools::intersect (*clipRectangle_in,
                                                        clip_rectangle);
    source_clip_rectangle = clip_rectangle;
    source_clip_rectangle.x -= myBGPosition.first;
    source_clip_rectangle.y -= myBGPosition.second;
  } // end IF
  // sanity check
  if (!clip_rectangle.w || !clip_rectangle.h)
    return; // nothing to do...
  if (lockedAccess_in && myScreenLock)
    myScreenLock->lock();
  // restore / clear background
  if (SDL_BlitSurface (myBG,                   // source
                       &source_clip_rectangle, // aspect
                       target_surface,         // target
                       &clip_rectangle))       // position, result
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_BlitSurface(): \"%s\", continuing\n"),
                ACE_TEXT (SDL_GetError ())));
  if (lockedAccess_in && myScreenLock)
    myScreenLock->unlock ();
}

void
RPG_Graphics_Cursor_Manager::updateBG (struct SDL_Rect& dirtyRegion_out,
                                       const struct SDL_Rect* clipRect_in,
                                       bool lockedAccess_in,
                                       bool debug_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Cursor_Manager::updateBG"));

  // sanity check(s)
  ACE_ASSERT(myBG);
  ACE_ASSERT(myHighlightWindow);
#if defined (SDL_USE)
  SDL_Surface* target_surface = myHighlightWindow->getScreen ();
#elif defined (SDL2_USE)
  SDL_Surface* target_surface =
    SDL_GetWindowSurface (myHighlightWindow->getScreen ());
#endif // SDL_USE || SDL2_USE
  ACE_ASSERT (target_surface);

  // init return value(s)
  ACE_OS::memset (&dirtyRegion_out, 0, sizeof (struct SDL_Rect));

  // *NOTE*: this function handles two distinct use-cases:
  // 1. the map window has changed (clipRect_in == NULL) --> update that
  //    bit (if any) of the cached bg
  // 2. a part of the screen has changed (clipRect_in != NULL)
  // --> update that bit (if any) of the cached bg

  // step1: set clip area
  struct SDL_Rect clip_area = {0, 0, 0, 0};
  if (clipRect_in == NULL)
    myHighlightWindow->getArea (clip_area,
                                false); // toplevel- ?
  else
    clip_area = *clipRect_in;

  // step2: intersect with cached bg
  struct SDL_Rect cached_area;
  cached_area.x = static_cast<int16_t> (myBGPosition.first);
  cached_area.y = static_cast<int16_t> (myBGPosition.second);
  cached_area.w = myBG->w;
  cached_area.h = myBG->h;
  struct SDL_Rect intersection = {0, 0, 0, 0};
  intersection =
      RPG_Graphics_SDL_Tools::intersect (cached_area,
                                         clip_area);
  // sanity check(s)
  if ((intersection.w == 0) || // cached bg fully outside of "dirty" area ?
      (intersection.h == 0))
    return; // --> nothing to do...
  if ((intersection.w == cached_area.w) &&
      (intersection.h == cached_area.h))
  {
    // cached cursor bg fully inside of map
    // --> just get a fresh copy
    RPG_Graphics_Surface::get (myBGPosition,
                               true, // use (fast) blitting method
                               *target_surface,
                               *myBG);

    // debug info
    if (debug_in)
    {
      // show bg surface in left upper corner
      ACE_ASSERT (target_surface->w >= myBG->w);
      ACE_ASSERT (target_surface->h >= myBG->h);

      // compute bounding box
      struct SDL_Rect dirty_region;
      dirty_region.x = 0;
      dirty_region.y = 0;
      dirty_region.w = myBG->w;
      dirty_region.h = myBG->h;

//      RPG_Graphics_Surface::unclip();
      if (myScreenLock && lockedAccess_in)
        myScreenLock->lock ();
      RPG_Graphics_Surface::put (std::make_pair (dirty_region.x,
                                                 dirty_region.y),
                                 *myBG,
                                 target_surface,
                                 dirty_region);
      if (myScreenLock && lockedAccess_in)
        myScreenLock->unlock ();
//      RPG_Graphics_Surface::clip();
    } // end IF

    return; // --> done...
  } // end IF

  // step3: adjust intersection coordinates (relative to cached bg surface)
  intersection.x -= cached_area.x;
  intersection.y -= cached_area.y;

  // step4: get a fresh copy from that part of the map
  SDL_Surface* new_bg =
    RPG_Graphics_Surface::create (static_cast<unsigned int> (myBG->w),
                                  static_cast<unsigned int> (myBG->h));
  ACE_ASSERT (new_bg);
  RPG_Graphics_Surface::get (myBGPosition,
                             true, // use (fast) blitting method
                             *target_surface,
                             *new_bg);

  // step5: mask the "dirty" bit of the cached bg
  if (SDL_FillRect (myBG,
                    &intersection,
                    RPG_Graphics_SDL_Tools::getColor (COLOR_BLACK_A0, // transparent
                                                      *myBG->format,
                                                      1.0f)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_FillRect(): \"%s\", aborting\n"),
                ACE_TEXT (SDL_GetError ())));
    SDL_FreeSurface (new_bg);
    return;
  } // end IF

  // step6: blit the cached bg onto the fresh copy
  if (SDL_BlitSurface (myBG,   // source
                       NULL,   // aspect (--> everything)
                       new_bg, // target
                       NULL))  // aspect (--> everything)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_BlitSurface(): %s, aborting\n"),
                ACE_TEXT (SDL_GetError ())));

    // clean up
    SDL_FreeSurface (new_bg);

    return;
  } // end IF

  // clean up
  SDL_FreeSurface (myBG);

  myBG = new_bg;

  // debug info
  if (debug_in)
  {
    // show bg surface in left upper corner
    ACE_ASSERT (target_surface->w >= myBG->w);
    ACE_ASSERT (target_surface->h >= myBG->h);

    // compute bounding box
    struct SDL_Rect dirty_region;
    dirty_region.x = 0;
    dirty_region.y = 0;
    dirty_region.w = myBG->w;
    dirty_region.h = myBG->h;

//    RPG_Graphics_Surface::unclip();
    if (myScreenLock && lockedAccess_in)
      myScreenLock->lock ();
    RPG_Graphics_Surface::put (std::make_pair (dirty_region.x,
                                               dirty_region.y),
                               *myBG,
                               target_surface,
                               dirty_region);
    if (myScreenLock && lockedAccess_in)
      myScreenLock->unlock ();
//    RPG_Graphics_Surface::clip();
  } // end IF
}

void
RPG_Graphics_Cursor_Manager::invalidateBG (const struct SDL_Rect* clipRect_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Cursor_Manager::invalidateBG"));

  // sanity check
  ACE_ASSERT (myBG);

  RPG_Graphics_Surface::clear (myBG,
                               clipRect_in);
}

RPG_Graphics_Position_t
RPG_Graphics_Cursor_Manager::getHighlightBGPosition (const unsigned int& index_in) const
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Cursor_Manager::getHighlightBGPosition"));

  // sanity checks
  if (myHighlightBGCache.empty () ||
      ((index_in + 1) > myHighlightBGCache.size ()))
    return std::make_pair (std::numeric_limits<unsigned int>::max (),
                           std::numeric_limits<unsigned int>::max ());

  if (index_in == std::numeric_limits<unsigned int>::max ())
    return myHighlightBGCache.back ().first; // return LAST element

  return myHighlightBGCache[index_in].first;
}

void
RPG_Graphics_Cursor_Manager::putHighlights (const RPG_Map_PositionList_t& mapPositions_in,
                                            const RPG_Graphics_Offsets_t& graphicsPositions_in,
                                            const RPG_Graphics_Position_t& viewPort_in,
                                            struct SDL_Rect& dirtyRegion_out,
                                            bool lockedAccess_in,
                                            bool debug_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Cursor_Manager::putHighlights"));

  // step0: initialize return value(s)
  ACE_OS::memset (&dirtyRegion_out, 0, sizeof (struct SDL_Rect));

  // sanity check
  ACE_ASSERT (myHighlightTile);
  ACE_ASSERT (myHighlightWindow);
#if defined (SDL_USE)
  SDL_Surface* target_surface = myHighlightWindow->getScreen ();
#elif defined (SDL2_USE)
  SDL_Surface* target_surface =
    SDL_GetWindowSurface (myHighlightWindow->getScreen ());
#endif // SDL_USE || SDL2_USE
  ACE_ASSERT (target_surface);

  // step1: restore old backgrounds
  if (myScreenLock && lockedAccess_in)
    myScreenLock->lock ();
  restoreHighlightBG (viewPort_in,
                      dirtyRegion_out,
                      NULL,
                      false,
                      debug_in);

  // step2: get new backgrounds
  SDL_Rect dirty_region;
  storeHighlightBG (mapPositions_in,
                    graphicsPositions_in,
                    dirty_region,
                    false);
  dirtyRegion_out = RPG_Graphics_SDL_Tools::boundingBox (dirty_region,
                                                         dirtyRegion_out);

  // step3: place highlights
  std::vector<struct SDL_Rect> clip_rectangles;
  SDL_Rect clip_rectangle = {static_cast<Sint16> (myBGPosition.first),
                             static_cast<Sint16> (myBGPosition.second),
                             static_cast<Uint16> (myBG->w),
                             static_cast<Uint16> (myBG->h)}, temp_rectangle;
  myHighlightWindow->clip ();
  for (RPG_Graphics_OffsetsConstIterator_t iterator = graphicsPositions_in.begin ();
       iterator != graphicsPositions_in.end ();
       iterator++)
  {
    RPG_Graphics_Surface::put (*iterator,
                               *myHighlightTile,
                               target_surface,
                               dirty_region);
    dirtyRegion_out = RPG_Graphics_SDL_Tools::boundingBox (dirty_region,
                                                           dirtyRegion_out);
    temp_rectangle = RPG_Graphics_SDL_Tools::intersect (clip_rectangle,
                                                        dirty_region);
    if (temp_rectangle.w || temp_rectangle.h)
      clip_rectangles.push_back (temp_rectangle);
  } // end FOR

  if (debug_in)
  {
    RPG_Graphics_Offset_t screen_position = graphicsPositions_in.front ();

    // step1: show coordinates in top border
    std::ostringstream converter;
    std::string text = ACE_TEXT_ALWAYS_CHAR ("[");
    converter << screen_position.first;
    text += converter.str ();
    text += ACE_TEXT_ALWAYS_CHAR (",");
    converter.str (ACE_TEXT_ALWAYS_CHAR (""));
    converter.clear ();
    converter << screen_position.second;
    text += converter.str ();
    text += ACE_TEXT_ALWAYS_CHAR ("]");
    RPG_Graphics_TextSize_t text_size =
        RPG_Graphics_Common_Tools::textSize (FONT_MAIN_SMALL,
                                             text);
    RPG_Graphics_IWindowBase* parent = myHighlightWindow->getParent ();
    unsigned int border_top, dummy;
    parent->getBorders (border_top,
                        dummy,
                        dummy,
                        dummy);
    struct SDL_Rect window_area;
    myHighlightWindow->getArea (window_area,
                                true); // toplevel- ?
    static RPG_Graphics_TextSize_t highlight_text_size_bg =
        std::make_pair (0, 0);
    static SDL_Surface* highlight_coordinates_bg = NULL;
    if (!highlight_coordinates_bg)
    {
      highlight_text_size_bg =
          RPG_Graphics_Common_Tools::textSize (FONT_MAIN_SMALL,
                                               ACE_TEXT_ALWAYS_CHAR ("[0000,0000]"));
      highlight_coordinates_bg =
          RPG_Graphics_Surface::create (highlight_text_size_bg.first,
                                        highlight_text_size_bg.second);
      RPG_Graphics_Surface::get (std::make_pair ((((window_area.w * 3) / 4) -
                                                  (highlight_text_size_bg.first / 2)),
                                                 ((border_top - highlight_text_size_bg.second) / 2)),
                                 true,
                                 *target_surface,
                                 *highlight_coordinates_bg);
    } // end IF
    else
    {
      RPG_Graphics_Surface::put (std::make_pair ((((window_area.w * 3) / 4) -
                                                  (highlight_text_size_bg.first / 2)),
                                                 ((border_top - highlight_text_size_bg.second) / 2)),
                                 *highlight_coordinates_bg,
                                 target_surface,
                                 dirty_region);
      dirtyRegion_out = RPG_Graphics_SDL_Tools::boundingBox (dirty_region,
                                                             dirtyRegion_out);
    } // end ELSE
    RPG_Graphics_Surface::putText (FONT_MAIN_SMALL,
                                   text,
                                   RPG_Graphics_SDL_Tools::colorToSDLColor (RPG_Graphics_SDL_Tools::getColor (RPG_GRAPHICS_FONT_DEF_COLOR,
                                                                                                              *target_surface->format,
                                                                                                              1.0f),
                                                                            *target_surface),
                                   true, // add shade
                                   RPG_Graphics_SDL_Tools::colorToSDLColor (RPG_Graphics_SDL_Tools::getColor (RPG_GRAPHICS_FONT_DEF_SHADECOLOR,
                                                                                                              *target_surface->format,
                                                                                                              1.0f),
                                                                            *target_surface),
                                   std::make_pair ((((window_area.w * 3) / 4) -
                                                    (text_size.first / 2)),
                                                   ((border_top - text_size.second) / 2)),
                                   target_surface,
                                   dirty_region);
    dirtyRegion_out = RPG_Graphics_SDL_Tools::boundingBox (dirty_region,
                                                           dirtyRegion_out);

    // step2: show highlight bg in top right corner
    SDL_Surface* highlight_bg = myHighlightBGCache.front ().second;
    ACE_ASSERT (target_surface->w >= highlight_bg->w);
    ACE_ASSERT (target_surface->h >= highlight_bg->h);

#if defined (SDL_USE)
    RPG_Graphics_Surface::unclip ();
#elif defined (SDL2_USE)
    RPG_Graphics_Surface::unclip (myHighlightWindow->getScreen ());
#endif // SDL_USE || SDL2_USE
    RPG_Graphics_Surface::put (std::make_pair ((target_surface->w - highlight_bg->w),
                                               0),
                               *highlight_bg,
                               target_surface,
                               dirty_region);
#if defined (SDL_USE)
    RPG_Graphics_Surface::clip ();
#elif defined (SDL2_USE)
    RPG_Graphics_Surface::clip (myHighlightWindow->getScreen ());
#endif // SDL_USE || SDL2_USE

    dirtyRegion_out = RPG_Graphics_SDL_Tools::boundingBox (dirty_region,
                                                           dirtyRegion_out);
  } // end IF

  // step4: update (part(s) of) cursor bg
  for (std::vector<struct SDL_Rect>::const_iterator iterator = clip_rectangles.begin ();
       iterator != clip_rectangles.end ();
       iterator++)
  {
    updateBG (dirty_region,
              &(*iterator),
              false,
              debug_in);
    dirtyRegion_out = RPG_Graphics_SDL_Tools::boundingBox (dirty_region,
                                                           dirtyRegion_out);
  } // end FOR
  if (myScreenLock && lockedAccess_in)
    myScreenLock->unlock ();
  myHighlightWindow->unclip ();
}

void
RPG_Graphics_Cursor_Manager::storeHighlightBG (const RPG_Map_PositionList_t& mapPositions_in,
                                               const RPG_Graphics_Offsets_t& graphicsPositions_in,
                                               struct SDL_Rect& dirtyRegion_out,
                                               bool lockedAccess_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Cursor_Manager::storeHighlightBG"));

  // sanity check(s)
  ACE_ASSERT(myHighlightWindow);
#if defined (SDL_USE)
  SDL_Surface* target_surface = myHighlightWindow->getScreen ();
#elif defined (SDL2_USE)
  SDL_Surface* target_surface =
    SDL_GetWindowSurface (myHighlightWindow->getScreen ());
#endif // SDL_USE || SDL2_USE
  ACE_ASSERT (target_surface);
  if (mapPositions_in.empty ())
    return; // nothing to do
  ACE_ASSERT (mapPositions_in.size () == graphicsPositions_in.size ());

  // init return value(s)
  ACE_OS::memset (&dirtyRegion_out, 0, sizeof (struct SDL_Rect));

  // grow/shrink cache as necessary
  ACE_INT64 delta = myHighlightBGCache.size () - mapPositions_in.size ();
  if (delta > 0)
  {
    for (ACE_INT64 i = delta;
         i > 0;
         i--)
    {
      SDL_FreeSurface (myHighlightBGCache.back ().second);
      myHighlightBGCache.pop_back ();
    } // end FOR
  } // end IF
  else if (delta < 0)
  {
    SDL_Surface* new_entry = NULL;
    for (ACE_INT64 i = -delta;
         i > 0;
         i--)
    {
      new_entry = RPG_Graphics_Surface::create (RPG_GRAPHICS_TILE_FLOOR_WIDTH,
                                                RPG_GRAPHICS_TILE_FLOOR_HEIGHT);
      if (!new_entry)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to RPG_Graphics_Surface::create(%u,%u), returning\n"),
                    RPG_GRAPHICS_TILE_FLOOR_WIDTH,
                    RPG_GRAPHICS_TILE_FLOOR_HEIGHT));
        return;
      } // end IF
      myHighlightBGCache.push_back (std::make_pair (std::make_pair (std::numeric_limits<unsigned int>::max (),
                                                                    std::numeric_limits<unsigned int>::max ()),
                                                    new_entry));
    } // end IF
  } // end ELSEIF
  ACE_ASSERT (myHighlightBGCache.size () == mapPositions_in.size ());

  // step1: restore (part of) the cursor bg so it is not included. This is safe,
  // as the highlight is only redrawn when the cursor moves --> the cursor will
  // (later) be redrawn anyway...
  // --> clip to the highlight area !
  RPG_Graphics_OffsetsConstIterator_t graphics_position_iterator =
    graphicsPositions_in.begin ();
  SDL_Rect clip_rectangle = {static_cast<Sint16> ((*graphics_position_iterator).first),
                             static_cast<Sint16> ((*graphics_position_iterator).second),
                             static_cast<Uint16> (myHighlightTile->w),
                             static_cast<Uint16> (myHighlightTile->h)};
  SDL_Rect temp_rectangle = {0, 0,
                             static_cast<Uint16> (myHighlightTile->w),
                             static_cast<Uint16> (myHighlightTile->h)};
  for (++graphics_position_iterator;
       graphics_position_iterator != graphicsPositions_in.end ();
       graphics_position_iterator++)
  {
    temp_rectangle.x = static_cast<Sint16> ((*graphics_position_iterator).first);
    temp_rectangle.y = static_cast<Sint16> ((*graphics_position_iterator).second);
    clip_rectangle = RPG_Graphics_SDL_Tools::boundingBox (clip_rectangle,
                                                          temp_rectangle);
  } // end FOR
  if (lockedAccess_in && myScreenLock)
    myScreenLock->lock ();
  restoreBG (dirtyRegion_out,
             &clip_rectangle,
             false);
  if (lockedAccess_in && myScreenLock)
    myScreenLock->unlock ();

  RPG_Graphics_TileCacheIterator_t cache_iterator = myHighlightBGCache.begin ();
  graphics_position_iterator = graphicsPositions_in.begin ();
//  myHighlightWindow->clip();
  for (RPG_Map_PositionListConstIterator_t map_position_iterator = mapPositions_in.begin ();
       map_position_iterator != mapPositions_in.end ();
       map_position_iterator++, graphics_position_iterator++, cache_iterator++)
  {
    RPG_Graphics_Surface::clear ((*cache_iterator).second);
    RPG_Graphics_Surface::get (*graphics_position_iterator,
                               true, // use (fast) blitting method
                               *target_surface,
                               *(*cache_iterator).second);
    (*cache_iterator).first = *map_position_iterator;
  } // end FOR
//  myHighlightWindow->unclip();
}

void
RPG_Graphics_Cursor_Manager::restoreHighlightBG (const RPG_Graphics_Position_t& viewPort_in,
                                                 struct SDL_Rect& dirtyRegion_out,
                                                 const struct SDL_Rect* clipRectangle_in,
                                                 bool lockedAccess_in,
                                                 bool debug_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Cursor_Manager::restoreHighlightBG"));

  // step0: initialize return value(s)
  ACE_OS::memset (&dirtyRegion_out, 0, sizeof (struct SDL_Rect));

  // sanity check(s)
  ACE_ASSERT (myHighlightWindow);
#if defined (SDL_USE)
  SDL_Surface* target_surface = myHighlightWindow->getScreen ();
#elif defined (SDL2_USE)
  SDL_Surface* target_surface =
    SDL_GetWindowSurface (myHighlightWindow->getScreen ());
#endif // SDL_USE || SDL2_USE
  ACE_ASSERT (target_surface);
  ACE_ASSERT (!myHighlightBGCache.empty ());
  if ((myHighlightBGCache.size () == 1) &&
      (myHighlightBGCache.front ().first == std::make_pair (std::numeric_limits<unsigned int>::max (),
                                                            std::numeric_limits<unsigned int>::max ())))
    return; // nothing to do

  struct SDL_Rect window_area, clip_area, clip_rectangle, source_clip_rectangle;
  myHighlightWindow->getArea (window_area,
                              true); // toplevel- ?
  myHighlightWindow->getArea (clip_area,
                              false); // toplevel- ?
  if (clipRectangle_in)
    clip_area = *clipRectangle_in;
  RPG_Graphics_Position_t screen_position;
  SDL_Rect cursor_clip_rectangle = {static_cast<Sint16> (myBGPosition.first),
                                    static_cast<Sint16> (myBGPosition.second),
                                    static_cast<Uint16> (myBG->w),
                                    static_cast<Uint16> (myBG->h)}, temp_rectangle;
  std::vector<SDL_Rect> cursor_relevant_clip_rectangles;
  myHighlightWindow->clip ();
  if (lockedAccess_in && myScreenLock)
    myScreenLock->lock ();
  for (RPG_Graphics_TileCacheConstIterator_t iterator = myHighlightBGCache.begin ();
       iterator != myHighlightBGCache.end ();
       iterator++)
  {
    screen_position =
        RPG_Graphics_Common_Tools::mapToScreen ((*iterator).first,
                                                std::make_pair (window_area.w,
                                                                window_area.h),
                                                viewPort_in);

    source_clip_rectangle.x = screen_position.first;
    source_clip_rectangle.y = screen_position.second;
    source_clip_rectangle.w = (*iterator).second->w;
    source_clip_rectangle.h = (*iterator).second->h;
    clip_rectangle = RPG_Graphics_SDL_Tools::intersect (source_clip_rectangle,
                                                        clip_area);
    dirtyRegion_out = RPG_Graphics_SDL_Tools::boundingBox (dirtyRegion_out,
                                                           clip_rectangle);
    source_clip_rectangle.x = clip_rectangle.x - screen_position.first;
    source_clip_rectangle.y = clip_rectangle.y - screen_position.second;
    if (!clip_rectangle.w || !clip_rectangle.h)
        continue; // nothing to do...

    temp_rectangle = clip_rectangle;
    // restore / clear background
    if (SDL_BlitSurface ((*iterator).second,     // source
                         &source_clip_rectangle, // aspect
                         target_surface,         // target
                         &clip_rectangle))       // aspect
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to SDL_BlitSurface(): \"%s\", continuing\n"),
                  ACE_TEXT (SDL_GetError ())));

    temp_rectangle = RPG_Graphics_SDL_Tools::intersect (cursor_clip_rectangle,
                                                        temp_rectangle);
    if (temp_rectangle.w || temp_rectangle.h)
      cursor_relevant_clip_rectangles.push_back (temp_rectangle);
  } // end FOR
  if (lockedAccess_in && myScreenLock)
    myScreenLock->unlock ();
  myHighlightWindow->unclip ();

  // *NOTE*: iff the highlight bg(s) intersected the cursor bg, an update is
  // needed of (that/those bit(s) of) the cursor bg...
  for (std::vector<struct SDL_Rect>::const_iterator iterator = cursor_relevant_clip_rectangles.begin ();
       iterator != cursor_relevant_clip_rectangles.end ();
       iterator++)
  {
    updateBG (temp_rectangle,
              &(*iterator),
              lockedAccess_in,
              debug_in);
    if (debug_in)
      dirtyRegion_out = RPG_Graphics_SDL_Tools::boundingBox (temp_rectangle,
                                                             dirtyRegion_out);
  } // end FOR
}

void
RPG_Graphics_Cursor_Manager::updateHighlightBG (const RPG_Graphics_Position_t& viewPort_in, 
                                                struct SDL_Rect& dirtyRegion_out,
                                                const struct SDL_Rect* clipRectangle_in,
                                                bool lockedAccess_in,
                                                bool debug_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Cursor_Manager::updateHighlightBG"));

  // sanity check(s)
  ACE_ASSERT (myHighlightWindow);
#if defined (SDL_USE)
  SDL_Surface* target_surface = myHighlightWindow->getScreen ();
#elif defined (SDL2_USE)
  SDL_Surface* target_surface =
    SDL_GetWindowSurface (myHighlightWindow->getScreen ());
#endif // SDL_USE || SDL2_USE
  ACE_ASSERT (target_surface);

  // init return value(s)
  ACE_OS::memset (&dirtyRegion_out, 0, sizeof (struct SDL_Rect));

  struct SDL_Rect window_area, clip_area, clip_rectangle, source_clip_rectangle;
  myHighlightWindow->getArea (window_area,
                              true); // toplevel- ?
  myHighlightWindow->getArea (clip_area,
                              false); // toplevel- ?
  if (clipRectangle_in)
      clip_area = *clipRectangle_in;
  RPG_Graphics_Position_t screen_position;
  SDL_Surface* new_background = NULL;
  if (myScreenLock && lockedAccess_in && debug_in)
      myScreenLock->lock ();
  for (RPG_Graphics_TileCacheIterator_t iterator = myHighlightBGCache.begin ();
           iterator != myHighlightBGCache.end ();
           iterator++)
  {
    screen_position =
        RPG_Graphics_Common_Tools::mapToScreen ((*iterator).first,
                                                std::make_pair (window_area.w,
                                                                window_area.h),
                                                viewPort_in);
    source_clip_rectangle.x = screen_position.first;
    source_clip_rectangle.y = screen_position.second;
    source_clip_rectangle.w = (*iterator).second->w;
    source_clip_rectangle.h = (*iterator).second->h;
    clip_rectangle = RPG_Graphics_SDL_Tools::intersect (clip_area,
                                                        source_clip_rectangle);
    if (!clip_rectangle.w || !clip_rectangle.h)
      continue; // nothing to do...

    if ((clip_rectangle.w == (*iterator).second->w) &&
        (clip_rectangle.h == (*iterator).second->h))
    {
      // cached highlight bg completely "dirty"
      // --> just get a fresh copy
      RPG_Graphics_Surface::get (screen_position,
                                 true, // use (fast) blitting method
                                 *target_surface,
                                 *(*iterator).second);

      // debug info
      if (debug_in)
      {
        // show bg surface in left upper corner
        ACE_ASSERT (target_surface->w >= (*iterator).second->w);
        ACE_ASSERT (target_surface->h >= (*iterator).second->h);

        // compute bounding box
        struct SDL_Rect dirty_region;
        dirty_region.x = target_surface->w - (*iterator).second->w;
        dirty_region.y = target_surface->h - (*iterator).second->h;
        dirty_region.w = (*iterator).second->w;
        dirty_region.h = (*iterator).second->h;

        RPG_Graphics_Surface::put (std::make_pair (dirty_region.x,
                                                   dirty_region.y),
                                   *(*iterator).second,
                                   target_surface,
                                   dirty_region);
        dirtyRegion_out = RPG_Graphics_SDL_Tools::boundingBox (dirty_region,
                                                              dirtyRegion_out);
      } // end IF

      continue;
    } // end IF

    // get a fresh copy from that part of the map
    new_background =
        RPG_Graphics_Surface::create (static_cast<unsigned int> ((*iterator).second->w),
                                      static_cast<unsigned int> ((*iterator).second->h));
    if (!new_background)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to RPG_Graphics_Surface::create(%u,%u), continuing\n"),
                  (*iterator).second->w, (*iterator).second->h));
      continue;
    } // end IF
    RPG_Graphics_Surface::get (screen_position,
                               true, // use (fast) blitting method
                               *target_surface,
                               *new_background);

    // mask the "dirty" bit of the cached bg
    // --> adjust intersection coordinates (relative to cached bg surface)
    clip_rectangle.x -= screen_position.first;
    clip_rectangle.y -= screen_position.second;
    if (SDL_FillRect ((*iterator).second,
                      &clip_rectangle,
                      RPG_Graphics_SDL_Tools::getColor (COLOR_BLACK_A0, // transparent
                                                        *(*iterator).second->format,
                                                        1.0f)))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to SDL_FillRect(): \"%s\", continuing\n"),
                  ACE_TEXT (SDL_GetError ())));
      SDL_FreeSurface (new_background);
      continue;
    } // end IF

    // blit the cached/masked bg onto the fresh copy
    if (SDL_BlitSurface ((*iterator).second, // source
                         NULL,               // aspect (--> everything)
                         new_background,     // target
                         NULL))              // aspect (--> everything)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to SDL_BlitSurface(): %s, continuing\n"),
                  ACE_TEXT (SDL_GetError ())));

      // clean up
      SDL_FreeSurface(new_background);

      continue;
    } // end IF

    // clean up
    SDL_FreeSurface((*iterator).second);

    (*iterator).second = new_background;

    // debug info
    if (debug_in)
    {
      // show bg surface in left upper corner
      ACE_ASSERT (target_surface->w >= (*iterator).second->w);
      ACE_ASSERT (target_surface->h >= (*iterator).second->h);

      // compute bounding box
      struct SDL_Rect dirty_region;
      dirty_region.x = 0;
      dirty_region.y = 0;
      dirty_region.w = (*iterator).second->w;
      dirty_region.h = (*iterator).second->h;

      RPG_Graphics_Surface::put (std::make_pair (dirty_region.x,
                                                 dirty_region.y),
                                 *(*iterator).second,
                                 target_surface,
                                 dirty_region);
      dirtyRegion_out = RPG_Graphics_SDL_Tools::boundingBox (dirty_region,
                                                             dirtyRegion_out);
    } // end IF
  } // end FOR
  if (myScreenLock && lockedAccess_in && debug_in)
    myScreenLock->unlock ();
}

void
RPG_Graphics_Cursor_Manager::resetHighlightBG (const RPG_Graphics_Position_t& position_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Cursor_Manager::resetHighlightBG"));

  // init/clear highlight BG cache
  while (myHighlightBGCache.size () > 1)
  {
    SDL_FreeSurface (myHighlightBGCache.back ().second);
    myHighlightBGCache.pop_back ();
  } // end WHILE
  myHighlightBGCache.front ().first = position_in;
  RPG_Graphics_Surface::clear (myHighlightBGCache.front ().second);
}
