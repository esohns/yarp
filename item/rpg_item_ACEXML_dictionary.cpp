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
#include "rpg_item_dictionary.h"

#include "rpg_item_dictionary_parser.h"

#include <ACEXML/common/FileCharStream.h>
#include <ACEXML/parser/parser/Parser.h>
#include <ace/Trace.h>

RPG_Item_Dictionary::RPG_Item_Dictionary()
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Dictionary::RPG_Item_Dictionary"));

}

RPG_Item_Dictionary::~RPG_Item_Dictionary()
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Dictionary::~RPG_Item_Dictionary"));

}

void RPG_Item_Dictionary::initItemDictionary(const std::string& filename_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Dictionary::initItemDictionary"));

  ACEXML_FileCharStream* fileStream = NULL;
  try
  {
    fileStream = new ACEXML_FileCharStream();
  }
  catch(...)
  {
    ACE_DEBUG((LM_CRITICAL,
               ACE_TEXT("caught exception in new, returning\n")));

    return;
  }
  if (!fileStream)
  {
    ACE_DEBUG((LM_CRITICAL,
               ACE_TEXT("unable to allocate memory, returning\n")));

    return;
  } // end IF

  if (fileStream->open(const_cast<char*> (filename_in.c_str())))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to open XML file: \"%s\", returning\n"),
               filename_in.c_str()));

    // clean up
    delete fileStream;
    fileStream = NULL;

    return;
  } // end IF
  ACEXML_CharStream* stream = fileStream;

  // *IMPORTANT NOTE*: input assumes lifetime responsibility for fileStream !
  ACEXML_InputSource input(stream);

  RPG_Item_Dictionary_Parser handler(filename_in,
                                     &RPG_ITEM_DICTIONARY_SINGLETON::instance()->myWeaponDictionary,
                                     &RPG_ITEM_DICTIONARY_SINGLETON::instance()->myArmorDictionary);
  ACEXML_Parser parser;
  parser.setContentHandler(&handler);
  parser.setDTDHandler(&handler);
  parser.setErrorHandler(&handler);
  parser.setEntityResolver(&handler);

  // OK: parse the file...
  try
  {
    parser.parse(&input);
  }
  catch(ACEXML_SAXParseException exception)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("RPG_Item_Dictionary::initItemDictionary(): exception occurred, returning\n")));

    exception.print();

    return;
  }
  catch(...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("RPG_Item_Dictionary::initItemDictionary(): exception occurred, returning\n")));

    return;
  }

  // debug info
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("parsed dictionary file \"%s\", retrieved %d weapons and %d armors...\n"),
             filename_in.c_str(),
             myWeaponDictionary.size(),
             myArmorDictionary.size()));
}
