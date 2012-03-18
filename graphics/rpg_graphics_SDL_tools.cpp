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
#include "rpg_graphics_common.h"

#include <rpg_common_macros.h>

#include <ace/OS.h>
#include <ace/Log_Msg.h>

// init statics
Uint32 RPG_Graphics_SDL_Tools::CLR32_BLACK_A0   = 0;
Uint32 RPG_Graphics_SDL_Tools::CLR32_BLACK_A10  = 0;
Uint32 RPG_Graphics_SDL_Tools::CLR32_BLACK_A30  = 0;
Uint32 RPG_Graphics_SDL_Tools::CLR32_BLACK_A50  = 0;
Uint32 RPG_Graphics_SDL_Tools::CLR32_BLACK_A70  = 0;
Uint32 RPG_Graphics_SDL_Tools::CLR32_BLACK_A90  = 0;
Uint32 RPG_Graphics_SDL_Tools::CLR32_BLACK      = 0;
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

SDL_Surface*
RPG_Graphics_SDL_Tools::initScreen(const int& width_in,
                                   const int& height_in,
                                   const int& colorDepth_in,
                                   const bool& doubleBuffer_in,
                                   const bool& fullScreen_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_SDL_Tools::initScreen"));

  // sanity check
  char driver[MAXPATHLEN];
  if (!SDL_VideoDriverName(driver,
                           sizeof(driver)))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_VideoDriverName(): \"%s\", aborting\n"),
               SDL_GetError()));

    return NULL;
  } // end IF

  // retrieve/list "best" available video mode
  const SDL_VideoInfo* videoInfo = NULL;
  videoInfo = SDL_GetVideoInfo();
  ACE_ASSERT(videoInfo);
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("*** video capabilities (driver: \"%s\") ***\nhardware surfaces: \"%s\"\nwindow manager: \"%s\"\nhardware to hardware blits accelerated: \"%s\"\nhardware to hardware colorkey blits accelerated: \"%s\"\nhardware to hardware alpha blits accelerated: \"%s\"\nsoftware to hardware blits accelerated: \"%s\"\nsoftware to hardware colorkey blits accelerated: \"%s\"\nsoftware to hardware alpha blits accelerated: \"%s\"\ncolor fills accelerated: \"%s\"\nvideo memory: %d bytes\n*** (suggested) video mode ***\npalette: %@\nbits[bytes]/pixel: %d[%d]\nmask[RGBA]: %x %x %x %x\nshift[RGBA]: %d %d %d %d\nloss[RGBA]: %d %d %d %d\ntransparent colorkey: %d\noverall surface alpha: %d\n"),
             driver,
             (videoInfo->hw_available ? ACE_TEXT("yes") : ACE_TEXT("no")),
             (videoInfo->wm_available ? ACE_TEXT("yes") : ACE_TEXT("no")),
             (videoInfo->blit_hw ? ACE_TEXT("yes") : ACE_TEXT("no")),
             (videoInfo->blit_hw_CC ? ACE_TEXT("yes") : ACE_TEXT("no")),
             (videoInfo->blit_hw_A ? ACE_TEXT("yes") : ACE_TEXT("no")),
             (videoInfo->blit_sw ? ACE_TEXT("yes") : ACE_TEXT("no")),
             (videoInfo->blit_sw_CC ? ACE_TEXT("yes") : ACE_TEXT("no")),
             (videoInfo->blit_sw_A ? ACE_TEXT("yes") : ACE_TEXT("no")),
             (videoInfo->blit_fill ? ACE_TEXT("yes") : ACE_TEXT("no")),
             videoInfo->video_mem,
             videoInfo->vfmt->palette,
             videoInfo->vfmt->BitsPerPixel,
             videoInfo->vfmt->BytesPerPixel,
             videoInfo->vfmt->Rmask, videoInfo->vfmt->Gmask, videoInfo->vfmt->Bmask, videoInfo->vfmt->Amask,
             videoInfo->vfmt->Rshift, videoInfo->vfmt->Gshift, videoInfo->vfmt->Bshift, videoInfo->vfmt->Ashift,
             videoInfo->vfmt->Rloss, videoInfo->vfmt->Gloss, videoInfo->vfmt->Bloss, videoInfo->vfmt->Aloss,
             videoInfo->vfmt->colorkey,
             static_cast<int> (videoInfo->vfmt->alpha)));

  // set surface flags
  Uint32 surface_flags = ((videoInfo->hw_available ? (SDL_HWSURFACE |
                                                      (doubleBuffer_in ? SDL_DOUBLEBUF : 0))
                                                   : SDL_SWSURFACE) |
//                         SDL_ASYNCBLIT | // "...will usually slow down blitting on single CPU machines,
//                                         //  but may provide a speed increase on SMP systems..."
                           SDL_ANYFORMAT | // "...Allow any video depth/pixel-format..."
                           SDL_HWPALETTE | // "...Surface has exclusive palette..."
//                         (videoConfig_in.doubleBuffer ? SDL_DOUBLEBUF : SDL_ANYFORMAT) |
                           (fullScreen_in ? (SDL_FULLSCREEN | SDL_NOFRAME)
                                          : SDL_RESIZABLE));
  // get available fullscreen/hardware/... modes
  SDL_Rect** modes = NULL;
  modes = SDL_ListModes(NULL,           // use same as videoInfo
                        surface_flags); // surface flags
  // --> any valid modes available ?
  if (modes == NULL)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("no available resolutions (flags: %x) --> change settings, aborting\n"),
               surface_flags));

    return NULL;
  } // end IF
  else if (modes == reinterpret_cast<SDL_Rect**> (-1))
  {
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("ALL resolutions available (flags: %x)...\n"),
               surface_flags));
  }
  else
  {
    // print valid modes
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("*** available resolutions (flags: %x) ***\n"),
               surface_flags));
    for (unsigned int i = 0;
         modes[i];
         i++)
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("[%d]: %d x %d\n"),
                 i + 1,
                 modes[i]->w,
                 modes[i]->h));
  } // end ELSE

  // check to see whether the requested mode is possible
  int suggested_bpp = SDL_VideoModeOK(width_in,
                                      height_in,
                                      colorDepth_in,
                                      surface_flags);
  switch (suggested_bpp)
  {
    case 0:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("requested mode (width: %d, height: %d, depth: %d, flags: %x) not supported --> change settings, aborting\n"),
                 width_in,
                 height_in,
                 colorDepth_in,
                 surface_flags));

      return NULL;
    }
    default:
    {
      if (suggested_bpp != colorDepth_in)
      {
        ACE_DEBUG((LM_WARNING,
                   ACE_TEXT("using suggested color depth: %d...\n"),
                   suggested_bpp));
      } // end IF

      break;
    }
  } // end SWITCH

  // switch to graphics mode... here we go !
  SDL_Surface* screen = NULL;
  screen = SDL_SetVideoMode(width_in,
                            height_in,
                            suggested_bpp,
                            surface_flags);
  if (!screen)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_SetVideoMode(%d, %d, %d, %x): \"%s\", aborting\n"),
               width_in,
               height_in,
               suggested_bpp,
               surface_flags,
               SDL_GetError()));

    return NULL;
  } // end IF

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("*** screen flags ***\nsurface: %sRAM\nasynch blits: \"%s\"\nany video depth/pixel-format: \"%s\"\nsurface has exclusive palette: \"%s\"\ndouble-buffered: \"%s\"\nblit uses hardware acceleration: \"%s\"\nblit uses a source color key: \"%s\"\nsurface is RLE encoded: \"%s\"\nblit uses source alpha blending: \"%s\"\nsurface uses preallocated memory: \"%s\"\n"),
             ((screen->flags & SDL_HWSURFACE) ? ACE_TEXT("Video") : ACE_TEXT("")),
             ((screen->flags & SDL_ASYNCBLIT) ? ACE_TEXT("yes") : ACE_TEXT("no")),
             ((screen->flags & SDL_ANYFORMAT) ? ACE_TEXT("yes") : ACE_TEXT("no")),
             ((screen->flags & SDL_HWPALETTE) ? ACE_TEXT("yes") : ACE_TEXT("no")),
             ((screen->flags & SDL_DOUBLEBUF) ? ACE_TEXT("yes") : ACE_TEXT("no")),
             ((screen->flags & SDL_HWACCEL) ? ACE_TEXT("yes") : ACE_TEXT("no")),
             ((screen->flags & SDL_SRCCOLORKEY) ? ACE_TEXT("yes") : ACE_TEXT("no")),
             ((screen->flags & SDL_RLEACCEL) ? ACE_TEXT("yes") : ACE_TEXT("no")),
             ((screen->flags & SDL_SRCALPHA) ? ACE_TEXT("yes") : ACE_TEXT("no")),
             ((screen->flags & SDL_PREALLOC) ? ACE_TEXT("yes") : ACE_TEXT("no"))));

  return screen;
}

const std::string
RPG_Graphics_SDL_Tools::keyToString(const SDL_keysym& key_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_SDL_Tools::keyToString"));

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
  RPG_TRACE(ACE_TEXT("RPG_Graphics_SDL_Tools::colorToSDLColor"));

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
  RPG_TRACE(ACE_TEXT("RPG_Graphics_SDL_Tools::initColors"));

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

  CLR32_BLACK_A0   = SDL_MapRGBA(dummy_converted->format, 0, 0, 0, 0);
  CLR32_BLACK_A10  = SDL_MapRGBA(dummy_converted->format, 0, 0, 0, 0x1a);
  CLR32_BLACK_A30  = SDL_MapRGBA(dummy_converted->format, 0, 0, 0, 0x4d);
  CLR32_BLACK_A50  = SDL_MapRGBA(dummy_converted->format, 0, 0, 0, 0x80);
  CLR32_BLACK_A70  = SDL_MapRGBA(dummy_converted->format, 0, 0, 0, 0xb3);
  CLR32_BLACK_A90  = SDL_MapRGBA(dummy_converted->format, 0, 0, 0, 0xe7);
  CLR32_BLACK      = SDL_MapRGBA(dummy_converted->format, 0, 0, 0, 0xff);
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

const SDL_Rect
RPG_Graphics_SDL_Tools::boundingBox(const SDL_Rect& rect1_in,
                                    const SDL_Rect& rect2_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_SDL_Tools::boundingBox"));

  // init result
  SDL_Rect result;
  result.x = static_cast<int16_t>((rect1_in.x < rect2_in.x) ? rect1_in.x : rect2_in.x);
  result.y = static_cast<int16_t>((rect1_in.y < rect2_in.y) ? rect1_in.y : rect2_in.y);
  result.w = static_cast<uint16_t>((((rect1_in.x + rect1_in.w) > (rect2_in.x + rect2_in.w)) ? (rect1_in.x + rect1_in.w - 1) : (rect2_in.x + rect2_in.w - 1)) - result.x) + 1;
  result.h = static_cast<uint16_t>((((rect1_in.y + rect1_in.h) > (rect2_in.y + rect2_in.h)) ? (rect1_in.y + rect1_in.h - 1) : (rect2_in.y + rect2_in.h - 1)) - result.y) + 1;

  return result;
}

const SDL_Rect
RPG_Graphics_SDL_Tools::intersect(const SDL_Rect& rect1_in,
                                  const SDL_Rect& rect2_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_SDL_Tools::intersect"));

  // init result
  SDL_Rect result;
  ACE_OS::memset(&result,
                 0,
                 sizeof(SDL_Rect));

  // test for intersection first
  if ((((rect1_in.x >= rect2_in.x) && (rect1_in.x <= (rect2_in.x + rect2_in.w))) ||
       ((rect2_in.x >= rect1_in.x) && (rect2_in.x <= (rect1_in.x + rect1_in.w)))) &&
      (((rect1_in.y >= rect2_in.y) && (rect1_in.y <= (rect2_in.y + rect2_in.h))) ||
       ((rect2_in.y >= rect1_in.y) && (rect2_in.y <= (rect1_in.y + rect1_in.h)))))
  {
    // compute overlap
    result.x = static_cast<int16_t>((rect1_in.x > rect2_in.x) ? rect1_in.x : rect2_in.x);
    result.y = static_cast<int16_t>((rect1_in.y > rect2_in.y) ? rect1_in.y : rect2_in.y);
    result.w = static_cast<uint16_t>((((rect1_in.x + rect1_in.w) < (rect2_in.x + rect2_in.w)) ? (rect1_in.x + rect1_in.w - 1) : (rect2_in.x + rect2_in.w - 1)) - result.x) + 1;
    result.h = static_cast<uint16_t>((((rect1_in.y + rect1_in.h) < (rect2_in.y + rect2_in.h)) ? (rect1_in.y + rect1_in.h - 1) : (rect2_in.y + rect2_in.h - 1)) - result.y) + 1;
  } // end IF

  return result;
}
