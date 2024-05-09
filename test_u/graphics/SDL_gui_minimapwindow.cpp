/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2011  <copyright holder> <email>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "stdafx.h"

#include "SDL_gui_minimapwindow.h"

#include "ace/Log_Msg.h"
#include "ace/OS.h"

#include "common_file_tools.h"

#include "rpg_client_common.h"
#include "rpg_client_defines.h"
#include "rpg_client_engine.h"

#include "rpg_engine.h"

#include "rpg_graphics_common.h"
#include "rpg_graphics_defines.h"
#include "rpg_graphics_surface.h"
#include "rpg_graphics_common_tools.h"
#include "rpg_graphics_SDL_tools.h"

#include "rpg_map_common.h"

#include "rpg_common_macros.h"
#include "rpg_common_defines.h"
#include "rpg_common_file_tools.h"

SDL_GUI_MinimapWindow::SDL_GUI_MinimapWindow(const RPG_Graphics_SDLWindowBase& parent_in,
                                             // *NOTE*: offset doesn't include any border(s) !
                                             const RPG_Graphics_Offset_t& offset_in,
                                             RPG_Engine* engine_in)
 : inherited (WINDOW_MINIMAP,             // type
              parent_in,                  // parent
              offset_in,                  // offset
              ACE_TEXT_ALWAYS_CHAR ("")), // title
   myEngine (engine_in),
   myState (NULL),
   myBG (NULL),
   mySurface (NULL)
{
  RPG_TRACE (ACE_TEXT ("SDL_GUI_MinimapWindow::SDL_GUI_MinimapWindow"));

  // load interface image
  struct RPG_Graphics_GraphicTypeUnion type;
  type.discriminator = RPG_Graphics_GraphicTypeUnion::IMAGE;
  type.image = IMAGE_INTERFACE_MINIMAP;
  myBG = RPG_Graphics_Common_Tools::loadGraphic (type,
                                                 true,   // convert to display format
                                                 false); // don't cache this one
  ACE_ASSERT (myBG);

  mySurface = RPG_Graphics_Surface::copy (*myBG);
  ACE_ASSERT (mySurface);

  // adjust position, size
  struct SDL_Rect parent_area;
  ACE_ASSERT (inherited::parent_);
  inherited::parent_->getArea (parent_area,
                               false); // toplevel- ?
  clipRectangle_.x =
      ((offset_in.first == std::numeric_limits<int>::max ()) ? ((parent_area.x + parent_area.w) -
                                                                myBG->w                         -
                                                                RPG_CLIENT_MINIMAP_DEF_OFFSET_X)
                                                             : offset_in.first);
  clipRectangle_.y =
      ((offset_in.second == std::numeric_limits<int>::max ()) ? (parent_area.y                   +
                                                                 RPG_CLIENT_MINIMAP_DEF_OFFSET_Y)
                                                              : offset_in.second);
  clipRectangle_.w = mySurface->w;
  clipRectangle_.h = mySurface->h;
}

SDL_GUI_MinimapWindow::~SDL_GUI_MinimapWindow ()
{
  RPG_TRACE(ACE_TEXT("SDL_GUI_MinimapWindow::~SDL_GUI_MinimapWindow"));

  // clean up
  SDL_FreeSurface (myBG);
  SDL_FreeSurface (mySurface);
}

RPG_Graphics_Position_t
SDL_GUI_MinimapWindow::getView() const
{
  RPG_TRACE (ACE_TEXT ("SDL_GUI_MinimapWindow::getView"));

  ACE_ASSERT (false);

  return std::make_pair (0, 0);
}

void
SDL_GUI_MinimapWindow::handleEvent (const SDL_Event& event_in,
                                    RPG_Graphics_IWindowBase* window_in,
                                    struct SDL_Rect& dirtyRegion_out)
{
  RPG_TRACE (ACE_TEXT ("SDL_GUI_MinimapWindow::handleEvent"));

  // initialize return value(s)
  ACE_OS::memset (&dirtyRegion_out, 0, sizeof (struct SDL_Rect));

  switch (event_in.type)
  {
    // *** mouse ***
    case SDL_MOUSEBUTTONDOWN:
    {
      // *WARNING*: falls through !
    }
    case RPG_GRAPHICS_SDL_MOUSEMOVEOUT:
    {

      break;
    }
    case SDL_KEYDOWN:
    case SDL_KEYUP:
    case SDL_MOUSEBUTTONUP:
    case SDL_MOUSEMOTION:
    case SDL_JOYAXISMOTION:
    case SDL_JOYBALLMOTION:
    case SDL_JOYHATMOTION:
    case SDL_JOYBUTTONDOWN:
    case SDL_JOYBUTTONUP:
    case SDL_QUIT:
    case SDL_SYSWMEVENT:
#if defined (SDL_USE)
    case SDL_ACTIVEEVENT:
    case SDL_VIDEORESIZE:
    case SDL_VIDEOEXPOSE:
#elif defined (SDL2_USE)
    case SDL_WINDOWEVENT_SHOWN:
    case SDL_WINDOWEVENT_RESIZED:
    case SDL_WINDOWEVENT_EXPOSED:
#endif // SDL_USE || SDL2_USE
    case SDL_USEREVENT:
    case RPG_GRAPHICS_SDL_HOVEREVENT:
    {
      // delegate these to the parent...
      getParent ()->handleEvent (event_in,
                                 window_in,
                                 dirtyRegion_out);

      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("received unknown event (was: %u)...\n"),
                  static_cast<unsigned int> (event_in.type)));
      break;
    }
  } // end SWITCH
}

void
SDL_GUI_MinimapWindow::initialize (state_t* state_in,
                                   Common_ILock* screenLock_in,
                                   bool flip_in)
{
  RPG_TRACE (ACE_TEXT ("SDL_GUI_MinimapWindow::initialize"));

  // sanity check(s)
  ACE_ASSERT (state_in);
  ACE_ASSERT (state_in->screen);

  myState = state_in;

  inherited::initialize (screenLock_in,
                         flip_in);
}

void
SDL_GUI_MinimapWindow::draw (SDL_Surface* targetSurface_in,
                             unsigned int offsetX_in,
                             unsigned int offsetY_in)
{
  RPG_TRACE (ACE_TEXT ("SDL_GUI_MinimapWindow::draw"));

  // sanity check(s)
  ACE_ASSERT (inherited::screen_);
#if defined (SDL_USE)
  SDL_Surface* surface_p = inherited::screen_;
#elif defined (SDL2_USE)
  SDL_Surface* surface_p = SDL_GetWindowSurface (inherited::screen_);
#endif // SDL_USE || SDL2_USE
  ACE_ASSERT (surface_p);
  SDL_Surface* target_surface = (targetSurface_in ? targetSurface_in
                                                  : surface_p);
  ACE_ASSERT (target_surface);
  ACE_UNUSED_ARG (offsetX_in);
  ACE_UNUSED_ARG (offsetY_in);
  ACE_ASSERT (myEngine);
  ACE_ASSERT (mySurface);

  // initialize surface
  struct SDL_Rect dirty_region;
  RPG_Graphics_Surface::put (std::make_pair (0, 0),
                             *myBG,
                             mySurface,
                             dirty_region);

  // lock surface during pixel access
  if (SDL_MUSTLOCK ((mySurface)))
    if (SDL_LockSurface (mySurface))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to SDL_LockSurface(): \"%s\", returning"),
                  ACE_TEXT (SDL_GetError ())));
      return;
    } // end IF
  RPG_Map_Position_t map_position = std::make_pair (0, 0);
  RPG_Client_MiniMapTile tile = RPG_CLIENT_MINIMAPTILE_INVALID;
  RPG_Graphics_ColorName color_name = RPG_GRAPHICS_COLORNAME_INVALID;
  Uint32 color = 0;
  SDL_Rect destination_rectangle = {0, 0, 3, 2};
  Uint32* pixels = NULL;
  RPG_Engine_EntityID_t entity_id = 0, active_entity_id;
  float blend_factor = 1.0f;
  myEngine->lock ();
  RPG_Map_Size_t size = myEngine->getSize (false); // locked access ?
  active_entity_id = myEngine->getActive (false);  // locked access ?
  RPG_Map_Positions_t visible_positions;
  RPG_Map_PositionsConstIterator_t visible_iterator;
  //RPG_Engine_SeenPositionsConstIterator_t has_seen_iterator =
  //  myState->seen_positions.end ();
  //RPG_Map_PositionsConstIterator_t has_seen_iterator_2;
  //myState->lock.acquire ();
  bool has_active_entity_seen_b = false;
  if (active_entity_id)
  {
    myEngine->getVisiblePositions (active_entity_id,
                                   visible_positions,
                                   false); // locked access ?
    //has_seen_iterator = myState->seen_positions.find (active_entity_id);
  } // end IF
  enum RPG_Map_Element map_element;
  for (unsigned int y = 0;
       y < size.second;
       y++)
    for (unsigned int x = 0;
         x < size.first;
         x++)
    {
      map_position = std::make_pair (x, y);

      if (active_entity_id)
        has_active_entity_seen_b = myEngine->hasSeen (active_entity_id,
                                                      map_position,
                                                      false); // locked access ?

      // step1: retrieve appropriate symbol
      tile = RPG_CLIENT_MINIMAPTILE_INVALID;
      entity_id = myEngine->hasEntity (map_position,
                                       false); // locked access ?
      if (entity_id)
      {
        if (entity_id == active_entity_id)
          tile = MINIMAPTILE_PLAYER_ACTIVE;
        else if (!myEngine->isMonster (entity_id,
                                       false)) // locked access ?
          tile = MINIMAPTILE_PLAYER;
        else
          tile = MINIMAPTILE_MONSTER;
      } // end IF
      else
      {
        map_element = myEngine->getElement (map_position,
                                            false); // locked access ?
        switch (map_element)
        {
          case MAPELEMENT_DOOR:
            tile = MINIMAPTILE_DOOR; break;
          case MAPELEMENT_FLOOR:
            tile = MINIMAPTILE_FLOOR; break;
          case MAPELEMENT_STAIRS:
            tile = MINIMAPTILE_STAIRS; break;
          case MAPELEMENT_UNMAPPED:
            tile = MINIMAPTILE_NONE; break;
          case MAPELEMENT_WALL:
            tile = MINIMAPTILE_WALL; break;
          default:
          {
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("invalid map element ([%u,%u] was: %d), continuing\n"),
                        x, y,
                        map_element));
            continue;
          }
        } // end SWITCH
      } // end IF

      // step2: map symbol to color
      color_name = RPG_GRAPHICS_COLORNAME_INVALID;
      switch (tile)
      {
        case MINIMAPTILE_DOOR:
          color_name = RPG_CLIENT_MINIMAPCOLOR_DOOR; break;
        case MINIMAPTILE_FLOOR:
          color_name = RPG_CLIENT_MINIMAPCOLOR_FLOOR; break;
        case MINIMAPTILE_MONSTER:
          color_name = RPG_CLIENT_MINIMAPCOLOR_MONSTER; break;
        case MINIMAPTILE_NONE:
          color_name = RPG_CLIENT_MINIMAPCOLOR_UNMAPPED; break;
        case MINIMAPTILE_PLAYER:
          color_name = RPG_CLIENT_MINIMAPCOLOR_PLAYER; break;
        case MINIMAPTILE_PLAYER_ACTIVE:
          color_name = RPG_CLIENT_MINIMAPCOLOR_PLAYER_ACTIVE; break;
        case MINIMAPTILE_STAIRS:
          color_name = RPG_CLIENT_MINIMAPCOLOR_STAIRS; break;
        case MINIMAPTILE_WALL:
          color_name = RPG_CLIENT_MINIMAPCOLOR_WALL; break;
        default:
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("invalid minimap tile type (was: %d), continuing\n"),
                      tile));
          continue;
        }
      } // end SWITCH
      blend_factor = 1.0f; // --> opaque
      if (color_name != RPG_CLIENT_MINIMAPCOLOR_UNMAPPED)
      {
        // handle vision
        visible_iterator = visible_positions.find (map_position);
        if ((visible_iterator == visible_positions.end ()) && // --> position not currently visible
            //(has_seen_iterator != myState->seen_positions.end ()) &&
            !myState->debug)
        {
          // seen previously ?
          //has_seen_iterator_2 = (*has_seen_iterator).second.find (map_position);
          //if (has_seen_iterator_2 == (*has_seen_iterator).second.end ())
          if (!has_active_entity_seen_b)
            color_name = RPG_CLIENT_MINIMAPCOLOR_UNMAPPED;
          else
            blend_factor = RPG_GRAPHICS_TILE_PREVSEEN_DEF_OPACITY;
        } // end IF
      } // end IF
      color = RPG_Graphics_SDL_Tools::getColor (color_name,
                                                *mySurface->format,
                                                blend_factor);

      // step3: draw tile onto surface
      // *NOTE*: a minimap symbol has this shape: _ C _
      //                                          C C C
      // where "_" is transparent and "C" is a colored pixel
      ACE_ASSERT(mySurface->format->BytesPerPixel == 4);

      // step3a: row 1
      destination_rectangle.x = 40 + (2 * x) - (2 * y);
      destination_rectangle.y = x + y;
      pixels = reinterpret_cast<Uint32*> (static_cast<char*>(mySurface->pixels) +
                                          (mySurface->pitch * (destination_rectangle.y + 6)) +
                                          ((destination_rectangle.x + 6) * 4));
//       pixels[0] = transparent -> dont write
      pixels[1] = color;
//       pixels[2] = transparent -> dont write
      // step3b: row 2
      pixels = reinterpret_cast<Uint32*> (static_cast<char*>(mySurface->pixels) +
                                          (mySurface->pitch * (destination_rectangle.y + 7)) +
                                          ((destination_rectangle.x + 6) * 4));
      pixels[0] = color;
      pixels[1] = color;
      pixels[2] = color;
    } // end FOR
  //myState->lock.release();
  myEngine->unlock ();
  if (SDL_MUSTLOCK (mySurface))
    SDL_UnlockSurface (mySurface);

  // step4: save BG ?
  if (!inherited::BGHasBeenSaved_)
    inherited::saveBG (inherited::clipRectangle_);

  // step5: paint surface
  inherited::clip ();
  if (inherited::screenLock_)
    inherited::screenLock_->lock();
  RPG_Graphics_Surface::put (std::make_pair (inherited::clipRectangle_.x,
                                             inherited::clipRectangle_.y),
                             *mySurface,
                             target_surface,
                             dirty_region);
  if (inherited::screenLock_)
    inherited::screenLock_->unlock();
  inherited::unclip ();

  // invalidate dirty region
  inherited::invalidate (dirty_region);

  // remember position of last realization
  lastAbsolutePosition_ = std::make_pair (inherited::clipRectangle_.x,
                                          inherited::clipRectangle_.y);

  inherited::isVisible_ = true;

  // debug info
  if (myState->debug)
  {
    std::string path = Common_File_Tools::getTempDirectory ();
    path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
    path += ACE_TEXT_ALWAYS_CHAR ("minimap.png");
    RPG_Graphics_Surface::savePNG (*mySurface,
                                   path,
                                   true); // save alpha ?
  } // end IF
}
