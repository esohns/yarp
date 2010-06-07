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
#include "rpg_graphics_common_tools.h"

#include "rpg_graphics_dictionary.h"

#include <rpg_common_file_tools.h>

#include <ace/Log_Msg.h>

// init statics
RPG_Graphics_CategoryToStringTable_t RPG_Graphics_CategoryHelper::myRPG_Graphics_CategoryToStringTable;
RPG_Graphics_TypeToStringTable_t RPG_Graphics_TypeHelper::myRPG_Graphics_TypeToStringTable;

std::string                  RPG_Graphics_Common_Tools::myGraphicsDirectory;
ACE_Thread_Mutex             RPG_Graphics_Common_Tools::myLock;
unsigned long                RPG_Graphics_Common_Tools::myOldestCacheEntry = 0;
unsigned long                RPG_Graphics_Common_Tools::myCacheSize = 0;
RPG_Graphics_GraphicsCache_t RPG_Graphics_Common_Tools::myGraphicsCache;
bool                         RPG_Graphics_Common_Tools::myInitialized = false;

void
RPG_Graphics_Common_Tools::init(const std::string& directory_in,
                                const unsigned long& cacheSize_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Common_Tools::init"));

  // sanity check(s)
  if (!RPG_Common_File_Tools::isDirectory(directory_in))
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid argument \"%s\": not a directory, aborting\n"),
               directory_in.c_str()));

    return;
  } // end IF
  myGraphicsDirectory = directory_in;
  myCacheSize = cacheSize_in;

  if (myInitialized)
  {
    // synch access to sound cache
    {
      ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

      // clear the sound cache
      for (RPG_Graphics_GraphicsCacheIterator_t iter = myGraphicsCache.begin();
           iter != myGraphicsCache.end();
           iter++)
      {

      } // end FOR
      myGraphicsCache.clear();
      myOldestCacheEntry = 0;
    } // end lock scope
  } // end IF
  else
  {
    // init string conversion
    initStringConversionTables();
  } // end ELSE

  myInitialized = true;
}

void
RPG_Graphics_Common_Tools::loadGraphic(const RPG_Graphics_Type& type_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Common_Tools::loadGraphic"));

  // step1: sound already cached ?
  RPG_Graphics_GraphicsCacheNode_t node;
  node.type = type_in;

  // synch access to graphics cache
  {
    ACE_Guard<ACE_Thread_Mutex> aGuard(myLock);

    RPG_Graphics_GraphicsCacheIterator_t iter = myGraphicsCache.begin();
    for (;
         iter != myGraphicsCache.end();
         iter++)
      if ((*iter) == node)
        break;
    if (iter == myGraphicsCache.end())
    {
      RPG_Graphics_t graphic;
      graphic.category = RPG_GRAPHICS_CATEGORY_INVALID;
      graphic.type = RPG_GRAPHICS_TYPE_INVALID;
      graphic.file.clear();

      // retrieve event properties from the dictionary
      graphic = RPG_GRAPHICS_DICTIONARY_SINGLETON::instance()->getGraphic(type_in);
      ACE_ASSERT(graphic.type == type_in);
      // load the file
      std::string path = myGraphicsDirectory;
      path += ACE_DIRECTORY_SEPARATOR_STR;
      path += graphic.file;
      // sanity check
      if (!RPG_Common_File_Tools::isReadable(path))
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT("invalid argument(\"%s\"): not readable, aborting\n"),
                   path.c_str()));

        return;
      } // end IF

      // add the chunk to our cache
      if (myGraphicsCache.size() == myCacheSize)
      {
        iter = myGraphicsCache.begin();
        std::advance(iter, myOldestCacheEntry);

        myGraphicsCache.erase(iter);
        myOldestCacheEntry++;
        if (myOldestCacheEntry == myCacheSize)
          myOldestCacheEntry = 0;
      } // end IF
//       node.chunk = chunk;
      myGraphicsCache.push_back(node);
      iter = myGraphicsCache.end(); iter--;
    } // end IF
  } // end lock scope
}

void
RPG_Graphics_Common_Tools::initStringConversionTables()
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Common_Tools::initStringConversionTables"));

  RPG_Graphics_CategoryHelper::init();
  RPG_Graphics_TypeHelper::init();

  // debug info
  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("RPG_Graphics_Common_Tools: initialized string conversion tables...\n")));
}
