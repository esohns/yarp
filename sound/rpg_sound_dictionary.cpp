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

#include "rpg_sound_dictionary.h"

#include "rpg_sound_defines.h"
#include "rpg_sound_XML_parser.h"
#include "rpg_sound_common_tools.h"

#include "rpg_common_macros.h"
#include "rpg_common_defines.h"
//#include "rpg_common_xsderrorhandler.h"
#include "rpg_common_XML_tools.h"

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

void
RPG_Sound_Dictionary::init(const std::string& filename_in,
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
                  unsigned_byte_p,
                  string_p,
                  unsigned_byte_p);

  RPG_Sound_Dictionary_Type                   dictionary_p(&myDictionary);
  dictionary_p.parsers(sound_p);

  // Parse the document to obtain the object model.
  //
  ::xml_schema::document doc_p(dictionary_p,                                          // parser
                               ACE_TEXT_ALWAYS_CHAR(RPG_COMMON_XML_TARGET_NAMESPACE), // namespace
                               ACE_TEXT_ALWAYS_CHAR(RPG_SOUND_DICTIONARY_INSTANCE),   // root element name
															 false);                                                // polymorphic ?

  dictionary_p.pre();

  // OK: parse the file...
  ::xml_schema::flags flags;
  if (!validateXML_in)
    flags = flags | ::xml_schema::flags::dont_validate;
  ::xml_schema::properties properties;
  try
  {
    //doc_p.parse(filename_in,
    //            RPG_XSDErrorHandler,
    //            flags);
    doc_p.parse(filename_in,
                *RPG_Common_XML_Tools::parser(),
                flags,
                properties);
  }
  catch (const ::xml_schema::parsing& exception)
  {
    std::ostringstream converter;
    converter << exception;
    std::string text = converter.str();
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("RPG_Sound_Dictionary::init(): exception occurred: \"%s\", returning\n"),
               text.c_str()));

    return;
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("RPG_Sound_Dictionary::init(): exception occurred, returning\n")));

    return;
  }

  dictionary_p.post_RPG_Sound_Dictionary_Type();

//   // debug info
//   ACE_DEBUG((LM_DEBUG,
//              ACE_TEXT("finished parsing sound dictionary file \"%s\"...\n"),
//              filename_in.c_str()));
}

const RPG_Sound_t&
RPG_Sound_Dictionary::get(const RPG_Sound_Event& event_in) const
{
  RPG_TRACE(ACE_TEXT("RPG_Sound_Dictionary::get"));

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

void RPG_Sound_Dictionary::dump() const
{
  RPG_TRACE(ACE_TEXT("RPG_Sound_Dictionary::dump"));

  std::ostringstream converter;
  unsigned int index = 0;
  for (RPG_Sound_DictionaryIterator_t iterator = myDictionary.begin();
       iterator != myDictionary.end();
       iterator++, index++)
  {
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("Sound[#%u]:\nCategory: %s\nEvent: %s\nFile: %s\nInterval: %u\n"),
               index,
               RPG_Sound_CategoryHelper::RPG_Sound_CategoryToString((iterator->second).category).c_str(),
               RPG_Sound_EventHelper::RPG_Sound_EventToString((iterator->second).sound_event).c_str(),
               ((iterator->second).file).c_str(),
               (iterator->second).interval));
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("===========================\n")));
  } // end FOR
}
