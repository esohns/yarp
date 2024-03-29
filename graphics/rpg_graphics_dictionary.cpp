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

#include "rpg_graphics_dictionary.h"

#include <string>
#include <sstream>

#include "ace/Log_Msg.h"

#include "common_file_tools.h"

#include "rpg_common_defines.h"
#include "rpg_common_macros.h"
#include "rpg_common_tools.h"
#include "rpg_common_XML_tools.h"
#include "rpg_common_xsderrorhandler.h"

#include "rpg_graphics_common_tools.h"
#include "rpg_graphics_defines.h"
#include "rpg_graphics_XML_parser.h"

RPG_Graphics_Dictionary::RPG_Graphics_Dictionary ()
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Dictionary::RPG_Graphics_Dictionary"));

}

bool
RPG_Graphics_Dictionary::initialize (const std::string& filename_in,
                                     bool validateXML_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Dictionary::initialize"));

  // Construct the parser.
  //
  RPG_Graphics_Category_Type                  category_p;
  RPG_Graphics_GraphicTypeUnion_Type          type_p;
  RPG_Graphics_Tile_Type                      tile_p;
  RPG_Graphics_TileType_Type                  tileType_p;
  RPG_Graphics_StyleUnion_Type                style_p;
  RPG_Graphics_Orientation_Type               orientation_p;
  ::xml_schema::string_pimpl                  string_p;
  ::xml_schema::int_pimpl                     int_p;
  ::xml_schema::boolean_pimpl                 bool_p;
  tile_p.parsers (tileType_p,
                  type_p,
                  style_p,
                  orientation_p,
                  string_p,
                  int_p,
                  int_p,
                  bool_p,
                  bool_p);
  RPG_Graphics_TileSetType_Type               tileSetType_p;
  RPG_Graphics_TileSet_Type                   tileSet_p;
  tileSet_p.parsers (tileSetType_p,
                     style_p,
                     tile_p,
                     bool_p);
  RPG_Graphics_ElementTypeUnion_Type          elementType_p;
  ::xml_schema::unsigned_int_pimpl            unsigned_int_p;
  RPG_Graphics_Element_Type                   element_p;
  RPG_Graphics_WindowType_Type                window_p;
  element_p.parsers (elementType_p,
                     window_p,
                     unsigned_int_p,
                     unsigned_int_p,
                     unsigned_int_p,
                     unsigned_int_p);
  RPG_Graphics_Graphic_Type                   graphic_p;
  graphic_p.parsers (category_p,
                     type_p,
                     tile_p,
                     tileSet_p,
                     element_p,
                     string_p,
                     unsigned_int_p);

  RPG_Graphics_Dictionary_Type                dictionary_p(&myDictionary);
  dictionary_p.parsers (graphic_p);

  // Parse the document to obtain the object model.
  //
  ::xml_schema::document doc_p (dictionary_p,                                            // parser
                                ACE_TEXT_ALWAYS_CHAR (RPG_COMMON_XML_TARGET_NAMESPACE),  // namespace
                                ACE_TEXT_ALWAYS_CHAR (RPG_GRAPHICS_DICTIONARY_INSTANCE), // root element name
                                false);                                                  // polymorphic ?

  dictionary_p.pre ();

  // OK: parse the file...
  ::xml_schema::flags flags;
  if (!validateXML_in)
    flags = flags | ::xml_schema::flags::dont_validate;
  ::xml_schema::properties properties;
  //std::string path = Common_File_Tools::directory (filename_in);
  //path += ACE_DIRECTORY_SEPARATOR_STR;
  //path += ACE_TEXT_ALWAYS_CHAR (RPG_GRAPHICS_SCHEMA_FILE);
  //path = RPG_Common_Tools::sanitizeURI (path);
  //path.insert (0, ACE_TEXT_ALWAYS_CHAR ("file:///"));
  //properties.schema_location (ACE_TEXT_ALWAYS_CHAR (RPG_COMMON_XML_TARGET_NAMESPACE),
  //                            path);
  try {
    //doc_p.parse(filename_in,
    //            RPG_XSDErrorHandler,
    //            flags);
    doc_p.parse (filename_in,
                 *RPG_Common_XML_Tools::parser (),
                 flags,
                 properties);
  } catch (const ::xml_schema::parsing& exception) {
    std::ostringstream converter;
    converter << exception;
    std::string text = converter.str();
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("RPG_Graphics_Dictionary::init(): exception occurred: \"%s\", aborting\n"),
               ACE_TEXT(text.c_str())));
    return false;
  } catch (...) {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("RPG_Graphics_Dictionary::init(): exception occurred, aborting\n")));
    return false;
  }

  dictionary_p.post_RPG_Graphics_Dictionary_Type ();

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("finished parsing graphics dictionary file \"%s\"...\n"),
              ACE_TEXT (filename_in.c_str())));

  return true;
}

const RPG_Graphics_t
RPG_Graphics_Dictionary::get (const struct RPG_Graphics_GraphicTypeUnion& type_in) const
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Dictionary::get"));

  switch (type_in.discriminator)
  {
    case RPG_Graphics_GraphicTypeUnion::CURSOR:
    {
      RPG_Graphics_CursorDictionaryIterator_t iterator = myDictionary.cursors.find (type_in.cursor);
      if (iterator != myDictionary.cursors.end ())
        return (*iterator).second;
      break;
    }
    case RPG_Graphics_GraphicTypeUnion::FONT:
    {
      RPG_Graphics_FontDictionaryIterator_t iterator = myDictionary.fonts.find (type_in.font);
      if (iterator != myDictionary.fonts.end ())
        return (*iterator).second;
      break;
    }
    case RPG_Graphics_GraphicTypeUnion::IMAGE:
    {
      RPG_Graphics_ImageDictionaryIterator_t iterator = myDictionary.images.find (type_in.image);
      if (iterator != myDictionary.images.end ())
        return (*iterator).second;
      break;
    }
    case RPG_Graphics_GraphicTypeUnion::SPRITE:
    {
      RPG_Graphics_SpriteDictionaryIterator_t iterator = myDictionary.sprites.find (type_in.sprite);
      if (iterator != myDictionary.sprites.end ())
        return (*iterator).second;
      break;
    }
    case RPG_Graphics_GraphicTypeUnion::TILEGRAPHIC:
    {
      RPG_Graphics_TileDictionaryIterator_t iterator = myDictionary.tiles.find (type_in.tilegraphic);
      if (iterator != myDictionary.tiles.end ())
        return (*iterator).second;
      break;
    }
    case RPG_Graphics_GraphicTypeUnion::TILESETGRAPHIC:
    {
      RPG_Graphics_TileSetDictionaryIterator_t iterator = myDictionary.tilesets.find (type_in.tilesetgraphic);
      if (iterator != myDictionary.tilesets.end ())
        return (*iterator).second;
      break;
    }
    default:
    {
      ACE_DEBUG((LM_ERROR,
                 ACE_TEXT ("invalid RPG_Graphics_GraphicTypeUnion type (was: %d), aborting\n"),
                 type_in.discriminator));
      break;
    }
  } // end SWITCH

  ACE_DEBUG ((LM_ERROR,
              ACE_TEXT ("type \"%s\" not found, aborting\n"),
              ACE_TEXT (RPG_Graphics_Common_Tools::toString (type_in).c_str ())));

  ACE_ASSERT (false);
  RPG_Graphics_t dummy;
  dummy.category = RPG_GRAPHICS_CATEGORY_INVALID;
  dummy.type.discriminator = RPG_Graphics_GraphicTypeUnion::INVALID;
#if defined (_MSC_VER)
  return dummy;
#else
  ACE_NOTREACHED (return dummy;)
#endif
}

RPG_Graphics_Fonts_t
RPG_Graphics_Dictionary::getFonts () const
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Dictionary::getFonts"));

  RPG_Graphics_Fonts_t result;

  RPG_Graphics_Font_t font;
  font.type = RPG_GRAPHICS_FONT_INVALID;
  font.size = 0;
  font.file.clear ();
  for (RPG_Graphics_FontDictionaryIterator_t iterator = myDictionary.fonts.begin ();
       iterator != myDictionary.fonts.end ();
       iterator++)
  { ACE_ASSERT ((*iterator).second.type.discriminator == RPG_Graphics_GraphicTypeUnion::FONT);
    font.type = (*iterator).second.type.font;
    font.size = (*iterator).second.size;
    font.file = (*iterator).second.file;
    result.push_back (font);
  } // end FOR

  return result;
}

void
RPG_Graphics_Dictionary::dump () const
{
  RPG_TRACE (ACE_TEXT ("RPG_Graphics_Dictionary::dump"));

  unsigned long index = 0;
  for (RPG_Graphics_CursorDictionaryIterator_t iterator = myDictionary.cursors.begin ();
       iterator != myDictionary.cursors.end ();
       iterator++, index++)
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("Graphic[#%u]:\n%s===========================\n"),
                index,
                ACE_TEXT (RPG_Graphics_Common_Tools::toString ((*iterator).second).c_str ())));
  for (RPG_Graphics_FontDictionaryIterator_t iterator = myDictionary.fonts.begin ();
       iterator != myDictionary.fonts.end ();
       iterator++, index++)
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("Graphic[#%u]:\n%s===========================\n"),
                index,
                ACE_TEXT (RPG_Graphics_Common_Tools::toString ((*iterator).second).c_str ())));
  for (RPG_Graphics_ImageDictionaryIterator_t iterator = myDictionary.images.begin ();
       iterator != myDictionary.images.end ();
       iterator++, index++)
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("Graphic[#%u]:\n%s===========================\n"),
                index,
                ACE_TEXT (RPG_Graphics_Common_Tools::toString ((*iterator).second).c_str ())));
  for (RPG_Graphics_TileDictionaryIterator_t iterator = myDictionary.tiles.begin ();
       iterator != myDictionary.tiles.end ();
       iterator++, index++)
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("Graphic[#%u]:\n%s===========================\n"),
                index,
                ACE_TEXT (RPG_Graphics_Common_Tools::toString ((*iterator).second).c_str ())));
  for (RPG_Graphics_TileSetDictionaryIterator_t iterator = myDictionary.tilesets.begin ();
       iterator != myDictionary.tilesets.end ();
       iterator++, index++)
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("Graphic[#%u]:\n%s===========================\n"),
                index,
                ACE_TEXT (RPG_Graphics_Common_Tools::toString ((*iterator).second).c_str ())));
}
