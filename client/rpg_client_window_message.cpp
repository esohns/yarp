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

#include "rpg_common_macros.h"

RPG_Client_Window_Message::RPG_Client_Window_Message(const RPG_Graphics_SDLWindowBase& parent_in)
 : inherited(WINDOW_MESSAGE,            // type
             parent_in,                 // parent
             std::make_pair(0, 0),      // offset
             ACE_TEXT_ALWAYS_CHAR("")), // title
//             NULL),                     // background
   myFont(RPG_CLIENT_DEF_MESSAGE_FONT),
   myNumLines(RPG_CLIENT_DEF_MESSAGE_LINES),
//   myLock(),
//   myMessages(),
   myBG(NULL)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Window_Message::RPG_Client_Window_Message"));

  // init background surface
  myBG = RPG_Graphics_Surface::create(20, 20);
  ACE_ASSERT(myBG);
  RPG_Graphics_Surface::fill(RPG_Graphics_SDL_Tools::CLR32_BLACK_A50, myBG);
}

RPG_Client_Window_Message::~RPG_Client_Window_Message()
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Window_Message::~RPG_Client_Window_Message"));

  // clean up
  SDL_FreeSurface(myBG);
}

void
RPG_Client_Window_Message::handleEvent(const SDL_Event& event_in,
                                       RPG_Graphics_IWindow* window_in,
                                       bool& redraw_out)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_Window_Message::handleEvent"));

  // init return value(s)
  redraw_out = false;

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
                               redraw_out);

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

  // set target surface
  SDL_Surface* targetSurface = (targetSurface_in ? targetSurface_in
                                                 : myScreen);

  // sanity check(s)
  ACE_ASSERT(targetSurface);
  ACE_UNUSED_ARG(offsetX_in);
  ACE_UNUSED_ARG(offsetY_in);
  ACE_ASSERT(myBG);

  SDL_Rect dirty_region;
  ACE_OS::memset(&dirty_region, 0, sizeof(dirty_region));

  // step0: restore background
  if (inherited::myScreenLock)
    inherited::myScreenLock->lock();
  if (inherited::myBGHasBeenSaved)
    inherited::restoreBG(dirty_region);

  // step1: compute required size / offset
  inherited::mySize   = std::make_pair(0, 0);
  inherited::myOffset = std::make_pair(0, 0);
  {
    ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

    RPG_Graphics_TextSize_t text_size = std::make_pair(0, 0);
    unsigned int index = 0;
    for (RPG_Client_MessageStackConstIterator_t iterator = myMessages.begin();
         ((iterator != myMessages.end()) && (index < myNumLines));
         iterator++, index++)
    {
      text_size = RPG_Graphics_Common_Tools::textSize(myFont, *iterator);
      inherited::mySize.second += text_size.second + 1;
      inherited::mySize.first = (inherited::mySize.first < text_size.first ? text_size.first
                                                                           : inherited::mySize.first);
    } // end FOR
    // add some padding
    inherited::mySize.first  += 4;
    inherited::mySize.second += 2;
    inherited::myOffset = std::make_pair((targetSurface->w -
                                          (inherited::myBorderLeft + inherited::myBorderRight) -
                                          inherited::mySize.first) / 2,
                                         (targetSurface->h -
                                          (inherited::myBorderTop + inherited::myBorderBottom) -
                                          inherited::mySize.second));

    // init clipping
    //clip(targetSurface,
    //     offsetX_in,
    //     offsetY_in);
    SDL_GetClipRect(targetSurface, &(inherited::myClipRect));
    SDL_Rect clip_rect = {static_cast<int16_t>(inherited::myBorderLeft + inherited::myOffset.first),
                          static_cast<int16_t>(inherited::myBorderTop + inherited::myOffset.second),
                          static_cast<uint16_t>(inherited::mySize.first),
                          static_cast<uint16_t>(inherited::mySize.second)};
    if (!SDL_SetClipRect(targetSurface, &clip_rect))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to SDL_SetClipRect(): %s, aborting\n"),
                 ACE_TEXT(SDL_GetError())));

      // clean up
      if (inherited::myScreenLock)
        inherited::myScreenLock->unlock();

      return;
    } // end IF

    // save background
    inherited::saveBG(std::make_pair(0, 0)); // --> save everything

    // shade text area
    for (unsigned int y = (inherited::myBorderTop +
                           inherited::myOffset.second);
         y < (inherited::myBorderTop     +
              inherited::myOffset.second +
              inherited::mySize.second);
         y += myBG->h)
      for (unsigned int x = (inherited::myBorderLeft +
                             inherited::myOffset.first);
           x < (inherited::myBorderLeft +
                inherited::myOffset.first +
                inherited::mySize.first);
           x += myBG->w)
        RPG_Graphics_Surface::put(x,
                                  y,
                                  *myBG,
                                  targetSurface);

    // draw messages
    index = 0;
    for (RPG_Client_MessageStackConstIterator_t iterator = myMessages.begin();
         ((iterator != myMessages.end()) && (index < myNumLines));
         iterator++, index++)
    {
      text_size = RPG_Graphics_Common_Tools::textSize(myFont, *iterator);
      RPG_Graphics_Surface::putText(myFont,
                                    *iterator,
                                    RPG_Graphics_SDL_Tools::colorToSDLColor(RPG_CLIENT_DEF_MESSAGE_COLOR,
                                                                            *targetSurface),
                                    RPG_CLIENT_DEF_MESSAGE_SHADE_LINES,
                                    RPG_Graphics_SDL_Tools::colorToSDLColor(RPG_CLIENT_DEF_MESSAGE_SHADECOLOR,
                                                                            *targetSurface),
                                    (inherited::myBorderLeft   +
                                     inherited::myOffset.first +
                                     ((inherited::mySize.first - text_size.first) / 2)),
                                    (inherited::myBorderTop     +
                                     inherited::myOffset.second +
                                     (index * (text_size.second + 1))),
                                    targetSurface);
    } // end FOR
  } // end lock scope
  if (inherited::myScreenLock)
    inherited::myScreenLock->unlock();

  // reset clipping
  //unclip(targetSurface);
  if (!SDL_SetClipRect(targetSurface, &(inherited::myClipRect)))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_SetClipRect(): %s, aborting\n"),
               ACE_TEXT(SDL_GetError())));

    return;
  } // end IF

  // invalidate dirty region
  dirty_region.w = ((inherited::mySize.first > dirty_region.w) ? inherited::mySize.first
                                                               : dirty_region.w);
  dirty_region.h = ((inherited::mySize.second > dirty_region.h) ? inherited::mySize.second
                                                                : dirty_region.h);
  if (dirty_region.x > static_cast<int16_t>(inherited::myBorderLeft + inherited::myOffset.first))
    dirty_region.x = static_cast<int16_t>(inherited::myBorderLeft + inherited::myOffset.first);
  if (dirty_region.y > static_cast<int16_t>(inherited::myBorderTop + inherited::myOffset.second))
    dirty_region.y = static_cast<int16_t>(inherited::myBorderTop + inherited::myOffset.second);
  invalidate(dirty_region);

  // remember position of last realization
  inherited::myLastAbsolutePosition = std::make_pair(inherited::myBorderLeft + inherited::myOffset.first,
                                                     inherited::myBorderTop  + inherited::myOffset.second);
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
