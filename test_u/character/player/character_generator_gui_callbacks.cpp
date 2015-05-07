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

#include "character_generator_gui_callbacks.h"

#include <sstream>

#include "ace/Log_Msg.h"

#include "common_file_tools.h"

#include "common_ui_common.h"
#include "common_ui_defines.h"
#include "common_ui_gtk_manager.h"
#include "common_ui_tools.h"

#include "rpg_common_defines.h"
#include "rpg_common_file_tools.h"
#include "rpg_common_macros.h"
#include "rpg_common_tools.h"

#include "rpg_item_armor.h"
#include "rpg_item_instance_manager.h"
#include "rpg_item_weapon.h"

#include "rpg_player_common_tools.h"
#include "rpg_player_defines.h"

#include "rpg_map_common_tools.h"
#include "rpg_map_defines.h"

#include "rpg_engine_common.h"
#include "rpg_engine_common_tools.h"
#include "rpg_engine_defines.h"

#include "rpg_graphics_common_tools.h"
#include "rpg_graphics_cursor.h"
#include "rpg_graphics_defines.h"
#include "rpg_graphics_dictionary.h"
#include "rpg_graphics_surface.h"

#include "rpg_client_callbacks.h"
#include "rpg_client_common.h"
#include "rpg_client_common_tools.h"
#include "rpg_client_defines.h"
//#include "rpg_client_ui_tools.h"

#include "character_generator_gui_common.h"

void
update_sprite_gallery (GTK_cb_data_t& CBData_in)
{
  RPG_TRACE (ACE_TEXT ("::update_sprite_gallery"));

  for (CBData_in.spriteGalleryIterator = CBData_in.spriteGallery.begin();
       CBData_in.spriteGalleryIterator != CBData_in.spriteGallery.end();
       CBData_in.spriteGalleryIterator++)
    if (*CBData_in.spriteGalleryIterator == CBData_in.currentSprite)
      break;

  // sanity check
  if (*CBData_in.spriteGalleryIterator != CBData_in.currentSprite)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("sprite (was: \"%s\") not in gallery (%u entries), aborting\n"),
                ACE_TEXT (RPG_Graphics_SpriteHelper::RPG_Graphics_SpriteToString (CBData_in.currentSprite).c_str ()),
                CBData_in.spriteGallery.size ()));
}

void
set_current_image (const RPG_Graphics_Sprite& sprite_in,
                   GladeXML* xml_in)
{
  RPG_TRACE (ACE_TEXT ("::set_current_image"));

  GtkImage* image =
    GTK_IMAGE (glade_xml_get_widget (xml_in,
                                     ACE_TEXT_ALWAYS_CHAR ("image_sprite")));
  ACE_ASSERT (image);
  gtk_image_clear (image);

  if (sprite_in != RPG_GRAPHICS_SPRITE_INVALID)
  {
    RPG_Graphics_GraphicTypeUnion type;
    type.discriminator = RPG_Graphics_GraphicTypeUnion::SPRITE;
    type.sprite = sprite_in;
    RPG_Graphics_t graphic;
    graphic.category = RPG_GRAPHICS_CATEGORY_INVALID;
    graphic.type.discriminator = RPG_Graphics_GraphicTypeUnion::INVALID;
    // retrieve properties from the dictionary
    graphic = RPG_GRAPHICS_DICTIONARY_SINGLETON::instance ()->get (type);
    ACE_ASSERT ((graphic.type.sprite == sprite_in) &&
                (graphic.type.discriminator == RPG_Graphics_GraphicTypeUnion::SPRITE));
    // sanity check
    if (graphic.category != CATEGORY_SPRITE)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid category (was: \"%s\"), not a sprite type, returning\n"),
                  ACE_TEXT (RPG_Graphics_CategoryHelper::RPG_Graphics_CategoryToString (graphic.category).c_str ())));
      return;
    } // end IF

    // assemble path
    std::string filename;
    RPG_Graphics_Common_Tools::graphicToFile (graphic, filename);
    ACE_ASSERT (!filename.empty ());
    gtk_image_set_from_file (image, filename.c_str ());
  } // end IF
  else
    gtk_image_clear (image);
}

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
G_MODULE_EXPORT gboolean
idle_initialize_UI_cb (gpointer userData_in)
{
  RPG_TRACE (ACE_TEXT ("::idle_initialize_UI_cb"));

  RPG_Client_GTK_CBData_t* data_p =
    static_cast<RPG_Client_GTK_CBData_t*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);

  Common_UI_GladeXMLsIterator_t iterator =
    data_p->GTKState.gladeXML.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != data_p->GTKState.gladeXML.end ());

  // step1: retrieve dialog(s)
  GtkWidget* main_dialog = NULL;
  main_dialog =
    GTK_WIDGET (glade_xml_get_widget ((*iterator).second.second,
                                      ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_DIALOG_MAIN_NAME)));
  ACE_ASSERT (main_dialog);

  GtkWidget* about_dialog =
    GTK_WIDGET (glade_xml_get_widget ((*iterator).second.second,
                                      ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_DIALOG_ABOUT_NAME)));
  ACE_ASSERT (about_dialog);

  // step2: populate combobox
  GtkComboBox* combobox =
    GTK_COMBO_BOX (glade_xml_get_widget ((*iterator).second.second,
                                         ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_COMBOBOX_CHARACTER_NAME)));
  ACE_ASSERT (combobox);
  gtk_cell_layout_clear (GTK_CELL_LAYOUT (combobox));
  GtkCellRenderer* renderer = gtk_cell_renderer_text_new ();
  ACE_ASSERT (renderer);
  gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combobox), renderer,
                              TRUE); // expand ?
  //   gtk_cell_layout_add_attribute(GTK_CELL_LAYOUT(combobox), renderer,
  //                                 ACE_TEXT_ALWAYS_CHAR("text"), 0);
  gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combobox), renderer,
                                  ACE_TEXT_ALWAYS_CHAR ("text"), 0,
                                  NULL);
  GtkListStore* list = gtk_list_store_new (1,
                                           G_TYPE_STRING);
  ACE_ASSERT (list);
  gtk_tree_sortable_set_sort_func (GTK_TREE_SORTABLE (list),
                                   0,
                                   (GtkTreeIterCompareFunc)combobox_sort_function,
                                   GINT_TO_POINTER (0),
                                   (GDestroyNotify)NULL);
  gtk_tree_sortable_set_sort_column_id (GTK_TREE_SORTABLE (list),
                                        0,
                                        GTK_SORT_ASCENDING);
  gtk_combo_box_set_model (combobox,
                           GTK_TREE_MODEL (list));
  g_object_unref (G_OBJECT (list));
  unsigned int num_entries = ::load_files (REPOSITORY_PROFILES,
                                           list);

  Common_UI_GladeXMLsIterator_t iterator_2 =
    data_p->GTKState.gladeXML.find (ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator_2 != data_p->GTKState.gladeXML.end ());

  GtkHBox* hbox =
    GTK_HBOX (glade_xml_get_widget ((*iterator).second.second,
                                    ACE_TEXT_ALWAYS_CHAR (CHARACTER_GENERATOR_GTK_HBOX_NAME)));
  ACE_ASSERT (hbox);
  GtkFrame* frame =
    GTK_FRAME (glade_xml_get_widget ((*iterator_2).second.second,
                                     ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_FRAME_CHARACTER_NAME)));
  ACE_ASSERT (frame);
  gtk_widget_ref (GTK_WIDGET (frame));
  gtk_widget_unparent (GTK_WIDGET (frame));
  gtk_box_pack_start (GTK_BOX (hbox),
                      GTK_WIDGET (frame),
                      FALSE, // expand
                      FALSE, // fill
                      0);    // padding

  // step4a: connect default signals
  g_signal_connect (main_dialog,
                    ACE_TEXT_ALWAYS_CHAR ("destroy"),
                    G_CALLBACK (quit_clicked_GTK_cb),
                    //                    G_CALLBACK(gtk_widget_destroyed),
                    //                    &main_dialog,
                    userData_in);
  g_signal_connect (about_dialog,
                    ACE_TEXT_ALWAYS_CHAR ("response"),
                    G_CALLBACK (gtk_widget_hide),
                    &about_dialog);

  // step4b: connect custom signals
  GtkButton* button = NULL;
  button =
    GTK_BUTTON (glade_xml_get_widget ((*iterator).second.second,
                                      ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_BUTTON_CREATE_NAME)));
  ACE_ASSERT (button);
  g_signal_connect (button,
                    ACE_TEXT_ALWAYS_CHAR ("clicked"),
                    G_CALLBACK (create_character_clicked_GTK_cb),
                    userData_in);

  button =
    GTK_BUTTON (glade_xml_get_widget ((*iterator).second.second,
                                      ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_BUTTON_DROP_NAME)));
  ACE_ASSERT (button);
  g_signal_connect (button,
                    ACE_TEXT_ALWAYS_CHAR ("clicked"),
                    G_CALLBACK (drop_character_clicked_GTK_cb),
                    userData_in);

  GtkFileChooserDialog* filechooser_dialog =
    GTK_FILE_CHOOSER_DIALOG (glade_xml_get_widget ((*iterator).second.second,
                                                   ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_DIALOG_FILECHOOSER_NAME)));
  ACE_ASSERT (filechooser_dialog);
  GtkFileFilter* file_filter = gtk_file_filter_new ();
  ACE_ASSERT (file_filter);
  if (!file_filter)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gtk_file_filter_new(): \"%m\", aborting\n")));
    return FALSE; // G_SOURCE_REMOVE
  } // end IF
  gtk_file_filter_set_name (file_filter,
                            ACE_TEXT (RPG_PLAYER_PROFILE_EXT));
  std::string pattern = ACE_TEXT_ALWAYS_CHAR ("*");
  pattern += ACE_TEXT_ALWAYS_CHAR (RPG_PLAYER_PROFILE_EXT);
  gtk_file_filter_add_pattern (file_filter, ACE_TEXT (pattern.c_str ()));
  gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (filechooser_dialog),
                               file_filter);
  //gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(filechooser_dialog),
  //                                    ACE_TEXT(profiles_directory.c_str()));
  g_signal_connect (filechooser_dialog,
                    ACE_TEXT_ALWAYS_CHAR ("file-activated"),
                    G_CALLBACK (character_file_activated_GTK_cb),
                    userData_in);

  button =
    GTK_BUTTON (glade_xml_get_widget ((*iterator).second.second,
                                      ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_BUTTON_LOAD_NAME)));
  ACE_ASSERT (button);
  g_signal_connect (button,
                    ACE_TEXT_ALWAYS_CHAR ("clicked"),
                    G_CALLBACK (load_character_clicked_GTK_cb),
                    userData_in);

  button =
    GTK_BUTTON (glade_xml_get_widget ((*iterator).second.second,
                                      ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_BUTTON_STORE_NAME)));
  ACE_ASSERT (button);
  g_signal_connect (button,
                    ACE_TEXT_ALWAYS_CHAR ("clicked"),
                    G_CALLBACK (store_character_clicked_GTK_cb),
                    userData_in);

  combobox =
    GTK_COMBO_BOX (glade_xml_get_widget ((*iterator).second.second,
                                         ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_COMBOBOX_CHARACTER_NAME)));
  ACE_ASSERT (combobox);
  g_signal_connect (combobox,
                    ACE_TEXT_ALWAYS_CHAR ("changed"),
                    G_CALLBACK (character_repository_combobox_changed_GTK_cb),
                    userData_in);

  button =
    GTK_BUTTON (glade_xml_get_widget ((*iterator).second.second,
                                      ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_BUTTON_REFRESH_NAME)));
  ACE_ASSERT (button);
  g_signal_connect (button,
                    ACE_TEXT_ALWAYS_CHAR ("clicked"),
                    G_CALLBACK (character_repository_button_clicked_GTK_cb),
                    userData_in);

  button =
    GTK_BUTTON (glade_xml_get_widget ((*iterator).second.second,
                                      ACE_TEXT_ALWAYS_CHAR (CHARACTER_GENERATOR_GTK_BUTTON_IMAGEPREV_NAME)));
  ACE_ASSERT (button);
  g_signal_connect (button,
                    ACE_TEXT_ALWAYS_CHAR ("clicked"),
                    G_CALLBACK (prev_image_clicked_GTK_cb),
                    userData_in);

  button =
    GTK_BUTTON (glade_xml_get_widget ((*iterator).second.second,
                                      ACE_TEXT_ALWAYS_CHAR (CHARACTER_GENERATOR_GTK_BUTTON_IMAGENEXT_NAME)));
  ACE_ASSERT (button);
  g_signal_connect (button,
                    ACE_TEXT_ALWAYS_CHAR ("clicked"),
                    G_CALLBACK (next_image_clicked_GTK_cb),
                    userData_in);

  button =
    GTK_BUTTON (glade_xml_get_widget ((*iterator).second.second,
                                      ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_BUTTON_QUIT_NAME)));
  ACE_ASSERT (button);
  g_signal_connect (button,
                    ACE_TEXT_ALWAYS_CHAR ("clicked"),
                    G_CALLBACK (quit_clicked_GTK_cb),
                    userData_in);

  button =
    GTK_BUTTON (glade_xml_get_widget ((*iterator).second.second,
                                      ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_BUTTON_ABOUT_NAME)));
  ACE_ASSERT (button);
  g_signal_connect (button,
                    ACE_TEXT_ALWAYS_CHAR ("clicked"),
                    G_CALLBACK (about_clicked_GTK_cb),
                    userData_in);

  // step5: auto-connect signals/slots
  //   glade_xml_signal_autoconnect(xml);

  //   // step6: use correct screen
  //   if (parentWidget_in)
  //     gtk_window_set_screen(GTK_WINDOW(dialog),
  //                           gtk_widget_get_screen(const_cast<GtkWidget*> (//                                                                parentWidget_in)));

  // step6: draw it
  gtk_widget_show_all (main_dialog);

  // step7: activate first repository entry (if any)
  gtk_widget_set_sensitive (GTK_WIDGET (combobox), (num_entries > 0));
  if (num_entries > 0)
    gtk_combo_box_set_active (combobox, 0);
  else
  {
    // make create button sensitive (if it's not already)
    button =
      GTK_BUTTON (glade_xml_get_widget ((*iterator).second.second,
                                        ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_BUTTON_CREATE_NAME)));
    ACE_ASSERT (button);
    gtk_widget_set_sensitive (GTK_WIDGET (button), TRUE);

    // make load button sensitive (if it's not already)
    button =
      GTK_BUTTON (glade_xml_get_widget ((*iterator).second.second,
                                        ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_BUTTON_LOAD_NAME)));
    ACE_ASSERT (button);
    gtk_widget_set_sensitive (GTK_WIDGET (button), TRUE);
  } // end ELSE

  return FALSE; // G_SOURCE_REMOVE
}

G_MODULE_EXPORT gboolean
idle_finalize_UI_cb (gpointer userData_in)
{
  RPG_TRACE (ACE_TEXT ("::idle_finalize_UI_cb"));

  ACE_UNUSED_ARG (userData_in);

  // leave GTK
  gtk_main_quit ();

  return FALSE; // G_SOURCE_REMOVE
}

G_MODULE_EXPORT gint
about_clicked_GTK_cb (GtkWidget* widget_in,
                      gpointer userData_in)
{
  RPG_TRACE (ACE_TEXT ("::about_clicked_GTK_cb"));

  ACE_UNUSED_ARG (widget_in);
  GTK_cb_data_t* data_p = static_cast<GTK_cb_data_t*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);

  Common_UI_GladeXMLsIterator_t iterator =
    data_p->GTKState.gladeXML.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != data_p->GTKState.gladeXML.end ());

  // retrieve about dialog handle
  GtkWidget* about_dialog =
    GTK_WIDGET (glade_xml_get_widget ((*iterator).second.second,
                                      ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_DIALOG_ABOUT_NAME)));
  ACE_ASSERT (about_dialog);

  gtk_dialog_run(GTK_DIALOG(about_dialog));
  gtk_widget_hide(about_dialog);
  //if (!GTK_WIDGET_VISIBLE(about_dialog))
  //  gtk_widget_show(about_dialog);

  return FALSE;
}

G_MODULE_EXPORT gint
quit_clicked_GTK_cb (GtkWidget* widget_in,
                     gpointer userData_in)
{
  RPG_TRACE (ACE_TEXT ("::quit_clicked_GTK_cb"));

  ACE_UNUSED_ARG (widget_in);
  ACE_UNUSED_ARG (userData_in);

  COMMON_UI_GTK_MANAGER_SINGLETON::instance ()->close (1);

  return FALSE;
}

G_MODULE_EXPORT gint
create_character_clicked_GTK_cb (GtkWidget* widget_in,
                                 gpointer userData_in)
{
  RPG_TRACE (ACE_TEXT ("::create_character_clicked_GTK_cb"));

  ACE_UNUSED_ARG (widget_in);
  GTK_cb_data_t* data_p = static_cast<GTK_cb_data_t*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);

  Common_UI_GladeXMLsIterator_t iterator =
    data_p->GTKState.gladeXML.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  Common_UI_GladeXMLsIterator_t iterator_2 =
    data_p->GTKState.gladeXML.find (ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != data_p->GTKState.gladeXML.end ());
  ACE_ASSERT (iterator_2 != data_p->GTKState.gladeXML.end ());

  // clean up
  if (data_p->entity.character)
  {
    delete data_p->entity.character;
    data_p->entity.character = NULL;
    data_p->entity.position =
      std::make_pair (std::numeric_limits<unsigned int>::max (),
                      std::numeric_limits<unsigned int>::max ());
    data_p->entity.modes.clear ();
    data_p->entity.actions.clear ();
    data_p->entity.is_spawned = false;
  } // end IF
  data_p->entity = RPG_Engine_Common_Tools::createEntity ();
  ACE_ASSERT (data_p->entity.character);
  data_p->currentSprite = RPG_GRAPHICS_SPRITE_INVALID;
  data_p->isTransient = true;

  // update entity profile widgets
  ::update_entity_profile (data_p->entity,
                           (*iterator_2).second.second);

  // make character display frame sensitive (if it's not already)
  GtkFrame* character_frame =
    GTK_FRAME (glade_xml_get_widget ((*iterator_2).second.second,
                                     ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_FRAME_CHARACTER_NAME)));
  ACE_ASSERT (character_frame);
  gtk_widget_set_sensitive (GTK_WIDGET (character_frame), TRUE);
  // make drop button sensitive (if it's not already)
  GtkButton* button =
    GTK_BUTTON (glade_xml_get_widget ((*iterator).second.second,
                                      ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_BUTTON_DROP_NAME)));
  ACE_ASSERT (button);
  gtk_widget_set_sensitive (GTK_WIDGET (button), TRUE);
  // make save button sensitive (if it's not already)
  button =
    GTK_BUTTON (glade_xml_get_widget ((*iterator).second.second,
                                      ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_BUTTON_STORE_NAME)));
  ACE_ASSERT (button);
  gtk_widget_set_sensitive (GTK_WIDGET (button), TRUE);
  // make load button in-sensitive (if it's not already)
  button =
    GTK_BUTTON (glade_xml_get_widget ((*iterator).second.second,
                                      ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_BUTTON_LOAD_NAME)));
  ACE_ASSERT (button);
  gtk_widget_set_sensitive (GTK_WIDGET (button), FALSE);
  // make this in-sensitive
  gtk_widget_set_sensitive (widget_in, FALSE);

  return FALSE;
}

G_MODULE_EXPORT gint
drop_character_clicked_GTK_cb (GtkWidget* widget_in,
                               gpointer userData_in)
{
  RPG_TRACE (ACE_TEXT ("::drop_character_clicked_GTK_cb"));

  ACE_UNUSED_ARG (widget_in);
  GTK_cb_data_t* data_p = static_cast<GTK_cb_data_t*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);

  Common_UI_GladeXMLsIterator_t iterator =
    data_p->GTKState.gladeXML.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  Common_UI_GladeXMLsIterator_t iterator_2 =
    data_p->GTKState.gladeXML.find (ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != data_p->GTKState.gladeXML.end ());
  ACE_ASSERT (iterator_2 != data_p->GTKState.gladeXML.end ());

  // delete file if non-transient ?
  if (!data_p->isTransient)
  {
    // retrieve confirmation dialog handle
    GtkWidget* confirmation_dialog =
      GTK_WIDGET (glade_xml_get_widget ((*iterator).second.second,
                                        ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_DIALOG_CONFIRMATION_NAME)));
    ACE_ASSERT (confirmation_dialog);
    gint response = gtk_dialog_run (GTK_DIALOG (confirmation_dialog));
    gtk_widget_hide (confirmation_dialog);
    if (response == GTK_RESPONSE_OK)
    {
      // retrieve active item
      GtkComboBox* repository_combobox =
        GTK_COMBO_BOX (glade_xml_get_widget ((*iterator).second.second,
                                             ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_COMBOBOX_CHARACTER_NAME)));
      ACE_ASSERT (repository_combobox);
      std::string active_item;
      GtkTreeIter selected;
      GtkTreeModel* model = NULL;
      GValue value;
      const gchar* text = NULL;
      if (!gtk_combo_box_get_active_iter (repository_combobox, &selected))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to gtk_combo_box_get_active_iter(), aborting\n")));
        return FALSE;
      } // end IF
      model = gtk_combo_box_get_model (repository_combobox);
      ACE_ASSERT (model);
      ACE_OS::memset (&value, 0, sizeof (value));
      gtk_tree_model_get_value (model, &selected,
                                0, &value);
      text = g_value_get_string (&value);
      // sanity check
      ACE_ASSERT (text);
      active_item = ACE_TEXT_ALWAYS_CHAR (text);
      g_value_unset (&value);

      // construct filename
      std::string filename =
        RPG_Player_Common_Tools::getPlayerProfilesDirectory ();
      filename += ACE_DIRECTORY_SEPARATOR_CHAR_A;
      filename += active_item;
      filename += ACE_TEXT_ALWAYS_CHAR (RPG_PLAYER_PROFILE_EXT);

      // remove file
      if (!Common_File_Tools::deleteFile (filename))
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to RPG_Common_File_Tools::deleteFile(\"%s\"), aborting\n"),
                    ACE_TEXT (filename.c_str ())));
      else
      {
        // refresh combobox
        GtkButton* button =
          GTK_BUTTON (glade_xml_get_widget ((*iterator).second.second,
                                            ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_BUTTON_REFRESH_NAME)));
        ACE_ASSERT (button);
        g_signal_emit_by_name (button,
                               ACE_TEXT_ALWAYS_CHAR ("clicked"),
                               userData_in);
      } // end IF
    } // end IF
    else
      return FALSE; // cancelled --> done
  } // end IF

  // clean up
  if (data_p->entity.character)
  {
    delete data_p->entity.character;
    data_p->entity.character = NULL;
    data_p->entity.position =
      std::make_pair (std::numeric_limits<unsigned int>::max (),
                      std::numeric_limits<unsigned int>::max ());
    data_p->entity.modes.clear ();
    data_p->entity.actions.clear ();
    data_p->entity.is_spawned = false;
  } // end IF

  // reset profile widgets
  ::reset_character_profile ((*iterator_2).second.second);
  data_p->currentSprite = CHARACTER_GENERATOR_DEF_ENTITY_SPRITE;
  ::update_sprite_gallery (*data_p);
  ::set_current_image (data_p->currentSprite,
                       (*iterator).second.second);

  // load active combobox entry (if any)
  bool desensitize_frame = false;
  GtkComboBox* repository_combobox =
    GTK_COMBO_BOX (glade_xml_get_widget ((*iterator).second.second,
                                         ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_COMBOBOX_CHARACTER_NAME)));
  ACE_ASSERT (repository_combobox);
  if (gtk_combo_box_get_active (repository_combobox) >= 0)
    g_signal_emit_by_name (repository_combobox,
                           ACE_TEXT_ALWAYS_CHAR ("changed"),
                           userData_in);
  else
    desensitize_frame = true;

  // make character display frame in-sensitive ?
  if (desensitize_frame)
  {
    GtkFrame* character_frame =
      GTK_FRAME (glade_xml_get_widget ((*iterator_2).second.second,
                                       ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_FRAME_CHARACTER_NAME)));
    ACE_ASSERT (character_frame);
    gtk_widget_set_sensitive (GTK_WIDGET (character_frame), FALSE);
  } // end IF
  // make create button sensitive
  GtkButton* button =
    GTK_BUTTON (glade_xml_get_widget ((*iterator).second.second,
                                      ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_BUTTON_CREATE_NAME)));
  ACE_ASSERT (button);
  gtk_widget_set_sensitive (GTK_WIDGET (button), TRUE);
  // make save button in-sensitive (if it's not already)
  button =
    GTK_BUTTON (glade_xml_get_widget ((*iterator).second.second,
                                      ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_BUTTON_STORE_NAME)));
  ACE_ASSERT (button);
  gtk_widget_set_sensitive (GTK_WIDGET (button), FALSE);
  // make load button sensitive (if it's not already)
  button =
    GTK_BUTTON (glade_xml_get_widget ((*iterator).second.second,
                                      ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_BUTTON_LOAD_NAME)));
  ACE_ASSERT (button);
  gtk_widget_set_sensitive (GTK_WIDGET (button), TRUE);
  // make this in-sensitive
  gtk_widget_set_sensitive (widget_in, FALSE);

  return FALSE;
}

G_MODULE_EXPORT gint
load_character_clicked_GTK_cb (GtkWidget* widget_in,
                               gpointer userData_in)
{
  RPG_TRACE (ACE_TEXT ("::load_character_clicked_GTK_cb"));

  ACE_UNUSED_ARG (widget_in);
  GTK_cb_data_t* data_p = static_cast<GTK_cb_data_t*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);

  Common_UI_GladeXMLsIterator_t iterator =
    data_p->GTKState.gladeXML.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != data_p->GTKState.gladeXML.end ());

  // *NOTE*: this callback just presents the file picker
  // --> business logic happens in character_file_actived_GTK_cb

  // retrieve file chooser dialog handle
  GtkFileChooserDialog* filechooser_dialog =
    GTK_FILE_CHOOSER_DIALOG (glade_xml_get_widget ((*iterator).second.second,
                                                   ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_DIALOG_FILECHOOSER_NAME)));
  ACE_ASSERT (filechooser_dialog);

  // draw it
  gtk_dialog_run (GTK_DIALOG (filechooser_dialog));
  gtk_widget_hide (GTK_WIDGET (filechooser_dialog));
  //if (!GTK_WIDGET_VISIBLE(filechooser_dialog))
  //  gtk_widget_show(GTK_WIDGET(filechooser_dialog));

  return FALSE;
}

G_MODULE_EXPORT gint
character_file_activated_GTK_cb (GtkWidget* widget_in,
                                 gpointer userData_in)
{
  RPG_TRACE (ACE_TEXT ("::character_file_activated_GTK_cb"));

  ACE_UNUSED_ARG (widget_in);
  GTK_cb_data_t* data_p = static_cast<GTK_cb_data_t*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);

  Common_UI_GladeXMLsIterator_t iterator =
    data_p->GTKState.gladeXML.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  Common_UI_GladeXMLsIterator_t iterator_2 =
    data_p->GTKState.gladeXML.find (ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != data_p->GTKState.gladeXML.end ());
  ACE_ASSERT (iterator_2 != data_p->GTKState.gladeXML.end ());

  // retrieve file chooser dialog handle and hide widget
  GtkFileChooserDialog* filechooser_dialog =
    GTK_FILE_CHOOSER_DIALOG (glade_xml_get_widget ((*iterator).second.second,
                                                   ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_DIALOG_FILECHOOSER_NAME)));
  ACE_ASSERT (filechooser_dialog);
  gtk_widget_hide (GTK_WIDGET (filechooser_dialog));
  std::string filename;
  gchar* filename_utf8 =
    gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (filechooser_dialog));
  if (!filename_utf8)
    return FALSE; // done
  filename = Common_UI_Tools::UTF82Locale (filename_utf8);
  g_free (filename_utf8);

  // clean up
  if (data_p->entity.character)
  {
    delete data_p->entity.character;
    data_p->entity.character = NULL;
    data_p->entity.position =
      std::make_pair (std::numeric_limits<unsigned int>::max (),
                      std::numeric_limits<unsigned int>::max ());
    data_p->entity.modes.clear ();
    data_p->entity.actions.clear ();
    data_p->entity.is_spawned = false;
  } // end IF

  // load entity
  RPG_Character_Conditions_t condition;
  condition.insert (CONDITION_NORMAL);
  short int hitpoints = std::numeric_limits<short int>::max ();
  RPG_Magic_Spells_t spells;
  data_p->entity.character = RPG_Player::load (filename,
                                               data_p->schemaRepository,
                                               condition,
                                               hitpoints,
                                               spells);
  if (!data_p->entity.character)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to RPG_Player::load(\"%s\"), aborting\n"),
                ACE_TEXT (filename.c_str ())));
    return FALSE;
  } // end IF
  RPG_Player_Player_Base* player_base = NULL;
  try
  {
    player_base =
      dynamic_cast<RPG_Player_Player_Base*> (data_p->entity.character);
  }
  catch (...)
  {
    player_base = NULL;
  }
  if (!player_base)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to dynamic_cast<RPG_Player_Player_Base*>(%@), aborting\n"),
                data_p->entity.character));
    return FALSE;
  } // end IF
  data_p->currentSprite =
    RPG_Client_Common_Tools::class2Sprite (player_base->getClass ());
  data_p->isTransient = false;

  // update entity profile widgets
  ::update_entity_profile (data_p->entity,
                           (*iterator_2).second.second);
  ::update_sprite_gallery (*data_p);

  // make character display frame sensitive (if it's not already)
  GtkFrame* character_frame =
    GTK_FRAME (glade_xml_get_widget ((*iterator_2).second.second,
                                     ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_FRAME_CHARACTER_NAME)));
  ACE_ASSERT (character_frame);
  gtk_widget_set_sensitive (GTK_WIDGET (character_frame), TRUE);
  // make create button in-sensitive (if it's not already)
  GtkButton* button =
    GTK_BUTTON (glade_xml_get_widget ((*iterator).second.second,
                                      ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_BUTTON_CREATE_NAME)));
  ACE_ASSERT (button);
  gtk_widget_set_sensitive (GTK_WIDGET (button), FALSE);
  // make drop button sensitive (if it's not already)
  button =
    GTK_BUTTON (glade_xml_get_widget ((*iterator).second.second,
                                      ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_BUTTON_DROP_NAME)));
  ACE_ASSERT (button);
  gtk_widget_set_sensitive (GTK_WIDGET (button), TRUE);
  // make save button in-sensitive (if it's not already)
  button =
    GTK_BUTTON (glade_xml_get_widget ((*iterator).second.second,
                                      ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_BUTTON_STORE_NAME)));
  ACE_ASSERT (button);
  gtk_widget_set_sensitive (GTK_WIDGET (button), FALSE);
  // make load button in-sensitive (if it's not already)
  button =
    GTK_BUTTON (glade_xml_get_widget ((*iterator).second.second,
                                      ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_BUTTON_LOAD_NAME)));
  ACE_ASSERT (button);
  gtk_widget_set_sensitive (GTK_WIDGET (button), FALSE);

  return FALSE;
}

G_MODULE_EXPORT gint
store_character_clicked_GTK_cb (GtkWidget* widget_in,
                                gpointer userData_in)
{
  RPG_TRACE (ACE_TEXT ("::store_character_clicked_GTK_cb"));

  ACE_UNUSED_ARG (widget_in);
  GTK_cb_data_t* data_p = static_cast<GTK_cb_data_t*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);
  ACE_ASSERT (data_p->entity.character);

  Common_UI_GladeXMLsIterator_t iterator =
    data_p->GTKState.gladeXML.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != data_p->GTKState.gladeXML.end ());

  // assemble target filename
  std::string filename = RPG_Player_Common_Tools::getPlayerProfilesDirectory ();
  filename += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  RPG_Player* player_p = NULL;
  try
  {
    player_p = dynamic_cast<RPG_Player*> (data_p->entity.character);
  }
  catch (...)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in dynamic_cast<RPG_Player*>(%@), aborting\n"),
                data_p->entity.character));
    return FALSE;
  }
  if (!player_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to dynamic_cast<RPG_Player*>(%@), aborting\n"),
                data_p->entity.character));
    return FALSE;
  }
  filename += RPG_Common_Tools::sanitize (player_p->getName ());
  filename += ACE_TEXT_ALWAYS_CHAR (RPG_PLAYER_PROFILE_EXT);
  if (!player_p->save (filename))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to RPG_Player::save(\"%s\"), continuing\n"),
                ACE_TEXT (filename.c_str ())));
  data_p->isTransient = false;

  // make create button sensitive (if it's not already)
  GtkButton* button =
    GTK_BUTTON (glade_xml_get_widget ((*iterator).second.second,
                                      ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_BUTTON_CREATE_NAME)));
  ACE_ASSERT (button);
  gtk_widget_set_sensitive (GTK_WIDGET (button), TRUE);
  // make store button in-sensitive
  gtk_widget_set_sensitive (widget_in, FALSE);
  // make create button sensitive (if it's not already)
  button =
    GTK_BUTTON (glade_xml_get_widget ((*iterator).second.second,
                                      ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_BUTTON_LOAD_NAME)));
  ACE_ASSERT (button);
  gtk_widget_set_sensitive (GTK_WIDGET (button), TRUE);

  return FALSE;
}

G_MODULE_EXPORT gint
character_repository_combobox_changed_GTK_cb (GtkWidget* widget_in,
                                              gpointer userData_in)
{
  RPG_TRACE (ACE_TEXT ("::character_repository_combobox_changed_GTK_cb"));

  GTK_cb_data_t* data_p = static_cast<GTK_cb_data_t*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (widget_in);
  ACE_ASSERT (data_p);

  Common_UI_GladeXMLsIterator_t iterator =
    data_p->GTKState.gladeXML.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  Common_UI_GladeXMLsIterator_t iterator_2 =
    data_p->GTKState.gladeXML.find (ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != data_p->GTKState.gladeXML.end ());
  ACE_ASSERT (iterator_2 != data_p->GTKState.gladeXML.end ());

  // retrieve active item
  std::string active_item;
  GtkTreeIter selected;
  GtkTreeModel* model = NULL;
  GValue value;
  const gchar* text = NULL;
  if (!gtk_combo_box_get_active_iter (GTK_COMBO_BOX (widget_in), &selected))
  {
    // *WARNING*: refreshing the combobox triggers removal of items
    // which also generates this signal...
    return FALSE;
  } // end IF
  model = gtk_combo_box_get_model (GTK_COMBO_BOX (widget_in));
  ACE_ASSERT (model);
  ACE_OS::memset (&value, 0, sizeof (value));
  gtk_tree_model_get_value (model, &selected,
                            0, &value);
  text = g_value_get_string (&value);
  // sanity check
  ACE_ASSERT (text);
  active_item = ACE_TEXT_ALWAYS_CHAR (text);
  g_value_unset (&value);

  // clean up
  if (data_p->entity.character)
  {
    delete data_p->entity.character;
    data_p->entity.character = NULL;
    data_p->entity.position =
      std::make_pair (std::numeric_limits<unsigned int>::max (),
                      std::numeric_limits<unsigned int>::max ());
    data_p->entity.modes.clear ();
    data_p->entity.actions.clear ();
    data_p->entity.is_spawned = false;
  } // end IF

  // construct filename
  std::string filename = RPG_Player_Common_Tools::getPlayerProfilesDirectory ();
  filename += ACE_DIRECTORY_SEPARATOR_CHAR_A;
  filename += active_item;
  filename += ACE_TEXT_ALWAYS_CHAR (RPG_PLAYER_PROFILE_EXT);
  // load entity profile
  RPG_Character_Conditions_t condition;
  short int hitpoints = std::numeric_limits<short int>::max ();
  RPG_Magic_Spells_t spells;
  data_p->entity.character = RPG_Player::load (filename,
                                               data_p->schemaRepository,
                                               condition,
                                               hitpoints,
                                               spells);
  if (!data_p->entity.character)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to RPG_Player::load(\"%s\"), aborting\n"),
                ACE_TEXT (filename.c_str ())));
    return FALSE;
  } // end IF
  RPG_Player* player_p = NULL;
  try
  {
    player_p =
      dynamic_cast<RPG_Player*> (data_p->entity.character);
  }
  catch (...)
  {
    player_p = NULL;
  }
  if (!player_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to dynamic_cast<RPG_Player*>(%@), aborting\n"),
                data_p->entity.character));
    return FALSE;
  } // end IF
  data_p->currentSprite =
    RPG_Client_Common_Tools::class2Sprite (player_p->getClass ());
  data_p->isTransient = false;

  // update entity profile widgets
  ::update_character_profile (*player_p,
                              (*iterator_2).second.second);
  ::set_current_image (data_p->currentSprite,
                       (*iterator).second.second);
  ::update_sprite_gallery (*data_p);

  // make character display frame sensitive (if it's not already)
  GtkFrame* character_frame =
    GTK_FRAME (glade_xml_get_widget ((*iterator_2).second.second,
                                     ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_FRAME_CHARACTER_NAME)));
  ACE_ASSERT (character_frame);
  gtk_widget_set_sensitive (GTK_WIDGET (character_frame), TRUE);
  // make drop button sensitive (if it's not already)
  GtkButton* button =
    GTK_BUTTON (glade_xml_get_widget ((*iterator).second.second,
                                      ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_BUTTON_DROP_NAME)));
  ACE_ASSERT (button);
  gtk_widget_set_sensitive (GTK_WIDGET (button),
                            (player_p->getName () != ACE_TEXT_ALWAYS_CHAR (RPG_PLAYER_DEF_NAME)));
  // make save button in-sensitive (if it's not already)
  button =
    GTK_BUTTON (glade_xml_get_widget ((*iterator).second.second,
                                      ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_BUTTON_STORE_NAME)));
  ACE_ASSERT (button);
  gtk_widget_set_sensitive (GTK_WIDGET (button), FALSE);

  return FALSE;
}

G_MODULE_EXPORT gint
character_repository_button_clicked_GTK_cb (GtkWidget* widget_in,
                                            gpointer userData_in)
{
  RPG_TRACE (ACE_TEXT ("::character_repository_button_clicked_GTK_cb"));

  ACE_UNUSED_ARG (widget_in);
  GTK_cb_data_t* data_p = static_cast<GTK_cb_data_t*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);

  Common_UI_GladeXMLsIterator_t iterator =
    data_p->GTKState.gladeXML.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != data_p->GTKState.gladeXML.end ());

  // retrieve tree model
  GtkComboBox* repository_combobox =
    GTK_COMBO_BOX (glade_xml_get_widget ((*iterator).second.second,
                                         ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_COMBOBOX_CHARACTER_NAME)));
  ACE_ASSERT (repository_combobox);
  GtkTreeModel* model = gtk_combo_box_get_model (repository_combobox);
  ACE_ASSERT (model);

  // re-load profile data
  std::string profiles_directory =
    RPG_Player_Common_Tools::getPlayerProfilesDirectory ();
  unsigned int num_entries = ::load_files (REPOSITORY_PROFILES,
                                           GTK_LIST_STORE (model));

  // ... sensitize/activate widgets as appropriate
  gtk_widget_set_sensitive (GTK_WIDGET (repository_combobox), (num_entries > 0));
  if (num_entries)
    gtk_combo_box_set_active (repository_combobox, 0);

  return FALSE;
}

G_MODULE_EXPORT gint
prev_image_clicked_GTK_cb (GtkWidget* widget_in,
                           gpointer userData_in)
{
  RPG_TRACE (ACE_TEXT ("::prev_image_clicked_GTK_cb"));

  ACE_UNUSED_ARG (widget_in);
  GTK_cb_data_t* data_p = static_cast<GTK_cb_data_t*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);
  ACE_ASSERT (!data_p->spriteGallery.empty ());

  Common_UI_GladeXMLsIterator_t iterator =
    data_p->GTKState.gladeXML.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != data_p->GTKState.gladeXML.end ());

  // update image
  if (data_p->spriteGalleryIterator == data_p->spriteGallery.begin ())
    data_p->spriteGalleryIterator = data_p->spriteGallery.end ();
  data_p->spriteGalleryIterator--;

  ::set_current_image (*(data_p->spriteGalleryIterator),
                       (*iterator).second.second);

  // make character save button sensitive (if it's not already)
  GtkButton* save =
    GTK_BUTTON (glade_xml_get_widget ((*iterator).second.second,
                                      ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_BUTTON_STORE_NAME)));
  ACE_ASSERT (save);
  gtk_widget_set_sensitive (GTK_WIDGET (save), TRUE);

  return FALSE;
}

G_MODULE_EXPORT gint
next_image_clicked_GTK_cb (GtkWidget* widget_in,
                           gpointer userData_in)
{
  RPG_TRACE (ACE_TEXT ("::next_image_clicked_GTK_cb"));

  ACE_UNUSED_ARG (widget_in);
  GTK_cb_data_t* data_p = static_cast<GTK_cb_data_t*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);
  ACE_ASSERT (!data_p->spriteGallery.empty ());

  Common_UI_GladeXMLsIterator_t iterator =
    data_p->GTKState.gladeXML.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != data_p->GTKState.gladeXML.end ());

  // update image
  data_p->spriteGalleryIterator++;
  if (data_p->spriteGalleryIterator == data_p->spriteGallery.end ())
    data_p->spriteGalleryIterator = data_p->spriteGallery.begin ();

  ::set_current_image (*(data_p->spriteGalleryIterator),
                       (*iterator).second.second);

  // make character save button sensitive (if it's not already)
  GtkButton* save =
    GTK_BUTTON (glade_xml_get_widget ((*iterator).second.second,
                                      ACE_TEXT_ALWAYS_CHAR (RPG_CLIENT_GTK_BUTTON_STORE_NAME)));
  ACE_ASSERT (save);
  gtk_widget_set_sensitive (GTK_WIDGET (save), TRUE);

  return FALSE;
}
#ifdef __cplusplus
}
#endif /* __cplusplus */
