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
#include "rpg_graphics_XML_parser.h"

#include <rpg_common_macros.h>

#include <ace/Log_Msg.h>

RPG_Graphics_Category RPG_Graphics_Category_Type::post_RPG_Graphics_Category_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Category_Type::post_RPG_Graphics_Category_Type"));

  return RPG_Graphics_CategoryHelper::stringToRPG_Graphics_Category(post_string());
}

RPG_Graphics_TileType RPG_Graphics_TileType_Type::post_RPG_Graphics_TileType_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_TileType_Type::post_RPG_Graphics_TileType_Type"));

  return RPG_Graphics_TileTypeHelper::stringToRPG_Graphics_TileType(post_string());
}

RPG_Graphics_Orientation RPG_Graphics_Orientation_Type::post_RPG_Graphics_Orientation_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Orientation_Type::post_RPG_Graphics_Orientation_Type"));

  return RPG_Graphics_OrientationHelper::stringToRPG_Graphics_Orientation(post_string());
}

RPG_Graphics_FloorStyle RPG_Graphics_FloorStyle_Type::post_RPG_Graphics_FloorStyle_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_FloorStyle_Type::post_RPG_Graphics_FloorStyle_Type"));

  return RPG_Graphics_FloorStyleHelper::stringToRPG_Graphics_FloorStyle(post_string());
}

RPG_Graphics_StairsStyle RPG_Graphics_StairsStyle_Type::post_RPG_Graphics_StairsStyle_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_StairsStyle_Type::post_RPG_Graphics_StairsStyle_Type"));

  return RPG_Graphics_StairsStyleHelper::stringToRPG_Graphics_StairsStyle(post_string());
}

RPG_Graphics_WallStyle RPG_Graphics_WallStyle_Type::post_RPG_Graphics_WallStyle_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_WallStyle_Type::post_RPG_Graphics_WallStyle_Type"));

  return RPG_Graphics_WallStyleHelper::stringToRPG_Graphics_WallStyle(post_string());
}

RPG_Graphics_DoorStyle RPG_Graphics_DoorStyle_Type::post_RPG_Graphics_DoorStyle_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_DoorStyle_Type::post_RPG_Graphics_DoorStyle_Type"));

  return RPG_Graphics_DoorStyleHelper::stringToRPG_Graphics_DoorStyle(post_string());
}

RPG_Graphics_StyleUnion_Type::RPG_Graphics_StyleUnion_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_StyleUnion_Type::RPG_Graphics_StyleUnion_Type"));

  myCurrentStyle.discriminator = RPG_Graphics_StyleUnion::INVALID;
  myCurrentStyle.floorstyle = RPG_GRAPHICS_FLOORSTYLE_INVALID;
}

void RPG_Graphics_StyleUnion_Type::_characters(const ::xml_schema::ro_string& styleType_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_StyleUnion_Type::_characters"));

  // can be either:
  // - RPG_Graphics_FloorStyle --> "FLOORSTYLE_xxx"
  // - RPG_Graphics_StairsStyle --> "STAIRSSTYLE_xxx"
  // - RPG_Graphics_WallStyle --> "WALLSTYLE_xxx"
  // - RPG_Graphics_DoorStyle --> "DOORSTYLE_xxx"
  std::string style = styleType_in;
  if (style.find(ACE_TEXT_ALWAYS_CHAR("FLOORSTYLE_")) == 0)
  {
    myCurrentStyle.discriminator = RPG_Graphics_StyleUnion::FLOORSTYLE;
    myCurrentStyle.floorstyle = RPG_Graphics_FloorStyleHelper::stringToRPG_Graphics_FloorStyle(styleType_in);
  } // end IF
  else if (style.find(ACE_TEXT_ALWAYS_CHAR("STAIRSSTYLE_")) == 0)
  {
    myCurrentStyle.discriminator = RPG_Graphics_StyleUnion::STAIRSSTYLE;
    myCurrentStyle.stairsstyle = RPG_Graphics_StairsStyleHelper::stringToRPG_Graphics_StairsStyle(styleType_in);
  } // end IF
  else if (style.find(ACE_TEXT_ALWAYS_CHAR("WALLSTYLE_")) == 0)
  {
    myCurrentStyle.discriminator = RPG_Graphics_StyleUnion::WALLSTYLE;
    myCurrentStyle.wallstyle = RPG_Graphics_WallStyleHelper::stringToRPG_Graphics_WallStyle(styleType_in);
  } // end IF
  else
  {
    myCurrentStyle.discriminator = RPG_Graphics_StyleUnion::DOORSTYLE;
    myCurrentStyle.doorstyle = RPG_Graphics_DoorStyleHelper::stringToRPG_Graphics_DoorStyle(styleType_in);
  } // end ELSE
}

RPG_Graphics_StyleUnion RPG_Graphics_StyleUnion_Type::post_RPG_Graphics_StyleUnion_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_StyleUnion_Type::post_RPG_Graphics_StyleUnion_Type"));

  RPG_Graphics_StyleUnion result = myCurrentStyle;

  // clear structure
  myCurrentStyle.discriminator = RPG_Graphics_StyleUnion::INVALID;
  myCurrentStyle.floorstyle = RPG_GRAPHICS_FLOORSTYLE_INVALID;

  return result;
}

RPG_Graphics_Type RPG_Graphics_Type_Type::post_RPG_Graphics_Type_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Type_Type::post_RPG_Graphics_Type_Type"));

  return RPG_Graphics_TypeHelper::stringToRPG_Graphics_Type(post_string());
}

RPG_Graphics_InterfaceElementType RPG_Graphics_InterfaceElementType_Type::post_RPG_Graphics_InterfaceElementType_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_InterfaceElementType_Type::post_RPG_Graphics_InterfaceElementType_Type"));

  return RPG_Graphics_InterfaceElementTypeHelper::stringToRPG_Graphics_InterfaceElementType(post_string());
}

RPG_Graphics_HotspotType RPG_Graphics_HotspotType_Type::post_RPG_Graphics_HotspotType_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_HotspotType_Type::post_RPG_Graphics_HotspotType_Type"));

  return RPG_Graphics_HotspotTypeHelper::stringToRPG_Graphics_HotspotType(post_string());
}

RPG_Graphics_ElementTypeUnion_Type::RPG_Graphics_ElementTypeUnion_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_ElementTypeUnion_Type::RPG_Graphics_ElementTypeUnion_Type"));

  myCurrentElementType.discriminator = RPG_Graphics_ElementTypeUnion::INVALID;
  myCurrentElementType.interfaceelementtype = RPG_GRAPHICS_INTERFACEELEMENTTYPE_INVALID;
}

void RPG_Graphics_ElementTypeUnion_Type::_characters(const ::xml_schema::ro_string& elementType_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_ElementTypeUnion_Type::_characters"));

  // can be either:
  // - RPG_Graphics_InterfaceElementType --> "INTERFACEELEMENT_xxx"
  // - RPG_Graphics_HotspotType_Type --> "HOTSPOT_xxx"
  std::string element = elementType_in;
  if (element.find(ACE_TEXT_ALWAYS_CHAR("INTERFACEELEMENT_")) == 0)
  {
    myCurrentElementType.discriminator = RPG_Graphics_ElementTypeUnion::INTERFACEELEMENTTYPE;
    myCurrentElementType.interfaceelementtype = RPG_Graphics_InterfaceElementTypeHelper::stringToRPG_Graphics_InterfaceElementType(elementType_in);
  } // end IF
  else if (element.find(ACE_TEXT_ALWAYS_CHAR("HOTSPOT_")) == 0)
  {
    myCurrentElementType.discriminator = RPG_Graphics_ElementTypeUnion::HOTSPOTTYPE;
    myCurrentElementType.hotspottype = RPG_Graphics_HotspotTypeHelper::stringToRPG_Graphics_HotspotType(elementType_in);
  } // end IF
  else
  {
    ACE_DEBUG((LM_ERROR,
               ACE_TEXT("invalid RPG_Graphics_ElementType (was: \"%s\"), continuing\n"),
               element.c_str()));
  } // end ELSE
}

RPG_Graphics_ElementTypeUnion RPG_Graphics_ElementTypeUnion_Type::post_RPG_Graphics_ElementTypeUnion_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_ElementTypeUnion_Type::post_RPG_Graphics_ElementTypeUnion_Type"));

  RPG_Graphics_ElementTypeUnion result = myCurrentElementType;

  // clear structure
  myCurrentElementType.discriminator = RPG_Graphics_ElementTypeUnion::INVALID;
  myCurrentElementType.interfaceelementtype = RPG_GRAPHICS_INTERFACEELEMENTTYPE_INVALID;

  return result;
}

RPG_Graphics_Element_Type::RPG_Graphics_Element_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Element_Type::RPG_Graphics_Element_Type"));

  myCurrentElement.type.discriminator = RPG_Graphics_ElementTypeUnion::INVALID;
  myCurrentElement.type.interfaceelementtype = RPG_GRAPHICS_INTERFACEELEMENTTYPE_INVALID;
  myCurrentElement.offsetX = 0;
  myCurrentElement.offsetY = 0;
  myCurrentElement.width = 0;
  myCurrentElement.height = 0;
}

void RPG_Graphics_Element_Type::type(const RPG_Graphics_ElementTypeUnion& type_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Element_Type::type"));

  myCurrentElement.type = type_in;
}

void RPG_Graphics_Element_Type::offsetX(unsigned int offsetX_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Element_Type::offsetX"));

  myCurrentElement.offsetX = offsetX_in;
}

void RPG_Graphics_Element_Type::offsetY(unsigned int offsetY_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Element_Type::offsetY"));

  myCurrentElement.offsetY = offsetY_in;
}

void RPG_Graphics_Element_Type::width(unsigned int width_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Element_Type::width"));

  myCurrentElement.width = width_in;
}

void RPG_Graphics_Element_Type::height(unsigned int height_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Element_Type::height"));

  myCurrentElement.height = height_in;
}

RPG_Graphics_Element RPG_Graphics_Element_Type::post_RPG_Graphics_Element_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Element_Type::post_RPG_Graphics_Element_Type"));

  RPG_Graphics_Element result = myCurrentElement;

  // clear structure
  myCurrentElement.type.discriminator = RPG_Graphics_ElementTypeUnion::INVALID;
  myCurrentElement.type.interfaceelementtype = RPG_GRAPHICS_INTERFACEELEMENTTYPE_INVALID;
  myCurrentElement.offsetX = 0;
  myCurrentElement.offsetY = 0;
  myCurrentElement.width = 0;
  myCurrentElement.height = 0;

  return result;
}

RPG_Graphics_Tile_Type::RPG_Graphics_Tile_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Tile_Type::RPG_Graphics_Tile_Type"));

  myCurrentTile.type = RPG_GRAPHICS_TILETYPE_INVALID;
  myCurrentTile.reference = RPG_GRAPHICS_TYPE_INVALID;
  myCurrentTile.style.discriminator = RPG_Graphics_StyleUnion::INVALID;
  myCurrentTile.style.floorstyle = RPG_GRAPHICS_FLOORSTYLE_INVALID;
  myCurrentTile.orientation = RPG_GRAPHICS_ORIENTATION_INVALID;
  myCurrentTile.file.clear();
  myCurrentTile.offsetX = 0;
  myCurrentTile.offsetY = 0;
  myCurrentTile.open = false;
  myCurrentTile.broken = false;
}

void RPG_Graphics_Tile_Type::type(const RPG_Graphics_TileType& type_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Tile_Type::type"));

  myCurrentTile.type = type_in;
}

void RPG_Graphics_Tile_Type::reference(const RPG_Graphics_Type& reference_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Tile_Type::reference"));

  myCurrentTile.reference = reference_in;
}

void RPG_Graphics_Tile_Type::style(const RPG_Graphics_StyleUnion& style_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Tile_Type::style"));

  myCurrentTile.style = style_in;
}

void RPG_Graphics_Tile_Type::orientation(const RPG_Graphics_Orientation& orientation_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Tile_Type::orientation"));

  myCurrentTile.orientation = orientation_in;
}

void RPG_Graphics_Tile_Type::file(const ::std::string& file_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Tile_Type::file"));

  myCurrentTile.file = file_in;
}

void RPG_Graphics_Tile_Type::offsetX(long long offsetX_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Tile_Type::offsetX"));

  myCurrentTile.offsetX = offsetX_in;
}

void RPG_Graphics_Tile_Type::offsetY(long long offsetY_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Tile_Type::offsetY"));

  myCurrentTile.offsetY = offsetY_in;
}

void RPG_Graphics_Tile_Type::open(bool open_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Tile_Type::open"));

  myCurrentTile.open = open_in;
}

void RPG_Graphics_Tile_Type::broken(bool broken_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Tile_Type::broken"));

  myCurrentTile.broken = broken_in;
}

RPG_Graphics_Tile RPG_Graphics_Tile_Type::post_RPG_Graphics_Tile_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Tile_Type::post_RPG_Graphics_Tile_Type"));

  RPG_Graphics_Tile result = myCurrentTile;

  // clear structure
  myCurrentTile.type = RPG_GRAPHICS_TILETYPE_INVALID;
  myCurrentTile.reference = RPG_GRAPHICS_TYPE_INVALID;
  myCurrentTile.style.discriminator = RPG_Graphics_StyleUnion::INVALID;
  myCurrentTile.style.floorstyle = RPG_GRAPHICS_FLOORSTYLE_INVALID;
  myCurrentTile.orientation = RPG_GRAPHICS_ORIENTATION_INVALID;
  myCurrentTile.file.clear();
  myCurrentTile.offsetX = 0;
  myCurrentTile.offsetY = 0;
  myCurrentTile.open = false;
  myCurrentTile.broken = false;

  return result;
}

RPG_Graphics_TileSet_Type::RPG_Graphics_TileSet_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_TileSet_Type::RPG_Graphics_TileSet_Type"));

  myCurrentTileSet.type = RPG_GRAPHICS_TILESETTYPE_INVALID;
  myCurrentTileSet.style.discriminator = RPG_Graphics_StyleUnion::INVALID;
  myCurrentTileSet.style.floorstyle = RPG_GRAPHICS_FLOORSTYLE_INVALID;
  myCurrentTileSet.tiles.clear();
  myCurrentTileSet.half = false;
}

void RPG_Graphics_TileSet_Type::type(const RPG_Graphics_TileSetType& type_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_TileSet_Type::type"));

  myCurrentTileSet.type = type_in;
}

void RPG_Graphics_TileSet_Type::style(const RPG_Graphics_StyleUnion& style_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_TileSet_Type::style"));

  myCurrentTileSet.style = style_in;
}

void RPG_Graphics_TileSet_Type::tile(const RPG_Graphics_Tile& tile_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_TileSet_Type::tile"));

  myCurrentTileSet.tiles.push_back(tile_in);
}

void RPG_Graphics_TileSet_Type::half(bool half_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_TileSet_Type::half"));

  myCurrentTileSet.half = half_in;
}

RPG_Graphics_TileSet RPG_Graphics_TileSet_Type::post_RPG_Graphics_TileSet_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_TileSet_Type::post_RPG_Graphics_TileSet_Type"));

  RPG_Graphics_TileSet result = myCurrentTileSet;

  // clear structure
  myCurrentTileSet.type = RPG_GRAPHICS_TILESETTYPE_INVALID;
  myCurrentTileSet.style.discriminator = RPG_Graphics_StyleUnion::INVALID;
  myCurrentTileSet.style.floorstyle = RPG_GRAPHICS_FLOORSTYLE_INVALID;
  myCurrentTileSet.tiles.clear();
  myCurrentTileSet.half = false;

  return result;
}

RPG_Graphics_TileSetType RPG_Graphics_TileSetType_Type::post_RPG_Graphics_TileSetType_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_TileSetType_Type::post_RPG_Graphics_TileSetType_Type"));

  return RPG_Graphics_TileSetTypeHelper::stringToRPG_Graphics_TileSetType(post_string());
}

RPG_Graphics_Graphic_Type::RPG_Graphics_Graphic_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Graphic_Type::RPG_Graphics_Graphic_Type"));

  myCurrentGraphic.category = RPG_GRAPHICS_CATEGORY_INVALID;
  myCurrentGraphic.type = RPG_GRAPHICS_TYPE_INVALID;
  myCurrentGraphic.tile.type = RPG_GRAPHICS_TILETYPE_INVALID;
  myCurrentGraphic.tile.style.discriminator = RPG_Graphics_StyleUnion::INVALID;
  myCurrentGraphic.tile.style.floorstyle = RPG_GRAPHICS_FLOORSTYLE_INVALID;
  myCurrentGraphic.tile.orientation = RPG_GRAPHICS_ORIENTATION_INVALID;
  myCurrentGraphic.tile.file.clear();
  myCurrentGraphic.tile.offsetX = 0;
  myCurrentGraphic.tile.offsetY = 0;
  myCurrentGraphic.tileset.type = RPG_GRAPHICS_TILESETTYPE_INVALID;
  myCurrentGraphic.tileset.style.discriminator = RPG_Graphics_StyleUnion::INVALID;
  myCurrentGraphic.tileset.style.floorstyle = RPG_GRAPHICS_FLOORSTYLE_INVALID;
  myCurrentGraphic.tileset.tiles.clear();
  myCurrentGraphic.tileset.half = false;
  myCurrentGraphic.elements.clear();
  myCurrentGraphic.file.clear();
  myCurrentGraphic.size = 0;
}

void RPG_Graphics_Graphic_Type::category(const RPG_Graphics_Category& category_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Graphic_Type::category"));

  myCurrentGraphic.category = category_in;
}

void RPG_Graphics_Graphic_Type::type(const RPG_Graphics_Type& type_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Graphic_Type::type"));

  myCurrentGraphic.type = type_in;
}

void RPG_Graphics_Graphic_Type::tile(const RPG_Graphics_Tile& tile_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Graphic_Type::tile"));

  myCurrentGraphic.tile = tile_in;
}

void RPG_Graphics_Graphic_Type::tileset(const RPG_Graphics_TileSet& tileSet_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Graphic_Type::tileset"));

  myCurrentGraphic.tileset = tileSet_in;
}

void RPG_Graphics_Graphic_Type::element(const RPG_Graphics_Element& element_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Graphic_Type::element"));

  myCurrentGraphic.elements.push_back(element_in);
}

void RPG_Graphics_Graphic_Type::file(const ::std::string& file_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Graphic_Type::file"));

  myCurrentGraphic.file = file_in;
}

void RPG_Graphics_Graphic_Type::size(unsigned int size_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Graphic_Type::size"));

  myCurrentGraphic.size = size_in;
}

RPG_Graphics_Graphic RPG_Graphics_Graphic_Type::post_RPG_Graphics_Graphic_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Graphic_Type::post_RPG_Graphics_Graphic_Type"));

  RPG_Graphics_Graphic result = myCurrentGraphic;

  // clear structure
  myCurrentGraphic.category = RPG_GRAPHICS_CATEGORY_INVALID;
  myCurrentGraphic.type = RPG_GRAPHICS_TYPE_INVALID;
  myCurrentGraphic.tile.type = RPG_GRAPHICS_TILETYPE_INVALID;
  myCurrentGraphic.tile.style.discriminator = RPG_Graphics_StyleUnion::INVALID;
  myCurrentGraphic.tile.style.floorstyle = RPG_GRAPHICS_FLOORSTYLE_INVALID;
  myCurrentGraphic.tile.orientation = RPG_GRAPHICS_ORIENTATION_INVALID;
  myCurrentGraphic.tile.offsetX = 0;
  myCurrentGraphic.tile.offsetY = 0;
  myCurrentGraphic.tileset.type = RPG_GRAPHICS_TILESETTYPE_INVALID;
  myCurrentGraphic.tileset.style.discriminator = RPG_Graphics_StyleUnion::INVALID;
  myCurrentGraphic.tileset.style.floorstyle = RPG_GRAPHICS_FLOORSTYLE_INVALID;
  myCurrentGraphic.tileset.tiles.clear();
  myCurrentGraphic.tileset.half = false;
  myCurrentGraphic.elements.clear();
  myCurrentGraphic.file.clear();
  myCurrentGraphic.size = 0;

  return result;
}

RPG_Graphics_Dictionary_Type::RPG_Graphics_Dictionary_Type(RPG_Graphics_Dictionary_t* graphicsDictionary_in)
 : myDictionary(graphicsDictionary_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Dictionary_Type::RPG_Graphics_Dictionary_Type"));

}

RPG_Graphics_Dictionary_Type::~RPG_Graphics_Dictionary_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Dictionary_Type::~RPG_Graphics_Dictionary_Type"));

}

// void RPG_Graphics_Dictionary_Type::pre()
// {
//   RPG_TRACE(ACE_TEXT("RPG_Graphics_Dictionary_Type::pre"));
//
// }

void RPG_Graphics_Dictionary_Type::graphic(const RPG_Graphics_Graphic& graphic_in)
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Dictionary_Type::graphic"));

  RPG_Graphics_t graphic = graphic_in;

  myDictionary->insert(std::make_pair(graphic.type, graphic));
}

void RPG_Graphics_Dictionary_Type::post_RPG_Graphics_Dictionary_Type()
{
  RPG_TRACE(ACE_TEXT("RPG_Graphics_Dictionary_Type::post_RPG_Graphics_Dictionary_Type"));

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("finished parsing dictionary, retrieved %d graphic(s)...\n"),
             myDictionary->size()));
}
