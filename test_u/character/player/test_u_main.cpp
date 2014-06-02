#include "stdafx.h"
#include "test_u_main.h"

// *NOTE*: need this to import correct PACKAGE_STRING/VERSION/... !
#ifdef HAVE_CONFIG_H
#include "rpg_config.h"
#endif

#include "character_generator_gui_callbacks.h"

#include "rpg_client_defines.h"
#include "rpg_client_callbacks.h"
#include "rpg_client_common_tools.h"

#include "rpg_graphics_defines.h"

#include "rpg_sound_defines.h"

#include "rpg_engine_common_tools.h"

#include "rpg_player_defines.h"
#include "rpg_player_common_tools.h"

#include "rpg_item_defines.h"

#include "rpg_magic_defines.h"

#include "rpg_common_macros.h"
#include "rpg_common_defines.h"
#include "rpg_common_tools.h"
#include "rpg_common_file_tools.h"

#include <gtk/gtk.h>

#include <ace/Global_Macros.h>
#include <ace/Get_Opt.h>
#include <ace/High_Res_Timer.h>

#include <string>

#define RPG_CHARACTER_GENERATOR_GUI_DEF_GNOME_UI_file        ACE_TEXT_ALWAYS_CHAR("character_generator_gui.glade")
#define RPG_CHARACTER_GENERATOR_GUI_DEF_GNOME_APPLICATION_ID ACE_TEXT_ALWAYS_CHAR("character generator")

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

  std::string configuration_path = RPG_Common_File_Tools::getWorkingDirectory();
  std::string data_path = RPG_Common_File_Tools::getWorkingDirectory();
#ifdef BASEDIR
  configuration_path =
      RPG_Common_File_Tools::getConfigurationDataDirectory(ACE_TEXT_ALWAYS_CHAR(BASEDIR),
                                                           true);
  data_path =
      RPG_Common_File_Tools::getConfigurationDataDirectory(ACE_TEXT_ALWAYS_CHAR(BASEDIR),
                                                           false);
#endif // #ifdef BASEDIR

  std::cout << ACE_TEXT("usage: ")
            << programName_in
            << ACE_TEXT(" [OPTIONS]")
            << std::endl
            << std::endl;
  std::cout << ACE_TEXT("currently available options:") << std::endl;
  std::string path = data_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined(DEBUG_DEBUGGER)
  path += ACE_TEXT_ALWAYS_CHAR("graphics");
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path += ACE_TEXT_ALWAYS_CHAR(RPG_COMMON_DATA_SUB);
#else
  path += ACE_TEXT_ALWAYS_CHAR(RPG_GRAPHICS_DEF_DATA_SUB);
#endif
  std::cout << ACE_TEXT("-d [DIR]    : graphics directory")
            << ACE_TEXT(" [\"")
            << ACE_TEXT(path.c_str())
            << ACE_TEXT("\"]")
            << std::endl;
  path = configuration_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined(DEBUG_DEBUGGER)
  path += ACE_TEXT_ALWAYS_CHAR("graphics");
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  path += ACE_TEXT_ALWAYS_CHAR(RPG_GRAPHICS_DEF_DICTIONARY_FILE);
  std::cout << ACE_TEXT("-g [FILE]   : graphics dictionary (*.xml)")
            << ACE_TEXT(" [\"")
            << ACE_TEXT(path.c_str())
            << ACE_TEXT("\"]")
            << std::endl;
  path = configuration_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined(DEBUG_DEBUGGER)
  path += ACE_TEXT_ALWAYS_CHAR("item");
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  path += ACE_TEXT_ALWAYS_CHAR(RPG_ITEM_DEF_DICTIONARY_FILE);
  std::cout << ACE_TEXT("-i [FILE]   : item dictionary (*.xml)")
            << ACE_TEXT(" [\"")
            << ACE_TEXT(path.c_str())
            << ACE_TEXT("\"]")
            << std::endl;
  std::cout << ACE_TEXT("-l          : log to a file")
            << ACE_TEXT(" [")
            << false
            << ACE_TEXT("]")
            << std::endl;
  path = configuration_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined(DEBUG_DEBUGGER)
  path += ACE_TEXT_ALWAYS_CHAR("magic");
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  path += ACE_TEXT_ALWAYS_CHAR(RPG_MAGIC_DEF_DICTIONARY_FILE);
  std::cout << ACE_TEXT("-m [FILE]   : magic dictionary (*.xml)")
            << ACE_TEXT(" [\"")
            << ACE_TEXT(path.c_str())
            << ACE_TEXT("\"]")
            << std::endl;
  std::cout << ACE_TEXT("-t          : trace information")
            << ACE_TEXT(" [")
            << false
            << ACE_TEXT("]")
            << std::endl;
  path = configuration_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined(DEBUG_DEBUGGER)
  path += ACE_TEXT_ALWAYS_CHAR("test_u");
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path += ACE_TEXT_ALWAYS_CHAR("character");
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path += ACE_TEXT_ALWAYS_CHAR("player");
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  path += RPG_CHARACTER_GENERATOR_GUI_DEF_GNOME_UI_file;
  std::cout << ACE_TEXT("-u [FILE]   : UI file")
            << ACE_TEXT(" [\"")
            << ACE_TEXT(path.c_str())
            << ACE_TEXT("\"]")
            << std::endl;
  std::cout << ACE_TEXT("-v          : print version information and exit")
            << ACE_TEXT(" [")
            << false
            << ACE_TEXT("]")
            << std::endl;
} // end print_usage

bool
test_u_main::process_arguments(const int argc_in,
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
  std::string configuration_path = RPG_Common_File_Tools::getWorkingDirectory();
  std::string data_path = RPG_Common_File_Tools::getWorkingDirectory();
#ifdef BASEDIR
  configuration_path =
      RPG_Common_File_Tools::getConfigurationDataDirectory(ACE_TEXT_ALWAYS_CHAR(BASEDIR),
                                                           true);
  data_path =
      RPG_Common_File_Tools::getConfigurationDataDirectory(ACE_TEXT_ALWAYS_CHAR(BASEDIR),
                                                           false);
#endif // #ifdef BASEDIR

  graphics_dictionary_out = configuration_path;
  graphics_dictionary_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined(DEBUG_DEBUGGER)
  graphics_dictionary_out += ACE_TEXT_ALWAYS_CHAR("graphics");
  graphics_dictionary_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  graphics_dictionary_out +=
      ACE_TEXT_ALWAYS_CHAR(RPG_GRAPHICS_DEF_DICTIONARY_FILE);

  item_dictionary_out = configuration_path;
  item_dictionary_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined(DEBUG_DEBUGGER)
  item_dictionary_out += ACE_TEXT_ALWAYS_CHAR("item");
  item_dictionary_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  item_dictionary_out += ACE_TEXT_ALWAYS_CHAR(RPG_ITEM_DEF_DICTIONARY_FILE);

  logToFile_out      = false;

  magic_dictionary_out = configuration_path;
  magic_dictionary_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined(DEBUG_DEBUGGER)
  magic_dictionary_out += ACE_TEXT_ALWAYS_CHAR("magic");
  magic_dictionary_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  magic_dictionary_out += ACE_TEXT_ALWAYS_CHAR(RPG_MAGIC_DEF_DICTIONARY_FILE);

  traceInformation_out = false;

  UI_file_out = configuration_path;
  UI_file_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined(DEBUG_DEBUGGER)
  UI_file_out += ACE_TEXT_ALWAYS_CHAR("test_u");
  UI_file_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  UI_file_out += ACE_TEXT_ALWAYS_CHAR("character");
  UI_file_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  UI_file_out += ACE_TEXT_ALWAYS_CHAR("player");
  UI_file_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  UI_file_out += RPG_CHARACTER_GENERATOR_GUI_DEF_GNOME_UI_file;

  graphics_directory_out = data_path;
  graphics_directory_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined(DEBUG_DEBUGGER)
  graphics_directory_out += ACE_TEXT_ALWAYS_CHAR("graphics");
  graphics_directory_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  graphics_directory_out += ACE_TEXT_ALWAYS_CHAR(RPG_COMMON_DATA_SUB);
#else
  graphics_directory_out += ACE_TEXT_ALWAYS_CHAR(RPG_GRAPHICS_DEF_DATA_SUB);
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

bool
test_u_main::init_GUI(const std::string& graphics_directory_in,
											const std::string& UI_file_in,
											GTK_cb_data_t& userData_in)
{
  RPG_TRACE(ACE_TEXT("test_u_main::init_GUI"));

  // sanity check(s)
  if (!RPG_Common_File_Tools::isDirectory(graphics_directory_in.c_str()))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid directory \"%s\", aborting\n"),
               graphics_directory_in.c_str()));

    return false;
  } // end IF
  if (!RPG_Common_File_Tools::isReadable(UI_file_in.c_str()))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("UI definition file \"%s\" doesn't exist, aborting\n"),
               UI_file_in.c_str()));

    return false;
  } // end IF
  ACE_ASSERT(userData_in.xml == NULL);

  // step1: load widget tree
  userData_in.xml = glade_xml_new(UI_file_in.c_str(), // definition file
                                  NULL,              // root widget --> construct all
                                  NULL);             // domain
  if (!userData_in.xml)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to glade_xml_new(): \"%m\", aborting\n")));

    return false;
  } // end IF

  // step2: retrieve dialog handles
	GtkWidget* main_dialog = NULL;
	main_dialog =
			GTK_WIDGET(glade_xml_get_widget(userData_in.xml,
																			ACE_TEXT_ALWAYS_CHAR("main_dialog")));
  ACE_ASSERT(main_dialog);

  GtkWidget* about_dialog =
      GTK_WIDGET(glade_xml_get_widget(userData_in.xml,
                                      ACE_TEXT_ALWAYS_CHAR("about_dialog")));
  ACE_ASSERT(about_dialog);

  // step3: populate combobox
  GtkComboBox* combobox =
      GTK_COMBO_BOX(glade_xml_get_widget(userData_in.xml,
                                         ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GNOME_CHARBOX_NAME)));
  ACE_ASSERT(combobox);
  gtk_cell_layout_clear(GTK_CELL_LAYOUT(combobox));
  GtkCellRenderer* renderer = gtk_cell_renderer_text_new();
  ACE_ASSERT(renderer);
  gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(combobox), renderer,
                             TRUE); // expand ?
  //   gtk_cell_layout_add_attribute(GTK_CELL_LAYOUT(combobox), renderer,
//                                 ACE_TEXT_ALWAYS_CHAR("text"), 0);
  gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(combobox), renderer,
                                 ACE_TEXT_ALWAYS_CHAR("text"), 0,
                                 NULL);
  GtkListStore* list = gtk_list_store_new(1,
                                          G_TYPE_STRING);
  ACE_ASSERT(list);
  gtk_combo_box_set_model(combobox,
                          GTK_TREE_MODEL(list));
  g_object_unref(G_OBJECT(list));
  std::string profiles_directory =
      RPG_Player_Common_Tools::getPlayerProfilesDirectory();
	unsigned int num_entries = ::load_files(profiles_directory,
																					true,
																					list);

  // step4a: connect default signals
  gpointer userData_p = const_cast<GTK_cb_data_t*>(&userData_in);
  g_signal_connect(main_dialog,
                   ACE_TEXT_ALWAYS_CHAR("destroy"),
                   G_CALLBACK(quit_clicked_GTK_cb),
//                    G_CALLBACK(gtk_widget_destroyed),
//                    &main_dialog,
                   userData_p);
  g_signal_connect(about_dialog,
                   ACE_TEXT_ALWAYS_CHAR("response"),
                   G_CALLBACK(gtk_widget_hide),
                   &about_dialog);

   // step4b: connect custom signals
  GtkButton* button = NULL;
  button = GTK_BUTTON(glade_xml_get_widget(userData_in.xml,
                                           ACE_TEXT_ALWAYS_CHAR("create")));
  ACE_ASSERT(button);
  g_signal_connect(button,
                   ACE_TEXT_ALWAYS_CHAR("clicked"),
                   G_CALLBACK(create_character_clicked_GTK_cb),
                   userData_p);

  button = GTK_BUTTON(glade_xml_get_widget(userData_in.xml,
                                           ACE_TEXT_ALWAYS_CHAR("drop")));
  ACE_ASSERT(button);
  g_signal_connect(button,
                   ACE_TEXT_ALWAYS_CHAR("clicked"),
                   G_CALLBACK(drop_character_clicked_GTK_cb),
                   userData_p);

  GtkFileChooserDialog* filechooser_dialog =
      GTK_FILE_CHOOSER_DIALOG(glade_xml_get_widget(userData_in.xml,
                                                   ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GNOME_FILECHOOSERDIALOG_NAME)));
  ACE_ASSERT(filechooser_dialog);
	GtkFileFilter* file_filter = gtk_file_filter_new();
  ACE_ASSERT(file_filter);
  if (!file_filter)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to gtk_file_filter_new(): \"%m\", aborting\n")));

    // clean up
    g_object_unref(G_OBJECT(userData_in.xml));
    userData_in.xml = NULL;

    return false;
  } // end IF
  gtk_file_filter_set_name(file_filter,
                           ACE_TEXT(RPG_PLAYER_PROFILE_EXT));
  std::string pattern = ACE_TEXT_ALWAYS_CHAR("*");
  pattern += ACE_TEXT_ALWAYS_CHAR(RPG_PLAYER_PROFILE_EXT);
  gtk_file_filter_add_pattern(file_filter, ACE_TEXT(pattern.c_str()));
  gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(filechooser_dialog),
                              file_filter);
  //gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(filechooser_dialog),
  //                                    ACE_TEXT(profiles_directory.c_str()));
  g_signal_connect(filechooser_dialog,
                   ACE_TEXT_ALWAYS_CHAR("file-activated"),
                   G_CALLBACK(character_file_activated_GTK_cb),
                   userData_p);

  button = GTK_BUTTON(glade_xml_get_widget(userData_in.xml,
                                           ACE_TEXT_ALWAYS_CHAR("load")));
  ACE_ASSERT(button);
  g_signal_connect(button,
                   ACE_TEXT_ALWAYS_CHAR("clicked"),
                   G_CALLBACK(load_character_clicked_GTK_cb),
                   userData_p);

  button = GTK_BUTTON(glade_xml_get_widget(userData_in.xml,
                                           ACE_TEXT_ALWAYS_CHAR("save")));
  ACE_ASSERT(button);
  g_signal_connect(button,
                   ACE_TEXT_ALWAYS_CHAR("clicked"),
                   G_CALLBACK(save_character_clicked_GTK_cb),
                   userData_p);

  combobox =
      GTK_COMBO_BOX(glade_xml_get_widget(userData_in.xml,
                                         ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GNOME_CHARBOX_NAME)));
  ACE_ASSERT(combobox);
  g_signal_connect(combobox,
                   ACE_TEXT_ALWAYS_CHAR("changed"),
                   G_CALLBACK(character_repository_combobox_changed_GTK_cb),
                   userData_p);

  button = GTK_BUTTON(glade_xml_get_widget(userData_in.xml,
                                           ACE_TEXT_ALWAYS_CHAR("refresh")));
  ACE_ASSERT(button);
  g_signal_connect(button,
                   ACE_TEXT_ALWAYS_CHAR("clicked"),
                   G_CALLBACK(character_repository_button_clicked_GTK_cb),
                   userData_p);

  button =
      GTK_BUTTON(glade_xml_get_widget(userData_in.xml,
                                      ACE_TEXT_ALWAYS_CHAR("image_button_prev")));
  ACE_ASSERT(button);
  g_signal_connect(button,
                   ACE_TEXT_ALWAYS_CHAR("clicked"),
                   G_CALLBACK(prev_image_clicked_GTK_cb),
                   userData_p);

  button =
      GTK_BUTTON(glade_xml_get_widget(userData_in.xml,
                                      ACE_TEXT_ALWAYS_CHAR("image_button_next")));
  ACE_ASSERT(button);
  g_signal_connect(button,
                   ACE_TEXT_ALWAYS_CHAR("clicked"),
                   G_CALLBACK(next_image_clicked_GTK_cb),
                   userData_p);

  button = GTK_BUTTON(glade_xml_get_widget(userData_in.xml,
                                           ACE_TEXT_ALWAYS_CHAR("quit")));
  ACE_ASSERT(button);
  g_signal_connect(button,
                   ACE_TEXT_ALWAYS_CHAR("clicked"),
                   G_CALLBACK(quit_clicked_GTK_cb),
                   userData_p);

  button = GTK_BUTTON(glade_xml_get_widget(userData_in.xml,
                                           ACE_TEXT_ALWAYS_CHAR("about")));
  ACE_ASSERT(button);
  g_signal_connect(button,
                   ACE_TEXT_ALWAYS_CHAR("clicked"),
                   G_CALLBACK(about_clicked_GTK_cb),
                   userData_p);

  // step5: auto-connect signals/slots
//   glade_xml_signal_autoconnect(xml);

//   // step6: use correct screen
//   if (parentWidget_in)
//     gtk_window_set_screen(GTK_WINDOW(dialog),
//                           gtk_widget_get_screen(const_cast<GtkWidget*> (//                                                                parentWidget_in)));

  // step6: draw it
  gtk_widget_show_all(main_dialog);

  // step7: activate first repository entry (if any)
	gtk_widget_set_sensitive(GTK_WIDGET(combobox), (num_entries > 0));
  if (num_entries > 0)
    gtk_combo_box_set_active(combobox, 0);
  else
  {
    // make create button sensitive (if it's not already)
    GtkButton* button =
        GTK_BUTTON(glade_xml_get_widget(userData_in.xml,
                                        ACE_TEXT_ALWAYS_CHAR("create")));
    ACE_ASSERT(button);
    gtk_widget_set_sensitive(GTK_WIDGET(button), TRUE);

    // make load button sensitive (if it's not already)
    button = GTK_BUTTON(glade_xml_get_widget(userData_in.xml,
                                             ACE_TEXT_ALWAYS_CHAR("load")));
    ACE_ASSERT(button);
    gtk_widget_set_sensitive(GTK_WIDGET(button), TRUE);
  } // end ELSE

  return true;
}

void
test_u_main::do_work(const std::string& schemaDirectory_in,
										 const std::string& magic_dictionary_in,
										 const std::string& item_dictionary_in,
										 const std::string& graphics_dictionary_in,
										 const std::string& graphics_directory_in,
										 const std::string& UI_file_in)
{
  RPG_TRACE(ACE_TEXT("test_u_main::do_work"));

  // step0a: init RPG engine
  std::string empty;
  RPG_Engine_Common_Tools::init(schemaDirectory_in,
                                magic_dictionary_in,
                                item_dictionary_in,
                                empty);
	RPG_Client_SDL_InputConfiguration_t input_configuration;
	input_configuration.key_repeat_initial_delay = SDL_DEFAULT_REPEAT_DELAY;
	input_configuration.key_repeat_interval = SDL_DEFAULT_REPEAT_INTERVAL;
	input_configuration.use_UNICODE = true;
	RPG_Sound_SDLConfiguration_t sound_configuration;
	sound_configuration.frequency = RPG_SOUND_DEF_AUDIO_FREQUENCY;
	sound_configuration.format = RPG_SOUND_DEF_AUDIO_FORMAT;
	sound_configuration.channels = RPG_SOUND_DEF_AUDIO_CHANNELS;
	sound_configuration.chunksize = RPG_SOUND_DEF_AUDIO_CHUNKSIZE;
  RPG_Client_Common_Tools::init(input_configuration,
		                            sound_configuration,
                                empty,
                                RPG_SOUND_DEF_AMBIENT_USE_CD,
                                RPG_SOUND_DEF_CACHESIZE,
                                false,
                                empty,
                                //
                                graphics_directory_in,
																RPG_CLIENT_DEF_GRAPHICS_CACHESIZE,
																graphics_dictionary_in,
                                false); // don't init SDL

  GTK_cb_data_t user_data;
  user_data.xml               = NULL;
  user_data.schema_repository = schemaDirectory_in;
  user_data.entity.character  = NULL;
  user_data.entity.position   =
      std::make_pair(std::numeric_limits<unsigned int>::max(),
                     std::numeric_limits<unsigned int>::max());
  user_data.entity.modes.clear();
  user_data.entity.actions.clear();
  user_data.entity.is_spawned = false;
  // init sprite gallery
  for (int index = 0;
       index < RPG_GRAPHICS_SPRITE_MAX;
       index++)
    user_data.sprite_gallery.push_back(static_cast<RPG_Graphics_Sprite>(index));
  user_data.sprite_gallery_iterator = user_data.sprite_gallery.begin();
  user_data.current_sprite = RPG_GRAPHICS_SPRITE_INVALID;
  user_data.is_transient = false;

  if (!init_GUI(graphics_directory_in, // graphics directory
                UI_file_in,            // glade file
                user_data))            // GTK cb data
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to initialize UI, aborting\n")));

    gdk_threads_leave();

    return;
  } // end IF

  // step2: setup event loops
  // - UI events --> GTK main loop
  gtk_main();

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("finished working...\n")));
}

void
test_u_main::print_version(const std::string& programName_in)
{
  RPG_TRACE(ACE_TEXT("test_u_main::print_version"));

  std::cout << programName_in
#ifdef HAVE_CONFIG_H
            << ACE_TEXT(" : ")
            << RPG_VERSION
#endif
            << std::endl;

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
test_u_main::run_i(int argc_in,
		               char* argv_in[])
{
	RPG_TRACE(ACE_TEXT("test_u_main::run_i"));

	// *PORTABILITY*: on Windows, need to init ACE...
#if defined(ACE_WIN32) || defined(ACE_WIN64)
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

  // step1a: process commandline arguments
  std::string graphics_dictionary = configuration_path;
  graphics_dictionary += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined(DEBUG_DEBUGGER)
  graphics_dictionary += ACE_TEXT_ALWAYS_CHAR("graphics");
  graphics_dictionary += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  graphics_dictionary += ACE_TEXT_ALWAYS_CHAR(RPG_GRAPHICS_DEF_DICTIONARY_FILE);

  std::string item_dictionary     = configuration_path;
  item_dictionary += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined(DEBUG_DEBUGGER)
  item_dictionary += ACE_TEXT_ALWAYS_CHAR("item");
  item_dictionary += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  item_dictionary += ACE_TEXT_ALWAYS_CHAR(RPG_ITEM_DEF_DICTIONARY_FILE);

  std::string magic_dictionary    = configuration_path;
  magic_dictionary += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined(DEBUG_DEBUGGER)
  magic_dictionary += ACE_TEXT_ALWAYS_CHAR("magic");
  magic_dictionary += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  magic_dictionary += ACE_TEXT_ALWAYS_CHAR(RPG_MAGIC_DEF_DICTIONARY_FILE);

  std::string UI_file             = configuration_path;
  UI_file += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined(DEBUG_DEBUGGER)
  UI_file += ACE_TEXT_ALWAYS_CHAR("test_u");
  UI_file += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  UI_file += ACE_TEXT_ALWAYS_CHAR("character");
  UI_file += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  UI_file += ACE_TEXT_ALWAYS_CHAR("player");
  UI_file += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  UI_file += RPG_CHARACTER_GENERATOR_GUI_DEF_GNOME_UI_file;

  std::string schema_repository   = configuration_path;
#if defined(DEBUG_DEBUGGER)
  schema_repository += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  schema_repository += ACE_TEXT_ALWAYS_CHAR("engine");
#endif

  std::string graphics_directory  = data_path;
  graphics_directory += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined(DEBUG_DEBUGGER)
  graphics_directory += ACE_TEXT_ALWAYS_CHAR(RPG_GRAPHICS_DEF_DATA_SUB);
#else
  graphics_directory += ACE_TEXT_ALWAYS_CHAR("graphics");
  graphics_directory += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  graphics_directory += ACE_TEXT_ALWAYS_CHAR(RPG_COMMON_DATA_SUB);
#endif

  // sanity check
  if (!RPG_Common_File_Tools::isDirectory(schema_repository))
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
    print_usage(std::string(ACE::basename(argv_in[0])));

    // *PORTABILITY*: on Windows, need to fini ACE...
#if defined(ACE_WIN32) || defined(ACE_WIN64)
    if (ACE::fini() == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE::fini(): \"%m\", continuing\n")));
#endif

    return EXIT_FAILURE;
  } // end IF

  // validate argument(s)
  if (!RPG_Common_File_Tools::isReadable(graphics_dictionary) ||
      !RPG_Common_File_Tools::isReadable(item_dictionary) ||
      !RPG_Common_File_Tools::isReadable(magic_dictionary) ||
      !RPG_Common_File_Tools::isReadable(UI_file) ||
      !RPG_Common_File_Tools::isDirectory(graphics_directory))
  {
    // make 'em learn...
    print_usage(std::string(ACE::basename(argv_in[0])));

    // *PORTABILITY*: on Windows, need to fini ACE...
#if defined(ACE_WIN32) || defined(ACE_WIN64)
    if (ACE::fini() == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE::fini(): \"%m\", continuing\n")));
#endif

    return EXIT_FAILURE;
  } // end IF

  // step1b: handle specific program modes
  if (print_version_and_exit)
  {
    print_version(std::string(ACE::basename(argv_in[0])));

    // *PORTABILITY*: on Windows, need to fini ACE...
#if defined(ACE_WIN32) || defined(ACE_WIN64)
    if (ACE::fini() == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE::fini(): \"%m\", continuing\n")));
#endif

    return EXIT_SUCCESS;
  } // end IF

  // step1c: initialize logging and/or tracing
  std::string log_file;
  if (!RPG_Common_Tools::initLogging(ACE::basename(argv_in[0]), // program name
                                     log_file,                  // logfile
                                     false,                     // log to syslog ?
                                     false,                     // trace messages ?
                                     trace_information,         // debug messages ?
                                     NULL))                     // logger
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Common_Tools::initLogging(), aborting\n")));

    // *PORTABILITY*: on Windows, need to fini ACE...
#if defined(ACE_WIN32) || defined(ACE_WIN64)
    if (ACE::fini() == -1)
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to ACE::fini(): \"%m\", continuing\n")));
#endif

    return EXIT_FAILURE;
  } // end IF

  // step2b: init GLIB / G(D|T)K[+] / GNOME
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  g_thread_init(NULL);
#endif
  gdk_threads_init();
  gtk_init(&argc_in,
           &argv_in);
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

  ACE_High_Res_Timer timer;
  timer.start();
  // step3: do actual work
  do_work(schema_repository,
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
  RPG_Common_Tools::period2String(working_time,
                                  working_time_string);

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("total working time (h:m:s.us): \"%s\"...\n"),
             ACE_TEXT(working_time_string.c_str())));

  // *PORTABILITY*: on Windows, need to fini ACE...
#if defined(ACE_WIN32) || defined(ACE_WIN64)
  if (ACE::fini() == -1)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE::fini(): \"%m\", continuing\n")));
#endif

  return EXIT_SUCCESS;
}
