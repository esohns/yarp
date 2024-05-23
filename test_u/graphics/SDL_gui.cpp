/***************************************************************************
 *   Copyright (C) 2009 by Erik Sohns                                      *
 *   erik.sohns@web.de                                                     *
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

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>

#define _SDL_main_h
#define SDL_main_h_
#include "SDL.h"
//#include "SDL_syswm.h"
#include "SDL_ttf.h"

#include "ace/ace_wchar.h"
#include "ace/Get_Opt.h"
#include "ace/High_Res_Timer.h"
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "ace/Init_ACE.h"
#endif // ACE_WIN32 || ACE_WIN64
#include "ace/Log_Msg.h"
#include "ace/OS_main.h"

#include "common_event_tools.h"
#include "common_file_tools.h"

#include "common_log_tools.h"

#include "common_timer_manager_common.h"
#include "common_timer_tools.h"

#include "net_client_defines.h"

#if defined (HAVE_CONFIG_H)
#include "rpg_config.h"
#endif // HAVE_CONFIG_H

#include "rpg_dice.h"
#include "rpg_dice_common_tools.h"

#include "rpg_common_defines.h"
#include "rpg_common_file_tools.h"
#include "rpg_common_macros.h"
#include "rpg_common_tools.h"
#include "rpg_common_XML_tools.h"

#include "rpg_magic_common_tools.h"
#include "rpg_magic_defines.h"
#include "rpg_magic_dictionary.h"

#include "rpg_item_common_tools.h"
#include "rpg_item_defines.h"
#include "rpg_item_dictionary.h"

#include "rpg_character_common_tools.h"

#include "rpg_player.h"
#include "rpg_player_common_tools.h"
#include "rpg_player_defines.h"

#include "rpg_monster_defines.h"

#include "rpg_map_common_tools.h"
#include "rpg_map_defines.h"

#include "rpg_engine.h"
#include "rpg_engine_common_tools.h"
#include "rpg_engine_defines.h"
#include "rpg_engine_iclient.h"

#include "rpg_graphics_common.h"
#include "rpg_graphics_common_tools.h"
#include "rpg_graphics_cursor_manager.h"
#include "rpg_graphics_defines.h"
#include "rpg_graphics_dictionary.h"
#include "rpg_graphics_surface.h"
#include "rpg_graphics_SDL_tools.h"

#include "rpg_client_common.h"
#include "rpg_client_common_tools.h"
#include "rpg_client_defines.h"
#include "rpg_client_entity_manager.h"
#include "rpg_client_graphicsmode.h"
#include "rpg_client_iwindow_level.h"

#include "SDL_gui_common.h"
#include "SDL_gui_defines.h"
#include "SDL_gui_mainwindow.h"

// init static(s)
static state_t state;

// ******* WORKAROUND *************
#if defined (ACE_WIN32) || defined (ACE_WIN64)
FILE _iob[] = { *stdin, *stdout, *stderr };
extern "C" FILE * __cdecl __iob_func (void)
{
  return _iob;
}
#endif // ACE_WIN32 || ACE_WIN64
// ********************************

Uint32
event_timer_SDL_cb (Uint32 interval_in,
                    void* argument_in)
{
  RPG_TRACE (ACE_TEXT ("::event_timer_SDL_cb"));

  state_t* state_p = static_cast<state_t*> (argument_in);
  ACE_ASSERT (state_p);

  // synch access
  { ACE_GUARD_RETURN (ACE_Thread_Mutex, aGuard, state_p->hover_lock, interval_in);
    state_p->hover_time += interval_in;
    if (state_p->hover_time > RPG_GRAPHICS_WINDOW_HOTSPOT_HOVER_DELAY)
    {
      // mouse is hovering --> trigger an event
      union SDL_Event sdl_event;
      sdl_event.type = RPG_GRAPHICS_SDL_HOVEREVENT;
      sdl_event.user.code = state_p->hover_time;

      if (SDL_PushEvent (&sdl_event) < 0)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to SDL_PushEvent(): \"%s\", continuing\n"),
                    ACE_TEXT (SDL_GetError ())));
    } // end IF

    state_p->angle += 1.0f;
    if (state_p->angle > 360.0f)
      state_p->angle = 0.0f;
  } // end lock scope

  // re-schedule
  return interval_in;
}

Uint32
input_timer_SDL_cb (Uint32 interval_in,
                    void* argument_in)
{
  RPG_TRACE (ACE_TEXT ("::input_timer_SDL_cb"));

  // create a timer event
  SDL_Event sdl_event;
  sdl_event.type = SDL_GUI_SDL_TIMEREVENT;
  sdl_event.user.data1 = argument_in;

  // push it onto the event queue
  if (SDL_PushEvent (&sdl_event) < 0)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_PushEvent(): \"%s\", continuing\n"),
                ACE_TEXT (SDL_GetError ())));

  // one-shot timer --> cancel
  return 0;
}

// wait for an input event; stop after timeout_in second(s) (0: wait forever)
void
do_SDL_waitForInput (unsigned int timeout_in,
                     union SDL_Event& event_out)
{
  RPG_TRACE (ACE_TEXT ("::do_SDL_waitForInput"));

  SDL_TimerID timer = 0;
  if (timeout_in)
  {
    timer = SDL_AddTimer ((timeout_in * 1000), // interval (ms)
                          input_timer_SDL_cb,  // timeout callback
                          NULL);               // callback argument
    if (!timer)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to SDL_AddTimer(%u): \"%s\", returning\n"),
                  (timeout_in * 1000),
                  ACE_TEXT (SDL_GetError ())));
      return;
    } // end IF
  } // end IF

  // loop until something interesting happens
  do
  {
    if (SDL_WaitEvent (&event_out) != 1)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to SDL_WaitEvent(): \"%s\", aborting\n"),
                  ACE_TEXT (SDL_GetError ())));
      break;
    } // end IF
    if ((event_out.type == SDL_KEYDOWN)         ||
        (event_out.type == SDL_MOUSEBUTTONDOWN) ||
        (event_out.type == SDL_QUIT)            ||
        (event_out.type == SDL_GUI_SDL_TIMEREVENT))
      break;
  } while (true);

  // clean up
  if (timeout_in &&
      (event_out.type != SDL_GUI_SDL_TIMEREVENT))
    if (!SDL_RemoveTimer (timer))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to SDL_RemoveTimer(): \"%s\", continuing\n"),
                  ACE_TEXT (SDL_GetError ())));
}

void
do_printUsage (const std::string& programName_in)
{
  RPG_TRACE (ACE_TEXT ("::do_printUsage"));

  // enable verbatim boolean output
  std::cout.setf (ios::boolalpha);

  std::cout << ACE_TEXT_ALWAYS_CHAR ("usage: ")
            << programName_in
            << ACE_TEXT_ALWAYS_CHAR (" [OPTIONS]")
            << std::endl
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("currently available options:") << std::endl;
  std::string path = RPG_Player_Common_Tools::getPlayerProfilesDirectory ();
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path += RPG_Common_Tools::sanitize (ACE_TEXT_ALWAYS_CHAR (RPG_PLAYER_DEF_NAME));
  path += ACE_TEXT_ALWAYS_CHAR (RPG_PLAYER_PROFILE_EXT);
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-c ([FILE]): player profile (*")
            << ACE_TEXT_ALWAYS_CHAR (RPG_PLAYER_PROFILE_EXT)
            << ACE_TEXT_ALWAYS_CHAR (") [")
            << path
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  std::string data_path =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (yarp_PACKAGE_NAME),
                                                          ACE_TEXT_ALWAYS_CHAR (""),
                                                          ACE_TEXT_ALWAYS_CHAR (RPG_GRAPHICS_SUB_DIRECTORY_STRING),
                                                          false); // data-
  path = data_path;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-d [DIR]   : graphics directory")
            << ACE_TEXT_ALWAYS_CHAR (" [\"")
            << path
            << ACE_TEXT_ALWAYS_CHAR ("\"]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-f         : debug")
            << ACE_TEXT_ALWAYS_CHAR (" [\"")
            << RPG_CLIENT_DEF_DEBUG
            << ACE_TEXT_ALWAYS_CHAR ("\"]")
            << std::endl;
  std::string configuration_path =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (yarp_PACKAGE_NAME),
                                                          ACE_TEXT_ALWAYS_CHAR (""),
                                                          ACE_TEXT_ALWAYS_CHAR (RPG_GRAPHICS_SUB_DIRECTORY_STRING),
                                                          true); // configuration-
  path = configuration_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path += ACE_TEXT_ALWAYS_CHAR (RPG_GRAPHICS_DICTIONARY_FILE);
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-g [FILE]  : graphics dictionary (*.xml)")
            << ACE_TEXT_ALWAYS_CHAR (" [\"")
            << path
            << ACE_TEXT_ALWAYS_CHAR ("\"]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-h [HEIGHT]: height [")
            << SDL_GUI_DEF_VIDEO_H
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  configuration_path =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (yarp_PACKAGE_NAME),
                                                          ACE_TEXT_ALWAYS_CHAR (""),
                                                          ACE_TEXT_ALWAYS_CHAR (RPG_ITEM_SUB_DIRECTORY_STRING),
                                                          true); // configuration-
  path = configuration_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path += ACE_TEXT_ALWAYS_CHAR (RPG_ITEM_DICTIONARY_FILE);
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-i [FILE]  : items dictionary (*.xml)")
            << ACE_TEXT_ALWAYS_CHAR (" [\"")
            << path
            << ACE_TEXT_ALWAYS_CHAR ("\"]")
            << std::endl;
  configuration_path =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (yarp_PACKAGE_NAME),
                                                          ACE_TEXT_ALWAYS_CHAR (""),
                                                          ACE_TEXT_ALWAYS_CHAR (RPG_MAGIC_SUB_DIRECTORY_STRING),
                                                          true);
  path = configuration_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path += ACE_TEXT_ALWAYS_CHAR (RPG_MAGIC_DICTIONARY_FILE);
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-m [FILE]  : magic dictionary (*.xml)")
            << ACE_TEXT_ALWAYS_CHAR (" [\"")
            << path
            << ACE_TEXT_ALWAYS_CHAR ("\"]")
            << std::endl;
  configuration_path =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (yarp_PACKAGE_NAME),
                                                          ACE_TEXT_ALWAYS_CHAR (""),
                                                          ACE_TEXT_ALWAYS_CHAR (RPG_MONSTER_SUB_DIRECTORY_STRING),
                                                          true); // configuration-
  path = configuration_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path += ACE_TEXT_ALWAYS_CHAR (RPG_MONSTER_DICTIONARY_FILE);
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-n [FILE]  : monster dictionary (*.xml)")
            << ACE_TEXT_ALWAYS_CHAR (" [\"")
            << path
            << ACE_TEXT_ALWAYS_CHAR ("\"]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-o         : OpenGL mode")
            << ACE_TEXT_ALWAYS_CHAR (" [")
            << false
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  data_path =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (yarp_PACKAGE_NAME),
                                                          ACE_TEXT_ALWAYS_CHAR (""),
                                                          ACE_TEXT_ALWAYS_CHAR (RPG_ENGINE_SUB_DIRECTORY_STRING),
                                                          false); // data-
  path = data_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path +=
    RPG_Common_Tools::sanitize (ACE_TEXT_ALWAYS_CHAR (RPG_ENGINE_LEVEL_DEF_NAME));
  path += ACE_TEXT_ALWAYS_CHAR (RPG_ENGINE_LEVEL_FILE_EXT);
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-p ([FILE]): map (*")
            << ACE_TEXT_ALWAYS_CHAR (RPG_ENGINE_LEVEL_FILE_EXT)
            << ACE_TEXT_ALWAYS_CHAR (") [")
            << path
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-s         : slideshow mode")
            << ACE_TEXT_ALWAYS_CHAR (" [")
            << (SDL_GUI_DEF_MODE == SDL_GUI_USERMODE_SLIDESHOW)
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-t         : trace information")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-v         : print version information and exit")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-w [WIDTH] : width [")
            << SDL_GUI_DEF_VIDEO_W
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("-x         : do NOT validate XML")
            << ACE_TEXT_ALWAYS_CHAR (" [")
            << !SDL_GUI_DEF_VALIDATE_XML
            << ACE_TEXT_ALWAYS_CHAR ("]")
            << std::endl;
}

bool
do_processArguments (int argc_in,
                     ACE_TCHAR** argv_in, // cannot be const...
                     std::string& magicDictionary_out,
                     int& height_out,
                     std::string& itemsDictionary_out,
                     std::string& monsterDictionary_out,
                     std::string& directory_out,
                     bool& debugMode_out,
                     std::string& graphicsDictionary_out,
                     std::string& entityFile_out,
                     bool& openGLMode_out,
                     bool& logToFile_out,
                     std::string& mapFile_out,
                     bool& slideShowMode_out,
                     bool& traceInformation_out,
                     bool& printVersionAndExit_out,
                     int& width_out,
                     bool& validateXML_out)
{
  RPG_TRACE (ACE_TEXT ("::do_processArguments"));

  // initialize results
  std::string configuration_path =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (yarp_PACKAGE_NAME),
                                                          ACE_TEXT_ALWAYS_CHAR (""),
                                                          ACE_TEXT_ALWAYS_CHAR (RPG_MAGIC_SUB_DIRECTORY_STRING),
                                                          true);
  magicDictionary_out     = configuration_path;
  magicDictionary_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  magicDictionary_out += ACE_TEXT_ALWAYS_CHAR (RPG_MAGIC_DICTIONARY_FILE);

  height_out              = SDL_GUI_DEF_VIDEO_H;

  configuration_path =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (yarp_PACKAGE_NAME),
                                                          ACE_TEXT_ALWAYS_CHAR (""),
                                                          ACE_TEXT_ALWAYS_CHAR (RPG_ITEM_SUB_DIRECTORY_STRING),
                                                          true); // configuration-
  itemsDictionary_out     = configuration_path;
  itemsDictionary_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  itemsDictionary_out += ACE_TEXT_ALWAYS_CHAR (RPG_ITEM_DICTIONARY_FILE);

  configuration_path =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (yarp_PACKAGE_NAME),
                                                          ACE_TEXT_ALWAYS_CHAR (""),
                                                          ACE_TEXT_ALWAYS_CHAR (RPG_MONSTER_SUB_DIRECTORY_STRING),
                                                          true); // configuration-
  monsterDictionary_out   = configuration_path;
  monsterDictionary_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  monsterDictionary_out += ACE_TEXT_ALWAYS_CHAR (RPG_MONSTER_DICTIONARY_FILE);

  debugMode_out           = RPG_CLIENT_DEF_DEBUG;

  std::string data_path =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (yarp_PACKAGE_NAME),
                                                          ACE_TEXT_ALWAYS_CHAR (""),
                                                          ACE_TEXT_ALWAYS_CHAR (RPG_GRAPHICS_SUB_DIRECTORY_STRING),
                                                          false); // data-
  directory_out           = data_path;

  configuration_path =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (yarp_PACKAGE_NAME),
                                                          ACE_TEXT_ALWAYS_CHAR (""),
                                                          ACE_TEXT_ALWAYS_CHAR (RPG_GRAPHICS_SUB_DIRECTORY_STRING),
                                                          true); // configuration-
  graphicsDictionary_out  = configuration_path;
  graphicsDictionary_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  graphicsDictionary_out += ACE_TEXT_ALWAYS_CHAR (RPG_GRAPHICS_DICTIONARY_FILE);

  entityFile_out          =
    RPG_Player_Common_Tools::getPlayerProfilesDirectory ();
  entityFile_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  entityFile_out +=
   RPG_Common_Tools::sanitize (ACE_TEXT_ALWAYS_CHAR (RPG_PLAYER_DEF_NAME));
  entityFile_out += ACE_TEXT_ALWAYS_CHAR (RPG_PLAYER_PROFILE_EXT);

  openGLMode_out          = false;
  logToFile_out           = false;

  data_path =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (yarp_PACKAGE_NAME),
                                                          ACE_TEXT_ALWAYS_CHAR (""),
                                                          ACE_TEXT_ALWAYS_CHAR (RPG_ENGINE_SUB_DIRECTORY_STRING),
                                                          false); // data-
  mapFile_out             = data_path;
  mapFile_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  mapFile_out +=
    RPG_Common_Tools::sanitize (ACE_TEXT_ALWAYS_CHAR (RPG_ENGINE_LEVEL_DEF_NAME));
  mapFile_out += ACE_TEXT_ALWAYS_CHAR (RPG_ENGINE_LEVEL_FILE_EXT);

  slideShowMode_out       = (SDL_GUI_DEF_MODE == SDL_GUI_USERMODE_SLIDESHOW);
  traceInformation_out    = false;
  printVersionAndExit_out = false;
  width_out               = SDL_GUI_DEF_VIDEO_W;

  validateXML_out = false; // SDL_GUI_DEF_VALIDATE_XML;

  ACE_Get_Opt argumentParser (argc_in,
                              argv_in,
                              ACE_TEXT ("c::d:fg:h:i:lm:n:op::stvw:x"));

  int option = 0;
  std::stringstream converter;
  while ((option = argumentParser ()) != EOF)
  {
    switch (option)
    {
      case 'c':
      {
        if (argumentParser.opt_arg ())
          entityFile_out = ACE_TEXT_ALWAYS_CHAR (argumentParser.opt_arg ());
        else
          entityFile_out.clear ();
        break;
      }
      case 'd':
      {
        directory_out = ACE_TEXT_ALWAYS_CHAR (argumentParser.opt_arg ());
        break;
      }
      case 'f':
      {
        debugMode_out = true;
        break;
      }
      case 'g':
      {
        graphicsDictionary_out = ACE_TEXT_ALWAYS_CHAR (argumentParser.opt_arg ());
        break;
      }
      case 'h':
      {
        converter.clear ();
        converter.str (ACE_TEXT_ALWAYS_CHAR (""));
        converter << ACE_TEXT_ALWAYS_CHAR (argumentParser.opt_arg ());
        converter >> height_out;
        break;
      }
      case 'i':
      {
        itemsDictionary_out = ACE_TEXT_ALWAYS_CHAR (argumentParser.opt_arg ());
        break;
      }
      case 'l':
      {
        logToFile_out = true;
        break;
      }
      case 'm':
      {
        magicDictionary_out = ACE_TEXT_ALWAYS_CHAR (argumentParser.opt_arg ());
        break;
      }
      case 'n':
      {
        monsterDictionary_out = ACE_TEXT_ALWAYS_CHAR (argumentParser.opt_arg ());
        break;
      }
      case 'o':
      {
        openGLMode_out = true;
        break;
      }
      case 'p':
      {
        if (argumentParser.opt_arg ())
          mapFile_out = ACE_TEXT_ALWAYS_CHAR (argumentParser.opt_arg ());
        else
          mapFile_out.clear ();
        break;
      }
      case 's':
      {
        slideShowMode_out = true;
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
      case 'w':
      {
        converter.clear ();
        converter.str (ACE_TEXT_ALWAYS_CHAR (""));
        converter << ACE_TEXT_ALWAYS_CHAR (argumentParser.opt_arg ());
        converter >> width_out;
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
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("unrecognized option \"%s\", aborting\n"),
                    argumentParser.last_option ()));
        return false;
      }
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("unrecognized option \"%c\", aborting\n"),
                    option));
        return false;
      }
    } // end SWITCH
  } // end WHILE

  return true;
}

void
do_slideshow (const std::string& graphicsDirectory_in,
              SDL_GUI_MainWindow* mainWindow_in)
{
  RPG_TRACE (ACE_TEXT ("::do_slideshow"));

  // this simply draws (random) images inside the main "window"

  union SDL_Event sdl_event;
  bool done = false;
  RPG_Graphics_IWindowBase* window = NULL;
  struct RPG_Graphics_GraphicTypeUnion type;
  type.discriminator = RPG_Graphics_GraphicTypeUnion::INVALID;
  type.image = RPG_GRAPHICS_IMAGE_INVALID;
  RPG_Graphics_t graphic;
  graphic.category = RPG_GRAPHICS_CATEGORY_INVALID;
  graphic.type.discriminator = RPG_Graphics_GraphicTypeUnion::INVALID;
  RPG_Dice_RollResult_t result;
  SDL_Surface* image = NULL;
  bool release_image = false;
  struct SDL_Rect dirty_region;
  do
  {
    window = NULL;
    image = NULL;
    release_image = false;
    ACE_OS::memset (&dirty_region, 0, sizeof (struct SDL_Rect));

    // reset screen
    mainWindow_in->draw ();
    mainWindow_in->update ();

    // step1: choose (random) category
    result.clear();
    RPG_Dice::generateRandomNumbers (RPG_GRAPHICS_CATEGORY_MAX,
                                     1,
                                     result);
    graphic.category =
      static_cast<RPG_Graphics_Category> ((result.front () - 1));

    // step2: choose (random) type within category
    result.clear();
    switch (graphic.category)
    {
      case CATEGORY_CURSOR:
      {
        RPG_Dice::generateRandomNumbers (RPG_GRAPHICS_CURSOR_MAX,
                                         1,
                                         result);
        type.cursor = static_cast<RPG_Graphics_Cursor> ((result.front () - 1));
        type.discriminator = RPG_Graphics_GraphicTypeUnion::CURSOR;

        break;
      }
      case CATEGORY_FONT:
      {
        RPG_Dice::generateRandomNumbers (RPG_GRAPHICS_FONT_MAX,
                                         1,
                                         result);
        type.font = static_cast<RPG_Graphics_Font> ((result.front () - 1));
        type.discriminator = RPG_Graphics_GraphicTypeUnion::FONT;

        break;
      }
      case CATEGORY_INTERFACE:
      case CATEGORY_IMAGE:
      {
        RPG_Dice::generateRandomNumbers (RPG_GRAPHICS_IMAGE_MAX,
                                         1,
                                         result);
        type.image = static_cast<RPG_Graphics_Image> ((result.front () - 1));
        type.discriminator = RPG_Graphics_GraphicTypeUnion::IMAGE;

        break;
      }
      case CATEGORY_SPRITE:
      {
        RPG_Dice::generateRandomNumbers (RPG_GRAPHICS_SPRITE_MAX,
                                         1,
                                         result);
        type.sprite = static_cast<enum RPG_Graphics_Sprite> ((result.front () - 1));
        type.discriminator = RPG_Graphics_GraphicTypeUnion::SPRITE;

        break;
      }
      case CATEGORY_TILE:
      {
        RPG_Dice::generateRandomNumbers (RPG_GRAPHICS_TILEGRAPHIC_MAX,
                                         1,
                                         result);
        type.tilegraphic =
            static_cast<enum RPG_Graphics_TileGraphic> ((result.front () - 1));
        type.discriminator = RPG_Graphics_GraphicTypeUnion::TILEGRAPHIC;

        break;
      }
      case CATEGORY_TILESET:
      {
        RPG_Dice::generateRandomNumbers (RPG_GRAPHICS_TILESETGRAPHIC_MAX,
                                         1,
                                         result);
        type.tilesetgraphic =
            static_cast<enum RPG_Graphics_TileSetGraphic> ((result.front () - 1));
        type.discriminator = RPG_Graphics_GraphicTypeUnion::TILESETGRAPHIC;

        break;
      }
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid category (was: \"%s\"), continuing\n"),
                    ACE_TEXT (RPG_Graphics_CategoryHelper::RPG_Graphics_CategoryToString (graphic.category).c_str ())));
        continue;
      }
    } // end SWITCH

    // step3: load image/graphic
    switch (graphic.category)
    {
      case CATEGORY_CURSOR:
      case CATEGORY_INTERFACE:
      case CATEGORY_IMAGE:
      case CATEGORY_SPRITE:
      case CATEGORY_TILE:
      {
        image = RPG_Graphics_Common_Tools::loadGraphic (type,
                                                        true,  // convert to display format
                                                        true); // cache graphic
        if (!image)
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to RPG_Graphics_Common_Tools::loadGraphic(\"%s\"), continuing\n"),
                      ACE_TEXT (RPG_Graphics_Common_Tools::toString (type).c_str ())));
          continue;
        } // end IF

        break;
      }
      case CATEGORY_FONT:
      {
        std::string text = ACE_TEXT_ALWAYS_CHAR (SDL_GUI_DEF_SLIDESHOW_TEXT);
        RPG_Graphics_TextSize_t text_size =
            RPG_Graphics_Common_Tools::textSize (type.font,
                                                 text);
        image = RPG_Graphics_Surface::create (text_size.first,
                                              text_size.second);
        if (!image)
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to RPG_Graphics_Surface::create(%u,%u), continuing\n"),
                      text_size.first, text_size.second));
          continue;
        } // end IF
        RPG_Graphics_Surface::fill (RPG_Graphics_SDL_Tools::getColor (COLOR_BLACK,
                                                                      *image->format,
                                                                      1.0f),
                                    image);
        if (!RPG_Graphics_Surface::putText (type.font,
                                            text,
                                            RPG_Graphics_SDL_Tools::colorToSDLColor (RPG_Graphics_SDL_Tools::getColor (SDL_GUI_DEF_TILE_INDEX_COLOR,
                                                                                                                       *image->format,
                                                                                                                       1.0f),
                                                                                     *image->format),
                                            false, // add shade ?
                                            RPG_Graphics_SDL_Tools::colorToSDLColor (RPG_Graphics_SDL_Tools::getColor (RPG_GRAPHICS_FONT_DEF_SHADECOLOR,
                                                                                                                       *image->format,
                                                                                                                       1.0f),
                                                                                     *image->format),
                                            std::make_pair (0, 0),
                                            image,
                                            dirty_region))
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to RPG_Graphics_Surface::putText(%s), continuing\n"),
                      ACE_TEXT (RPG_Graphics_FontHelper::RPG_Graphics_FontToString (type.font).c_str ())));
          SDL_FreeSurface (image);
          continue;
        } // end IF
        release_image = true;

        break;
      }
      case CATEGORY_TILESET:
      {
        // need to load this manually --> retrieve filename(s)
        graphic.category = RPG_GRAPHICS_CATEGORY_INVALID;
        graphic.type.discriminator = RPG_Graphics_GraphicTypeUnion::INVALID;
        graphic = RPG_GRAPHICS_DICTIONARY_SINGLETON::instance ()->get (type);
        ACE_ASSERT ((graphic.type.discriminator == type.discriminator) && (graphic.type.tilesetgraphic == type.tilesetgraphic));
        ACE_ASSERT (!graphic.tileset.tiles.empty ());

        // choose random tile
        std::vector<struct RPG_Graphics_Tile>::const_iterator iterator =
            graphic.tileset.tiles.begin ();
        result.clear ();
        RPG_Dice::generateRandomNumbers (static_cast<unsigned int> (graphic.tileset.tiles.size ()),
                                         1,
                                         result);
        std::advance (iterator, (result.front () - 1));

        // assemble filename
        std::string filename = graphicsDirectory_in;
        filename += ACE_DIRECTORY_SEPARATOR_CHAR_A;
        switch (graphic.tileset.type)
        {
          case TILESETTYPE_DOOR:
          {
            filename +=
                ACE_TEXT_ALWAYS_CHAR (RPG_GRAPHICS_TILE_DOORS_SUB);
            break;
          }
          case TILESETTYPE_FLOOR:
          {
            filename += ACE_TEXT_ALWAYS_CHAR (RPG_GRAPHICS_TILE_FLOORS_SUB);
            break;
          }
          case TILESETTYPE_WALL:
          {
            filename += ACE_TEXT_ALWAYS_CHAR (RPG_GRAPHICS_TILE_WALLS_SUB);
            break;
          }
          default:
          {
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("invalid tileset type (was: \"%s\"), continuing\n"),
                        ACE_TEXT (RPG_Graphics_TileSetTypeHelper::RPG_Graphics_TileSetTypeToString (graphic.tileset.type).c_str ())));

            continue;
          }
        } // end SWITCH
        filename += ACE_DIRECTORY_SEPARATOR_CHAR_A;
        filename += (*iterator).file;

        image = RPG_Graphics_Surface::load (filename, // file
                                            true);    // convert to display format
        if (!image)
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to RPG_Graphics_Surface::load(\"%s\"), continuing\n"),
                      ACE_TEXT (filename.c_str ())));

          continue;
        } // end IF
        release_image = true;

        break;
      }
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid category (was: \"%s\"), continuing\n"),
                    ACE_TEXT (RPG_Graphics_CategoryHelper::RPG_Graphics_CategoryToString (graphic.category).c_str ())));
        continue;
      }
    } // end SWITCH
    ACE_ASSERT (image);

    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("showing graphics type \"%s\"...\n"),
                ACE_TEXT (RPG_Graphics_Common_Tools::toString (type).c_str ())));

    // step4: draw image to screen
#if defined (SDL_USE)
    SDL_Surface* surface_p = state.screen;
#elif defined (SDL2_USE)
    SDL_Surface* surface_p = SDL_GetWindowSurface (state.screen);
#endif // SDL_USE || SDL2_USE
    RPG_Graphics_Surface::put (std::make_pair (((surface_p->w - image->w) / 2),  // location x
                                               ((surface_p->h - image->h) / 2)), // location y
                               *image,                                           // image
                               surface_p,                                        // screen
                               dirty_region);                                    // return value: "dirty" region
#if defined (SDL_USE)
    if (SDL_Flip (state.screen))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to SDL_Flip(): \"%s\", returning\n"),
                  ACE_TEXT (SDL_GetError ())));
      if (release_image)
        SDL_FreeSurface (image);
      return;
    } // end IF
#elif defined (SDL2_USE)
    if (SDL_UpdateWindowSurface (state.screen) < 0)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to SDL_UpdateWindowSurface(%@): \"%s\", continuing\n"),
                  state.screen,
                  ACE_TEXT (SDL_GetError ())));
      if (release_image)
        SDL_FreeSurface (image);
      return;
    } // end IF
#endif // SDL_USE || SDL2_USE

    // step5: wait a little while
    do_SDL_waitForInput (SDL_GUI_DEF_SLIDESHOW_DELAY, // second(s)
                         sdl_event);                  // return value: event
    switch (sdl_event.type)
    {
      case SDL_KEYDOWN:
      {
        switch (sdl_event.key.keysym.sym)
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
      case SDL_MOUSEBUTTONDOWN:
      {
        // find window
        RPG_Graphics_Position_t mouse_position =
            std::make_pair (sdl_event.button.x,
                            sdl_event.button.y);
        window = mainWindow_in->getWindow (mouse_position);
        if (window)
        {
          try {
            window->handleEvent (sdl_event,
                                 window,
                                 dirty_region);
          } catch (...) {
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("caught exception in RPG_Graphics_IWindow::handleEvent(), continuing\n")));
          }
        } // end IF

        break;
      }
      case SDL_QUIT:
      {
        // finished event processing
        done = true;

        break;
      }
      case SDL_GUI_SDL_TIMEREVENT:
        break;
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid SDL event (type: %u), continuing\n"),
                    sdl_event.type));

        break;
      }
    } // end SWITCH

    // clean up
    if (release_image)
      SDL_FreeSurface (image);
  } while (!done);
}

void
do_UI (struct RPG_Engine_Entity& entity_in,
       RPG_Engine* engine_in,
       const struct RPG_Engine_LevelData& level_in,
       const struct RPG_Map_FloorPlan_Configuration& mapConfig_in,
       SDL_GUI_MainWindow* mainWindow_in,
       bool openGL_in,
       bool debug_in)
{
  RPG_TRACE (ACE_TEXT ("::do_UI"));

  union SDL_Event sdl_event;
  bool event_handled = false;
  bool done = false;
  RPG_Client_IWindowLevel* map_window =
      dynamic_cast<RPG_Client_IWindowLevel*> (mainWindow_in->child (WINDOW_MAP));
  ACE_ASSERT (map_window);
  RPG_Graphics_IWindowBase* window = NULL;
  RPG_Graphics_IWindowBase* previous_window = NULL;
  RPG_Graphics_Position_t mouse_position;
  unsigned int map_index = 1;
  std::ostringstream converter;
  struct RPG_Engine_LevelData current_level;
  struct SDL_Rect dirty_region;
  bool has_hovered = false;
  do
  {
    event_handled = false;
    window = NULL;
    mouse_position = std::make_pair (std::numeric_limits<unsigned int>::max (),
                                     std::numeric_limits<unsigned int>::max ());
    ACE_OS::memset (&dirty_region, 0, sizeof (struct SDL_Rect));
    has_hovered = false;

    // step1: retrieve next event
    if (SDL_WaitEvent (&sdl_event) != 1)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to SDL_WaitEvent(): \"%s\", aborting\n"),
                  ACE_TEXT (SDL_GetError ())));
      return;
    } // end IF

    // if necessary, reset hover_time
    if (sdl_event.type != RPG_GRAPHICS_SDL_HOVEREVENT)
    { ACE_GUARD (ACE_Thread_Mutex, aGuard, state.hover_lock);
      state.hover_time = 0;
    } // end IF

    // step2: process current event
    switch (sdl_event.type)
    {
      case SDL_KEYDOWN:
      {
        switch (sdl_event.key.keysym.sym)
        {
          case SDLK_e:
          {
            std::string dump_path =
                RPG_Player_Common_Tools::getPlayerProfilesDirectory ();
            dump_path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
            dump_path += entity_in.character->getName ();
            dump_path += ACE_TEXT_ALWAYS_CHAR (RPG_PLAYER_PROFILE_EXT);
            RPG_Player* player_p = NULL;
            try {
              player_p = dynamic_cast<RPG_Player*> (entity_in.character);
            } catch (...) {
              ACE_DEBUG ((LM_ERROR,
                          ACE_TEXT ("caught exception in dynamic_cast<RPG_Player*>(%@), aborting\n"),
                          entity_in.character));
              player_p = NULL;
            }
            if (!player_p)
            {
              ACE_DEBUG ((LM_ERROR,
                          ACE_TEXT ("failed to dynamic_cast<RPG_Player*>(%@), returning\n"),
                          entity_in.character));
              return;
            } // end IF
            if (!player_p->save (dump_path))
            {
              ACE_DEBUG ((LM_ERROR,
                          ACE_TEXT ("failed to RPG_Player::save(\"%s\"), aborting\n"),
                          ACE_TEXT (dump_path.c_str ())));
              return;
            } // end IF
            ACE_DEBUG ((LM_DEBUG,
                        ACE_TEXT ("saved player...\n")));

            event_handled = true;

            break;
          }
          case SDLK_l:
          {
            ACE_DEBUG ((LM_DEBUG,
                        ACE_TEXT ("generating level map...\n")));

            RPG_Engine_Level::create (mapConfig_in,
                                      current_level);

            if (engine_in->isRunning ())
              engine_in->stop ();

            entity_in.position = current_level.map.start;

            // *NOTE*: triggers a center/redraw/refresh of the map window !
            // --> but as we're not using the client engine, it doesn't redraw...
            engine_in->set (current_level);
            engine_in->start ();

            // center map window...
//            RPG_Client_IWindowLevel* map_window =
//                dynamic_cast<RPG_Client_IWindowLevel*>(mainWindow_in->child(WINDOW_MAP));
//            ACE_ASSERT(map_window);
            try {
              map_window->setView (entity_in.position);
              map_window->draw ();
              map_window->getArea (dirty_region,
                                   false); // toplevel- ?
            } catch (...) {
              ACE_DEBUG ((LM_ERROR,
                          ACE_TEXT ("caught exception in RPG_Graphics_IWindow, continuing\n")));
            }
            event_handled = true;

            break;
          }
          case SDLK_p:
          {
            RPG_Engine_Level::print (current_level);
            event_handled = true;

            break;
          }
          case SDLK_r:
          { ACE_ASSERT (map_window);
            try {
              map_window->draw ();
              map_window->getArea (dirty_region,
                                   false); // toplevel- ?
            } catch (...) {
              ACE_DEBUG ((LM_ERROR,
                          ACE_TEXT ("caught exception in RPG_Graphics_IWindow, continuing\n")));
            }
            RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance ()->reset (false,     // update ?
                                                                       true,      // locked access ?
                                                                       debug_in); // debug ?

            // draw "active" tile highlight
            int x, y;
            SDL_GetMouseState (&x, &y);
            mouse_position = std::make_pair (x, y);
            struct SDL_Rect window_area;
            map_window->getArea (window_area,
                                 true); // toplevel- ?
            engine_in->lock ();
            RPG_Graphics_Position_t map_position =
                RPG_Graphics_Common_Tools::screenToMap (mouse_position,
                                                        engine_in->getSize (false), // locked access ?
                                                        std::make_pair (window_area.w,
                                                                        window_area.h),
                                                        map_window->getView ());
            // inside map ?
            if (map_position == std::make_pair (std::numeric_limits<unsigned int>::max (),
                                                std::numeric_limits<unsigned int>::max ()))
            {
              engine_in->unlock ();
              event_handled = true;
              break; // off the map
            } // end IF
            struct SDL_Rect dirty_region_2;
            ACE_OS::memset (&dirty_region_2, 0, sizeof (struct SDL_Rect));
            if (engine_in->isValid (map_position, false))
            {
              RPG_Map_PositionList_t positions;
              RPG_Graphics_Offsets_t screen_positions;
              for (RPG_Map_PositionListConstIterator_t iterator = state.positions.begin ();
                   iterator != state.positions.end ();
                   iterator++)
              {
                positions.push_back (*iterator);
                screen_positions.push_back (RPG_Graphics_Common_Tools::mapToScreen (*iterator,
                                                                                    std::make_pair (window_area.w,
                                                                                                    window_area.h),
                                                                                    map_window->getView ()));
              } // end FOR
              RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance ()->putHighlights (positions,
                                                                                 screen_positions,
                                                                                 map_window->getView (),
                                                                                 dirty_region_2,
                                                                                 true, // locked access ? (screen lock)
                                                                                 debug_in);
              dirty_region =
                RPG_Graphics_SDL_Tools::boundingBox (dirty_region,
                                                     dirty_region_2);
            } // end IF
            engine_in->unlock ();
            event_handled = true;

            break;
          }
          case SDLK_s:
          {
            std::string dump_path;
#if defined (ACE_WIN32) && defined (ACE_WIN64)
            dump_path =
                ACE_OS::getenv (ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_TEMPORARY_STORAGE_VARIABLE));
#else
            dump_path =
                ACE_TEXT_ALWAYS_CHAR (COMMON_LOCATION_TEMPORARY_STORAGE_DIRECTORY);
#endif // ACE_WIN32 || ACE_WIN64
            dump_path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
            dump_path += current_level.metadata.name;
            dump_path += ACE_TEXT_ALWAYS_CHAR ("_");
            converter.str (ACE_TEXT_ALWAYS_CHAR (""));
            converter << map_index++;
            dump_path += converter.str ();
            dump_path += ACE_TEXT_ALWAYS_CHAR (RPG_ENGINE_LEVEL_FILE_EXT);
            engine_in->save (dump_path);
            ACE_DEBUG ((LM_DEBUG,
                        ACE_TEXT ("saved map \"%s\"...\n"),
                        ACE_TEXT (dump_path.c_str ())));
            event_handled = true;

            break;
          }
          case SDLK_q:
          {
            // finished event processing
            done = true;
            event_handled = true;

            break;
          }
          default:
            break;
        } // end SWITCH

        if (done)
          break; // leave
        // *WARNING*: falls through !
      }
#if defined (SDL_USE)
      case SDL_ACTIVEEVENT:
#elif defined (SDL2_USE)
      case SDL_WINDOWEVENT:
#endif // SDL_USE || SDL2_USE
      case SDL_MOUSEBUTTONDOWN:
      case SDL_MOUSEMOTION:
      case RPG_GRAPHICS_SDL_HOVEREVENT: // hovering...
      {
        // find window
        switch (sdl_event.type)
        {
          case SDL_MOUSEMOTION:
            mouse_position = std::make_pair (sdl_event.motion.x,
                                             sdl_event.motion.y); break;
          case SDL_MOUSEBUTTONDOWN:
            mouse_position = std::make_pair (sdl_event.button.x,
                                             sdl_event.button.y); break;
          default:
          {
            int x, y;
            SDL_GetMouseState (&x, &y);
            mouse_position = std::make_pair (x, y);

            break;
          }
        } // end SWITCH
        window = mainWindow_in->getWindow (mouse_position);
        ACE_ASSERT (window);

        if (sdl_event.type == SDL_MOUSEMOTION)
        {
          // notify previously "active" window upon losing "focus"
          if (previous_window &&
//                 (previous_window != mainWindow)
              (previous_window != window))
          {
            sdl_event.type = RPG_GRAPHICS_SDL_MOUSEMOVEOUT;
            try {
              previous_window->handleEvent (sdl_event,
                                            previous_window,
                                            dirty_region);
            } catch (...) {
              ACE_DEBUG ((LM_ERROR,
                          ACE_TEXT ("caught exception in RPG_Graphics_IWindow::handleEvent(), continuing\n")));
            }
            sdl_event.type = SDL_MOUSEMOTION;
          } // end IF
        } // end IF
        // remember last "active" window
        previous_window = window;

        // notify "active" window
        if (!event_handled)
        {
          struct SDL_Rect dirty_region_2;
          ACE_OS::memset (&dirty_region_2, 0, sizeof (struct SDL_Rect));
          RPG_Graphics_IWindow* window_2 = NULL;
          try {
            window_2 = dynamic_cast<RPG_Graphics_IWindow*> (window);
          } catch (...) {
            window_2 = NULL;
          }
          if (!window_2)
          {
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("failed to dynamic_cast<RPG_Graphics_IWindow*>(%@), returning\n"),
                        window));
            break;
          } // end IF
          if (!window_2->visible ())
            window = window->getParent ();
          ACE_ASSERT (window);
          try {
            window->handleEvent (sdl_event,
                                 window,
                                 dirty_region_2);
          } catch (...) {
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("caught exception in RPG_Graphics_IWindow::handleEvent(), continuing\n")));
          }
          dirty_region = RPG_Graphics_SDL_Tools::boundingBox (dirty_region,
                                                              dirty_region_2);

          if ((sdl_event.type == RPG_GRAPHICS_SDL_HOVEREVENT) &&
              (dirty_region_2.w && dirty_region_2.h))
            has_hovered = true;
        } // end IF

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
        break;
      case SDL_SYSWMEVENT:
      {
//        // debug info
//        std::ostringstream version_number;
//        version_number << sdl_event.syswm.msg->version.major;
//        version_number << ACE_TEXT_ALWAYS_CHAR(".");
//        version_number << sdl_event.syswm.msg->version.minor;
//        version_number << ACE_TEXT_ALWAYS_CHAR(".");
//        version_number << sdl_event.syswm.msg->version.patch;
//        ACE_DEBUG((LM_DEBUG,
//                   ACE_TEXT("SDL_SYSWMEVENT event (version: %s, subsystem: %d, type: %d)\n"),
//                   ACE_TEXT(version_number.str().c_str()),
//                   sdl_event.syswm.msg->subsystem,
//#if defined(ACE_WIN32) || defined(ACE_WIN64)

//#else
//                   sdl_event.syswm.msg->event.xevent.type));
//#endif

        break;
      }
#if defined (SDL_USE)
      case SDL_VIDEOEXPOSE:
      case SDL_VIDEORESIZE:
#endif // SDL_USE
        break;
      case SDL_GUI_SDL_TIMEREVENT:
      default:
        break;
    } // end SWITCH

    // redraw cursor ?
    switch (sdl_event.type)
    {
#if defined (SDL_USE)
      case SDL_ACTIVEEVENT:
#elif defined (SDL2_USE)
      case SDL_WINDOWEVENT:
#endif // SDL_USE || SDL2_USE
      case SDL_KEYDOWN:
      case SDL_MOUSEBUTTONDOWN:
      {
        // map hasn't changed --> no need to redraw
        if ((dirty_region.w == 0) &&
            (dirty_region.h == 0))
          break;

        // *WARNING*: falls through !
      }
      case SDL_MOUSEMOTION:
      case RPG_GRAPHICS_SDL_HOVEREVENT:
      {
        // sanity check
        if ((sdl_event.type == RPG_GRAPHICS_SDL_HOVEREVENT) &&
            !has_hovered)
            break; // nothing to do...

        // map has changed, cursor MAY have been drawn over...
        // --> redraw cursor
        struct SDL_Rect dirty_region_2;
        ACE_OS::memset (&dirty_region_2, 0, sizeof (struct SDL_Rect));
        RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance()->putCursor (std::make_pair (mouse_position.first,
                                                                                      mouse_position.second),
                                                                      dirty_region_2,
                                                                      true, // locked access ?
                                                                      debug_in);
        dirty_region = RPG_Graphics_SDL_Tools::boundingBox (dirty_region,
                                                            dirty_region_2);
        mainWindow_in->invalidate (dirty_region);

        break;
      }
      default:
        break;
    } // end SWITCH

    // update screen ?
    //if ((dirty_region.w != 0) ||
    //    (dirty_region.h != 0))
    //{
      try {
        mainWindow_in->update ();
        //if (openGL_in)
        //{
        //  map_window->draw();
        //  map_window->update();
        //} // end IF
      } catch (...) {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("caught exception in RPG_Graphics_IWindow::update(), continuing\n")));
      }
    //} // end IF
  } while (!done);
}

void
do_work (mode_t mode_in,
         const std::string& entity_in,
         const std::string& map_in,
         const struct RPG_Map_FloorPlan_Configuration& mapConfiguration_in,
         const struct RPG_Graphics_SDL_VideoConfiguration& videoConfiguration_in,
         const std::string& magicDictionary_in,
         const std::string& itemsDictionary_in,
         const std::string& monsterDictionary_in,
         const std::string& graphicsDictionary_in,
         bool debugMode_in,
         const std::string& graphicsDirectory_in,
         const std::string& schemaRepository_in,
         unsigned int cacheSize_in,
         bool validateXML_in)
{
  RPG_TRACE (ACE_TEXT ("::do_work"));

  // step0: initialize: random seed, string conversion facilities, ...
  std::string schema_repository_string = schemaRepository_in;
  std::string schema_repository_string_2 =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (yarp_PACKAGE_NAME),
                                                          ACE_TEXT_ALWAYS_CHAR (""),
                                                          ACE_TEXT_ALWAYS_CHAR (RPG_GRAPHICS_SUB_DIRECTORY_STRING),
                                                          true); // configuration-
  std::vector<std::string> schema_directories_a;
  schema_directories_a.push_back (schema_repository_string);
  schema_directories_a.push_back (schema_repository_string_2);
  RPG_Engine_Common_Tools::initialize (schema_directories_a,
                                       magicDictionary_in,
                                       itemsDictionary_in,
                                       monsterDictionary_in);

  struct Common_TimerConfiguration timer_configuration;
  COMMON_TIMERMANAGER_SINGLETON::instance ()->initialize (timer_configuration);
  COMMON_TIMERMANAGER_SINGLETON::instance ()->start (NULL);

  // step1: initialize video
  // step1a: initialize video system
   if (!RPG_Graphics_SDL_Tools::preInitializeVideo (videoConfiguration_in,                       // configuration
                                                    ACE_TEXT_ALWAYS_CHAR (SDL_GUI_DEF_CAPTION))) // window/icon caption
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to RPG_Graphics_SDL_Tools::preInitVideo, aborting\n")));
    return;
  } // end IF
  // step1b: pre-initialize graphics
  RPG_Graphics_Common_Tools::preInitialize ();

#if defined (DEBUG_DEBUGGER)
  RPG_Client_Common_Tools::initializeClientDictionaries ();
#endif // DEBUG_DEBUGGER
  // step1c: initialize graphics dictionary
  if (!RPG_GRAPHICS_DICTIONARY_SINGLETON::instance ()->initialize (graphicsDictionary_in,
                                                                   validateXML_in))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to RPG_Graphics_Dictionary::initialize(): \"%m\", returning\n")));
    return;
  } // end IF

  // step1b: initialize graphics
  if (!RPG_Graphics_Common_Tools::initialize (graphicsDirectory_in,
                                              cacheSize_in,
                                              true)) // initialize SDL ?
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to RPG_Graphics_Common_Tools::initialize(): \"%m\", returning\n")));
    return;
  } // end IF

  // step1d: initialize video window
  if (!RPG_Graphics_SDL_Tools::initializeVideo (videoConfiguration_in,                      // configuration
                                                ACE_TEXT_ALWAYS_CHAR (SDL_GUI_DEF_CAPTION), // window/icon caption
                                                state.screen,                               // return value: window surface
                                                state.renderer,                             // return value: renderer
                                                state.GLContext,                            // return value: OpenGL context
                                                true))                                      // initialize window surface ?
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to RPG_Graphics_SDL_Tools::initializeVideo(), returning\n")));
    return;
  } // end IF
  ACE_ASSERT (state.screen);

  // step2: init input
  struct RPG_Client_SDL_InputConfiguration input_configuration;
#if defined (SDL_USE)
  input_configuration.key_repeat_initial_delay = SDL_DEFAULT_REPEAT_DELAY;
  input_configuration.key_repeat_interval = SDL_DEFAULT_REPEAT_INTERVAL;
#endif // SDL_USE || SDL2_USE
  input_configuration.use_UNICODE = true;
  if (!RPG_Client_Common_Tools::initializeSDLInput (input_configuration))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to RPG_Client_Common_Tools::initializeSDLInput(), returning\n")));
    return;
  } // end IF

  // step3: setup main "window"
  struct RPG_Graphics_GraphicTypeUnion type;
  type.discriminator = RPG_Graphics_GraphicTypeUnion::IMAGE;
  type.image = SDL_GUI_DEF_GRAPHICS_WINDOWSTYLE_TYPE;
  std::string title =
    ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GRAPHICS_WINDOW_MAIN_DEF_TITLE);

#if defined (SDL_USE)
  SDL_Surface* surface_p = state.screen;
#elif defined (SDL2_USE)
  SDL_Surface* surface_p = SDL_GetWindowSurface (state.screen);
#endif // SDL_USE || SDL2_USE
  ACE_ASSERT (surface_p);
  SDL_GUI_MainWindow main_window (std::make_pair (surface_p->w,
                                                  surface_p->h), // size
                                 type,                           // interface elements
                                 title,                          // title (== caption)
                                 FONT_MAIN_LARGE);               // title font
  main_window.initializeSDL (NULL,
                             state.screen,
                             state.GLContext);

  // ***** mouse setup *****
#if defined (SDL_USE)
  SDL_WarpMouse ((surface_p->w / 2), (surface_p->h / 2));
#endif // SDL_USE || SDL2_USE

  switch (mode_in)
  {
    case SDL_GUI_USERMODE_SLIDESHOW:
    {
      do_slideshow (graphicsDirectory_in,
                    &main_window);
      break;
    }
    case SDL_GUI_USERMODE_FLOOR_PLAN:
    {
      struct Common_EventDispatchConfiguration dispatch_configuration_s;
      dispatch_configuration_s.dispatch =
        RPG_ENGINE_USES_REACTOR ? COMMON_EVENT_DISPATCH_REACTOR
                                : COMMON_EVENT_DISPATCH_PROACTOR;
      dispatch_configuration_s.numberOfProactorThreads =
        NET_CLIENT_DEFAULT_NUMBER_OF_PROACTOR_DISPATCH_THREADS;
      dispatch_configuration_s.numberOfReactorThreads =
        NET_CLIENT_DEFAULT_NUMBER_OF_REACTOR_DISPATCH_THREADS;
      Common_Event_Tools::initializeEventDispatch (dispatch_configuration_s);
      struct Common_EventDispatchState dispatch_state_s;
      dispatch_state_s.configuration = &dispatch_configuration_s;
      Common_Event_Tools::startEventDispatch (dispatch_state_s);

      // step1: create/load initial level map
      struct RPG_Engine_LevelData level;
      if (map_in.empty ())
      {
        ACE_DEBUG ((LM_DEBUG,
                    ACE_TEXT ("generating level map...\n")));

        RPG_Engine_Level::create (mapConfiguration_in,
                                  level);
      } // end IF
      else
      {
        if (!RPG_Engine_Level::load (map_in,
                                     schemaRepository_in,
                                     level))
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to RPG_Engine_Level::load(\"%s\"), aborting\n"),
                      ACE_TEXT (map_in.c_str ())));
          Common_Event_Tools::finalizeEventDispatch (dispatch_state_s,
                                                     true,  // wait for completion ?
                                                     true); // close singletons ?
          break;
        } // end IF

        ACE_DEBUG ((LM_DEBUG,
                    ACE_TEXT ("loaded map (\"%s\":\n%s\n"),
                    ACE_TEXT (map_in.c_str ()),
                    ACE_TEXT (RPG_Map_Level::info (level.map).c_str ())));
      } // end ELSE
      if (debugMode_in)
        RPG_Engine_Level::print (level);

      // step2: set default cursor
      struct SDL_Rect dirty_region;
      ACE_OS::memset (&dirty_region, 0, sizeof (struct SDL_Rect));
      RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance ()->setCursor (CURSOR_NORMAL,
                                                                     dirty_region,
                                                                     true); // locked access ? (screen lock)

      // step3: create/load initial entity
      //std::string configuration_path =
      //  RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (yarp_PACKAGE_NAME),
      //                                                        ACE_TEXT_ALWAYS_CHAR (""),
      //                                                        ACE_TEXT_ALWAYS_CHAR (RPG_ENGINE_SUB_DIRECTORY_STRING),
      //                                                        true);
      //std::string schema_repository = configuration_path;

      struct RPG_Engine_Entity entity;
      entity.character = NULL;
      entity.is_spawned = false;
      entity.position =
          std::make_pair (std::numeric_limits<unsigned int>::max (),
                          std::numeric_limits<unsigned int>::max ());
      if (entity_in.empty ())
      {
        entity = RPG_Engine_Common_Tools::createEntity ();
        if (!entity.character)
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to RPG_Engine_Common_Tools::createEntity(), aborting\n")));
          Common_Event_Tools::finalizeEventDispatch (dispatch_state_s,
                                                     true,  // wait for completion ?
                                                     true); // close singletons ?
          break;
        } // end IF

        ACE_DEBUG ((LM_DEBUG,
                    ACE_TEXT ("generated entity...\n")));
      } // end IF
      else
      {
        RPG_Character_Conditions_t condition;
        condition.insert (CONDITION_NORMAL);
        short int HP = std::numeric_limits<short int>::max ();
        RPG_Magic_Spells_t spells;
        entity.character = RPG_Player::load (entity_in,
                                             schemaRepository_in,
                                             condition,
                                             HP,
                                             spells);
        if (!entity.character)
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to RPG_Player::load(\"%s\"), returning\n"),
                      ACE_TEXT (entity_in.c_str ())));
          Common_Event_Tools::finalizeEventDispatch (dispatch_state_s,
                                                     true,  // wait for completion ?
                                                     true); // close singletons ?
          return;
        } // end IF

        ACE_DEBUG ((LM_DEBUG,
                    ACE_TEXT ("loaded entity \"%s\":\n%s\n"),
                    ACE_TEXT (entity_in.c_str()),
                    ACE_TEXT (RPG_Engine_Common_Tools::info (entity).c_str ())));
      } // end ELSE
      entity.position = level.map.start;

      // step4: initialize sub-windows (level window, hotspots, minimap, ...)
//      RPG_Client_Engine client_engine;
      RPG_Engine level_engine;
      main_window.initialize (&state,
                              //&client_engine,
                              &level_engine,
                              (videoConfiguration_in.use_OpenGL ? GRAPHICSMODE_2D_OPENGL
                                                                : GRAPHICSMODE_2D_ISOMETRIC),
#if defined (SDL_USE)
                              (state.screen->flags & SDL_DOUBLEBUF));
#elif defined (SDL2_USE)
                              RPG_GRAPHICS_DEF_FLIP);
#endif // SDL_USE || SDL2_USE

      // step4a: draw main window borders...
      try {
        main_window.draw (NULL,
                          0, 0);
        main_window.update (NULL);
      } catch (...) {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("caught exception in RPG_Graphics_IWindow::draw()/update(), continuing\n")));
      }

      // step5: initialize level state engine
      RPG_Engine_IClient* iclient_p =
        dynamic_cast<RPG_Engine_IClient*> (main_window.child (WINDOW_MAP));
      ACE_ASSERT (iclient_p);
      level_engine.initialize (iclient_p, // client engine handle
                               false);    // server session ?
      // initialize/add entity to the graphics cache
      RPG_Client_IWindowLevel* map_window =
        dynamic_cast<RPG_Client_IWindowLevel*> (main_window.child (WINDOW_MAP));
      ACE_ASSERT (map_window);
      RPG_GRAPHICS_CURSOR_MANAGER_SINGLETON::instance ()->initialize (&main_window,
                                                                      map_window);
      RPG_CLIENT_ENTITY_MANAGER_SINGLETON::instance ()->initialize (&main_window,
                                                                    map_window);

      level_engine.set (level);
      level_engine.start ();
      level_engine.lock ();
      RPG_Engine_EntityID_t entity_ID = level_engine.add (&entity,
                                                          false); // locked access ?
      level_engine.setActive (entity_ID,
                              false); // locked access ?
      RPG_Map_Position_t entity_position =
        level_engine.getPosition (entity_ID,
                                  false); // locked access ?
      level_engine.unlock ();

      // step5a: draw map window...
      try {
        map_window->setView (entity_position);
        map_window->draw (NULL,
                          0, 0);
        map_window->update (NULL);
      } catch (...) {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("caught exception in RPG_Graphics_IWindow::setView/draw/update, continuing\n")));
      }

      // step6: start timer (triggers hover events)
      SDL_TimerID timer = 0;
      timer = SDL_AddTimer (SDL_GUI_SDL_EVENT_TIMEOUT, // interval (ms)
                            event_timer_SDL_cb,        // event timer callback
                            &state);                   // callback argument
      if (!timer)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to SDL_AddTimer(%u): \"%s\", returning\n"),
                    SDL_GUI_SDL_EVENT_TIMEOUT,
                    ACE_TEXT (SDL_GetError ())));
        return;
      } // end IF

      // step7: run interface
      do_UI (entity,
             &level_engine,
             level,
             mapConfiguration_in,
             &main_window,
             videoConfiguration_in.use_OpenGL,
             debugMode_in);

      // clean up
      if (!SDL_RemoveTimer (timer))
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to SDL_RemoveTimer(): \"%s\", continuing\n"),
                    ACE_TEXT (SDL_GetError ())));
      level_engine.stop (true); // locked access ?
      COMMON_TIMERMANAGER_SINGLETON::instance ()->stop (true,   // wait for completion ?
                                                        false); // high priority ?
      Common_Event_Tools::finalizeEventDispatch (dispatch_state_s,
                                                 true,  // wait for completion ?
                                                 true); // close singletons ?

      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid mode (was: %d), aborting\n"),
                  mode_in));
      break;
    }
  } // end SWITCH

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("finished working...\n")));
} // end do_work

void
do_printVersion (const std::string& programName_in)
{
  RPG_TRACE (ACE_TEXT ("::do_printVersion"));

  // step1: program version
//   std::cout << programName_in << ACE_TEXT(" : ") << VERSION << std::endl;
  std::cout << programName_in
            << ACE_TEXT_ALWAYS_CHAR (": ")
            << yarp_PACKAGE_VERSION
            << std::endl;

  // step2: SDL version
  struct SDL_version sdl_version;
  ACE_OS::memset (&sdl_version, 0, sizeof (struct SDL_version));
  SDL_VERSION (&sdl_version);
  std::ostringstream version_number;
  version_number << sdl_version.major;
  version_number << ACE_TEXT_ALWAYS_CHAR (".");
  version_number << sdl_version.minor;
  version_number << ACE_TEXT_ALWAYS_CHAR (".");
  version_number << sdl_version.patch;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("SDL (compiled): ")
            << version_number.str ()
            << std::endl;
#if defined (SDL_USE)
  const SDL_version* sdl_version_p = SDL_Linked_Version ();
  if (!sdl_version_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_Linked_Version(): \"%s\", returning\n"),
                ACE_TEXT (SDL_GetError ())));
    return;
  } // end IF
  version_number.str ("");
  version_number << sdl_version_p->major;
  version_number << ACE_TEXT_ALWAYS_CHAR (".");
  version_number << sdl_version_p->minor;
  version_number << ACE_TEXT_ALWAYS_CHAR (".");
  version_number << sdl_version_p->patch;
  std::cout << ACE_TEXT_ALWAYS_CHAR ("SDL (linked): ")
            << version_number.str ()
            << std::endl;
#endif // SDL_USE

  // step3: ACE version
  // *NOTE*: cannot use ACE_VERSION, as it doesn't contain the (potential) beta
  // version number (this is needed, as the library soname is compared to this
  // string)
  version_number.str ("");
  version_number << ACE::major_version ();
  version_number << ACE_TEXT_ALWAYS_CHAR (".");
  version_number << ACE::minor_version ();
  version_number << ACE_TEXT_ALWAYS_CHAR (".");
  version_number << ACE::beta_version ();
  std::cout << ACE_TEXT_ALWAYS_CHAR ("ACE: ")
//             << ACE_VERSION
            << version_number.str ()
            << std::endl;
}

int
ACE_TMAIN (int argc_in,
           ACE_TCHAR* argv_in[])
{
  RPG_TRACE (ACE_TEXT ("::main"));

  // step0: initialize ACE
  // *PORTABILITY*: on Windows, ACE needs initialization...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (ACE::init () == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE::init(): \"%m\", aborting\n")));
    return EXIT_FAILURE;
  } // end IF
#endif // ACE_WIN32 || ACE_WIN64

  // step1: initialize
  state.screen = NULL;
//  state.hover_lock();
  state.hover_time = 0;
  state.debug = RPG_CLIENT_DEF_DEBUG;
  state.style.floor = RPG_CLIENT_GRAPHICS_DEF_FLOORSTYLE;
  state.style.edge = RPG_CLIENT_GRAPHICS_DEF_EDGESTYLE;
  state.style.wall = RPG_CLIENT_GRAPHICS_DEF_WALLSTYLE;
  state.style.half_height_walls = RPG_CLIENT_GRAPHICS_DEF_WALLSTYLE_HALF;
  state.style.door = RPG_CLIENT_GRAPHICS_DEF_DOORSTYLE;
  state.selection_mode = SELECTIONMODE_NORMAL;
  //state.seen_positions.clear ();
  //
  state.path.clear ();
  state.positions.clear ();
  state.radius = 0;
  state.source = std::make_pair (std::numeric_limits<unsigned int>::max (),
                                 std::numeric_limits<unsigned int>::max ());
  //
  state.angle = 0.0f;

  // step1a set defaults
  Common_File_Tools::initialize (ACE_TEXT_ALWAYS_CHAR (argv_in[0]));
  std::string configuration_path =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (yarp_PACKAGE_NAME),
                                                          ACE_TEXT_ALWAYS_CHAR (""),
                                                          ACE_TEXT_ALWAYS_CHAR (RPG_MAGIC_SUB_DIRECTORY_STRING),
                                                          true); // configuration-

  std::string magic_dictionary    = configuration_path;
  magic_dictionary += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  magic_dictionary += ACE_TEXT_ALWAYS_CHAR (RPG_MAGIC_DICTIONARY_FILE);

  configuration_path =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (yarp_PACKAGE_NAME),
                                                          ACE_TEXT_ALWAYS_CHAR (""),
                                                          ACE_TEXT_ALWAYS_CHAR (RPG_ITEM_SUB_DIRECTORY_STRING),
                                                          true); // configuration-
  std::string items_dictionary    = configuration_path;
  items_dictionary += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  items_dictionary += ACE_TEXT_ALWAYS_CHAR (RPG_ITEM_DICTIONARY_FILE);

  configuration_path =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (yarp_PACKAGE_NAME),
                                                          ACE_TEXT_ALWAYS_CHAR (""),
                                                          ACE_TEXT_ALWAYS_CHAR (RPG_MONSTER_SUB_DIRECTORY_STRING),
                                                          true); // configuration-
  std::string monster_dictionary  = configuration_path;
  monster_dictionary += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  monster_dictionary += ACE_TEXT_ALWAYS_CHAR (RPG_MONSTER_DICTIONARY_FILE);

  std::string data_path =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (yarp_PACKAGE_NAME),
                                                          ACE_TEXT_ALWAYS_CHAR (""),
                                                          ACE_TEXT_ALWAYS_CHAR (RPG_GRAPHICS_SUB_DIRECTORY_STRING),
                                                          false); // data-
  std::string graphics_directory  = data_path;

  unsigned int cache_size         = RPG_CLIENT_GRAPHICS_DEF_CACHESIZE;

  configuration_path =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (yarp_PACKAGE_NAME),
                                                          ACE_TEXT_ALWAYS_CHAR (""),
                                                          ACE_TEXT_ALWAYS_CHAR (RPG_GRAPHICS_SUB_DIRECTORY_STRING),
                                                          true); // configuration-
  std::string graphics_dictionary = configuration_path;
  graphics_dictionary += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  graphics_dictionary += ACE_TEXT_ALWAYS_CHAR (RPG_GRAPHICS_DICTIONARY_FILE);

  mode_t mode                     = SDL_GUI_DEF_MODE;

  std::string entity_filename     =
      RPG_Player_Common_Tools::getPlayerProfilesDirectory ();
  entity_filename += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  entity_filename +=
    RPG_Common_Tools::sanitize (ACE_TEXT_ALWAYS_CHAR (RPG_PLAYER_DEF_NAME));
  entity_filename += ACE_TEXT_ALWAYS_CHAR (RPG_PLAYER_PROFILE_EXT);

  data_path =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (yarp_PACKAGE_NAME),
                                                          ACE_TEXT_ALWAYS_CHAR (""),
                                                          ACE_TEXT_ALWAYS_CHAR (RPG_ENGINE_SUB_DIRECTORY_STRING),
                                                          false); // data-
  std::string map_filename        = data_path;
  map_filename += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  map_filename +=
    RPG_Common_Tools::sanitize (ACE_TEXT_ALWAYS_CHAR (RPG_ENGINE_LEVEL_DEF_NAME));
  map_filename += ACE_TEXT_ALWAYS_CHAR (RPG_ENGINE_LEVEL_FILE_EXT);

  bool log_to_file_b              = false;
  bool slideshow_mode             = (SDL_GUI_DEF_MODE == SDL_GUI_USERMODE_SLIDESHOW);
  bool trace_information          = false;
  bool print_version_and_exit     = false;
  bool validate_XML = false;// SDL_GUI_DEF_VALIDATE_XML;

  // *** map ***
  struct RPG_Map_FloorPlan_Configuration map_configuration;
  map_configuration.min_room_size          = SDL_GUI_DEF_MAP_MIN_ROOM_SIZE;
  map_configuration.doors                  = SDL_GUI_DEF_MAP_DOORS;
  map_configuration.corridors              = SDL_GUI_DEF_MAP_CORRIDORS;
  map_configuration.max_num_doors_per_room = SDL_GUI_DEF_MAP_MAX_NUM_DOORS_PER_ROOM;
  map_configuration.maximize_rooms         = SDL_GUI_DEF_MAP_MAXIMIZE_ROOMS;
  map_configuration.num_areas              = SDL_GUI_DEF_MAP_NUM_AREAS;
  map_configuration.square_rooms           = SDL_GUI_DEF_MAP_SQUARE_ROOMS;
  map_configuration.map_size_x             = SDL_GUI_DEF_MAP_SIZE_X;
  map_configuration.map_size_y             = SDL_GUI_DEF_MAP_SIZE_Y;

  // *** video ***
  struct RPG_Graphics_SDL_VideoConfiguration video_configuration;
  video_configuration.screen_width      = SDL_GUI_DEF_VIDEO_W;
  video_configuration.screen_height     = SDL_GUI_DEF_VIDEO_H;
  video_configuration.screen_colordepth = SDL_GUI_DEF_VIDEO_BPP;
//   video_configuration.screen_flags      = ;
  video_configuration.double_buffer     = SDL_GUI_DEF_VIDEO_DOUBLEBUFFER;
  video_configuration.use_OpenGL        = (SDL_GUI_DEF_GRAPHICS_MODE == GRAPHICSMODE_2D_OPENGL);
  video_configuration.full_screen       = SDL_GUI_DEF_VIDEO_FULLSCREEN;

  // step1b: parse/process/validate configuration
  if (!do_processArguments (argc_in,
                            argv_in,
                            magic_dictionary,
                            video_configuration.screen_height,
                            items_dictionary,
                            monster_dictionary,
                            graphics_directory,
                            state.debug,
                            graphics_dictionary,
                            entity_filename,
                            video_configuration.use_OpenGL,
                            log_to_file_b,
                            map_filename,
                            slideshow_mode,
                            trace_information,
                            print_version_and_exit,
                            video_configuration.screen_width,
                            validate_XML))
  {
    // make 'em learn...
    do_printUsage (ACE_TEXT_ALWAYS_CHAR (ACE::basename (argv_in[0], ACE_DIRECTORY_SEPARATOR_CHAR)));

    // *PORTABILITY*: on Windows, need to fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini () == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE::fini(): \"%m\", continuing\n")));
#endif // ACE_WIN32 || ACE_WIN64

    return EXIT_FAILURE;
  } // end IF

  // step1b: validate arguments
  if (!Common_File_Tools::isReadable (magic_dictionary)    ||
      !Common_File_Tools::isReadable (items_dictionary)    ||
      !Common_File_Tools::isDirectory (graphics_directory) ||
      !Common_File_Tools::isReadable (graphics_dictionary) ||
      (!entity_filename.empty () &&
       !Common_File_Tools::isReadable (entity_filename))   ||
      (!map_filename.empty () &&
       !Common_File_Tools::isReadable (map_filename))      ||
      (slideshow_mode && video_configuration.use_OpenGL))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid argument, aborting\n")));

    // make 'em learn...
    do_printUsage (ACE_TEXT_ALWAYS_CHAR (ACE::basename (argv_in[0])));

    // *PORTABILITY*: on Windows, need to fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini () == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif // ACE_WIN32 || ACE_WIN64
 
    return EXIT_FAILURE;
  } // end IF
  mode = (slideshow_mode ? SDL_GUI_USERMODE_SLIDESHOW : mode);

  std::string schema_repository =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (yarp_PACKAGE_NAME),
                                                          ACE_TEXT_ALWAYS_CHAR (""),
                                                          ACE_TEXT_ALWAYS_CHAR (RPG_ENGINE_SUB_DIRECTORY_STRING),
                                                          true); // configuration-

  // step1c: handle specific program modes
  if (print_version_and_exit)
  {
    do_printVersion (ACE_TEXT_ALWAYS_CHAR (ACE::basename (argv_in[0])));

    // *PORTABILITY*: on Windows, need to fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini () == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif // ACE_WIN32 || ACE_WIN64

    return EXIT_SUCCESS;
  } // end IF

  // step1d: initialize logging and/or tracing
  std::string log_file =
    (log_to_file_b ? Common_Log_Tools::getLogFilename (ACE_TEXT_ALWAYS_CHAR (yarp_PACKAGE_NAME),
                                                       ACE_TEXT_ALWAYS_CHAR (ACE::basename (argv_in[0])))
                   : ACE_TEXT_ALWAYS_CHAR (""));
  if (!Common_Log_Tools::initializeLogging (ACE_TEXT_ALWAYS_CHAR (ACE::basename (argv_in[0])), // program name
                                            log_file,                                          // logfile
                                            false,                                             // log to syslog ?
                                            false,                                             // trace messages ?
                                            trace_information,                                 // debug messages ?
                                            NULL))                                             // logger
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Tools::initializeLogging(), aborting\n")));

    // *PORTABILITY*: on Windows, need to fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini () == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif // ACE_WIN32 || ACE_WIN64

    return EXIT_FAILURE;
  } // end IF

  // step2: init SDL
  if (SDL_Init (SDL_INIT_TIMER |
                SDL_INIT_VIDEO |
                SDL_INIT_NOPARACHUTE) == -1) // "...Prevents SDL from catching fatal signals..."
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to SDL_Init(): \"%s\", aborting\n"),
                ACE_TEXT (SDL_GetError ())));

    Common_Log_Tools::finalizeLogging ();
    // *PORTABILITY*: on Windows, need to fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini () == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif // ACE_WIN32 || ACE_WIN64

    return EXIT_FAILURE;
  } // end IF
  // setup font engine
  if (TTF_Init () == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to TTF_Init(): \"%s\", aborting\n"),
                ACE_TEXT (SDL_GetError ())));

    SDL_Quit ();
    Common_Log_Tools::finalizeLogging ();
    // *PORTABILITY*: on Windows, need to fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini () == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif // ACE_WIN32 || ACE_WIN64

    return EXIT_FAILURE;
  } // end IF
  // enable WM events
  Uint8 event_state = 0;
//  event_state = SDL_EventState(SDL_SYSWMEVENT, SDL_QUERY);
//  if ((event_state == SDL_IGNORE) ||
//      (event_state == SDL_DISABLE))
//  {
//    SDL_EventState(SDL_SYSWMEVENT, SDL_ENABLE);

//    ACE_DEBUG((LM_DEBUG,
//               ACE_TEXT("enabled SDL_SYSWMEVENT events...\n")));
//  } // end IF
#if defined (SDL_USE)
  event_state = SDL_EventState (SDL_ACTIVEEVENT, SDL_QUERY);
  if ((event_state == SDL_IGNORE) ||
      (event_state == SDL_DISABLE))
  {
    SDL_EventState (SDL_ACTIVEEVENT, SDL_ENABLE);

    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("enabled SDL_ACTIVEEVENT events...\n")));
  } // end IF
#endif // SDL_USE

  // step3: do actual work
  ACE_High_Res_Timer timer;
  timer.start ();
  do_work (mode,
           entity_filename,
           map_filename,
           map_configuration,
           video_configuration,
           magic_dictionary,
           items_dictionary,
           monster_dictionary,
           graphics_dictionary,
           state.debug,
           graphics_directory,
           schema_repository,
           cache_size,
           validate_XML);
  timer.stop ();
  // debug info
  ACE_Time_Value working_time;
  timer.elapsed_time (working_time);
  std::string working_time_string =
    Common_Timer_Tools::periodToString (working_time);
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("total working time (h:m:s.us): \"%s\"...\n"),
              ACE_TEXT (working_time_string.c_str ())));

  // step4: clean up
  TTF_Quit ();
  SDL_Quit ();
  Common_Log_Tools::finalizeLogging ();
  // *PORTABILITY*: on Windows, fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (ACE::fini () == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE::fini(): \"%m\", aborting\n")));
    return EXIT_FAILURE;
  } // end IF
#endif // ACE_WIN32 || ACE_WIN64

  return EXIT_SUCCESS;
} // end main
