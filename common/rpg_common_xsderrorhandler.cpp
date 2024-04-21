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

#include "rpg_common_xsderrorhandler.h"

#include "xercesc/util/XMLString.hpp"
#include "xercesc/util/PlatformUtils.hpp"

#include "ace/Log_Msg.h"

#include "rpg_XMLSchema_XML_tree.h"
#include "rpg_common_macros.h"

RPG_Common_XSDErrorHandler::RPG_Common_XSDErrorHandler ()
 : inherited ()
 , myFailed (false)
{
  RPG_TRACE (ACE_TEXT ("RPG_Common_XSDErrorHandler::RPG_Common_XSDErrorHandler"));

}

bool
RPG_Common_XSDErrorHandler::handle (const std::string& id_in,
                                    unsigned long line_in,
                                    unsigned long column_in,
                                    ::xsd::cxx::xml::error_handler<char>::severity severity_in,
                                    const std::string& message_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Common_XSDErrorHandler::handle"));

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("error occured (ID: \"%s\", location: [%d,%d]): \"%s\", continuing\n"),
//              ACE_TEXT(id_in.c_str()),
//              line_in,
//              column_in,
//              ACE_TEXT(message_in.c_str())));

  switch (severity_in)
  {
    case ::xml_schema::error_handler::severity::warning:
    {
      ACE_DEBUG ((LM_WARNING,
                  ACE_TEXT ("WARNING: error occured (ID: \"%s\", location: [%d,%d]): \"%s\", continuing\n"),
                  ACE_TEXT (id_in.c_str ()),
                  line_in,
                  column_in,
                  ACE_TEXT (message_in.c_str ())));
      break;
    }
    case ::xml_schema::error_handler::severity::error:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("ERROR: error occured (ID: \"%s\", location: [%d,%d]): \"%s\", aborting\n"),
                  ACE_TEXT (id_in.c_str ()),
                  line_in,
                  column_in,
                  ACE_TEXT (message_in.c_str ())));
      myFailed = true;
      break;
    }
    case ::xml_schema::error_handler::severity::fatal:
    {
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("FATAL: error occured (ID: \"%s\", location: [%d,%d]): \"%s\", aborting\n"),
                  ACE_TEXT (id_in.c_str ()),
                  line_in,
                  column_in,
                  ACE_TEXT (message_in.c_str ())));
      myFailed = true;
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("unkown error occured (ID: \"%s\", location: [%d,%d]): \"%s\", aborting\n"),
                  ACE_TEXT (id_in.c_str ()),
                  line_in,
                  column_in,
                  ACE_TEXT (message_in.c_str ())));
      myFailed = true;
      break;
    }
  } // end SWITCH

  // try to continue ?
  return myFailed;
}

// *********************************************************************** //

RPG_Common_XercesErrorHandler::RPG_Common_XercesErrorHandler ()
 : inherited ()
 , myFailed (false)
{
  RPG_TRACE (ACE_TEXT ("RPG_Common_XercesErrorHandler::RPG_Common_XercesErrorHandler"));

}

void
RPG_Common_XercesErrorHandler::warning (const ::xercesc::SAXParseException& exception_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Common_XercesErrorHandler::warning"));

  std::string system_id =
    ::xercesc::XMLString::transcode (exception_in.getSystemId (),
                                     ::xercesc::XMLPlatformUtils::fgMemoryManager);
  std::string message   =
    ::xercesc::XMLString::transcode (exception_in.getMessage (),
                                     ::xercesc::XMLPlatformUtils::fgMemoryManager);

  ACE_DEBUG ((LM_WARNING,
              ACE_TEXT ("WARNING: error occured (ID: \"%s\", location [%Q,%Q]): \"%s\", continuing\n"),
              ACE_TEXT (system_id.c_str ()),
              exception_in.getLineNumber (),
              exception_in.getColumnNumber (),
              ACE_TEXT (message.c_str ())));
}

void
RPG_Common_XercesErrorHandler::error (const ::xercesc::SAXParseException& exception_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Common_XercesErrorHandler::error"));

  std::string system_id =
    ::xercesc::XMLString::transcode (exception_in.getSystemId (),
                                     ::xercesc::XMLPlatformUtils::fgMemoryManager);
  std::string message   =
    ::xercesc::XMLString::transcode (exception_in.getMessage (),
                                     ::xercesc::XMLPlatformUtils::fgMemoryManager);

  ACE_DEBUG ((LM_ERROR,
              ACE_TEXT ("ERROR: error occured (ID: \"%s\", location: [%Q,%Q]): \"%s\", continuing\n"),
              ACE_TEXT (system_id.c_str()),
              exception_in.getLineNumber (),
              exception_in.getColumnNumber (),
              ACE_TEXT (message.c_str ())));

  myFailed = true;
}

void
RPG_Common_XercesErrorHandler::fatalError (const ::xercesc::SAXParseException& exception_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Common_XercesErrorHandler::fatalError"));

  std::string system_id =
    ::xercesc::XMLString::transcode (exception_in.getSystemId (),
                                     ::xercesc::XMLPlatformUtils::fgMemoryManager);
  std::string message   =
    ::xercesc::XMLString::transcode (exception_in.getMessage (),
                                     ::xercesc::XMLPlatformUtils::fgMemoryManager);

  ACE_DEBUG ((LM_ERROR,
              ACE_TEXT ("FATAL: error occured (ID: \"%s\", location: [%Q,%Q]): \"%s\", continuing\n"),
              ACE_TEXT (system_id.c_str ()),
              exception_in.getLineNumber (),
              exception_in.getColumnNumber (),
              ACE_TEXT (message.c_str ())));

  myFailed = true;
}
