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

#include "rpg_graphics_defines.h"
#include "rpg_graphics_dictionary.h"
#include "rpg_graphics_common_tools.h"

RPG_Graphics_SDLWindow::RPG_Graphics_SDLWindow(const RPG_Graphics_WindowSize_t& size_in,
                                               const RPG_Graphics_InterfaceWindow_t& type_in,
                                               const RPG_Graphics_Type& graphicType_in,
                                               const std::string& title_in,
                                               const RPG_Graphics_Type& fontType_in)
 : mySize(size_in),
   myBorderTop(0),
   myBorderBottom(0),
   myBorderLeft(0),
   myBorderRight(0),
   myTitle(title_in),
   myTitleFont(fontType_in),
   myOffset(std::make_pair(0, 0)),
   myInitialized(false),
   myParent(NULL),
   myType(type_in),
   myElementGraphicsType(graphicType_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_SDLWindow::RPG_Graphics_SDLWindow"));

  // (try to) load interface element graphics
  myInitialized = loadGraphics(myElementGraphicsType);
}

RPG_Graphics_SDLWindow::RPG_Graphics_SDLWindow(const RPG_Graphics_SDLWindow& parent_in,
                                               const RPG_Graphics_Offset_t& offset_in,
                                               const RPG_Graphics_InterfaceWindow_t& type_in)
  : myBorderTop(0),
    myBorderBottom(0),
    myBorderLeft(0),
    myBorderRight(0),
//     myTitle(),
    myTitleFont(RPG_GRAPHICS_TYPE_INVALID),
    myOffset(offset_in),
    myInitialized(false),
    myParent(&parent_in),
    myType(type_in),
    myElementGraphicsType(RPG_GRAPHICS_TYPE_INVALID)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_SDLWindow::RPG_Graphics_SDLWindow"));

  // compute parent's borders
  myParent->getBorders(myBorderTop,
                       myBorderBottom,
                       myBorderLeft,
                       myBorderRight);

  RPG_Graphics_WindowSize_t size_parent = myParent->getSize();
  mySize.first = size_parent.first - (myBorderLeft + myBorderRight);
  mySize.second = size_parent.second - (myBorderTop + myBorderBottom);

  myInitialized = true;
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

void
RPG_Graphics_SDLWindow::getBorders(unsigned long& borderTop_out,
                                   unsigned long& borderBottom_out,
                                   unsigned long& borderLeft_out,
                                   unsigned long& borderRight_out) const
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_SDLWindow::getBorders"));

  // init return value(s)
  borderTop_out = myBorderTop;
  borderBottom_out = myBorderBottom;
  borderLeft_out = myBorderLeft;
  borderRight_out = myBorderRight;

  // iterate over all parent(s)
  unsigned long borderTop;
  unsigned long borderBottom;
  unsigned long borderLeft;
  unsigned long borderRight;
  for (const RPG_Graphics_SDLWindow* current = myParent;
       current != NULL;
       current = current->getParent())
  {
    current->getBorders(borderTop,
                        borderBottom,
                        borderLeft,
                        borderRight);
    borderTop_out += borderTop;
    borderBottom_out += borderBottom;
    borderLeft_out += borderLeft;
    borderRight_out += borderRight;
  } // end FOR
}

const RPG_Graphics_SDLWindow*
RPG_Graphics_SDLWindow::getParent() const
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_SDLWindow::getParent"));

  return myParent;
}

void
RPG_Graphics_SDLWindow::refresh(SDL_Surface* targetSurface_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_SDLWindow::refresh"));

  // sanity check(s)
  ACE_ASSERT(targetSurface_in);
  if (myDirtyRegions.empty())
    return; // nothing to do !

  Sint32 topLeftX = std::numeric_limits<int>::max();
  Sint32 topLeftY = std::numeric_limits<int>::max();
  Sint32 width = 0;
  Sint32 height = 0;

  // find bounding box of dirty areas
  for (RPG_Graphics_DirtyRegionsConstIterator_t iterator = myDirtyRegions.begin();
       iterator != myDirtyRegions.end();
       iterator++)
  {
    if ((*iterator).x < topLeftX)
      topLeftX = (*iterator).x;
    if ((*iterator).y < topLeftY)
      topLeftY = (*iterator).y;
    if (width < (*iterator).w)
      width = (*iterator).w;
    if (height < (*iterator).h)
      height = (*iterator).h;
  } // end FOR

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("refreshing bbox [[%d,%d][%d,%d]]...\n"),
//              topLeftX,
//              topLeftY,
//              topLeftX + width - 1,
//              topLeftY + height - 1));

  SDL_UpdateRect(targetSurface_in,
                 topLeftX,
                 topLeftY,
                 width,
                 height);

  // all fresh & shiny !
  myDirtyRegions.clear();
}

const RPG_Graphics_WindowSize_t
RPG_Graphics_SDLWindow::getSize() const
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_SDLWindow::getSize"));

  if (myParent)
    return myParent->getSize();

  return mySize;
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
                                                     (*iterator).width,
                                                     (*iterator).height,
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

      // store border sizes
      if ((*iterator).type.interfaceelementtype == INTERFACEELEMENT_BORDER_TOP)
        myBorderTop = element_image->h;
      else if ((*iterator).type.interfaceelementtype == INTERFACEELEMENT_BORDER_BOTTOM)
        myBorderBottom = element_image->h;
      else if ((*iterator).type.interfaceelementtype == INTERFACEELEMENT_BORDER_LEFT)
        myBorderLeft = element_image->w;
      else if ((*iterator).type.interfaceelementtype == INTERFACEELEMENT_BORDER_RIGHT)
        myBorderRight = element_image->w;

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
