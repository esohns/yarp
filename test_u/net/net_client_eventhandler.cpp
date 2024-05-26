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

#include "net_client_eventhandler.h"

#include "gtk/gtk.h"

#include "common_file_tools.h"

#include "common_ui_common.h"

#include "rpg_common_defines.h"
#include "rpg_common_macros.h"
#include "rpg_common_tools.h"

#include "rpg_player.h"
#include "rpg_player_defines.h"

#include "rpg_engine.h"
#include "rpg_engine_defines.h"
#include "rpg_engine_level.h"

#include "rpg_client_common.h"
#include "rpg_client_engine.h"

#include "net_callbacks.h"

Net_Client_EventHandler::Net_Client_EventHandler (Net_Client_GTK_CBData* CBData_in)
 : CBData_ (CBData_in)
{
  RPG_TRACE (ACE_TEXT ("Net_Client_EventHandler::Net_Client_EventHandler"));

  ACE_ASSERT (CBData_);
}

void
Net_Client_EventHandler::start (Stream_SessionId_t sessionId_in,
                                const struct RPG_Net_Protocol_SessionData& sessionData_in)
{
  RPG_TRACE (ACE_TEXT ("Net_Client_EventHandler::start"));

  ACE_UNUSED_ARG (sessionId_in);
  ACE_UNUSED_ARG (sessionData_in);

  ACE_ASSERT (CBData_);
  ACE_ASSERT (CBData_->UIState);

  guint event_source_id = g_idle_add (idle_start_session_client_cb,
                                      CBData_);
  if (!event_source_id)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to g_idle_add(idle_start_session_client_cb): \"%m\", returning\n")));
    return;
  } // end IF

  { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, CBData_->UIState->lock);
    CBData_->UIState->eventStack.insert (COMMON_UI_EVENT_CONNECT);
  } // end lock scope
}

void
Net_Client_EventHandler::notify (Stream_SessionId_t sessionId_in,
                                 const enum Stream_SessionMessageType& notification_in)
{
  RPG_TRACE (ACE_TEXT ("Net_Client_EventHandler::notify"));

  // *TODO*
  ACE_UNUSED_ARG (sessionId_in);
  ACE_UNUSED_ARG (notification_in);
}

void
Net_Client_EventHandler::end (Stream_SessionId_t sessionId_in)
{
  RPG_TRACE (ACE_TEXT ("Net_Client_EventHandler::end"));

  ACE_UNUSED_ARG (sessionId_in);

  ACE_ASSERT (CBData_);
  ACE_ASSERT (CBData_->UIState);

  guint event_source_id = g_idle_add (idle_end_session_client_cb,
                                      CBData_);
  if (!event_source_id)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to g_idle_add(idle_end_session_client_cb): \"%m\", returning\n")));
    return;
  } // end IF

  { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, CBData_->UIState->lock);
    CBData_->UIState->eventStack.insert (COMMON_UI_EVENT_DISCONNECT);
  } // end lock scope
}

void
Net_Client_EventHandler::notify (Stream_SessionId_t sessionId_in,
                                 const RPG_Net_Protocol_Message& message_in)
{
  RPG_TRACE (ACE_TEXT ("Net_Client_EventHandler::notify"));

  ACE_UNUSED_ARG (sessionId_in);

  const struct RPG_Net_Protocol_Command& data_r = message_in.getR ();

  if (data_r.clientCommand != RPG_NET_PROTOCOL_CLIENT_COMMAND_INVALID)
    goto client_command;

  switch (data_r.command)
  {
    case NET_COMMAND_LEVEL_LOAD:
    { ACE_ASSERT (!data_r.xml.empty ());

      std::string file_string = data_r.xml;
      // replace all RPG_COMMON_XML_CRLF_REPLACEMENT_CHAR with crlf
      std::string::size_type position_i =
        file_string.find (RPG_COMMON_XML_CRLF_REPLACEMENT_CHAR, 0);
      while (position_i != std::string::npos)
      {
        file_string.replace (position_i, 1, ACE_TEXT_ALWAYS_CHAR ("\r\n"));
        position_i = file_string.find (RPG_COMMON_XML_CRLF_REPLACEMENT_CHAR, 0);
      } // end WHILE
    
      std::string temp_filename = Common_File_Tools::getTempDirectory ();
      temp_filename += ACE_DIRECTORY_SEPARATOR_CHAR_A;
      temp_filename +=
        RPG_Common_Tools::sanitize (ACE_TEXT_ALWAYS_CHAR (RPG_ENGINE_LEVEL_DEF_NAME));
      temp_filename += ACE_TEXT_ALWAYS_CHAR (RPG_ENGINE_LEVEL_FILE_EXT);
      if (!Common_File_Tools::store (temp_filename,
                                     reinterpret_cast<const uint8_t*> (file_string.c_str ()),
                                     file_string.size (),
                                     false))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to Common_File_Tools::store(\"%s\"), returning\n"),
                    ACE_TEXT (temp_filename.c_str ())));
        return;
      } // end IF

      struct RPG_Engine_LevelData level;
      if (!RPG_Engine_Level::load (temp_filename,
                                   CBData_->schemaRepository,
                                   level))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to RPG_Engine_Level::load(\"%s\"), returning\n"),
                    ACE_TEXT (temp_filename.c_str ())));
        Common_File_Tools::deleteFile (temp_filename);
        return;
      } // end IF
      Common_File_Tools::deleteFile (temp_filename);
      ACE_ASSERT (CBData_->levelEngine);
      bool was_running_b = false;
      if (CBData_->levelEngine->isRunning ())
      {
        was_running_b = true;
        CBData_->levelEngine->stop ();
      } // end IF
      CBData_->levelEngine->set (level);
      if (was_running_b)
        CBData_->levelEngine->start ();

      guint event_source_id = g_idle_add (idle_new_level_client_cb,
                                          CBData_);
      if (!event_source_id)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to g_idle_add(idle_new_level_client_cb): \"%m\", returning\n")));
        return;
      } // end IF

      break;
    }
    case NET_COMMAND_PLAYER_LOAD:
    { ACE_ASSERT (!data_r.xml.empty ());
      std::string file_string = data_r.xml;
      // replace all RPG_COMMON_XML_CRLF_REPLACEMENT_CHAR with crlf
      std::string::size_type position_i =
        file_string.find (RPG_COMMON_XML_CRLF_REPLACEMENT_CHAR, 0);
      while (position_i != std::string::npos)
      {
        file_string.replace (position_i, 1, ACE_TEXT_ALWAYS_CHAR ("\r\n"));
        position_i = file_string.find (RPG_COMMON_XML_CRLF_REPLACEMENT_CHAR, 0);
      } // end WHILE

      std::string temp_filename = Common_File_Tools::getTempDirectory ();
      temp_filename += ACE_DIRECTORY_SEPARATOR_CHAR_A;
      temp_filename +=
        RPG_Common_Tools::sanitize (ACE_TEXT_ALWAYS_CHAR (RPG_PLAYER_DEF_NAME));
      temp_filename += ACE_TEXT_ALWAYS_CHAR (RPG_PLAYER_PROFILE_EXT);
      if (!Common_File_Tools::store (temp_filename,
                                     reinterpret_cast<const uint8_t*> (file_string.c_str ()),
                                     file_string.size (),
                                     false))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to Common_File_Tools::store(\"%s\"), returning\n"),
                    ACE_TEXT (temp_filename.c_str ())));
        return;
      } // end IF

      RPG_Character_Conditions_t condition;
      condition.insert (CONDITION_NORMAL);
      ACE_INT16 hitpoints = std::numeric_limits<ACE_INT16>::max ();
      RPG_Magic_Spells_t spells;

      struct RPG_Engine_Entity* entity_p = NULL;
      ACE_NEW_NORETURN (entity_p,
                        struct RPG_Engine_Entity ());
      ACE_ASSERT (entity_p);
      entity_p->character = RPG_Player::load (temp_filename,
                                              CBData_->schemaRepository,
                                              condition,
                                              hitpoints,
                                              spells);
      Common_File_Tools::deleteFile (temp_filename);
      ACE_ASSERT (entity_p->character);
      ACE_ASSERT (CBData_->levelEngine);
      RPG_Engine_EntityID_t id_i = CBData_->levelEngine->add (entity_p,
                                                              true); // locked access ?
      ACE_ASSERT (id_i);
      CBData_->entities.insert (std::make_pair (id_i, entity_p));
      break;
    }
    case NET_ENGINE_COMMAND_MAX:
    case NET_ENGINE_COMMAND_INVALID:
    case RPG_NET_PROTOCOL_ENGINE_COMMAND_MAX:
    case RPG_NET_PROTOCOL_ENGINE_COMMAND_INVALID:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("received invalid engine command (was: %d), returning\n"),
                  data_r.command));
      return;
    }
    default:
    { ACE_ASSERT (CBData_->levelEngine);
      ACE_ASSERT (!CBData_->entities.empty ());

      RPG_Engine_EntitiesConstIterator_t iterator = CBData_->entities.begin ();
      RPG_Engine_EntityID_t id_i = (*iterator).first;
      ACE_ASSERT (id_i);

      struct RPG_Engine_Action action_s;
      action_s.command = static_cast<enum RPG_Engine_Command> (data_r.command);
      action_s.path = data_r.path;
      action_s.position = data_r.position;
      action_s.target = data_r.entity_id;

      CBData_->levelEngine->action (id_i,     // entity id
                                    action_s, // action
                                    true);    // locked access ?

      break;
    }
  } // end SWITCH

  return;

client_command:
  switch (data_r.clientCommand)
  {
    case NET_CLIENT_COMMAND_MAX:
    case NET_CLIENT_COMMAND_INVALID:
    case RPG_NET_PROTOCOL_CLIENT_COMMAND_MAX:
    case RPG_NET_PROTOCOL_CLIENT_COMMAND_INVALID:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("received invalid client command (was: %d), returning\n"),
                  data_r.clientCommand));
      return;
    }
    default:
    { ACE_ASSERT (CBData_->clientEngine);
      ACE_ASSERT (CBData_->mapWindow);

      struct RPG_Client_Action action_s;
      action_s.command =
        static_cast<enum RPG_Client_Command> (data_r.clientCommand);
      action_s.previous = data_r.previous;
      action_s.position = data_r.position;
      action_s.window = CBData_->mapWindow; // *TODO*
      action_s.cursor = data_r.cursor;
      action_s.entity_id = data_r.entity_id;
      action_s.sound = data_r.sound;
      // replace all RPG_COMMON_XML_CRLF_REPLACEMENT_CHAR with crlf
      std::string message_string = data_r.message;
      std::string::size_type position_i =
        message_string.find (RPG_COMMON_XML_CRLF_REPLACEMENT_CHAR, 0);
      while (position_i != std::string::npos)
      {
        message_string.replace (position_i, 1, ACE_TEXT_ALWAYS_CHAR ("\r\n"));
        position_i =
          message_string.find (RPG_COMMON_XML_CRLF_REPLACEMENT_CHAR, 0);
      } // end WHILE
      action_s.message = message_string;
      action_s.path = data_r.path;
      action_s.source = data_r.source;
      action_s.positions = data_r.positions;
      action_s.radius = data_r.radius;

      CBData_->clientEngine->action (action_s); // action

      break;
    }
  } // end SWITCH
}

void
Net_Client_EventHandler::notify (Stream_SessionId_t sessionId_in,
                                 const RPG_Net_Protocol_SessionMessage& sessionMessage_in)
{
  RPG_TRACE (ACE_TEXT ("Net_Client_EventHandler::notify"));

  // *TODO*
  ACE_UNUSED_ARG (sessionId_in);
  ACE_UNUSED_ARG (sessionMessage_in);
}
