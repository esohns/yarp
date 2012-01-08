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

#ifndef RPG_ITEM_DICTIONARY_PARSER_H
#define RPG_ITEM_DICTIONARY_PARSER_H

#include "rpg_item_common.h"

#include <ace/Global_Macros.h>
#include <ACEXML/common/DefaultHandler.h>

#include <string>

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Item_Dictionary_Parser
 : public ACEXML_DefaultHandler
{
 public:
  RPG_Item_Dictionary_Parser(const std::string&,           // dictionary filename
                             RPG_Item_WeaponDictionary_t*, // weapon dictionary
                             RPG_Item_ArmorDictionary_t*); // armor dictionary
  virtual ~RPG_Item_Dictionary_Parser();

  virtual void characters(const ACEXML_Char*, // ch
                          size_t,             // start
                          size_t);            // length

//   virtual void startDocument();
//   virtual void endDocument();
  virtual void startElement(const ACEXML_Char*,  // namespaceURI
                            const ACEXML_Char*,  // localName
                            const ACEXML_Char*,  // qName
                            ACEXML_Attributes*); // atts
  virtual void endElement(const ACEXML_Char*,  // namespaceURI
                          const ACEXML_Char*,  // localName
                          const ACEXML_Char*); // qName
//   virtual void startPrefixMapping(const ACEXML_Char*,  // prefix
//                                   const ACEXML_Char*); // uri
//   virtual void endPrefixMapping(const ACEXML_Char*); // prefix

  virtual void ignorableWhitespace(const ACEXML_Char*, // ch
                                   int,                // start
                                   int);               // length
  virtual void processingInstruction(const ACEXML_Char*,  // target
                                     const ACEXML_Char*); // data
  virtual void setDocumentLocator(ACEXML_Locator*); // locator
  virtual void skippedEntity(const ACEXML_Char*); // name

  // *** Methods inherit from ACEXML_DTDHandler.
  virtual void notationDecl(const ACEXML_Char*,  // name
                            const ACEXML_Char*,  // publicId
                            const ACEXML_Char*); // systemId
  virtual void unparsedEntityDecl(const ACEXML_Char*,  // name
                                  const ACEXML_Char*,  // publicId
                                  const ACEXML_Char*,  // systemId
                                  const ACEXML_Char*); // notationName

  // Methods inherit from ACEXML_EnitityResolver.
  virtual ACEXML_InputSource* resolveEntity(const ACEXML_Char*,  // publicId
                                            const ACEXML_Char*); // systemId

  // Methods inherit from ACEXML_ErrorHandler.
  virtual void error(ACEXML_SAXParseException&); // exception
  virtual void fatalError(ACEXML_SAXParseException&); // exception
  virtual void warning(ACEXML_SAXParseException&); // exception

 private:
  typedef ACEXML_DefaultHandler inherited;

  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Item_Dictionary_Parser());
  ACE_UNIMPLEMENTED_FUNC(RPG_Item_Dictionary_Parser(const RPG_Item_Dictionary_Parser&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Item_Dictionary_Parser& operator=(const RPG_Item_Dictionary_Parser&));

  void resetCurrentItems();

  std::string                  myFileName;
  RPG_Item_WeaponDictionary_t* myWeaponDictionary;
  RPG_Item_Weapon_Properties   myCurrentWeapon;
  RPG_Item_ArmorDictionary_t*  myArmorDictionary;
  RPG_Item_Armor_Properties    myCurrentArmor;

  ACEXML_Locator*              myLocator;
};

#endif
