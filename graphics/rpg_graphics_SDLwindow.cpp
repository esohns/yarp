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
 : myBorderTop(0),
   myBorderBottom(0),
   myBorderLeft(0),
   myBorderRight(0),
   myType(type_in),
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

void
RPG_Graphics_SDLWindow::draw(SDL_Surface* targetSurface_in,
                             const unsigned long& offsetX_in,
                             const unsigned long& offsetY_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_SDLWindow::draw"));

  // sanity check(s)
  ACE_ASSERT(targetSurface_in);
  ACE_ASSERT(ACE_static_cast(int, offsetX_in) <= targetSurface_in->w);
  ACE_ASSERT(ACE_static_cast(int, offsetY_in) <= targetSurface_in->h);

  RPG_Graphics_InterfaceElementsConstIterator_t iterator;
//   unsigned long border = 0;
  SDL_Rect clipRect;
  unsigned long i = 0;

  // step1: draw borders
  clipRect.x = offsetX_in + myBorderLeft;
  clipRect.y = offsetY_in;
  clipRect.w = (targetSurface_in->w - (myBorderLeft + myBorderRight));
  clipRect.h = myBorderTop;
  if (!SDL_SetClipRect(targetSurface_in, &clipRect))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_SetClipRect(): %s, aborting\n"),
               SDL_GetError()));

    return;
  } // end IF
  iterator = myElementGraphics.find(INTERFACEELEMENT_BORDER_TOP);
  ACE_ASSERT(iterator != myElementGraphics.end());
  for (i = offsetX_in + myBorderLeft;
       i < (ACE_static_cast(unsigned long, targetSurface_in->w) - myBorderRight);
       i += (*iterator).second->w)
    RPG_Graphics_Common_Tools::put(i,
                                   offsetY_in,
                                   *(*iterator).second,
                                   targetSurface_in);

  clipRect.x = offsetX_in;
  clipRect.y = (offsetY_in + myBorderTop);
  clipRect.w = myBorderLeft;
  clipRect.h = (targetSurface_in->h - (myBorderTop + myBorderBottom));
  if (!SDL_SetClipRect(targetSurface_in, &clipRect))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_SetClipRect(): %s, aborting\n"),
               SDL_GetError()));

    return;
  } // end IF
  iterator = myElementGraphics.find(INTERFACEELEMENT_BORDER_LEFT);
  ACE_ASSERT(iterator != myElementGraphics.end());
  for (i = (offsetY_in + myBorderTop);
       i < (ACE_static_cast(unsigned long, targetSurface_in->h) - myBorderBottom);
       i += (*iterator).second->h)
    RPG_Graphics_Common_Tools::put(offsetX_in,
                                   i,
                                   *(*iterator).second,
                                   targetSurface_in);

  clipRect.x = (targetSurface_in->w - myBorderRight);
  clipRect.y = offsetY_in + myBorderTop;
  clipRect.w = myBorderRight;
  clipRect.h = (targetSurface_in->h - (myBorderTop + myBorderBottom));
  if (!SDL_SetClipRect(targetSurface_in, &clipRect))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_SetClipRect(): %s, aborting\n"),
               SDL_GetError()));

    return;
  } // end IF
  iterator = myElementGraphics.find(INTERFACEELEMENT_BORDER_RIGHT);
  ACE_ASSERT(iterator != myElementGraphics.end());
  for (i = (offsetY_in + myBorderTop);
       i < (ACE_static_cast(unsigned long, targetSurface_in->h) - myBorderBottom);
       i += (*iterator).second->h)
    RPG_Graphics_Common_Tools::put((targetSurface_in->w - myBorderRight),
                                   i,
                                   *(*iterator).second,
                                   targetSurface_in);

  clipRect.x = offsetX_in + myBorderLeft;
  clipRect.y = (targetSurface_in->h - myBorderBottom);
  clipRect.w = (targetSurface_in->w - (myBorderLeft + myBorderRight));
  clipRect.h = myBorderBottom;
  if (!SDL_SetClipRect(targetSurface_in, &clipRect))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_SetClipRect(): %s, aborting\n"),
               SDL_GetError()));

    return;
  } // end IF
  iterator = myElementGraphics.find(INTERFACEELEMENT_BORDER_BOTTOM);
  ACE_ASSERT(iterator != myElementGraphics.end());
  for (i = (offsetX_in + myBorderLeft);
       i < (targetSurface_in->w - myBorderRight);
       i += (*iterator).second->w)
    RPG_Graphics_Common_Tools::put(i,
                                   (targetSurface_in->h - myBorderBottom),
                                   *(*iterator).second,
                                   targetSurface_in);

  // reset clipping area
  clipRect.x = 0;
  clipRect.y = 0;
  clipRect.w = targetSurface_in->w;
  clipRect.h = targetSurface_in->h;
  if (!SDL_SetClipRect(targetSurface_in, &clipRect))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_SetClipRect(): %s, aborting\n"),
               SDL_GetError()));

    return;
  } // end IF

  // step2: draw corners
  iterator = myElementGraphics.find(INTERFACEELEMENT_CORNER_TL);
  ACE_ASSERT(iterator != myElementGraphics.end());
  RPG_Graphics_Common_Tools::put(offsetX_in,
                                 offsetY_in,
                                 *(*iterator).second,
                                 targetSurface_in);

  iterator = myElementGraphics.find(INTERFACEELEMENT_CORNER_TR);
  ACE_ASSERT(iterator != myElementGraphics.end());
  RPG_Graphics_Common_Tools::put((targetSurface_in->w - (*iterator).second->w),
                                 offsetY_in,
                                 *(*iterator).second,
                                 targetSurface_in);

  iterator = myElementGraphics.find(INTERFACEELEMENT_CORNER_BL);
  ACE_ASSERT(iterator != myElementGraphics.end());
  RPG_Graphics_Common_Tools::put(offsetX_in,
                                 (targetSurface_in->h - (*iterator).second->h),
                                 *(*iterator).second,
                                 targetSurface_in);

  iterator = myElementGraphics.find(INTERFACEELEMENT_CORNER_BR);
  ACE_ASSERT(iterator != myElementGraphics.end());
  RPG_Graphics_Common_Tools::put((targetSurface_in->w - (*iterator).second->w),
                                 (targetSurface_in->h - (*iterator).second->h),
                                 *(*iterator).second,
                                 targetSurface_in);

  // step3: fill central area
  iterator = myElementGraphics.find(INTERFACEELEMENT_CENTER);
  ACE_ASSERT(iterator != myElementGraphics.end());
  clipRect.x = offsetX_in + myBorderLeft;
  clipRect.y = offsetY_in + myBorderTop;
  clipRect.w = (targetSurface_in->w - (myBorderLeft + myBorderRight));
  clipRect.h = (targetSurface_in->h - (myBorderTop + myBorderBottom));
  if (!SDL_SetClipRect(targetSurface_in, &clipRect))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_SetClipRect(): %s, aborting\n"),
               SDL_GetError()));

    return;
  } // end IF
  for (i = (offsetY_in + myBorderTop);
       i < (targetSurface_in->h - myBorderBottom);
       i += (*iterator).second->h)
    for (unsigned long j = (offsetX_in + myBorderLeft);
         j < (targetSurface_in->w - myBorderRight);
         j += (*iterator).second->w)
      RPG_Graphics_Common_Tools::put(j,
                                     i,
                                     *(*iterator).second,
                                     targetSurface_in);

  // reset clipping area
  clipRect.x = 0;
  clipRect.y = 0;
  clipRect.w = targetSurface_in->w;
  clipRect.h = targetSurface_in->h;
  if (!SDL_SetClipRect(targetSurface_in, &clipRect))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_SetClipRect(): %s, aborting\n"),
               SDL_GetError()));

    return;
  } // end IF

  // step4: draw title
//   pos_x = this->abs_x + border_left;
//   pos_y = this->abs_y + border_top;
//
//   if (!caption.empty())
//     vultures_put_text_shadow(V_FONT_HEADLINE, caption, vultures_screen, pos_x,
//                              pos_y, V_COLOR_TEXT, V_COLOR_BACKGROUND);

  // whole window needs a refresh...
  myDirtyRegions.push_back(clipRect);
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

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("refreshing bbox [[%d,%d][%d,%d]]...\n"),
             topLeftX,
             topLeftY,
             topLeftX + width - 1,
             topLeftY + height - 1));

  SDL_UpdateRect(targetSurface_in,
                 topLeftX,
                 topLeftY,
                 width,
                 height);

  // all fresh & shiny !
  myDirtyRegions.clear();
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
