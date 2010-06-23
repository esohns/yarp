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
#include "rpg_client_window_main.h"

#include <rpg_graphics_defines.h>
#include <rpg_graphics_common_tools.h>

#include <ace/Log_Msg.h>

RPG_Client_WindowMain::RPG_Client_WindowMain(const RPG_Graphics_InterfaceWindow_t& type_in,
                                             const RPG_Graphics_Type& graphicType_in,
                                             const std::string& title_in,
                                             const RPG_Graphics_Type& fontType_in)
 : inherited(type_in,
             graphicType_in,
             title_in,
             fontType_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Client_WindowMain::RPG_Client_WindowMain"));

}

RPG_Client_WindowMain::~RPG_Client_WindowMain()
{
  ACE_TRACE(ACE_TEXT("RPG_Client_WindowMain::~RPG_Client_WindowMain"));

}

void
RPG_Client_WindowMain::draw(SDL_Surface* targetSurface_in,
                            const RPG_Graphics_Position_t& offset_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Client_WindowMain::draw"));

  // sanity check(s)
  ACE_ASSERT(targetSurface_in);
  ACE_ASSERT(ACE_static_cast(int, offset_in.first) <= targetSurface_in->w);
  ACE_ASSERT(ACE_static_cast(int, offset_in.second) <= targetSurface_in->h);

  RPG_Graphics_InterfaceElementsConstIterator_t iterator;
//   unsigned long border = 0;
  SDL_Rect clipRect;
  unsigned long i = 0;

  // step1: draw borders
  clipRect.x = offset_in.first + myBorderLeft;
  clipRect.y = offset_in.second;
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
  for (i = offset_in.first + myBorderLeft;
       i < (ACE_static_cast(unsigned long, targetSurface_in->w) - myBorderRight);
       i += (*iterator).second->w)
    RPG_Graphics_Common_Tools::put(i,
                                   offset_in.second,
                                   *(*iterator).second,
                                   targetSurface_in);

  clipRect.x = offset_in.first;
  clipRect.y = (offset_in.second + myBorderTop);
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
  for (i = (offset_in.second + myBorderTop);
       i < (ACE_static_cast(unsigned long, targetSurface_in->h) - myBorderBottom);
       i += (*iterator).second->h)
    RPG_Graphics_Common_Tools::put(offset_in.first,
                                   i,
                                   *(*iterator).second,
                                   targetSurface_in);

  clipRect.x = (targetSurface_in->w - myBorderRight);
  clipRect.y = offset_in.second + myBorderTop;
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
  for (i = (offset_in.second + myBorderTop);
       i < (ACE_static_cast(unsigned long, targetSurface_in->h) - myBorderBottom);
       i += (*iterator).second->h)
    RPG_Graphics_Common_Tools::put((targetSurface_in->w - myBorderRight),
                                   i,
                                   *(*iterator).second,
                                   targetSurface_in);

  clipRect.x = offset_in.first + myBorderLeft;
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
  for (i = (offset_in.first + myBorderLeft);
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
  RPG_Graphics_Common_Tools::put(offset_in.first,
                                 offset_in.second,
                                 *(*iterator).second,
                                 targetSurface_in);

  iterator = myElementGraphics.find(INTERFACEELEMENT_CORNER_TR);
  ACE_ASSERT(iterator != myElementGraphics.end());
  RPG_Graphics_Common_Tools::put((targetSurface_in->w - (*iterator).second->w),
                                 offset_in.second,
                                 *(*iterator).second,
                                 targetSurface_in);

  iterator = myElementGraphics.find(INTERFACEELEMENT_CORNER_BL);
  ACE_ASSERT(iterator != myElementGraphics.end());
  RPG_Graphics_Common_Tools::put(offset_in.first,
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
  clipRect.x = offset_in.first + myBorderLeft;
  clipRect.y = offset_in.second + myBorderTop;
  clipRect.w = (targetSurface_in->w - (myBorderLeft + myBorderRight));
  clipRect.h = (targetSurface_in->h - (myBorderTop + myBorderBottom));
  if (!SDL_SetClipRect(targetSurface_in, &clipRect))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_SetClipRect(): %s, aborting\n"),
               SDL_GetError()));

    return;
  } // end IF
  for (i = (offset_in.second + myBorderTop);
       i < (targetSurface_in->h - myBorderBottom);
       i += (*iterator).second->h)
    for (unsigned long j = (offset_in.first + myBorderLeft);
         j < (targetSurface_in->w - myBorderRight);
         j += (*iterator).second->w)
      RPG_Graphics_Common_Tools::put(j,
                                     i,
                                     *(*iterator).second,
                                     targetSurface_in);

  // step4: draw title (if any)
  if (!myTitle.empty())
  {
    clipRect.x = myBorderLeft;
    clipRect.y = 0;
    clipRect.w = targetSurface_in->w - (myBorderLeft + myBorderRight);
    clipRect.h = myBorderTop;
    if (!SDL_SetClipRect(targetSurface_in, &clipRect))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to SDL_SetClipRect(): %s, aborting\n"),
                 SDL_GetError()));

      return;
    } // end IF
    RPG_Graphics_TextSize_t title_size = RPG_Graphics_Common_Tools::textSize(myTitleFont,
                                                                             myTitle);
    RPG_Graphics_Common_Tools::putText(myTitleFont,
                                       myTitle,
                                       RPG_Graphics_Common_Tools::colorToSDLColor(RPG_GRAPHICS_FONT_DEF_COLOR,
                                                                                  *targetSurface_in),
                                       true, // add shade
                                       RPG_Graphics_Common_Tools::colorToSDLColor(RPG_GRAPHICS_FONT_DEF_SHADECOLOR,
                                                                                  *targetSurface_in),
                                       myBorderLeft, // top left
                                       ((myBorderTop - title_size.second) / 2), // center of top border
                                       targetSurface_in);
  } // end IF

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

  // whole window needs a refresh...
  myDirtyRegions.push_back(clipRect);
}
