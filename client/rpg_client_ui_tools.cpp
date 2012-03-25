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

#include "rpg_client_ui_tools.h"

#include <rpg_common_macros.h>

#include <ace/Log_Msg.h>

const std::string
RPG_Client_UI_Tools::UTF82Locale(const gchar* string_in,
                                 const gssize& length_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_UI_Tools::UTF82Locale"));

  // init result
  std::string result;

  gchar* converted_text = NULL;
  GError* conversion_error = NULL;
  converted_text = g_locale_from_utf8(string_in,          // text
                                      length_in,          // number of bytes
                                      NULL,               // bytes read (don't care)
                                      NULL,               // bytes written (don't care)
                                      &conversion_error); // return value: error
  if (conversion_error)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to convert string: \"%s\", aborting\n"),
               conversion_error->message));

    // clean up
    g_error_free(conversion_error);
  } // end IF
  else
    result = converted_text;

  // clean up
  g_free(converted_text);

  return result;
}

gchar*
RPG_Client_UI_Tools::Locale2UTF8(const std::string& string_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Client_UI_Tools::Locale2UTF8"));

  // init result
  gchar* result = NULL;

  GError* conversion_error = NULL;
  result = g_locale_to_utf8(string_in.c_str(),  // text
                            -1,                 // \0-terminated
                            NULL,               // bytes read (don't care)
                            NULL,               // bytes written (don't care)
                            &conversion_error); // return value: error
  if (conversion_error)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to convert string: \"%s\", aborting\n"),
               conversion_error->message));

      // clean up
    g_error_free(conversion_error);

    return NULL;
  } // end IF

  return result;
}
