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

#include "rpg_graphics_toplevel.h"

#include "rpg_common_macros.h"

#include "rpg_graphics_common_tools.h"
#include "rpg_graphics_dictionary.h"
#include "rpg_graphics_surface.h"

RPG_Graphics_TopLevel::RPG_Graphics_TopLevel(const RPG_Graphics_Size_t& size_in,
                                             const RPG_Graphics_GraphicTypeUnion& elementType_in,
                                             const std::string& title_in)
//                                              SDL_Surface* backGround_in)
 : inherited (WINDOW_MAIN,
              size_in,
              title_in)
//              backGround_in),
 , myElementGraphicsType (elementType_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_TopLevel::RPG_Graphics_TopLevel"));

  // (try to) load (interface) element graphics
  if (!loadGraphics(myElementGraphicsType))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to load interface graphics, aborting\n")));

    return;
  } // end IF

  // set borders (if any)
  RPG_Graphics_InterfaceElementsConstIterator_t iterator =
    myElementGraphics.find (INTERFACEELEMENT_BORDER_TOP);
  if (iterator != myElementGraphics.end ())
    borderTop_ = (*iterator).second->h;
  iterator =
    myElementGraphics.find (INTERFACEELEMENT_BORDER_LEFT);
  if (iterator != myElementGraphics.end ())
    borderLeft_ = (*iterator).second->w;
  iterator =
    myElementGraphics.find (INTERFACEELEMENT_BORDER_RIGHT);
  if (iterator != myElementGraphics.end ())
    borderRight_ = (*iterator).second->w;
  iterator =
    myElementGraphics.find (INTERFACEELEMENT_BORDER_BOTTOM);
  if (iterator != myElementGraphics.end ())
    borderBottom_ = (*iterator).second->h;
}

RPG_Graphics_TopLevel::~RPG_Graphics_TopLevel()
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_TopLevel::~RPG_Graphics_TopLevel"));

  // free interface element graphics (if any)
  for (RPG_Graphics_InterfaceElementsConstIterator_t iterator = myElementGraphics.begin();
       iterator != myElementGraphics.end();
       iterator++)
    SDL_FreeSurface((*iterator).second);
}

RPG_Graphics_Position_t
RPG_Graphics_TopLevel::getView() const
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_TopLevel::getView"));

  ACE_ASSERT(false);

  return std::make_pair(0, 0);
}

// void
// RPG_Graphics_TopLevel::child(const RPG_Graphics_WindowSize_t& size_in,
//                              const RPG_Graphics_WindowType& type_in,
//                              const RPG_Graphics_Offset_t& offset_in,
//                              SDL_Surface* backGround_in)
// {
//   RPG_TRACE(ACE_TEXT("RPG_Graphics_TopLevel::child"));
//
// }

bool
RPG_Graphics_TopLevel::loadGraphics(const RPG_Graphics_GraphicTypeUnion& type_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_TopLevel::loadGraphics"));

  // step1: load interface image
  RPG_Graphics_t graphic;
  graphic.category = RPG_GRAPHICS_CATEGORY_INVALID;
  graphic.type.discriminator = RPG_Graphics_GraphicTypeUnion::INVALID;
  // retrieve properties from the dictionary
  graphic = RPG_GRAPHICS_DICTIONARY_SINGLETON::instance()->get(type_in);
  if ((graphic.category != CATEGORY_INTERFACE) ||
      (graphic.type.discriminator != type_in.discriminator)) // too weak
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Graphics_Dictionary::get(\"%s\"), aborting\n"),
               ACE_TEXT(RPG_Graphics_Common_Tools::toString(type_in).c_str())));

    return false;
  } // end IF

  SDL_Surface* interface_image = NULL;
  interface_image = RPG_Graphics_Common_Tools::loadGraphic(type_in,
                                                           true,    // convert to display format
                                                           false);  // don't cache this one
  if (!interface_image)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Graphics_Common_Tools::loadGraphic(\"%s\"), aborting\n"),
               ACE_TEXT(RPG_Graphics_Common_Tools::toString(type_in).c_str())));

    return false;
  } // end IF

  // step2: load individual element images and store them for reference
  SDL_Surface* element_image = NULL;
  for (RPG_Graphics_ElementsConstIterator_t iterator = graphic.elements.begin();
       iterator != graphic.elements.end();
       iterator++)
  {
    element_image = NULL;

    // element part of the interface ?
    if ((*iterator).type.discriminator ==
      RPG_Graphics_ElementTypeUnion::INTERFACEELEMENTTYPE)
    {
      element_image =
        RPG_Graphics_Surface::get(std::make_pair((*iterator).offsetX,
                                                 (*iterator).offsetY),
                                  (*iterator).width,
                                  (*iterator).height,
                                  *interface_image);
      if (!element_image)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to RPG_Graphics_Surface::get(%u,%u,%u,%u) from \"%s\", aborting\n"),
                   (*iterator).offsetX,
                   (*iterator).offsetY,
                   ((*iterator).offsetX + (*iterator).width),
                   ((*iterator).offsetY + (*iterator).height),
                   ACE_TEXT(RPG_Graphics_Common_Tools::toString(type_in).c_str())));

        // clean up
        SDL_FreeSurface(interface_image);
        for (RPG_Graphics_InterfaceElementsConstIterator_t iterator = myElementGraphics.begin();
             iterator != myElementGraphics.end();
             iterator++)
          SDL_FreeSurface((*iterator).second);
        myElementGraphics.clear();

        return false;
      } // end IF

      // store surface handle
      myElementGraphics.insert(std::make_pair((*iterator).type.interfaceelementtype,
                                              element_image));
    } // end IF
  } // end FOR

  // clean up
  SDL_FreeSurface(interface_image);

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("loaded %u interface element graphic(s) from \"%s\"...\n"),
             myElementGraphics.size(),
             ACE_TEXT(RPG_Graphics_Common_Tools::toString(type_in).c_str())));

  return true;
}
