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
#include "rpg_graphics_surface.h"

#include "rpg_graphics_defines.h"
#include "rpg_graphics_common_tools.h"
#include "rpg_graphics_SDL_tools.h"

#include <rpg_common_file_tools.h>

#include <ace/OS.h>
#include <ace/Log_Msg.h>

#include <png.h>

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

RPG_Graphics_Surface::RPG_Graphics_Surface(SDL_Surface* surface_in,
                                           const bool& ownSurface_in)
 : mySurface(surface_in),
   myOwnSurface(ownSurface_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Surface::RPG_Graphics_Surface"));

}

RPG_Graphics_Surface::~RPG_Graphics_Surface()
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Surface::~RPG_Graphics_Surface"));

  // clean up
  if (myOwnSurface)
    SDL_FreeSurface(mySurface);
}

SDL_Surface*
RPG_Graphics_Surface::load(const std::string& filename_in,
                           const bool& convertToDisplayFormat_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Surface::load"));

  // init return value(s)
  SDL_Surface* result = NULL;

  // sanity check
  if (!RPG_Common_File_Tools::isReadable(filename_in))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid argument(\"%s\"): not readable, aborting\n"),
               filename_in.c_str()));

    return NULL;
  } // end IF

  // load complete file into memory
  // *TODO*: this isn't really necessary...
  unsigned char* srcbuf = NULL;
  if (!RPG_Common_File_Tools::loadFile(filename_in,
                                       srcbuf))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Common_File_Tools::loadFile(\"%s\"), aborting\n"),
               filename_in.c_str()));

    return NULL;
  } // end IF

  // *NOTE*: beyond this point:
  // - delete[] srcbuf

  // extract SDL surface from PNG
  result = RPG_Graphics_Surface::loadPNG(srcbuf);
//                                          alpha_in);
  if (!result)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Graphics_Surface::loadPNG(\"%s\"), aborting\n"),
               filename_in.c_str()));

    // clean up
    delete[] srcbuf;

    return NULL;
  } // end IF

  // clean up
  delete[] srcbuf;

  if (convertToDisplayFormat_in)
  {
    SDL_Surface* convert = NULL;

//     // enable "per-surface" alpha blending
//     if (SDL_SetAlpha(result,
//                       (SDL_SRCALPHA | SDL_RLEACCEL), // alpha blending/RLE acceleration
//                       alpha_in))
//     {
//       ACE_DEBUG((LM_ERROR,
//                   ACE_TEXT("failed to SDL_SetAlpha(%u): %s, aborting\n"),
//                   ACE_static_cast(unsigned long, alpha_in),
//                   SDL_GetError()));
    //
//       // clean up
//       SDL_FreeSurface(result);
    //
//       return NULL;
//     } // end IF

    // convert surface to the pixel format/colors of the video framebuffer
    // --> allows fast(er) blitting
//     convert = SDL_DisplayFormat(result);
    convert = SDL_DisplayFormatAlpha(result);
    if (!convert)
    {
      ACE_DEBUG((LM_ERROR,
//                  ACE_TEXT("failed to SDL_DisplayFormat(): %s, aborting\n"),
                 ACE_TEXT("failed to SDL_DisplayFormatAlpha(): %s, aborting\n"),
                 SDL_GetError()));

      // clean up
      SDL_FreeSurface(result);

      return NULL;
    } // end IF

    // clean up
    SDL_FreeSurface(result);
    result = convert;
  } // end IF

  return result;
}

void
RPG_Graphics_Surface::savePNG(const SDL_Surface& surface_in,
                              const std::string& targetFile_in,
                              const bool& alpha_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Surface::savePNG"));

  // sanity check(s)
  if (RPG_Common_File_Tools::isReadable(targetFile_in))
    if (!RPG_Common_File_Tools::deleteFile(targetFile_in))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to RPG_Common_File_Tools::deleteFile(\"%s\"), aborting\n"),
                 targetFile_in.c_str()));

      return;
    } // end IF

  unsigned char* output = NULL;
  try
  {
    output = new unsigned char[(surface_in.w * surface_in.h * (alpha_in ? surface_in.format->BytesPerPixel
                                                                        : (surface_in.format->BytesPerPixel - 1)))];
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to allocate memory(%u): %m, aborting\n"),
               (surface_in.w * surface_in.h * (alpha_in ? surface_in.format->BytesPerPixel
                                                        : (surface_in.format->BytesPerPixel - 1)))));

    return;
  }
  if (!output)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to allocate memory(%u): %m, aborting\n"),
               (surface_in.w * surface_in.h * (alpha_in ? surface_in.format->BytesPerPixel
                                                        : (surface_in.format->BytesPerPixel - 1)))));

    return;
  } // end IF

  png_bytep* image = NULL;
  try
  {
    image = new png_bytep[surface_in.h];
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to allocate memory(%u): %m, aborting\n"),
               (sizeof(png_bytep) * surface_in.h)));

    return;
  }
  if (!image)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to allocate memory(%u): %m, aborting\n"),
               (sizeof(png_bytep) * surface_in.h)));

    return;
  } // end IF

  // lock surface during pixel access
  if (SDL_MUSTLOCK((&surface_in)))
    if (SDL_LockSurface(&ACE_const_cast(SDL_Surface&, surface_in)))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to SDL_LockSurface(): %s, aborting\n"),
                 SDL_GetError()));

      return;
    } // end IF

  // (if neccessary,) strip out alpha bytes and reorder the image bytes to RGB
  Uint32* pixels = ACE_static_cast(Uint32*, surface_in.pixels);
  for (unsigned long j = 0;
       j < ACE_static_cast(unsigned long, surface_in.h);
       j++)
  {
    image[j] = output;

    for (unsigned long i = 0;
         i < ACE_static_cast(unsigned long, surface_in.w);
         i++)
    {
      *output++ = (((*pixels) & surface_in.format->Rmask) >> surface_in.format->Rshift);   /* red   */
      *output++ = (((*pixels) & surface_in.format->Gmask) >> surface_in.format->Gshift);   /* green */
      *output++ = (((*pixels) & surface_in.format->Bmask) >> surface_in.format->Bshift);   /* blue  */
      if (alpha_in)
        *output++ = (((*pixels) & surface_in.format->Amask) >> surface_in.format->Ashift); /* alpha */

      pixels++;
    } // end FOR
  } // end FOR

  if (SDL_MUSTLOCK((&surface_in)))
    SDL_UnlockSurface(&ACE_const_cast(SDL_Surface&, surface_in));

  // open the file
  FILE* fp = NULL;
  fp = ACE_OS::fopen(targetFile_in.c_str(),
                     ACE_TEXT_ALWAYS_CHAR("wb"));
  if (!fp)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to open file(\"%s\"): %m, aborting\n"),
               targetFile_in.c_str()));

    // clean up
    // *NOTE*: output has been modified, but the value is still present in image[0]
    delete[] image[0];
    delete[] image;

    return;
  } // end IF

  // create png image data structures
  png_structp png_ptr = NULL;
  png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, // version
                                    NULL,                  // error
                                    NULL,                  // error handler
                                    NULL);                 // warning handler
  if (!png_ptr)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to png_create_write_struct(): %m, aborting\n")));

    // clean up
    if (ACE_OS::fclose(fp))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to close file(\"%s\"): %m, continuing\n"),
                 targetFile_in.c_str()));
    } // end IF
    // *NOTE*: output has been modified, but the value is still present in image[0]
    delete[] image[0];
    delete[] image;

    return;
  } // end IF

  png_infop info_ptr = NULL;
  info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to png_create_info_struct(): %m, aborting\n")));

    // clean up
    png_destroy_write_struct(&png_ptr,
                             NULL);
    if (ACE_OS::fclose(fp))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to close file(\"%s\"): %m, continuing\n"),
                 targetFile_in.c_str()));
    } // end IF
    // *NOTE*: output has been modified, but the value is still present in image[0]
    delete[] image[0];
    delete[] image;

    return;
  } // end IF

  // save stack context, set up error handling
  if (::setjmp(png_ptr->jmpbuf))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ::setjmp(): %m, aborting\n")));

    // clean up
    png_destroy_write_struct(&png_ptr,
                             &info_ptr);
    if (ACE_OS::fclose(fp))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to close file(\"%s\"): %m, continuing\n"),
                 targetFile_in.c_str()));
    } // end IF
    // *NOTE*: output has been modified, but the value is still present in image[0]
    delete[] image[0];
    delete[] image;

    return;
  } // end IF

  // write the image
  png_init_io(png_ptr, fp);

  png_set_IHDR(png_ptr,                         // context
               info_ptr,                        // header info
               surface_in.w,                    // width
               surface_in.h,                    // height
               8,                               // bit-depth
               (alpha_in ? PNG_COLOR_TYPE_RGBA  // color-type
                         : PNG_COLOR_TYPE_RGB),
               PNG_INTERLACE_NONE,              // interlace
               PNG_COMPRESSION_TYPE_DEFAULT,    // compression
               PNG_FILTER_TYPE_DEFAULT);        // filter

  png_set_rows(png_ptr,
               info_ptr,
               image);

  png_write_png(png_ptr,  // context
                info_ptr, // header info
                0,        // transforms
                NULL);    // params

  // clean up
  png_destroy_write_struct(&png_ptr,
                           &info_ptr);
  if (ACE_OS::fclose(fp))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to close file(\"%s\"): %m, continuing\n"),
               targetFile_in.c_str()));
  } // end IF
  // *NOTE*: output has been modified, but the value is still present in image[0]
  delete[] image[0];
  delete[] image;

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("wrote PNG file \"%s\"...\n"),
             targetFile_in.c_str()));
}

SDL_Surface*
RPG_Graphics_Surface::get(const unsigned long& offsetX_in,
                          const unsigned long& offsetY_in,
                          const unsigned long& width_in,
                          const unsigned long& height_in,
                          const SDL_Surface& image_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Surface::get"));

  // sanity check(s)
  ACE_ASSERT(width_in <= ACE_static_cast(unsigned long, image_in.w));
  ACE_ASSERT((offsetX_in + width_in) <= (ACE_static_cast(unsigned long, image_in.w) - 1));
  ACE_ASSERT(height_in <= ACE_static_cast(unsigned long, image_in.h));
  ACE_ASSERT((offsetY_in + height_in) <= (ACE_static_cast(unsigned long, image_in.h) - 1));

  // init return value
  SDL_Surface* result = NULL;
  result = SDL_CreateRGBSurface((SDL_HWSURFACE | // TRY to (!) place the surface in VideoRAM
                                 SDL_ASYNCBLIT |
                                 SDL_SRCCOLORKEY |
                                 SDL_SRCALPHA),
                                width_in,
                                height_in,
                                image_in.format->BitsPerPixel,
                                image_in.format->Rmask,
                                image_in.format->Gmask,
                                image_in.format->Bmask,
                                image_in.format->Amask);
  if (!result)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_CreateRGBSurface(): %s, aborting\n"),
               SDL_GetError()));

    return NULL;
  } // end IF

  // *NOTE*: blitting does not preserve the alpha channel...

  // lock surface during pixel access
  if (SDL_MUSTLOCK((&image_in)))
    if (SDL_LockSurface(&ACE_const_cast(SDL_Surface&, image_in)))
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
           (ACE_static_cast(unsigned char*, image_in.pixels) + ((offsetY_in + i) * image_in.pitch) + (offsetX_in * 4)),
           (width_in * image_in.format->BytesPerPixel)); // RGBA --> 4 bytes (?!!!)

  if (SDL_MUSTLOCK((&image_in)))
    SDL_UnlockSurface(&ACE_const_cast(SDL_Surface&, image_in));

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
RPG_Graphics_Surface::put(const unsigned long& offsetX_in,
                          const unsigned long& offsetY_in,
                          const SDL_Surface& image_in,
                          SDL_Surface* targetSurface_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Surface::put"));

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
RPG_Graphics_Surface::putText(const RPG_Graphics_Type& type_in,
                              const std::string& textString_in,
                              const SDL_Color& color_in,
                              const bool& shade_in,
                              const SDL_Color& shadeColor_in,
                              const unsigned long& offsetX_in,
                              const unsigned long& offsetY_in,
                              SDL_Surface* targetSurface_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Surface::putText"));

  // render text
  SDL_Surface* rendered_text = NULL;
  if (shade_in)
  {
    rendered_text = RPG_Graphics_Common_Tools::renderText(type_in,
                                                          textString_in,
                                                          shadeColor_in);
    if (!rendered_text)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to RPG_Graphics_Common_Tools::renderText(\"%s\", \"%s\"), aborting\n"),
                 RPG_Graphics_TypeHelper::RPG_Graphics_TypeToString(type_in).c_str(),
                 textString_in.c_str()));

      return;
    } // end IF

    RPG_Graphics_Surface::put(offsetX_in + 1,
                              offsetY_in + 1,
                              *rendered_text,
                              targetSurface_in);

    // clean up
    SDL_FreeSurface(rendered_text);
    rendered_text = NULL;
  } // end IF
  rendered_text = RPG_Graphics_Common_Tools::renderText(type_in,
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

  RPG_Graphics_Surface::put(offsetX_in,
                            offsetY_in,
                            *rendered_text,
                            targetSurface_in);

  // clean up
  SDL_FreeSurface(rendered_text);
}

SDL_Surface*
RPG_Graphics_Surface::shade(const SDL_Surface& sourceImage_in,
                            const Uint8& opacity_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Surface::shade"));

  SDL_Surface* result = NULL;
  result = RPG_Graphics_SDL_Tools::copy(sourceImage_in);
  if (!result)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Graphics_SDL_Tools::copy(), aborting\n")));

    return NULL;
  } // end IF

  // *NOTE*: SDL_SetAlpha() will not work, as transparent pixels should remain that way...
  // --> do it manually
  // lock surface during pixel access
  if (SDL_MUSTLOCK(result))
    if (SDL_LockSurface(result))
    {
      ACE_DEBUG((LM_ERROR,
                ACE_TEXT("failed to SDL_LockSurface(): %s, aborting\n"),
                SDL_GetError()));

      // clean up
      SDL_FreeSurface(result);

      return NULL;
    } // end IF

  Uint32* pixels = ACE_static_cast(Uint32*, result->pixels);
  for (unsigned long j = 0;
       j < ACE_static_cast(unsigned long, result->h);
       j++)
    for (unsigned long i = 0;
         i < ACE_static_cast(unsigned long, result->w);
         i++)
  {
      // ignore transparent pixels
    if (((pixels[(result->w * j) + i] & result->format->Amask) >> result->format->Ashift) == SDL_ALPHA_TRANSPARENT)
      continue;

    pixels[(result->w * j) + i] &= ~ACE_static_cast(Uint32, (SDL_ALPHA_OPAQUE << result->format->Ashift));
    pixels[(result->w * j) + i] |= (ACE_static_cast(Uint32, opacity_in) << result->format->Ashift);
  } // end FOR

  if (SDL_MUSTLOCK(result))
    SDL_UnlockSurface(result);

  return result;
}

SDL_Surface*
RPG_Graphics_Surface::loadPNG(const unsigned char* buffer_in)
//                               const unsigned char& alpha_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Surface::loadPNG"));

  // init return value(s)
  SDL_Surface* result = NULL;

  // sanity check
  //--> buffer must contain a PNG file
  if (png_sig_cmp(ACE_const_cast(unsigned char*, buffer_in), // buffer
                  0,                                         // start at the beginning
                  RPG_GRAPHICS_PNG_SIGNATURE_BYTES))         // #signature bytes to check
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to png_sig_cmp(): %m, aborting\n")));

    return NULL;
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

    return NULL;
  } // end IF

  // *NOTE* beyond this point:
  // - cleanup "png_ptr"

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

    return NULL;
  } // end IF

  // *NOTE* beyond this point:
  // - cleanup "png_ptr"
  // - cleanup "info_ptr"

  // save stack context, set up error handling
  if (::setjmp(png_ptr->jmpbuf))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ::setjmp(): %m, aborting\n")));

    // clean up
    png_destroy_read_struct(&png_ptr,
                            &info_ptr,
                            NULL);

    return NULL;
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
             ACE_TEXT("loading PNG [w,h,d,#c,t,i,c,f]: %u,%u,%d,%u,%d,%d,%d,%d...\n"),
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

//   // *NOTE*: we're using per-surface alpha only
//   // --> strip any alpha channel
//   png_set_strip_alpha(png_ptr);

  // add an (opaque) alpha channel to anything that doesn't have one yet
  // --> add a filler byte to 8-bit Gray or 24-bit RGB images
  png_set_filler(png_ptr,
                 0xFF,
//                  alpha_in,
                 PNG_FILLER_AFTER);
  png_set_add_alpha(png_ptr,
                    0xFF,
//                     alpha_in,
                    PNG_FILLER_AFTER);
  info_ptr->color_type |= PNG_COLOR_MASK_ALPHA;

  // allocate surface
  // *NOTE*:
  // - always TRY to (!) place the surface in VideoRAM; this may not work (check flags after loading !)
  // - conversion between PNG header info <--> SDL argument formats requires some casts
  result = SDL_CreateRGBSurface((SDL_HWSURFACE | // TRY to (!) place the surface in VideoRAM
                                 SDL_ASYNCBLIT |
                                 SDL_SRCCOLORKEY |
                                 SDL_SRCALPHA),
                                ACE_static_cast(int, width),
                                ACE_static_cast(int, height),
//                                 (bit_depth * 8),
                                32,
                                ((SDL_BYTEORDER == SDL_LIL_ENDIAN) ? 0x000000FF : 0xFF000000),
                                ((SDL_BYTEORDER == SDL_LIL_ENDIAN) ? 0x0000FF00 : 0x00FF0000),
                                ((SDL_BYTEORDER == SDL_LIL_ENDIAN) ? 0x00FF0000 : 0x0000FF00),
                                ((SDL_BYTEORDER == SDL_LIL_ENDIAN) ? 0xFF000000 : 0x000000FF));
//                                 Rmask,
//                                 Gmask,
//                                 Bmask,
//                                 Amask);
  if (!result)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_CreateRGBSurface(): %s, aborting\n"),
               SDL_GetError()));

    // clean up
    png_destroy_read_struct(&png_ptr,
                            &info_ptr,
                            NULL);

    return NULL;
  } // end IF

  // *NOTE* beyond this point:
  // - cleanup "png_ptr"
  // - cleanup "info_ptr"
  // - SDL_FreeSurface() result

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
    SDL_FreeSurface(result);

    return NULL;
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
    SDL_FreeSurface(result);

    return NULL;
  } // end IF

  // *NOTE* beyond this point:
  // - cleanup "png_ptr"
  // - cleanup "info_ptr"
  // - SDL_FreeSurface() result
  // - delete[] row_pointers

  // lock surface during pixel access
  if (SDL_MUSTLOCK(result))
    if (SDL_LockSurface(result))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to SDL_LockSurface(): %s, aborting\n"),
                 SDL_GetError()));

      // clean up
      png_destroy_read_struct(&png_ptr,
                              &info_ptr,
                              NULL);
      SDL_FreeSurface(result);
      delete[] row_pointers;

      return NULL;
    } // end IF

  for (unsigned long row = 0;
       row < height;
       row++)
    row_pointers[row] = ACE_static_cast(png_bytep,
                                        ACE_static_cast(Uint8*,
                                                        result->pixels) + (row * result->pitch));

  // read the image from the memory buffer onto the surface buffer
  // --> read the whole image into memory at once
  png_read_image(png_ptr, row_pointers);

  // unlock surface
  if (SDL_MUSTLOCK(result))
    SDL_UnlockSurface(result);

  // clean up
  png_destroy_read_struct(&png_ptr,
                          &info_ptr,
                          NULL);
  delete[] row_pointers;

  return result;
}
