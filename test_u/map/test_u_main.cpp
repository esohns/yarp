#include "stdafx.h"

#include "test_u_main.h"

#include <string>

#include "ace/ACE.h"
#if defined(ACE_WIN32) || defined(ACE_WIN64)
#include "ace/Init_ACE.h"
#endif
#include "ace/Get_Opt.h"
#include "ace/High_Res_Timer.h"
#include "ace/Log_Msg.h"

//#include "gtk/gtk.h"

#include "common_tools.h"
#include "common_file_tools.h"

#include "common_ui_defines.h"
#include "common_ui_glade_definition.h"
#include "common_ui_gtk_manager.h"

#ifdef HAVE_CONFIG_H
#include "rpg_config.h"
#endif

#include "rpg_common_defines.h"
//#include "rpg_common_tools.h"
#include "rpg_common_file_tools.h"
#include "rpg_common_macros.h"

#include "rpg_magic_defines.h"

#include "rpg_item_defines.h"

#include "rpg_player_common_tools.h"

#include "rpg_engine_common_tools.h"
#include "rpg_engine_defines.h"

#include "rpg_sound_defines.h"

#include "rpg_graphics_defines.h"

#include "rpg_client_callbacks.h"
#include "rpg_client_common_tools.h"
#include "rpg_client_defines.h"

#include "map_generator_defines.h"

#include "map_generator_gui_callbacks.h"
#include "map_generator_gui_common.h"

test_u_main::test_u_main(void)
{
  RPG_TRACE(ACE_TEXT("test_u_main::test_u_main"));

}

test_u_main::~test_u_main(void)
{
  RPG_TRACE(ACE_TEXT("test_u_main::~test_u_main"));

}

void
test_u_main::print_usage(const std::string& programName_in)
{
  RPG_TRACE(ACE_TEXT("test_u_main::print_usage"));

  // enable verbatim boolean output
  std::cout.setf(ios::boolalpha);

  std::string configuration_path =
      RPG_Common_File_Tools::getConfigurationDataDirectory(ACE_TEXT_ALWAYS_CHAR(BASEDIR),
                                                           true);
  std::string data_path =
      RPG_Common_File_Tools::getConfigurationDataDirectory(ACE_TEXT_ALWAYS_CHAR(BASEDIR),
                                                           false);
#if defined (DEBUG_DEBUGGER)
  configuration_path = Common_File_Tools::getWorkingDirectory();
  data_path = Common_File_Tools::getWorkingDirectory();
#endif

  std::cout << ACE_TEXT("usage: ")
            << programName_in
            << ACE_TEXT(" [OPTIONS]")
            << std::endl
            << std::endl;
  std::cout << ACE_TEXT("currently available options:") << std::endl;
  std::string path = data_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  path += ACE_TEXT_ALWAYS_CHAR("graphics");
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path += ACE_TEXT_ALWAYS_CHAR(RPG_COMMON_DATA_SUB);
#else
  path += ACE_TEXT_ALWAYS_CHAR(RPG_GRAPHICS_DATA_SUB);
#endif
  std::cout << ACE_TEXT("-d [DIR]    : graphics directory")
            << ACE_TEXT(" [\"")
            << path
            << ACE_TEXT("\"]")
            << std::endl;
  path = configuration_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  path += ACE_TEXT_ALWAYS_CHAR("graphics");
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  path += ACE_TEXT_ALWAYS_CHAR(RPG_GRAPHICS_DICTIONARY_FILE);
  std::cout << ACE_TEXT("-g [FILE]   : graphics dictionary (*.xml)")
            << ACE_TEXT(" [\"")
            << path
            << ACE_TEXT("\"]")
            << std::endl;
  path = configuration_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  path += ACE_TEXT_ALWAYS_CHAR("item");
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  path += ACE_TEXT_ALWAYS_CHAR(RPG_ITEM_DICTIONARY_FILE);
  std::cout << ACE_TEXT("-i [FILE]   : item dictionary (*.xml)")
            << ACE_TEXT(" [\"")
            << path
            << ACE_TEXT("\"]")
            << std::endl;
  std::cout << ACE_TEXT("-l          : log to a file")
            << ACE_TEXT(" [")
            << false
            << ACE_TEXT("]")
            << std::endl;
  path = configuration_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  path += ACE_TEXT_ALWAYS_CHAR("magic");
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  path += ACE_TEXT_ALWAYS_CHAR(RPG_MAGIC_DICTIONARY_FILE);
  std::cout << ACE_TEXT("-m [FILE]   : magic dictionary (*.xml)")
            << ACE_TEXT(" [\"")
            << path
            << ACE_TEXT("\"]")
            << std::endl;
  std::cout << ACE_TEXT("-t          : trace information")
            << ACE_TEXT(" [")
            << false
            << ACE_TEXT("]")
            << std::endl;
  path = configuration_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  path += ACE_TEXT_ALWAYS_CHAR("test_u");
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path += ACE_TEXT_ALWAYS_CHAR("map");
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  path += ACE_TEXT_ALWAYS_CHAR(MAP_GENERATOR_GUI_GNOME_UI_FILE);
  std::cout << ACE_TEXT("-u [FILE]   : UI file")
            << ACE_TEXT(" [\"")
            << path
            << ACE_TEXT("\"]")
            << std::endl;
  std::cout << ACE_TEXT("-v          : print version information and exit")
            << ACE_TEXT(" [")
            << false
            << ACE_TEXT("]")
            << std::endl;
} // end print_usage

bool
test_u_main::process_arguments(int argc_in,
                               ACE_TCHAR* argv_in[], // cannot be const...
                               std::string& graphics_dictionary_out,
                               std::string& item_dictionary_out,
                               bool& logToFile_out,
                               std::string& magic_dictionary_out,
                               bool& traceInformation_out,
                               std::string& UI_file_out,
                               std::string& graphics_directory_out,
                               bool& printVersionAndExit_out)
{
  RPG_TRACE(ACE_TEXT("test_u_main::process_arguments"));

  // init configuration
  std::string configuration_path =
      RPG_Common_File_Tools::getConfigurationDataDirectory(ACE_TEXT_ALWAYS_CHAR(BASEDIR),
                                                           true);
  std::string data_path =
      RPG_Common_File_Tools::getConfigurationDataDirectory(ACE_TEXT_ALWAYS_CHAR(BASEDIR),
                                                           false);
#if defined (DEBUG_DEBUGGER)
  configuration_path = Common_File_Tools::getWorkingDirectory();
  data_path = Common_File_Tools::getWorkingDirectory();
#endif

  graphics_dictionary_out = configuration_path;
  graphics_dictionary_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  graphics_dictionary_out += ACE_TEXT_ALWAYS_CHAR("graphics");
  graphics_dictionary_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  graphics_dictionary_out +=
      ACE_TEXT_ALWAYS_CHAR(RPG_GRAPHICS_DICTIONARY_FILE);

  item_dictionary_out = configuration_path;
  item_dictionary_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  item_dictionary_out += ACE_TEXT_ALWAYS_CHAR("item");
  item_dictionary_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  item_dictionary_out += ACE_TEXT_ALWAYS_CHAR(RPG_ITEM_DICTIONARY_FILE);

  logToFile_out      = false;

  magic_dictionary_out = configuration_path;
  magic_dictionary_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  magic_dictionary_out += ACE_TEXT_ALWAYS_CHAR("magic");
  magic_dictionary_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  magic_dictionary_out += ACE_TEXT_ALWAYS_CHAR(RPG_MAGIC_DICTIONARY_FILE);

  traceInformation_out = false;

  UI_file_out = configuration_path;
  UI_file_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  UI_file_out += ACE_TEXT_ALWAYS_CHAR("test_u");
  UI_file_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  UI_file_out += ACE_TEXT_ALWAYS_CHAR("map");
  UI_file_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  UI_file_out += ACE_TEXT_ALWAYS_CHAR(MAP_GENERATOR_GUI_GNOME_UI_FILE);

  graphics_directory_out = data_path;
  graphics_directory_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  graphics_directory_out += ACE_TEXT_ALWAYS_CHAR("graphics");
  graphics_directory_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  graphics_directory_out += ACE_TEXT_ALWAYS_CHAR(RPG_COMMON_DATA_SUB);
#else
  graphics_directory_out += ACE_TEXT_ALWAYS_CHAR(RPG_GRAPHICS_DATA_SUB);
#endif

  printVersionAndExit_out  = false;

  ACE_Get_Opt argumentParser(argc_in,
                             argv_in,
                             ACE_TEXT("d:g:i:lm:tu:v"),
                             1,                         // skip command name
                             1,                         // report parsing errors
                             ACE_Get_Opt::PERMUTE_ARGS, // ordering
                             0);                        // don't use long options

  int option = 0;
  while ((option = argumentParser()) != EOF)
  {
    switch (option)
    {
      case 'd':
      {
        graphics_directory_out = argumentParser.opt_arg();

        break;
      }
      case 'g':
      {
        graphics_dictionary_out = argumentParser.opt_arg();

        break;
      }
      case 'i':
      {
        item_dictionary_out = argumentParser.opt_arg();

        break;
      }
      case 'l':
      {
        logToFile_out = true;

        break;
      }
      case 'm':
      {
        magic_dictionary_out = argumentParser.opt_arg();

        break;
      }
      case 't':
      {
        traceInformation_out = true;

        break;
      }
      case 'u':
      {
        UI_file_out = argumentParser.opt_arg();

        break;
      }
      case 'v':
      {
        printVersionAndExit_out = true;

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
                   ACE_TEXT(argumentParser.last_option())));

        return false;
      }
      case 0:
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("found long option \"%s\", aborting\n"),
                   ACE_TEXT(argumentParser.long_option())));

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

void
test_u_main::do_work(GTK_cb_data_t& userData_in,
                     const std::string& schemaDirectory_in,
                     const std::string& magicDictionary_in,
                     const std::string& itemDictionary_in,
                     const std::string& graphicsDictionary_in,
                     const std::string& graphicsDirectory_in,
                     const std::string& UIDefinitionFile_in)
{
  RPG_TRACE(ACE_TEXT("test_u_main::do_work"));

  // step0a: initialize RPG engine
  std::string empty;
  RPG_Engine_Common_Tools::init(schemaDirectory_in,
                                magicDictionary_in,
                                itemDictionary_in,
                                empty);
  RPG_Client_SDL_InputConfiguration_t input_configuration;
  input_configuration.key_repeat_initial_delay = SDL_DEFAULT_REPEAT_DELAY;
  input_configuration.key_repeat_interval = SDL_DEFAULT_REPEAT_INTERVAL;
  input_configuration.use_UNICODE = true;
  RPG_Sound_SDLConfiguration_t sound_configuration;
  sound_configuration.frequency = RPG_SOUND_AUDIO_DEF_FREQUENCY;
  sound_configuration.format = RPG_SOUND_AUDIO_DEF_FORMAT;
  sound_configuration.channels = RPG_SOUND_AUDIO_DEF_CHANNELS;
  sound_configuration.chunksize = RPG_SOUND_AUDIO_DEF_CHUNKSIZE;
  if (!RPG_Client_Common_Tools::initialize(input_configuration,
                                           sound_configuration,
                                           empty,
                                           RPG_SOUND_AMBIENT_DEF_USE_CD,
                                           RPG_SOUND_DEF_CACHESIZE,
                                           false,
                                           empty,
                                           //
                                           graphicsDirectory_in,
                                           RPG_CLIENT_GRAPHICS_DEF_CACHESIZE,
                                           graphicsDictionary_in,
                                           false)) // don't init SDL
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to RPG_Client_Common_Tools::initialize(): \"%m\", returning\n")));
    return;
  } // end IF
  userData_in.GTKState.finalizationHook = idle_finalize_UI_cb;
  userData_in.GTKState.initializationHook = idle_initialize_UI_cb;
  userData_in.GTKState.gladeXML[ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN)] =
    std::make_pair (UIDefinitionFile_in, static_cast<GladeXML*> (NULL));
  userData_in.GTKState.userData = &userData_in;

  userData_in.currentLevel.metadata.environment.plane
      = RPG_COMMON_PLANE_INVALID;
  userData_in.currentLevel.metadata.environment.terrain
      = RPG_COMMON_TERRAIN_INVALID;
  userData_in.currentLevel.metadata.environment.climate
      = RPG_COMMON_CLIMATE_INVALID;
  userData_in.currentLevel.metadata.environment.time
      = RPG_COMMON_TIMEOFDAY_INVALID;
  userData_in.currentLevel.metadata.environment.lighting =
      RPG_COMMON_AMBIENTLIGHTING_INVALID;
  userData_in.currentLevel.metadata.environment.outdoors = false;
  userData_in.isTransient                                = false;
  userData_in.mapConfiguration.corridors                 =
      MAP_GENERATOR_DEF_CORRIDORS;
  userData_in.mapConfiguration.doors                     =
      MAP_GENERATOR_DEF_DOORS;
  userData_in.mapConfiguration.map_size_x                =
      MAP_GENERATOR_DEF_DIMENSION_X;
  userData_in.mapConfiguration.map_size_y                =
      MAP_GENERATOR_DEF_DIMENSION_Y;
  userData_in.mapConfiguration.max_num_doors_per_room    =
      MAP_GENERATOR_DEF_MAX_NUMDOORS_PER_ROOM;
  userData_in.mapConfiguration.maximize_rooms            =
      MAP_GENERATOR_DEF_MAXIMIZE_ROOMSIZE;
  userData_in.mapConfiguration.min_room_size             =
      MAP_GENERATOR_DEF_MIN_ROOMSIZE;
  userData_in.mapConfiguration.num_areas                 =
      MAP_GENERATOR_DEF_NUM_AREAS;
  userData_in.mapConfiguration.square_rooms              =
      MAP_GENERATOR_DEF_SQUARE_ROOMS;
  userData_in.schemaRepository                           = schemaDirectory_in;

  COMMON_UI_GTK_MANAGER_SINGLETON::instance ()->start ();
  if (!COMMON_UI_GTK_MANAGER_SINGLETON::instance ()->isRunning ())
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to start GTK event dispatch, returning\n")));
    return;
  } // end IF

  int result = COMMON_UI_GTK_MANAGER_SINGLETON::instance ()->wait ();
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Task_Base::wait(): \"%m\", returning\n")));
    return;
  } // end IF

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("finished working...\n")));
}

void
test_u_main::print_version(const std::string& programName_in)
{
  RPG_TRACE(ACE_TEXT("test_u_main::print_version"));

  std::cout << programName_in
            << ACE_TEXT(" : ")
            << YARP_VERSION
            << std::endl;

  // create version string...
  // *NOTE*: cannot use ACE_VERSION, as it doesn't contain the (potential) beta
  // version number... (need this, as the library soname is compared to this
  // string)
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
test_u_main::run_i(int argc_in,
                   char* argv_in[])
{
  RPG_TRACE(ACE_TEXT("test_u_main::run_i"));

  // *PORTABILITY*: on Windows, init ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (ACE::init() == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE::init(): \"%m\", aborting\n")));
    return EXIT_FAILURE;
  } // end IF
#endif

  // step1 init/validate configuration
  std::string configuration_path =
      RPG_Common_File_Tools::getConfigurationDataDirectory(ACE_TEXT_ALWAYS_CHAR(BASEDIR),
                                                           true);
  std::string data_path =
      RPG_Common_File_Tools::getConfigurationDataDirectory(ACE_TEXT_ALWAYS_CHAR(BASEDIR),
                                                           false);
#if defined(DEBUG_DEBUGGER)
  configuration_path = Common_File_Tools::getWorkingDirectory();
  data_path = Common_File_Tools::getWorkingDirectory();
#endif

  // step1a: process commandline arguments
  std::string graphics_dictionary = configuration_path;
  graphics_dictionary += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  graphics_dictionary += ACE_TEXT_ALWAYS_CHAR(RPG_GRAPHICS_DATA_SUB);
  graphics_dictionary += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  graphics_dictionary += ACE_TEXT_ALWAYS_CHAR(RPG_GRAPHICS_DICTIONARY_FILE);

  std::string item_dictionary     = configuration_path;
  item_dictionary += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  item_dictionary += ACE_TEXT_ALWAYS_CHAR("item");
  item_dictionary += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  item_dictionary += ACE_TEXT_ALWAYS_CHAR(RPG_ITEM_DICTIONARY_FILE);

  std::string magic_dictionary    = configuration_path;
  magic_dictionary += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  magic_dictionary += ACE_TEXT_ALWAYS_CHAR("magic");
  magic_dictionary += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  magic_dictionary += ACE_TEXT_ALWAYS_CHAR(RPG_MAGIC_DICTIONARY_FILE);

  std::string UI_file             = configuration_path;
  UI_file += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  UI_file += ACE_TEXT_ALWAYS_CHAR("test_u");
  UI_file += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  UI_file += ACE_TEXT_ALWAYS_CHAR("map");
  UI_file += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  UI_file += ACE_TEXT_ALWAYS_CHAR(MAP_GENERATOR_GUI_GNOME_UI_FILE);

  std::string schema_repository   = configuration_path;
#if defined (DEBUG_DEBUGGER)
  schema_repository += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  schema_repository += ACE_TEXT_ALWAYS_CHAR("engine");
#endif

  std::string graphics_directory  = data_path;
  graphics_directory += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  graphics_directory += ACE_TEXT_ALWAYS_CHAR(RPG_GRAPHICS_DATA_SUB);
#if defined (DEBUG_DEBUGGER)
  graphics_directory += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  graphics_directory += ACE_TEXT_ALWAYS_CHAR(RPG_COMMON_DATA_SUB);
#endif

  // sanity check
  if (!Common_File_Tools::isDirectory(schema_repository))
  {
    ACE_DEBUG((LM_WARNING,
               ACE_TEXT("invalid XML schema repository (was: \"%s\"), continuing\n"),
               ACE_TEXT(schema_repository.c_str())));

    // try fallback
    schema_repository.clear();
  } // end IF

  bool log_to_file = false;
  bool trace_information = false;
  bool print_version_and_exit = false;
  if (!process_arguments(argc_in,
                         argv_in,
                         graphics_dictionary,
                         item_dictionary,
                         log_to_file,
                         magic_dictionary,
                         trace_information,
                         UI_file,
                         graphics_directory,
                         print_version_and_exit))
  {
    // make 'em learn...
    print_usage(ACE::basename(argv_in[0]));

    // *PORTABILITY*: on Windows, need to fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini() == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE::fini(): \"%m\", continuing\n")));
#endif

    return EXIT_FAILURE;
  } // end IF

  // validate argument(s)
  if (!Common_File_Tools::isReadable(graphics_dictionary) ||
      !Common_File_Tools::isReadable(item_dictionary) ||
      !Common_File_Tools::isReadable(magic_dictionary) ||
      !Common_File_Tools::isReadable(UI_file) ||
      !Common_File_Tools::isDirectory(graphics_directory))
  {
    // make 'em learn...
    print_usage(ACE::basename(argv_in[0]));

    // *PORTABILITY*: on Windows, need to fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini() == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE::fini(): \"%m\", continuing\n")));
#endif

    return EXIT_FAILURE;
  } // end IF

  // step1b: handle specific program modes
  if (print_version_and_exit)
  {
    print_version(ACE::basename(argv_in[0]));

    // *PORTABILITY*: on Windows, need to fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini() == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE::fini(): \"%m\", continuing\n")));
#endif

    return EXIT_SUCCESS;
  } // end IF

  // step1c: initialize logging and/or tracing
  std::string log_file;
  if (!Common_Tools::initializeLogging(ACE::basename(argv_in[0]), // program name
                                       log_file,                  // logfile
                                       false,                     // log to syslog ?
                                       false,                     // trace messages ?
                                       trace_information,         // debug messages ?
                                       NULL))                     // logger
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to Common_Tools::initializeLogging(), aborting\n")));

    // *PORTABILITY*: on Windows, need to fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini() == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE::fini(): \"%m\", continuing\n")));
#endif

    return EXIT_FAILURE;
  } // end IF

  // step2b: init GLIB / G(D|T)K[+] / GNOME
//   GnomeClient* gnomeSession = NULL;
//   gnomeSession = gnome_client_new();
//   ACE_ASSERT(gnomeSession);
//   gnome_client_set_program(gnomeSession, ACE::basename(argv_in[0]));
//  GnomeProgram* gnomeProgram = NULL;
//  gnomeProgram = gnome_program_init(RPG_CHARACTER_GENERATOR_GUI_DEF_GNOME_APPLICATION_ID, // app ID
//#ifdef HAVE_CONFIG_H
////                                     ACE_TEXT_ALWAYS_CHAR(VERSION),     // version
//                                    ACE_TEXT_ALWAYS_CHAR(RPG_VERSION),    // version
//#else
//	                                NULL,
//#endif
//                                    LIBGNOMEUI_MODULE,                    // module info
//                                    argc_in,                              // cmdline
//                                    argv_in,                              // cmdline
//                                    NULL);                                // property name(s)
//  ACE_ASSERT(gnomeProgram);
  GTK_cb_data_t user_data;
  Common_UI_GladeDefinition ui_definition (argc_in,
                                           argv_in);
  COMMON_UI_GTK_MANAGER_SINGLETON::instance ()->initialize (argc_in,
                                                            argv_in,
                                                            &user_data.GTKState,
                                                            &ui_definition);

  ACE_High_Res_Timer timer;
  timer.start();
  // step3: do actual work
  do_work(user_data,
          schema_repository,
          magic_dictionary,
          item_dictionary,
          graphics_dictionary,
          graphics_directory,
          UI_file);
  timer.stop();
  // debug info
  std::string working_time_string;
  ACE_Time_Value working_time;
  timer.elapsed_time(working_time);
  Common_Tools::period2String(working_time,
                              working_time_string);

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("total working time (h:m:s.us): \"%s\"...\n"),
             ACE_TEXT(working_time_string.c_str())));

  // *PORTABILITY*: on Windows, fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (ACE::fini() == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE::fini(): \"%m\", aborting\n")));
    return EXIT_FAILURE;
  } // end IF
#endif

  return EXIT_SUCCESS;
}
