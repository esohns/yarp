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
#include "stdafx.h"

#include "rpg_graphics_surface.h"

#include "ace/OS.h"
#include "ace/ACE.h"
#include "ace/Log_Msg.h"

#include "png.h"

#include "common_file_tools.h"

#include "rpg_common_macros.h"

#include "rpg_graphics_defines.h"
#include "rpg_graphics_common_tools.h"
#include "rpg_graphics_SDL_tools.h"

// init static(s)
Uint32   RPG_Graphics_Surface::SDL_surface_flags = 0;
SDL_Rect RPG_Graphics_Surface::myClipRectangle = {0, 0, 0, 0};

RPG_Graphics_Surface::RPG_Graphics_Surface ()
 : mySurface (NULL),
//    myType({{RPG_GRAPHICS_CURSOR_INVALID}, RPG_Graphics_GraphicTypeUnion::INVALID}),
   myOwnSurface (false)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Surface::RPG_Graphics_Surface"));

  myType.cursor = RPG_GRAPHICS_CURSOR_INVALID;
  myType.discriminator = RPG_Graphics_GraphicTypeUnion::INVALID;
}

RPG_Graphics_Surface::RPG_Graphics_Surface (const RPG_Graphics_Surface& surface_in)
 : mySurface (surface_in.mySurface),
   myType (surface_in.myType),
   myOwnSurface (surface_in.myOwnSurface)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Surface::RPG_Graphics_Surface"));

  // "steal" ownership
  if (myOwnSurface)
    const_cast<RPG_Graphics_Surface&> (surface_in).myOwnSurface = false;
}

RPG_Graphics_Surface::RPG_Graphics_Surface (const RPG_Graphics_GraphicTypeUnion& type_in,
                                            bool ownSurface_in)
 : mySurface (NULL),
//    myType({{RPG_GRAPHICS_CURSOR_INVALID}, RPG_Graphics_GraphicTypeUnion::INVALID}),
   myOwnSurface (false)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Surface::RPG_Graphics_Surface"));

  myType.cursor = RPG_GRAPHICS_CURSOR_INVALID;
  myType.discriminator = RPG_Graphics_GraphicTypeUnion::INVALID;

  // (try to) load graphic
  mySurface = RPG_Graphics_Common_Tools::loadGraphic (type_in,         // graphic
                                                      true,            // convert to display format
                                                      !ownSurface_in); // cache graphic ?
  if (!mySurface)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to RPG_Graphics_Common_Tools::loadGraphic(%s), aborting\n"),
                ACE_TEXT (RPG_Graphics_Common_Tools::toString (type_in).c_str ())));
    return;
  } // end IF

  myOwnSurface = ownSurface_in;
}

RPG_Graphics_Surface::RPG_Graphics_Surface (SDL_Surface* surface_in,
                                            bool ownSurface_in)
 : mySurface (surface_in),
//    myType({{RPG_GRAPHICS_CURSOR_INVALID}, RPG_Graphics_GraphicTypeUnion::INVALID}),
   myOwnSurface (ownSurface_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Surface::RPG_Graphics_Surface"));

  myType.cursor = RPG_GRAPHICS_CURSOR_INVALID;
  myType.discriminator = RPG_Graphics_GraphicTypeUnion::INVALID;
}

RPG_Graphics_Surface::~RPG_Graphics_Surface()
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Surface::~RPG_Graphics_Surface"));

  // clean up
  if (myOwnSurface &&
      mySurface)
    SDL_FreeSurface (mySurface);
}

RPG_Graphics_GraphicTypeUnion
RPG_Graphics_Surface::type () const
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Surface::type"));

  return myType;
}

SDL_Surface*
RPG_Graphics_Surface::surface () const
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Surface::surface"));

  return mySurface;
}

void
RPG_Graphics_Surface::init (const RPG_Graphics_GraphicTypeUnion& type_in,
                            bool ownSurface_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Surface::init"));

  // clean up
  if (mySurface)
  {
    if (myOwnSurface)
    {
      SDL_FreeSurface (mySurface);
      myOwnSurface = false;
    } // end IF
    mySurface = NULL;
  } // end IF

  // (try to) load graphic
  mySurface = RPG_Graphics_Common_Tools::loadGraphic (type_in,         // graphic
                                                      true,            // convert to display format
                                                      !ownSurface_in); // cache graphic ?
  if (!mySurface)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to RPG_Graphics_Common_Tools::loadGraphic(%s), aborting\n"),
                ACE_TEXT (RPG_Graphics_Common_Tools::toString (type_in).c_str ())));
    return;
  } // end IF

  myType = type_in;
  myOwnSurface = ownSurface_in;
}

void
RPG_Graphics_Surface::init (SDL_Surface* surface_in,
                            bool ownSurface_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Surface::init"));

  // sanity check(s)
  ACE_ASSERT(surface_in);

  // clean up
  if (mySurface)
  {
    if (myOwnSurface)
    {
      SDL_FreeSurface(mySurface);
      myOwnSurface = false;
    } // end IF
    mySurface = NULL;
  } // end IF

  mySurface = surface_in;
  myType.cursor = RPG_GRAPHICS_CURSOR_INVALID;
  myType.discriminator = RPG_Graphics_GraphicTypeUnion::INVALID;
  myOwnSurface = ownSurface_in;
}

void
#if defined (SDL_USE)
RPG_Graphics_Surface::clip ()
#elif defined (SDL2_USE)
RPG_Graphics_Surface::clip (SDL_Window* window_in)
#endif // SDL_USE || SDL2_USE
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Surface::clip"));

  // sanity check(s)
#if defined (SDL_USE)
  SDL_Surface* surface_p = SDL_GetVideoSurface ();
#elif defined (SDL2_USE)
  SDL_Surface* surface_p = SDL_GetWindowSurface (window_in);
#endif // SDL_USE || SDL2_USE
  ACE_ASSERT (surface_p);
  if (((myClipRectangle.w == 0) ||
       (myClipRectangle.h == 0)) ||
      !surface_p)
    return; // nothing to do

  SDL_SetClipRect (surface_p,
                   &myClipRectangle);
}

void
#if defined (SDL_USE)
RPG_Graphics_Surface::unclip ()
#elif defined (SDL2_USE)
RPG_Graphics_Surface::unclip (SDL_Window* window_in)
#endif // SDL_USE || SDL2_USE
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Surface::unclip"));

  // sanity check(s)
#if defined (SDL_USE)
  SDL_Surface* surface_p = SDL_GetVideoSurface ();
#elif defined (SDL2_USE)
  SDL_Surface* surface_p = SDL_GetWindowSurface (window_in);
#endif // SDL_USE || SDL2_USE
  ACE_ASSERT (surface_p);
  if (!surface_p)
    return; // nothing to do

  // cache previous entry
  SDL_GetClipRect (surface_p,
                   &myClipRectangle);

  SDL_SetClipRect (surface_p, NULL);
}

SDL_Surface*
RPG_Graphics_Surface::load (const std::string& filename_in,
                            bool convertToDisplayFormat_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Surface::load"));

  // init return value(s)
  SDL_Surface* result = NULL;

  // sanity check
  if (!Common_File_Tools::isReadable (filename_in))
  {
    ACE_DEBUG ((LM_ERROR,
               ACE_TEXT ("invalid argument(\"%s\"): not readable, aborting\n"),
               ACE_TEXT (filename_in.c_str ())));

    return NULL;
  } // end IF

  // *TODO*: there seems to be a bug in WIN32 ACE_OS::fopen()
  //FILE* file_ptr = ACE_OS::fopen (filename_in.c_str (),         // filename
  FILE* file_ptr = ::fopen (filename_in.c_str (),         // filename
                            ACE_TEXT_ALWAYS_CHAR ("rb")); // mode
  if (file_ptr == NULL)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::fopen(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (filename_in.c_str ())));
    return NULL;
  } // end IF

  // extract SDL surface from PNG
  result =
    RPG_Graphics_Surface::loadPNG (ACE_TEXT_ALWAYS_CHAR (ACE::basename (filename_in.c_str ())),
                                   file_ptr);
  if (!result)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to RPG_Graphics_Surface::loadPNG(\"%s\"), aborting\n"),
                ACE_TEXT (filename_in.c_str ())));

    // clean up
  if (ACE_OS::fclose (file_ptr))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::fclose(\"%s\"): \"%m\", continuing\n"),
                ACE_TEXT (filename_in.c_str ())));
    //delete[] srcbuf;

    return NULL;
  } // end IF

  // clean up
  if (ACE_OS::fclose (file_ptr))
  ACE_DEBUG ((LM_ERROR,
              ACE_TEXT ("failed to ACE_OS::fclose(\"%s\"): \"%m\", continuing\n"),
              ACE_TEXT (filename_in.c_str ())));
  //delete[] srcbuf;

  if (convertToDisplayFormat_in)
  {
    SDL_Surface* convert = NULL;

#if defined (SDL_USE)
    // convert surface to the pixel format/colors of the video framebuffer
    // --> allows fast(er) blitting
//     convert = SDL_DisplayFormat(result);
    convert = SDL_DisplayFormatAlpha (result);
    if (!convert)
    {
      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT("failed to SDL_DisplayFormat(): %s, aborting\n"),
                  ACE_TEXT ("failed to SDL_DisplayFormatAlpha(): \"%s\", aborting\n"),
                  ACE_TEXT (SDL_GetError ())));
      SDL_FreeSurface (result);
      return NULL;
    } // end IF
#elif defined (SDL2_USE)
    convert = SDL_ConvertSurfaceFormat (result,
                                        SDL_PIXELFORMAT_RGBA8888,
                                        0);
    if (!convert)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to SDL_ConvertSurfaceFormat(): \"%s\", aborting\n"),
                  ACE_TEXT (SDL_GetError ())));
      SDL_FreeSurface (result);
      return NULL;
    } // end IF
#endif // SDL_USE || SDL2_USE

    // clean up
    SDL_FreeSurface (result);
    result = convert;
  } // end IF

  return result;
}

void
RPG_Graphics_Surface::savePNG (const SDL_Surface& surface_in,
                               const std::string& targetFile_in,
                               bool alpha_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Surface::savePNG"));

  // sanity check(s)
  if (Common_File_Tools::isReadable (targetFile_in))
    if (!Common_File_Tools::deleteFile (targetFile_in))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to RPG_Common_File_Tools::deleteFile(\"%s\"), returning\n"),
                  ACE_TEXT (targetFile_in.c_str ())));
      return;
    } // end IF
  ACE_ASSERT (surface_in.format->BytesPerPixel == 4);

  unsigned char* output = NULL;
  try {
    output =
        new unsigned char[(surface_in.w * surface_in.h * (alpha_in ? surface_in.format->BytesPerPixel
                                                                   : (surface_in.format->BytesPerPixel - 1)))];
  } catch (...) {
    output = NULL;
  }
  if (!output)
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory(%u): \"%m\", returning\n"),
                (surface_in.w * surface_in.h * (alpha_in ? surface_in.format->BytesPerPixel
                                                         : (surface_in.format->BytesPerPixel - 1)))));

    return;
  } // end IF

  png_bytep* image = NULL;
  try {
    image = new png_bytep[surface_in.h];
  } catch (...) {
    image = NULL;
  }
  if (!image)
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory(%u): \"%m\", returning\n"),
                (sizeof(png_bytep) * surface_in.h)));

    return;
  } // end IF

  // lock surface during pixel access
  if (SDL_MUSTLOCK ((&surface_in)))
    if (SDL_LockSurface (&const_cast<SDL_Surface&> (surface_in)))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to SDL_LockSurface(): \"%s\", returning\n"),
                  ACE_TEXT (SDL_GetError ())));

      return;
    } // end IF

  // (if neccessary,) strip out alpha bytes and reorder the image bytes to RGB
  Uint32* pixels = static_cast<Uint32*> (surface_in.pixels);
  for (unsigned int j = 0;
       j < static_cast<unsigned int> (surface_in.h);
       j++)
  {
    image[j] = output;

    for (unsigned int i = 0;
         i < static_cast<unsigned int> (surface_in.w);
         i++)
    {
      *output++ =
          (((*pixels) & surface_in.format->Rmask) >> surface_in.format->Rshift); /* red   */
      *output++ =
          (((*pixels) & surface_in.format->Gmask) >> surface_in.format->Gshift); /* green */
      *output++ =
          (((*pixels) & surface_in.format->Bmask) >> surface_in.format->Bshift); /* blue  */
      if (alpha_in)
        *output++ =
          (((*pixels) & surface_in.format->Amask) >> surface_in.format->Ashift); /* alpha */

      pixels++;
    } // end FOR
  } // end FOR

  if (SDL_MUSTLOCK ((&surface_in)))
    SDL_UnlockSurface (&const_cast<SDL_Surface&> (surface_in));

  // open the file
  FILE* fp = ::fopen (ACE_TEXT_ALWAYS_CHAR (targetFile_in.c_str ()),
                      ACE_TEXT_ALWAYS_CHAR ("wb"));
  if (!fp)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to open file(\"%s\"): \"%m\", returning\n"),
                ACE_TEXT (targetFile_in.c_str ())));

    // clean up
    // *NOTE*: output has been modified, but the value is still present in image[0]
    delete[] image[0];
    delete[] image;

    return;
  } // end IF

  // create png image data structures
  png_structp png_ptr = NULL;
  png_ptr = png_create_write_struct (PNG_LIBPNG_VER_STRING, // version
                                     NULL,                  // error
                                     NULL,                  // error handler
                                     NULL);                 // warning handler
  if (!png_ptr)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to png_create_write_struct(): \"%m\", returning\n")));

    // clean up
    if (ACE_OS::fclose (fp))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to close file(\"%s\"): \"%m\", continuing\n"),
                  ACE_TEXT (targetFile_in.c_str ())));
    // *NOTE*: output has been modified, but the value is still present in image[0]
    delete[] image[0];
    delete[] image;

    return;
  } // end IF

  png_infop info_ptr = NULL;
  info_ptr = png_create_info_struct (png_ptr);
  if (!info_ptr)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to png_create_info_struct(): \"%m\", returning\n")));

    // clean up
    png_destroy_write_struct (&png_ptr,
                              NULL);
    if (ACE_OS::fclose (fp))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to close file(\"%s\"): \"%m\", continuing\n"),
                  ACE_TEXT (targetFile_in.c_str ())));
    // *NOTE*: output has been modified, but the value is still present in image[0]
    delete[] image[0];
    delete[] image;

    return;
  } // end IF

  // save stack context, set up error handling
  if (::setjmp (png_jmpbuf (png_ptr)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::setjmp(): \"%m\", aborting\n")));

    // clean up
    png_destroy_write_struct (&png_ptr,
                              &info_ptr);
    if (ACE_OS::fclose (fp))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to close file(\"%s\"): \"%m\", continuing\n"),
                  ACE_TEXT (targetFile_in.c_str ())));
    // *NOTE*: output has been modified, but the value is still present in image[0]
    delete[] image[0];
    delete[] image;

    return;
  } // end IF

  // write the image
  png_init_io (png_ptr, fp);

  png_set_IHDR (png_ptr,                         // context
                info_ptr,                        // header info
                surface_in.w,                    // width
                surface_in.h,                    // height
                8,                               // bit-depth
                (alpha_in ? PNG_COLOR_TYPE_RGBA  // color-type
                          : PNG_COLOR_TYPE_RGB),
                PNG_INTERLACE_NONE,              // interlace
                PNG_COMPRESSION_TYPE_DEFAULT,    // compression
                PNG_FILTER_TYPE_DEFAULT);        // filter

  png_set_rows (png_ptr,
                info_ptr,
                image);

  png_write_png (png_ptr,  // context
                 info_ptr, // header info
                 0,        // transforms
                 NULL);    // params

  // clean up
  png_destroy_write_struct (&png_ptr,
                            &info_ptr);
  if (ACE_OS::fclose (fp))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to close file(\"%s\"): \"%m\", continuing\n"),
                ACE_TEXT (targetFile_in.c_str ())));
  // *NOTE*: output has been modified, but the value is still present in image[0]
  delete[] image[0];
  delete[] image;

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("wrote PNG file \"%s\"...\n"),
              ACE_TEXT (targetFile_in.c_str ())));
}

SDL_Surface*
RPG_Graphics_Surface::create (unsigned int width_in,
                              unsigned int height_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Surface::create"));

  SDL_Surface* result = NULL;
  // *NOTE*: surface is initialized as transparent
  result =
    SDL_CreateRGBSurface (RPG_Graphics_Surface::SDL_surface_flags,
                          static_cast<int> (width_in),
                          static_cast<int> (height_in),
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
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_CreateRGBSurface(): %s, aborting\n"),
                ACE_TEXT (SDL_GetError ())));
    return NULL;
  } // end IF

  return result;
}

SDL_Surface*
RPG_Graphics_Surface::get (const RPG_Graphics_Offset_t& offset_in,
                           unsigned int width_in,
                           unsigned int height_in,
                           const SDL_Surface& source_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Surface::get"));

  // sanity check(s)
  ACE_ASSERT (width_in <= static_cast<unsigned int>(source_in.w));
//   ACE_ASSERT((offset_in.first + width_in) <= (static_cast<unsigned int>(image_in.w) - 1));
  ACE_ASSERT (height_in <= static_cast<unsigned int>(source_in.h));
//   ACE_ASSERT((offset_in.second + height_in) <= (static_cast<unsigned int>(image_in.h) - 1));
  // clip where necessary...
  unsigned int clipped_width, clipped_height;
  clipped_width = (source_in.w - offset_in.first); // available width
  clipped_width = ((clipped_width > width_in) ? width_in : clipped_width);
  clipped_height = (source_in.h - offset_in.second); // available height
  clipped_height = ((clipped_height > height_in) ? height_in : clipped_height);

  // init return value
  SDL_Surface* result = NULL;
  result = SDL_CreateRGBSurface (RPG_Graphics_Surface::SDL_surface_flags,
                                 width_in,
                                 height_in,
                                 source_in.format->BitsPerPixel,
                                 source_in.format->Rmask,
                                 source_in.format->Gmask,
                                 source_in.format->Bmask,
                                 source_in.format->Amask);
  if (!result)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_CreateRGBSurface(): \"%s\", aborting\n"),
                ACE_TEXT (SDL_GetError ())));
    return NULL;
  } // end IF

  // *NOTE*: blitting does not preserve the alpha channel...

  // lock surface during pixel access
  if (SDL_MUSTLOCK ((&source_in)))
    if (SDL_LockSurface (&const_cast<SDL_Surface&>(source_in)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_LockSurface(): \"%s\", aborting\n"),
                ACE_TEXT (SDL_GetError ())));

    // clean up
    SDL_FreeSurface (result);

    return NULL;
  } // end IF

  for (unsigned int i = 0;
       i < clipped_height;
       i++)
  ::memcpy ((static_cast<unsigned char*>(result->pixels) + (result->pitch * i)),
            (static_cast<unsigned char*>(source_in.pixels) +
             ((offset_in.second + i) * source_in.pitch) + (offset_in.first * 4)),
            (clipped_width * result->format->BytesPerPixel)); // RGBA --> 4 bytes

  if (SDL_MUSTLOCK ((&source_in)))
    SDL_UnlockSurface (&const_cast<SDL_Surface&>(source_in));

  return result;
}

void
RPG_Graphics_Surface::get (const RPG_Graphics_Offset_t& offset_in,
                           bool blit_in,
                           const SDL_Surface& source_in,
                           SDL_Surface& target_inout)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Surface::get"));

  // sanity check(s)
  ACE_ASSERT (target_inout.w <= source_in.w);
  ACE_ASSERT (target_inout.h <= source_in.h);

  // clip where necessary...
  unsigned int clipped_width, clipped_height;
  clipped_width = (source_in.w - offset_in.first); // available width
  clipped_width =
      ((clipped_width > static_cast<unsigned int>(target_inout.w)) ? target_inout.w
                                                                   : clipped_width);
  clipped_height = (source_in.h - offset_in.second); // available height
  clipped_height =
      ((clipped_height > static_cast<unsigned int>(target_inout.h)) ? target_inout.h
                                                                    : clipped_height);

  // *WARNING*: blitting does not preserve the alpha channel...
  if (blit_in)
  {
    // bounding box
    SDL_Rect clip_rectangle;
    clip_rectangle.x = static_cast<int16_t>(offset_in.first);
    clip_rectangle.y = static_cast<int16_t>(offset_in.second);
    clip_rectangle.w = static_cast<uint16_t>(clipped_width);
    clip_rectangle.h = static_cast<uint16_t>(clipped_height);
    if (SDL_BlitSurface (const_cast<SDL_Surface*>(&source_in), // source
                         &clip_rectangle,                      // aspect
                         &target_inout,                        // target
                         NULL))                                // aspect (--> everything)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to SDL_BlitSurface(): \"%s\", returning\n"),
                  ACE_TEXT (SDL_GetError ())));
      return;
    } // end IF

    return;
  } // end IF

  // lock surfaces during pixel access
  if (SDL_MUSTLOCK ((&source_in)))
    if (SDL_LockSurface (&const_cast<SDL_Surface&>(source_in)))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to SDL_LockSurface(): \"%s\", returning\n"),
                  ACE_TEXT (SDL_GetError())));
      return;
    } // end IF
  if (SDL_MUSTLOCK ((&target_inout)))
    if (SDL_LockSurface (&const_cast<SDL_Surface&>(target_inout)))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to SDL_LockSurface(): \"%s\", aborting\n"),
                  ACE_TEXT (SDL_GetError ())));

      // clean up
      if (SDL_MUSTLOCK ((&source_in)))
        SDL_UnlockSurface (&const_cast<SDL_Surface&>(source_in));

      return;
    } // end IF

  for (unsigned int i = 0;
       i < clipped_height;
       i++)
    ::memcpy ((static_cast<unsigned char*>(target_inout.pixels) +
               (target_inout.pitch * i)),
              (static_cast<unsigned char*>(source_in.pixels) +
               ((offset_in.second + i) * source_in.pitch) + (offset_in.first * 4)),
              (clipped_width * target_inout.format->BytesPerPixel)); // RGBA --> 4 bytes

  if (SDL_MUSTLOCK ((&source_in)))
    SDL_UnlockSurface (&const_cast<SDL_Surface&>(source_in));
  if (SDL_MUSTLOCK ((&target_inout)))
    SDL_UnlockSurface (&const_cast<SDL_Surface&>(target_inout));
}

void
RPG_Graphics_Surface::put (const RPG_Graphics_Offset_t& offset_in,
                           const SDL_Surface& image_in,
                           SDL_Surface* targetSurface_in,
                           struct SDL_Rect& dirtyRegion_out)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Surface::put"));

  // initialize return value(s)
  ACE_OS::memset (&dirtyRegion_out, 0, sizeof (struct SDL_Rect));

  // sanity check(s)
  ACE_ASSERT (targetSurface_in);

  // clipping
  SDL_Rect target_rectangle;
  target_rectangle.x = static_cast<Sint16>(offset_in.first);
  target_rectangle.y = static_cast<Sint16>(offset_in.second);
  target_rectangle.w = 0; // *NOTE*: ignored
  target_rectangle.h = 0; // *NOTE*: ignored
  if (SDL_BlitSurface (&const_cast<SDL_Surface&> (image_in), // source
                       NULL,                                 // aspect (--> everything)
                       targetSurface_in,                     // target
                       &target_rectangle))                   // aspect
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_BlitSurface(): \"%s\", returning\n"),
                ACE_TEXT (SDL_GetError ())));
    return;
  } // end IF

  // compute dirty region
  dirtyRegion_out = target_rectangle;
}

bool
RPG_Graphics_Surface::putText (enum RPG_Graphics_Font font_in,
                               const std::string& textString_in,
                               const struct SDL_Color& color_in,
                               bool shade_in,
                               const struct SDL_Color& shadeColor_in,
                               const RPG_Graphics_Offset_t& offset_in,
                               SDL_Surface* targetSurface_in,
                               struct SDL_Rect& dirtyRegion_out)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Surface::putText"));

  // initialize return value(s)
  ACE_OS::memset (&dirtyRegion_out, 0, sizeof (struct SDL_Rect));

  // sanity check(s)
  ACE_ASSERT (targetSurface_in);

  SDL_Surface* rendered_text = NULL;
  SDL_Rect dirty_region;
  // step1: render shade ?
  if (shade_in)
  {
    rendered_text = RPG_Graphics_Common_Tools::renderText (font_in,
                                                           textString_in,
                                                           shadeColor_in);
    if (!rendered_text)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to RPG_Graphics_Common_Tools::renderText(\"%s\", \"%s\"), aborting\n"),
                  ACE_TEXT (RPG_Graphics_FontHelper::RPG_Graphics_FontToString (font_in).c_str ()),
                  ACE_TEXT (textString_in.c_str ())));
      return false;
    } // end IF

    RPG_Graphics_Offset_t offset = offset_in;
    offset.first++;
    offset.second++;
    RPG_Graphics_Surface::put (offset,
                               *rendered_text,
                               targetSurface_in,
                               dirty_region);
    dirtyRegion_out = dirty_region;

    // clean up
    SDL_FreeSurface (rendered_text);
    rendered_text = NULL;
  } // end IF

  // step2: render text
  rendered_text = RPG_Graphics_Common_Tools::renderText (font_in,
                                                         textString_in,
                                                         color_in);
  if (!rendered_text)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to RPG_Graphics_Common_Tools::renderText(\"%s\", \"%s\"), aborting\n"),
                ACE_TEXT (RPG_Graphics_FontHelper::RPG_Graphics_FontToString (font_in).c_str ()),
                ACE_TEXT (textString_in.c_str ())));
    return false;
  } // end IF
  RPG_Graphics_Surface::put (offset_in,
                             *rendered_text,
                             targetSurface_in,
                             dirty_region);
  dirtyRegion_out = RPG_Graphics_SDL_Tools::boundingBox (dirty_region,
                                                         dirtyRegion_out);

  // clean up
  SDL_FreeSurface (rendered_text);

  return true;
}

void
RPG_Graphics_Surface::putRectangle (const SDL_Rect& rectangle_in,
                                    Uint32 color_in,
                                    SDL_Surface* targetSurface_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Surface::putRectangle"));

  // sanity check(s)
  ACE_ASSERT (targetSurface_in);
  SDL_Rect intersection;
  ACE_OS::memset(&intersection, 0, sizeof (SDL_Rect));
  intersection = RPG_Graphics_SDL_Tools::intersect (rectangle_in,
                                                    targetSurface_in->clip_rect);
  if ((intersection.w == 0) || (intersection.h == 0))
    return; // rectangle is COMPLETELY outside of target surface...

  // lock surface during pixel access
  if (SDL_MUSTLOCK ((targetSurface_in)))
    if (SDL_LockSurface (targetSurface_in))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_LockSurface(): \"%s\", returning\n"),
                ACE_TEXT (SDL_GetError ())));
    return;
  } // end IF

  Uint32* pixels = static_cast<Uint32*>(targetSurface_in->pixels);
  for (int y = intersection.y;
       y < (intersection.y + intersection.h);
       y++)
  {
    // sanity check
    if (y < 0)
      continue;
    else if (y >= targetSurface_in->h)
      break;

    // top/bottom row(s)
    if ((y == intersection.y) ||
        (y == (intersection.y + intersection.h - 1)))
    {
      for (int x = intersection.x;
           x < (intersection.x + intersection.w);
           x++)
      {
        // sanity check
        if (x < 0)
          continue;
        else if (x >= targetSurface_in->w)
          break;

        pixels[(targetSurface_in->w * y) + x] = color_in;
      } // end FOR

      continue;
    } // end IF

    // left/right column(s)
    if (intersection.x > 0) // sanity check
      pixels[(targetSurface_in->w * y) + intersection.x] = color_in;

    // sanity check
    if ((intersection.x + intersection.w) >= targetSurface_in->w)
      continue;

    pixels[(targetSurface_in->w * y) + intersection.x + (intersection.w - 1)] = color_in;
  } // end FOR

  if (SDL_MUSTLOCK (targetSurface_in))
    SDL_UnlockSurface (targetSurface_in);
}

void
RPG_Graphics_Surface::alpha (Uint8 opacity_in,
                             SDL_Surface& targetImage_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Surface::alpha"));

  // *NOTE*: SDL_SetAlpha() will not work, as transparent pixels should remain
  // that way --> do it manually

  // lock surface during pixel access
  if (SDL_MUSTLOCK ((&targetImage_in)))
    if (SDL_LockSurface (&targetImage_in))
    {
      ACE_DEBUG ((LM_ERROR,
                 ACE_TEXT ("failed to SDL_LockSurface(): \"%s\", aborting\n"),
                 ACE_TEXT (SDL_GetError ())));
      return;
    } // end IF

  Uint32* pixels = static_cast<Uint32*> (targetImage_in.pixels);
  for (unsigned int j = 0;
       j < static_cast<unsigned int> (targetImage_in.h);
       j++)
    for (unsigned int i = 0;
         i < static_cast<unsigned int> (targetImage_in.w);
         i++)
  {
    // ignore transparent pixels
    if (((pixels[(targetImage_in.w * j) + i] & targetImage_in.format->Amask) >> targetImage_in.format->Ashift) == SDL_ALPHA_TRANSPARENT)
      continue;

    pixels[(targetImage_in.w * j) + i] &= ~static_cast<Uint32> ((SDL_ALPHA_OPAQUE << targetImage_in.format->Ashift));
    pixels[(targetImage_in.w * j) + i] |= (static_cast<Uint32> (opacity_in) << targetImage_in.format->Ashift);
  } // end FOR

  if (SDL_MUSTLOCK ((&targetImage_in)))
    SDL_UnlockSurface (&targetImage_in);
}

SDL_Surface*
RPG_Graphics_Surface::alpha (const SDL_Surface& sourceImage_in,
                             Uint8 opacity_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Surface::alpha"));

  SDL_Surface* result = NULL;
  result = RPG_Graphics_Surface::copy (sourceImage_in);
  if (!result)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to RPG_Graphics_Surface::copy(), aborting\n")));
    return NULL;
  } // end IF

  // *NOTE*: SDL_SetAlpha() will not work, as transparent pixels should remain
  // that way --> do it manually

  // lock surface during pixel access
  if (SDL_MUSTLOCK (result))
    if (SDL_LockSurface (result))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to SDL_LockSurface(): \"%s\", aborting\n"),
                  ACE_TEXT (SDL_GetError ())));
      SDL_FreeSurface (result);
      return NULL;
    } // end IF

  Uint32* pixels = static_cast<Uint32*> (result->pixels);
  for (unsigned int j = 0;
       j < static_cast<unsigned int> (result->h);
       j++)
    for (unsigned int i = 0;
         i < static_cast<unsigned int> (result->w);
         i++)
  {
    // ignore transparent pixels
    if (((pixels[(result->w * j) + i] & result->format->Amask) >> result->format->Ashift) == SDL_ALPHA_TRANSPARENT)
      continue;

    pixels[(result->w * j) + i] &= ~static_cast<Uint32> ((SDL_ALPHA_OPAQUE << result->format->Ashift));
    pixels[(result->w * j) + i] |= (static_cast<Uint32> (opacity_in) << result->format->Ashift);
  } // end FOR

  if (SDL_MUSTLOCK (result))
    SDL_UnlockSurface (result);

  return result;
}

void
RPG_Graphics_Surface::clear (SDL_Surface* targetSurface_in,
                             const struct SDL_Rect* clipRectangle_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Surface::clear"));

  ACE_ASSERT (targetSurface_in);

  RPG_Graphics_Surface::fill (RPG_Graphics_SDL_Tools::getColor (COLOR_BLACK_A0, // transparent
                                                                *targetSurface_in->format,
                                                                1.0f),
                              targetSurface_in,
                              clipRectangle_in);
}

void
RPG_Graphics_Surface::fill (Uint32 color_in,
                            SDL_Surface* targetSurface_in,
                            const struct SDL_Rect* clipRectangle_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Surface::fill"));

  // sanity check
  ACE_ASSERT (targetSurface_in);

  // fill with color
  if (SDL_FillRect (targetSurface_in,                                // target
                    const_cast<struct SDL_Rect*> (clipRectangle_in), // aspect
                    color_in))                                       // color
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_FillRect(): \"%s\", continuing\n"),
                ACE_TEXT (SDL_GetError ())));
}

void
RPG_Graphics_Surface::copy (const SDL_Surface& sourceImage_in,
                            SDL_Surface& targetImage_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Surface::copy"));

  // sanity check(s)
  ACE_ASSERT ((sourceImage_in.w == targetImage_in.w) && (sourceImage_in.h == targetImage_in.h));
  //if ((sourceImage_in.w != targetImage_in.w) ||
  //    (sourceImage_in.h != targetImage_in.h))
  //{
  //  ACE_DEBUG ((LM_ERROR,
  //              ACE_TEXT ("invalid image format: [%d,%d] != [%d,%d], aborting\n"),
  //              sourceImage_in.w, sourceImage_in.h,
  //              targetImage_in.w, targetImage_in.h));
  //  return;
  //} // end IF

  // *NOTE*: blitting does not preserve the alpha channel...
  // --> do it manually
//   if (SDL_BlitSurface(&const_cast<SDL_Surface&> (sourceImage_in),
//                       NULL,
//                       result,
//                       NULL))
//   {
//     ACE_DEBUG((LM_ERROR,
//                ACE_TEXT("failed to SDL_BlitSurface(): %s, aborting\n"),
//                ACE_TEXT(SDL_GetError())));
  //
//     // clean up
//     SDL_FreeSurface(result);
  //
//     return NULL;
//   } // end IF

  // lock surface during pixel access
  if (SDL_MUSTLOCK ((&sourceImage_in)))
    if (SDL_LockSurface (&const_cast<SDL_Surface&> (sourceImage_in)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_LockSurface(): \"%s\", aborting\n"),
                ACE_TEXT (SDL_GetError ())));
    return;
  } // end IF

  for (unsigned int i = 0;
       i < static_cast<unsigned int> (sourceImage_in.h);
       i++)
    ::memcpy ((static_cast<ACE_UINT8*> (targetImage_in.pixels) + (targetImage_in.pitch * i)),
              (static_cast<ACE_UINT8*> (sourceImage_in.pixels) + (sourceImage_in.pitch * i)),
              (sourceImage_in.w * sourceImage_in.format->BytesPerPixel)); // RGBA --> 4 bytes (?!!!)

  if (SDL_MUSTLOCK ((&sourceImage_in)))
    SDL_UnlockSurface (&const_cast<SDL_Surface&> (sourceImage_in));
}

SDL_Surface*
RPG_Graphics_Surface::copy (const SDL_Surface& sourceImage_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Surface::copy"));

  SDL_Surface* result = NULL;
  result = SDL_CreateRGBSurface (RPG_Graphics_Surface::SDL_surface_flags,
                                 sourceImage_in.w,
                                 sourceImage_in.h,
                                 sourceImage_in.format->BitsPerPixel,
                                 sourceImage_in.format->Rmask,
                                 sourceImage_in.format->Gmask,
                                 sourceImage_in.format->Bmask,
                                 sourceImage_in.format->Amask);
  if (!result)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_CreateRGBSurface(): \"%s\", aborting\n"),
                ACE_TEXT (SDL_GetError ())));
    return NULL;
  } // end IF

  // *NOTE*: blitting does not preserve the alpha channel...
  // --> do it manually
//   if (SDL_BlitSurface(&const_cast<SDL_Surface&> (sourceImage_in),
//                       NULL,
//                       result,
//                       NULL))
//   {
//     ACE_DEBUG((LM_ERROR,
//                ACE_TEXT("failed to SDL_BlitSurface(): %s, aborting\n"),
//                ACE_TEXT(SDL_GetError())));
  //
//     // clean up
//     SDL_FreeSurface(result);
  //
//     return NULL;
//   } // end IF

  // lock surface during pixel access
  if (SDL_MUSTLOCK ((&sourceImage_in)))
    if (SDL_LockSurface (&const_cast<SDL_Surface&> (sourceImage_in)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_LockSurface(): \"%s\", aborting\n"),
                ACE_TEXT (SDL_GetError ())));

    // clean up
    SDL_FreeSurface (result);

    return NULL;
  } // end IF

  for (unsigned int i = 0;
       i < static_cast<unsigned int> (sourceImage_in.h);
       i++)
    ::memcpy ((static_cast<unsigned char*> (result->pixels) + (result->pitch * i)),
              (static_cast<unsigned char*> (sourceImage_in.pixels) + (sourceImage_in.pitch * i)),
              (sourceImage_in.w * sourceImage_in.format->BytesPerPixel)); // RGBA --> 4 bytes (?!!!)

  if (SDL_MUSTLOCK ((&sourceImage_in)))
    SDL_UnlockSurface (&const_cast<SDL_Surface&> (sourceImage_in));

  return result;
}

void
#if defined (SDL_USE)
RPG_Graphics_Surface::update (const struct SDL_Rect& dirty_in,
                              SDL_Surface* targetSurface_in)
#elif defined (SDL2_USE)
RPG_Graphics_Surface::update (const struct SDL_Rect& dirty_in,
                              SDL_Window* targetWindow_in)
#endif // SDL_USE || SDL2_USE
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Surface::update"));

  // sanity check
#if defined (SDL_USE)
  ACE_ASSERT (targetSurface_in);
#elif defined (SDL2_USE)
  ACE_ASSERT (targetWindow_in);
#endif // SDL_USE || SDL2_USE

  // handle clipping
#if defined (SDL_USE)
  SDL_Rect intersection =
      RPG_Graphics_SDL_Tools::intersect (targetSurface_in->clip_rect, dirty_in);
  if ((intersection.w == 0) || (intersection.h == 0))
    return; // nothing to do...
#endif // SDL_USE

#if defined (SDL_USE)
  //SDL_UpdateRects(targetSurface_in,
  //                1,
  //                &dirty_in);
  SDL_UpdateRect (targetSurface_in,
                  intersection.x, intersection.y,
                  intersection.w, intersection.h);
#elif defined (SDL2_USE)
  SDL_UpdateWindowSurfaceRects (targetWindow_in,
                                &dirty_in,
                                1);
#endif // SDL_USE || SDL2_USE
}

SDL_Surface*
RPG_Graphics_Surface::loadPNG (const std::string& filename_in,
                               FILE* file_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Surface::loadPNG"));

  // init return value(s)
  SDL_Surface* result = NULL;

  // sanity check
  //--> file must have a PNG signature
  unsigned char sig_buffer[RPG_GRAPHICS_PNG_SIGNATURE_BYTES];
  if (ACE_OS::fread (sig_buffer,
                     RPG_GRAPHICS_PNG_SIGNATURE_BYTES,
                     1,
                     file_in) != 1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::fread(%u): \"%m\", aborting\n"),
                RPG_GRAPHICS_PNG_SIGNATURE_BYTES));
    return NULL;
  } // end IF
  if (png_sig_cmp (sig_buffer,                        // buffer
                   0,                                 // start at the beginning
                   RPG_GRAPHICS_PNG_SIGNATURE_BYTES)) // #signature bytes to check
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to png_sig_cmp(): \"%m\", aborting\n")));
    return NULL;
  } // end IF

  png_structp png_ptr = NULL;
  // create the PNG loading context structure
  png_ptr = png_create_read_struct (PNG_LIBPNG_VER_STRING, // version
                                    NULL,                  // error
                                    NULL,                  // error handler
                                    NULL);                 // warning handler
  if (!png_ptr)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to png_create_read_struct(): \"%m\", aborting\n")));
    return NULL;
  } // end IF
  png_set_sig_bytes (png_ptr,
                     RPG_GRAPHICS_PNG_SIGNATURE_BYTES);

  // *NOTE* beyond this point:
  // - cleanup "png_ptr"

  png_infop info_ptr = NULL;
  // allocate/initialize memory for any image information [REQUIRED]
  info_ptr = png_create_info_struct (png_ptr);
  if (!info_ptr)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to png_create_info_struct(): \"%m\", aborting\n")));

    // clean up
    png_destroy_read_struct (&png_ptr,
                             NULL,
                             NULL);

    return NULL;
  } // end IF

  // *NOTE* beyond this point:
  // - cleanup "png_ptr"
  // - cleanup "info_ptr"

  // save stack context, set up error handling
  if (::setjmp (png_jmpbuf (png_ptr)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::setjmp(): \"%m\", aborting\n")));

    // clean up
    png_destroy_read_struct (&png_ptr,
                             &info_ptr,
                             NULL);

    return NULL;
  } // end IF

  // set up data input callback
  png_init_io (png_ptr, file_in);
  //// *TODO*: use png_init_io() instead (load directly from the file...)
  //png_set_read_fn(png_ptr,
  //                const_cast<unsigned char*>(buffer_in),
  //                PNG_read_callback);

  // read PNG header info
  png_read_info (png_ptr,
                 info_ptr);

  png_uint_32 width = 0;
  png_uint_32 height = 0;
  int bit_depth = 0;
  int color_type = 0;
  int interlace = 0;
  int compression = 0;
  int filter = 0;
  png_get_IHDR (png_ptr,
                info_ptr,
                &width,
                &height,
                &bit_depth,
                &color_type,
                &interlace,
                &compression,
                &filter);

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("loading PNG \"%s\" [wxh,d,#c,t,i,c,f]: %ux%u,%d,%u,%d,%d,%d,%d...\n"),
              ACE_TEXT (filename_in.c_str()),
              width,
              height,
              bit_depth,
              static_cast<unsigned int> (png_get_channels (png_ptr, info_ptr)),
              color_type,
              interlace,
              compression,
              filter));

  // reduce 16 bit per channel to 8 bit per channel
  // --> strip the second byte of information from a 16-bit depth file
  png_set_strip_16 (png_ptr);

  // expand:
  // - 1,2,4 bit grayscale to 8 bit grayscale
  // - paletted images to RGB
  // - tRNS to true alpha channel
  // --> expand data to 24-bit RGB, or 8-bit grayscale, with alpha if available
  png_set_expand (png_ptr);

  // expand grayscale to full RGB
  // --> expand the grayscale to 24-bit RGB if necessary
  png_set_gray_to_rgb (png_ptr);

//   // *NOTE*: we're using per-surface alpha only
//   // --> strip any alpha channel
//   png_set_strip_alpha(png_ptr);

  if ((color_type & PNG_COLOR_MASK_ALPHA) == 0)
  {
    // add an (opaque) alpha channel to anything that doesn't have one yet
    // --> add a filler byte to 8-bit Gray or 24-bit RGB images
    png_set_filler (png_ptr,
                    0xFF,
                    //                   alpha_in,
                    PNG_FILLER_AFTER);
    png_set_add_alpha (png_ptr,
                       0xFF,
                       //                      alpha_in,
                       PNG_FILLER_AFTER);
    png_read_update_info (png_ptr, info_ptr);
  } // end IF

  // allocate surface
  // *NOTE*:
  // - always TRY to (!) place the surface in VideoRAM; this may not work (check flags after loading !)
  // - conversion between PNG header info <--> SDL argument formats requires some casts
  result = RPG_Graphics_Surface::create (width,
                                         height);
  if (!result)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to RPG_Graphics_Surface::create(%u,%u), aborting\n"),
                width, height));

    // clean up
    png_destroy_read_struct (&png_ptr,
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
  try {
    row_pointers = new png_bytep[height];
  } catch (...) {
    row_pointers = NULL;
  }
  if (!row_pointers)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to allocate memory(%u): \"%m\", aborting\n"),
                (sizeof(png_bytep) * height)));

    // clean up
    png_destroy_read_struct (&png_ptr,
                             &info_ptr,
                             NULL);
    SDL_FreeSurface (result);

    return NULL;
  } // end IF

  // *NOTE* beyond this point:
  // - cleanup "png_ptr"
  // - cleanup "info_ptr"
  // - SDL_FreeSurface() result
  // - delete[] row_pointers

  // lock surface during pixel access
  if (SDL_MUSTLOCK (result))
    if (SDL_LockSurface (result))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to SDL_LockSurface(): \"%s\", aborting\n"),
                  ACE_TEXT (SDL_GetError ())));

      // clean up
      png_destroy_read_struct (&png_ptr,
                               &info_ptr,
                               NULL);
      SDL_FreeSurface (result);
      delete [] row_pointers;

      return NULL;
    } // end IF

  for (unsigned int row = 0;
       row < height;
       row++)
    row_pointers[row] =
        static_cast<png_bytep>(static_cast<Uint8*>(result->pixels) + (row * result->pitch));

  // read the image from the memory buffer onto the surface buffer
  // --> read the whole image into memory at once
  png_read_image (png_ptr, row_pointers);

  // unlock surface
  if (SDL_MUSTLOCK (result))
    SDL_UnlockSurface (result);

  // clean up
  png_destroy_read_struct (&png_ptr,
                           &info_ptr,
                           NULL);
  delete [] row_pointers;

  return result;
}
