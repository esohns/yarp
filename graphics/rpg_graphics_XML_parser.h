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
#ifndef RPG_GRAPHICS_XML_PARSER_H
#define RPG_GRAPHICS_XML_PARSER_H

#include "rpg_graphics_XML_types.h"
#include "rpg_graphics_common.h"

#include <ace/Global_Macros.h>

/**
	@author Erik Sohns <erik.sohns@web.de>
*/
class RPG_Graphics_Category_Type
 : public RPG_Graphics_Category_Type_pskel,
   public ::xml_schema::string_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Graphics_Category post_RPG_Graphics_Category_Type();
};

class RPG_Graphics_Orientation_Type
 : public RPG_Graphics_Orientation_Type_pskel,
   public ::xml_schema::string_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Graphics_Orientation post_RPG_Graphics_Orientation_Type();
};

class RPG_Graphics_TileType_Type
 : public RPG_Graphics_TileType_Type_pskel,
   public ::xml_schema::string_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Graphics_TileType post_RPG_Graphics_TileType_Type();
};

class RPG_Graphics_FloorStyle_Type
 : public RPG_Graphics_FloorStyle_Type_pskel,
   public ::xml_schema::string_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Graphics_FloorStyle post_RPG_Graphics_FloorStyle_Type();
};

class RPG_Graphics_StairsStyle_Type
 : public RPG_Graphics_StairsStyle_Type_pskel,
   public ::xml_schema::string_pimpl
{
  public:
//   virtual void pre();
    virtual RPG_Graphics_StairsStyle post_RPG_Graphics_StairsStyle_Type();
};

class RPG_Graphics_WallStyle_Type
 : public RPG_Graphics_WallStyle_Type_pskel,
   public ::xml_schema::string_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Graphics_WallStyle post_RPG_Graphics_WallStyle_Type();
};

class RPG_Graphics_DoorStyle_Type
 : public RPG_Graphics_DoorStyle_Type_pskel,
   public ::xml_schema::string_pimpl
{
  public:
//   virtual void pre();
    virtual RPG_Graphics_DoorStyle post_RPG_Graphics_DoorStyle_Type();
};

class RPG_Graphics_StyleUnion_Type
 : public RPG_Graphics_StyleUnion_Type_pskel
{
 public:
  RPG_Graphics_StyleUnion_Type();

//   virtual void pre();
  virtual void _characters(const ::xml_schema::ro_string&);
  virtual RPG_Graphics_StyleUnion post_RPG_Graphics_StyleUnion_Type();

 private:
  RPG_Graphics_StyleUnion myCurrentStyle;
};

class RPG_Graphics_Type_Type
 : public RPG_Graphics_Type_Type_pskel,
   public ::xml_schema::string_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Graphics_Type post_RPG_Graphics_Type_Type();
};

class RPG_Graphics_InterfaceElementType_Type
 : public RPG_Graphics_InterfaceElementType_Type_pskel,
   public ::xml_schema::string_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Graphics_InterfaceElementType post_RPG_Graphics_InterfaceElementType_Type();
};

class RPG_Graphics_HotspotType_Type
 : public RPG_Graphics_HotspotType_Type_pskel,
   public ::xml_schema::string_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Graphics_HotspotType post_RPG_Graphics_HotspotType_Type();
};

class RPG_Graphics_ElementTypeUnion_Type
 : public RPG_Graphics_ElementTypeUnion_Type_pskel
{
 public:
  RPG_Graphics_ElementTypeUnion_Type();

//   virtual void pre();
  virtual void _characters(const ::xml_schema::ro_string&);
  virtual RPG_Graphics_ElementTypeUnion post_RPG_Graphics_ElementTypeUnion_Type();

 private:
  RPG_Graphics_ElementTypeUnion myCurrentElementType;
};

class RPG_Graphics_Element_Type
 : public RPG_Graphics_Element_Type_pskel
{
 public:
  RPG_Graphics_Element_Type();

//     virtual void pre();
  virtual void type(const RPG_Graphics_ElementTypeUnion&);
  virtual void offsetX(unsigned int);
  virtual void offsetY(unsigned int);
  virtual void width(unsigned int);
  virtual void height(unsigned int);
  virtual RPG_Graphics_Element post_RPG_Graphics_Element_Type();

 private:
  RPG_Graphics_Element myCurrentElement;
};

class RPG_Graphics_Tile_Type
 : public RPG_Graphics_Tile_Type_pskel
{
 public:
  RPG_Graphics_Tile_Type();

//     virtual void pre();
  virtual void type(const RPG_Graphics_TileType&);
  virtual void style(const RPG_Graphics_StyleUnion&);
  virtual void orientation(const RPG_Graphics_Orientation&);
  virtual void file(const ::std::string&);
  virtual void offsetX(long long);
  virtual void offsetY(long long);
  virtual void open(bool);
  virtual void broken(bool);
  virtual RPG_Graphics_Tile post_RPG_Graphics_Tile_Type();

 private:
  RPG_Graphics_Tile myCurrentTile;
};

class RPG_Graphics_TileSetType_Type
 : public RPG_Graphics_TileSetType_Type_pskel,
   public ::xml_schema::string_pimpl
{
 public:
//   virtual void pre();
  virtual RPG_Graphics_TileSetType post_RPG_Graphics_TileSetType_Type();
};

class RPG_Graphics_TileSet_Type
 : public RPG_Graphics_TileSet_Type_pskel
{
 public:
  RPG_Graphics_TileSet_Type();

//     virtual void pre();
  virtual void type(const RPG_Graphics_TileSetType&);
  virtual void style(const RPG_Graphics_StyleUnion&);
  virtual void tile(const RPG_Graphics_Tile&);
  virtual void half(bool);
  virtual RPG_Graphics_TileSet post_RPG_Graphics_TileSet_Type();

 private:
  RPG_Graphics_TileSet myCurrentTileSet;
};

class RPG_Graphics_Graphic_Type
 : public RPG_Graphics_Graphic_Type_pskel
{
 public:
  RPG_Graphics_Graphic_Type();

//     virtual void pre();
  virtual void category(const RPG_Graphics_Category&);
  virtual void type(const RPG_Graphics_Type&);
  virtual void tile(const RPG_Graphics_Tile&);
  virtual void tileset(const RPG_Graphics_TileSet&);
  virtual void element(const RPG_Graphics_Element&);
  virtual void file(const ::std::string&);
  virtual void size(unsigned int);
  virtual RPG_Graphics_Graphic post_RPG_Graphics_Graphic_Type();

 private:
  RPG_Graphics_Graphic myCurrentGraphic;
};

class RPG_Graphics_Dictionary_Type
 : public RPG_Graphics_Dictionary_Type_pskel
{
 public:
  RPG_Graphics_Dictionary_Type(RPG_Graphics_Dictionary_t*); // graphics dictionary
  virtual ~RPG_Graphics_Dictionary_Type();

//   virtual void pre();
  virtual void graphic(const RPG_Graphics_Graphic&);
  virtual void post_RPG_Graphics_Dictionary_Type();

 private:
  // safety measures
  ACE_UNIMPLEMENTED_FUNC(RPG_Graphics_Dictionary_Type());
  ACE_UNIMPLEMENTED_FUNC(RPG_Graphics_Dictionary_Type(const RPG_Graphics_Dictionary_Type&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Graphics_Dictionary_Type& operator=(const RPG_Graphics_Dictionary_Type&));

  RPG_Graphics_Dictionary_t* myDictionary;
};

#endif
