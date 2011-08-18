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
#include "character_generator_gui_callbacks.h"

#include "character_generator_gui_common.h"

#include <rpg_client_defines.h>
#include <rpg_client_callbacks.h>

#include <rpg_engine_common.h>
#include <rpg_engine_common_tools.h>

#include <rpg_graphics_defines.h>
#include <rpg_graphics_surface.h>
#include <rpg_graphics_cursor.h>
#include <rpg_graphics_dictionary.h>
#include <rpg_graphics_common_tools.h>

#include <rpg_map_defines.h>
#include <rpg_map_common_tools.h>

#include <rpg_character_player_common_tools.h>

#include <rpg_character_defines.h>

#include <rpg_item_instance_manager.h>
#include <rpg_item_weapon.h>
#include <rpg_item_armor.h>

#include <rpg_common_macros.h>
#include <rpg_common_defines.h>
#include <rpg_common_tools.h>
#include <rpg_common_file_tools.h>

#include <gmodule.h>

#include <ace/Log_Msg.h>

#include <sstream>

void
update_sprite_gallery(GTK_cb_data_t& CBData_in)
{
  RPG_TRACE(ACE_TEXT("::update_sprite_gallery"));

  for (CBData_in.current_sprite = CBData_in.sprite_gallery.begin();
       CBData_in.current_sprite != CBData_in.sprite_gallery.end();
       CBData_in.current_sprite++)
    if ((*CBData_in.current_sprite) == CBData_in.entity.sprite)
      break;

  // sanity check
  if ((*CBData_in.current_sprite) != CBData_in.entity.sprite)
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("sprite (was: \"%s\") not in gallery (%u entrie(s)), aborting\n"),
               RPG_Graphics_SpriteHelper::RPG_Graphics_SpriteToString(CBData_in.entity.sprite).c_str(),
               CBData_in.sprite_gallery.size()));
}

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
G_MODULE_EXPORT gint
about_activated_GTK_cb(GtkWidget* widget_in,
                       gpointer userData_in)
{
  RPG_TRACE(ACE_TEXT("::about_activated_GTK_cb"));

  ACE_UNUSED_ARG(widget_in);
  GTK_cb_data_t* data = static_cast<GTK_cb_data_t*> (userData_in);
  ACE_ASSERT(data);

  // sanity check(s)
  ACE_ASSERT(data->xml);

  // retrieve about dialog handle
  GtkWidget* about_dialog = GTK_WIDGET(glade_xml_get_widget(data->xml,
                                                            RPG_CLIENT_DEF_GNOME_ABOUTDIALOG_NAME));
  ACE_ASSERT(about_dialog);
  if (!about_dialog)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to glade_xml_get_widget(\"%s\"): \"%m\", aborting\n"),
               RPG_CLIENT_DEF_GNOME_ABOUTDIALOG_NAME));

    return TRUE; // propagate
  } // end IF

  // draw it
  if (!GTK_WIDGET_VISIBLE(about_dialog))
    gtk_widget_show_all(about_dialog);

  return FALSE;
}

G_MODULE_EXPORT gint
quit_activated_GTK_cb(GtkWidget* widget_in,
                      gpointer userData_in)
{
  RPG_TRACE(ACE_TEXT("::quit_activated_GTK_cb"));

  ACE_UNUSED_ARG(widget_in);
  ACE_UNUSED_ARG(userData_in);

  // leave GTK
  gtk_main_quit();

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("leaving GTK...\n")));

  // this is the "delete-event" handler
  // --> destroy the main dialog widget
  return TRUE; // --> propagate
}

G_MODULE_EXPORT gint
create_character_activated_GTK_cb(GtkWidget* widget_in,
                                  gpointer userData_in)
{
  RPG_TRACE(ACE_TEXT("::create_character_activated_GTK_cb"));

  ACE_UNUSED_ARG(widget_in);
  GTK_cb_data_t* data = static_cast<GTK_cb_data_t*> (userData_in);
  ACE_ASSERT(data);

  // erase current entity profile
  if (data->entity.character)
  {
    delete data->entity.character;
    data->entity.character = NULL;
  } // end IF
  data->entity.position = std::make_pair(0, 0);
  data->entity.sprite = *(data->current_sprite);
  if (data->entity.graphic)
  {
    SDL_FreeSurface(data->entity.graphic);
    data->entity.graphic = NULL;
  } // end IF

  // generate (random) player
  data->entity.character = RPG_Character_Player_Common_Tools::generatePlayerCharacter();
  ACE_ASSERT(data->entity.character);

  // update entity profile widgets
  ::update_entity_profile(data->entity,
                          data->xml);

  return FALSE;
}

G_MODULE_EXPORT gint
load_character_activated_GTK_cb(GtkWidget* widget_in,
                                gpointer userData_in)
{
  RPG_TRACE(ACE_TEXT("::load_character_activated_GTK_cb"));

  ACE_UNUSED_ARG(widget_in);
  GTK_cb_data_t* data = static_cast<GTK_cb_data_t*> (userData_in);
  ACE_ASSERT(data);

  // sanity check(s)
  ACE_ASSERT(data->xml);

  // *NOTE*: this callback just presents the file picker
  // --> business logic happens in character_file_actived_GTK_cb

  // retrieve file chooser dialog handle
  GtkFileChooserDialog* filechooser_dialog = GTK_FILE_CHOOSER_DIALOG(glade_xml_get_widget(data->xml,
                                                                                          RPG_CLIENT_DEF_GNOME_FILECHOOSERDIALOG_NAME));
  ACE_ASSERT(filechooser_dialog);
  if (!filechooser_dialog)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to glade_xml_get_widget(\"%s\"): \"%m\", aborting\n"),
               RPG_CLIENT_DEF_GNOME_FILECHOOSERDIALOG_NAME));

    return FALSE;
  } // end IF

  // draw it
  if (!GTK_WIDGET_VISIBLE(filechooser_dialog))
    gtk_widget_show_all(GTK_WIDGET(filechooser_dialog));

  return FALSE;
}

G_MODULE_EXPORT gint
character_file_activated_GTK_cb(GtkWidget* widget_in,
                                gpointer userData_in)
{
  RPG_TRACE(ACE_TEXT("::character_file_activated_GTK_cb"));

  ACE_UNUSED_ARG(widget_in);
  GTK_cb_data_t* data = static_cast<GTK_cb_data_t*> (userData_in);
  ACE_ASSERT(data);

  // sanity check(s)
  ACE_ASSERT(data->xml);

  // retrieve file chooser dialog handle
  GtkFileChooserDialog* filechooser_dialog = GTK_FILE_CHOOSER_DIALOG(glade_xml_get_widget(data->xml,
                                                                                          RPG_CLIENT_DEF_GNOME_FILECHOOSERDIALOG_NAME));
  ACE_ASSERT(filechooser_dialog);

  // hide widget
  gtk_widget_hide(GTK_WIDGET(filechooser_dialog));

  // retrieve selected filename
  std::string filename(gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(filechooser_dialog)));

  // load entity profile
  if (data->entity.character)
  {
    try
    {
      delete data->entity.character;
    }
    catch (...)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("caught exception, aborting\n")));

      return FALSE;
    }
    data->entity.character = NULL;
  } // end IF
  if (data->entity.graphic)
  {
    SDL_FreeSurface(data->entity.graphic);
    data->entity.graphic = NULL;
  } // end IF
  data->entity = RPG_Engine_Common_Tools::loadEntity(filename,
                                                     data->schemaRepository);

  // update entity profile widgets
  ::update_entity_profile(data->entity,
                          data->xml);
  ::update_sprite_gallery(*data);

  // make character display frame sensitive (if it's not already)
  GtkFrame* character_frame = GTK_FRAME(glade_xml_get_widget(data->xml,
                                                             ACE_TEXT_ALWAYS_CHAR("character")));
  ACE_ASSERT(character_frame);
  gtk_widget_set_sensitive(GTK_WIDGET(character_frame), TRUE);

  return FALSE;
}

G_MODULE_EXPORT gint
save_character_activated_GTK_cb(GtkWidget* widget_in,
                                gpointer userData_in)
{
  RPG_TRACE(ACE_TEXT("::save_character_activated_GTK_cb"));

  ACE_UNUSED_ARG(widget_in);
  GTK_cb_data_t* data = static_cast<GTK_cb_data_t*> (userData_in);
  ACE_ASSERT(data);

  // sanity check(s)
  ACE_ASSERT(data->entity.character);

  // set current selected sprite
  data->entity.sprite = *(data->current_sprite);

  // assemble target filename
  std::string filename = RPG_COMMON_DUMP_DIR;
  filename += ACE_DIRECTORY_SEPARATOR_STR;
  filename += data->entity.character->getName();
  filename += RPG_CHARACTER_PROFILE_EXT;

  if (!RPG_Engine_Common_Tools::saveEntity(data->entity,
                                           filename))
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to RPG_Engine_Common_Tools::saveEntity(\"%s\"), continuing\n"),
               filename.c_str()));

  return FALSE;
}

G_MODULE_EXPORT gint
characters_activated_GTK_cb(GtkWidget* widget_in,
                            gpointer userData_in)
{
  RPG_TRACE(ACE_TEXT("::characters_activated_GTK_cb"));

  GTK_cb_data_t* data = static_cast<GTK_cb_data_t*> (userData_in);
  ACE_ASSERT(data);

  // sanity check(s)
  ACE_ASSERT(widget_in);
  ACE_ASSERT(data->xml);

  // retrieve active item
  std::string active_item;
  GtkTreeIter selected;
  GtkTreeModel* model = NULL;
  GValue value;
  const gchar* text = NULL;
  if (!gtk_combo_box_get_active_iter(GTK_COMBO_BOX(widget_in), &selected))
  {
    // *WARNING*: refreshing the combobox triggers removal of items
    // which also generates this signal...
    return FALSE;
  } // end IF
  model = gtk_combo_box_get_model(GTK_COMBO_BOX(widget_in));
  ACE_ASSERT(model);
  ACE_OS::memset(&value,
                 0,
                 sizeof(value));
  gtk_tree_model_get_value(model, &selected,
                           0, &value);
  text = g_value_get_string(&value);
  // sanity check
  ACE_ASSERT(text);
  active_item = text;
  g_value_unset(&value);

  // construct filename
  std::string filename = RPG_CLIENT_DEF_CHARACTER_REPOSITORY;
  filename += ACE_DIRECTORY_SEPARATOR_STR;
  filename += active_item;
  filename += RPG_CHARACTER_PROFILE_EXT;

  // load entity profile
  if (data->entity.character)
  {
    try
    {
      delete data->entity.character;
    }
    catch (...)
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("caught exception, aborting\n")));

      return FALSE;
    }
    data->entity.character = NULL;
  } // end IF
  if (data->entity.graphic)
  {
    SDL_FreeSurface(data->entity.graphic);
    data->entity.graphic = NULL;
  } // end IF
  data->entity = RPG_Engine_Common_Tools::loadEntity(filename,
                                                     data->schemaRepository,
                                                     false); // don't load SDL surface !

  // update entity profile widgets
  ::update_entity_profile(data->entity,
                          data->xml);
  ::update_sprite_gallery(*data);

  // make character display frame sensitive (if it's not already)
  GtkFrame* character_frame = GTK_FRAME(glade_xml_get_widget(data->xml,
                                                             ACE_TEXT_ALWAYS_CHAR("character")));
  ACE_ASSERT(character_frame);
  gtk_widget_set_sensitive(GTK_WIDGET(character_frame), TRUE);

  return FALSE;
}

G_MODULE_EXPORT gint
characters_refresh_activated_GTK_cb(GtkWidget* widget_in,
                                    gpointer userData_in)
{
  RPG_TRACE(ACE_TEXT("::characters_refresh_activated_GTK_cb"));

  ACE_UNUSED_ARG(widget_in);
  GTK_cb_data_t* data = static_cast<GTK_cb_data_t*> (userData_in);
  ACE_ASSERT(data);

  // sanity check(s)
  ACE_ASSERT(data->xml);

  // retrieve tree model
  GtkComboBox* available_characters = GTK_COMBO_BOX(glade_xml_get_widget(data->xml,
                                                                         RPG_CLIENT_DEF_GNOME_CHARBOX_NAME));
  ACE_ASSERT(available_characters);
  GtkTreeModel* model = gtk_combo_box_get_model(available_characters);
  ACE_ASSERT(model);

  // re-load profile data
  ::load_profiles(RPG_CLIENT_DEF_CHARACTER_REPOSITORY,
                  GTK_LIST_STORE(model));

  // set sensitive as appropriate
  GtkFrame* character_frame = GTK_FRAME(glade_xml_get_widget(data->xml,
                                                             ACE_TEXT_ALWAYS_CHAR("character")));
  ACE_ASSERT(character_frame);
  if (g_list_length(gtk_container_get_children(GTK_CONTAINER(available_characters))))
  {
    gtk_widget_set_sensitive(GTK_WIDGET(available_characters), TRUE);
    gtk_widget_set_sensitive(GTK_WIDGET(character_frame), TRUE);
  } // end IF
  else
  {
    gtk_widget_set_sensitive(GTK_WIDGET(available_characters), FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(character_frame), FALSE);
  } // end ELSE

  // ... activate first entry as appropriate
  if (gtk_widget_is_sensitive(GTK_WIDGET(available_characters)))
    gtk_combo_box_set_active(available_characters, 0);

  return FALSE;
}

G_MODULE_EXPORT gint
prev_image_activated_GTK_cb(GtkWidget* widget_in,
                            gpointer userData_in)
{
  RPG_TRACE(ACE_TEXT("::prev_image_activated_GTK_cb"));

  ACE_UNUSED_ARG(widget_in);
  GTK_cb_data_t* data = static_cast<GTK_cb_data_t*> (userData_in);
  ACE_ASSERT(data);
  ACE_ASSERT(data->xml);
  ACE_ASSERT(!data->sprite_gallery.empty());

  // update image
  if (data->current_sprite == data->sprite_gallery.begin())
    data->current_sprite = data->sprite_gallery.end();
  data->current_sprite--;

  // retrieve image widget
  GtkImage* image = GTK_IMAGE(glade_xml_get_widget(data->xml,
                                                   ACE_TEXT_ALWAYS_CHAR("image_sprite")));
  ACE_ASSERT(image);

  // retrieve graphic
  RPG_Graphics_GraphicTypeUnion type;
  type.discriminator = RPG_Graphics_GraphicTypeUnion::SPRITE;
  type.sprite = *(data->current_sprite);
  RPG_Graphics_t graphic = RPG_GRAPHICS_DICTIONARY_SINGLETON::instance()->get(type);
  ACE_ASSERT((graphic.type.discriminator == type.discriminator) &&
             (graphic.type.sprite == type.sprite));
  // assemble path
  std::string filename;
  RPG_Graphics_Common_Tools::graphicToFile(graphic, filename);

  if (!filename.empty())
    gtk_image_set_from_file(image, filename.c_str());
  else
    gtk_image_clear(image);

  // make character save button sensitive (if it's not already)
  GtkButton* save = GTK_BUTTON(glade_xml_get_widget(data->xml,
                                                    ACE_TEXT_ALWAYS_CHAR("save")));
  ACE_ASSERT(save);
  gtk_widget_set_sensitive(GTK_WIDGET(save), TRUE);

  return FALSE;
}

G_MODULE_EXPORT gint
next_image_activated_GTK_cb(GtkWidget* widget_in,
                            gpointer userData_in)
{
  RPG_TRACE(ACE_TEXT("::next_image_activated_GTK_cb"));

  ACE_UNUSED_ARG(widget_in);
  GTK_cb_data_t* data = static_cast<GTK_cb_data_t*> (userData_in);
  ACE_ASSERT(data);
  ACE_ASSERT(data->xml);
  ACE_ASSERT(!data->sprite_gallery.empty());

  // update image
  data->current_sprite++;
  if (data->current_sprite == data->sprite_gallery.end())
    data->current_sprite = data->sprite_gallery.begin();

  // retrieve image widget
  GtkImage* image = GTK_IMAGE(glade_xml_get_widget(data->xml,
                                                   ACE_TEXT_ALWAYS_CHAR("image_sprite")));
  ACE_ASSERT(image);

  // retrieve graphic
  RPG_Graphics_GraphicTypeUnion type;
  type.discriminator = RPG_Graphics_GraphicTypeUnion::SPRITE;
  type.sprite = *(data->current_sprite);
  RPG_Graphics_t graphic = RPG_GRAPHICS_DICTIONARY_SINGLETON::instance()->get(type);
  ACE_ASSERT((graphic.type.discriminator == type.discriminator) &&
             (graphic.type.sprite == type.sprite));
  // assemble path
  std::string filename;
  RPG_Graphics_Common_Tools::graphicToFile(graphic, filename);

  if (!filename.empty())
    gtk_image_set_from_file(image, filename.c_str());
  else
    gtk_image_clear(image);

  // make character save button sensitive (if it's not already)
  GtkButton* save = GTK_BUTTON(glade_xml_get_widget(data->xml,
                                                    ACE_TEXT_ALWAYS_CHAR("save")));
  ACE_ASSERT(save);
  gtk_widget_set_sensitive(GTK_WIDGET(save), TRUE);

  return FALSE;
}
#ifdef __cplusplus
}
#endif /* __cplusplus */
