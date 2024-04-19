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

#include "rpg_client_window_minimap.h"

#include "ace/Log_Msg.h"

#include "common_file_tools.h"

#include "rpg_common_defines.h"
#include "rpg_common_macros.h"

#include "rpg_map_common.h"

#include "rpg_engine.h"

#include "rpg_graphics_common.h"
#include "rpg_graphics_common_tools.h"
#include "rpg_graphics_defines.h"
#include "rpg_graphics_surface.h"
#include "rpg_graphics_SDL_tools.h"

#include "rpg_client_common.h"
#include "rpg_client_defines.h"
#include "rpg_client_engine.h"

RPG_Client_Window_MiniMap::RPG_Client_Window_MiniMap (const RPG_Graphics_SDLWindowBase& parent_in,
                                                      // *NOTE*: offset doesn't include any border(s) !
                                                      const RPG_Graphics_Offset_t& offset_in,
                                                      bool debug_in)
 : inherited (WINDOW_MINIMAP,            // type
              parent_in,                 // parent
              offset_in,                 // offset
              ACE_TEXT_ALWAYS_CHAR ("")) // title
 , myClient (NULL)
 , myEngine (NULL)
 , myDebug (debug_in)
 , myBG (NULL)
 , mySurface (NULL)
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Window_MiniMap::RPG_Client_Window_MiniMap"));

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
  //parent_in.getArea (parent_area, false);
  parent_in.getArea (parent_area,
                     true); // toplevel ?
  parent_in.getBorders (borderTop_,
                        borderBottom_,
                        borderLeft_,
                        borderRight_,
                        true); // recursive ?
  inherited::clipRectangle_.x =
    ((offset_in.first == std::numeric_limits<int>::max ()) ? ((parent_area.w - 1)            -
                                                              borderRight_                   -
                                                              //(parent_area.x + parent_area.w) -
                                                              myBG->w                        -
                                                              RPG_CLIENT_MINIMAP_DEF_OFFSET_X)
                                                           : offset_in.first);
  inherited::clipRectangle_.y =
    ((offset_in.second == std::numeric_limits<int>::max ()) ? (parent_area.y                  +
                                                               borderTop_                     +
                                                               RPG_CLIENT_MINIMAP_DEF_OFFSET_Y)
                                                            : offset_in.second);
  inherited::clipRectangle_.w = mySurface->w;
  inherited::clipRectangle_.h = mySurface->h;

  inherited::borderTop_ = inherited::borderBottom_ = inherited::borderLeft_ = inherited::borderRight_ = 0;
}

RPG_Client_Window_MiniMap::~RPG_Client_Window_MiniMap ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Window_MiniMap::~RPG_Client_Window_MiniMap"));

  // clean up
  SDL_FreeSurface (myBG);
  SDL_FreeSurface (mySurface);
}

void
RPG_Client_Window_MiniMap::handleEvent (const union SDL_Event& event_in,
                                        RPG_Graphics_IWindowBase* window_in,
                                        struct SDL_Rect& dirtyRegion_out)
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Window_MiniMap::handleEvent"));

  // initialize return value(s)
  ACE_OS::memset (&dirtyRegion_out, 0, sizeof (struct SDL_Rect));

  switch (event_in.type)
  {
    // *** mouse ***
    case RPG_GRAPHICS_SDL_MOUSEMOVEOUT:
      break;
    case SDL_MOUSEMOTION:
    {

      // *WARNING*: falls through !
    }
    case SDL_KEYDOWN:
    case SDL_KEYUP:
    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
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
    default:
    {
      // delegate these to the parent...
      getParent ()->handleEvent (event_in,
                                 window_in,
                                 dirtyRegion_out);

      break;
    }
  } // end SWITCH
}

void
RPG_Client_Window_MiniMap::draw (SDL_Surface* targetSurface_in,
                                 unsigned int offsetX_in,
                                 unsigned int offsetY_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Window_MiniMap::draw"));

  // sanity check(s)
  ACE_ASSERT (inherited::screen_);
#if defined (SDL_USE)
  SDL_Surface* surface_p = inherited::screen_;
#elif defined (SDL2_USE)
  SDL_Surface* surface_p = SDL_GetWindowSurface (inherited::screen_);
#endif // SDL_USE || SDL2_USE
  ACE_ASSERT (surface_p);
  ACE_ASSERT (myClient);
  ACE_ASSERT (myEngine);
  ACE_ASSERT (mySurface);
  SDL_Surface* target_surface =
    (targetSurface_in ? targetSurface_in : surface_p);
  ACE_ASSERT (target_surface);
  ACE_UNUSED_ARG (offsetX_in);
  ACE_UNUSED_ARG (offsetY_in);

  // initialize clipping
  inherited::clip (NULL, 0, 0);

  // initialize surface
  struct SDL_Rect dirty_region = {0, 0, 0, 0};
  // lock surface during pixel access
  if (SDL_MUSTLOCK ((mySurface)))
    if (SDL_LockSurface (mySurface))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to SDL_LockSurface(): \"%s\", returning\n"),
                  ACE_TEXT (SDL_GetError ())));
      return;
    } // end IF
  RPG_Graphics_Surface::put (std::make_pair (0, 0),
                             *myBG,
                             target_surface,
                             dirty_region);

  RPG_Map_Position_t map_position;
  enum RPG_Client_MiniMapTile tile;
  enum RPG_Graphics_ColorName color_name;
  Uint32 color;
  struct SDL_Rect destrect = {0, 0, 3, 2};
  Uint32* pixels;
  RPG_Engine_EntityID_t entity_id;
  myEngine->lock ();
  RPG_Engine_EntityID_t active_entity_id = myEngine->getActive (false); // locked access ?
  RPG_Map_Size_t map_size = myEngine->getSize (false); // locked access ?
  for (unsigned int y = 0;
       y < map_size.second;
       y++)
    for (unsigned int x = 0;
         x < map_size.first;
         x++)
    {
      // step1: retrieve appropriate symbol
      map_position = std::make_pair (x, y);

      tile = RPG_CLIENT_MINIMAPTILE_INVALID;
 
      if (!active_entity_id)
      {
        tile = MINIMAPTILE_NONE;
        goto continue_;
      } // end IF

      entity_id = myEngine->hasEntity (map_position,
                                       false); // locked access ?
      if (entity_id)
      {
        if (entity_id == active_entity_id)
          tile = MINIMAPTILE_PLAYER_ACTIVE;
        else if (myEngine->canSee (active_entity_id,
                                   map_position,
                                   false)) // locked access ?
          tile = (myEngine->isMonster (entity_id,
                                       false) ? MINIMAPTILE_MONSTER // locked access ?
                                              : MINIMAPTILE_PLAYER);
      } // end IF

      if (tile == RPG_CLIENT_MINIMAPTILE_INVALID)
      {
        if (myEngine->hasSeen (active_entity_id,
                               map_position,
                               false)) // locked access ?
        {
          enum RPG_Map_Element map_element =
            myEngine->getElement (map_position,
                                  false); // locked access ?
          switch (map_element)
          {
            case MAPELEMENT_UNMAPPED:
              tile = MINIMAPTILE_NONE; break;
            case MAPELEMENT_WALL:
              tile = MINIMAPTILE_WALL; break;
            case MAPELEMENT_FLOOR:
              tile = MINIMAPTILE_FLOOR; break;
            case MAPELEMENT_STAIRS:
              tile = MINIMAPTILE_STAIRS; break;
            case MAPELEMENT_DOOR:
              tile = MINIMAPTILE_DOOR; break;
            default:
            {
              ACE_DEBUG ((LM_ERROR,
                          ACE_TEXT ("invalid/unknown map element ([%u,%u] was: %d), continuing\n"),
                          x, y,
                          map_element));
              tile = MINIMAPTILE_NONE;
              break;
            }
          } // end SWITCH
        } // end IF
        else
          tile = MINIMAPTILE_NONE;
      } // end IF
continue_:
      ACE_ASSERT (tile != RPG_CLIENT_MINIMAPTILE_INVALID);

      // step2a: map symbol to color
      color_name = RPG_GRAPHICS_COLORNAME_INVALID;
      switch (tile)
      {
        case MINIMAPTILE_NONE:
          color_name = RPG_CLIENT_MINIMAPCOLOR_UNMAPPED; break;
        case MINIMAPTILE_DOOR:
          color_name = RPG_CLIENT_MINIMAPCOLOR_DOOR; break;
        case MINIMAPTILE_FLOOR:
          color_name = RPG_CLIENT_MINIMAPCOLOR_FLOOR; break;
        case MINIMAPTILE_MONSTER:
          color_name = RPG_CLIENT_MINIMAPCOLOR_MONSTER; break;
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
                      ACE_TEXT ("invalid/unknown minimap tile type (was: %d), continuing\n"),
                      tile));
          color_name = RPG_CLIENT_MINIMAPCOLOR_UNMAPPED;
          break;
        }
      } // end SWITCH
      ACE_ASSERT (color_name != RPG_GRAPHICS_COLORNAME_INVALID);

      // step2b: convert color
      color =
        RPG_Graphics_SDL_Tools::getColor (color_name,
                                          *mySurface->format,
                                          1.0f);

      // step3: draw tile onto surface
      // *NOTE*: a minimap symbol has this shape: _ C _
      //                                          C C C
      // where "_" is transparent and "C" is a colored pixel
      ACE_ASSERT (mySurface->format->BytesPerPixel == 4);

      // step3a: row 1
      destrect.x = 40 + (2 * x) - (2 * y);
      destrect.y = x + y;
      pixels = reinterpret_cast<Uint32*> (static_cast<char*> (mySurface->pixels) +
                                          (mySurface->pitch * (destrect.y + 6)) +
                                          ((destrect.x + 6) * mySurface->format->BytesPerPixel));
//       pixels[0] = transparent -> don't write
      pixels[1] = color;
//       pixels[2] = transparent -> don't write
      // step3b: row 2
      pixels = reinterpret_cast<Uint32*> (static_cast<char*> (mySurface->pixels) +
                                          (mySurface->pitch * (destrect.y + 7)) +
                                          ((destrect.x + 6) * mySurface->format->BytesPerPixel));
      pixels[0] = color;
      pixels[1] = color;
      pixels[2] = color;
    } // end FOR
  myEngine->unlock ();

  if (SDL_MUSTLOCK (mySurface))
    SDL_UnlockSurface (mySurface);

  // step4: paint surface
  ACE_OS::memset (&dirty_region, 0, sizeof (struct SDL_Rect));
  if (inherited::screenLock_)
    inherited::screenLock_->lock ();
  RPG_Graphics_Surface::put (std::make_pair (target_surface->clip_rect.x,
                                             target_surface->clip_rect.y),
                             *mySurface,
                             target_surface,
                             dirty_region);
  if (inherited::screenLock_)
    inherited::screenLock_->unlock ();

  // reset clipping
  inherited::unclip (NULL);

  // invalidate dirty region
  inherited::invalidate (dirty_region);

  // remember position of last realization
  lastAbsolutePosition_ = std::make_pair (target_surface->clip_rect.x,
                                          target_surface->clip_rect.y);

#if defined (_DEBUG)
  if (myDebug)
  {
    std::string path = Common_File_Tools::getTempDirectory ();
    path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
    path += ACE_TEXT_ALWAYS_CHAR ("minimap.png");
    RPG_Graphics_Surface::savePNG (*mySurface,
                                   path,
                                   true);
  } // end IF
#endif // _DEBUG
}

void
RPG_Client_Window_MiniMap::initialize (RPG_Client_Engine* client_in,
                                       RPG_Engine* engine_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Client_Window_MiniMap::initialize"));

  // sanity check(s)
  ACE_ASSERT (engine_in);
  ACE_ASSERT (engine_in);

  myClient = client_in;
  myEngine = engine_in;
}
