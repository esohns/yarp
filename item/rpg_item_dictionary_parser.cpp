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
#include "rpg_item_dictionary_parser.h"

#include <ace/Trace.h>

RPG_Item_Dictionary_Parser::RPG_Item_Dictionary_Parser(const std::string& fileName_in,
                                                       RPG_ITEM_WEAPON_DICTIONARY_T* weaponDictionary_in,
                                                       RPG_ITEM_ARMOR_DICTIONARY_T* armorDictionary_in)
 : myFileName(fileName_in),
   myWeaponDictionary(weaponDictionary_in),
   myArmorDictionary(armorDictionary_in),
   myLocator(NULL)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Dictionary_Parser::RPG_Item_Dictionary_Parser"));

}

RPG_Item_Dictionary_Parser::~RPG_Item_Dictionary_Parser()
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Dictionary_Parser::~RPG_Item_Dictionary_Parser"));

}

void RPG_Item_Dictionary_Parser::characters(const ACEXML_Char* cdata_in,
                                            size_t start_in,
                                            size_t length_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Dictionary_Parser::characters"));

  // debug info
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("* Event characters () ** start: %u  end: %u ***************\n%s\n- End event characters () ---------------\n"),
             start_in,
             length_in,
             cdata_in));
}

void RPG_Item_Dictionary_Parser::endDocument()
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Dictionary_Parser::endDocument"));

  // debug info
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("* Event endDocument () ***************\n")));
}

void RPG_Item_Dictionary_Parser::endElement(const ACEXML_Char* uri_in,
                                            const ACEXML_Char* name_in,
                                            const ACEXML_Char* qName_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Dictionary_Parser::endElement"));

  // debug info
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("* Event endElement (%s, %s, %s) ***************\n"),
             uri_in,
             name_in,
             qName_in));
}

void RPG_Item_Dictionary_Parser::endPrefixMapping(const ACEXML_Char* prefix_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Dictionary_Parser::endPrefixMapping"));

  // debug info
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("* Event endPrefixMapping (%s) ***************\n"),
             prefix_in));
}

void RPG_Item_Dictionary_Parser::ignorableWhitespace(const ACEXML_Char* whitespace_in,
                                                     int start_in,
                                                     int length_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Dictionary_Parser::ignorableWhitespace"));

  // debug info
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("* Event ignorableWhitespace () ***************\n")));
}

void RPG_Item_Dictionary_Parser::processingInstruction(const ACEXML_Char* target_in,
                                                       const ACEXML_Char* data_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Dictionary_Parser::processingInstruction"));

  // debug info
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("* Event processingInstruction (%s, %s) ***************\n"),
             target_in,
             data_in));
}

void RPG_Item_Dictionary_Parser::setDocumentLocator(ACEXML_Locator* locator_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Dictionary_Parser::setDocumentLocator"));

  myLocator = locator_in;

  // debug info
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("* Event setDocumentLocator () ***************\n")));
}

void RPG_Item_Dictionary_Parser::skippedEntity(const ACEXML_Char* name_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Dictionary_Parser::skippedEntity"));

  // debug info
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("* Event skippedEntity (%s) ***************\n"),
             name_in));
}

void RPG_Item_Dictionary_Parser::startDocument()
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Dictionary_Parser::startDocument"));

  // debug info
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("* Event startDocument () ***************\n")));
}

void RPG_Item_Dictionary_Parser::startElement(const ACEXML_Char* uri_in,
                                              const ACEXML_Char* name_in,
                                              const ACEXML_Char* qName_in,
                                              ACEXML_Attributes* alist_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Dictionary_Parser::startElement"));

  // debug info
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("* Event startElement (%s, %s, %s) ***************\n"),
             uri_in,
             name_in,
             qName_in));

  if (alist_in)
  {
    for (size_t i = 0;
         i < alist_in->getLength();
         ++i)
    {
      ACE_DEBUG((LM_DEBUG,
                 ACE_TEXT("    %s = \"%s\"\n"),
                 alist_in->getQName(i),
                 alist_in->getValue(i)));
    } // end FOR
  } // end IF
}

void RPG_Item_Dictionary_Parser::startPrefixMapping(const ACEXML_Char* prefix_in,
                                                    const ACEXML_Char* uri_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Dictionary_Parser::startPrefixMapping"));

  // debug info
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("* Event startPrefixMapping () ***************\n")));

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("Prefix = %s, URI = %s\n"),
             prefix_in,
             uri_in));
}

// *** Methods inherit from ACEXML_DTDHandler.
void RPG_Item_Dictionary_Parser::notationDecl(const ACEXML_Char* name_in,
                                              const ACEXML_Char* publicID_in,
                                              const ACEXML_Char* systemID_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Dictionary_Parser::notationDecl"));

  // debug info
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("* Event notationDecl: (%s) "),
             name_in));

  if (publicID_in == NULL)
  {
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("SYSTEM %s\n"),
               systemID_in));
  } // end IF
  else if (systemID_in == NULL)
  {
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("PUBLIC %s\n"),
               publicID_in));
  } // end ELSE IF
  else
  {
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("PUBLIC %s %s\n"),
               publicID_in,
               systemID_in));
  } // end ELSE
}

void RPG_Item_Dictionary_Parser::unparsedEntityDecl(const ACEXML_Char* name_in,
                                                    const ACEXML_Char* publicID_in,
                                                    const ACEXML_Char* systemID_in,
                                                    const ACEXML_Char* notationName_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Dictionary_Parser::unparsedEntityDecl"));

  // debug info
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("* Unparsed Entity: %s"),
             name_in));

  if (publicID_in == NULL)
  {
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT(" SYSTEM %s"),
               systemID_in));
  } // end IF
  else
  {
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT(" PUBLIC %s %s"),
               publicID_in,
               systemID_in));
  } // end ELSE

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT(" NDATA %s\n"),
             notationName_in));
}

// Methods inherit from ACEXML_EnitityResolver.
ACEXML_InputSource* RPG_Item_Dictionary_Parser::resolveEntity(const ACEXML_Char* publicID_in,
                                                              const ACEXML_Char* systemID_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Dictionary_Parser::resolveEntity"));

  ACE_UNUSED_ARG(publicID_in);
  ACE_UNUSED_ARG(systemID_in);

  // No-op.
  return NULL;
}

// Methods inherit from ACEXML_ErrorHandler.
void RPG_Item_Dictionary_Parser::error(ACEXML_SAXParseException& exception_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Dictionary_Parser::error"));

  // debug info
  if (myLocator)
  {
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("%s: line: %d col: %d "),
               (myLocator->getSystemId() == NULL ? myFileName.c_str() : myLocator->getSystemId()),
               myLocator->getLineNumber(),
               myLocator->getColumnNumber()));
  } // end IF

  exception_in.print();
}

void RPG_Item_Dictionary_Parser::fatalError(ACEXML_SAXParseException& exception_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Dictionary_Parser::fatalError"));

  // debug info
  if (myLocator)
  {
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("%s: line: %d col: %d "),
               (myLocator->getSystemId() == NULL ? myFileName.c_str() : myLocator->getSystemId()),
               myLocator->getLineNumber(),
               myLocator->getColumnNumber()));
  } // end IF

  exception_in.print();
}

void RPG_Item_Dictionary_Parser::warning(ACEXML_SAXParseException& exception_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Item_Dictionary_Parser::warning"));

  // debug info
  if (myLocator)
  {
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("%s: line: %d col: %d "),
               (myLocator->getSystemId() == NULL ? myFileName.c_str() : myLocator->getSystemId()),
               myLocator->getLineNumber(),
               myLocator->getColumnNumber()));
  } // end IF

  exception_in.print();
}
