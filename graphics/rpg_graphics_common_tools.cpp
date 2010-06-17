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
#include "rpg_graphics_common_tools.h"

#include "rpg_graphics_defines.h"
#include "rpg_graphics_dictionary.h"

#include <rpg_common_file_tools.h>

#include <ace/Log_Msg.h>

#include <png.h>

#include <sstream>

// init statics
RPG_Graphics_CategoryToStringTable_t RPG_Graphics_CategoryHelper::myRPG_Graphics_CategoryToStringTable;
RPG_Graphics_TileOrientationToStringTable_t RPG_Graphics_TileOrientationHelper::myRPG_Graphics_TileOrientationToStringTable;
RPG_Graphics_FloorStyleToStringTable_t RPG_Graphics_FloorStyleHelper::myRPG_Graphics_FloorStyleToStringTable;
RPG_Graphics_StairsStyleToStringTable_t RPG_Graphics_StairsStyleHelper::myRPG_Graphics_StairsStyleToStringTable;
RPG_Graphics_WallStyleToStringTable_t RPG_Graphics_WallStyleHelper::myRPG_Graphics_WallStyleToStringTable;
RPG_Graphics_DoorStyleToStringTable_t RPG_Graphics_DoorStyleHelper::myRPG_Graphics_DoorStyleToStringTable;
RPG_Graphics_TypeToStringTable_t RPG_Graphics_TypeHelper::myRPG_Graphics_TypeToStringTable;
RPG_Graphics_InterfaceElementTypeToStringTable_t RPG_Graphics_InterfaceElementTypeHelper::myRPG_Graphics_InterfaceElementTypeToStringTable;
RPG_Graphics_HotspotTypeToStringTable_t RPG_Graphics_HotspotTypeHelper::myRPG_Graphics_HotspotTypeToStringTable;

std::string                  RPG_Graphics_Common_Tools::myGraphicsDirectory;

ACE_Thread_Mutex             RPG_Graphics_Common_Tools::myCacheLock;
unsigned long                RPG_Graphics_Common_Tools::myOldestCacheEntry = 0;
unsigned long                RPG_Graphics_Common_Tools::myCacheSize = 0;
RPG_Graphics_GraphicsCache_t RPG_Graphics_Common_Tools::myGraphicsCache;

RPG_Graphics_FontCache_t     RPG_Graphics_Common_Tools::myFontCache;

bool                         RPG_Graphics_Common_Tools::myInitialized = false;

Uint32 RPG_Graphics_Common_Tools::CLR32_BLACK      = 0;
Uint32 RPG_Graphics_Common_Tools::CLR32_BLACK_A30  = 0;
Uint32 RPG_Graphics_Common_Tools::CLR32_BLACK_A50  = 0;
Uint32 RPG_Graphics_Common_Tools::CLR32_BLACK_A70  = 0;
Uint32 RPG_Graphics_Common_Tools::CLR32_GREEN      = 0;
Uint32 RPG_Graphics_Common_Tools::CLR32_YELLOW     = 0;
Uint32 RPG_Graphics_Common_Tools::CLR32_ORANGE     = 0;
Uint32 RPG_Graphics_Common_Tools::CLR32_RED        = 0;
Uint32 RPG_Graphics_Common_Tools::CLR32_GRAY20     = 0;
Uint32 RPG_Graphics_Common_Tools::CLR32_GRAY70     = 0;
Uint32 RPG_Graphics_Common_Tools::CLR32_GRAY77     = 0;
Uint32 RPG_Graphics_Common_Tools::CLR32_PURPLE44   = 0;
Uint32 RPG_Graphics_Common_Tools::CLR32_LIGHTPINK  = 0;
Uint32 RPG_Graphics_Common_Tools::CLR32_LIGHTGREEN = 0;
Uint32 RPG_Graphics_Common_Tools::CLR32_BROWN      = 0;
Uint32 RPG_Graphics_Common_Tools::CLR32_WHITE      = 0;
Uint32 RPG_Graphics_Common_Tools::CLR32_BLESS_BLUE = 0;
Uint32 RPG_Graphics_Common_Tools::CLR32_CURSE_RED  = 0;
Uint32 RPG_Graphics_Common_Tools::CLR32_GOLD_SHADE = 0;

static void
PNG_read_callback(png_structp png_ptr_in,
                  png_bytep target_inout,
                  png_size_t size_in)
{
  ACE_TRACE(ACE_TEXT("::PNG_read_callback"));

  ACE_OS::memcpy(target_inout,
                 ACE_static_cast(void*, png_ptr_in->io_ptr),
                 size_in);
  png_ptr_in->io_ptr = (ACE_static_cast(unsigned char*, png_ptr_in->io_ptr) + size_in);
}

void
RPG_Graphics_Common_Tools::init(const std::string& directory_in,
                                const unsigned long& cacheSize_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Common_Tools::init"));

  // sanity check(s)
  if (!RPG_Common_File_Tools::isDirectory(directory_in))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid argument \"%s\": not a directory, aborting\n"),
               directory_in.c_str()));

    return;
  } // end IF
  myGraphicsDirectory = directory_in;
  myCacheSize = cacheSize_in;

  if (myInitialized)
  {
    // clean up
    fini();

    myInitialized = false;
  } // end IF
  else
  {
    // init string conversion
    initStringConversionTables();
    // init colors
    initColors();
  } // end ELSE

  // init fonts
  myInitialized = initFonts();
}

void
RPG_Graphics_Common_Tools::fini()
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Common_Tools::fini"));

  // synch cache access
  {
    ACE_Guard<ACE_Thread_Mutex> aGuard(myCacheLock);

    // clear the graphics cache
    for (RPG_Graphics_GraphicsCacheIterator_t iter = myGraphicsCache.begin();
         iter != myGraphicsCache.end();
         iter++)
    {
      SDL_FreeSurface((*iter).image);
    } // end FOR
    myGraphicsCache.clear();
    myOldestCacheEntry = 0;

    // clear the font cache
    for (RPG_Graphics_FontCacheIterator_t iter = myFontCache.begin();
         iter != myFontCache.end();
         iter++)
    {
      TTF_CloseFont((*iter).second);
    } // end FOR
    myFontCache.clear();
  } // end lock scope

  myInitialized = false;
}

const std::string
RPG_Graphics_Common_Tools::styleToString(const RPG_Graphics_StyleUnion& style_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Common_Tools::styleToString"));

  switch (style_in.discriminator)
  {
    case RPG_Graphics_StyleUnion::FLOORSTYLE:
      return RPG_Graphics_FloorStyleHelper::RPG_Graphics_FloorStyleToString(style_in.floorstyle);
    case RPG_Graphics_StyleUnion::STAIRSSTYLE:
      return RPG_Graphics_StairsStyleHelper::RPG_Graphics_StairsStyleToString(style_in.stairsstyle);
    case RPG_Graphics_StyleUnion::WALLSTYLE:
      return RPG_Graphics_WallStyleHelper::RPG_Graphics_WallStyleToString(style_in.wallstyle);
    case RPG_Graphics_StyleUnion::DOORSTYLE:
      return RPG_Graphics_DoorStyleHelper::RPG_Graphics_DoorStyleToString(style_in.doorstyle);
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid style (was: %d), aborting\n"),
                 style_in.discriminator));

      break;
    }
  } // end SWITCH

  return std::string(ACE_TEXT_ALWAYS_CHAR("INVALID"));
}

const std::string
RPG_Graphics_Common_Tools::elementTypeToString(const RPG_Graphics_ElementTypeUnion& elementType_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Common_Tools::elementTypeToString"));

  switch (elementType_in.discriminator)
  {
    case RPG_Graphics_ElementTypeUnion::INTERFACEELEMENTTYPE:
      return RPG_Graphics_InterfaceElementTypeHelper::RPG_Graphics_InterfaceElementTypeToString(elementType_in.interfaceelementtype);
    case RPG_Graphics_ElementTypeUnion::HOTSPOTTYPE:
      return RPG_Graphics_HotspotTypeHelper::RPG_Graphics_HotspotTypeToString(elementType_in.hotspottype);
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid element type (was: %d), aborting\n"),
                 elementType_in.discriminator));

      break;
    }
  } // end SWITCH

  return std::string(ACE_TEXT_ALWAYS_CHAR("INVALID"));
}

const std::string
RPG_Graphics_Common_Tools::elementsToString(const RPG_Graphics_Elements_t& elements_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Common_Tools::elementsToString"));

  std::string result;

  std::ostringstream converter;
  unsigned long index = 0;
  for (RPG_Graphics_ElementsConstIterator_t iterator = elements_in.begin();
       iterator != elements_in.end();
       iterator++, index++)
  {
    result += ACE_TEXT_ALWAYS_CHAR("#");
    converter.clear();
    converter.str(ACE_TEXT_ALWAYS_CHAR(""));
    converter << index;
    result += converter.str();
    result += ACE_TEXT_ALWAYS_CHAR(" ");
    result += RPG_Graphics_Common_Tools::elementTypeToString((*iterator).type);
    result += ACE_TEXT_ALWAYS_CHAR(" {x: ");
    converter.clear();
    converter.str(ACE_TEXT_ALWAYS_CHAR(""));
    converter << (*iterator).offsetX;
    result += converter.str();
    result += ACE_TEXT_ALWAYS_CHAR(", y: ");
    converter.clear();
    converter.str(ACE_TEXT_ALWAYS_CHAR(""));
    converter << (*iterator).offsetY;
    result += converter.str();
    result += ACE_TEXT_ALWAYS_CHAR(", w: ");
    converter.clear();
    converter.str(ACE_TEXT_ALWAYS_CHAR(""));
    converter << (*iterator).width;
    result += converter.str();
    result += ACE_TEXT_ALWAYS_CHAR(", h: ");
    converter.clear();
    converter.str(ACE_TEXT_ALWAYS_CHAR(""));
    converter << (*iterator).height;
    result += converter.str();
    result += ACE_TEXT_ALWAYS_CHAR("}\n");
  } // end FOR

  return result;
}

SDL_Surface*
RPG_Graphics_Common_Tools::loadGraphic(const RPG_Graphics_Type& type_in,
                                       const bool& cacheGraphic_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Common_Tools::loadGraphic"));

  RPG_Graphics_GraphicsCacheIterator_t iter;
  RPG_Graphics_GraphicsCacheNode_t node;
  node.type = type_in;
  // init return value(s)
  node.image = NULL;

  // step1: graphic already cached ?
  if (cacheGraphic_in)
  {
    // synch access to graphics cache
    {
      ACE_Guard<ACE_Thread_Mutex> aGuard(myCacheLock);

      for (iter = myGraphicsCache.begin();
           iter != myGraphicsCache.end();
           iter++)
        if ((*iter) == node)
          break;

      if (iter != myGraphicsCache.end())
        return (*iter).image;
    } // end lock scope
  } // end IF

  // step2: load image from file
  RPG_Graphics_t graphic;
  graphic.type = RPG_GRAPHICS_TYPE_INVALID;

  // retrieve properties from the dictionary
  graphic = RPG_GRAPHICS_DICTIONARY_SINGLETON::instance()->getGraphic(type_in);
  ACE_ASSERT(graphic.type == type_in);

  std::string path = myGraphicsDirectory;
  path += ACE_DIRECTORY_SEPARATOR_STR;
  path += graphic.file;
  // sanity check
  if (!RPG_Common_File_Tools::isReadable(path))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid argument(\"%s\"): not readable, aborting\n"),
               path.c_str()));

    return NULL;
  } // end IF

  // load complete file into memory
  // *TODO*: this isn't really necessary...
  unsigned char* srcbuf = NULL;
  if (!RPG_Common_File_Tools::loadFile(path,
                                       srcbuf))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Common_File_Tools::loadFile(\"%s\"), aborting\n"),
               path.c_str()));

    return NULL;
  } // end IF

  // extract SDL surface from PNG
  if (!loadPNG(srcbuf,
               node.image))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to loadPNG(\"%s\"), aborting\n"),
               path.c_str()));

    // clean up
    delete[] srcbuf;

    return NULL;
  } // end IF

  // clean up
  delete[] srcbuf;

  if (cacheGraphic_in)
  {
    // synch cache access
    {
      ACE_Guard<ACE_Thread_Mutex> aGuard(myCacheLock);

      if (myGraphicsCache.size() == myCacheSize)
      {
        iter = myGraphicsCache.begin();
        std::advance(iter, myOldestCacheEntry);
        // *TODO*: what if it's still being used ?...
        SDL_FreeSurface((*iter).image);
        myGraphicsCache.erase(iter);
        myOldestCacheEntry++;
        if (myOldestCacheEntry == myCacheSize)
          myOldestCacheEntry = 0;
      } // end IF

      myGraphicsCache.push_back(node);
    } // end lock scope
  } // end IF

  return node.image;
}

SDL_Surface*
RPG_Graphics_Common_Tools::renderText(const RPG_Graphics_Type& type_in,
                                      const std::string& textString_in,
                                      const SDL_Color& color_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Common_Tools::renderText"));

  // init return value(s)
  SDL_Surface* result = NULL;

  // synch cache access
  {
    ACE_Guard<ACE_Thread_Mutex> aGuard(myCacheLock);

    // step1: retrieve font cache entry
    RPG_Graphics_FontCacheIterator_t iterator = myFontCache.find(type_in);
    ACE_ASSERT(iterator != myFontCache.end());

    result = TTF_RenderText_Blended((*iterator).second,
                                    textString_in.c_str(),
                                    color_in);
  } // end lock scope
  if (!result)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to TTF_RenderText_Blended(\"%s\"): %s, aborting\n"),
               textString_in.c_str(),
               SDL_GetError()));

    return NULL;
  } // end IF

  return result;
}

SDL_Surface*
RPG_Graphics_Common_Tools::get(const unsigned long& offsetX_in,
                               const unsigned long& offsetY_in,
                               const unsigned long& width_in,
                               const unsigned long& height_in,
                               const SDL_Surface* image_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Common_Tools::get"));

  // sanity check(s)
  ACE_ASSERT(image_in);
  ACE_ASSERT(width_in <= ACE_static_cast(unsigned long, image_in->w));
  ACE_ASSERT((offsetX_in + width_in) <= (ACE_static_cast(unsigned long, image_in->w) - 1));
  ACE_ASSERT(height_in <= ACE_static_cast(unsigned long, image_in->h));
  ACE_ASSERT((offsetY_in + height_in) <= (ACE_static_cast(unsigned long, image_in->h) - 1));

  // init return value
  SDL_Surface* result = NULL;
  result = SDL_CreateRGBSurface((SDL_HWSURFACE | // TRY to (!) place the surface in VideoRAM
                                 SDL_ASYNCBLIT |
                                 SDL_SRCCOLORKEY |
                                 SDL_SRCALPHA),
                                width_in,
                                height_in,
                                image_in->format->BitsPerPixel,
                                image_in->format->Rmask,
                                image_in->format->Gmask,
                                image_in->format->Bmask,
                                image_in->format->Amask);

  // *NOTE*: blitting does not preserve the alpha channel...

  // lock the surface for direct access to the pixels
  if (SDL_MUSTLOCK(image_in))
    if (SDL_LockSurface(ACE_const_cast(SDL_Surface*, image_in)))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to SDL_LockSurface(): %s, aborting\n"),
                 SDL_GetError()));

      // clean up
      SDL_FreeSurface(result);

      return NULL;
    } // end IF

  for (unsigned long i = 0;
       i < height_in;
       i++)
    ::memcpy((ACE_static_cast(unsigned char*, result->pixels) + (result->pitch * i)),
             (ACE_static_cast(unsigned char*, image_in->pixels) + ((offsetY_in + i) * image_in->pitch) + (offsetX_in * 4)),
             (width_in * 4)); // RGBA --> 4 bytes (?!!!)

  if (SDL_MUSTLOCK(image_in))
    SDL_UnlockSurface(ACE_const_cast(SDL_Surface*, image_in));

//   // bounding box
//   SDL_Rect toRect;
//   toRect.x = topLeftX_in;
//   toRect.y = topLeftY_in;
//   toRect.w = (bottomRightX_in - topLeftX_in) + 1;
//   toRect.h = (bottomRightY_in - topLeftY_in) + 1;
//   if (SDL_BlitSurface(ACE_const_cast(SDL_Surface*, image_in), // source
//                       &toRect,                                // aspect
//                       subImage_out,                           // target
//                       NULL))                                  // aspect (--> everything)
//   {
//     ACE_DEBUG((LM_ERROR,
//                ACE_TEXT("failed to SDL_BlitSurface(): %s, aborting\n"),
//                SDL_GetError()));
//
//     return;
//   } // end IF

  return result;
}

void
RPG_Graphics_Common_Tools::put(const unsigned long& offsetX_in,
                               const unsigned long& offsetY_in,
                               const SDL_Surface& image_in,
                               SDL_Surface* targetSurface_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Common_Tools::put"));

  // sanity check(s)
  ACE_ASSERT(targetSurface_in);

  // compute bounding box
  SDL_Rect toRect;
  toRect.x = offsetX_in;
  toRect.y = offsetY_in;
  toRect.w = image_in.w;
  toRect.h = image_in.h;

  if (SDL_BlitSurface(&ACE_const_cast(SDL_Surface&, image_in), // source
                      NULL,                                    // aspect (--> everything)
                      targetSurface_in,                        // target
                      &toRect))                                // aspect
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_BlitSurface(): %s, aborting\n"),
               SDL_GetError()));

    return;
  } // end IF
}

void
RPG_Graphics_Common_Tools::putText(const RPG_Graphics_Type& type_in,
                                   const std::string& textString_in,
                                   const SDL_Color& color_in,
                                   const bool& shade_in,
                                   const unsigned long& offsetX_in,
                                   const unsigned long& offsetY_in,
                                   SDL_Surface* targetSurface_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Common_Tools::putText"));

  // render text
  SDL_Surface* rendered_text = NULL;
  rendered_text = renderText(type_in,
                             textString_in,
                             color_in);
  if (!rendered_text)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Graphics_Common_Tools::renderText(\"%s\", \"%s\"), aborting\n"),
               RPG_Graphics_TypeHelper::RPG_Graphics_TypeToString(type_in).c_str(),
               textString_in.c_str()));

    return;
  } // end IF

  put(offsetX_in,
      offsetY_in,
      *rendered_text,
      targetSurface_in);

  // clean up
  SDL_FreeSurface(rendered_text);
}

void
RPG_Graphics_Common_Tools::fade(const bool& fadeIn_in,
                                const float& interval_in,
                                const Uint32& color_in,
                                SDL_Surface* screen_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Common_Tools::fade"));

  SDL_Surface* target_image = NULL;
  // step1: create a screen-sized surface without an alpha-channel
  // --> i.e. (alpha mask == 0)
  target_image = SDL_CreateRGBSurface((SDL_HWSURFACE | // TRY to (!) place the surface in VideoRAM
                                       SDL_ASYNCBLIT |
                                       SDL_SRCCOLORKEY |
                                       SDL_SRCALPHA),
                                      screen_in->w,
                                      screen_in->h,
                                      screen_in->format->BitsPerPixel,
                                      screen_in->format->Rmask,
                                      screen_in->format->Gmask,
                                      screen_in->format->Bmask,
                                      0);
  if (!target_image)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_CreateRGBSurface(): %s, aborting\n"),
               SDL_GetError()));

    return;
  } // end IF

  if (fadeIn_in)
  {
    // ...copy the pixel data from the framebuffer
    if (SDL_BlitSurface(screen_in,
                        NULL,
                        target_image,
                        NULL))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to SDL_BlitSurface(): %s, aborting\n"),
                 SDL_GetError()));

      // clean up
      SDL_FreeSurface(target_image);

      return;
    } // end IF

    // set the screen to the background color (black ?)
    if (SDL_FillRect(screen_in, // screen
                     NULL,      // fill screen
                     color_in)) // black ?
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to SDL_FillRect(): %s, aborting\n"),
                 SDL_GetError()));

      // clean up
      SDL_FreeSurface(target_image);

      return;
    } // end IF
    // ...and display that
    if (SDL_Flip(screen_in))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to SDL_Flip(): %s, aborting\n"),
                 SDL_GetError()));

      // clean up
      SDL_FreeSurface(target_image);

      return;
    } // end IF
  } // end IF
  else
  {
    // fill the target image with the requested color
    if (SDL_FillRect(target_image, // screen
                     NULL,         // fill screen
                     color_in))    // target color
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to SDL_FillRect(): %s, aborting\n"),
                 SDL_GetError()));

      // clean up
      SDL_FreeSurface(target_image);

      return;
    } // end IF
  } // end ELSE

  // step4: slowly fade in/out
  fade(interval_in,
       target_image,
       screen_in);

  // clean up
  SDL_FreeSurface(target_image);
}

void
RPG_Graphics_Common_Tools::initStringConversionTables()
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Common_Tools::initStringConversionTables"));

  RPG_Graphics_CategoryHelper::init();
  RPG_Graphics_TileOrientationHelper::init();
  RPG_Graphics_FloorStyleHelper::init();
  RPG_Graphics_StairsStyleHelper::init();
  RPG_Graphics_WallStyleHelper::init();
  RPG_Graphics_DoorStyleHelper::init();
  RPG_Graphics_TypeHelper::init();
  RPG_Graphics_InterfaceElementTypeHelper::init();
  RPG_Graphics_HotspotTypeHelper::init();

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("RPG_Graphics_Common_Tools: initialized string conversion tables...\n")));
}

void
RPG_Graphics_Common_Tools::initColors()
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Common_Tools::initColors"));

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
  CLR32_BLACK_A30  = SDL_MapRGBA(dummy_converted->format, 0,0,0, 0x50);
  CLR32_BLACK_A50  = SDL_MapRGBA(dummy_converted->format, 0,0,0, 0x80);
  CLR32_BLACK_A70  = SDL_MapRGBA(dummy_converted->format, 0,0,0, 0xB0);
  CLR32_GREEN      = SDL_MapRGBA(dummy_converted->format, 0x57, 0xff, 0x57, 0xff);
  CLR32_YELLOW     = SDL_MapRGBA(dummy_converted->format, 0xff, 0xff, 0x57, 0xff);
  CLR32_ORANGE     = SDL_MapRGBA(dummy_converted->format, 0xff, 0xc7, 0x3b, 0xff);
  CLR32_RED        = SDL_MapRGBA(dummy_converted->format, 0xff, 0x23, 0x07, 0xff);
  CLR32_GRAY20     = SDL_MapRGBA(dummy_converted->format, 0xb7, 0xab, 0xab, 0xff);
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
             ACE_TEXT("RPG_Graphics_Common_Tools: initialized colors...\n")));
}

const bool
RPG_Graphics_Common_Tools::initFonts()
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Common_Tools::initFonts"));

  // step1: retrieve list of configured fonts
  RPG_Graphics_Fonts_t fonts = RPG_GRAPHICS_DICTIONARY_SINGLETON::instance()->getFonts();

  // step2: load all fonts into the cache
  std::string path = myGraphicsDirectory;
  TTF_Font* font = NULL;

  // synch cache access
  {
    ACE_Guard<ACE_Thread_Mutex> aGuard(myCacheLock);

    for (RPG_Graphics_FontsConstIterator_t iterator = fonts.begin();
         iterator != fonts.end();
         iterator++)
    {
      font = NULL;

      // construct FQ filename
      path = myGraphicsDirectory;
      path += ACE_DIRECTORY_SEPARATOR_STR;
      path += (*iterator).file;
      // sanity check
      if (!RPG_Common_File_Tools::isReadable(path))
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("invalid file(\"%s\"): not readable, aborting\n"),
                   path.c_str()));

        // clean up
        for (RPG_Graphics_FontCacheIterator_t iter = myFontCache.begin();
             iter != myFontCache.end();
             iter++)
        {
          TTF_CloseFont((*iter).second);
        } // end FOR
        myFontCache.clear();

        return false;
      } // end IF

      font = TTF_OpenFont(path.c_str(),      // filename
                          (*iterator).size); // point size
      if (!font)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to TTF_OpenFont(\"%s\", %u): %s, continuing\n"),
                   path.c_str(),
                   (*iterator).size,
                   SDL_GetError()));

        // clean up
        for (RPG_Graphics_FontCacheIterator_t iter = myFontCache.begin();
             iter != myFontCache.end();
             iter++)
        {
          TTF_CloseFont((*iter).second);
        } // end FOR
        myFontCache.clear();

        return false;
      } // end IF

      // cache this font
      myFontCache.insert(std::make_pair((*iterator).type, font));
    } // end FOR
  } // end lock scope

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("RPG_Graphics_Common_Tools: loaded %u font(s)...\n"),
             fonts.size()));

  return true;
}

const bool
RPG_Graphics_Common_Tools::loadPNG(const unsigned char* buffer_in,
                                   SDL_Surface*& surface_out)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Common_Tools::loadPNG"));

  // init return value(s)
  surface_out = NULL;

  // sanity check
  //--> buffer must contain a PNG file
  if (png_sig_cmp(ACE_const_cast(unsigned char*, buffer_in), // buffer
                  0,                                         // start at the beginning
                  RPG_GRAPHICS_PNG_SIGNATURE_BYTES))         // #signature bytes to check
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to png_sig_cmp(): %m, aborting\n")));

    return false;
  } // end IF

  png_structp png_ptr = NULL;
  // create the PNG loading context structure
  png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, // version
                                   NULL,                  // error
                                   NULL,                  // error handler
                                   NULL);                 // warning handler
  if (!png_ptr)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to png_create_read_struct(): %m, aborting\n")));

    return false;
  } // end IF

  // *NOTE* need to cleanup "png_ptr" beyond this point

  png_infop info_ptr = NULL;
  // allocate/initialize memory for any image information [REQUIRED]
  info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to png_create_info_struct(): %m, aborting\n")));

    // clean up
    png_destroy_read_struct(&png_ptr,
                            NULL,
                            NULL);

    return false;
  } // end IF

  // *NOTE* need to cleanup "png_ptr" and "info_ptr" beyond this point

  // save stack context, set up error handling
  if (::setjmp(png_ptr->jmpbuf))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ::setjmp(): %m, aborting\n")));

    // clean up
    png_destroy_read_struct(&png_ptr,
                            &info_ptr,
                            NULL);

    return false;
  } // end IF

  // set up data input callback
  // *TODO*: use png_init_io() instead (load directly from the file...)
  png_set_read_fn(png_ptr,
                  ACE_const_cast(unsigned char*, buffer_in),
                  PNG_read_callback);

  // read PNG header info
  png_read_info(png_ptr,
                info_ptr);
  png_uint_32 width = 0;
  png_uint_32 height = 0;
  int bit_depth = 0;
  int color_type = 0;
  int interlace = 0;
  int compression = 0;
  int filter = 0;
  png_get_IHDR(png_ptr,
               info_ptr,
               &width,
               &height,
               &bit_depth,
               &color_type,
               &interlace,
               &compression,
               &filter);

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("loading PNG [h,w,d,#c,t,i,c,f]: %u,%u,%d,%u,%d,%d,%d,%d...\n"),
             width,
             height,
             bit_depth,
             ACE_static_cast(unsigned long, png_get_channels(png_ptr, info_ptr)),
             color_type,
             interlace,
             compression,
             filter));

  // reduce 16 bit per channel to 8 bit per channel
  // --> strip the second byte of information from a 16-bit depth file
  png_set_strip_16(png_ptr);

  // expand:
  // - 1,2,4 bit grayscale to 8 bit grayscale
  // - paletted images to RGB
  // - tRNS to true alpha channel
  // --> expand data to 24-bit RGB, or 8-bit grayscale, with alpha if available
  png_set_expand(png_ptr);

  // expand grayscale to full RGB
  // --> expand the grayscale to 24-bit RGB if necessary
  png_set_gray_to_rgb(png_ptr);

  // add an opaque alpha channel to anything that doesn't have one yet
  // --> add a filler byte to 8-bit Gray or 24-bit RGB images
  png_set_filler(png_ptr,
                 0xff,
                 PNG_FILLER_AFTER);
  png_set_add_alpha(png_ptr,
                    0xff,
                    PNG_FILLER_AFTER);
  info_ptr->color_type |= PNG_COLOR_MASK_ALPHA;

//   Uint32 Rmask, Gmask, Bmask, Amask = 0;
//   // get the component mask for the surface
//   if (SDL_BYTEORDER == SDL_LIL_ENDIAN)
//   {
//     Rmask = 0x000000FF;
//     Gmask = 0x0000FF00;
//     Bmask = 0x00FF0000;
//     Amask = 0xFF000000;
//   } // end IF
//   else
//   {
//     Rmask = 0xFF000000;
//     Gmask = 0x00FF0000;
//     Bmask = 0x0000FF00;
//     Amask = 0x000000FF;
//   } // end ELSE

  // allocate surface
  // *NOTE*:
  // - always TRY to (!) place the surface in VideoRAM; this may not work (check flags after loading !)
  // - conversion between PNG header info <--> SDL argument formats requires some casts
  surface_out = SDL_CreateRGBSurface((SDL_HWSURFACE | // TRY to (!) place the surface in VideoRAM
                                      SDL_ASYNCBLIT |
                                      SDL_SRCCOLORKEY |
                                      SDL_SRCALPHA),
                                     ACE_static_cast(int, width),
                                     ACE_static_cast(int, height),
//                                      (bit_depth * 8),
                                     32,
                                     ((SDL_BYTEORDER == SDL_LIL_ENDIAN) ? 0x000000FF : 0xFF000000),
                                     ((SDL_BYTEORDER == SDL_LIL_ENDIAN) ? 0x0000FF00 : 0x00FF0000),
                                     ((SDL_BYTEORDER == SDL_LIL_ENDIAN) ? 0x00FF0000 : 0x0000FF00),
                                     ((SDL_BYTEORDER == SDL_LIL_ENDIAN) ? 0xFF000000 : 0x000000FF));
//                                      Rmask,
//                                      Gmask,
//                                      Bmask,
//                                      Amask);
  if (!surface_out)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_CreateRGBSurface(): %s, aborting\n"),
               SDL_GetError()));

    // clean up
    png_destroy_read_struct(&png_ptr,
                             &info_ptr,
                             NULL);

    return false;
  } // end IF

  // *NOTE* need to cleanup "png_ptr", "info_ptr" and "surface_out" beyond this point

  // create the array of row pointers to image data
  png_bytep* row_pointers = NULL;
  try
  {
    row_pointers = new png_bytep[height];
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to allocate memory(%u): %m, aborting\n"),
               (sizeof(png_bytep) * height)));

    // clean up
    png_destroy_read_struct(&png_ptr,
                            &info_ptr,
                            NULL);
    SDL_FreeSurface(surface_out);
    surface_out = NULL;

    return false;
  }
  if (!row_pointers)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to allocate memory(%u): %m, aborting\n"),
               (sizeof(png_bytep) * height)));

    // clean up
    png_destroy_read_struct(&png_ptr,
                            &info_ptr,
                            NULL);
    SDL_FreeSurface(surface_out);
    surface_out = NULL;

    return false;
  } // end IF

  // *NOTE* need to cleanup beyond this point:
  // - "png_ptr", "info_ptr"
  // - "surface_out"
  // - "row_pointers"

  for (unsigned long row = 0;
       row < height;
       row++)
    row_pointers[row] = ACE_static_cast(png_bytep,
                                        ACE_static_cast(Uint8*,
                                                        surface_out->pixels) + (row * surface_out->pitch));

  // read the image from the memory buffer onto the surface buffer
  // --> read the whole image into memory at once
  png_read_image(png_ptr, row_pointers);

  // clean up
  delete[] row_pointers;
  png_destroy_read_struct(&png_ptr,
                          &info_ptr,
                          NULL);

  SDL_Surface* convert = NULL;
  // *TODO*: is this really necessary at all ?
  // (if possible), convert surface to the pixel format and colors of the
  // video framebuffer (allows fast(er) blitting)
  convert = SDL_DisplayFormatAlpha(surface_out);
  if (!convert)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_DisplayFormatAlpha(): %s, aborting\n"),
               SDL_GetError()));

    // clean up
    SDL_FreeSurface(surface_out);
    surface_out = NULL;

    return false;
  } // end IF

  // clean up
  SDL_FreeSurface(surface_out);
  surface_out = convert;

  return true;
}

// void
// RPG_Graphics_Common_Tools::fade(const double& interval_in,
//                                 SDL_Surface* targetImage_in,
//                                 SDL_Surface* screen_in)
// {
//   ACE_TRACE(ACE_TEXT("RPG_Graphics_Common_Tools::fade"));
//
//   // *NOTE*: fading works by blitting the target image onto the screen,
//   // slowly decreasing transparency to reach full brightness/darkness
//
//   // initialize/start with maximal transparency
//   int alpha = SDL_ALPHA_TRANSPARENT; // transparent
//
//   Uint32 cur_clock, end_clock, start_clock;
//   float percentage = 1.0;
//   start_clock = cur_clock = SDL_GetTicks();
//   end_clock = start_clock + (interval_in * 1000);
//   while (cur_clock <= end_clock)
//   {
//     // adjust transparency
//     if (SDL_SetAlpha(targetImage_in,
//                      (SDL_SRCALPHA | SDL_RLEACCEL), // alpha blending/RLE acceleration
//                      (SDL_ALPHA_TRANSPARENT + alpha)))
//     {
//       ACE_DEBUG((LM_ERROR,
//                  ACE_TEXT("failed to SDL_SetAlpha(): %s, aborting\n"),
//                  SDL_GetError()));
//
//       return;
//     } // end IF
//     // *NOTE*: only change the framebuffer !
//     if (SDL_Flip(screen_in))
//     {
//       ACE_DEBUG((LM_ERROR,
//                  ACE_TEXT("failed to SDL_Flip(): %s, aborting\n"),
//                  SDL_GetError()));
//
//       return;
//     } // end IF
//     if (SDL_BlitSurface(targetImage_in,
//                         NULL,
//                         screen_in,
//                         NULL))
//     {
//       ACE_DEBUG((LM_ERROR,
//                  ACE_TEXT("failed to SDL_BlitSurface(): %s, aborting\n"),
//                  SDL_GetError()));
//
//       return;
//     } // end IF
//     if (SDL_Flip(screen_in))
//     {
//       ACE_DEBUG((LM_ERROR,
//                  ACE_TEXT("failed to SDL_Flip(): %s, aborting\n"),
//                  SDL_GetError()));
//
//       return;
//     } // end IF
//
//     cur_clock = SDL_GetTicks();
//     percentage = (ACE_static_cast(float, (cur_clock - start_clock)) /
//                   ACE_static_cast(float, (end_clock - start_clock)));
//     alpha = ((SDL_ALPHA_OPAQUE - SDL_ALPHA_TRANSPARENT) * percentage);
//   } // end WHILE
//
//   // ensure that the target image is fully faded in
//   if (SDL_SetAlpha(targetImage_in,
//                    (SDL_SRCALPHA | SDL_RLEACCEL), // alpha blending/RLE acceleration
//                    SDL_ALPHA_OPAQUE))
//   {
//     ACE_DEBUG((LM_ERROR,
//                ACE_TEXT("failed to SDL_SetAlpha(): %s, aborting\n"),
//                SDL_GetError()));
//
//     return;
//   } // end IF
//   if (SDL_BlitSurface(targetImage_in,
//                       NULL,
//                       screen_in,
//                       NULL))
//   {
//     ACE_DEBUG((LM_ERROR,
//                ACE_TEXT("failed to SDL_BlitSurface(): %s, aborting\n"),
//                SDL_GetError()));
//
//     return;
//   } // end IF
//   if (SDL_Flip(screen_in))
//   {
//     ACE_DEBUG((LM_ERROR,
//                ACE_TEXT("failed to SDL_Flip(): %s, aborting\n"),
//                SDL_GetError()));
//
//     return;
//   } // end IF
// }

void
RPG_Graphics_Common_Tools::fade(const float& interval_in,
                                SDL_Surface* targetImage_in,
                                SDL_Surface* screen_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Common_Tools::fade"));

  // calculate the number of blends
  int n_steps = RPG_GRAPHICS_FADE_REFRESH_RATE * interval_in;
  if (SDL_SetAlpha(targetImage_in,
                   (SDL_SRCALPHA | SDL_RLEACCEL), // alpha blending/RLE acceleration
                   (SDL_ALPHA_OPAQUE / n_steps)))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_SetAlpha(): %s, aborting\n"),
               SDL_GetError()));

    return;
  } // end IF

  Uint32 cur_clock, end_clock, start_clock, sleeptime, elapsed;
  start_clock = cur_clock = SDL_GetTicks();
  end_clock = start_clock + (interval_in * 1000);
  while (cur_clock <= end_clock)
  {
    // *NOTE*: blitting the image onto the screen repeatedly
    // will slowly add up to full brightness, while
    // drawing over the screen with semi-transparent
    // darkness repeatedly gives a fade-out effect
    if (SDL_BlitSurface(targetImage_in,
                        NULL,
                        screen_in,
                        NULL))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to SDL_BlitSurface(): %s, aborting\n"),
                 SDL_GetError()));

      return;
    } // end IF
    if (SDL_Flip(screen_in))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to SDL_Flip(): %s, aborting\n"),
                 SDL_GetError()));

      return;
    } // end IF

    // delay a little while, to impress the blended image
    elapsed = SDL_GetTicks() - cur_clock;
    sleeptime = ((1000/RPG_GRAPHICS_FADE_REFRESH_RATE) > elapsed) ? ((1000/RPG_GRAPHICS_FADE_REFRESH_RATE) - elapsed)
                                                                  : 0;
    SDL_Delay(sleeptime);

    cur_clock += (elapsed + sleeptime);
  } // end WHILE

  // ensure that the target image is fully faded in
  if (SDL_SetAlpha(targetImage_in,
                   0, // alpha blending/RLE acceleration
                   0))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_SetAlpha(): %s, aborting\n"),
               SDL_GetError()));

    return;
  } // end IF
  if (SDL_BlitSurface(targetImage_in,
                      NULL,
                      screen_in,
                      NULL))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_BlitSurface(): %s, aborting\n"),
               SDL_GetError()));

    return;
  } // end IF
  if (SDL_Flip(screen_in))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_Flip(): %s, aborting\n"),
               SDL_GetError()));

    return;
  } // end IF
}
