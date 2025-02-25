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
#include "stdafx.h"

#include "rpg_graphics_SDL_tools.h"

#include <sstream>

#define _SDL_main_h
#define SDL_main_h_
#include "SDL.h"

#include "ace/OS.h"
#include "ace/Log_Msg.h"

#include "rpg_common_macros.h"
#include "rpg_common_defines.h"

#include "rpg_graphics_defines.h"
#include "rpg_graphics_common.h"
#include "rpg_graphics_dictionary.h"
#include "rpg_graphics_surface.h"
#include "rpg_graphics_common_tools.h"

// initialize statics
bool                    RPG_Graphics_SDL_Tools::myVideoPreInitialized = false;
RPG_Graphics_ColorMap_t RPG_Graphics_SDL_Tools::myColors;

bool
RPG_Graphics_SDL_Tools::preInitializeVideo (const struct RPG_Graphics_SDL_VideoConfiguration& configuration_in,
                                            const std::string& caption_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_SDL_Tools::preInitializeVideo"));

  // sanity check
  if (myVideoPreInitialized)
    return true; // nothing to do

#if defined (SDL2_USE) || defined (SDL3_USE)
  int number_of_video_drivers_i = SDL_GetNumVideoDrivers ();
  if (number_of_video_drivers_i < 0)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_GetNumVideoDrivers(): \"%s\", aborting\n"),
                ACE_TEXT (SDL_GetError ())));
    return false;
  } // end IF
  for (int i = 0;
       i < number_of_video_drivers_i;
       ++i)
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("[%d]: video driver: \"%s\"\n"),
                i, ACE_TEXT (SDL_GetVideoDriver (i))));
#endif // SDL2_USE || SDL3_USE

  // step1: initialize video subsystem
  std::string video_driver = configuration_in.video_driver;
  if (video_driver.empty ())
  {
    char* environment =
        ACE_OS::getenv (ACE_TEXT_ALWAYS_CHAR (RPG_GRAPHICS_SDL_VIDEO_DRIVER_ENV_VAR));
    if (environment)
      video_driver = environment;
  } // end IF
  // sanity check
  if (video_driver.empty ())
    video_driver =
      ACE_TEXT_ALWAYS_CHAR (RPG_GRAPHICS_DEF_SDL_VIDEO_DRIVER_NAME); // fallback
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("using video driver: \"%s\"\n"),
              ACE_TEXT (video_driver.c_str ())));

  // set flags passed to SDL_StartEventLoop
  int result = -1;
#if defined (SDL_USE)
  Uint32 flags = 0;
  result = SDL_VideoInit (video_driver.c_str (), // driver name
                          flags);
  if (result < 0)
#elif defined (SDL2_USE)
  result = SDL_VideoInit (video_driver.c_str ()); // driver name
  if (result < 0)
#elif defined (SDL3_USE)
  result = SDL_TRUE;
  if (result == SDL_FALSE)
#endif // SDL_USE || SDL2_USE || SDL3_USE
  {
#if defined (SDL_USE)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_VideoInit(\"%s\", %x): \"%s\", aborting\n"),
                ACE_TEXT (video_driver.c_str ()),
                flags,
                ACE_TEXT (SDL_GetError ())));
    char driver_name[BUFSIZ];
    SDL_VideoDriverName (driver_name,
                         sizeof (char[BUFSIZ]));
#elif defined (SDL2_USE)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_VideoInit(\"%s\"): \"%s\", aborting\n"),
                ACE_TEXT (video_driver.c_str ()),
                ACE_TEXT (SDL_GetError ())));
#elif defined (SDL3_USE)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to select video driver: \"%s\", aborting\n"),
                ACE_TEXT (video_driver.c_str ())));
#endif // SDL_USE || SDL2_USE || SDL3_USE
  } // end IF

  // debug info
#if defined (SDL_USE) || defined (SDL2_USE)
  SDL_version version_s;
#endif // SDL_USE || SDL2_USE
#if defined (SDL_USE)
  SDL_SysWMinfo wm_info_s;
  ACE_OS::memset (&wm_info_s, 0, sizeof (SDL_SysWMinfo));
  if (SDL_GetWMInfo (&wm_info_s) <= 0)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_GetWMInfo(): \"%s\", continuing"),
                ACE_TEXT (SDL_GetError ())));
#endif // SDL_USE
#if defined (SDL_USE) || defined (SDL2_USE)
  SDL_VERSION (&version_s);
  std::ostringstream version_number;
  version_number << static_cast<unsigned int> (version_s.major);
  version_number << ACE_TEXT_ALWAYS_CHAR (".");
  version_number << static_cast<unsigned int> (version_s.minor);
  version_number << ACE_TEXT_ALWAYS_CHAR (".");
  version_number << static_cast<unsigned int> (version_s.patch);
#endif // SDL_USE || SDL2_USE

#if defined (SDL_USE)
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("*** wm info (SDL version: %s) ***\nwindow: \t\t%@\nOpenGL context: \t\t%@\n"),
              ACE_TEXT (version_number.str ().c_str ()),
              wm_info_s.window,
              wm_info_s.hglrc));
#else
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("*** wm info (SDL version: %s) ***\nsubsystem:\t\t\t\t%d\ndisplay:\t\t\t\t%@\nwindow:\t\t\t\t\t%u\nfull-screen window:\t\t\t%u\nmanaged input window:\t\t\t%u\nrender display:\t\t\t\t%@\n"),
              ACE_TEXT (version_number.str ().c_str ()),
              wm_info_s.subsystem,
              wm_info_s.info.x11.display,
              wm_info_s.info.x11.window,
              wm_info_s.info.x11.fswindow,
              wm_info_s.info.x11.wmwindow,
              wm_info_s.info.x11.gfxdisplay));
#endif // ACE_WIN32 || ACE_WIN64
#elif defined (SDL2_USE)
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("*** SDL version: %s ***\n"),
              ACE_TEXT (version_number.str ().c_str ())));
#elif defined (SDL3_USE)
  std::ostringstream version_number;
  // version_number << SDL_VERSION;
  version_number << static_cast<unsigned int> (SDL_MAJOR_VERSION);
  version_number << ACE_TEXT_ALWAYS_CHAR (".");
  version_number << static_cast<unsigned int> (SDL_MINOR_VERSION);
  version_number << ACE_TEXT_ALWAYS_CHAR (".");
  version_number << static_cast<unsigned int> (SDL_MICRO_VERSION);
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("*** SDL version: %s ***\n"),
              ACE_TEXT (version_number.str ().c_str ())));
#endif // SDL_USE || SDL2_USE || SDL3_USE

  // step2: set window/icon caption
#if defined (SDL_USE)
  SDL_WM_SetCaption (caption_in.c_str (),  // window caption
                     caption_in.c_str ()); // icon caption
#endif // SDL_USE

  myVideoPreInitialized = true;

  return true;
}

#if defined (SDL_USE)
bool
RPG_Graphics_SDL_Tools::initializeVideo (const struct RPG_Graphics_SDL_VideoConfiguration& configuration_in,
                                         const std::string& caption_in,
                                         SDL_Surface*& windowSurface_out,
                                         bool initWindow_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_SDL_Tools::initializeVideo"));

  // init return value(s)
  windowSurface_out = NULL;

  // sanity check
  if (!myVideoPreInitialized)
    if (!preInitializeVideo (configuration_in,
                             caption_in))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to RPG_Graphics_SDL_Tools::preInitVideo(), aborting\n")));
      return false;
    } // end IF
  ACE_ASSERT (myVideoPreInitialized);

  // set window icon
  RPG_Graphics_GraphicTypeUnion type;
  type.discriminator = RPG_Graphics_GraphicTypeUnion::IMAGE;
  type.image = IMAGE_WM_ICON;
  RPG_Graphics_t icon_graphic = RPG_GRAPHICS_DICTIONARY_SINGLETON::instance()->get(type);
  ACE_ASSERT(icon_graphic.type.image == IMAGE_WM_ICON);
  std::string path = RPG_Graphics_Common_Tools::getGraphicsDirectory();
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path += ACE_TEXT_ALWAYS_CHAR(RPG_GRAPHICS_TILE_IMAGES_SUB);
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path += icon_graphic.file;
  SDL_Surface* icon_image = NULL;
  icon_image = RPG_Graphics_Surface::load (path,   // graphics file
                                           false); // don't convert to display format (no screen yet !)
  if (!icon_image)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to RPG_Graphics_Common_Tools::loadFile(\"%s\"), aborting\n"),
                ACE_TEXT (path.c_str ())));
    return false;
  } // end IF
  SDL_WM_SetIcon (icon_image, // surface
                  NULL);      // mask (--> everything)

  // step3: initialize screen
  if (initWindow_in)
  {
    windowSurface_out = RPG_Graphics_SDL_Tools::initializeScreen (configuration_in);
    if (!windowSurface_out)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to RPG_Graphics_SDL_Tools::initScreen(%d,%d,%d), aborting\n"),
                  configuration_in.screen_width,
                  configuration_in.screen_height,
                  configuration_in.screen_colordepth));
      return false;
    } // end IF
  } // end IF

  return true;
}

SDL_Surface*
RPG_Graphics_SDL_Tools::initializeScreen (const struct RPG_Graphics_SDL_VideoConfiguration& configuration_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_SDL_Tools::initScreen"));

  // initialize return value
  SDL_Surface* result = NULL;

  // sanity check
  char driver[BUFSIZ];
  if (!SDL_VideoDriverName (driver,
                            sizeof (char[BUFSIZ])))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_VideoDriverName(): \"%s\", aborting\n"),
                ACE_TEXT (SDL_GetError ())));
    return result;
  } // end IF

  // retrieve/list "best" available video mode
  const SDL_VideoInfo* video_info_p = SDL_GetVideoInfo ();
  if (video_info_p)
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("*** video capabilities (driver: \"%s\") ***\nHW surfaces:\t\t\t\t\"%s\"\nwindow manager:\t\t\t\t\"%s\"\naccelerated HW --> HW [blits/colorkey/alpha]:\t\"%s\"/\"%s\"/\"%s\"\naccelerated SW --> HW [blits/colorkey/alpha]:\t\"%s\"/\"%s\"/\"%s\"\ncolor fills accelerated:\t\t\"%s\"\nvideo memory:\t\t\t\t%d kBytes\n*** (suggested) video mode ***\npalette:\t\t\t\t%@\nbits[bytes]/pixel:\t\t\t%d[%d]\nmask[RGBA]:\t\t\t\t%x %x %x %x\nshift[RGBA]:\t\t\t\t%d %d %d %d\nloss[RGBA]:\t\t\t\t%d %d %d %d\ntransparent colorkey:\t\t\t%d\noverall surface alpha:\t\t\t%d\n"),
                ACE_TEXT (driver),
                (video_info_p->hw_available ? ACE_TEXT ("yes") : ACE_TEXT ("no")),
                (video_info_p->wm_available ? ACE_TEXT ("yes") : ACE_TEXT ("no")),
                (video_info_p->blit_hw ? ACE_TEXT ("yes") : ACE_TEXT ("no")),
                (video_info_p->blit_hw_CC ? ACE_TEXT ("yes") : ACE_TEXT ("no")),
                (video_info_p->blit_hw_A ? ACE_TEXT ("yes") : ACE_TEXT ("no")),
                (video_info_p->blit_sw ? ACE_TEXT ("yes") : ACE_TEXT ("no")),
                (video_info_p->blit_sw_CC ? ACE_TEXT ("yes") : ACE_TEXT ("no")),
                (video_info_p->blit_sw_A ? ACE_TEXT ("yes") : ACE_TEXT ("no")),
                (video_info_p->blit_fill ? ACE_TEXT ("yes") : ACE_TEXT ("no")),
                video_info_p->video_mem,
                video_info_p->vfmt->palette,
                video_info_p->vfmt->BitsPerPixel,
                video_info_p->vfmt->BytesPerPixel,
                video_info_p->vfmt->Rmask, video_info_p->vfmt->Gmask, video_info_p->vfmt->Bmask, video_info_p->vfmt->Amask,
                video_info_p->vfmt->Rshift, video_info_p->vfmt->Gshift, video_info_p->vfmt->Bshift, video_info_p->vfmt->Ashift,
                video_info_p->vfmt->Rloss, video_info_p->vfmt->Gloss, video_info_p->vfmt->Bloss, video_info_p->vfmt->Aloss,
                video_info_p->vfmt->colorkey,
                static_cast<int> (video_info_p->vfmt->alpha)));

  // set surface flags
  RPG_Graphics_Surface::SDL_surface_flags =
      ((video_info_p && video_info_p->hw_available ? SDL_HWSURFACE : SDL_SWSURFACE) |
       SDL_SRCCOLORKEY | // "This flag turns on color keying for blits from this
                         // surface. ...Use SDL_SetColorKey to set or clear this
                         // flag after surface creation."
       SDL_SRCALPHA);    // "This flag turns on alpha-blending for blits from
                         // this surface. ...Use SDL_SetAlpha to set or clear
                         // this flag after surface creation."
  Uint32 SDL_surface_flags =
      ((video_info_p && video_info_p->hw_available ? SDL_HWSURFACE : SDL_SWSURFACE) |
       SDL_ASYNCBLIT | // "...will usually slow down blitting on single CPU
                       // machines, but may provide a speed increase on SMP
                       // systems..."
       SDL_ANYFORMAT | // "...Allow any video depth/pixel-format..."
       SDL_HWPALETTE | // "...Surface has exclusive palette..."
       (configuration_in.double_buffer ? SDL_DOUBLEBUF                  : 0) |
       (configuration_in.use_OpenGL    ? (SDL_OPENGL | SDL_OPENGLBLIT)  : 0) |
       // *TODO*: implement SDL_RESIZABLE ?
       (configuration_in.full_screen   ? (SDL_FULLSCREEN | SDL_NOFRAME) : 0));
  // get available fullscreen/hardware/... modes
  struct SDL_Rect** modes = SDL_ListModes (NULL,               // use same as videoInfo
                                           SDL_surface_flags); // surface flags
  // --> any valid modes available ?
  if (modes == NULL)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("no available resolutions (flags: 0x%x) --> change settings, continuing\n"),
                SDL_surface_flags));
    goto continue_;
  } // end IF
  else if (modes == reinterpret_cast<struct SDL_Rect**> (-1))
  {
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("all resolutions available (flags: 0x%x)...\n"),
                SDL_surface_flags));
  }
  else
  {
    // print valid modes
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("*** available resolutions (flags: 0x%x) ***\n"),
                SDL_surface_flags));
    for (unsigned int i = 0;
         modes[i];
         i++)
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("[%d]: %d x %d\n"),
                  i + 1,
                  modes[i]->w,
                  modes[i]->h));
  } // end ELSE

continue_:
  // check to see whether the requested mode is possible
  int suggested_bpp = SDL_VideoModeOK (configuration_in.screen_width,
                                       configuration_in.screen_height,
                                       configuration_in.screen_colordepth,
                                       SDL_surface_flags);
  switch (suggested_bpp)
  {
    case 0:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("requested mode (width: %d, height: %d, depth: %d, flags: 0x%x) not supported --> change settings, aborting\n"),
                  configuration_in.screen_width,
                  configuration_in.screen_height,
                  configuration_in.screen_colordepth,
                  SDL_surface_flags));
      return NULL;
    }
    default:
    {
      if (suggested_bpp != configuration_in.screen_colordepth)
      {
        ACE_DEBUG ((LM_WARNING,
                    ACE_TEXT ("using suggested color depth: %d...\n"),
                    suggested_bpp));
      } // end IF

      break;
    }
  } // end SWITCH

  // init OpenGL flags
  if (configuration_in.use_OpenGL)
  {
    int failed = 0;
    int return_value = SDL_GL_SetAttribute (SDL_GL_RED_SIZE, 5);
    failed |= return_value;
    if (return_value)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to SDL_GL_SetAttribute(%d, %d): \"%s\", continuing\n"),
                  SDL_GL_RED_SIZE, 5,
                  ACE_TEXT (SDL_GetError ())));
    return_value = SDL_GL_SetAttribute (SDL_GL_GREEN_SIZE, 5);
    failed |= return_value;
    if (return_value)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to SDL_GL_SetAttribute(%d, %d): \"%s\", continuing\n"),
                  SDL_GL_GREEN_SIZE, 5,
                  ACE_TEXT (SDL_GetError ())));
    return_value = SDL_GL_SetAttribute (SDL_GL_BLUE_SIZE, 5);
    failed |= return_value;
    if (return_value)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to SDL_GL_SetAttribute(%d, %d): \"%s\", continuing\n"),
                  SDL_GL_BLUE_SIZE, 5,
                  ACE_TEXT (SDL_GetError ())));

//    return_value = SDL_GL_SetAttribute (SDL_GL_ALPHA_SIZE, 0);
//    failed |= return_value;
//    if (return_value)
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to SDL_GL_SetAttribute(%d, %d): \"%s\", continuing\n"),
//                  SDL_GL_ALPHA_SIZE, 0,
//                  ACE_TEXT (SDL_GetError ())));

//    return_value = SDL_GL_SetAttribute (SDL_GL_BUFFER_SIZE, 0);
//    failed |= return_value;
//    if (return_value)
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to SDL_GL_SetAttribute(%d, %d): \"%s\", continuing\n"),
//                  SDL_GL_BUFFER_SIZE, 0,
//                  ACE_TEXT (SDL_GetError ())));

    return_value = SDL_GL_SetAttribute (SDL_GL_DOUBLEBUFFER,
                                        (configuration_in.double_buffer ? 1 : 0));
    failed |= return_value;
    if (return_value)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to SDL_GL_SetAttribute(%d, %d): \"%s\", continuing\n"),
                  SDL_GL_DOUBLEBUFFER,
                  (configuration_in.double_buffer ? 1 : 0),
                  ACE_TEXT (SDL_GetError ())));

//    return_value = SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
//    failed |= return_value;
//    if (return_value)
//      ACE_DEBUG((LM_ERROR,
//                 ACE_TEXT("failed to SDL_GL_SetAttribute(%d, %d): \"%s\", continuing\n"),
//                 SDL_GL_DEPTH_SIZE, 16,
//                 ACE_TEXT(SDL_GetError())));

//    return_value = SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0);
//    failed |= return_value;
//    if (return_value)
//      ACE_DEBUG((LM_ERROR,
//                 ACE_TEXT("failed to SDL_GL_SetAttribute(%d, %d): \"%s\", continuing\n"),
//                 SDL_GL_STENCIL_SIZE, 0,
//                 ACE_TEXT(SDL_GetError())));

//    return_value = SDL_GL_SetAttribute(SDL_GL_ACCUM_RED_SIZE, 0);
//    failed |= return_value;
//    if (return_value)
//      ACE_DEBUG((LM_ERROR,
//                 ACE_TEXT("failed to SDL_GL_SetAttribute(%d, %d): \"%s\", continuing\n"),
//                 SDL_GL_ACCUM_RED_SIZE, 0,
//                 ACE_TEXT(SDL_GetError())));
//    return_value = SDL_GL_SetAttribute(SDL_GL_ACCUM_GREEN_SIZE, 0);
//    failed |= return_value;
//    if (return_value)
//      ACE_DEBUG((LM_ERROR,
//                 ACE_TEXT("failed to SDL_GL_SetAttribute(%d, %d): \"%s\", continuing\n"),
//                 SDL_GL_ACCUM_GREEN_SIZE, 0,
//                 ACE_TEXT(SDL_GetError())));
//    return_value = SDL_GL_SetAttribute(SDL_GL_ACCUM_BLUE_SIZE, 0);
//    failed |= return_value;
//    if (return_value)
//      ACE_DEBUG((LM_ERROR,
//                 ACE_TEXT("failed to SDL_GL_SetAttribute(%d, %d): \"%s\", continuing\n"),
//                 SDL_GL_ACCUM_BLUE_SIZE, 0,
//                 ACE_TEXT(SDL_GetError())));

//    return_value = SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE, 0);
//    failed |= return_value;
//    if (return_value)
//      ACE_DEBUG((LM_ERROR,
//                 ACE_TEXT("failed to SDL_GL_SetAttribute(%d, %d): \"%s\", continuing\n"),
//                 SDL_GL_ACCUM_ALPHA_SIZE, 0,
//                 ACE_TEXT(SDL_GetError())));

//    return_value = SDL_GL_SetAttribute(SDL_GL_STEREO, 0);
//    failed |= return_value;
//    if (return_value)
//      ACE_DEBUG((LM_ERROR,
//                 ACE_TEXT("failed to SDL_GL_SetAttribute(%d, %d): \"%s\", continuing\n"),
//                 SDL_GL_STEREO, 0,
//                 ACE_TEXT(SDL_GetError())));

//    return_value = SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
//    failed |= return_value;
//    if (return_value)
//      ACE_DEBUG((LM_ERROR,
//                 ACE_TEXT("failed to SDL_GL_SetAttribute(%d, %d): \"%s\", continuing\n"),
//                 SDL_GL_MULTISAMPLEBUFFERS, 0,
//                 ACE_TEXT(SDL_GetError())));
//    return_value = SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 0);
//    failed |= return_value;
//    if (return_value)
//      ACE_DEBUG((LM_ERROR,
//                 ACE_TEXT("failed to SDL_GL_SetAttribute(%d, %d): \"%s\", continuing\n"),
//                 SDL_GL_MULTISAMPLESAMPLES, 0,
//                 ACE_TEXT(SDL_GetError())));

    return_value =
        SDL_GL_SetAttribute (SDL_GL_ACCELERATED_VISUAL,
                             (RPG_GRAPHICS_OPENGL_DEF_HW_ACCELERATION ? 1 : 0));
    failed |= return_value;
    if (return_value)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to SDL_GL_SetAttribute(%d, %d): \"%s\", continuing\n"),
                  SDL_GL_ACCELERATED_VISUAL,
                  (RPG_GRAPHICS_OPENGL_DEF_HW_ACCELERATION ? 1 : 0),
                  ACE_TEXT (SDL_GetError ())));

//    return_value = SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1);
//    failed |= return_value;
//    if (return_value)
//      ACE_DEBUG((LM_ERROR,
//                 ACE_TEXT("failed to SDL_GL_SetAttribute(%d, %d): \"%s\", continuing\n"),
//                 SDL_GL_SWAP_CONTROL, 1,
//                 ACE_TEXT(SDL_GetError())));

//    return_value = SDL_GL_SetAttribute(SDL_GL_RETAINED_BACKING, 0);
//    failed |= return_value;
//    if (return_value)
//      ACE_DEBUG((LM_ERROR,
//                 ACE_TEXT("failed to SDL_GL_SetAttribute(%d, %d): \"%s\", continuing\n"),
//                 SDL_GL_RETAINED_BACKING, 0,
//                 ACE_TEXT(SDL_GetError())));

//    return_value = SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 0);
//    failed |= return_value;
//    if (return_value)
//      ACE_DEBUG((LM_ERROR,
//                 ACE_TEXT("failed to SDL_GL_SetAttribute(%d, %d): \"%s\", continuing\n"),
//                 SDL_GL_CONTEXT_MAJOR_VERSION, 0,
//                 ACE_TEXT(SDL_GetError())));
//    return_value = SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
//    failed |= return_value;
//    if (return_value)
//      ACE_DEBUG((LM_ERROR,
//                 ACE_TEXT("failed to SDL_GL_SetAttribute(%d, %d): \"%s\", continuing\n"),
//                 SDL_GL_CONTEXT_MINOR_VERSION, 0,
//                 ACE_TEXT(SDL_GetError())));

//    int context_flags = 0;
//    context_flags =
//        ((configuration_in.debug ? SDL_GL_CONTEXT_DEBUG_FLAG : 0)  |
//         (RPG_GRAPHICS_OPENGL_CONTEXT_FORWARD_COMPATIBLE_FLAG ? SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG
//                                                              : 0) |
//         (RPG_GRAPHICS_OPENGL_CONTEXT_ROBUST_ACCESS_FLAG ? SDL_GL_CONTEXT_ROBUST_ACCESS_FLAG
//                                                         : 0)      |
//         (RPG_GRAPHICS_OPENGL_CONTEXT_RESET_ISOLATION_FLAG ? SDL_GL_CONTEXT_RESET_ISOLATION_FLAG
//                                                           : 0));
//    return_value = SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS,
//                                       context_flags);
//    failed |= return_value;
//    if (return_value)
//      ACE_DEBUG((LM_ERROR,
//                 ACE_TEXT("failed to SDL_GL_SetAttribute(%d, 0x%x): \"%s\", continuing\n"),
//                 SDL_GL_CONTEXT_FLAGS, context_flags,
//                 ACE_TEXT(SDL_GetError())));
//    context_flags =
//        (RPG_GRAPHICS_DEF_OPENGL_CONTEXT_PROFILE_ES ? SDL_GL_CONTEXT_PROFILE_ES
//                                                    : (RPG_GRAPHICS_OPENGL_CONTEXT_FORWARD_COMPATIBLE_FLAG ? SDL_GL_CONTEXT_PROFILE_CORE
//                                                                                                           : SDL_GL_CONTEXT_PROFILE_COMPATIBILITY));
//    return_value = SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, context_flags);
//    failed |= return_value;
//    if (return_value)
//      ACE_DEBUG((LM_ERROR,
//                 ACE_TEXT("failed to SDL_GL_SetAttribute(%d, 0x%x): \"%s\", continuing\n"),
//                 SDL_GL_CONTEXT_PROFILE_MASK, context_flags,
//                 ACE_TEXT(SDL_GetError())));

//    return_value = SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 0);
//    failed |= return_value;
//    if (return_value)
//      ACE_DEBUG((LM_ERROR,
//                 ACE_TEXT("failed to SDL_GL_SetAttribute(%d, %d): \"%s\", continuing\n"),
//                 SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 0,
//                 ACE_TEXT(SDL_GetError())));

//    return_value = SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, 0);
//    failed |= return_value;
//    if (return_value)
//      ACE_DEBUG((LM_ERROR,
//                 ACE_TEXT("failed to SDL_GL_SetAttribute(%d, %d): \"%s\", continuing\n"),
//                 SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, 0,
//                 ACE_TEXT(SDL_GetError())));

//    return_value = SDL_GL_SetAttribute(SDL_GL_CONTEXT_EGL, 0);
//    failed |= return_value;
//    if (return_value)
//      ACE_DEBUG((LM_ERROR,
//                 ACE_TEXT("failed to SDL_GL_SetAttribute(%d, %d): \"%s\", continuing\n"),
//                 SDL_GL_CONTEXT_EGL, 0,
//                 ACE_TEXT(SDL_GetError())));

    if (failed)
      return NULL;
  } // end IF

  // open SDL window
  SDL_Surface* screen = NULL;
  screen = SDL_SetVideoMode (configuration_in.screen_width,
                             configuration_in.screen_height,
                             suggested_bpp,
                             SDL_surface_flags);
  if (!screen)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_SetVideoMode(%d, %d, %d, 0x%x): \"%s\", aborting\n"),
                configuration_in.screen_width,
                configuration_in.screen_height,
                suggested_bpp,
                SDL_surface_flags,
                ACE_TEXT (SDL_GetError ())));
    return NULL;
  } // end IF

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("*** screen flags ***\nsurface:\t\t\t\t%sRAM\nasynch blits:\t\t\t\t\"%s\"\nany video depth/pixel-format:\t\t\"%s\"\nsurface has exclusive palette:\t\t\"%s\"\ndouble-buffered:\t\t\t\"%s\"\nblit uses HW acceleration:\t\t\"%s\"\nblit uses a source color key:\t\t\"%s\"\nsurface is RLE encoded:\t\t\t\"%s\"\nblit uses source alpha blending:\t\"%s\"\nsurface uses preallocated memory:\t\"%s\"\n"),
              ((screen->flags & SDL_HWSURFACE)   ? ACE_TEXT("Video") : ACE_TEXT("")),
              ((screen->flags & SDL_ASYNCBLIT)   ? ACE_TEXT("yes")   : ACE_TEXT("no")),
              ((screen->flags & SDL_ANYFORMAT)   ? ACE_TEXT("yes")   : ACE_TEXT("no")),
              ((screen->flags & SDL_HWPALETTE)   ? ACE_TEXT("yes")   : ACE_TEXT("no")),
              ((screen->flags & SDL_DOUBLEBUF)   ? ACE_TEXT("yes")   : ACE_TEXT("no")),
              ((screen->flags & SDL_HWACCEL)     ? ACE_TEXT("yes")   : ACE_TEXT("no")),
              ((screen->flags & SDL_SRCCOLORKEY) ? ACE_TEXT("yes")   : ACE_TEXT("no")),
              ((screen->flags & SDL_RLEACCEL)    ? ACE_TEXT("yes")   : ACE_TEXT("no")),
              ((screen->flags & SDL_SRCALPHA)    ? ACE_TEXT("yes")   : ACE_TEXT("no")),
              ((screen->flags & SDL_PREALLOC)    ? ACE_TEXT("yes")   : ACE_TEXT("no"))));

  return screen;
}
#elif defined (SDL2_USE) || defined (SDL3_USE)
bool
RPG_Graphics_SDL_Tools::initializeVideo (const struct RPG_Graphics_SDL_VideoConfiguration& configuration_in,
                                         const std::string& caption_in,
                                         SDL_Window*& windowHandle_out,
                                         SDL_Renderer*& rendererHandle_out,
                                         SDL_GLContext& GLContextHandle_out,
                                         bool initializeWindow_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_SDL_Tools::initializeVideo"));

  // initialize return value(s)
  windowHandle_out = NULL;
  rendererHandle_out = NULL;
  GLContextHandle_out = NULL;

  // sanity check
  if (!myVideoPreInitialized)
    if (!RPG_Graphics_SDL_Tools::preInitializeVideo (configuration_in,
                                                     caption_in))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to RPG_Graphics_SDL_Tools::preInitializeVideo(), aborting\n")));
      return false;
    } // end IF
  ACE_ASSERT (myVideoPreInitialized);

  // step3: initialize screen
  if (initializeWindow_in)
  {
    if (!RPG_Graphics_SDL_Tools::initializeScreen (configuration_in,
                                                   caption_in,
                                                   windowHandle_out,
                                                   rendererHandle_out,
                                                   GLContextHandle_out))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to RPG_Graphics_SDL_Tools::initializeScreen(%d,%d,%d), aborting\n"),
                  configuration_in.screen_width,
                  configuration_in.screen_height,
                  configuration_in.screen_colordepth));
      return false;
    } // end IF

    // set window icon
    struct RPG_Graphics_GraphicTypeUnion type;
    type.discriminator = RPG_Graphics_GraphicTypeUnion::IMAGE;
    type.image = IMAGE_WM_ICON;
    RPG_Graphics_t icon_graphic =
      RPG_GRAPHICS_DICTIONARY_SINGLETON::instance ()->get (type);
    ACE_ASSERT (icon_graphic.type.image == IMAGE_WM_ICON);
    std::string path = RPG_Graphics_Common_Tools::getGraphicsDirectory ();
    path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
    path += ACE_TEXT_ALWAYS_CHAR (RPG_GRAPHICS_TILE_IMAGES_SUB);
    path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
    path += icon_graphic.file;
    SDL_Surface* icon_image =
      RPG_Graphics_Surface::load (path,  // graphics file
                                  true); // convert to display format
    if (!icon_image)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT ("failed to RPG_Graphics_Surface::load(\"%s\",true), aborting\n"),
                 ACE_TEXT (path.c_str ())));
      SDL_GL_DeleteContext (GLContextHandle_out); GLContextHandle_out = NULL;
      SDL_DestroyRenderer (rendererHandle_out); rendererHandle_out = NULL;
      SDL_DestroyWindow (windowHandle_out); windowHandle_out = NULL;
      return false;
    } // end IF
    SDL_SetWindowIcon (windowHandle_out, // window handle
                       icon_image);      // surface
  } // end IF

  return true;
}

bool
RPG_Graphics_SDL_Tools::initializeScreen (const struct RPG_Graphics_SDL_VideoConfiguration& configuration_in,
                                          const std::string& caption_in,
                                          SDL_Window*& window_out,
                                          SDL_Renderer*& renderer_out,
                                          SDL_GLContext& GLContext_out)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_SDL_Tools::initializeScreen"));

  // initialize return value(s)
  if (GLContext_out)
  {
    SDL_GL_DeleteContext (GLContext_out); GLContext_out = NULL;
  } // end IF
  if (renderer_out)
  {
    SDL_DestroyRenderer (renderer_out); renderer_out = NULL;
  } // end IF
  if (window_out)
  {
    SDL_DestroyWindow (window_out); window_out = NULL;
  } // end IF

  int result_2 = -1;

  // get available fullscreen/hardware/... modes
#if defined (SDL2_USE)
  int number_of_video_displays_i = SDL_GetNumVideoDisplays ();
  if (number_of_video_displays_i < 0)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_GetNumVideoDisplays(): \"%s\", aborting\n"),
                ACE_TEXT (SDL_GetError ())));
    return false;
  } // end IF
  for (int i = 0;
       i < number_of_video_displays_i;
       i++)
  {
    int number_of_display_modes_i = SDL_GetNumDisplayModes (i);
    if (number_of_display_modes_i < 0)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to SDL_GetNumDisplayModes(%d): \"%s\", aborting\n"),
                  i,
                  ACE_TEXT (SDL_GetError ())));
      return false;
    } // end IF

    // print valid modes
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("*** available resolutions (display: %d) ***\n"),
                i));

    SDL_DisplayMode display_mode_s;
    ACE_OS::memset (&display_mode_s, 0, sizeof (SDL_DisplayMode));
    for (int j = 0;
         j < number_of_display_modes_i;
         j++)
    {
      result_2 = SDL_GetDisplayMode (i, j, &display_mode_s);
      if (result_2 < 0)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to SDL_GetDisplayMode(%u,%u): \"%s\", aborting\n"),
                    i, j,
                    ACE_TEXT (SDL_GetError ())));
        return false;
      } // end IF

      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("[%d]: %u bpp, %d x %d @ %u Hz\n"),
                  j + 1,
                  SDL_BITSPERPIXEL (display_mode_s.format),
                  display_mode_s.w,
                  display_mode_s.h,
                  display_mode_s.refresh_rate));
    } // end FOR
  } // end FOR
#elif defined (SDL3_USE)
  int number_of_video_displays_i = 0;
  SDL_DisplayID* display_ids_a = SDL_GetDisplays (&number_of_video_displays_i);
  if (!display_ids_a)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_GetDisplays(): \"%s\", aborting\n"),
                ACE_TEXT (SDL_GetError ())));
    return false;
  } // end IF
  for (int i = 0;
       i < number_of_video_displays_i;
       i++)
  {
    // print valid modes
    const char* display_name_p = SDL_GetDisplayName (display_ids_a[i]);
    ACE_ASSERT (display_name_p);
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("*** available resolutions (display: %s) ***\n"),
                ACE_TEXT (display_name_p)));

    int number_of_display_modes_i = 0;
    const SDL_DisplayMode** display_modes_a =
      SDL_GetFullscreenDisplayModes (display_ids_a[i],
                                     &number_of_display_modes_i);
    ACE_ASSERT (display_modes_a);
    for (int j = 0;
         j < number_of_display_modes_i;
         j++)
    {
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("[%d]: %u bpp, %d x %d @ %f Hz\n"),
                  j + 1,
                  SDL_BITSPERPIXEL (display_modes_a[j]->format),
                  display_modes_a[j]->w,
                  display_modes_a[j]->h,
                  display_modes_a[j]->refresh_rate));
    } // end FOR
    SDL_free (display_modes_a);
  } // end FOR
  SDL_free (display_ids_a);
#endif // SDL2_USE || SDL3_USE

#if defined (SDL2_USE) || defined (SDL3_USE)
  struct SDL_RendererInfo render_driver_info_s;
#endif // SDL2_USE || SDL3_USE
#if defined (SDL2_USE)
  // select a render driver
  // - use a render driver supporting software rendering
  Uint32 renderer_flags = SDL_RENDERER_SOFTWARE;
  int number_of_render_drivers = SDL_GetNumRenderDrivers ();
  int index = 0;
  while (index < number_of_render_drivers)
  {
    if (SDL_GetRenderDriverInfo (index,
                                 &render_driver_info_s) < 0)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to SDL_GetRenderDriverInfo(%d): \"%s\", aborting\n"),
                  index,
                  ACE_TEXT (SDL_GetError ())));
      return false;
    } // end IF
    if (((render_driver_info_s.flags & renderer_flags) == renderer_flags) &&
        ((render_driver_info_s.flags & SDL_RENDERER_SOFTWARE) == SDL_RENDERER_SOFTWARE))
    {
      // SDL_SetHint (ACE_TEXT_ALWAYS_CHAR (SDL_HINT_RENDER_DRIVER),
      //              render_driver_info_s.name);
      // ACE_DEBUG ((LM_DEBUG,
      //             ACE_TEXT ("pre-selecting \"%s\" render driver\n"),
      //             ACE_TEXT (render_driver_info_s.name)));
      break;
    } // end IF
    ++index;
  } // end WHILE
#elif defined (SDL3_USE)
  SDL_SetHint (ACE_TEXT_ALWAYS_CHAR (SDL_HINT_RENDER_DRIVER),
               ACE_TEXT_ALWAYS_CHAR ("software"));
#endif // SDL2_USE || SDL3_USE

  // open SDL window
  Uint32 flags_i = 0;//SDL_WINDOW_SHOWN;
  if (configuration_in.use_OpenGL)
    flags_i |= SDL_WINDOW_OPENGL;

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
//   SDL_GL_LoadLibrary (NULL);

  int context_flags_i;
  // context_flags_i = SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG |
  //                   SDL_GL_CONTEXT_ROBUST_ACCESS_FLAG      |
  //                   SDL_GL_CONTEXT_RESET_ISOLATION_FLAG;
  context_flags_i = 0;
#if defined (_DEBUG)
  context_flags_i |= SDL_GL_CONTEXT_DEBUG_FLAG;
#endif // _DEBUG
  SDL_GL_SetAttribute (SDL_GL_CONTEXT_FLAGS, context_flags_i);
  // SDL_GL_SetAttribute (SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
#endif // ACE_WIN32 || ACE_WIN64

#if defined (SDL2_USE)
  // result_2 = SDL_CreateWindowAndRenderer (configuration_in.screen_width,
  //                                         configuration_in.screen_height,
  //                                         flags_i,
  //                                         &window_out,
  //                                         &renderer_out);
  window_out = SDL_CreateWindow (caption_in.c_str (),
                                 SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                 configuration_in.screen_width,
                                 configuration_in.screen_height,
                                 flags_i);
  ACE_ASSERT (window_out);
  renderer_out = SDL_CreateSoftwareRenderer (SDL_GetWindowSurface (window_out));
  if (!renderer_out)
  {
    ACE_DEBUG ((LM_ERROR,
               ACE_TEXT ("failed to SDL_CreateSoftwareRenderer(): \"%s\", aborting\n"),
               ACE_TEXT (SDL_GetError ())));
    return false;
  } // end IF
#elif defined (SDL3_USE)
  // result_2 = SDL_CreateWindowAndRenderer (caption_in.c_str (),
  //                                         configuration_in.screen_width,
  //                                         configuration_in.screen_height,
  //                                         flags_i,
  //                                         &window_out,
  //                                         &renderer_out);
  window_out = SDL_CreateWindow (caption_in.c_str (),
                                 configuration_in.screen_width,
                                 configuration_in.screen_height,
                                 flags_i);
  ACE_ASSERT (window_out);
  renderer_out = SDL_CreateSoftwareRenderer (SDL_GetWindowSurface (window_out));
  if (!renderer_out)
  {
    ACE_DEBUG ((LM_ERROR,
               ACE_TEXT ("failed to SDL_CreateSoftwareRenderer(): \"%s\", aborting\n"),
               ACE_TEXT (SDL_GetError ())));
    return false;
  } // end IF
#endif // SDL2_USE || SDL3_USE
  if (!window_out)
  // if (result_2 < 0)
  {
    ACE_DEBUG ((LM_ERROR,
                //ACE_TEXT ("failed to SDL_CreateWindowAndRenderer(%d, %d, 0x%x): \"%s\", aborting\n"),
                ACE_TEXT ("failed to SDL_CreateWindow(%d, %d, 0x%x): \"%s\", aborting\n"),
                configuration_in.screen_width,
                configuration_in.screen_height,
                flags_i,
                ACE_TEXT (SDL_GetError ())));
    return false;
  } // end IF
  ACE_ASSERT (window_out);
  //ACE_ASSERT (window_out && renderer_out);

  if (renderer_out)
  {
    SDL_GetRendererInfo (renderer_out,
                         &render_driver_info_s);
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("using \"%s\" render driver\n"),
                ACE_TEXT (render_driver_info_s.name)));
  } // end IF

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  // GLContext_out = SDL_GL_CreateContext (window_out);
  // if (GLContext_out == NULL)
  // {
  //   ACE_DEBUG ((LM_ERROR,
  //               ACE_TEXT ("failed to SDL_GL_CreateContext(): \"%s\", aborting\n"),
  //               ACE_TEXT (SDL_GetError ())));
  //   SDL_DestroyRenderer (renderer_out); renderer_out = NULL;
  //   SDL_DestroyWindow (window_out); window_out = NULL;
  //   return false;
  // } // end IF
  if (SDL_GL_MakeCurrent (window_out, NULL) < 0)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_GL_MakeCurrent(): \"%s\", aborting\n"),
                ACE_TEXT (SDL_GetError ())));
    SDL_DestroyRenderer (renderer_out); renderer_out = NULL;
    SDL_DestroyWindow (window_out); window_out = NULL;
    return false;
  } // end IF
#endif // ACE_WIN32 || ACE_WIN64

#if defined (SDL3_USE)
  result_2 = SDL_ShowWindow (window_out);
  if (result_2 < 0)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_ShowWindow(%@): \"%s\", aborting\n"),
                window_out,
                ACE_TEXT (SDL_GetError ())));
    SDL_DestroyRenderer (renderer_out); renderer_out = NULL;
    SDL_DestroyWindow (window_out); window_out = NULL;
    return false;
  } // end IF
  result_2 = SDL_RaiseWindow (window_out);
  if (result_2 < 0)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_RaiseWindow(%@): \"%s\", aborting\n"),
                window_out,
                ACE_TEXT (SDL_GetError ())));
    SDL_DestroyRenderer (renderer_out); renderer_out = NULL;
    SDL_DestroyWindow (window_out); window_out = NULL;
    return false;
  } // end IF
#endif // SDL3_USE

  return true;
}
#endif // SDL_USE || SDL2_USE || SDL3_USE

#if defined (SDL_USE)
std::string
RPG_Graphics_SDL_Tools::keyToString (const SDL_keysym& key_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_SDL_Tools::keyToString"));

  std::string result;

  char buffer[RPG_GRAPHICS_SDL_KEYSYM_BUFFER_SIZE];
  result += ACE_TEXT_ALWAYS_CHAR ("scancode: ");
  ACE_OS::sprintf(buffer,
                  ACE_TEXT_ALWAYS_CHAR("0x%02X"),
                  key_in.scancode);
  result += buffer;
  result += ACE_TEXT_ALWAYS_CHAR("\n");
  result += ACE_TEXT_ALWAYS_CHAR ("name: \"");
  result += SDL_GetKeyName(key_in.sym);
  result += ACE_TEXT_ALWAYS_CHAR ("\"\n");
  result += ACE_TEXT_ALWAYS_CHAR ("unicode: ");
  ACE_OS::sprintf (buffer,
                   ACE_TEXT_ALWAYS_CHAR ("0x%04X"),
                   key_in.unicode);
  result += buffer;
  result += ACE_TEXT_ALWAYS_CHAR ("\n");
  result += ACE_TEXT_ALWAYS_CHAR ("modifier(s): ");
  if (key_in.mod == KMOD_NONE)
    result += ACE_TEXT_ALWAYS_CHAR ("N/A");
  else
  {
    if (key_in.mod & KMOD_NUM)
      result += ACE_TEXT_ALWAYS_CHAR ("NUMLOCK ");
    if (key_in.mod & KMOD_CAPS)
      result += ACE_TEXT_ALWAYS_CHAR ("CAPSLOCK ");
    if (key_in.mod & KMOD_LCTRL)
      result += ACE_TEXT_ALWAYS_CHAR ("LCTRL ");
    if (key_in.mod & KMOD_RCTRL)
      result += ACE_TEXT_ALWAYS_CHAR ("RCTRL ");
    if (key_in.mod & KMOD_LSHIFT)
      result += ACE_TEXT_ALWAYS_CHAR ("LSHIFT ");
    if (key_in.mod & KMOD_RSHIFT)
      result += ACE_TEXT_ALWAYS_CHAR ("RSHIFT ");
    if (key_in.mod & KMOD_LALT)
      result += ACE_TEXT_ALWAYS_CHAR ("LALT ");
    if (key_in.mod & KMOD_RALT)
      result += ACE_TEXT_ALWAYS_CHAR ("RALT ");
    //     if (key_in.mod & KMOD_CTRL)
//       result += ACE_TEXT_ALWAYS_CHAR ("CTRL ");
//     if (key_in.mod & KMOD_SHIFT)
//       result += ACE_TEXT_ALWAYS_CHAR ("SHIFT ");
//     if (key_in.mod & KMOD_ALT)
//       result += ACE_TEXT_ALWAYS_CHAR ("ALT ");
  } // end ELSE

  return result;
}
#elif defined (SDL2_USE) || defined (SDL3_USE)
std::string
RPG_Graphics_SDL_Tools::keyToString (const SDL_Keysym& key_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_SDL_Tools::keyToString"));

  std::string result;

  char buffer[RPG_GRAPHICS_SDL_KEYSYM_BUFFER_SIZE];
  result += ACE_TEXT_ALWAYS_CHAR ("scancode: ");
  ACE_OS::sprintf (buffer,
                   ACE_TEXT_ALWAYS_CHAR ("0x%02X"),
                   key_in.scancode);
  result += buffer;
  result += ACE_TEXT_ALWAYS_CHAR ("\n");
  result += ACE_TEXT_ALWAYS_CHAR ("name: \"");
  result += SDL_GetKeyName (key_in.sym);
  result += ACE_TEXT_ALWAYS_CHAR ("\"\n");
  result += ACE_TEXT_ALWAYS_CHAR ("modifier(s): ");
#if defined (SDL2_USE)
  if (key_in.mod == KMOD_NONE)
#elif defined (SDL3_USE)
  if (key_in.mod == SDL_KMOD_NONE)
#endif // SDL2_USE || SDL3_USE
    result += ACE_TEXT_ALWAYS_CHAR ("N/A");
  else
  {
#if defined (SDL2_USE)
    if (key_in.mod & KMOD_NUM)
#elif defined (SDL3_USE)
    if (key_in.mod & SDL_KMOD_NUM)
#endif // SDL2_USE || SDL3_USE
      result += ACE_TEXT_ALWAYS_CHAR ("NUMLOCK ");
#if defined (SDL2_USE)
    if (key_in.mod & KMOD_CAPS)
#elif defined (SDL3_USE)
    if (key_in.mod & SDL_KMOD_CAPS)
#endif // SDL2_USE || SDL3_USE
      result += ACE_TEXT_ALWAYS_CHAR ("CAPSLOCK ");
#if defined (SDL2_USE)
    if (key_in.mod & KMOD_LCTRL)
#elif defined (SDL3_USE)
    if (key_in.mod & SDL_KMOD_LCTRL)
#endif // SDL2_USE || SDL3_USE
      result += ACE_TEXT_ALWAYS_CHAR ("LCTRL ");
#if defined (SDL2_USE)
    if (key_in.mod & KMOD_RCTRL)
#elif defined (SDL3_USE)
    if (key_in.mod & SDL_KMOD_RCTRL)
#endif // SDL2_USE || SDL3_USE
      result += ACE_TEXT_ALWAYS_CHAR ("RCTRL ");
#if defined (SDL2_USE)
    if (key_in.mod & KMOD_LSHIFT)
#elif defined (SDL3_USE)
    if (key_in.mod & SDL_KMOD_LSHIFT)
#endif // SDL2_USE || SDL3_USE
      result += ACE_TEXT_ALWAYS_CHAR ("LSHIFT ");
#if defined (SDL2_USE)
    if (key_in.mod & KMOD_RSHIFT)
#elif defined (SDL3_USE)
    if (key_in.mod & SDL_KMOD_RSHIFT)
#endif // SDL2_USE || SDL3_USE
      result += ACE_TEXT_ALWAYS_CHAR ("RSHIFT ");
#if defined (SDL2_USE)
    if (key_in.mod & KMOD_LALT)
#elif defined (SDL3_USE)
    if (key_in.mod & SDL_KMOD_LALT)
#endif // SDL2_USE || SDL3_USE
      result += ACE_TEXT_ALWAYS_CHAR ("LALT ");
#if defined (SDL2_USE)
    if (key_in.mod & KMOD_RALT)
#elif defined (SDL3_USE)
    if (key_in.mod & SDL_KMOD_RALT)
#endif // SDL2_USE || SDL3_USE
      result += ACE_TEXT_ALWAYS_CHAR ("RALT ");
    //     if (key_in.mod & KMOD_CTRL)
//       result += ACE_TEXT_ALWAYS_CHAR ("CTRL ");
//     if (key_in.mod & KMOD_SHIFT)
//       result += ACE_TEXT_ALWAYS_CHAR ("SHIFT ");
//     if (key_in.mod & KMOD_ALT)
//       result += ACE_TEXT_ALWAYS_CHAR ("ALT ");
  } // end ELSE

  return result;
}
#endif // SDL_USE || SDL2_USE || SDL3_USE

struct SDL_Color
RPG_Graphics_SDL_Tools::colorToSDLColor (Uint32 SDLColor_in,
                                         const struct SDL_PixelFormat& pixelFormat_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_SDL_Tools::colorToSDLColor"));

  // initialize return value
  struct SDL_Color result;
  ACE_OS::memset (&result, 0, sizeof (struct SDL_Color));

  // extract components from the 32-bit color value
  result.r =
          (SDLColor_in & pixelFormat_in.Rmask) >> pixelFormat_in.Rshift;
  result.g =
          (SDLColor_in & pixelFormat_in.Gmask) >> pixelFormat_in.Gshift;
  result.b =
          (SDLColor_in & pixelFormat_in.Bmask) >> pixelFormat_in.Bshift;
  result.a =
          (SDLColor_in & pixelFormat_in.Amask) >> pixelFormat_in.Ashift;

  return result;
}

Uint32
RPG_Graphics_SDL_Tools::getColor (enum RPG_Graphics_ColorName colorName_in,
                                  const struct SDL_PixelFormat& pixelFormat_in,
                                  float blendFactor_in) // opacity
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_SDL_Tools::getColor"));

  // sanity check(s)
  ACE_ASSERT (blendFactor_in >= 0.0f && blendFactor_in <= 1.0f);

  RPG_Graphics_ColorMapConstIterator_t iterator = myColors.find (colorName_in);
  if (iterator == myColors.end ())
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("unknown/invalid color (was: \"%s\"), aborting\n"),
                ACE_TEXT (RPG_Graphics_ColorNameHelper::RPG_Graphics_ColorNameToString (colorName_in).c_str ())));
    return 0;
  } // end IF

  return SDL_MapRGBA (&pixelFormat_in,
                      (*iterator).second.r,
                      (*iterator).second.g,
                      (*iterator).second.b,
                      static_cast<Uint8> ((*iterator).second.a * blendFactor_in));
}

void
RPG_Graphics_SDL_Tools::initializeColors ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_SDL_Tools::initializeColors"));

  myColors.clear ();
  struct RPG_Graphics_ColorRGBA color;
  for (int i = 0;
       i < RPG_GRAPHICS_COLORNAME_MAX;
       i++)
  {
    switch (i)
    {
      case COLOR_BLACK_A0:
        color.a = 0;
        color.r = 0;
        color.g = 0;
        color.b = 0;
        break;
      case COLOR_BLACK_A10:
        color.a = 0x1a;
        color.r = 0;
        color.g = 0;
        color.b = 0;
        break;
      case COLOR_BLACK_A30:
        color.a = 0x4d;
        color.r = 0;
        color.g = 0;
        color.b = 0;
        break;
      case COLOR_BLACK_A50:
        color.a = 0x80;
        color.r = 0;
        color.g = 0;
        color.b = 0;
        break;
      case COLOR_BLACK_A70:
        color.a = 0xb3;
        color.r = 0;
        color.g = 0;
        color.b = 0;
        break;
      case COLOR_BLACK_A90:
        color.a = 0xe7;
        color.r = 0;
        color.g = 0;
        color.b = 0;
        break;
      case COLOR_BLACK:
        color.a = 0xff;
        color.r = 0;
        color.g = 0;
        color.b = 0;
        break;
      case COLOR_GREEN:
        color.a = 0xff;
        color.r = 0x57;
        color.g = 0xff;
        color.b = 0x57;
        break;
      case COLOR_YELLOW:
        color.a = 0xff;
        color.r = 0xff;
        color.g = 0xff;
        color.b = 0x57;
        break;
      case COLOR_ORANGE:
        color.a = 0xff;
        color.r = 0xff;
        color.g = 0xc7;
        color.b = 0x3b;
        break;
      case COLOR_RED:
        color.a = 0xff;
        color.r = 0xff;
        color.g = 0x23;
        color.b = 0x07;
        break;
      case COLOR_GRAY20:
        color.a = 0xff;
        color.r = 0xb7;
        color.g = 0xab;
        color.b = 0xab;
        break;
      case COLOR_GRAY20_A10:
        color.a = 0x1a;
        color.r = 0xb7;
        color.g = 0xab;
        color.b = 0xab;
        break;
      case COLOR_GRAY70:
        color.a = 0xff;
        color.r = 0x53;
        color.g = 0x53;
        color.b = 0x53;
        break;
      case COLOR_GRAY77:
        color.a = 0xff;
        color.r = 0x43;
        color.g = 0x3b;
        color.b = 0x3b;
        break;
      case COLOR_PURPLE44:
        color.a = 0xff;
        color.r = 0x4f;
        color.g = 0x43;
        color.b = 0x6f;
        break;
      case COLOR_LIGHTPINK:
        color.a = 0xff;
        color.r = 0xcf;
        color.g = 0xbb;
        color.b = 0xd3;
        break;
      case COLOR_LIGHTGREEN:
        color.a = 0xff;
        color.r = 0xaa;
        color.g = 0xff;
        color.b = 0xcc;
        break;
      case COLOR_BROWN:
        color.a = 0xff;
        color.r = 0x9b;
        color.g = 0x6f;
        color.b = 0x57;
        break;
      case COLOR_WHITE:
        color.a = 0xff;
        color.r = 0xff;
        color.g = 0xff;
        color.b = 0xff;
        break;
      case COLOR_BLESS_BLUE:
        color.a = 0x60;
        color.r = 0x96;
        color.g = 0xdc;
        color.b = 0xfe;
        break;
      case COLOR_CURSE_RED:
        color.a = 0x50;
        color.r = 0x60;
        color.g = 0;
        color.b = 0;
        break;
      case COLOR_GOLD_SHADE:
        color.a = 0x40;
        color.r = 0xf0;
        color.g = 0xe0;
        color.b = 0x57;
        break;
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("unknown/invalid color (was: %d), returning\n"),
                    i));
        return;
      }
    } // end SWITCH

    myColors[static_cast<enum RPG_Graphics_ColorName> (i)] = color;
  } // end FOR

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("RPG_Graphics_SDL_Tools: initialized colors...\n")));
}

SDL_Rect
RPG_Graphics_SDL_Tools::boundingBox (const struct SDL_Rect& rect1_in,
                                     const struct SDL_Rect& rect2_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_SDL_Tools::boundingBox"));

  // sanity check(s)
  if ((rect1_in.w == 0) || (rect1_in.h == 0))
    return rect2_in;
  else if ((rect2_in.w == 0) || (rect2_in.h == 0))
    return rect1_in;

  // initialize result
  struct SDL_Rect result;
  ACE_OS::memset (&result, 0, sizeof (struct SDL_Rect));

  result.x =
    static_cast<int16_t> ((rect1_in.x < rect2_in.x) ? rect1_in.x
                                                    : rect2_in.x);
  result.y =
    static_cast<int16_t> ((rect1_in.y < rect2_in.y) ? rect1_in.y
                                                    : rect2_in.y);
  result.w =
    static_cast<uint16_t> ((((rect1_in.x + rect1_in.w) >
                             (rect2_in.x + rect2_in.w)) ? (rect1_in.x + rect1_in.w - 1)
                                                        : (rect2_in.x + rect2_in.w - 1)) - result.x) + 1;
  result.h =
    static_cast<uint16_t> ((((rect1_in.y + rect1_in.h) >
                             (rect2_in.y + rect2_in.h)) ? (rect1_in.y + rect1_in.h - 1)
                                                        : (rect2_in.y + rect2_in.h - 1)) - result.y) + 1;

  return result;
}

SDL_Rect
RPG_Graphics_SDL_Tools::intersect (const struct SDL_Rect& rect1_in,
                                   const struct SDL_Rect& rect2_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_SDL_Tools::intersect"));

  // initialize result
  struct SDL_Rect result;
  ACE_OS::memset (&result, 0, sizeof (struct SDL_Rect));

  // *NOTE*: adapted from SDL_surface.c (SDL 1.2.15)
  int Amin, Amax, Bmin, Bmax;

  // horizontal intersection
  Amin = rect1_in.x;
  Amax = Amin + rect1_in.w;
  Bmin = rect2_in.x;
  Bmax = Bmin + rect2_in.w;
  if (Bmin > Amin)
    Amin = Bmin;
  result.x = Amin;
  if (Bmax < Amax)
    Amax = Bmax;
  result.w = ((Amax - Amin > 0) ? (Amax - Amin) : 0);

  // vertical intersection
  Amin = rect1_in.y;
  Amax = Amin + rect1_in.h;
  Bmin = rect2_in.y;
  Bmax = Bmin + rect2_in.h;
  if (Bmin > Amin)
    Amin = Bmin;
  result.y = Amin;
  if (Bmax < Amax)
    Amax = Bmax;
  result.h = ((Amax - Amin > 0) ? (Amax - Amin) : 0);

  // no intersection ? --> clean up
  if (!result.w || !result.h)
    ACE_OS::memset (&result, 0, sizeof(result));

  return result;
}

bool
RPG_Graphics_SDL_Tools::equal (const struct SDL_Rect& rect1_in,
                               const struct SDL_Rect& rect2_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_SDL_Tools::equal"));
  
  // *TODO*: use memcmp ?
  return ((rect1_in.x == rect2_in.x) &&
          (rect1_in.y == rect2_in.y) &&
          (rect1_in.w == rect2_in.w) &&
          (rect1_in.h == rect2_in.h));
}

SDL_Rect
RPG_Graphics_SDL_Tools::difference (const struct SDL_Rect& rect1_in,
                                    const struct SDL_Rect& rect2_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_SDL_Tools::difference"));

  // initialize result
  struct SDL_Rect result;
  ACE_OS::memset (&result, 0, sizeof (struct SDL_Rect));

  // sanity check(s)
  struct SDL_Rect temp = RPG_Graphics_SDL_Tools::intersect (rect1_in, rect2_in);
  if ((temp.x == 0) &&
      (temp.y == 0) &&
      (temp.w == 0) &&
      (temp.h == 0))
    return temp; // no overlap
  else if (RPG_Graphics_SDL_Tools::equal (temp, rect1_in))
    return result; // full overlap

  // partial overlap --> there are exactly 4 valid cases:
  // case1: rect2 overlaps rect1 from left
  // case2: rect2 overlaps rect1 from right
  // case3: rect2 overlaps rect1 from the top
  // case4: rect2 overlaps rect1 from the bottom
  // in all other cases, the result is not rectangular...
  if (temp.w != rect1_in.w)
    if (temp.h == rect1_in.h)
    {
      // --> case 1/2 ?
      if (temp.x == rect1_in.x)
      {
         // case 1
        result.x = (temp.x + temp.w);
        result.y = temp.y;
        result.w = (rect1_in.w - temp.w);
        result.h = temp.h;
      } // end IF
      else if (temp.x < (rect1_in.x + rect1_in.w))
      {
        // case 2
        result.x = rect1_in.x;
        result.y = temp.y;
        result.w = (rect1_in.w - temp.w);
        result.h = temp.h;
      } // end IF
    } // end ELSEIF
  if (temp.h != rect1_in.h)
    if (temp.w == rect1_in.w)
    {
      // case 3/4 ?
      if (temp.y == rect1_in.y)
      {
        // case 3
        result.x = temp.x;
        result.y = (temp.y + temp.h);
        result.w = temp.w;
        result.h = (rect1_in.h - temp.h);
      } // end IF
      else if (temp.y < (rect1_in.y + rect1_in.h))
      {
        // case 4
        result.x = temp.x;
        result.y = rect1_in.y;
        result.w = temp.w;
        result.h = (rect1_in.h - temp.h);
      } // end ELSEIF
    } // end IF

  return result;
}
