#include "stdafx.h"
#include "test_u_main.h"

#include <string>

#include "ace/ACE.h"
#if defined(ACE_WIN32) || defined(ACE_WIN64)
#include "ace/Init_ACE.h"
#endif
#include "ace/Get_Opt.h"
#include "ace/Global_Macros.h"
#include "ace/High_Res_Timer.h"

#include "common_file_tools.h"

#include "common_timer_tools.h"

#include "common_log_tools.h"

#include "common_ui_defines.h"

//#include "common_ui_glade_definition.h"
#include "common_ui_gtk_builder_definition.h"
#include "common_ui_gtk_common.h"
#include "common_ui_gtk_manager.h"
#include "common_ui_gtk_manager_common.h"

#ifdef HAVE_CONFIG_H
#include "rpg_config.h"
#endif

#include "rpg_common_defines.h"
#include "rpg_common_file_tools.h"
#include "rpg_common_macros.h"

#include "rpg_magic_defines.h"

#include "rpg_item_defines.h"

#include "rpg_player_common_tools.h"
#include "rpg_player_defines.h"

#include "rpg_engine_common_tools.h"

#include "rpg_sound_defines.h"

#include "rpg_graphics_defines.h"

#include "rpg_client_callbacks.h"
#include "rpg_client_common.h"
#include "rpg_client_common_tools.h"
#include "rpg_client_defines.h"

#include "character_generator_gui_callbacks.h"

#define CHARACTER_GENERATOR_GUI_GNOME_APPLICATION_ID "character generator"

test_u_main::test_u_main (void)
{
  RPG_TRACE (ACE_TEXT ("test_u_main::test_u_main"));

}

test_u_main::~test_u_main (void)
{
  RPG_TRACE (ACE_TEXT ("test_u_main::~test_u_main"));

}

void
test_u_main::print_usage(const std::string& programName_in)
{
  RPG_TRACE(ACE_TEXT("test_u_main::print_usage"));

  // enable verbatim boolean output
  std::cout.setf(ios::boolalpha);

  std::string configuration_path =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (BASEDIR),
                                                          true);
  std::string data_path =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (BASEDIR),
                                                          false);
#if defined (DEBUG_DEBUGGER)
  configuration_path = Common_File_Tools::getWorkingDirectory ();
  data_path = Common_File_Tools::getWorkingDirectory ();
#endif

  std::cout << ACE_TEXT("usage: ")
            << programName_in
            << ACE_TEXT(" [OPTIONS]")
            << std::endl
            << std::endl;
  std::cout << ACE_TEXT("currently available options:") << std::endl;
  std::string path = configuration_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  path += ACE_TEXT_ALWAYS_CHAR ("client");
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  path += ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_UI_FILE);
  std::cout << ACE_TEXT ("-c [FILE]   : client UI file")
    << ACE_TEXT (" [\"")
    << path
    << ACE_TEXT ("\"]")
    << std::endl;
  path = data_path;
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
  path += ACE_TEXT_ALWAYS_CHAR("character");
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path += ACE_TEXT_ALWAYS_CHAR("player");
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  path += ACE_TEXT_ALWAYS_CHAR(CHARACTER_GENERATOR_GTK_GLADE_FILE);
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
test_u_main::process_arguments (const int& argc_in,
                                ACE_TCHAR** argv_in, // cannot be const...
                                std::string& clientUIFile_out,
                                std::string& graphicsDictionary_out,
                                std::string& itemDictionary_out,
                                bool& logToFile_out,
                                std::string& magicDictionary_out,
                                bool& traceInformation_out,
                                std::string& UIFile_out,
                                std::string& graphicsDirectory_out,
                                bool& printVersionAndExit_out)
{
  RPG_TRACE (ACE_TEXT ("test_u_main::process_arguments"));

  // init configuration
  std::string configuration_path =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (BASEDIR),
                                                          true);
  std::string data_path =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (BASEDIR),
                                                          false);
#if defined (DEBUG_DEBUGGER)
  configuration_path = Common_File_Tools::getWorkingDirectory ();
  data_path = Common_File_Tools::getWorkingDirectory ();
#endif

  clientUIFile_out = configuration_path;
  clientUIFile_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  clientUIFile_out += ACE_TEXT_ALWAYS_CHAR ("client");
  clientUIFile_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  clientUIFile_out += ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_UI_FILE);

  graphicsDictionary_out  = configuration_path;
  graphicsDictionary_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  graphicsDictionary_out += ACE_TEXT_ALWAYS_CHAR ("graphics");
  graphicsDictionary_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  graphicsDictionary_out +=
    ACE_TEXT_ALWAYS_CHAR (RPG_GRAPHICS_DICTIONARY_FILE);

  itemDictionary_out      = configuration_path;
  itemDictionary_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  itemDictionary_out += ACE_TEXT_ALWAYS_CHAR ("item");
  itemDictionary_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  itemDictionary_out += ACE_TEXT_ALWAYS_CHAR (RPG_ITEM_DICTIONARY_FILE);

  logToFile_out = false;

  magicDictionary_out = configuration_path;
  magicDictionary_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  magicDictionary_out += ACE_TEXT_ALWAYS_CHAR ("magic");
  magicDictionary_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  magicDictionary_out += ACE_TEXT_ALWAYS_CHAR (RPG_MAGIC_DICTIONARY_FILE);

  traceInformation_out = false;

  UIFile_out = configuration_path;
  UIFile_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  UIFile_out += ACE_TEXT_ALWAYS_CHAR ("test_u");
  UIFile_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  UIFile_out += ACE_TEXT_ALWAYS_CHAR ("character");
  UIFile_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  UIFile_out += ACE_TEXT_ALWAYS_CHAR ("player");
  UIFile_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  UIFile_out += CHARACTER_GENERATOR_GTK_GLADE_FILE;

  graphicsDirectory_out   = data_path;
  graphicsDirectory_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  graphicsDirectory_out += ACE_TEXT_ALWAYS_CHAR ("graphics");
  graphicsDirectory_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  graphicsDirectory_out += ACE_TEXT_ALWAYS_CHAR (RPG_COMMON_DATA_SUB);
#else
  graphicsDirectory_out += ACE_TEXT_ALWAYS_CHAR(RPG_GRAPHICS_DATA_SUB);
#endif

  printVersionAndExit_out = false;

  ACE_Get_Opt argumentParser(argc_in,
                             argv_in,
                             ACE_TEXT("c:d:g:i:lm:tu:v"),
                             1,                         // skip command name
                             1,                         // report parsing errors
                             ACE_Get_Opt::PERMUTE_ARGS, // ordering
                             0);                        // don't use long options

  int option = 0;
  while ((option = argumentParser()) != EOF)
  {
    switch (option)
    {
      case 'c':
      {
        clientUIFile_out = ACE_TEXT_ALWAYS_CHAR (argumentParser.opt_arg ());

        break;
      }
      case 'd':
      {
        graphicsDirectory_out = ACE_TEXT_ALWAYS_CHAR (argumentParser.opt_arg ());

        break;
      }
      case 'g':
      {
        graphicsDictionary_out = ACE_TEXT_ALWAYS_CHAR (argumentParser.opt_arg ());

        break;
      }
      case 'i':
      {
        itemDictionary_out = ACE_TEXT_ALWAYS_CHAR (argumentParser.opt_arg ());

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
      case 't':
      {
        traceInformation_out = true;

        break;
      }
      case 'u':
      {
        UIFile_out = ACE_TEXT_ALWAYS_CHAR (argumentParser.opt_arg ());

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
test_u_main::do_work (struct GTK_CBData& userData_in,
                      const std::string& schemaDirectory_in,
                      const std::string& clientUIFile_in,
                      const std::string& magicDictionary_in,
                      const std::string& itemDictionary_in,
                      const std::string& graphicsDictionary_in,
                      const std::string& graphicsDirectory_in,
                      const std::string& UIFile_in)
{
  RPG_TRACE (ACE_TEXT ("test_u_main::do_work"));

  //// step0a: init XML parsing
  //RPG_Common_XML_Tools::init(schemaDirectory_in);
  // step0b: init RPG engine
  std::string empty;
  RPG_Engine_Common_Tools::initialize (schemaDirectory_in,
                                       magicDictionary_in,
                                       itemDictionary_in,
                                       empty);
  struct RPG_Client_SDL_InputConfiguration input_configuration;
  input_configuration.key_repeat_initial_delay = SDL_DEFAULT_REPEAT_DELAY;
  input_configuration.key_repeat_interval = SDL_DEFAULT_REPEAT_INTERVAL;
  input_configuration.use_UNICODE = true;
  struct RPG_Sound_SDLConfiguration sound_configuration;
  sound_configuration.frequency = RPG_SOUND_AUDIO_DEF_FREQUENCY;
  sound_configuration.format = RPG_SOUND_AUDIO_DEF_FORMAT;
  sound_configuration.channels = RPG_SOUND_AUDIO_DEF_CHANNELS;
  sound_configuration.chunksize = RPG_SOUND_AUDIO_DEF_CHUNKSIZE;
  if (!RPG_Client_Common_Tools::initialize (input_configuration,
                                            sound_configuration,
                                            empty,
                                            RPG_SOUND_AMBIENT_DEF_USE_CD,
                                            RPG_SOUND_DEF_CACHESIZE,
                                            false,
                                            empty,
                                            //
                                            graphicsDirectory_in,
                                            RPG_GRAPHICS_DEF_CACHESIZE,
                                            graphicsDictionary_in,
                                            false)) // don't init SDL
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to RPG_Client_Common_Tools::initialize(): \"%m\", returning\n")));
    return;
  } // end IF

  Common_UI_GTK_Manager_t* gtk_manager_p =
    COMMON_UI_GTK_MANAGER_SINGLETON::instance ();
  ACE_ASSERT (gtk_manager_p);
  Common_UI_GTK_State_t& state_r =
    const_cast<Common_UI_GTK_State_t&> (gtk_manager_p->getR_2 ());
  state_r.builders[ACE_TEXT_ALWAYS_CHAR (COMMON_UI_DEFINITION_DESCRIPTOR_MAIN)] =
    std::make_pair (UIFile_in, static_cast<GtkBuilder*> (NULL));
  state_r.builders[ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_DEFINITION_DESCRIPTOR_MAIN)] =
    std::make_pair (clientUIFile_in, static_cast<GtkBuilder*> (NULL));
  //userData_in.GTKState.userData = &userData_in;

  userData_in.schemaRepository = schemaDirectory_in;
  userData_in.entity.character = NULL;
  userData_in.entity.position =
    std::make_pair (std::numeric_limits<unsigned int>::max (),
                    std::numeric_limits<unsigned int>::max ());
  userData_in.entity.modes.clear ();
  userData_in.entity.actions.clear ();
  userData_in.entity.is_spawned = false;
  // init sprite gallery
  for (int index = 0;
       index < RPG_GRAPHICS_SPRITE_MAX;
       index++)
    userData_in.spriteGallery.push_back (static_cast<RPG_Graphics_Sprite> (index));
  userData_in.spriteGalleryIterator = userData_in.spriteGallery.begin ();
  userData_in.currentSprite = RPG_GRAPHICS_SPRITE_INVALID;
  userData_in.isTransient = false;

  ACE_thread_t thread_id;
  COMMON_UI_GTK_MANAGER_SINGLETON::instance ()->start (thread_id);
  if (!COMMON_UI_GTK_MANAGER_SINGLETON::instance ()->isRunning ())
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to start GTK event dispatch, returning\n")));
    return;
  } // end IF

  COMMON_UI_GTK_MANAGER_SINGLETON::instance ()->wait ();

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("finished working...\n")));
}

void
test_u_main::print_version (const std::string& programName_in)
{
  RPG_TRACE (ACE_TEXT ("test_u_main::print_version"));

  std::cout << programName_in
#ifdef HAVE_CONFIG_H
            << ACE_TEXT(" : ")
            //<< RPG_VERSION
#endif
            << std::endl;

  // create version string...
  // *NOTE*: cannot use ACE_VERSION, as it doesn't contain the (potential) beta
  // version number... We need this, as the library soname is compared to this
  // string
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
test_u_main::run_i (int argc_in,
                    char* argv_in[])
{
  RPG_TRACE (ACE_TEXT ("test_u_main::run_i"));

  // *PORTABILITY*: on Windows, need to init ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (ACE::init () == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE::init(): \"%m\", aborting\n")));
    return EXIT_FAILURE;
  } // end IF
#endif

  // step1 init/validate configuration
  std::string configuration_path =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (BASEDIR),
                                                          true);
  std::string data_path =
    RPG_Common_File_Tools::getConfigurationDataDirectory (ACE_TEXT_ALWAYS_CHAR (BASEDIR),
                                                          false);
#if defined(DEBUG_DEBUGGER)
  configuration_path = Common_File_Tools::getWorkingDirectory ();
  data_path = Common_File_Tools::getWorkingDirectory ();
#endif

  // step1a: process commandline arguments
  std::string client_ui_file = configuration_path;
  client_ui_file += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  client_ui_file += ACE_TEXT_ALWAYS_CHAR ("client");
  client_ui_file += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  client_ui_file += ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_UI_FILE);;

  std::string graphics_dictionary = configuration_path;
  graphics_dictionary += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  graphics_dictionary += ACE_TEXT_ALWAYS_CHAR("graphics");
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

  std::string ui_file             = configuration_path;
  ui_file += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  ui_file += ACE_TEXT_ALWAYS_CHAR ("test_u");
  ui_file += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  ui_file += ACE_TEXT_ALWAYS_CHAR ("character");
  ui_file += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  ui_file += ACE_TEXT_ALWAYS_CHAR ("player");
  ui_file += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  ui_file += ACE_TEXT_ALWAYS_CHAR (CHARACTER_GENERATOR_GTK_GLADE_FILE);

  std::string schema_repository   = configuration_path;
#if defined (DEBUG_DEBUGGER)
  schema_repository += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  schema_repository += ACE_TEXT_ALWAYS_CHAR("engine");
#endif

  std::string graphics_directory  = data_path;
  graphics_directory += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined (DEBUG_DEBUGGER)
  graphics_directory += ACE_TEXT_ALWAYS_CHAR("graphics");
  graphics_directory += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  graphics_directory += ACE_TEXT_ALWAYS_CHAR(RPG_COMMON_DATA_SUB);
#else
  graphics_directory += ACE_TEXT_ALWAYS_CHAR(RPG_GRAPHICS_DATA_SUB);
#endif

  // sanity check
  if (!Common_File_Tools::isDirectory (schema_repository))
  {
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("invalid XML schema repository (was: \"%s\"), continuing\n"),
                ACE_TEXT (schema_repository.c_str ())));

    // try fallback
    schema_repository.clear();
  } // end IF

  bool log_to_file = false;
  bool trace_information = false;
  bool print_version_and_exit = false;
  if (!process_arguments (argc_in,
                          argv_in,
                          client_ui_file,
                          graphics_dictionary,
                          item_dictionary,
                          log_to_file,
                          magic_dictionary,
                          trace_information,
                          ui_file,
                          graphics_directory,
                          print_version_and_exit))
  {
    // make 'em learn...
    print_usage (ACE::basename (argv_in[0]));

    // *PORTABILITY*: on Windows, need to fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini () == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif

    return EXIT_FAILURE;
  } // end IF

  // validate argument(s)
  if (!Common_File_Tools::isReadable (client_ui_file)      ||
      !Common_File_Tools::isReadable (graphics_dictionary) ||
      !Common_File_Tools::isReadable (item_dictionary)     ||
      !Common_File_Tools::isReadable (magic_dictionary)    ||
      !Common_File_Tools::isReadable (ui_file)             ||
      !Common_File_Tools::isDirectory (graphics_directory))
  {
    // make 'em learn...
    print_usage(ACE::basename(argv_in[0]));

    // *PORTABILITY*: on Windows, need to fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini () == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif

    return EXIT_FAILURE;
  } // end IF

  // step1b: handle specific program modes
  if (print_version_and_exit)
  {
    print_version(ACE::basename(argv_in[0]));

    // *PORTABILITY*: on Windows, need to fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini () == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif

    return EXIT_SUCCESS;
  } // end IF

  // step1c: initialize logging and/or tracing
  std::string log_file;
  if (!Common_Log_Tools::initializeLogging (ACE::basename (argv_in[0]), // program name
                                            log_file,                   // logfile
                                            false,                      // log to syslog ?
                                            false,                      // trace messages ?
                                            trace_information,          // debug messages ?
                                            NULL))                      // logger
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_Tools::initializeLogging(), aborting\n")));

    // *PORTABILITY*: on Windows, need to fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini () == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
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
  struct GTK_CBData user_data;
  Common_UI_GtkBuilderDefinition_t ui_definition;
  struct Common_UI_GTK_Configuration gtk_configuration;
  gtk_configuration.argc = argc_in;
  gtk_configuration.argv = argv_in;
  gtk_configuration.CBData = &user_data;
  gtk_configuration.definition = &ui_definition;
  gtk_configuration.eventHooks.initHook = idle_initialize_UI_cb;
  gtk_configuration.eventHooks.finiHook = idle_finalize_UI_cb;
  if (!COMMON_UI_GTK_MANAGER_SINGLETON::instance ()->initialize (gtk_configuration))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to initialize GTK manager, aborting\n")));

    // *PORTABILITY*: on Windows, need to fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (ACE::fini () == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE::fini(): \"%m\", continuing\n")));
#endif

    return EXIT_FAILURE;
  } // end IF

  ACE_High_Res_Timer timer;
  timer.start ();
  // step3: do actual work
  do_work (user_data,
           schema_repository,
           client_ui_file,
           magic_dictionary,
           item_dictionary,
           graphics_dictionary,
           graphics_directory,
           ui_file);
  timer.stop ();
  // debug info
  std::string working_time_string;
  ACE_Time_Value working_time;
  timer.elapsed_time (working_time);
  working_time_string = Common_Timer_Tools::periodToString (working_time);

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("total working time (h:m:s.us): \"%s\"...\n"),
              ACE_TEXT (working_time_string.c_str ())));

  // *PORTABILITY*: on Windows, fini ACE...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (ACE::fini () == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE::fini(): \"%m\", aborting\n")));
    return EXIT_FAILURE;
  } // end IF
#endif

  return EXIT_SUCCESS;
}
