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
#include "SDL_gui_defines.h"
#include "SDL_gui_mainwindow.h"
#include "SDL_gui_levelwindow.h"

#include <rpg_map_common_tools.h>

#include <rpg_graphics_defines.h>
#include <rpg_graphics_dictionary.h>
#include <rpg_graphics_surface.h>
#include <rpg_graphics_common_tools.h>
#include <rpg_graphics_SDL_tools.h>

#include <rpg_common_tools.h>
#include <rpg_common_file_tools.h>

#include <rpg_dice.h>
#include <rpg_dice_common_tools.h>

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

#include <ace/ACE.h>
#include <ace/Log_Msg.h>
#include <ace/Get_Opt.h>
#include <ace/High_Res_Timer.h>
#include <ace/Synch.h>

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>

// *NOTE*: need this to import correct PACKAGE_STRING/VERSION/... !
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

enum userMode_t
{
  MODE_RANDOM_IMAGES = 0,
  MODE_LEVEL_MAP,
  //
  MODE_INVALID,
  MODE_MAX
};

struct map_config_t
{
  unsigned long min_room_size; // 0: don't care
  bool          doors;
  bool          corridors;
  unsigned long max_num_doors_per_room;
  bool          maximize_rooms;
  unsigned long num_areas;
  bool          square_rooms;
  unsigned long map_size_x;
  unsigned long map_size_y;
};

// *NOTE* types as used by SDL
struct SDL_video_config_t
{
  int    screen_width;
  int    screen_height;
  int    screen_colordepth; // bits/pixel
//   Uint32 screen_flags;
  bool   fullScreen;
  bool   doubleBuffer;
};

static SDL_Surface*     screen     = NULL;
static ACE_Thread_Mutex hover_lock;
static unsigned long    hover_time = 0;

const bool
do_initVideo(const std::string& graphicsDirectory_in,
             const SDL_video_config_t& config_in)
{
  ACE_TRACE(ACE_TEXT("::do_initVideo"));

  // init SDL Video

  // ***** window/screen setup *****
  // set window caption
  SDL_WM_SetCaption(ACE_TEXT_ALWAYS_CHAR(PACKAGE_STRING),  // window caption
                    ACE_TEXT_ALWAYS_CHAR(PACKAGE_STRING)); // icon caption
  // set window icon
  RPG_Graphics_t icon_graphic = RPG_GRAPHICS_DICTIONARY_SINGLETON::instance()->getGraphic(TYPE_IMAGE_WM_ICON);
  ACE_ASSERT(icon_graphic.type == TYPE_IMAGE_WM_ICON);
  std::string path = graphicsDirectory_in;
  path += ACE_DIRECTORY_SEPARATOR_STR;
  path += icon_graphic.file;
  SDL_Surface* icon_image = NULL;
  icon_image = RPG_Graphics_Surface::load(path,   // file
                                          false); // DON'T convert to display format (no screen yet !)
  if (!icon_image)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Graphics_Surface::load(\"%s\"), aborting\n"),
               path.c_str()));

    return false;
  } // end IF
  SDL_WM_SetIcon(icon_image, // surface
                 NULL);      // mask (--> everything)
  // enable cursor
  SDL_ShowCursor(SDL_ENABLE);

  char driver[MAXPATHLEN];
  if (!SDL_VideoDriverName(driver,
                           sizeof(driver)))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_VideoDriverName(): \"%s\", aborting\n"),
               SDL_GetError()));

    return false;
  } // end IF
  // retrieve/list "best" available video mode
  const SDL_VideoInfo* videoInfo = NULL;
  videoInfo = SDL_GetVideoInfo();
  ACE_ASSERT(videoInfo);
  ACE_DEBUG((LM_INFO,
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
             ACE_static_cast(int, videoInfo->vfmt->alpha)));

  // set surface flags
  Uint32 surface_flags = ((videoInfo->hw_available ? (SDL_HWSURFACE |
                                                      (config_in.doubleBuffer ? SDL_DOUBLEBUF : SDL_ANYFORMAT))
                                                                              : SDL_SWSURFACE) |
//                                                    SDL_ASYNCBLIT | // "...will usually slow down blitting on single CPU machines,
//                                                                    //  but may provide a speed increase on SMP systems..."
                                                      SDL_ANYFORMAT |    // "...Allow any video depth/pixel-format..."
                                                      SDL_HWPALETTE |    // "...Surface has exclusive palette..."
//                                                    (videoConfig_in.doubleBuffer ? SDL_DOUBLEBUF : SDL_ANYFORMAT) |
                                                      (config_in.fullScreen ? (SDL_FULLSCREEN | SDL_NOFRAME)
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

    return false;
  } // end IF
  else if (modes == ACE_reinterpret_cast(SDL_Rect**, -1))
  {
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("ALL resolutions available (flags: %x)...\n"),
               surface_flags));
  }
  else
  {
    // print valid modes
    ACE_DEBUG((LM_INFO,
               ACE_TEXT("*** available resolutions (flags: %x) ***\n"),
               surface_flags));
    for (unsigned int i = 0;
         modes[i];
         i++)
      ACE_DEBUG((LM_INFO,
                 ACE_TEXT("[%d]: %d x %d\n"),
                 i + 1,
                 modes[i]->w,
                 modes[i]->h));
  } // end ELSE

  // check to see whether the requested mode is possible
  int suggested_bpp = SDL_VideoModeOK(config_in.screen_width,
                                      config_in.screen_height,
                                      config_in.screen_colordepth,
                                      surface_flags);
  switch (suggested_bpp)
  {
    case 0:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("requested mode (width: %d, height: %d, depth: %d, flags: %x) not supported --> change settings, aborting\n"),
                 config_in.screen_width,
                 config_in.screen_height,
                 config_in.screen_colordepth,
                 surface_flags));

      return false;
    }
    default:
    {
      if (suggested_bpp != config_in.screen_colordepth)
      {
        ACE_DEBUG((LM_WARNING,
                   ACE_TEXT("using suggested depth: %d...\n"),
                   suggested_bpp));
      } // end IF

      break;
    }
  } // end SWITCH

  // switch to graphics mode... here we go !
  screen = SDL_SetVideoMode(config_in.screen_width,
                            config_in.screen_height,
                            suggested_bpp,
                            surface_flags);
  if (!screen)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_SetVideoMode(%d, %d, %d, %x): \"%s\", aborting\n"),
               config_in.screen_width,
               config_in.screen_height,
               suggested_bpp,
               surface_flags,
               SDL_GetError()));

    return false;
  } // end IF

  ACE_DEBUG((LM_INFO,
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

  return true;
}

Uint32
event_timer_SDL_cb(Uint32 interval_in,
                   void* argument_in)
{
  ACE_TRACE(ACE_TEXT("::event_timer_SDL_cb"));

  unsigned long* current_hovertime_p = ACE_static_cast(unsigned long*, argument_in);
  ACE_ASSERT(current_hovertime_p);

  // synch access
  {
    ACE_Guard<ACE_Thread_Mutex> aGuard(hover_lock);

    *current_hovertime_p += interval_in;
    if (*current_hovertime_p > RPG_GRAPHICS_WINDOW_HOTSPOT_HOVER_DELAY)
    {
      // mouse is hovering --> trigger an event
      SDL_Event event;
      event.type = SDL_GUI_SDL_HOVEREVENT;
      event.user.code = ACE_static_cast(int, *current_hovertime_p);

      if (SDL_PushEvent(&event))
      {
        ACE_DEBUG((LM_ERROR,
                  ACE_TEXT("failed to SDL_PushEvent(): \"%s\", continuing\n"),
                  SDL_GetError()));
      } // end IF
    } // end IF
  } // end lock scope

  // re-schedule
  return interval_in;
}

Uint32
input_timer_SDL_cb(Uint32 interval_in,
                   void* argument_in)
{
  ACE_TRACE(ACE_TEXT("::input_timer_SDL_cb"));

  // create a timer event
  SDL_Event event;
  event.type = SDL_GUI_SDL_TIMEREVENT;
  event.user.data1 = argument_in;

  // push it onto the event queue
  if (SDL_PushEvent(&event))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_PushEvent(): \"%s\", continuing\n"),
               SDL_GetError()));
  } // end IF

  // one-shot timer --> cancel
  return 0;
}

// wait for an input event; stop after timeout_in second(s) (0: wait forever)
void
do_SDL_waitForInput(const unsigned long& timeout_in,
                    SDL_Event& event_out)
{
  ACE_TRACE(ACE_TEXT("::do_SDL_waitForInput"));

  SDL_TimerID timer = NULL;
  if (timeout_in)
    timer = SDL_AddTimer((timeout_in * 1000), // interval (ms)
                         input_timer_SDL_cb,  // timeout callback
                         NULL);               // callback argument
  ACE_ASSERT(timer);
  if (!timer)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_AddTimer(%u): \"%s\", aborting\n"),
               (timeout_in * 1000),
               SDL_GetError()));

    return;
  } // end IF

  // loop until something interesting happens
  do
  {
    if (SDL_WaitEvent(&event_out) != 1)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to SDL_WaitEvent(): \"%s\", aborting\n"),
                 SDL_GetError()));

      // what else can we do ?
      break;
    } // end IF
    if (event_out.type == SDL_KEYDOWN ||
        event_out.type == SDL_KEYUP ||
        event_out.type == SDL_MOUSEMOTION ||
        event_out.type == SDL_MOUSEBUTTONDOWN ||
        event_out.type == SDL_QUIT ||
        event_out.type == SDL_GUI_SDL_TIMEREVENT)
      break;
  } while (true);

  // clean up
  if (timeout_in &&
      (event_out.type != SDL_GUI_SDL_TIMEREVENT))
    if (!SDL_RemoveTimer(timer))
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to SDL_RemoveTimer(): \"%s\", continuing\n"),
                 SDL_GetError()));
    } // end IF
}

void
print_usage(const std::string& programName_in)
{
  ACE_TRACE(ACE_TEXT("::print_usage"));

  std::cout << ACE_TEXT("usage: ") << programName_in << ACE_TEXT(" [OPTIONS]") << std::endl << std::endl;
  std::cout << ACE_TEXT("currently available options:") << std::endl;
  std::cout << ACE_TEXT("-d [DIR] : graphics directory") << std::endl;
  std::cout << ACE_TEXT("-g [FILE]: graphics dictionary (*.xml)") << std::endl;
  std::cout << ACE_TEXT("-l       : generate level map") << std::endl;
  std::cout << ACE_TEXT("-t       : trace information") << std::endl;
  std::cout << ACE_TEXT("-v       : print version information and exit") << std::endl;
  std::cout << ACE_TEXT("-x       : do NOT validate XML") << std::endl;
} // end print_usage

const bool
process_arguments(const int argc_in,
                  ACE_TCHAR* argv_in[], // cannot be const...
                  std::string& directory_out,
                  std::string& filename_out,
                  bool& levelMap_out,
                  bool& traceInformation_out,
                  bool& printVersionAndExit_out,
                  bool& validateXML_out)
{
  ACE_TRACE(ACE_TEXT("::process_arguments"));

  // init results
  directory_out = SDL_GUI_DEF_GRAPHICS_DIRECTORY;
  filename_out = SDL_GUI_DEF_GRAPHICS_DICTIONARY;
  levelMap_out = (SDL_GUI_DEF_MODE == MODE_LEVEL_MAP);
  traceInformation_out = false;
  printVersionAndExit_out = false;
  validateXML_out = true;

  ACE_Get_Opt argumentParser(argc_in,
                             argv_in,
                             ACE_TEXT("d:g:ltvx"));

  int option = 0;
  while ((option = argumentParser()) != EOF)
  {
    switch (option)
    {
      case 'd':
      {
        directory_out = argumentParser.opt_arg();

        break;
      }
      case 'g':
      {
        filename_out = argumentParser.opt_arg();

        break;
      }
      case 'l':
      {
        levelMap_out = true;

        break;
      }
      case 't':
      {
        traceInformation_out = true;

        break;
      }
      case 'v':
      {
        printVersionAndExit_out = true;

        break;
      }
      case 'x':
      {
        validateXML_out = false;

        break;
      }
      // error handling
      case '?':
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("unrecognized option \"%s\", aborting\n"),
                   argumentParser.last_option()));

        return false;
      }
      default:
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("unrecognized option \"%c\", aborting\n"),
                   option));

        return false;
      }
    } // end SWITCH
  } // end WHILE

  return true;
}

void
do_work(const mode_t& mode_in,
        const map_config_t& mapConfig_in,
        const SDL_video_config_t& videoConfig_in,
        const std::string& dictionary_in,
        const std::string& graphicsDirectory_in,
        const unsigned long& cacheSize_in,
        const bool& validateXML_in)
{
  ACE_TRACE(ACE_TEXT("::do_work"));

  // step0: init: random seed, string conversion facilities, ...
  RPG_Dice::init();
  RPG_Dice_Common_Tools::initStringConversionTables();
  RPG_Graphics_Common_Tools::initStringConversionTables();

  // step1a: init graphics dictionary
  try
  {
    RPG_GRAPHICS_DICTIONARY_SINGLETON::instance()->init(dictionary_in,
                                                        validateXML_in);
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("caught exception in RPG_Graphics_Dictionary::init(), aborting\n")));

    return;
  }

  // step1b: init Video
  if (!do_initVideo(graphicsDirectory_in,
                    videoConfig_in))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to initialize video, aborting\n")));

    return;
  } // end IF
  // step1c: init graphics directory, cache, fonts, ...
  RPG_Graphics_Common_Tools::init(graphicsDirectory_in,
                                  cacheSize_in);

  // step2: setup main "window"
  std::string title = SDL_GUI_DEF_GRAPHICS_MAINWINDOW_TITLE;
  SDL_GUI_MainWindow mainWindow(std::make_pair(screen->w, screen->h),  // size
                                SDL_GUI_DEF_GRAPHICS_WINDOWSTYLE_TYPE, // interface elements
                                title,                                 // title (== caption)
                                TYPE_FONT_MAIN_LARGE);                 // title font
  RPG_Graphics_Position_t position = std::make_pair(0, 0);
  mainWindow.draw(screen,
                  position);
  mainWindow.refresh(screen);

  // step3a: setup level
  RPG_Map_FloorPlan_t plan;
  RPG_Map_Positions_t seedPoints;
  RPG_Map_Common_Tools::createFloorPlan(mapConfig_in.map_size_x,
                                        mapConfig_in.map_size_y,
                                        mapConfig_in.num_areas,
                                        mapConfig_in.square_rooms,
                                        mapConfig_in.maximize_rooms,
                                        mapConfig_in.min_room_size,
                                        mapConfig_in.doors,
                                        mapConfig_in.corridors,
                                        true, // *NOTE*: currently, doors fill one position
                                        mapConfig_in.max_num_doors_per_room,
                                        seedPoints,
                                        plan);
  // debug info
  RPG_Map_Common_Tools::displayFloorPlan(plan);

  // step3b: setup style
  RPG_Graphics_FloorStyle floorStyle = FLOORSTYLE_AIR;
  RPG_Graphics_WallStyle wallStyle = WALLSTYLE_BRICK;
  RPG_Graphics_DoorStyle doorStyle = DOORSTYLE_WOOD;

  SDL_Event event;
  bool done = false;
  RPG_Graphics_IWindow* window = NULL;
  bool redraw_map = false;
  switch (mode_in)
  {
    case MODE_RANDOM_IMAGES:
    {
      // step4: show (random) images inside main "window"
      RPG_Graphics_Type type = RPG_GRAPHICS_TYPE_INVALID;
      RPG_Graphics_t graphic;
      RPG_Dice_RollResult_t result;
      SDL_Surface* image = NULL;
      do
      {
        window = NULL;
        redraw_map = false;

        // reset screen
        mainWindow.draw(screen,
                        position);
        mainWindow.refresh(screen);

        result.clear();
        RPG_Dice::generateRandomNumbers(RPG_GRAPHICS_TYPE_MAX,
                                        1,
                                        result);
        type = ACE_static_cast(RPG_Graphics_Type, (result.front() - 1));

        graphic.type = RPG_GRAPHICS_TYPE_INVALID;
        // *NOTE*: cannot load all types (some are fonts, ...)
        // --> retrieve properties from the dictionary
        graphic = RPG_GRAPHICS_DICTIONARY_SINGLETON::instance()->getGraphic(type);
        ACE_ASSERT(graphic.type == type);
        // sanity check
        if ((graphic.category != CATEGORY_INTERFACE) &&
            (graphic.category != CATEGORY_IMAGE) &&
            (graphic.category != CATEGORY_TILE))
          continue;

        image = RPG_Graphics_Common_Tools::loadGraphic(type);
        if (!image)
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("failed to RPG_Graphics_Common_Tools::loadGraphic(\"%s\"), aborting\n"),
                     RPG_Graphics_TypeHelper::RPG_Graphics_TypeToString(type).c_str()));

          return;
        } // end IF

        ACE_DEBUG((LM_DEBUG,
                   ACE_TEXT("showing graphics type \"%s\"...\n"),
                   RPG_Graphics_TypeHelper::RPG_Graphics_TypeToString(type).c_str()));

        RPG_Graphics_Surface::put((screen->w - image->w) / 2, // location x
                                  (screen->h - image->h) / 2, // location y
                                  *image,                     // image
                                  screen);                    // screen
        if (SDL_Flip(screen))
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("failed to SDL_Flip(): \"%s\", aborting\n"),
                     SDL_GetError()));

          return;
        } // end IF

        // step5: wait a little while (max: 3 seconds)
        do
        {
          do_SDL_waitForInput(3,      // second(s)
                              event); // return value: event
          switch (event.type)
          {
            case SDL_KEYDOWN:
            {
              switch (event.key.keysym.sym)
              {
                case SDLK_q:
                {
                  // finished event processing
                  done = true;

                  break;
                } // end IF
                default:
                  break;
              } // end SWITCH

              if (done)
                break;
              // *WARNING*: falls through !
            }
            case SDL_ACTIVEEVENT:
//             case SDL_MOUSEMOTION:
            case SDL_MOUSEBUTTONDOWN:
//             case SDL_GUI_SDL_HOVEREVENT: // hovering...
            {
              // find window
              RPG_Graphics_Position_t mouse_position(0, 0);
              switch (event.type)
              {
                case SDL_MOUSEMOTION:
                  mouse_position = std::make_pair(event.motion.x,
                                                  event.motion.y); break;
                case SDL_MOUSEBUTTONDOWN:
                  mouse_position = std::make_pair(event.button.x,
                                                  event.button.y); break;
                default:
                {
                  int x,y;
                  SDL_GetMouseState(&x, &y);
                  mouse_position = std::make_pair(x,
                                                  y);

                  break;
                }
              } // end SWITCH
              window = mainWindow.getWindow(mouse_position);
              ACE_ASSERT(window);
              try
              {
                window->handleEvent(event,
                                    window,
                                    redraw_map);
              }
              catch (...)
              {
                ACE_DEBUG((LM_ERROR,
                           ACE_TEXT("caught exception in RPG_Graphics_IWindow::handleEvent(), continuing\n")));
              }

              break;
            }
            case SDL_QUIT:
            {
              // finished event processing
              done = true;

              break;
            }
            case SDL_KEYUP:
            case SDL_MOUSEBUTTONUP:
            case SDL_JOYAXISMOTION:
            case SDL_JOYBALLMOTION:
            case SDL_JOYHATMOTION:
            case SDL_JOYBUTTONDOWN:
            case SDL_JOYBUTTONUP:
            case SDL_SYSWMEVENT:
            case SDL_VIDEORESIZE:
            case SDL_VIDEOEXPOSE:
            case SDL_GUI_SDL_TIMEREVENT:
            default:
            {

              break;
            }
          } // end SWITCH
        } while (event.type == SDL_MOUSEMOTION);
      } while (!done);

      break;
    }
    case MODE_LEVEL_MAP:
    {
      // step4: setup level "window"
      // *NOTE*: need to allocate this dynamically so parent window can
      // assume ownership...
      SDL_GUI_LevelWindow* mapWindow = NULL;
      try
      {
        mapWindow = new SDL_GUI_LevelWindow(mainWindow, // parent
                                            floorStyle, // floor style
                                            wallStyle,  // wall style
                                            doorStyle,  // door style
                                            plan);      // map
      }
      catch (...)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to allocate memory(%u): %m, aborting\n"),
                   sizeof(SDL_GUI_LevelWindow)));

        return;
      }
      if (!mapWindow)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("failed to allocate memory(%u): %m, aborting\n"),
                   sizeof(SDL_GUI_LevelWindow)));

        return;
      } // end IF

//       mapWindow->init(floorStyle,
//                         wallStyle,
//                         doorStyle,
//                         plan);

      // refresh screen
      try
      {
        mapWindow->draw(screen,
                        position);
        mapWindow->refresh(screen);
      }
      catch (...)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("caught exception in RPG_Graphics_IWindow::draw()/refresh(), continuing\n")));
      }

      // start timer (triggers hover events)
      SDL_TimerID timer = NULL;
      timer = SDL_AddTimer(SDL_GUI_SDL_EVENT_TIMEOUT, // interval (ms)
                           event_timer_SDL_cb,        // event timer callback
                           &hover_time);              // callback argument

      do
      {
        window = NULL;
        redraw_map = false;

        // step5: process events
        if (SDL_WaitEvent(&event) != 1)
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("failed to SDL_WaitEvent(): \"%s\", aborting\n"),
                     SDL_GetError()));

          return;
        } // end IF

        // if necessary, reset hover_time
        if (event.type != SDL_GUI_SDL_HOVEREVENT)
        {
          // synch access
          ACE_Guard<ACE_Thread_Mutex> aGuard(hover_lock);

          hover_time = 0;
        } // end IF

        switch (event.type)
        {
          case SDL_KEYDOWN:
          {
            switch (event.key.keysym.sym)
            {
              case SDLK_q:
              {
                // finished event processing
                done = true;

                break;
              }
              default:
              {
                break;
              }
            } // end SWITCH

            if (done)
              break; // leave
            // *WARNING*: falls through !
          }
          case SDL_ACTIVEEVENT:
          case SDL_MOUSEMOTION:
          case SDL_MOUSEBUTTONDOWN:
          case SDL_GUI_SDL_HOVEREVENT: // hovering...
          {
            // find window
            RPG_Graphics_Position_t mouse_position(0, 0);
            switch (event.type)
            {
              case SDL_MOUSEMOTION:
                mouse_position = std::make_pair(event.motion.x,
                                                event.motion.y); break;
              case SDL_MOUSEBUTTONDOWN:
                mouse_position = std::make_pair(event.button.x,
                                                event.button.y); break;
              default:
              {
                int x,y;
                SDL_GetMouseState(&x, &y);
                mouse_position = std::make_pair(x,
                                                y);

                break;
              }
            } // end SWITCH
            window = mainWindow.getWindow(mouse_position);
            ACE_ASSERT(window);
            try
            {
              window->handleEvent(event,
                                  window,
                                  redraw_map);
            }
            catch (...)
            {
              ACE_DEBUG((LM_ERROR,
                         ACE_TEXT("caught exception in RPG_Graphics_IWindow::handleEvent(), continuing\n")));
            }

            break;
          }
          case SDL_QUIT:
          {
            // finished event processing
            done = true;

            break;
          }
          case SDL_KEYUP:
          case SDL_MOUSEBUTTONUP:
          case SDL_JOYAXISMOTION:
          case SDL_JOYBALLMOTION:
          case SDL_JOYHATMOTION:
          case SDL_JOYBUTTONDOWN:
          case SDL_JOYBUTTONUP:
          case SDL_SYSWMEVENT:
          case SDL_VIDEORESIZE:
          case SDL_VIDEOEXPOSE:
          case SDL_GUI_SDL_TIMEREVENT:
          default:
          {

            break;
          }
        } // end SWITCH

        if (redraw_map)
        {
          // refresh map
          try
          {
            mapWindow->draw(screen,
                            position);
            mapWindow->refresh(screen);
          }
          catch (...)
          {
            ACE_DEBUG((LM_ERROR,
                       ACE_TEXT("caught exception in RPG_Graphics_IWindow::draw()/refresh(), continuing\n")));
          }
        } // end IF
      } while (!done);

      break;
    }
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("invalid mode (was: %d), aborting\n"),
                 mode_in));

      break;
    }
  } // end SWITCH

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("finished working...\n")));
} // end do_work

void
do_printVersion(const std::string& programName_in)
{
  ACE_TRACE(ACE_TEXT("::do_printVersion"));

  std::cout << programName_in << ACE_TEXT(" : ") << VERSION << std::endl;

  // create version string...
  // *NOTE*: cannot use ACE_VERSION, as it doesn't contain the (potential) beta version
  // number... We need this, as the library soname is compared to this string.
  std::ostringstream version_number;
  if (version_number << ACE::major_version())
  {
    version_number << ACE_TEXT(".");
  } // end IF
  else
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to convert: \"%m\", returning\n")));

    return;
  } // end ELSE
  if (version_number << ACE::minor_version())
  {
    version_number << ACE_TEXT(".");

    if (version_number << ACE::beta_version())
    {

    } // end IF
    else
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to convert: \"%m\", returning\n")));

      return;
    } // end ELSE
  } // end IF
  else
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to convert: \"%m\", returning\n")));

    return;
  } // end ELSE
  std::cout << ACE_TEXT("ACE: ") << version_number.str() << std::endl;
//   std::cout << "ACE: "
//             << ACE_VERSION
//             << std::endl;
}

int
ACE_TMAIN(int argc,
          ACE_TCHAR* argv[])
{
  ACE_TRACE(ACE_TEXT("::main"));

  // step0: init ACE
  // *PORTABILITY*: on Windows, we need to init ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (ACE::init() == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE::init(): \"%m\", aborting\n")));

    return EXIT_FAILURE;
  } // end IF
#endif

  // step1: init
  // step1a set defaults
  std::string graphicsDirectory  = SDL_GUI_DEF_GRAPHICS_DIRECTORY;
  unsigned long cacheSize        = SDL_GUI_DEF_GRAPHICS_CACHESIZE;
  std::string filename           = SDL_GUI_DEF_GRAPHICS_DICTIONARY;
  mode_t mode                    = SDL_GUI_DEF_MODE;
  bool generateLevelMap          = (SDL_GUI_DEF_MODE == MODE_LEVEL_MAP);
  bool traceInformation          = false;
  bool printVersionAndExit       = false;
  bool validateXML               = true;

  // *** map ***
  map_config_t map_config;
  map_config.min_room_size          = SDL_GUI_DEF_MAP_MIN_ROOM_SIZE;
  map_config.doors                  = SDL_GUI_DEF_MAP_DOORS;
  map_config.corridors              = SDL_GUI_DEF_MAP_CORRIDORS;
  map_config.max_num_doors_per_room = SDL_GUI_DEF_MAP_MAX_NUM_DOORS_PER_ROOM;
  map_config.maximize_rooms         = SDL_GUI_DEF_MAP_MAXIMIZE_ROOMS;
  map_config.num_areas              = SDL_GUI_DEF_MAP_NUM_AREAS;
  map_config.square_rooms           = SDL_GUI_DEF_MAP_SQUARE_ROOMS;
  map_config.map_size_x             = SDL_GUI_DEF_MAP_SIZE_X;
  map_config.map_size_y             = SDL_GUI_DEF_MAP_SIZE_Y;

  // *** video ***
  SDL_video_config_t video_config;
  video_config.screen_width      = SDL_GUI_DEF_VIDEO_W;
  video_config.screen_height     = SDL_GUI_DEF_VIDEO_H;
  video_config.screen_colordepth = SDL_GUI_DEF_VIDEO_BPP;
//   video_config.screen_flags      = ;
  video_config.fullScreen        = SDL_GUI_DEF_VIDEO_FULLSCREEN;
  video_config.doubleBuffer      = SDL_GUI_DEF_VIDEO_DOUBLEBUFFER;

  // step1b: parse/process/validate configuration
  if (!(process_arguments(argc,
                          argv,
                          graphicsDirectory,
                          filename,
                          generateLevelMap,
                          traceInformation,
                          printVersionAndExit,
                          validateXML)))
  {
    // make 'em learn...
    print_usage(std::string(ACE::basename(argv[0])));

    // *PORTABILITY*: on Windows, we must fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini() == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE::fini(): \"%m\", continuing\n")));
#endif

    return EXIT_FAILURE;
  } // end IF

  // step1b: validate arguments
  if (filename.empty() ||
      !RPG_Common_File_Tools::isReadable(filename))
  {
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("invalid (XML) filename \"%s\", aborting\n"),
               filename.c_str()));

    // make 'em learn...
    print_usage(std::string(ACE::basename(argv[0])));

    // *PORTABILITY*: on Windows, we must fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini() == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE::fini(): \"%m\", continuing\n")));
#endif

    return EXIT_FAILURE;
  } // end IF
  if (!RPG_Common_File_Tools::isDirectory(graphicsDirectory))
  {
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("invalid graphics directory \"%s\", aborting\n"),
               graphicsDirectory.c_str()));

    // *PORTABILITY*: on Windows, we must fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini() == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE::fini(): \"%m\", continuing\n")));
#endif

    return EXIT_FAILURE;
  } // end IF
  mode = (generateLevelMap ? MODE_LEVEL_MAP : mode);

  // step1c: set correct trace level
  //ACE_Trace::start_tracing();
  if (!traceInformation)
  {
    u_long process_priority_mask = (LM_SHUTDOWN |
                                    //LM_INFO |  // <-- DISABLE trace messages !
                                    //LM_DEBUG |
                                    LM_INFO |
                                    LM_NOTICE |
                                    LM_WARNING |
                                    LM_STARTUP |
                                    LM_ERROR |
                                    LM_CRITICAL |
                                    LM_ALERT |
                                    LM_EMERGENCY);

    // set new mask...
    ACE_LOG_MSG->priority_mask(process_priority_mask,
                               ACE_Log_Msg::PROCESS);

    //ACE_LOG_MSG->stop_tracing();

    // don't go VERBOSE...
    //ACE_LOG_MSG->clr_flags(ACE_Log_Msg::VERBOSE_LITE);
  } // end IF

  // step1d: handle specific program modes
  if (printVersionAndExit)
  {
    do_printVersion(std::string(ACE::basename(argv[0])));

    return EXIT_SUCCESS;
  } // end IF

  // step2: init SDL
  if (SDL_Init(SDL_INIT_TIMER |
               SDL_INIT_VIDEO |
               SDL_INIT_NOPARACHUTE) == -1) // "...Prevents SDL from catching fatal signals..."
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_Init(): \"%s\", aborting\n"),
               SDL_GetError()));

    // *PORTABILITY*: on Windows, we must fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini() == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE::fini(): \"%m\", continuing\n")));
#endif

    return EXIT_FAILURE;
  } // end IF
  // ***** keyboard setup *****
  // enable Unicode translation
  SDL_EnableUNICODE(1);
  // enable key repeat
  SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY,
                      SDL_DEFAULT_REPEAT_INTERVAL);
  // setup font engine
  if (TTF_Init() == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to TTF_Init(): \"%s\", aborting\n"),
               SDL_GetError()));

    SDL_Quit();
    // *PORTABILITY*: on Windows, we must fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini() == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE::fini(): \"%m\", continuing\n")));
#endif

    return EXIT_FAILURE;
  } // end IF
  // enable WM events
  Uint8 event_state = 0;
  event_state = SDL_EventState(SDL_SYSWMEVENT, SDL_QUERY);
  if ((event_state == SDL_IGNORE) ||
      (event_state == SDL_DISABLE))
  {
    SDL_EventState(SDL_SYSWMEVENT, SDL_ENABLE);

    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("enabled SDL_SYSWMEVENT events...\n")));
  } // end IF
  event_state = SDL_EventState(SDL_ACTIVEEVENT, SDL_QUERY);
  if ((event_state == SDL_IGNORE) ||
      (event_state == SDL_DISABLE))
  {
    SDL_EventState(SDL_ACTIVEEVENT, SDL_ENABLE);

    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("enabled SDL_ACTIVEEVENT events...\n")));
  } // end IF

  // step3: do actual work
  ACE_High_Res_Timer timer;
  timer.start();
  do_work(mode,
          map_config,
          video_config,
          filename,
          graphicsDirectory,
          cacheSize,
          validateXML);
  timer.stop();
  // debug info
  std::string working_time_string;
  ACE_Time_Value working_time;
  timer.elapsed_time(working_time);
  RPG_Common_Tools::period2String(working_time,
                                  working_time_string);
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("total working time (h:m:s.us): \"%s\"...\n"),
             working_time_string.c_str()));

  // step4a: fini SDL
  TTF_Quit();
  SDL_Quit();

  // *PORTABILITY*: on Windows, we must fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (ACE::fini() == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE::fini(): \"%m\", aborting\n")));

    return EXIT_FAILURE;
  } // end IF
#endif

  return EXIT_SUCCESS;
} // end main
