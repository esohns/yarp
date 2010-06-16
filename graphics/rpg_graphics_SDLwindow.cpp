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
#include "rpg_graphics_SDLwindow.h"

#include "rpg_graphics_dictionary.h"
#include "rpg_graphics_common_tools.h"

RPG_Graphics_SDLWindow::RPG_Graphics_SDLWindow(const RPG_Graphics_InterfaceWindow_t& type_in,
                                               const RPG_Graphics_Type& graphicType_in)
 : myType(type_in),
   myGraphicsType(graphicType_in),
   myInitialized(false)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_SDLWindow::RPG_Graphics_SDLWindow"));

  // (try to) load interface element graphics
  myInitialized = loadGraphics(myGraphicsType);
}

RPG_Graphics_SDLWindow::~RPG_Graphics_SDLWindow()
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_SDLWindow::~RPG_Graphics_SDLWindow"));

  if (myInitialized)
  {
    // clean up
    for (RPG_Graphics_InterfaceElementsConstIterator_t iterator = myElementGraphics.begin();
         iterator != myElementGraphics.end();
         iterator++)
      SDL_FreeSurface((*iterator).second);
    myElementGraphics.clear();
  } // end IF
}

const bool
RPG_Graphics_SDLWindow::loadGraphics(const RPG_Graphics_Type& graphicType_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_SDLWindow::loadGraphics"));

  // step1: load interface image
  RPG_Graphics_t graphic;
  graphic.type = graphicType_in;
  // retrieve properties from the dictionary
  graphic = RPG_GRAPHICS_DICTIONARY_SINGLETON::instance()->getGraphic(graphicType_in);
  if (graphic.type != graphicType_in)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Graphics_Dictionary::getGraphic(\"%s\"), aborting\n"),
               RPG_Graphics_TypeHelper::RPG_Graphics_TypeToString(graphicType_in).c_str()));

    return false;
  } // end IF

  SDL_Surface* interface_image = NULL;
  interface_image = RPG_Graphics_Common_Tools::loadGraphic(graphicType_in,
                                                           false); // don't cache this one
  if (!interface_image)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Graphics_Common_Tools::loadGraphic(\"%s\"), aborting\n"),
               RPG_Graphics_TypeHelper::RPG_Graphics_TypeToString(graphicType_in).c_str()));

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
    if ((*iterator).type.discriminator == RPG_Graphics_ElementTypeUnion::INTERFACEELEMENTTYPE)
    {
      element_image = RPG_Graphics_Common_Tools::get((*iterator).offsetX,
                                                     (*iterator).offsetY,
                                                     ((*iterator).offsetX + (*iterator).width),
                                                     ((*iterator).offsetY + (*iterator).height),
                                                     interface_image);
      if (!element_image)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to RPG_Graphics_Common_Tools::get(%u,%u,%u,%u) from \"%s\", aborting\n"),
                   (*iterator).offsetX,
                   (*iterator).offsetY,
                   ((*iterator).offsetX + (*iterator).width),
                   ((*iterator).offsetY + (*iterator).height),
                   RPG_Graphics_TypeHelper::RPG_Graphics_TypeToString(graphicType_in).c_str()));

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
             RPG_Graphics_TypeHelper::RPG_Graphics_TypeToString(graphicType_in).c_str()));

  return true;
}
