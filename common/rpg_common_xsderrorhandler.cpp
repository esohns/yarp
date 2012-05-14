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

#include "rpg_common_macros.h"
#include "rpg_XMLSchema_XML_tree.h"

#include <ace/Log_Msg.h>

RPG_Common_XSDErrorHandler::RPG_Common_XSDErrorHandler()
 : myFailed(false)
{
  RPG_TRACE(ACE_TEXT("RPG_Common_XSDErrorHandler::RPG_Common_XSDErrorHandler"));

}

RPG_Common_XSDErrorHandler::~RPG_Common_XSDErrorHandler()
{
  RPG_TRACE(ACE_TEXT("RPG_Common_XSDErrorHandler::~RPG_Common_XSDErrorHandler"));

}

void
RPG_Common_XSDErrorHandler::reset()
{
  RPG_TRACE(ACE_TEXT("RPG_Common_XSDErrorHandler::reset"));

  myFailed = false;
}

bool
RPG_Common_XSDErrorHandler::failed()
{
  RPG_TRACE(ACE_TEXT("RPG_Common_XSDErrorHandler::failed"));

  return myFailed;
}

bool
RPG_Common_XSDErrorHandler::handle(const std::string& id_in,
                                   unsigned long line_in,
                                   unsigned long column_in,
                                   ::xsd::cxx::xml::error_handler<char>::severity severity_in,
                                   const std::string& message_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Common_XSDErrorHandler::handle"));

//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("error occured (ID: \"%s\", location: %d, %d): \"%s\", continuing\n"),
//              id_in.c_str(),
//              line_in,
//              column_in,
//              message_in.c_str()));

  switch (severity_in)
  {
    case ::xml_schema::error_handler::severity::warning:
    {
      ACE_DEBUG((LM_WARNING,
                 ACE_TEXT("WARNING: error occured (ID: \"%s\", location: %d, %d): \"%s\", continuing\n"),
                 id_in.c_str(),
                 line_in,
                 column_in,
                 message_in.c_str()));

      break;
    }
    case ::xml_schema::error_handler::severity::error:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("ERROR: error occured (ID: \"%s\", location: %d, %d): \"%s\", continuing\n"),
                 id_in.c_str(),
                 line_in,
                 column_in,
                 message_in.c_str()));

      myFailed = true;

      break;
    }
    case ::xml_schema::error_handler::severity::fatal:
    {
      ACE_DEBUG((LM_CRITICAL,
                 ACE_TEXT("FATAL: error occured (ID: \"%s\", location: %d, %d): \"%s\", continuing\n"),
                 id_in.c_str(),
                 line_in,
                 column_in,
                 message_in.c_str()));

      myFailed = true;

      break;
    }
    default:
    {
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("unkown error occured (ID: \"%s\", location: %d, %d): \"%s\", continuing\n"),
                 id_in.c_str(),
                 line_in,
                 column_in,
                 message_in.c_str()));

      myFailed = true;

      break;
    }
  } // end SWITCH

  // try to continue ?
  return myFailed;
}

// *********************************************************************** //

RPG_Common_XercesErrorHandler::RPG_Common_XercesErrorHandler()
 : myFailed(false)
{
  RPG_TRACE(ACE_TEXT("RPG_Common_XercesErrorHandler::RPG_Common_XercesErrorHandler"));

}

RPG_Common_XercesErrorHandler::~RPG_Common_XercesErrorHandler()
{
  RPG_TRACE(ACE_TEXT("RPG_Common_XercesErrorHandler::~RPG_Common_XercesErrorHandler"));

}

void
RPG_Common_XercesErrorHandler::resetErrors()
{
  RPG_TRACE(ACE_TEXT("RPG_Common_XercesErrorHandler::resetErrors"));

  myFailed = false;
}

bool
RPG_Common_XercesErrorHandler::failed()
{
  RPG_TRACE(ACE_TEXT("RPG_Common_XercesErrorHandler::failed"));

  return myFailed;
}

void
RPG_Common_XercesErrorHandler::warning(const ::xercesc::SAXParseException& exception_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Common_XercesErrorHandler::warning"));

  ACE_DEBUG((LM_WARNING,
             ACE_TEXT("WARNING: error occured (ID: \"%s\", location: %d, %d): \"%s\", continuing\n"),
             exception_in.getSystemId(),
             exception_in.getLineNumber(),
             exception_in.getColumnNumber(),
             exception_in.getMessage()));
}

void
RPG_Common_XercesErrorHandler::error(const ::xercesc::SAXParseException& exception_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Common_XercesErrorHandler::error"));

  ACE_DEBUG((LM_ERROR,
             ACE_TEXT("ERROR: error occured (ID: \"%s\", location: %d, %d): \"%s\", continuing\n"),
             exception_in.getSystemId(),
             exception_in.getLineNumber(),
             exception_in.getColumnNumber(),
             exception_in.getMessage()));

  myFailed = true;
}

void
RPG_Common_XercesErrorHandler::fatalError(const ::xercesc::SAXParseException& exception_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Common_XercesErrorHandler::fatalError"));

  ACE_DEBUG((LM_ERROR,
             ACE_TEXT("FATAL: error occured (ID: \"%s\", location: %d, %d): \"%s\", continuing\n"),
             exception_in.getSystemId(),
             exception_in.getLineNumber(),
             exception_in.getColumnNumber(),
             exception_in.getMessage()));

  myFailed = true;
}
