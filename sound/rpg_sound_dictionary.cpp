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
#include "rpg_sound_dictionary.h"

#include "rpg_sound_XML_parser.h"
#include "rpg_sound_common_tools.h"

#include <rpg_common_macros.h>

#include <ace/Log_Msg.h>

#include <string>
#include <sstream>

RPG_Sound_Dictionary::RPG_Sound_Dictionary()
{
  RPG_TRACE(ACE_TEXT("RPG_Sound_Dictionary::RPG_Sound_Dictionary"));

}

RPG_Sound_Dictionary::~RPG_Sound_Dictionary()
{
  RPG_TRACE(ACE_TEXT("RPG_Sound_Dictionary::~RPG_Sound_Dictionary"));

}

void RPG_Sound_Dictionary::init(const std::string& filename_in,
                                const bool& validateXML_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Sound_Dictionary::init"));

  // Construct the parser.
  //
  RPG_Sound_Category_Type                     category_p;
  RPG_Sound_Event_Type                        event_p;
  ::xml_schema::string_pimpl                  string_p;
  ::xml_schema::unsigned_byte_pimpl           unsigned_byte_p;
  RPG_Sound_Type                              sound_p;
  sound_p.parsers(category_p,
                  event_p,
                  string_p,
                  unsigned_byte_p);

  RPG_Sound_Dictionary_Type                   dictionary_p(&myDictionary);
  dictionary_p.parsers(sound_p);

  // Parse the document to obtain the object model.
  //
  ::xml_schema::document doc_p(dictionary_p,
                               ACE_TEXT_ALWAYS_CHAR("urn:rpg"),
                               ACE_TEXT_ALWAYS_CHAR("soundDictionary"));

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
               ACE_TEXT("RPG_Sound_Dictionary::init(): exception occurred: \"%s\", returning\n"),
               text.c_str()));

    throw(exception);
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("RPG_Sound_Dictionary::init(): exception occurred, returning\n")));

    throw;
  }

  dictionary_p.post_RPG_Sound_Dictionary_Type();

//   // debug info
//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("finished parsing sound dictionary file \"%s\"...\n"),
//              filename_in.c_str()));
}

const RPG_Sound_t RPG_Sound_Dictionary::getSound(const RPG_Sound_Event& event_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Sound_Dictionary::getSound"));

  RPG_Sound_DictionaryIterator_t iterator = myDictionary.find(event_in);
  if (iterator == myDictionary.end())
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid sound event \"%s\", continuing\n"),
               RPG_Sound_EventHelper::RPG_Sound_EventToString(event_in).c_str()));

    // *TODO*: what else can we do ?
    ACE_ASSERT(false);
  } // end IF

  return iterator->second;
}

bool RPG_Sound_Dictionary::XSD_Error_Handler::handle(const std::string& id_in,
                                                     unsigned long line_in,
                                                     unsigned long column_in,
                                                     ::xml_schema::error_handler::severity severity_in,
                                                     const std::string& message_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Sound_Dictionary::XSD_Error_Handler::handle"));

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

      break;
    }
  } // end SWITCH

  // try to continue anyway...
  return true;
}

void RPG_Sound_Dictionary::dump() const
{
  RPG_TRACE(ACE_TEXT("RPG_Sound_Dictionary::dump"));

  std::ostringstream converter;
  unsigned long index = 0;
  for (RPG_Sound_DictionaryIterator_t iterator = myDictionary.begin();
       iterator != myDictionary.end();
       iterator++, index++)
  {
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("Sound[#%u]:\nCategory: %s\nEvent: %s\nFile: %s\nInterval: %u\n"),
               index,
               RPG_Sound_CategoryHelper::RPG_Sound_CategoryToString((iterator->second).category).c_str(),
               RPG_Sound_EventHelper::RPG_Sound_EventToString((iterator->second).event).c_str(),
               ((iterator->second).file).c_str(),
               (iterator->second).interval));
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("===========================\n")));
  } // end FOR
}
