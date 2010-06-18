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

#include <ace/Log_Msg.h>

RPG_Graphics_Category RPG_Graphics_Category_Type::post_RPG_Graphics_Category_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Category_Type::post_RPG_Graphics_Category_Type"));

  return RPG_Graphics_CategoryHelper::stringToRPG_Graphics_Category(post_string());
}

RPG_Graphics_TileOrientation RPG_Graphics_TileOrientation_Type::post_RPG_Graphics_TileOrientation_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_TileOrientation_Type::post_RPG_Graphics_TileOrientation_Type"));

  return RPG_Graphics_TileOrientationHelper::stringToRPG_Graphics_TileOrientation(post_string());
}

RPG_Graphics_FloorStyle RPG_Graphics_FloorStyle_Type::post_RPG_Graphics_FloorStyle_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_FloorStyle_Type::post_RPG_Graphics_FloorStyle_Type"));

  return RPG_Graphics_FloorStyleHelper::stringToRPG_Graphics_FloorStyle(post_string());
}

RPG_Graphics_StairsStyle RPG_Graphics_StairsStyle_Type::post_RPG_Graphics_StairsStyle_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_StairsStyle_Type::post_RPG_Graphics_StairsStyle_Type"));

  return RPG_Graphics_StairsStyleHelper::stringToRPG_Graphics_StairsStyle(post_string());
}

RPG_Graphics_WallStyle RPG_Graphics_WallStyle_Type::post_RPG_Graphics_WallStyle_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_WallStyle_Type::post_RPG_Graphics_WallStyle_Type"));

  return RPG_Graphics_WallStyleHelper::stringToRPG_Graphics_WallStyle(post_string());
}

RPG_Graphics_DoorStyle RPG_Graphics_DoorStyle_Type::post_RPG_Graphics_DoorStyle_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_DoorStyle_Type::post_RPG_Graphics_DoorStyle_Type"));

  return RPG_Graphics_DoorStyleHelper::stringToRPG_Graphics_DoorStyle(post_string());
}

RPG_Graphics_StyleUnion_Type::RPG_Graphics_StyleUnion_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_StyleUnion_Type::RPG_Graphics_StyleUnion_Type"));

  myCurrentStyle.discriminator = RPG_Graphics_StyleUnion::INVALID;
  myCurrentStyle.floorstyle = RPG_GRAPHICS_FLOORSTYLE_INVALID;
}

void RPG_Graphics_StyleUnion_Type::_characters(const ::xml_schema::ro_string& styleType_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_StyleUnion_Type::_characters"));

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
  ACE_TRACE(ACE_TEXT("RPG_Graphics_StyleUnion_Type::post_RPG_Graphics_StyleUnion_Type"));

  RPG_Graphics_StyleUnion result = myCurrentStyle;

  // clear structure
  myCurrentStyle.discriminator = RPG_Graphics_StyleUnion::INVALID;
  myCurrentStyle.floorstyle = RPG_GRAPHICS_FLOORSTYLE_INVALID;

  return result;
}

RPG_Graphics_Type RPG_Graphics_Type_Type::post_RPG_Graphics_Type_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Type_Type::post_RPG_Graphics_Type_Type"));

  return RPG_Graphics_TypeHelper::stringToRPG_Graphics_Type(post_string());
}

RPG_Graphics_InterfaceElementType RPG_Graphics_InterfaceElementType_Type::post_RPG_Graphics_InterfaceElementType_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_InterfaceElementType_Type::post_RPG_Graphics_InterfaceElementType_Type"));

  return RPG_Graphics_InterfaceElementTypeHelper::stringToRPG_Graphics_InterfaceElementType(post_string());
}

RPG_Graphics_HotspotType RPG_Graphics_HotspotType_Type::post_RPG_Graphics_HotspotType_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_HotspotType_Type::post_RPG_Graphics_HotspotType_Type"));

  return RPG_Graphics_HotspotTypeHelper::stringToRPG_Graphics_HotspotType(post_string());
}

RPG_Graphics_ElementTypeUnion_Type::RPG_Graphics_ElementTypeUnion_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_ElementTypeUnion_Type::RPG_Graphics_ElementTypeUnion_Type"));

  myCurrentElementType.discriminator = RPG_Graphics_ElementTypeUnion::INVALID;
  myCurrentElementType.interfaceelementtype = RPG_GRAPHICS_INTERFACEELEMENTTYPE_INVALID;
}

void RPG_Graphics_ElementTypeUnion_Type::_characters(const ::xml_schema::ro_string& elementType_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_ElementTypeUnion_Type::_characters"));

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
  ACE_TRACE(ACE_TEXT("RPG_Graphics_ElementTypeUnion_Type::post_RPG_Graphics_ElementTypeUnion_Type"));

  RPG_Graphics_ElementTypeUnion result = myCurrentElementType;

  // clear structure
  myCurrentElementType.discriminator = RPG_Graphics_ElementTypeUnion::INVALID;
  myCurrentElementType.interfaceelementtype = RPG_GRAPHICS_INTERFACEELEMENTTYPE_INVALID;

  return result;
}

RPG_Graphics_Element_Type::RPG_Graphics_Element_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Element_Type::RPG_Graphics_Element_Type"));

  myCurrentElement.type.discriminator = RPG_Graphics_ElementTypeUnion::INVALID;
  myCurrentElement.type.interfaceelementtype = RPG_GRAPHICS_INTERFACEELEMENTTYPE_INVALID;
  myCurrentElement.offsetX = 0;
  myCurrentElement.offsetY = 0;
  myCurrentElement.width = 0;
  myCurrentElement.height = 0;
}

void RPG_Graphics_Element_Type::type(const RPG_Graphics_ElementTypeUnion& type_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Element_Type::type"));

  myCurrentElement.type = type_in;
}

void RPG_Graphics_Element_Type::offsetX(unsigned int offsetX_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Element_Type::offsetX"));

  myCurrentElement.offsetX = offsetX_in;
}

void RPG_Graphics_Element_Type::offsetY(unsigned int offsetY_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Element_Type::offsetY"));

  myCurrentElement.offsetY = offsetY_in;
}

void RPG_Graphics_Element_Type::width(unsigned int width_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Element_Type::width"));

  myCurrentElement.width = width_in;
}

void RPG_Graphics_Element_Type::height(unsigned int height_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Element_Type::height"));

  myCurrentElement.height = height_in;
}

RPG_Graphics_Element RPG_Graphics_Element_Type::post_RPG_Graphics_Element_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Element_Type::post_RPG_Graphics_Element_Type"));

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

RPG_Graphics_Graphic_Type::RPG_Graphics_Graphic_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Graphic_Type::RPG_Graphics_Graphic_Type"));

  myCurrentGraphic.category = RPG_GRAPHICS_CATEGORY_INVALID;
  myCurrentGraphic.type = RPG_GRAPHICS_TYPE_INVALID;
  myCurrentGraphic.orientation = RPG_GRAPHICS_TILEORIENTATION_INVALID;
  myCurrentGraphic.style.discriminator = RPG_Graphics_StyleUnion::INVALID;
  myCurrentGraphic.style.floorstyle = RPG_GRAPHICS_FLOORSTYLE_INVALID;
  myCurrentGraphic.elements.clear();
  myCurrentGraphic.file.clear();
  myCurrentGraphic.size = 0;
}

void RPG_Graphics_Graphic_Type::category(const RPG_Graphics_Category& category_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Graphic_Type::category"));

  myCurrentGraphic.category = category_in;
}

void RPG_Graphics_Graphic_Type::type(const RPG_Graphics_Type& type_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Graphic_Type::type"));

  myCurrentGraphic.type = type_in;
}

void RPG_Graphics_Graphic_Type::orientation(const RPG_Graphics_TileOrientation& orientation_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Graphic_Type::orientation"));

  myCurrentGraphic.orientation = orientation_in;
}

void RPG_Graphics_Graphic_Type::style(const RPG_Graphics_StyleUnion& style_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Graphic_Type::style"));

  myCurrentGraphic.style = style_in;
}

void RPG_Graphics_Graphic_Type::element(const RPG_Graphics_Element& element_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Graphic_Type::element"));

  myCurrentGraphic.elements.push_back(element_in);
}

void RPG_Graphics_Graphic_Type::file(const ::std::string& file_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Graphic_Type::file"));

  myCurrentGraphic.file = file_in;
}

void RPG_Graphics_Graphic_Type::size(unsigned int size_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Graphic_Type::size"));

  myCurrentGraphic.size = size_in;
}

RPG_Graphics_Graphic RPG_Graphics_Graphic_Type::post_RPG_Graphics_Graphic_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Graphic_Type::post_RPG_Graphics_Graphic_Type"));

  RPG_Graphics_Graphic result = myCurrentGraphic;

  // clear structure
  myCurrentGraphic.category = RPG_GRAPHICS_CATEGORY_INVALID;
  myCurrentGraphic.type = RPG_GRAPHICS_TYPE_INVALID;
  myCurrentGraphic.orientation = RPG_GRAPHICS_TILEORIENTATION_INVALID;
  myCurrentGraphic.style.discriminator = RPG_Graphics_StyleUnion::INVALID;
  myCurrentGraphic.style.floorstyle = RPG_GRAPHICS_FLOORSTYLE_INVALID;
  myCurrentGraphic.elements.clear();
  myCurrentGraphic.file.clear();
  myCurrentGraphic.size = 0;

  return result;
}

RPG_Graphics_Dictionary_Type::RPG_Graphics_Dictionary_Type(RPG_Graphics_Dictionary_t* graphicsDictionary_in)
 : myDictionary(graphicsDictionary_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Dictionary_Type::RPG_Graphics_Dictionary_Type"));

}

RPG_Graphics_Dictionary_Type::~RPG_Graphics_Dictionary_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Dictionary_Type::~RPG_Graphics_Dictionary_Type"));

}

// void RPG_Graphics_Dictionary_Type::pre()
// {
//   ACE_TRACE(ACE_TEXT("RPG_Graphics_Dictionary_Type::pre"));
//
// }

void RPG_Graphics_Dictionary_Type::graphic(const RPG_Graphics_Graphic& graphic_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Dictionary_Type::graphic"));

  RPG_Graphics_t graphic = graphic_in;

  myDictionary->insert(std::make_pair(graphic.type, graphic));
}

void RPG_Graphics_Dictionary_Type::post_RPG_Graphics_Dictionary_Type()
{
  ACE_TRACE(ACE_TEXT("RPG_Graphics_Dictionary_Type::post_RPG_Graphics_Dictionary_Type"));

  ACE_DEBUG((LM_DEBUG,
             ACE_TEXT("finished parsing dictionary, retrieved %d graphic(s)...\n"),
             myDictionary->size()));
}
