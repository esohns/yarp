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
#include "rpg_graphics_dictionary.h"

#include "rpg_graphics_XML_parser.h"
#include "rpg_graphics_common_tools.h"

#include <ace/Log_Msg.h>

#include <string>
#include <sstream>

RPG_Graphics_Dictionary::RPG_Graphics_Dictionary()
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Dictionary::RPG_Graphics_Dictionary"));

}

RPG_Graphics_Dictionary::~RPG_Graphics_Dictionary()
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Dictionary::~RPG_Graphics_Dictionary"));

}

void
RPG_Graphics_Dictionary::init(const std::string& filename_in,
                              const bool& validateXML_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Dictionary::init"));

  // Construct the parser.
  //
  RPG_Graphics_Category_Type                  category_p;
  RPG_Graphics_Type_Type                      type_p;
  ::xml_schema::string_pimpl                  string_p;
  RPG_Graphics_Graphic_Type                   graphic_p;
  graphic_p.parsers(category_p,
                    type_p,
                    string_p);

  RPG_Graphics_Dictionary_Type                dictionary_p(&myDictionary);
  dictionary_p.parsers(graphic_p);

  // Parse the document to obtain the object model.
  //
  ::xml_schema::document doc_p(dictionary_p,
                               ACE_TEXT_ALWAYS_CHAR("urn:rpg"),
                               ACE_TEXT_ALWAYS_CHAR("graphicsDictionary"));

  dictionary_p.pre();

  // OK: parse the file...
  ::xml_schema::flags flags;
  if (!validateXML_in)
    flags = flags | ::xml_schema::flags::dont_validate;
  try
  {
    doc_p.parse(filename_in,
                myXSDErrorHandler,
                flags);
  }
  catch (const ::xml_schema::parsing& exception)
  {
    std::ostringstream converter;
    converter << exception;
    std::string text = converter.str();
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("RPG_Graphics_Dictionary::init(): exception occurred: \"%s\", returning\n"),
               text.c_str()));

    throw(exception);
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("RPG_Graphics_Dictionary::init(): exception occurred, returning\n")));

    throw;
  }

  dictionary_p.post_RPG_Graphics_Dictionary_Type();

//   // debug info
//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("finished parsing graphics dictionary file \"%s\"...\n"),
//              filename_in.c_str()));
}

const RPG_Graphics_t
RPG_Graphics_Dictionary::getGraphic(const RPG_Graphics_Type& type_in) const
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Dictionary::getGraphic"));

  RPG_Graphics_DictionaryIterator_t iterator = myDictionary.find(type_in);
  if (iterator == myDictionary.end())
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid graphics type \"%s\", continuing\n"),
               RPG_Graphics_TypeHelper::RPG_Graphics_TypeToString(type_in).c_str()));

    // *TODO*: what else can we do ?
    ACE_ASSERT(false);
  } // end IF

  return iterator->second;
}

bool
RPG_Graphics_Dictionary::XSD_Error_Handler::handle(const std::string& id_in,
                                                   unsigned long line_in,
                                                   unsigned long column_in,
                                                   ::xml_schema::error_handler::severity severity_in,
                                                   const std::string& message_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Dictionary::XSD_Error_Handler::handle"));

//   // debug info
//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("error occured (ID: \"%s\", location: %d, %d): \"%s\" --> check implementation !, continuing\n"),
//              id_in.c_str(),
//              line_in,
//              column_in,
//              message_in.c_str()));

  switch (severity_in)
  {
    case ::xml_schema::error_handler::severity::warning:
    {
      ACE_DEBUG((LM_WARNING,
                 ACE_TEXT("WARNING: error occured (ID: \"%s\", location: %d, %d): \"%s\" --> check implementation !, continuing\n"),
                 id_in.c_str(),
                 line_in,
                 column_in,
                 message_in.c_str()));

      break;
    }
    case ::xml_schema::error_handler::severity::error:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("ERROR: error occured (ID: \"%s\", location: %d, %d): \"%s\" --> check implementation !, continuing\n"),
                 id_in.c_str(),
                 line_in,
                 column_in,
                 message_in.c_str()));

      break;
    }
    case ::xml_schema::error_handler::severity::fatal:
    {
      ACE_DEBUG((LM_CRITICAL,
                 ACE_TEXT("FATAL: error occured (ID: \"%s\", location: %d, %d): \"%s\" --> check implementation !, continuing\n"),
                 id_in.c_str(),
                 line_in,
                 column_in,
                 message_in.c_str()));

      break;
    }
    default:
    {
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("unkown error occured (ID: \"%s\", location: %d, %d): \"%s\" --> check implementation !, continuing\n"),
                 id_in.c_str(),
                 line_in,
                 column_in,
                 message_in.c_str()));

      break;
    }
  } // end SWITCH

  // try to continue anyway...
  return true;
}

void
RPG_Graphics_Dictionary::dump() const
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Dictionary::dump"));

  std::ostringstream converter;
  unsigned long index = 0;
  for (RPG_Graphics_DictionaryIterator_t iterator = myDictionary.begin();
       iterator != myDictionary.end();
       iterator++, index++)
  {
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("Graphic[#%u]:\nCategory: %s\nType: %s\nFile: %s\n"),
               index,
               RPG_Graphics_CategoryHelper::RPG_Graphics_CategoryToString((iterator->second).category).c_str(),
               RPG_Graphics_TypeHelper::RPG_Graphics_TypeToString((iterator->second).type).c_str(),
               ((iterator->second).file).c_str()));
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("===========================\n")));
  } // end FOR
}
