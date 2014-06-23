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

#include "rpg_common_macros.h"
#include "rpg_common_defines.h"
#include "rpg_common_file_tools.h"
#include "rpg_common_xsderrorhandler.h"

#include <xercesc/util/XMemory.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/XMLGrammarPoolImpl.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>

#include <ace/Dirent_Selector.h>
#include <ace/Log_Msg.h>

using namespace xercesc;

// init statics
XMLGrammarPool* RPG_Common_XML_Tools::myGrammarPool = NULL;
SAX2XMLReader*  RPG_Common_XML_Tools::myParser = NULL;
bool            RPG_Common_XML_Tools::myInitialized = false;

int
RPG_Common_XML_Tools::dirent_selector(const dirent* entry_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Common_XML_Tools::dirent_selector"));

  // *NOTE*: select XML schemas
  std::string filename(entry_in->d_name);
  std::string extension(ACE_TEXT_ALWAYS_CHAR(RPG_COMMON_XML_SCHEMA_FILE_EXT));
  if (filename.rfind(extension,
                     std::string::npos) != (filename.size() - extension.size()))
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
RPG_Common_XML_Tools::dirent_comparator(const dirent** entry1_in,
                                        const dirent** entry2_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Common_XML_Tools::dirent_comparator"));

  return ACE_OS::strcmp((*entry1_in)->d_name,
                        (*entry2_in)->d_name);
}

void
RPG_Common_XML_Tools::init(const std::string& schemaDirectory_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Common_XML_Tools::init"));

  if (RPG_Common_XML_Tools::myInitialized)
    return;

  XMLPlatformUtils::Initialize();

  // sanity check(s)
  if (!RPG_Common_File_Tools::isDirectory(schemaDirectory_in))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid argument, not a directory (was: \"%s\"), aborting\n"),
               ACE_TEXT(schemaDirectory_in.c_str())));

    return;
  } // end IF

  // retrieve all relevant schema files...
  ACE_Dirent_Selector entries;
  if (entries.open(ACE_TEXT(schemaDirectory_in.c_str()),
                   &RPG_Common_XML_Tools::dirent_selector,
                   &RPG_Common_XML_Tools::dirent_comparator) == -1)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to ACE_Dirent_Selector::open(\"%s\"): \"%m\", aborting\n"),
               ACE_TEXT(schemaDirectory_in.c_str())));

    return;
  } // end IF

  // *TODO*
//  ACE_NEW_NORETURN(myGrammarPool,
//                   XMLGrammarPoolImpl());
  try
  {
    myGrammarPool =
        static_cast<XMLGrammarPool*>(new XMLGrammarPoolImpl(XMLPlatformUtils::fgMemoryManager));
  }
  catch (...)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to allocate memory: \"%m\", aborting\n")));

    // clean up
    entries.close();

    return;
  } // end IF
  if (!myGrammarPool)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to allocate memory: \"%m\", aborting\n")));

    // clean up
    entries.close();

    return;
  } // end IF

  myParser = XMLReaderFactory::createXMLReader(XMLPlatformUtils::fgMemoryManager,
                                               myGrammarPool);
  if (!myParser)
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("failed to allocate memory: \"%m\", aborting\n")));

    // clean up
    entries.close();

    return;
  } // end IF
  myParser->setErrorHandler(&RPG_XercesErrorHandler);
  // Commonly useful configuration.
  myParser->setFeature(XMLUni::fgSAX2CoreNameSpaces, true);
  myParser->setFeature(XMLUni::fgSAX2CoreNameSpacePrefixes, true);
  myParser->setFeature(XMLUni::fgSAX2CoreValidation, true);
  // Enable validation.
  myParser->setFeature(XMLUni::fgXercesSchema, true);
  myParser->setFeature(XMLUni::fgXercesSchemaFullChecking, true);
  myParser->setFeature(XMLUni::fgXercesValidationErrorAsFatal, true);
  // Use the loaded grammar during parsing.
  myParser->setFeature(XMLUni::fgXercesUseCachedGrammarInParse, true);
  // Don't load schemas from any other source (e.g., from XML document's
  // xsi:schemaLocation attributes).
  myParser->setFeature(XMLUni::fgXercesLoadSchema, false);
  // Xerces-C++ 3.1.0 is the first version with working multi import
  // support.
#if _XERCES_VERSION >= 30100
  myParser->setFeature(XMLUni::fgXercesHandleMultipleImports, true);
#endif

  std::string path;
  for (unsigned int i = 0;
       i < static_cast<unsigned int>(entries.length());
       i++)
  {
    path = schemaDirectory_in;
    path += ACE_DIRECTORY_SEPARATOR_STR_A;
    path += ACE_TEXT_ALWAYS_CHAR(entries[i]->d_name);
    if (!myParser->loadGrammar(path.c_str(),
                               Grammar::SchemaGrammarType,
                               true) ||
        RPG_XercesErrorHandler.failed())
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT("failed to loadGrammar(\"%s\"), aborting\n"),
                 ACE_TEXT(path.c_str())));

      break;
    } // end IF

    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("loaded \"%s\"\n"),
               entries[i]->d_name));
  } // end FOR

  myGrammarPool->lockPool();

  // clean up
  entries.close();

  myInitialized = true;
}

void
RPG_Common_XML_Tools::fini()
{
  RPG_TRACE(ACE_TEXT("RPG_Common_XML_Tools::fini"));

  // clean up
//  delete(myGrammarPool,
//         XMLPlatformUtils::fgMemoryManager);
  delete myGrammarPool;
  //delete(myParser,
  //                  XMLPlatformUtils::fgMemoryManager);
  delete myParser;

  XMLPlatformUtils::Terminate();
}

SAX2XMLReader*
RPG_Common_XML_Tools::parser()
{
  RPG_TRACE(ACE_TEXT("RPG_Common_XML_Tools::parser"));

  ACE_ASSERT(myInitialized &&
             myParser);

  return myParser;
}
