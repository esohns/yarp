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

#include "rpg_engine_defines.h"
#include "rpg_engine_common_tools.h"

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

#define RPG_CHARACTER_GENERATOR_GUI_DEF_GNOME_UI_FILE        ACE_TEXT_ALWAYS_CHAR("character_generator_gui.glade")
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

  std::string config_path = RPG_Common_File_Tools::getWorkingDirectory();
  std::string data_path = RPG_Common_File_Tools::getWorkingDirectory();
#ifdef BASEDIR
  config_path = RPG_Common_File_Tools::getConfigurationDataDirectory(ACE_TEXT_ALWAYS_CHAR(BASEDIR),
                                                                     true);
  data_path = RPG_Common_File_Tools::getConfigurationDataDirectory(ACE_TEXT_ALWAYS_CHAR(BASEDIR),
                                                                   false);
#endif // #ifdef BASEDIR

  std::cout << ACE_TEXT("usage: ") << programName_in << ACE_TEXT(" [OPTIONS]") << std::endl << std::endl;
  std::cout << ACE_TEXT("currently available options:") << std::endl;
  std::string path = data_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined(_DEBUG) && !defined(DEBUG_RELEASE)
  path += ACE_TEXT_ALWAYS_CHAR("graphics");
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path += ACE_TEXT_ALWAYS_CHAR(RPG_COMMON_DATA_SUB);
#else
  path += ACE_TEXT_ALWAYS_CHAR(RPG_GRAPHICS_DEF_DATA_SUB);
#endif
  std::cout << ACE_TEXT("-d [DIR]    : graphics directory") << ACE_TEXT(" [\"") << path.c_str() << ACE_TEXT("\"]") << std::endl;
  path = config_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined(_DEBUG) && !defined(DEBUG_RELEASE)
  path += ACE_TEXT_ALWAYS_CHAR("graphics");
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  path += ACE_TEXT_ALWAYS_CHAR(RPG_GRAPHICS_DEF_DICTIONARY_FILE);
  std::cout << ACE_TEXT("-g [FILE]   : graphics dictionary (*.xml)") << ACE_TEXT(" [\"") << path.c_str() << ACE_TEXT("\"]") << std::endl;
  path = config_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined(_DEBUG) && !defined(DEBUG_RELEASE)
  path += ACE_TEXT_ALWAYS_CHAR("item");
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  path += ACE_TEXT_ALWAYS_CHAR(RPG_ITEM_DEF_DICTIONARY_FILE);
  std::cout << ACE_TEXT("-i [FILE]   : item dictionary (*.xml)") << ACE_TEXT(" [\"") << path.c_str() << ACE_TEXT("\"]") << std::endl;
  std::cout << ACE_TEXT("-l          : log to a file") << ACE_TEXT(" [") << false << ACE_TEXT("]") << std::endl;
  path = config_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined(_DEBUG) && !defined(DEBUG_RELEASE)
  path += ACE_TEXT_ALWAYS_CHAR("magic");
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  path += ACE_TEXT_ALWAYS_CHAR(RPG_MAGIC_DEF_DICTIONARY_FILE);
  std::cout << ACE_TEXT("-m [FILE]   : magic dictionary (*.xml)") << ACE_TEXT(" [\"") << path.c_str() << ACE_TEXT("\"]") << std::endl;
  std::cout << ACE_TEXT("-t          : trace information") << ACE_TEXT(" [") << false << ACE_TEXT("]") << std::endl;
  path = config_path;
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined(_DEBUG) && !defined(DEBUG_RELEASE)
  path += ACE_TEXT_ALWAYS_CHAR("test_u");
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path += ACE_TEXT_ALWAYS_CHAR("character");
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  path += ACE_TEXT_ALWAYS_CHAR("player");
  path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  path += RPG_CHARACTER_GENERATOR_GUI_DEF_GNOME_UI_FILE;
  std::cout << ACE_TEXT("-u [FILE]   : UI file") << ACE_TEXT(" [\"") << path.c_str() << ACE_TEXT("\"]") << std::endl;
  std::cout << ACE_TEXT("-v          : print version information and exit") << ACE_TEXT(" [") << false << ACE_TEXT("]") << std::endl;
} // end print_usage

bool
test_u_main::process_arguments(const int argc_in,
															 ACE_TCHAR* argv_in[], // cannot be const...
															 std::string& graphicsDictionary_out,
															 std::string& itemDictionary_out,
															 bool& logToFile_out,
															 std::string& magicDictionary_out,
															 bool& traceInformation_out,
															 std::string& UIfile_out,
															 std::string& graphicsDirectory_out,
															 bool& printVersionAndExit_out)
{
  RPG_TRACE(ACE_TEXT("test_u_main::process_arguments"));

  // init configuration
  std::string config_path = RPG_Common_File_Tools::getWorkingDirectory();
  std::string data_path = RPG_Common_File_Tools::getWorkingDirectory();
#ifdef BASEDIR
  config_path = RPG_Common_File_Tools::getConfigurationDataDirectory(ACE_TEXT_ALWAYS_CHAR(BASEDIR),
                                                                     true);
  data_path = RPG_Common_File_Tools::getConfigurationDataDirectory(ACE_TEXT_ALWAYS_CHAR(BASEDIR),
                                                                   false);
#endif // #ifdef BASEDIR

  graphicsDictionary_out = config_path;
  graphicsDictionary_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined(_DEBUG) && !defined(DEBUG_RELEASE)
  graphicsDictionary_out += ACE_TEXT_ALWAYS_CHAR("graphics");
  graphicsDictionary_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  graphicsDictionary_out += ACE_TEXT_ALWAYS_CHAR(RPG_GRAPHICS_DEF_DICTIONARY_FILE);

  itemDictionary_out = config_path;
  itemDictionary_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined(_DEBUG) && !defined(DEBUG_RELEASE)
  itemDictionary_out += ACE_TEXT_ALWAYS_CHAR("item");
  itemDictionary_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  itemDictionary_out += ACE_TEXT_ALWAYS_CHAR(RPG_ITEM_DEF_DICTIONARY_FILE);

  logToFile_out      = false;

  magicDictionary_out = config_path;
  magicDictionary_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined(_DEBUG) && !defined(DEBUG_RELEASE)
  magicDictionary_out += ACE_TEXT_ALWAYS_CHAR("magic");
  magicDictionary_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  magicDictionary_out += ACE_TEXT_ALWAYS_CHAR(RPG_MAGIC_DEF_DICTIONARY_FILE);

  traceInformation_out = false;

  UIfile_out = config_path;
  UIfile_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined(_DEBUG) && !defined(DEBUG_RELEASE)
  UIfile_out += ACE_TEXT_ALWAYS_CHAR("test_u");
  UIfile_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  UIfile_out += ACE_TEXT_ALWAYS_CHAR("character");
  UIfile_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  UIfile_out += ACE_TEXT_ALWAYS_CHAR("player");
  UIfile_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  UIfile_out += RPG_CHARACTER_GENERATOR_GUI_DEF_GNOME_UI_FILE;

  graphicsDirectory_out = data_path;
  graphicsDirectory_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined(_DEBUG) && !defined(DEBUG_RELEASE)
  graphicsDirectory_out += ACE_TEXT_ALWAYS_CHAR("graphics");
  graphicsDirectory_out += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  graphicsDirectory_out += ACE_TEXT_ALWAYS_CHAR(RPG_COMMON_DATA_SUB);
#else
  graphicsDirectory_out += ACE_TEXT_ALWAYS_CHAR(RPG_GRAPHICS_DEF_DATA_SUB);
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
        graphicsDirectory_out = argumentParser.opt_arg();

        break;
      }
      case 'g':
      {
        graphicsDictionary_out = argumentParser.opt_arg();

        break;
      }
      case 'i':
      {
        itemDictionary_out = argumentParser.opt_arg();

        break;
      }
      case 'l':
      {
        logToFile_out = true;

        break;
      }
      case 'm':
      {
        magicDictionary_out = argumentParser.opt_arg();

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
test_u_main::init_GUI(const std::string& graphicsDirectory_in,
                      const std::string& UIfile_in,
											GTK_cb_data_t& userData_in)
{
  RPG_TRACE(ACE_TEXT("test_u_main::init_GUI"));

  // sanity check(s)
  if (!RPG_Common_File_Tools::isDirectory(graphicsDirectory_in.c_str()))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid directory \"%s\", aborting\n"),
               graphicsDirectory_in.c_str()));

    return false;
  } // end IF
  if (!RPG_Common_File_Tools::isReadable(UIfile_in.c_str()))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("UI definition file \"%s\" doesn't exist, aborting\n"),
               UIfile_in.c_str()));

    return false;
  } // end IF
  ACE_ASSERT(userData_in.xml == NULL);

  // step1: load widget tree
  userData_in.xml = glade_xml_new(UIfile_in.c_str(), // definition file
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
  main_dialog = GTK_WIDGET(glade_xml_get_widget(userData_in.xml,
                                                ACE_TEXT_ALWAYS_CHAR("main_dialog")));
  ACE_ASSERT(main_dialog);

  GtkWidget* about_dialog = GTK_WIDGET(glade_xml_get_widget(userData_in.xml,
                                                            ACE_TEXT_ALWAYS_CHAR("about_dialog")));
  ACE_ASSERT(about_dialog);

  // step3: populate combobox
  GtkComboBox* combobox = GTK_COMBO_BOX(glade_xml_get_widget(userData_in.xml,
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
  std::string profiles_directory = RPG_Player_Common_Tools::getPlayerProfilesDirectory();
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

  GtkFileChooserDialog* filechooser_dialog = GTK_FILE_CHOOSER_DIALOG(glade_xml_get_widget(userData_in.xml,
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
	gtk_file_filter_set_name(file_filter, ACE_TEXT(RPG_ENGINE_ENTITY_PROFILE_EXT));
  std::string pattern = ACE_TEXT_ALWAYS_CHAR("*");
  pattern += ACE_TEXT_ALWAYS_CHAR(RPG_ENGINE_ENTITY_PROFILE_EXT);
  gtk_file_filter_add_pattern(file_filter, ACE_TEXT(pattern.c_str()));
  gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(filechooser_dialog), file_filter);
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

  combobox = GTK_COMBO_BOX(glade_xml_get_widget(userData_in.xml,
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

  button = GTK_BUTTON(glade_xml_get_widget(userData_in.xml,
                                           ACE_TEXT_ALWAYS_CHAR("image_button_prev")));
  ACE_ASSERT(button);
  g_signal_connect(button,
                   ACE_TEXT_ALWAYS_CHAR("clicked"),
                   G_CALLBACK(prev_image_clicked_GTK_cb),
                   userData_p);

  button = GTK_BUTTON(glade_xml_get_widget(userData_in.xml,
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
    GtkButton* button = GTK_BUTTON(glade_xml_get_widget(userData_in.xml,
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
      							 const std::string& magicDictionary_in,
										 const std::string& itemDictionary_in,
										 const std::string& graphicsDictionary_in,
										 const std::string& graphicsDirectory_in,
										 const std::string& UIFile_in)
{
  RPG_TRACE(ACE_TEXT("test_u_main::do_work"));

  // step0a: init RPG engine
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
                                graphicsDirectory_in,
																RPG_CLIENT_DEF_GRAPHICS_CACHESIZE,
                                graphicsDictionary_in,
                                false); // don't init SDL

  GTK_cb_data_t userData;
  userData.xml              = NULL;
  userData.schemaRepository = schemaDirectory_in;
  userData.entity.character = NULL;
  userData.entity.position  = std::make_pair(std::numeric_limits<unsigned int>::max(),
                                             std::numeric_limits<unsigned int>::max());
  userData.entity.modes.clear();
  userData.entity.actions.clear();
  userData.entity.sprite = RPG_GRAPHICS_SPRITE_INVALID;
  userData.entity.is_spawned = false;
  // init sprite gallery
  for (int index = 0;
       index < RPG_GRAPHICS_SPRITE_MAX;
       index++)
    userData.sprite_gallery.push_back(static_cast<RPG_Graphics_Sprite>(index));
  userData.current_sprite = userData.sprite_gallery.begin();
  userData.is_transient = false;

  if (!init_GUI(graphicsDirectory_in, // graphics directory
                UIFile_in,            // glade file
								userData))            // GTK cb data
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

  // step1 init/validate configuration
  std::string config_path = RPG_Common_File_Tools::getWorkingDirectory();
  std::string data_path = RPG_Common_File_Tools::getWorkingDirectory();
#ifdef BASEDIR
  config_path = RPG_Common_File_Tools::getConfigurationDataDirectory(ACE_TEXT_ALWAYS_CHAR(BASEDIR),
                                                                     true);
  data_path = RPG_Common_File_Tools::getConfigurationDataDirectory(ACE_TEXT_ALWAYS_CHAR(BASEDIR),
                                                                   false);
#endif // #ifdef BASEDIR

  // step1a: process commandline arguments
  std::string graphicsDictionary = config_path;
  graphicsDictionary += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined(_DEBUG) && !defined(DEBUG_RELEASE)
  graphicsDictionary += ACE_TEXT_ALWAYS_CHAR("graphics");
  graphicsDictionary += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  graphicsDictionary += ACE_TEXT_ALWAYS_CHAR(RPG_GRAPHICS_DEF_DICTIONARY_FILE);

  std::string itemDictionary = config_path;
  itemDictionary += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined(_DEBUG) && !defined(DEBUG_RELEASE)
  itemDictionary += ACE_TEXT_ALWAYS_CHAR("item");
  itemDictionary += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  itemDictionary += ACE_TEXT_ALWAYS_CHAR(RPG_ITEM_DEF_DICTIONARY_FILE);

  std::string magicDictionary = config_path;
  magicDictionary += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined(_DEBUG) && !defined(DEBUG_RELEASE)
  magicDictionary += ACE_TEXT_ALWAYS_CHAR("magic");
  magicDictionary += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  magicDictionary += ACE_TEXT_ALWAYS_CHAR(RPG_MAGIC_DEF_DICTIONARY_FILE);

  std::string UIfile = config_path;
  UIfile += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined(_DEBUG) && !defined(DEBUG_RELEASE)
  UIfile += ACE_TEXT_ALWAYS_CHAR("test_u");
  UIfile += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  UIfile += ACE_TEXT_ALWAYS_CHAR("character");
  UIfile += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  UIfile += ACE_TEXT_ALWAYS_CHAR("player");
  UIfile += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#endif
  UIfile += RPG_CHARACTER_GENERATOR_GUI_DEF_GNOME_UI_FILE;

  std::string schemaRepository = config_path;
#if defined(_DEBUG) && !defined(DEBUG_RELEASE)
  schemaRepository += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  schemaRepository += ACE_TEXT_ALWAYS_CHAR("engine");
#endif

  std::string graphicsDirectory = data_path;
  graphicsDirectory += ACE_DIRECTORY_SEPARATOR_CHAR_A;
#if defined(_DEBUG) && !defined(DEBUG_RELEASE)
  graphicsDirectory += ACE_TEXT_ALWAYS_CHAR(RPG_GRAPHICS_DEF_DATA_SUB);
#else
  graphicsDirectory += ACE_TEXT_ALWAYS_CHAR("graphics");
  graphicsDirectory += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  graphicsDirectory += ACE_TEXT_ALWAYS_CHAR(RPG_COMMON_DATA_SUB);
#endif

  // sanity check
  if (!RPG_Common_File_Tools::isDirectory(schemaRepository))
  {
    ACE_DEBUG((LM_WARNING,
               ACE_TEXT("invalid XML schema repository (was: \"%s\"), continuing\n"),
               schemaRepository.c_str()));

    // try fallback
    schemaRepository.clear();
  } // end IF

  bool logToFile                     = false;
  bool traceInformation              = false;
  bool printVersionAndExit           = false;
  if (!process_arguments(argc_in,
                         argv_in,
                         graphicsDictionary,
                         itemDictionary,
                         logToFile,
                         magicDictionary,
                         traceInformation,
                         UIfile,
                         graphicsDirectory,
                         printVersionAndExit))
  {
    // make 'em learn...
    print_usage(std::string(ACE::basename(argv_in[0])));

    return EXIT_FAILURE;
  } // end IF

  // validate argument(s)
  if (!RPG_Common_File_Tools::isReadable(graphicsDictionary) ||
      !RPG_Common_File_Tools::isReadable(itemDictionary) ||
      !RPG_Common_File_Tools::isReadable(magicDictionary) ||
      !RPG_Common_File_Tools::isReadable(UIfile) ||
      !RPG_Common_File_Tools::isDirectory(graphicsDirectory))
  {
    // make 'em learn...
    print_usage(std::string(ACE::basename(argv_in[0])));

    return EXIT_FAILURE;
  } // end IF

  // step1b: handle specific program modes
  if (printVersionAndExit)
  {
    print_version(std::string(ACE::basename(argv_in[0])));

    return EXIT_SUCCESS;
  } // end IF

  // step1c: set correct trace level
  // step2c: set correct verbosity
  u_long process_priority_mask = (LM_SHUTDOWN |
//                                   LM_TRACE |  // <-- DISABLE trace messages !
//                                   LM_DEBUG |  // <-- DISABLE ACE_DEBUG messages !
                                  LM_INFO |
                                  LM_NOTICE |
                                  LM_WARNING |
                                  LM_STARTUP |
                                  LM_ERROR |
                                  LM_CRITICAL |
                                  LM_ALERT |
                                  LM_EMERGENCY);
  if (traceInformation)
  {
    process_priority_mask |= LM_DEBUG;

    // don't go VERBOSE...
    //ACE_LOG_MSG->clr_flags(ACE_Log_Msg::VERBOSE_LITE);
  } // end IF
  // set process priority mask...
  ACE_LOG_MSG->priority_mask(process_priority_mask,
                             ACE_Log_Msg::PROCESS);

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
  do_work(schemaRepository,
          magicDictionary,
          itemDictionary,
          graphicsDictionary,
          graphicsDirectory,
          UIfile);
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

  return EXIT_SUCCESS;
}
