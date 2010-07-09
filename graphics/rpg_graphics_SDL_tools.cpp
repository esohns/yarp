/***************************************************************************
 *   Copyright (C) 2009 by Erik Sohns   *
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
#include "rpg_graphics_SDL_tools.h"

#include "rpg_graphics_defines.h"

#include <ace/Log_Msg.h>

// init statics
Uint32 RPG_Graphics_SDL_Tools::CLR32_BLACK      = 0;
Uint32 RPG_Graphics_SDL_Tools::CLR32_BLACK_A10  = 0;
Uint32 RPG_Graphics_SDL_Tools::CLR32_BLACK_A30  = 0;
Uint32 RPG_Graphics_SDL_Tools::CLR32_BLACK_A50  = 0;
Uint32 RPG_Graphics_SDL_Tools::CLR32_BLACK_A70  = 0;
Uint32 RPG_Graphics_SDL_Tools::CLR32_BLACK_A90  = 0;
Uint32 RPG_Graphics_SDL_Tools::CLR32_GREEN      = 0;
Uint32 RPG_Graphics_SDL_Tools::CLR32_YELLOW     = 0;
Uint32 RPG_Graphics_SDL_Tools::CLR32_ORANGE     = 0;
Uint32 RPG_Graphics_SDL_Tools::CLR32_RED        = 0;
Uint32 RPG_Graphics_SDL_Tools::CLR32_GRAY20     = 0;
Uint32 RPG_Graphics_SDL_Tools::CLR32_GRAY20_A10 = 0;
Uint32 RPG_Graphics_SDL_Tools::CLR32_GRAY70     = 0;
Uint32 RPG_Graphics_SDL_Tools::CLR32_GRAY77     = 0;
Uint32 RPG_Graphics_SDL_Tools::CLR32_PURPLE44   = 0;
Uint32 RPG_Graphics_SDL_Tools::CLR32_LIGHTPINK  = 0;
Uint32 RPG_Graphics_SDL_Tools::CLR32_LIGHTGREEN = 0;
Uint32 RPG_Graphics_SDL_Tools::CLR32_BROWN      = 0;
Uint32 RPG_Graphics_SDL_Tools::CLR32_WHITE      = 0;
Uint32 RPG_Graphics_SDL_Tools::CLR32_BLESS_BLUE = 0;
Uint32 RPG_Graphics_SDL_Tools::CLR32_CURSE_RED  = 0;
Uint32 RPG_Graphics_SDL_Tools::CLR32_GOLD_SHADE = 0;

const std::string
RPG_Graphics_SDL_Tools::keyToString(const SDL_keysym& key_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_SDL_Tools::keyToString"));

  std::string result;

  char buffer[RPG_GRAPHICS_SDL_KEYSYM_BUFFER_SIZE];
  result += ACE_TEXT("scancode: ");
  ACE_OS::sprintf(buffer,
                  ACE_TEXT_ALWAYS_CHAR("0x%02X"),
                  key_in.scancode);
  result += buffer;
  result += ACE_TEXT_ALWAYS_CHAR("\n");
  result += ACE_TEXT("name: \"");
  result += SDL_GetKeyName(key_in.sym);
  result += ACE_TEXT_ALWAYS_CHAR("\"\n");
  result += ACE_TEXT("unicode: ");
  ACE_OS::sprintf(buffer,
                  ACE_TEXT_ALWAYS_CHAR("0x%04X"),
                  key_in.unicode);
  result += buffer;
  result += ACE_TEXT_ALWAYS_CHAR("\n");
  result += ACE_TEXT("modifier(s): ");
  if (key_in.mod == KMOD_NONE)
    result += ACE_TEXT("N/A");
  else
  {
    if (key_in.mod & KMOD_NUM)
      result += ACE_TEXT("NUMLOCK ");
    if (key_in.mod & KMOD_CAPS)
      result += ACE_TEXT("CAPSLOCK ");
    if (key_in.mod & KMOD_LCTRL)
      result += ACE_TEXT("LCTRL ");
    if (key_in.mod & KMOD_RCTRL)
      result += ACE_TEXT("RCTRL ");
    if (key_in.mod & KMOD_LSHIFT)
      result += ACE_TEXT("LSHIFT ");
    if (key_in.mod & KMOD_RSHIFT)
      result += ACE_TEXT("RSHIFT ");
    if (key_in.mod & KMOD_LALT)
      result += ACE_TEXT("LALT ");
    if (key_in.mod & KMOD_RALT)
      result += ACE_TEXT("RALT ");
//     if (key_in.mod & KMOD_CTRL)
//       result += ACE_TEXT("CTRL ");
//     if (key_in.mod & KMOD_SHIFT)
//       result += ACE_TEXT("SHIFT ");
//     if (key_in.mod & KMOD_ALT)
//       result += ACE_TEXT("ALT ");
  } // end ELSE

  return result;
}

const SDL_Color
RPG_Graphics_SDL_Tools::colorToSDLColor(const Uint32& color_in,
                                        const SDL_Surface& targetSurface_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_SDL_Tools::colorToSDLColor"));

  SDL_Color result;

  // extract components from the 32-bit color value
  result.r = (color_in & targetSurface_in.format->Rmask) >> targetSurface_in.format->Rshift;
  result.g = (color_in & targetSurface_in.format->Gmask) >> targetSurface_in.format->Gshift;
  result.b = (color_in & targetSurface_in.format->Bmask) >> targetSurface_in.format->Bshift;
  result.unused = 0;

  return result;
}

void
RPG_Graphics_SDL_Tools::initColors()
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_SDL_Tools::initColors"));

  // set up the colors used in the game
  // *NOTE*: the only way to do this without needing graphics to have been loaded first
  // is to manually create a surface and put it into display format + alpha
  SDL_Surface* dummy = NULL;
  dummy = SDL_CreateRGBSurface((SDL_HWSURFACE | // TRY to (!) place the surface in VideoRAM
                                SDL_ASYNCBLIT |
                                SDL_SRCCOLORKEY |
                                SDL_SRCALPHA),
                               1, // dummy
                               1, // dummy
                               32,
                               ((SDL_BYTEORDER == SDL_LIL_ENDIAN) ? 0x000000FF : 0xFF000000),
                               ((SDL_BYTEORDER == SDL_LIL_ENDIAN) ? 0x0000FF00 : 0x00FF0000),
                               ((SDL_BYTEORDER == SDL_LIL_ENDIAN) ? 0x00FF0000 : 0x0000FF00),
                               ((SDL_BYTEORDER == SDL_LIL_ENDIAN) ? 0xFF000000 : 0x000000FF));
  if (!dummy)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_CreateRGBSurface(): %s, aborting\n"),
               SDL_GetError()));

    return;
  } // end IF
  SDL_Surface* dummy_converted = NULL;
  dummy_converted = SDL_DisplayFormatAlpha(dummy);
  if (!dummy_converted)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_DisplayFormatAlpha(): %s, aborting\n"),
               SDL_GetError()));

    // clean up
    SDL_FreeSurface(dummy);

    return;
  } // end IF

  // clean up
  SDL_FreeSurface(dummy);

//   SDL_PixelFormat* pixelFormat = NULL;
//   try
//   {
//     pixelFormat = new SDL_PixelFormat;
//   }
//   catch (...)
//   {
//     ACE_DEBUG((LM_ERROR,
//                ACE_TEXT("failed to allocate memory(%u): %m, aborting\n"),
//                sizeof(SDL_PixelFormat)));
//
//     // clean up
//     SDL_FreeSurface(dummy_converted);
//
//     return;
//   }
//   if (!pixelFormat)
//   {
//     ACE_DEBUG((LM_ERROR,
//                ACE_TEXT("failed to allocate memory(%u): %m, aborting\n"),
//                sizeof(SDL_PixelFormat)));
//
//     // clean up
//     SDL_FreeSurface(dummy_converted);
//
//     return;
//   } // end IF
//   ACE_OS::memcpy(pixelFormat,
//                  dummy_converted->format,
//                  sizeof(SDL_PixelFormat));

  CLR32_BLACK      = SDL_MapRGBA(dummy_converted->format, 0,0,0, 0xff);
  CLR32_BLACK_A10  = SDL_MapRGBA(dummy_converted->format, 0,0,0, 0x1a);
  CLR32_BLACK_A30  = SDL_MapRGBA(dummy_converted->format, 0,0,0, 0x4d);
  CLR32_BLACK_A50  = SDL_MapRGBA(dummy_converted->format, 0,0,0, 0x80);
  CLR32_BLACK_A70  = SDL_MapRGBA(dummy_converted->format, 0,0,0, 0xb3);
  CLR32_BLACK_A90  = SDL_MapRGBA(dummy_converted->format, 0,0,0, 0xe7);
  CLR32_GREEN      = SDL_MapRGBA(dummy_converted->format, 0x57, 0xff, 0x57, 0xff);
  CLR32_YELLOW     = SDL_MapRGBA(dummy_converted->format, 0xff, 0xff, 0x57, 0xff);
  CLR32_ORANGE     = SDL_MapRGBA(dummy_converted->format, 0xff, 0xc7, 0x3b, 0xff);
  CLR32_RED        = SDL_MapRGBA(dummy_converted->format, 0xff, 0x23, 0x07, 0xff);
  CLR32_GRAY20     = SDL_MapRGBA(dummy_converted->format, 0xb7, 0xab, 0xab, 0xff);
  CLR32_GRAY20_A10 = SDL_MapRGBA(dummy_converted->format, 0xb7, 0xab, 0xab, 0x1a);
  CLR32_GRAY70     = SDL_MapRGBA(dummy_converted->format, 0x53, 0x53, 0x53, 0xff);
  CLR32_GRAY77     = SDL_MapRGBA(dummy_converted->format, 0x43, 0x3b, 0x3b, 0xff);
  CLR32_PURPLE44   = SDL_MapRGBA(dummy_converted->format, 0x4f, 0x43, 0x6f, 0xff);
  CLR32_LIGHTPINK  = SDL_MapRGBA(dummy_converted->format, 0xcf, 0xbb, 0xd3, 0xff);
  CLR32_LIGHTGREEN = SDL_MapRGBA(dummy_converted->format, 0xaa, 0xff, 0xcc, 0xff);
  CLR32_BROWN      = SDL_MapRGBA(dummy_converted->format, 0x9b, 0x6f, 0x57, 0xff);
  CLR32_WHITE      = SDL_MapRGBA(dummy_converted->format, 0xff, 0xff, 0xff, 0xff);
  CLR32_BLESS_BLUE = SDL_MapRGBA(dummy_converted->format, 0x96, 0xdc, 0xfe, 0x60);
  CLR32_CURSE_RED  = SDL_MapRGBA(dummy_converted->format, 0x60, 0x00, 0x00, 0x50);
  CLR32_GOLD_SHADE = SDL_MapRGBA(dummy_converted->format, 0xf0, 0xe0, 0x57, 0x40);

  // clean up
  SDL_FreeSurface(dummy_converted);

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("RPG_Graphics_SDL_Tools: initialized colors...\n")));
}

SDL_Surface*
RPG_Graphics_SDL_Tools::copy(const SDL_Surface& sourceImage_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_SDL_Tools::copy"));

  SDL_Surface* result = NULL;
  result = SDL_CreateRGBSurface((SDL_HWSURFACE | // TRY to (!) place the surface in VideoRAM
                                 SDL_ASYNCBLIT |
                                 SDL_SRCCOLORKEY |
                                 SDL_SRCALPHA),
                                sourceImage_in.w,
                                sourceImage_in.h,
                                sourceImage_in.format->BitsPerPixel,
                                sourceImage_in.format->Rmask,
                                sourceImage_in.format->Gmask,
                                sourceImage_in.format->Bmask,
                                sourceImage_in.format->Amask);
  if (!result)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_CreateRGBSurface(): %s, aborting\n"),
               SDL_GetError()));

    return NULL;
  } // end IF

  // *NOTE*: blitting does not preserve the alpha channel...
  // --> do it manually
//   if (SDL_BlitSurface(&ACE_const_cast(SDL_Surface&, sourceImage_in),
//                       NULL,
//                       result,
//                       NULL))
//   {
//     ACE_DEBUG((LM_ERROR,
//                ACE_TEXT("failed to SDL_BlitSurface(): %s, aborting\n"),
//                SDL_GetError()));
//
//     // clean up
//     SDL_FreeSurface(result);
//
//     return NULL;
//   } // end IF

  // lock surface during pixel access
  if (SDL_MUSTLOCK((&sourceImage_in)))
    if (SDL_LockSurface(&ACE_const_cast(SDL_Surface&, sourceImage_in)))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_LockSurface(): %s, aborting\n"),
               SDL_GetError()));

    // clean up
    SDL_FreeSurface(result);

    return NULL;
  } // end IF

  for (unsigned long i = 0;
       i < ACE_static_cast(unsigned long, sourceImage_in.h);
       i++)
    ::memcpy((ACE_static_cast(unsigned char*, result->pixels) + (result->pitch * i)),
             (ACE_static_cast(unsigned char*, sourceImage_in.pixels) + (sourceImage_in.pitch * i)),
             (sourceImage_in.w * sourceImage_in.format->BytesPerPixel)); // RGBA --> 4 bytes (?!!!)

  if (SDL_MUSTLOCK((&sourceImage_in)))
    SDL_UnlockSurface(&ACE_const_cast(SDL_Surface&, sourceImage_in));

  return result;
}
