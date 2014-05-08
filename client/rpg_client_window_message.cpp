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

#include "rpg_client_window_message.h"

#include "rpg_client_defines.h"

#include "rpg_graphics_defines.h"
#include "rpg_graphics_surface.h"
#include "rpg_graphics_common_tools.h"
#include "rpg_graphics_SDL_tools.h"

#include "rpg_common_macros.h"

RPG_Client_Window_Message::RPG_Client_Window_Message(const RPG_Graphics_SDLWindowBase& parent_in)
 : inherited(WINDOW_MESSAGE,            // type
             parent_in,                 // parent
             std::make_pair(0, 0),      // offset
             ACE_TEXT_ALWAYS_CHAR("")), // title
//             NULL),                     // background
   myFont(RPG_CLIENT_MESSAGE_FONT),
   myNumLines(RPG_CLIENT_DEF_MESSAGE_LINES),
//   myLock(),
//   myMessages(),
   myBG(NULL)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Window_Message::RPG_Client_Window_Message"));

}

RPG_Client_Window_Message::~RPG_Client_Window_Message()
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Window_Message::~RPG_Client_Window_Message"));

  // clean up
  SDL_FreeSurface(myBG);
}

void
RPG_Client_Window_Message::handleEvent(const SDL_Event& event_in,
                                       RPG_Graphics_IWindowBase* window_in,
                                       SDL_Rect& dirtyRegion_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Window_Message::handleEvent"));

  // init return value(s)
  ACE_OS::memset(&dirtyRegion_out, 0, sizeof(dirtyRegion_out));

  //   ACE_DEBUG((LM_DEBUG,
  //              ACE_TEXT("RPG_Client_Window_Message::handleEvent(%s)\n"),
  //              RPG_Graphics_TypeHelper::RPG_Graphics_TypeToString(myType).c_str()));

  switch (event_in.type)
  {
    // *** mouse ***
    case RPG_GRAPHICS_SDL_MOUSEMOVEOUT:
    {

      break;
    }
    case SDL_MOUSEMOTION:
    {

      // *WARNING*: falls through !
    }
    case SDL_ACTIVEEVENT:
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
    case SDL_VIDEORESIZE:
    case SDL_VIDEOEXPOSE:
    case SDL_USEREVENT:
    case RPG_GRAPHICS_SDL_HOVEREVENT:
    default:
    {
      // delegate these to the parent...
      getParent()->handleEvent(event_in,
                               window_in,
                               dirtyRegion_out);

      break;
    }
    //     default:
    //     {
      //       ACE_DEBUG((LM_ERROR,
      //                  ACE_TEXT("received unknown event (was: %u)...\n"),
      //                  static_cast<unsigned int>(event_in.type)));
      //
      //       break;
      //     }
  } // end SWITCH
}

void
RPG_Client_Window_Message::draw(SDL_Surface* targetSurface_in,
                                const unsigned int& offsetX_in,
                                const unsigned int& offsetY_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Window_Message::draw"));

  // sanity check(s)
  SDL_Surface* target_surface = (targetSurface_in ? targetSurface_in
                                                  : myScreen);
  ACE_ASSERT(target_surface);
  ACE_UNUSED_ARG(offsetX_in);
  ACE_UNUSED_ARG(offsetY_in);
  ACE_ASSERT(myBG);
  ACE_ASSERT(myParent);

  SDL_Rect dirty_region;
  ACE_OS::memset(&dirty_region, 0, sizeof(dirty_region));

  // step0: restore background
  if (inherited::myScreenLock)
    inherited::myScreenLock->lock();
  if (inherited::myBGHasBeenSaved)
    inherited::restoreBG(dirty_region);

  // step1: compute required size / offset
  ACE_OS::memset(&myClipRect, 0, sizeof(myClipRect));
  {
    ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

    RPG_Graphics_TextSize_t text_size = std::make_pair(0, 0);
    unsigned int index = 0;
    for (RPG_Client_MessageStackConstIterator_t iterator = myMessages.begin();
         ((iterator != myMessages.end()) && (index < myNumLines));
         iterator++, index++)
    {
      text_size = RPG_Graphics_Common_Tools::textSize(myFont, *iterator);
      myClipRect.h += text_size.second + 1;
      myClipRect.w = ((myClipRect.w < text_size.first) ? text_size.first
                                                       : myClipRect.w);
    } // end FOR
    // add some padding
    myClipRect.w += 2;
    myClipRect.h += 1;
    SDL_Rect parent_area;
    myParent->getArea(parent_area);
    myClipRect.x = (parent_area.x +
                    ((parent_area.w - myClipRect.w) / 2));
    myClipRect.y = (parent_area.y +
                    parent_area.h -
                    myClipRect.h);

    // save background
    inherited::saveBG(std::make_pair(0, 0)); // --> save everything

    // shade text area
    if (myBG)
    {
      SDL_FreeSurface(myBG);
      myBG = NULL;
    } // end IF
    myBG = RPG_Graphics_Surface::create(myClipRect.w, myClipRect.h);
    if (!myBG)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to RPG_Graphics_Surface::create(%u,%u), aborting\n"),
                 myClipRect.w, myClipRect.h));

      // clean up
      if (inherited::myScreenLock)
        inherited::myScreenLock->unlock();

      return;
    } // end IF
    RPG_Graphics_Surface::fill(RPG_Graphics_SDL_Tools::getColor(COLOR_BLACK_A50,
                                                                *myBG),
                               myBG);

    // init clipping
    //clip(targetSurface,
    //     offsetX_in,
    //     offsetY_in);
    SDL_Rect clip_rect_orig;
    SDL_GetClipRect(target_surface, &clip_rect_orig);
    if (!SDL_SetClipRect(target_surface, &myClipRect))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to SDL_SetClipRect(): %s, aborting\n"),
                 ACE_TEXT(SDL_GetError())));

      // clean up
      if (inherited::myScreenLock)
        inherited::myScreenLock->unlock();

      return;
    } // end IF

    RPG_Graphics_Surface::put(std::make_pair(myClipRect.x,
                                             myClipRect.y),
                              *myBG,
                              target_surface,
                              dirty_region);

    // draw messages
    index = 0;
    for (RPG_Client_MessageStackConstIterator_t iterator = myMessages.begin();
         ((iterator != myMessages.end()) && (index < myNumLines));
         iterator++, index++)
    {
      text_size = RPG_Graphics_Common_Tools::textSize(myFont, *iterator);
      RPG_Graphics_Surface::putText(myFont,
                                    *iterator,
                                    RPG_Graphics_SDL_Tools::colorToSDLColor(RPG_CLIENT_MESSAGE_COLOR,
                                                                            *target_surface),
                                    RPG_CLIENT_MESSAGE_SHADE_LINES,
                                    RPG_Graphics_SDL_Tools::colorToSDLColor(RPG_CLIENT_MESSAGE_SHADECOLOR,
                                                                            *target_surface),
                                    std::make_pair(parent_area.x +
                                                   ((parent_area.w - text_size.first) / 2),
                                                   (parent_area.y +
                                                    parent_area.h -
                                                    (index * (text_size.second + 1)))),
                                    target_surface,
                                    dirty_region);
    } // end FOR

    // reset clipping
//    unclip(targetSurface);
    if (!SDL_SetClipRect(target_surface, &clip_rect_orig))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to SDL_SetClipRect(): %s, aborting\n"),
                 ACE_TEXT(SDL_GetError())));

      // clean up
      if (inherited::myScreenLock)
        inherited::myScreenLock->unlock();

      return;
    } // end IF
  } // end lock scope
  if (inherited::myScreenLock)
    inherited::myScreenLock->unlock();

  // invalidate dirty region
  dirty_region = RPG_Graphics_SDL_Tools::boundingBox(dirty_region,
                                                     myClipRect);
  invalidate(dirty_region);

  // remember position of last realization
  myLastAbsolutePosition = std::make_pair(myClipRect.x,
                                          myClipRect.y);
}

void
RPG_Client_Window_Message::init(RPG_Common_ILock* screenLock_in,
                                const RPG_Graphics_Font& font_in,
                                const unsigned int& numLines_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Window_Message::init"));

  inherited::init(screenLock_in);

  // sanity check(s)
  ACE_ASSERT(numLines_in);

  myFont     = font_in;
  myNumLines = numLines_in;
}

void
RPG_Client_Window_Message::push(const std::string& message_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Window_Message::push"));

  ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

  // age cache ?
  if (myMessages.size() == RPG_CLIENT_DEF_MESSAGE_CACHE_SIZE)
    myMessages.pop_back();
  myMessages.push_front(message_in);
}
