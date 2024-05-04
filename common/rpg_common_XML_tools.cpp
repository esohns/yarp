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

#include "rpg_common_XML_tools.h"

#include <algorithm>
#include <vector>

#include "xercesc/framework/XMLGrammarPoolImpl.hpp"
#include "xercesc/sax2/XMLReaderFactory.hpp"
#include "xercesc/util/XMemory.hpp"
#include "xercesc/util/XMLUni.hpp"
#include "xercesc/util/PlatformUtils.hpp"

#include "ace/Dirent_Selector.h"
#include "ace/Log_Msg.h"
#include "ace/OS_Memory.h"

#include "common_file_tools.h"
#include "common_macros.h"

#include "rpg_common_defines.h"
#include "rpg_common_macros.h"
#include "rpg_common_xsderrorhandler.h"

using namespace xercesc;

// initialize statics
XMLGrammarPool* RPG_Common_XML_Tools::grammarPool_ = NULL;
SAX2XMLReader*  RPG_Common_XML_Tools::parser_ = NULL;
bool            RPG_Common_XML_Tools::initialized_ = false;

int
RPG_Common_XML_Tools::dirent_selector (const dirent* entry_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Common_XML_Tools::dirent_selector"));

  // *NOTE*: select XML schemas
  std::string filename (entry_in->d_name);
  std::string extension (ACE_TEXT_ALWAYS_CHAR (RPG_COMMON_XML_SCHEMA_FILE_EXT));
  if (filename.rfind (extension,
                      std::string::npos) != (filename.size () - extension.size ()))
//                     -1) != (filename.size() - extension.size()))
  {
//     ACE_DEBUG((LM_DEBUG,
//                ACE_TEXT("ignoring \"%s\"...\n"),
//                entry_in->d_name));
    return 0;
  } // end IF

  return 1;
}

int
RPG_Common_XML_Tools::dirent_comparator (const dirent** entry1_in,
                                         const dirent** entry2_in)
{
  //RPG_TRACE (ACE_TEXT ("RPG_Common_XML_Tools::dirent_comparator"));

  static std::vector<std::string> rpg_common_xml_list_of_files_a = {
    ACE_TEXT_ALWAYS_CHAR ("rpg_dice.xsd"),
    ACE_TEXT_ALWAYS_CHAR ("rpg_common.xsd"),
    ACE_TEXT_ALWAYS_CHAR ("rpg_common_environment.xsd"),
    ACE_TEXT_ALWAYS_CHAR ("rpg_magic.xsd"),
    ACE_TEXT_ALWAYS_CHAR ("rpg_item.xsd"),
    ACE_TEXT_ALWAYS_CHAR ("rpg_character.xsd"),
    ACE_TEXT_ALWAYS_CHAR ("rpg_combat.xsd"),
    ACE_TEXT_ALWAYS_CHAR ("rpg_player.xsd"),
    ACE_TEXT_ALWAYS_CHAR ("rpg_monster.xsd"),
    ACE_TEXT_ALWAYS_CHAR ("rpg_map.xsd"),
    ACE_TEXT_ALWAYS_CHAR ("rpg_graphics.xsd"),
    ACE_TEXT_ALWAYS_CHAR ("rpg_sound.xsd"),
    ACE_TEXT_ALWAYS_CHAR ("rpg_engine.xsd")
  };

  std::vector<std::string>::const_iterator iterator_1 =
    std::find (rpg_common_xml_list_of_files_a.begin (), rpg_common_xml_list_of_files_a.end (),
               ACE_TEXT_ALWAYS_CHAR ((*entry1_in)->d_name));
  std::vector<std::string>::const_iterator iterator_2 =
    std::find (rpg_common_xml_list_of_files_a.begin (), rpg_common_xml_list_of_files_a.end (),
               ACE_TEXT_ALWAYS_CHAR ((*entry2_in)->d_name));
  if ((iterator_1 == rpg_common_xml_list_of_files_a.end ()) || (iterator_2 == rpg_common_xml_list_of_files_a.end ()))
    goto fallback;

  return ((iterator_1 < iterator_2) ? -1 : 1);

fallback:
  return ACE_OS::strcmp (ACE_TEXT_ALWAYS_CHAR ((*entry1_in)->d_name),
                         ACE_TEXT_ALWAYS_CHAR ((*entry2_in)->d_name));
}

bool
RPG_Common_XML_Tools::initialize (const std::vector<std::string>& schemaDirectories_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Common_XML_Tools::initialize"));

  int result = -1;

  if (!RPG_Common_XML_Tools::initialized_)
    XMLPlatformUtils::Initialize ();

  if (!RPG_Common_XML_Tools::grammarPool_)
  {
    // *TODO*
    //ACE_NEW_NORETURN (grammarPool_,
    //                  XMLGrammarPoolImpl (xercesc_3_1::XMLPlatformUtils::fgMemoryManager));
    try {
      RPG_Common_XML_Tools::grammarPool_ =
        static_cast<XMLGrammarPool*> (new XMLGrammarPoolImpl (XMLPlatformUtils::fgMemoryManager));
    } catch (...) {
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("failed to allocate memory: \"%m\", aborting\n")));
      return false;
    } // end IF
    if (unlikely (!RPG_Common_XML_Tools::grammarPool_))
    {
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("failed to allocate memory: \"%m\", aborting\n")));
      return false;
    } // end IF
  } // end IF
  ACE_ASSERT (RPG_Common_XML_Tools::grammarPool_);
  RPG_Common_XML_Tools::grammarPool_->unlockPool ();

  if (!RPG_Common_XML_Tools::parser_)
  {
    RPG_Common_XML_Tools::parser_ =
      XMLReaderFactory::createXMLReader (XMLPlatformUtils::fgMemoryManager,
                                         RPG_Common_XML_Tools::grammarPool_);
    if (unlikely (!RPG_Common_XML_Tools::parser_))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to XMLReaderFactory::createXMLReader: \"%m\", aborting\n")));
      return false;
    } // end IF
    RPG_Common_XML_Tools::parser_->setErrorHandler (&RPG_XercesErrorHandler);
    // Commonly useful configuration.
    RPG_Common_XML_Tools::parser_->setFeature (XMLUni::fgSAX2CoreNameSpaces, true);
    RPG_Common_XML_Tools::parser_->setFeature (XMLUni::fgSAX2CoreNameSpacePrefixes, true);
    RPG_Common_XML_Tools::parser_->setFeature (XMLUni::fgSAX2CoreValidation, true);
    // Enable validation.
    RPG_Common_XML_Tools::parser_->setFeature (XMLUni::fgXercesSchema, true);
    RPG_Common_XML_Tools::parser_->setFeature (XMLUni::fgXercesSchemaFullChecking, true);
    RPG_Common_XML_Tools::parser_->setFeature (XMLUni::fgXercesContinueAfterFatalError, true);
    RPG_Common_XML_Tools::parser_->setFeature (XMLUni::fgXercesValidationErrorAsFatal, true);
    // Use the loaded grammar during parsing.
    RPG_Common_XML_Tools::parser_->setFeature (XMLUni::fgXercesUseCachedGrammarInParse, true);
    // Don't load schemas from any other source (e.g., from XML document's
    // xsi:schemaLocation attributes).
    RPG_Common_XML_Tools::parser_->setFeature (XMLUni::fgXercesLoadSchema, false);
    // Xerces-C++ 3.1.0 is the first version with working multi import
    // support.
#if (_XERCES_VERSION >= 30100)
    RPG_Common_XML_Tools::parser_->setFeature (XMLUni::fgXercesHandleMultipleImports, true);
#endif // _XERCES_VERSION >= 30100
  } // end IF
  ACE_ASSERT (RPG_Common_XML_Tools::parser_);

  // load all required XML schema files
  for (std::vector<std::string>::const_iterator iterator = schemaDirectories_in.begin ();
       iterator != schemaDirectories_in.end ();
       ++iterator)
  { // sanity check(s)
    ACE_ASSERT (Common_File_Tools::isDirectory (*iterator));

    std::string path_string;
    ACE_Dirent_Selector entries;
    result = entries.open (ACE_TEXT ((*iterator).c_str ()),
                           &RPG_Common_XML_Tools::dirent_selector,
                           &RPG_Common_XML_Tools::dirent_comparator);
    if (unlikely (result == -1))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Dirent_Selector::open(\"%s\"): \"%m\", aborting\n"),
                  ACE_TEXT ((*iterator).c_str ())));
      return false;
    } // end IF
    for (unsigned int i = 0;
         i < static_cast<unsigned int> (entries.length ());
         i++)
    {
      path_string = *iterator;
      path_string += ACE_DIRECTORY_SEPARATOR_STR_A;
      path_string += ACE_TEXT_ALWAYS_CHAR (entries[i]->d_name);
      RPG_XercesErrorHandler.resetErrors ();
      if (unlikely (!RPG_Common_XML_Tools::parser_->loadGrammar (path_string.c_str (),
                                                                 Grammar::SchemaGrammarType,
                                                                 true) || // cache grammar
                    RPG_XercesErrorHandler.failed ()))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to SAX2XMLReader::loadGrammar(\"%s\"), aborting\n"),
                    ACE_TEXT (path_string.c_str ())));
        result = entries.close ();
        if (result == -1)
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to ACE_Dirent_Selector::close(): \"%m\", continuing\n")));
        return false;
      } // end IF
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("loaded XML schema \"%s\"\n"),
                  ACE_TEXT (entries[i]->d_name)));
    } // end FOR
    RPG_Common_XML_Tools::grammarPool_->lockPool ();
    result = entries.close ();
    if (unlikely (result == -1))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Dirent_Selector::close(): \"%m\", continuing\n")));
  } // end FOR

  RPG_Common_XML_Tools::initialized_ = true;

  return true;
}

void
RPG_Common_XML_Tools::finalize ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Common_XML_Tools::finalize"));

  // clean up
//  delete(myGrammarPool,
//         XMLPlatformUtils::fgMemoryManager);
  delete RPG_Common_XML_Tools::grammarPool_; RPG_Common_XML_Tools::grammarPool_ = NULL;
  //delete(myParser,
  //                  XMLPlatformUtils::fgMemoryManager);
  delete RPG_Common_XML_Tools::parser_; RPG_Common_XML_Tools::parser_ = NULL;

  XMLPlatformUtils::Terminate ();

  RPG_Common_XML_Tools::initialized_ = false;
}

SAX2XMLReader*
RPG_Common_XML_Tools::parser ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Common_XML_Tools::parser"));

  // sanity check(s)
  ACE_ASSERT (RPG_Common_XML_Tools::initialized_ && RPG_Common_XML_Tools::parser_);

  return RPG_Common_XML_Tools::parser_;
}
