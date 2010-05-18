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

#include "rpg_client_common.h"

#include <rpg_common_tools.h>
#include <rpg_common_file_tools.h>

#include <gnome.h>
#include <glade/glade.h>
#include <gtk/gtk.h>

#include <SDL.h>

#include <ace/ACE.h>
#include <ace/Version.h>
#include <ace/Get_Opt.h>
#include <ace/Profile_Timer.h>
#include <ace/Reactor.h>
#include <ace/TP_Reactor.h>
#include <ace/Signal.h>
#include <ace/Sig_Handler.h>
#include <ace/High_Res_Timer.h>
#include <ace/Configuration.h>
#include <ace/Configuration_Import_Export.h>
#include <ace/Thread_Manager.h>

#include <string>
#include <iostream>
#include <sstream>

// *NOTE*: need this to import correct VERSION !
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#define RPG_CLIENT_DEF_INI_FILE                  ACE_TEXT("rpg_client.ini")
#define RPG_CLIENT_CNF_GNOME_APPLICATION_ID      ACE_TEXT_ALWAYS_CHAR("rpg_client")
#define RPG_CLIENT_CNF_CLIENT_SECTION_HEADER     ACE_TEXT("client")
#define RPG_CLIENT_CNF_CONNECTION_SECTION_HEADER ACE_TEXT("connection")
#define RPG_CLIENT_DEF_CLIENT_USES_TP            false
#define RPG_CLIENT_DEF_NUM_TP_THREADS            5
#define RPG_CLIENT_DEF_UI_FILE                   ACE_TEXT("rpg_client.glade")
#define RPG_CLIENT_DEF_VIDEO_W                   1024
#define RPG_CLIENT_DEF_VIDEO_H                   786
#define RPG_CLIENT_DEF_VIDEO_BPP                 32
#define RPG_CLIENT_DEF_VIDEO_FULLSCREEN          false
#define RPG_CLIENT_DEF_VIDEO_DOUBLEBUFFER        false

struct cb_data
{
  std::string bla;
};

// *NOTE* types as used by SDL
struct video_config
{
  int    screen_width;
  int    screen_height;
  int    screen_colordepth; // bits/pixel
  Uint32 screen_flags;
  bool   fullScreen;
  bool   doubleBuffer;
};

static cb_data      userData;
// static GtkBuilder* builder  = NULL;
static GladeXML*    xml     = NULL;
static GtkWidget*   dialog  = NULL;
static SDL_Surface* screen  = NULL;
static int          grp_id  = -1;

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
G_MODULE_EXPORT gint
quit_activated_cb(GtkWidget* widget_in,
                  GdkEvent* event_in,
                  gpointer userData_in)
{
  ACE_TRACE(ACE_TEXT("::quit_activated_cb"));

  ACE_UNUSED_ARG(widget_in);
  ACE_UNUSED_ARG(userData_in);

  // stop reactor
  if (ACE_Reactor::instance()->end_event_loop() == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Reactor::end_event_loop(): \"%m\", continuing\n")));
  } // end IF

  // ... and wait for the reactor worker(s) to join
  ACE_Thread_Manager::instance()->wait_grp(grp_id);

  // no more data will arrive from here on...

  // ...and leave GTK
  gtk_main_quit();

  // this is the "delete-event" handler
  // --> destroy the dialog widget
  return TRUE;
}
#ifdef __cplusplus
}
#endif /* __cplusplus */

int
filter_events(const SDL_Event* event_in)
{
  ACE_TRACE(ACE_TEXT("::filter_events"));

//   static int boycott = 1;

  // sanity check(s)
  ACE_ASSERT(event_in);

  switch (event_in->type)
  {
//     case SDL_QUIT:
//     {
//       if (boycott)
//       {
//         ACE_DEBUG((LM_DEBUG,
//                    ACE_TEXT("filtered quit event -- try again, aborting\n")));
//
//         boycott = 0;
//
//         return 0; // drop it, we've handled it
//       } // end IF
//
//       break;
//     }
    case SDL_MOUSEMOTION:
    {
//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("mouse moved to (%d, %d), aborting\n"),
//                  event_in->motion.x,
//                  event_in->motion.y));

      return 0; // drop it, we've handled it
    }
    default:
    {

      break;
    }
  } // end SWITCH

  return 1;
}

void
do_SDLEventLoop()
{
  ACE_TRACE(ACE_TEXT("::do_SDLEventLoop"));

  SDL_Event event;
  // loop waiting for ESC+Mouse_Button
  while (SDL_WaitEvent(&event) >= 0)
  {
    switch (event.type)
    {
      case SDL_ACTIVEEVENT:
      {
        if (event.active.state & SDL_APPACTIVE)
        {
          if (event.active.gain)
          {
//             ACE_DEBUG((LM_DEBUG,
//                        ACE_TEXT("activated...\n")));
          } // end IF
          else
          {
//             ACE_DEBUG((LM_DEBUG,
//                        ACE_TEXT("iconified...\n")));
          } // end ELSE
        } // end IF

        break;
      }
      case SDL_MOUSEBUTTONDOWN:
      {
        Uint8* keys = NULL;
        keys = SDL_GetKeyState(NULL);
        ACE_ASSERT(keys);
        if (keys[SDLK_ESCAPE] == SDL_PRESSED)
        {
          // simply fall through !
        } // end IF
        else
        {
//           ACE_DEBUG((LM_DEBUG,
//                      ACE_TEXT("mouse button pressed...\n")));

          break;
        } // end ELSE
      }
      case SDL_QUIT:
      {
        // stop reactor
        if (ACE_Reactor::instance()->end_event_loop() == -1)
        {
          ACE_DEBUG((LM_ERROR,
                     ACE_TEXT("failed to ACE_Reactor::end_event_loop(): \"%m\", continuing\n")));
        } // end IF

        // ... and wait for the reactor worker(s) to join
        ACE_Thread_Manager::instance()->wait_grp(grp_id);

        // no more data will arrive from here on...

        ACE_DEBUG((LM_DEBUG,
                   ACE_TEXT("leaving...\n")));

        return;
      }
      default:
      {
        break;
      }
    } // end SWITCH
  } // end WHILE
}

void
print_usage(const std::string& programName_in)
{
  ACE_TRACE(ACE_TEXT("::print_usage"));

  // enable verbatim boolean output
  std::cout.setf(ios::boolalpha);

  std::cout << ACE_TEXT("usage: ") << programName_in << ACE_TEXT(" [OPTIONS]") << std::endl << std::endl;
  std::cout << ACE_TEXT("currently available options:") << std::endl;
  std::cout << ACE_TEXT("-c [FILE]   : config file") << ACE_TEXT(" [") << RPG_CLIENT_DEF_INI_FILE << ACE_TEXT("]") << std::endl;
  std::cout << ACE_TEXT("-l          : log to a file") << ACE_TEXT(" [") << false << ACE_TEXT("]") << std::endl;
  std::cout << ACE_TEXT("-t          : trace information") << ACE_TEXT(" [") << false << ACE_TEXT("]") << std::endl;
  std::cout << ACE_TEXT("-u [FILE]   : UI file") << ACE_TEXT(" [") << RPG_CLIENT_DEF_UI_FILE << ACE_TEXT("]") << std::endl;
  std::cout << ACE_TEXT("-v          : print version information and exit") << ACE_TEXT(" [") << false << ACE_TEXT("]") << std::endl;
  std::cout << ACE_TEXT("-x<[VALUE]> : use thread pool <#threads>") << ACE_TEXT(" [") << RPG_CLIENT_DEF_CLIENT_USES_TP  << ACE_TEXT(" : ") << RPG_CLIENT_DEF_NUM_TP_THREADS << ACE_TEXT("]") << std::endl;
} // end print_usage

const bool
process_arguments(const int argc_in,
                  ACE_TCHAR* argv_in[], // cannot be const...
                  std::string& iniFile_out,
                  bool& logToFile_out,
                  bool& traceInformation_out,
                  std::string& UIfile_out,
                  bool& printVersionAndExit_out,
                  bool& useThreadPool_out,
                  unsigned long& numThreadPoolThreads_out)
{
  ACE_TRACE(ACE_TEXT("::process_arguments"));

  // init results
  iniFile_out              = RPG_CLIENT_DEF_INI_FILE;
  logToFile_out            = false;
  traceInformation_out     = false;
  UIfile_out               = RPG_CLIENT_DEF_UI_FILE;
  printVersionAndExit_out  = false;
  useThreadPool_out        = RPG_CLIENT_DEF_CLIENT_USES_TP;
  numThreadPoolThreads_out = RPG_CLIENT_DEF_NUM_TP_THREADS;

  ACE_Get_Opt argumentParser(argc_in,
                             argv_in,
                             ACE_TEXT("c:ltu:vx::"),
                             1, // skip command name
                             1, // report parsing errors
                             ACE_Get_Opt::PERMUTE_ARGS, // ordering
                             0); // for now, don't use long options

  int option = 0;
  std::stringstream converter;
  while ((option = argumentParser()) != EOF)
  {
    switch (option)
    {
      case 'c':
      {
        iniFile_out = argumentParser.opt_arg();

        break;
      }
      case 'l':
      {
        logToFile_out = true;

        break;
      }
      case 't':
      {
        traceInformation_out = true;

        break;
      }
      case 'u':
      {
        UIfile_out = argumentParser.opt_arg();

        break;
      }
      case 'v':
      {
        printVersionAndExit_out = true;

        break;
      }
      case 'x':
      {
        useThreadPool_out = true;
        converter.clear();
        converter.str(ACE_TEXT_ALWAYS_CHAR(""));
        converter << argumentParser.opt_arg();
        converter >> numThreadPoolThreads_out;

        break;
      }
      // error handling
      case ':':
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("option \"%c\" requires an argument, aborting\n"),
                   argumentParser.opt_opt()));

        return false;
      }
      case '?':
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("unrecognized option \"%s\", aborting\n"),
                   argumentParser.last_option()));

        return false;
      }
      case 0:
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("found long option \"%s\", aborting\n"),
                   argumentParser.long_option()));

        return false;
      }
      default:
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("parse error, aborting\n")));

        return false;
      }
    } // end SWITCH
  } // end WHILE

  return true;
}

const bool
init_threadPool()
{
  ACE_TRACE(ACE_TEXT("::init_threadPool"));

  ACE_TP_Reactor* threadpool_reactor = NULL;
  ACE_NEW_RETURN(threadpool_reactor,
                 ACE_TP_Reactor(),
                 false);
  ACE_Reactor* new_reactor = NULL;
  ACE_NEW_RETURN(new_reactor,
                 ACE_Reactor(threadpool_reactor, 1), // delete in dtor
                 false);
  // make this the "default" reactor...
  ACE_Reactor::instance(new_reactor, 1); // delete in dtor

  return true;
}

static
ACE_THR_FUNC_RETURN
tp_worker_func(void* args_in)
{
  ACE_TRACE(ACE_TEXT("::tp_worker_func"));

  ACE_UNUSED_ARG(args_in);

  while (!ACE_Reactor::event_loop_done())
  {
    // block and wait for an event...
    if (ACE_Reactor::instance()->handle_events(NULL) == -1)
      ACE_ERROR((LM_ERROR,
                 ACE_TEXT("(%t) error handling events: \"%m\"\n")));
  } // end WHILE

  ACE_ERROR((LM_DEBUG,
             ACE_TEXT("(%t) worker leaving...\n")));

  return 0;
}

static
ACE_THR_FUNC_RETURN
reactor_worker_func(void* args_in)
{
  ACE_TRACE(ACE_TEXT("::reactor_worker_func"));

  ACE_UNUSED_ARG(args_in);

  // assume ownership over the reactor...
  ACE_Reactor::instance()->owner(ACE_OS::thr_self(),
                                 NULL);

  if (ACE_Reactor::instance()->run_reactor_event_loop() == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Reactor::run_reactor_event_loop(): \"%m\", aborting\n")));
  } // end IF

  ACE_ERROR((LM_DEBUG,
             ACE_TEXT("(%t) worker leaving...\n")));

  return 0;
}

void
do_GUI(const std::string& UIfile_in,
       const cb_data& userData_in,
       const video_config& videoConfig_in)
{
  ACE_TRACE(ACE_TEXT("::do_GUI"));

  // sanity check(s)
  if (!RPG_Common_File_Tools::isReadable(UIfile_in.c_str()))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("UI definition file \"%s\" doesn't exist, aborting\n"),
               UIfile_in.c_str()));

    return;
  } // end IF
  if (xml)
  {
    // clean up
    g_object_unref(G_OBJECT(xml));
    xml = NULL;
  } // end IF
  if (dialog)
  {
    // clean up
    gtk_widget_destroy(dialog);
    dialog = NULL;
  } // end IF

//   // step1: load widget tree
//   xml = glade_xml_new(UIfile_in.c_str(), // definition file
//                       NULL,              // root widget --> construct all
//                       NULL);             // domain
//   ACE_ASSERT(xml);
//   if (!xml)
//   {
//     ACE_DEBUG((LM_ERROR,
//                ACE_TEXT("failed to glade_xml_new(): \"%m\", aborting\n")));
//
//     return;
//   } // end IF
//
//   // step2: auto-connect signals/slots
// //   glade_xml_signal_autoconnect(xml);
//
//   // step3: retrieve dialog handle
//   dialog = GTK_WIDGET(glade_xml_get_widget(xml,
//                                            ACE_TEXT_ALWAYS_CHAR("dialog")));
//   ACE_ASSERT(dialog);
//   if (!dialog)
//   {
//     ACE_DEBUG((LM_ERROR,
//                ACE_TEXT("failed to glade_xml_get_widget(\"dialog\"): \"%m\", aborting\n")));
//
//     // clean up
//     g_object_unref(G_OBJECT(xml));
//     xml = NULL;
//
//     return;
//   } // end IF
//
//   // step4: connect default signals
//   g_signal_connect(dialog,
//                    ACE_TEXT_ALWAYS_CHAR("destroy"),
//                    G_CALLBACK(gtk_widget_destroyed),
//                    &dialog);
//
//   GtkButton* button = NULL;
//   button = GTK_BUTTON(glade_xml_get_widget(xml,
//                                            ACE_TEXT_ALWAYS_CHAR("quit")));
//   ACE_ASSERT(button);
//   g_signal_connect(button,
//                    ACE_TEXT_ALWAYS_CHAR("clicked"),
//                    G_CALLBACK(quit_activated_cb),
//                    &ACE_const_cast(cb_data&, userData_in));
//
//   // step4a: attach user data
//   glade_xml_signal_connect_data(xml,
//                                 ACE_TEXT_ALWAYS_CHAR("quit_activated_cb"),
//                                 G_CALLBACK(quit_activated_cb),
//                                 &ACE_const_cast(cb_data&, userData_in));

//   // step5: use correct screen
//   if (parentWidget_in)
//     gtk_window_set_screen(GTK_WINDOW(dialog),
//                           gtk_widget_get_screen(ACE_const_cast(GtkWidget*,
//                                                                parentWidget_in)));

//   // step6: draw it
//   gtk_widget_show_all(dialog);

  // init SDL UI handling

  // ***** keyboard setup *****
  // enable Unicode translation
  SDL_EnableUNICODE(1);
  // enable key repeat
  SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY,
                      SDL_DEFAULT_REPEAT_INTERVAL);
  // ignore key events
  SDL_EventState(SDL_KEYDOWN, SDL_IGNORE);
  SDL_EventState(SDL_KEYUP, SDL_IGNORE);

  // filter key, mouse and quit events
  SDL_SetEventFilter(filter_events);

  // ***** window/screen setup *****
  // set window caption
  SDL_WM_SetCaption(ACE_TEXT_ALWAYS_CHAR(PACKAGE_STRING), // title
                    NULL);                                // icon
  // don't show (double) cursor
  SDL_ShowCursor(SDL_DISABLE);

  char driver[MAXPATHLEN];
  ACE_ASSERT(MAXPATHLEN == sizeof(driver));
  if (!SDL_VideoDriverName(driver, sizeof(driver)))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_VideoDriverName(): \"%s\", aborting\n"),
               SDL_GetError()));

    return;
  } // end IF
  // retrieve/list "best" available video mode
  const SDL_VideoInfo* videoInfo = NULL;
  videoInfo = SDL_GetVideoInfo();
  ACE_ASSERT(videoInfo);
  // debug info
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

  // set flags
  Uint32 surface_flags = ((videoInfo->hw_available ? (SDL_HWSURFACE |
                                                      (videoConfig_in.doubleBuffer ? SDL_DOUBLEBUF : SDL_ANYFORMAT))
                                                   : SDL_SWSURFACE) |
//                           SDL_ASYNCBLIT | // "...will usually slow down blitting on single CPU machines,
//                                           //  but may provide a speed increase on SMP systems..."
                          SDL_ANYFORMAT |    // "...Allow any video depth/pixel-format..."
                          SDL_HWPALETTE |    // "...Surface has exclusive palette..."
//                           (videoConfig_in.doubleBuffer ? SDL_DOUBLEBUF : SDL_ANYFORMAT) |
                          (videoConfig_in.fullScreen ? (SDL_FULLSCREEN | SDL_NOFRAME) : SDL_RESIZABLE));
  // get available fullscreen/hardware/... modes
  SDL_Rect** modes = NULL;
  modes = SDL_ListModes(NULL,           // use same as videoInfo
                        surface_flags); // surface flags
  // --> any valid modes available ?
  if (modes == NULL)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("no available resolutions (flags: %x: change your settings), aborting\n"),
               surface_flags));

    return;
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
               ACE_TEXT("*** available resolutions [flags: %x] ***\n"),
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
  int suggested_bpp = SDL_VideoModeOK(videoConfig_in.screen_width,
                                      videoConfig_in.screen_height,
                                      videoConfig_in.screen_colordepth,
                                      surface_flags);
  switch (suggested_bpp)
  {
    case 0:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("requested mode (width: %d, height: %d, depth: %d, flags: %x) not supported (change your settings), aborting\n"),
                 videoConfig_in.screen_width,
                 videoConfig_in.screen_height,
                 videoConfig_in.screen_colordepth,
                 surface_flags));

      return;
    }
    default:
    {
      if (suggested_bpp != videoConfig_in.screen_colordepth)
      {
        ACE_DEBUG((LM_DEBUG,
                   ACE_TEXT("suggested depth: %d...\n"),
                   suggested_bpp));
      } // end IF

      break;
    }
  } // end SWITCH

  // switch to graphics mode... here we go !
  screen = SDL_SetVideoMode(videoConfig_in.screen_width,
                            videoConfig_in.screen_height,
                            videoConfig_in.screen_colordepth,
                            surface_flags);
  if (!screen)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_SetVideoMode(%d, %d, %d, %x): \"%s\", aborting\n"),
               videoConfig_in.screen_width,
               videoConfig_in.screen_height,
               videoConfig_in.screen_colordepth,
               surface_flags,
               SDL_GetError()));

    return;
  } // end IF

  // debug info
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

}

void
do_work(const RPG_Client_Config& config_in,
        const bool& useThreadPool_in,
        const unsigned long& numThreadPoolThreads_in,
        const std::string& UIfile_in,
        cb_data& userData_in,
        const video_config& videoConfig_in)
{
  ACE_TRACE(ACE_TEXT("::do_work"));

  // step0: (if necessary) init the TP_Reactor
  if (useThreadPool_in)
  {
    if (!init_threadPool())
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to init_threadPool(), aborting\n")));

      return;
    } // end IF
  } // end IF

  // step1: setup UI
  do_GUI(UIfile_in,    // glade file
         userData_in,  // cb data
         videoConfig_in); // video config
//   ACE_ASSERT(xml);
//   ACE_ASSERT(dialog);

  // step2: setup event loops
  // - perform (signal handling, socket I/O, ...) --> ACE_Reactor
  // - UI events --> GTK main loop

//   // *NOTE*: make sure we generally restart system calls (after e.g. EINTR) for the reactor...
//   ACE_Reactor::instance()->restart(1);

  // step3: dispatch events...
  // *NOTE*: if we use a thread pool, we invoke a different function...
  if (useThreadPool_in)
  {
    // start a (group of) worker(s)...
    grp_id = ACE_Thread_Manager::instance()->spawn_n(numThreadPoolThreads_in,     // # threads
                                                     ::tp_worker_func,            // function
                                                     NULL,                        // argument
                                                     (THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED), // flags
                                                     ACE_DEFAULT_THREAD_PRIORITY, // priority
                                                     -1,                          // group id --> create new
                                                     NULL,                        // task
                                                     NULL,                        // handle(s)
                                                     NULL,                        // stack(s)
                                                     NULL,                        // stack size(s)
                                                     NULL);                       // name(s)
  } // end IF
  else
  {
    // start a worker...
    grp_id = ACE_Thread_Manager::instance()->spawn_n(1,                           // # threads
                                                     ::reactor_worker_func,       // function
                                                     NULL,                        // argument
                                                     (THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED), // flags
                                                     ACE_DEFAULT_THREAD_PRIORITY, // priority
                                                     -1,                          // group id --> create new
                                                     NULL,                        // task
                                                     NULL,                        // handle(s)
                                                     NULL,                        // stack(s)
                                                     NULL,                        // stack size(s)
                                                     NULL);                       // name(s)
  } // end ELSE
  if (grp_id == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Thread_Manager::spawn_n(%u): \"%m\", aborting\n"),
               (useThreadPool_in ? numThreadPoolThreads_in : 1)));

    return;
  } // end IF

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("started group (ID: %u) of %u worker thread(s)...\n"),
             grp_id,
             (useThreadPool_in ? numThreadPoolThreads_in : 1)));

//   // dispatch GTK events from the "main" thread
//   gtk_main();
  // dispatch SDL events from the "main" thread
  do_SDLEventLoop();

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("finished working...\n")));
}

void
do_parseIniFile(const std::string& iniFilename_in,
                RPG_Client_Config& config_out,
                video_config& videoConfig_out)
{
  ACE_TRACE(ACE_TEXT("::do_parseIniFile"));

  // init return value(s)
//   config_out.bla = ACE_TEXT("");
  videoConfig_out.screen_width = RPG_CLIENT_DEF_VIDEO_W;
  videoConfig_out.screen_height = RPG_CLIENT_DEF_VIDEO_H;
  videoConfig_out.screen_colordepth = RPG_CLIENT_DEF_VIDEO_BPP;
  videoConfig_out.fullScreen = RPG_CLIENT_DEF_VIDEO_FULLSCREEN;
  videoConfig_out.doubleBuffer = RPG_CLIENT_DEF_VIDEO_DOUBLEBUFFER;

  ACE_Configuration_Heap config_heap;
  if (config_heap.open())
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("ACE_Configuration_Heap::open failed, returning\n")));

    return;
  } // end IF

  ACE_Ini_ImpExp import(config_heap);
  if (import.import_config(iniFilename_in.c_str()))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("ACE_Ini_ImpExp::import_config(\"%s\") failed, returning\n"),
               iniFilename_in.c_str()));

    return;
  } // end IF

  // find/open "client" section...
  ACE_Configuration_Section_Key section_key;
  if (config_heap.open_section(config_heap.root_section(),
                               RPG_CLIENT_CNF_CLIENT_SECTION_HEADER,
                               0, // MUST exist !
                               section_key) != 0)
  {
    ACE_ERROR((LM_ERROR,
               ACE_TEXT("failed to ACE_Configuration_Heap::open_section(%s), returning\n"),
               RPG_CLIENT_CNF_CLIENT_SECTION_HEADER));

    return;
  } // end IF

  // import values...
  int val_index = 0;
  ACE_TString val_name, val_value;
  ACE_Configuration::VALUETYPE val_type;
  while (config_heap.enumerate_values(section_key,
                                      val_index,
                                      val_name,
                                      val_type) == 0)
  {
    if (config_heap.get_string_value(section_key,
                                     val_name.c_str(),
                                     val_value))
    {
      ACE_ERROR((LM_ERROR,
                 ACE_TEXT("failed to ACE_Configuration_Heap::get_string_value(%s), returning\n"),
                 val_name.c_str()));

      return;
    } // end IF

//     // debug info
//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("enumerated %s, type %d\n"),
//                val_name.c_str(),
//                val_type));

    // *TODO*: move these strings...
    if (val_name == ACE_TEXT("screen_width"))
    {
      videoConfig_out.screen_width = ::atoi(val_value.c_str());
    }
    else if (val_name == ACE_TEXT("screen_height"))
    {
      videoConfig_out.screen_height = ::atoi(val_value.c_str());
    }
    else if (val_name == ACE_TEXT("screen_colordepth"))
    {
      videoConfig_out.screen_colordepth = ::atoi(val_value.c_str());
    }
    else
    {
      ACE_ERROR((LM_ERROR,
                 ACE_TEXT("unexpected key \"%s\", continuing\n"),
                 val_name.c_str()));
    } // end ELSE

    ++val_index;
  } // end WHILE

//   // find/open "connection" section...
//   if (config_heap.open_section(config_heap.root_section(),
//                                RPG_CLIENT_CNF_CONNECTION_SECTION_HEADER,
//                                0, // MUST exist !
//                                section_key) != 0)
//   {
//     ACE_ERROR((LM_ERROR,
//                ACE_TEXT("failed to ACE_Configuration_Heap::open_section(%s), returning\n"),
//                RPG_CLIENT_CNF_CONNECTION_SECTION_HEADER));
//
//     return;
//   } // end IF
//
//   // import values...
//   val_index = 0;
//   while (config_heap.enumerate_values(section_key,
//                                       val_index,
//                                       val_name,
//                                       val_type) == 0)
//   {
//     if (config_heap.get_string_value(section_key,
//                                      val_name.c_str(),
//                                      val_value))
//     {
//       ACE_ERROR((LM_ERROR,
//                  ACE_TEXT("failed to ACE_Configuration_Heap::get_string_value(%s), returning\n"),
//                  val_name.c_str()));
//
//       return;
//     } // end IF
//
// //     // debug info
// //     ACE_DEBUG((LM_DEBUG,
// //                ACE_TEXT("enumerated %s, type %d\n"),
// //                val_name.c_str(),
// //                val_type));
//
//     // *TODO*: move these strings...
//     if (val_name == ACE_TEXT("server"))
//     {
//       serverHostname_out = val_value.c_str();
//     }
//     else if (val_name == ACE_TEXT("port"))
//     {
//       serverPortNumber_out = ::atoi(val_value.c_str());
//     }
//     else if (val_name == ACE_TEXT("password"))
//     {
//       loginOptions_out.password = val_value.c_str();
//     }
//     else if (val_name == ACE_TEXT("nick"))
//     {
//       loginOptions_out.nick = val_value.c_str();
//     }
//     else if (val_name == ACE_TEXT("user"))
//     {
//       loginOptions_out.user.username = val_value.c_str();
//     }
//     else if (val_name == ACE_TEXT("realname"))
//     {
//       loginOptions_out.user.realname = val_value.c_str();
//     }
//     else if (val_name == ACE_TEXT("channel"))
//     {
//       loginOptions_out.channel = val_value.c_str();
//     }
//     else
//     {
//       ACE_ERROR((LM_ERROR,
//                  ACE_TEXT("unexpected key \"%s\", continuing\n"),
//                  val_name.c_str()));
//     } // end ELSE
//
//     ++val_index;
//   } // end WHILE

//   // debug info
//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("imported \"%s\"...\n"),
//              configFilename_in.c_str()));
}

void
do_printVersion(const std::string& programName_in)
{
  ACE_TRACE(ACE_TEXT("::do_printVersion"));

  std::cout << programName_in << ACE_TEXT(" : ") << VERSION << std::endl;

  // create version string...
  // *NOTE*: cannot use ACE_VERSION, as it doesn't contain the (potential) beta version
  // number... We need this, as the library soname is compared to this string.
  std::ostringstream version_number;
  version_number << ACE::major_version();
  version_number << ACE_TEXT(".");
  version_number << ACE::minor_version();
  if (ACE::beta_version())
  {
    version_number << ACE_TEXT(".");
    version_number << ACE::beta_version();
  } // end IF

  std::cout << ACE_TEXT("ACE: ") << version_number.str() << std::endl;
//   std::cout << "ACE: "
//             << ACE_VERSION
//             << std::endl;
}

int
ACE_TMAIN(int argc_in,
          ACE_TCHAR* argv_in[])
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

  // *PROCESS PROFILE*
  ACE_Profile_Timer process_profile;
  // start profile timer...
  process_profile.start();

  // step1 init/validate configuration

  // step1a: process commandline arguments
  std::string iniFile                = RPG_CLIENT_DEF_INI_FILE;
  bool logToFile                     = false;
  bool traceInformation              = false;
  std::string UIfile                 = RPG_CLIENT_DEF_UI_FILE;
  bool printVersionAndExit           = false;
  bool useThreadPool                 = RPG_CLIENT_DEF_CLIENT_USES_TP;
  unsigned long numThreadPoolThreads = RPG_CLIENT_DEF_NUM_TP_THREADS;
  if (!(process_arguments(argc_in,
                          argv_in,
                          iniFile,
                          logToFile,
                          traceInformation,
                          UIfile,
                          printVersionAndExit,
                          useThreadPool,
                          numThreadPoolThreads)))
  {
    // make 'em learn...
    print_usage(std::string(ACE::basename(argv_in[0])));

    return EXIT_FAILURE;
  } // end IF

  // step1b: handle specific program modes
  if (printVersionAndExit)
  {
    do_printVersion(std::string(ACE::basename(argv_in[0])));

    return EXIT_SUCCESS;
  } // end IF

  // step1c: set correct trace level
  //ACE_Trace::start_tracing();
  if (!traceInformation)
  {
    u_long process_priority_mask = (LM_SHUTDOWN |
                                    //LM_TRACE |  // <-- DISABLE trace messages !
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

  // step1d: init configuration object
  RPG_Client_Config config;
//   // step1da: populate config object with default/collected data
//   // ************ connection config data ************
//   config.socketBufferSize = RPG_NET_DEF_SOCK_RECVBUF_SIZE;
//   config.messageAllocator = &messageAllocator;
//   config.defaultBufferSize = RPG_NET_PROTOCOL_DEF_NETWORK_BUFFER_SIZE;
//   // ************ protocol config data **************
//   config.clientPingInterval = 0; // servers do this...
// //   config.loginOptions = userData.loginOptions;
//   // ************ stream config data ****************
//   config.debugParser = debugParser;
//   config.module = &IRChandlerModule;
//   // *WARNING*: set at runtime, by the appropriate connection handler
//   config.sessionID = 0; // (== socket handle !)
//   config.statisticsReportingInterval = 0; // == off
  video_config videoConfig;
  videoConfig.screen_width = RPG_CLIENT_DEF_VIDEO_W;
  videoConfig.screen_height = RPG_CLIENT_DEF_VIDEO_H;
  videoConfig.screen_colordepth = RPG_CLIENT_DEF_VIDEO_BPP;
  videoConfig.fullScreen = RPG_CLIENT_DEF_VIDEO_FULLSCREEN;
  videoConfig.doubleBuffer = RPG_CLIENT_DEF_VIDEO_DOUBLEBUFFER;
  // step1db: parse .ini file (if any)
  if (!iniFile.empty())
    do_parseIniFile(iniFile,
                    config,
                    videoConfig);

  // *TODO*: step1de: init callback user data
  userData.bla = ACE_TEXT("");

  // step2a: init SDL
  if (SDL_Init(SDL_INIT_VIDEO |
               SDL_INIT_AUDIO |
               SDL_INIT_NOPARACHUTE) == -1) // "...Prevents SDL from catching fatal signals..."
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to SDL_Init(): \"%s\", aborting\n"),
               SDL_GetError()));

    return EXIT_FAILURE;
  } // end IF

  // step2b: init GTK / GTK+ / GNOME
  gtk_init(&argc_in,
           &argv_in);
//   GnomeClient* gnomeSession = NULL;
//   gnomeSession = gnome_client_new();
//   ACE_ASSERT(gnomeSession);
//   gnome_client_set_program(gnomeSession, ACE::basename(argv_in[0]));
  GnomeProgram* gnomeProgram = NULL;
  gnomeProgram = gnome_program_init(RPG_CLIENT_CNF_GNOME_APPLICATION_ID, // app ID
                                    ACE_TEXT_ALWAYS_CHAR(VERSION),       // version
                                    LIBGNOMEUI_MODULE,                   // module info
                                    argc_in,                             // cmdline
                                    argv_in,                             // cmdline
                                    NULL);                               // property name(s)
  ACE_ASSERT(gnomeProgram);

  ACE_High_Res_Timer timer;
  timer.start();
  // step3: do actual work
  do_work(config,
          useThreadPool,
          numThreadPoolThreads,
          UIfile,
          userData,
          videoConfig);
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

  // step4: clean up
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

  // stop profile timer...
  process_profile.stop();

  // only process profile left to do...
  ACE_Profile_Timer::ACE_Elapsed_Time elapsed_time;
  elapsed_time.real_time = 0.0;
  elapsed_time.user_time = 0.0;
  elapsed_time.system_time = 0.0;
  if (process_profile.elapsed_time(elapsed_time) == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Profile_Timer::elapsed_time: \"%m\", aborting\n")));

    return EXIT_FAILURE;
  } // end IF
  ACE_Profile_Timer::Rusage elapsed_rusage;
  ACE_OS::memset(&elapsed_rusage,
                 0,
                 sizeof(ACE_Profile_Timer::Rusage));
  process_profile.elapsed_rusage(elapsed_rusage);
  ACE_Time_Value user_time(elapsed_rusage.ru_utime);
  ACE_Time_Value system_time(elapsed_rusage.ru_stime);
  std::string user_time_string;
  std::string system_time_string;
  RPG_Common_Tools::period2String(user_time,
                                  user_time_string);
  RPG_Common_Tools::period2String(system_time,
                                  system_time_string);

  // debug info
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT(" --> Process Profile <--\nreal time = %A seconds\nuser time = %A seconds\nsystem time = %A seconds\n --> Resource Usage <--\nuser time used: %s\nsystem time used: %s\nmaximum resident set size = %d\nintegral shared memory size = %d\nintegral unshared data size = %d\nintegral unshared stack size = %d\npage reclaims = %d\npage faults = %d\nswaps = %d\nblock input operations = %d\nblock output operations = %d\nmessages sent = %d\nmessages received = %d\nsignals received = %d\nvoluntary context switches = %d\ninvoluntary context switches = %d\n"),
             elapsed_time.real_time,
             elapsed_time.user_time,
             elapsed_time.system_time,
             user_time_string.c_str(),
             system_time_string.c_str(),
             elapsed_rusage.ru_maxrss,
             elapsed_rusage.ru_ixrss,
             elapsed_rusage.ru_idrss,
             elapsed_rusage.ru_isrss,
             elapsed_rusage.ru_minflt,
             elapsed_rusage.ru_majflt,
             elapsed_rusage.ru_nswap,
             elapsed_rusage.ru_inblock,
             elapsed_rusage.ru_oublock,
             elapsed_rusage.ru_msgsnd,
             elapsed_rusage.ru_msgrcv,
             elapsed_rusage.ru_nsignals,
             elapsed_rusage.ru_nvcsw,
             elapsed_rusage.ru_nivcsw));

  return EXIT_SUCCESS;
} // end main
