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

#include "rpg_graphics_texture.h"

#include "ace/OS.h"
#include "ace/ACE.h"
#include "ace/Log_Msg.h"

#include "png.h"

#include "common_file_tools.h"

#include "rpg_common_macros.h"

#include "rpg_graphics_defines.h"
#include "rpg_graphics_common_tools.h"
#include "rpg_graphics_SDL_tools.h"
#include "rpg_graphics_surface.h"

RPG_Graphics_Texture::RPG_Graphics_Texture ()
 : myTexture (NULL)
 , myOwnTexture (false)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Texture::RPG_Graphics_Texture"));

}

RPG_Graphics_Texture::RPG_Graphics_Texture (const RPG_Graphics_Texture& texture_in)
 : myTexture (texture_in.myTexture),
   myOwnTexture (texture_in.myOwnTexture)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Texture::RPG_Graphics_Texture"));

  // "steal" ownership
  if (myOwnTexture)
    const_cast<RPG_Graphics_Texture&> (texture_in).myOwnTexture = false;
}

RPG_Graphics_Texture::RPG_Graphics_Texture (SDL_Renderer* renderer_in,
                                            SDL_Surface* surface_in)
 : myTexture (SDL_CreateTextureFromSurface (renderer_in, surface_in))
 , myOwnTexture (true)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Texture::RPG_Graphics_Texture"));

  if (!myTexture)
  { // *TODO*: throw an exception
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_CreateTextureFromSurface(): \"%s\", returning\n"),
                ACE_TEXT (SDL_GetError ())));
    return;
  } // end IF
}

RPG_Graphics_Texture::RPG_Graphics_Texture (SDL_Texture* texture_in,
                                            bool ownTexture_in)
 : myTexture (texture_in),
   myOwnTexture (ownTexture_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Texture::RPG_Graphics_Texture"));

}

RPG_Graphics_Texture::~RPG_Graphics_Texture()
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Texture::~RPG_Graphics_Texture"));

  // clean up
  if (myOwnTexture && myTexture)
    SDL_DestroyTexture (myTexture);
}

void
RPG_Graphics_Texture::initialize (SDL_Texture* texture_in,
                                  bool owntexture_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Texture::initialize"));

  // sanity check(s)
  ACE_ASSERT (texture_in);

  // clean up
  if (myTexture)
  {
    if (myOwnTexture)
    {
      SDL_DestroyTexture (myTexture);
      myOwnTexture = false;
    } // end IF
    myTexture = NULL;
  } // end IF

  myTexture = texture_in;
  myOwnTexture = owntexture_in;
}

//void
//RPG_Graphics_Texture::clip ()
//{
//  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Texture::clip"));
//
//  SDL_Texture* surface_p = SDL_GetVideoSurface ();
//  // sanity check(s)
//  ACE_ASSERT (surface_p);
//  if (((myClipRectangle.w == 0) ||
//       (myClipRectangle.h == 0)) ||
//      !surface_p)
//    return; // nothing to do
//
//  SDL_SetClipRect (surface_p,
//	                 &myClipRectangle);
//}
//
//void
//RPG_Graphics_Texture::unclip ()
//{
//  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Texture::unclip"));
//
//  SDL_Texture* surface_p = SDL_GetVideoSurface ();
//  // sanity check(s)
//  ACE_ASSERT (surface_p);
//  if (!surface_p)
//    return; // nothing to do
//
//  // cache previous entry
//  SDL_GetClipRect (surface_p,
//                   &myClipRectangle);
//
//  SDL_SetClipRect (surface_p, NULL);
//}

SDL_Texture*
RPG_Graphics_Texture::load (SDL_Renderer* renderer_in,
                            const std::string& filename_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Texture::load"));

  // initialize return value(s)
  SDL_Texture* result = NULL;

  // sanity check(s)
  if (!Common_File_Tools::isReadable (filename_in))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid argument(\"%s\"): not readable, aborting\n"),
                ACE_TEXT (filename_in.c_str ())));
    return NULL;
  } // end IF

  // *TODO*: there seems to be a bug in WIN32 ACE_OS::fopen()
  // FILE* file_ptr = ACE_OS::fopen (filename_in.c_str (),         // filename
  FILE* file_ptr = ::fopen (filename_in.c_str (),         // filename
                            ACE_TEXT_ALWAYS_CHAR ("rb")); // mode
  if (!file_ptr)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::fopen(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (filename_in.c_str ())));
    return NULL;
  } // end IF

  // extract SDL surface from PNG
  result =
    RPG_Graphics_Texture::loadPNG (renderer_in,
                                   ACE_TEXT_ALWAYS_CHAR (ACE::basename (filename_in.c_str (), ACE_DIRECTORY_SEPARATOR_CHAR)),
                                   file_ptr);
  if (!result)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to RPG_Graphics_Texture::loadPNG(\"%s\"), aborting\n"),
                ACE_TEXT (filename_in.c_str ())));
  if (ACE_OS::fclose (file_ptr))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::fclose(\"%s\"): \"%m\", continuing\n"),
                ACE_TEXT (filename_in.c_str ())));
    return NULL;
  } // end IF

  // clean up
  if (ACE_OS::fclose (file_ptr))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::fclose(\"%s\"): \"%m\", continuing\n"),
                ACE_TEXT (filename_in.c_str ())));

  return result;
}

void
RPG_Graphics_Texture::savePNG (const SDL_Texture& texture_in,
                               const std::string& targetFile_in,
                               bool alpha_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Texture::savePNG"));

  // preliminary check(s)
  if (Common_File_Tools::isReadable (targetFile_in))
    if (!Common_File_Tools::deleteFile (targetFile_in))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to RPG_Common_File_Tools::deleteFile(\"%s\"), returning\n"),
                  ACE_TEXT (targetFile_in.c_str ())));
      return;
    } // end IF

  Uint32 format_i = 0;
  int width_i = 0, height_i = 0;
  if (SDL_QueryTexture (&const_cast<SDL_Texture&> (texture_in),
                        &format_i,
                        NULL,
                        &width_i,
                        &height_i) < 0)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_LockTexture(): \"%s\", returning\n"),
                ACE_TEXT (SDL_GetError ())));
    return;
  } // end IF
  ACE_ASSERT (format_i && width_i && height_i);
  SDL_PixelFormat* format_p = SDL_AllocFormat (format_i);
  ACE_ASSERT (format_p);

  void* data_p = NULL;
  int pitch_i = 0;
  if (SDL_LockTexture (&const_cast<SDL_Texture&> (texture_in),
                       NULL,
                       &data_p,
                       &pitch_i) < 0)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_LockTexture(): \"%s\", returning\n"),
                ACE_TEXT (SDL_GetError ())));
    SDL_FreeFormat (format_p);
    return;
  } // end IF
  ACE_ASSERT (data_p && pitch_i);

  unsigned char* output = NULL;
  try {
    output =
        new unsigned char[(width_i * height_i * (alpha_in ? format_p->BytesPerPixel
                                                          : format_p->BytesPerPixel - 1))];
  } catch (...) {
    output = NULL;
  }
  if (!output)
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory(%u): \"%m\", returning\n"),
                (width_i * height_i * (alpha_in ? format_p->BytesPerPixel
                                                : format_p->BytesPerPixel - 1))));
    SDL_FreeFormat (format_p);
    SDL_UnlockTexture (&const_cast<SDL_Texture&> (texture_in));
    return;
  } // end IF

  png_bytep* image = NULL;
  try {
    image = new png_bytep[height_i];
  } catch (...) {
    image = NULL;
  }
  if (!image)
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory(%u): \"%m\", returning\n"),
                (sizeof(png_bytep) * height_i)));
    delete [] output;
    SDL_FreeFormat (format_p);
    SDL_UnlockTexture (&const_cast<SDL_Texture&> (texture_in));
    return;
  } // end IF

  // (if neccessary,) strip out alpha bytes and reorder the image bytes to RGB
  Uint32* pixels = static_cast<Uint32*> (data_p);
  for (unsigned int j = 0;
       j < static_cast<unsigned int> (height_i);
       j++)
  {
    image[j] = output;

    for (unsigned int i = 0;
         i < static_cast<unsigned int> (width_i);
         i++)
    {
      *output++ = (((*pixels) & format_p->Rmask) >> format_p->Rshift); /* red   */
      *output++ = (((*pixels) & format_p->Gmask) >> format_p->Gshift); /* green */
      *output++ = (((*pixels) & format_p->Bmask) >> format_p->Bshift); /* blue  */
      if (alpha_in)
        *output++ = (((*pixels) & format_p->Amask) >> format_p->Ashift); /* alpha */

      pixels++;
    } // end FOR
  } // end FOR

  SDL_FreeFormat (format_p); format_p = NULL;
  SDL_UnlockTexture (&const_cast<SDL_Texture&> (texture_in));

  // open the file
  FILE* fp = NULL;
  fp = ACE_OS::fopen (ACE_TEXT (targetFile_in.c_str ()),
                      ACE_TEXT_ALWAYS_CHAR ("wb"));
  if (!fp)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to open file(\"%s\"): \"%m\", returning\n"),
                ACE_TEXT (targetFile_in.c_str ())));
    // *NOTE*: output has been modified, but the value is still present in image[0]
    delete [] image[0];
    delete [] image;
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
    if (ACE_OS::fclose (fp))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to close file(\"%s\"): \"%m\", continuing\n"),
                  ACE_TEXT (targetFile_in.c_str ())));
    // *NOTE*: output has been modified, but the value is still present in image[0]
    delete [] image[0];
    delete [] image;
    return;
  } // end IF

  png_infop info_ptr = NULL;
  info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to png_create_info_struct(): \"%m\", returning\n")));
    png_destroy_write_struct (&png_ptr,
                              NULL);
    if (ACE_OS::fclose (fp))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to close file(\"%s\"): \"%m\", continuing\n"),
                  ACE_TEXT (targetFile_in.c_str ())));
    // *NOTE*: output has been modified, but the value is still present in image[0]
    delete [] image[0];
    delete [] image;
    return;
  } // end IF

  // save stack context, set up error handling
  if (::setjmp (png_jmpbuf (png_ptr)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::setjmp(): \"%m\", aborting\n")));
    png_destroy_write_struct (&png_ptr,
                              &info_ptr);
    if (ACE_OS::fclose (fp))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to close file(\"%s\"): \"%m\", continuing\n"),
                  ACE_TEXT (targetFile_in.c_str ())));
    // *NOTE*: output has been modified, but the value is still present in image[0]
    delete [] image[0];
    delete [] image;
    return;
  } // end IF

  // write the image
  png_init_io (png_ptr, fp);

  png_set_IHDR (png_ptr,                         // context
                info_ptr,                        // header info
                width_i,                         // width
                height_i,                        // height
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
  delete [] image[0];
  delete [] image;

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("wrote PNG file \"%s\"...\n"),
              ACE_TEXT (targetFile_in.c_str ())));
}

SDL_Texture*
RPG_Graphics_Texture::create (SDL_Renderer* renderer_in,
                              unsigned int width_in,
                              unsigned int height_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Texture::create"));

  // sanity check(s)
  ACE_ASSERT (renderer_in);

  SDL_Texture* result =
    SDL_CreateTexture (renderer_in,
                       SDL_PIXELFORMAT_ARGB8888,
                       SDL_TEXTUREACCESS_STREAMING,
                       static_cast<int> (width_in),
                       static_cast<int> (height_in));
  if (!result)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_CreateTexture(): \"%s\", aborting\n"),
                ACE_TEXT (SDL_GetError ())));
    return NULL;
  } // end IF

  return result;
}

SDL_Texture*
RPG_Graphics_Texture::get (SDL_Renderer* renderer_in,
                           const RPG_Graphics_Offset_t& offset_in,
                           unsigned int width_in,
                           unsigned int height_in,
                           const SDL_Texture& source_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Texture::get"));

  // sanity check(s)
  ACE_ASSERT (renderer_in);

  struct SDL_Rect source_rect =
    {offset_in.first, offset_in.second, static_cast<int> (width_in), static_cast<int> (height_in)};
  struct SDL_Rect dest_rect =
    {0, 0, static_cast<int> (width_in), static_cast<int> (height_in)};
  if (SDL_RenderCopy (renderer_in,
                      &const_cast<SDL_Texture&> (source_in),
                      &source_rect,
                      &dest_rect) < 0)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_RenderCopy(): \"%s\", aborting\n"),
                ACE_TEXT (SDL_GetError ())));
    return NULL;
  } // end IF

  Uint32 format_i = 0;
  int width_i = 0, height_i = 0;
  if (SDL_QueryTexture (&const_cast<SDL_Texture&> (source_in),
                        &format_i,
                        NULL,
                        &width_i,
                        &height_i) < 0)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_QueryTexture(): \"%s\", aborting\n"),
                ACE_TEXT (SDL_GetError ())));
    return NULL;
  } // end IF
  ACE_ASSERT (format_i && width_i && height_i);
  SDL_PixelFormat* format_p = SDL_AllocFormat (format_i);
  ACE_ASSERT (format_p);

  unsigned char* output = NULL;
  try {
    output =
        new unsigned char[(width_in * height_in * format_p->BytesPerPixel)];
  } catch (...) {
    output = NULL;
  }
  if (!output)
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory(%u): \"%m\", aborting\n"),
                width_in * height_in * format_p->BytesPerPixel));
    SDL_FreeFormat (format_p);
    return NULL;
  } // end IF

  SDL_Rect rect_s = { 0, 0, static_cast<int> (width_in), static_cast<int> (height_in) };
  if (SDL_RenderReadPixels (renderer_in,
                            &rect_s,
                            format_i,
                            output,
                            width_in * format_p->BytesPerPixel) < 0)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_RenderReadPixels(): \"%s\", aborting\n"),
                ACE_TEXT (SDL_GetError ())));
    delete [] output;
    SDL_FreeFormat (format_p);
    return NULL;
  } // end IF

  SDL_Surface* surface_p =
    SDL_CreateRGBSurfaceFrom (output,
                              width_in,
                              height_in,
                              format_p->BytesPerPixel * 8,
                              width_in * format_p->BytesPerPixel,
                              format_p->Rmask,
                              format_p->Gmask,
                              format_p->Bmask, 
                              format_p->Amask);
  if (!surface_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_RenderReadPixels(): \"%s\", aborting\n"),
                ACE_TEXT (SDL_GetError ())));
    delete [] output;
    SDL_FreeFormat (format_p);
    return NULL;
  } // end IF

  // initialize return value
  SDL_Texture* result = SDL_CreateTextureFromSurface (renderer_in,
                                                      surface_p);
  if (!result)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_CreateTextureFromSurface(): \"%s\", aborting\n"),
                ACE_TEXT (SDL_GetError ())));
    SDL_FreeSurface (surface_p);
    delete [] output;
    SDL_FreeFormat (format_p);
    return NULL;
  } // end IF

  SDL_FreeSurface (surface_p);
  delete [] output;
  SDL_FreeFormat (format_p);

  return result;
}

void
RPG_Graphics_Texture::get (SDL_Renderer* renderer_in,
                           const RPG_Graphics_Offset_t& offset_in,
                           const SDL_Texture& source_in,
                           SDL_Texture& target_inout)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Texture::get"));

  // sanity check(s)
  ACE_ASSERT (renderer_in);
  Uint32 format_i = 0, format_2 = 0;
  int width_i = 0, height_i = 0, width_2 = 0, height_2 = 0;
  if (SDL_QueryTexture (&const_cast<SDL_Texture&> (source_in),
                        &format_i,
                        NULL,
                        &width_i,
                        &height_i) < 0)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_QueryTexture(): \"%s\", returning\n"),
                ACE_TEXT (SDL_GetError ())));
    return;
  } // end IF
  ACE_ASSERT (format_i && width_i && height_i);
  if (SDL_QueryTexture (&target_inout,
                        &format_2,
                        NULL,
                        &width_2,
                        &height_2) < 0)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_QueryTexture(): \"%s\", returning\n"),
                ACE_TEXT (SDL_GetError ())));
    return;
  } // end IF
  ACE_ASSERT (format_2 && width_2 && height_2);
  ACE_ASSERT (width_2 >= width_i - offset_in.first);
  ACE_ASSERT (height_2 >= height_i - offset_in.second);

  if (SDL_SetRenderTarget (renderer_in,
                           &target_inout) < 0)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_SetRenderTarget(): \"%s\", returning\n"),
                ACE_TEXT (SDL_GetError ())));
    return;
  } // end IF

  SDL_Rect source_rect = { offset_in.first, offset_in.second, width_i - offset_in.first, height_i - offset_in.second };
  SDL_Rect dest_rect = { 0, 0, width_i - offset_in.first, height_i - offset_in.second };
  if (SDL_RenderCopy (renderer_in,
                      &const_cast<SDL_Texture&> (source_in),
                      &source_rect,
                      &dest_rect) < 0)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_RenderCopy(): \"%s\", returning\n"),
                ACE_TEXT (SDL_GetError ())));
    SDL_SetRenderTarget (renderer_in, NULL);
    return;
  } // end IF

  SDL_SetRenderTarget (renderer_in, NULL);
}

void
RPG_Graphics_Texture::put (SDL_Renderer* renderer_in,
                           const RPG_Graphics_Offset_t& offset_in,
                           const SDL_Texture& source_in,
                           SDL_Texture* target_inout,
                           struct SDL_Rect& dirtyRegion_out)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Texture::put"));

  // sanity check(s)
  ACE_ASSERT (renderer_in);
  Uint32 format_i = 0, format_2 = 0;
  int width_i = 0, height_i = 0, width_2 = 0, height_2 = 0;
  if (SDL_QueryTexture (&const_cast<SDL_Texture&> (source_in),
                        &format_i,
                        NULL,
                        &width_i,
                        &height_i) < 0)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_QueryTexture(): \"%s\", returning\n"),
                ACE_TEXT (SDL_GetError ())));
    return;
  } // end IF
  ACE_ASSERT (format_i && width_i && height_i);
  ACE_ASSERT (target_inout);
  if (SDL_QueryTexture (target_inout,
                        &format_2,
                        NULL,
                        &width_2,
                        &height_2) < 0)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_QueryTexture(): \"%s\", returning\n"),
                ACE_TEXT (SDL_GetError ())));
    return;
  } // end IF
  ACE_ASSERT (format_2 && width_2 && height_2);
  ACE_ASSERT (width_2 >= width_i - offset_in.first);
  ACE_ASSERT (height_2 >= height_i - offset_in.second);

  // initialize return value(s)
  ACE_OS::memset (&dirtyRegion_out, 0, sizeof (struct SDL_Rect));

  if (SDL_SetRenderTarget (renderer_in,
                           target_inout) < 0)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_SetRenderTarget(): \"%s\", returning\n"),
                ACE_TEXT (SDL_GetError ())));
    return;
  } // end IF

  SDL_Rect source_rect = { offset_in.first, offset_in.second, width_i - offset_in.first, height_i - offset_in.second };
  SDL_Rect dest_rect = { 0, 0, width_i - offset_in.first, height_i - offset_in.second };
  if (SDL_RenderCopy (renderer_in,
                      &const_cast<SDL_Texture&> (source_in),
                      &source_rect,
                      &dest_rect) < 0)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_RenderCopy(): \"%s\", returning\n"),
                ACE_TEXT (SDL_GetError ())));
    SDL_SetRenderTarget (renderer_in, NULL);
    return;
  } // end IF

  SDL_SetRenderTarget (renderer_in, NULL);

  // compute dirty region
  dirtyRegion_out.w = width_i - offset_in.first;
  dirtyRegion_out.h = height_i - offset_in.second;
}

void
RPG_Graphics_Texture::put (const RPG_Graphics_Offset_t& offset_in,
                           const SDL_Surface& source_in,
                           SDL_Texture* target_inout,
                           struct SDL_Rect& dirtyRegion_out)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Texture::put"));

  // sanity check(s)
  Uint32 format_i = 0;
  int width_i = 0, height_i = 0;
  if (SDL_QueryTexture (target_inout,
                        &format_i,
                        NULL,
                        &width_i,
                        &height_i) < 0)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_QueryTexture(): \"%s\", returning\n"),
                ACE_TEXT (SDL_GetError ())));
    return;
  } // end IF
  ACE_ASSERT (format_i && width_i && height_i);
  ACE_ASSERT (target_inout);
  ACE_ASSERT (width_i >= source_in.w - offset_in.first);
  ACE_ASSERT (height_i >= source_in.h - offset_in.second);

  // initialize return value(s)
  ACE_OS::memset (&dirtyRegion_out, 0, sizeof (SDL_Rect));

  uint8_t* data_p = static_cast<uint8_t*> (source_in.pixels);
  data_p += (offset_in.second * source_in.pitch) + offset_in.first;
  if (SDL_UpdateTexture (target_inout,
                         NULL,
                         data_p,
                         source_in.pitch) < 0)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_UpdateTexture(): \"%s\", returning\n"),
                ACE_TEXT (SDL_GetError ())));
    return;
  } // end IF

  // compute dirty region
  dirtyRegion_out.w = width_i - offset_in.first;
  dirtyRegion_out.h = height_i - offset_in.second;
}

bool
RPG_Graphics_Texture::putText (enum RPG_Graphics_Font font_in,
                               const std::string& textString_in,
                               const struct SDL_Color& color_in,
                               bool shade_in,
                               const struct SDL_Color& shadeColor_in,
                               const RPG_Graphics_Offset_t& offset_in,
                               SDL_Texture* target_in,
                               struct SDL_Rect& dirtyRegion_out)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Texture::putText"));

  // init return value(s)
  ACE_OS::memset (&dirtyRegion_out, 0, sizeof (struct SDL_Rect));

  // sanity check(s)
  ACE_ASSERT (target_in);

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
    RPG_Graphics_Texture::put (offset,
                               *rendered_text,
                               target_in,
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
  RPG_Graphics_Texture::put (offset_in,
                             *rendered_text,
                             target_in,
                             dirty_region);
  dirtyRegion_out = RPG_Graphics_SDL_Tools::boundingBox (dirty_region,
                                                         dirtyRegion_out);

  // clean up
  SDL_FreeSurface (rendered_text);

  return true;
}

void
RPG_Graphics_Texture::putRectangle (SDL_Renderer* renderer_in,
                                    const struct SDL_Rect& rectangle_in,
                                    Uint32 color_in,
                                    SDL_Texture* target_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Texture::putRectangle"));

  // sanity check(s)
  ACE_ASSERT (target_in);
  Uint32 format_i = 0;
  int width_i = 0, height_i = 0;
  if (SDL_QueryTexture (target_in,
                        &format_i,
                        NULL,
                        &width_i,
                        &height_i) < 0)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_QueryTexture(): \"%s\", returning\n"),
                ACE_TEXT (SDL_GetError ())));
    return;
  } // end IF
  ACE_ASSERT (format_i && width_i && height_i);
  SDL_Rect dest_rect = { 0, 0, width_i, height_i };
  SDL_Rect intersection;
  ACE_OS::memset (&intersection, 0, sizeof (SDL_Rect));
  intersection =
    RPG_Graphics_SDL_Tools::intersect (rectangle_in,
                                       dest_rect);
  if ((intersection.w == 0) || (intersection.h == 0))
    return; // rectangle is COMPLETELY outside of target surface

  SDL_PixelFormat* format_p = SDL_AllocFormat (format_i);
  ACE_ASSERT (format_p);
  Uint8 r = 0, g = 0, b = 0, a = 0;
  SDL_GetRGBA (color_in,
               format_p,
               &r, &g, &b, &a);
  SDL_FreeFormat (format_p); format_p = NULL;

  if (SDL_SetRenderDrawColor (renderer_in,
                              r, g, b, a) < 0)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_SetRenderDrawColor(): \"%s\", returning\n"),
                ACE_TEXT (SDL_GetError ())));
    return;
  } // end IF
  if (SDL_SetRenderTarget (renderer_in,
                           target_in) < 0)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_SetRenderTarget(): \"%s\", returning\n"),
                ACE_TEXT (SDL_GetError ())));
    return;
  } // end IF
  if (SDL_RenderFillRect (renderer_in,
                          &rectangle_in) < 0)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_RenderFillRect(): \"%s\", returning\n"),
                ACE_TEXT (SDL_GetError ())));
    return;
  } // end IF
  SDL_SetRenderTarget (renderer_in, NULL);
}

void
RPG_Graphics_Texture::alpha (Uint8 opacity_in,
                             SDL_Texture& target_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Texture::alpha"));

  if (SDL_SetTextureAlphaMod (&target_in,
                              opacity_in) < 0)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_SetTextureAlphaMod(): \"%s\", returning\n"),
                ACE_TEXT (SDL_GetError ())));
    return;
  } // end IF
}

SDL_Texture*
RPG_Graphics_Texture::alpha (SDL_Renderer* renderer_in,
                             const SDL_Texture& source_in,
                             Uint8 opacity_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Texture::alpha"));

  SDL_Texture* result = NULL;
  result = RPG_Graphics_Texture::copy (renderer_in,
                                       source_in);
  if (!result)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to RPG_Graphics_Texture::copy(), aborting\n")));
    return NULL;
  } // end IF

  RPG_Graphics_Texture::alpha (opacity_in,
                               *result);

  return result;
}

void
RPG_Graphics_Texture::clear (SDL_Renderer* renderer_in,
                             SDL_Texture* target_in,
                             const struct SDL_Rect* clipRectangle_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Texture::clear"));

  // sanity check(s)
  ACE_ASSERT (target_in);
  Uint32 format_i = 0;
  int width_i = 0, height_i = 0;
  if (SDL_QueryTexture (target_in,
                        &format_i,
                        NULL,
                        &width_i,
                        &height_i) < 0)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_QueryTexture(): \"%s\", returning\n"),
                ACE_TEXT (SDL_GetError ())));
    return;
  } // end IF
  ACE_ASSERT (format_i && width_i && height_i);
  SDL_PixelFormat* format_p = SDL_AllocFormat (format_i);
  ACE_ASSERT (format_p);

  RPG_Graphics_Texture::fill (renderer_in,
                              RPG_Graphics_SDL_Tools::getColor (COLOR_BLACK, // opaque
                                                                *format_p,
                                                                1.0f),
                              target_in,
                              clipRectangle_in);

  SDL_FreeFormat (format_p); format_p = NULL;
}

void
RPG_Graphics_Texture::fill (SDL_Renderer* renderer_in,
                            Uint32 color_in,
                            SDL_Texture* target_in,
                            const struct SDL_Rect* clipRectangle_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Texture::fill"));

  // sanity check(s)
  ACE_ASSERT (target_in);
  Uint32 format_i = 0;
  int width_i = 0, height_i = 0;
  if (SDL_QueryTexture (target_in,
                        &format_i,
                        NULL,
                        &width_i,
                        &height_i) < 0)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_QueryTexture(): \"%s\", returning\n"),
                ACE_TEXT (SDL_GetError ())));
    return;
  } // end IF
  ACE_ASSERT (format_i && width_i && height_i);

  SDL_PixelFormat* format_p = SDL_AllocFormat (format_i);
  ACE_ASSERT (format_p);
  Uint8 r = 0, g = 0, b = 0, a = 0;
  SDL_GetRGBA (color_in,
               format_p,
               &r, &g, &b, &a);
  SDL_FreeFormat (format_p); format_p = NULL;

  // fill with color
  if (SDL_SetRenderDrawColor (renderer_in,
                              r, g, b, a) < 0)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_SetRenderDrawColor(): \"%s\", returning\n"),
                ACE_TEXT (SDL_GetError ())));
    return;
  } // end IF
  if (SDL_SetRenderTarget (renderer_in,
                           target_in) < 0)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_SetRenderTarget(): \"%s\", returning\n"),
                ACE_TEXT (SDL_GetError ())));
    return;
  } // end IF
  if (SDL_RenderFillRect (renderer_in,
                          clipRectangle_in) < 0)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_RenderFillRect(): \"%s\", returning\n"),
                ACE_TEXT (SDL_GetError ())));
    return;
  } // end IF
  SDL_SetRenderTarget (renderer_in, NULL);
}

void
RPG_Graphics_Texture::copy (SDL_Renderer* renderer_in,
                            const SDL_Texture& source_in,
                            SDL_Texture& target_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Texture::copy"));

  // sanity check(s)
  ACE_ASSERT (renderer_in);
  Uint32 format_i = 0, format_2 = 0;
  int width_i = 0, height_i = 0, width_2 = 0, height_2 = 0;
  if (SDL_QueryTexture (&const_cast<SDL_Texture&> (source_in),
                        &format_i,
                        NULL,
                        &width_i,
                        &height_i) < 0)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_QueryTexture(): \"%s\", returning\n"),
                ACE_TEXT (SDL_GetError ())));
    return;
  } // end IF
  ACE_ASSERT (format_i && width_i && height_i);
  if (SDL_QueryTexture (&target_in,
                        &format_2,
                        NULL,
                        &width_2,
                        &height_2) < 0)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_QueryTexture(): \"%s\", returning\n"),
                ACE_TEXT (SDL_GetError ())));
    return;
  } // end IF
  ACE_ASSERT (format_2 && width_2 && height_2);
  ACE_ASSERT (width_2 >= width_i);
  ACE_ASSERT (height_2 >= height_i);

  if (SDL_SetRenderTarget (renderer_in,
                           &target_in) < 0)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_SetRenderTarget(): \"%s\", returning\n"),
                ACE_TEXT (SDL_GetError ())));
    return;
  } // end IF

  SDL_Rect source_rect = { 0, 0, width_i, height_i };
  if (SDL_RenderCopy (renderer_in,
                      &const_cast<SDL_Texture&> (source_in),
                      &source_rect,
                      &source_rect) < 0)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_RenderCopy(): \"%s\", returning\n"),
                ACE_TEXT (SDL_GetError ())));
    SDL_SetRenderTarget (renderer_in, NULL);
    return;
  } // end IF

  SDL_SetRenderTarget (renderer_in, NULL);
}

SDL_Texture*
RPG_Graphics_Texture::copy (SDL_Renderer* renderer_in,
                            const SDL_Texture& source_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Texture::copy"));

  // sanity check(s)
  ACE_ASSERT (renderer_in);
  Uint32 format_i = 0;
  int width_i = 0, height_i = 0;
  if (SDL_QueryTexture (&const_cast<SDL_Texture&> (source_in),
                        &format_i,
                        NULL,
                        &width_i,
                        &height_i) < 0)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_QueryTexture(): \"%s\", aborting\n"),
                ACE_TEXT (SDL_GetError ())));
    return NULL;
  } // end IF
  ACE_ASSERT (format_i && width_i && height_i);

  SDL_Texture* result = NULL;
  result = SDL_CreateTexture (renderer_in,
                              format_i,
                              SDL_TEXTUREACCESS_TARGET,
                              width_i, height_i);
  if (!result)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_CreateTexture(): \"%s\", aborting\n"),
                ACE_TEXT (SDL_GetError ())));
    return NULL;
  } // end IF
  if (SDL_SetRenderTarget (renderer_in,
                           result) < 0)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_SetRenderTarget(): \"%s\", aborting\n"),
                ACE_TEXT (SDL_GetError ())));
    SDL_DestroyTexture (result); result = NULL;
    return NULL;
  } // end IF

  SDL_Rect source_rect = { 0, 0, width_i, height_i };
  if (SDL_RenderCopy (renderer_in,
                      &const_cast<SDL_Texture&> (source_in),
                      &source_rect,
                      &source_rect) < 0)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_RenderCopy(): \"%s\", returning\n"),
                ACE_TEXT (SDL_GetError ())));
    SDL_SetRenderTarget (renderer_in, NULL);
    SDL_DestroyTexture (result); result = NULL;
    return NULL;
  } // end IF

  SDL_SetRenderTarget (renderer_in, NULL);

  return result;
}

SDL_Texture*
RPG_Graphics_Texture::loadPNG (SDL_Renderer* renderer_in,
                               const std::string& filename_in,
                               FILE* file_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Texture::loadPNG"));

  // initialize return value(s)
  SDL_Texture* result = NULL;

  // sanity check
  //--> file must have a PNG signature
  unsigned char sig_buffer[RPG_GRAPHICS_PNG_SIGNATURE_BYTES];
  if (ACE_OS::fread (sig_buffer,
                     RPG_GRAPHICS_PNG_SIGNATURE_BYTES,
                     1,
                     file_in) != 1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to fread(%u): \"%m\", aborting\n"),
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
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to png_create_read_struct(): \"%m\", aborting\n")));
    return NULL;
  } // end IF
  png_set_sig_bytes (png_ptr, RPG_GRAPHICS_PNG_SIGNATURE_BYTES);

  // *NOTE* beyond this point:
  // - cleanup "png_ptr"

  png_infop info_ptr = NULL;
  // allocate/initialize memory for any image information [REQUIRED]
  info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to png_create_info_struct(): \"%m\", aborting\n")));
    png_destroy_read_struct (&png_ptr,
                             NULL,
                             NULL);
    return NULL;
  } // end IF

  // *NOTE* beyond this point:
  // - cleanup "png_ptr"
  // - cleanup "info_ptr"

  // save stack context, set up error handling
  if (::setjmp(png_jmpbuf(png_ptr)))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ::setjmp(): \"%m\", aborting\n")));
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
              static_cast<unsigned int>(png_get_channels(png_ptr, info_ptr)),
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
    png_read_update_info (png_ptr,
                          info_ptr);
  } // end IF

  // allocate surface
  // *NOTE*:
  // - conversion between PNG header info <--> SDL argument formats requires some casts
  SDL_Surface* surface_p = RPG_Graphics_Surface::create (width,
                                                         height);
  if (!surface_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to RPG_Graphics_Surface::create(%u,%u), aborting\n"),
                width, height));
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
    png_destroy_read_struct (&png_ptr,
                             &info_ptr,
                             NULL);
    SDL_FreeSurface (surface_p);
    return NULL;
  } // end IF

  // *NOTE* beyond this point:
  // - cleanup "png_ptr"
  // - cleanup "info_ptr"
  // - SDL_FreeSurface() result
  // - delete[] row_pointers

  // lock surface during pixel access
  if (SDL_MUSTLOCK (surface_p))
    if (SDL_LockSurface (surface_p))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to SDL_LockSurface(): \"%s\", aborting\n"),
                  ACE_TEXT (SDL_GetError ())));
      png_destroy_read_struct (&png_ptr,
                               &info_ptr,
                               NULL);
      SDL_FreeSurface (surface_p);
      delete [] row_pointers;
      return NULL;
    } // end IF

  for (unsigned int row = 0;
       row < height;
       row++)
    row_pointers[row] =
        static_cast<png_bytep>(static_cast<Uint8*> (surface_p->pixels) + (row * surface_p->pitch));

  // read the image from the memory buffer onto the surface buffer
  // --> read the whole image into memory at once
  png_read_image (png_ptr, row_pointers);

  // unlock surface
  if (SDL_MUSTLOCK (surface_p))
    SDL_UnlockSurface (surface_p);

  // clean up
  png_destroy_read_struct (&png_ptr,
                           &info_ptr,
                           NULL);
  delete [] row_pointers; row_pointers = NULL;

  result = SDL_CreateTextureFromSurface (renderer_in,
                                         surface_p);
  if (!result)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_RenderCopy(): \"%s\", returning\n"),
                ACE_TEXT (SDL_GetError ())));
    SDL_FreeSurface (surface_p); surface_p = NULL;
    return NULL;
  } // end IF

  SDL_FreeSurface (surface_p); surface_p = NULL;

  return result;
}
