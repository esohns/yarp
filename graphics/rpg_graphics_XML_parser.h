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

#include "ace/Global_Macros.h"

#include "rpg_graphics_common.h"
//#include "rpg_graphics_exports.h"
#include "rpg_graphics_XML_types.h"

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

class RPG_Graphics_EdgeStyle_Type
  : public RPG_Graphics_EdgeStyle_Type_pskel,
  public ::xml_schema::string_pimpl
{
  public:
//   virtual void pre();
    virtual RPG_Graphics_EdgeStyle post_RPG_Graphics_EdgeStyle_Type();
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

class RPG_Graphics_Cursor_Type
  : public RPG_Graphics_Cursor_Type_pskel,
  public ::xml_schema::string_pimpl
{
  public:
//   virtual void pre();
    virtual RPG_Graphics_Cursor post_RPG_Graphics_Cursor_Type();
};

class RPG_Graphics_Font_Type
  : public RPG_Graphics_Font_Type_pskel,
  public ::xml_schema::string_pimpl
{
  public:
//   virtual void pre();
    virtual RPG_Graphics_Font post_RPG_Graphics_Font_Type();
};

class RPG_Graphics_Image_Type
  : public RPG_Graphics_Image_Type_pskel,
  public ::xml_schema::string_pimpl
{
  public:
//   virtual void pre();
    virtual RPG_Graphics_Image post_RPG_Graphics_Image_Type();
};

class RPG_Graphics_Sprite_Type
  : public RPG_Graphics_Sprite_Type_pskel,
  public ::xml_schema::string_pimpl
{
  public:
//   virtual void pre();
    virtual RPG_Graphics_Sprite post_RPG_Graphics_Sprite_Type();
};

class RPG_Graphics_TileGraphic_Type
  : public RPG_Graphics_TileGraphic_Type_pskel,
  public ::xml_schema::string_pimpl
{
  public:
//   virtual void pre();
    virtual RPG_Graphics_TileGraphic post_RPG_Graphics_TileGraphic_Type();
};

class RPG_Graphics_TileSetGraphic_Type
  : public RPG_Graphics_TileSetGraphic_Type_pskel,
   public ::xml_schema::string_pimpl
{
 public:
//   virtual void pre();
   virtual RPG_Graphics_TileSetGraphic post_RPG_Graphics_TileSetGraphic_Type();
};

class RPG_Graphics_GraphicTypeUnion_Type
  : public RPG_Graphics_GraphicTypeUnion_Type_pskel
{
  public:
    RPG_Graphics_GraphicTypeUnion_Type();

//   virtual void pre();
    virtual void _characters(const ::xml_schema::ro_string&);
    virtual RPG_Graphics_GraphicTypeUnion post_RPG_Graphics_GraphicTypeUnion_Type();

  private:
    RPG_Graphics_GraphicTypeUnion myCurrentType;
};

class RPG_Graphics_WindowType_Type
: public RPG_Graphics_WindowType_Type_pskel,
public ::xml_schema::string_pimpl
{
public:
  //   virtual void pre();
  virtual RPG_Graphics_WindowType post_RPG_Graphics_WindowType_Type();
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
  virtual void window(const RPG_Graphics_WindowType&);
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
  virtual void reference(const RPG_Graphics_GraphicTypeUnion&);
  virtual void style(const RPG_Graphics_StyleUnion&);
  virtual void orientation(const RPG_Graphics_Orientation&);
  virtual void file(const ::std::string&);
  virtual void offsetX(int);
  virtual void offsetY(int);
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
  virtual void type(const RPG_Graphics_GraphicTypeUnion&);
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
  ACE_UNIMPLEMENTED_FUNC(RPG_Graphics_Dictionary_Type());
  ACE_UNIMPLEMENTED_FUNC(RPG_Graphics_Dictionary_Type(const RPG_Graphics_Dictionary_Type&));
  ACE_UNIMPLEMENTED_FUNC(RPG_Graphics_Dictionary_Type& operator=(const RPG_Graphics_Dictionary_Type&));

  RPG_Graphics_Dictionary_t* myDictionary;
};

class RPG_Graphics_ColorName_Type
 : public RPG_Graphics_ColorName_Type_pskel,
   public ::xml_schema::string_pimpl
{
 public:
  //   virtual void pre();
  virtual RPG_Graphics_ColorName post_RPG_Graphics_ColorName_Type();
};

class RPG_Graphics_ColorRGBA_Type
 : public RPG_Graphics_ColorRGBA_Type_pskel
{
 public:
  RPG_Graphics_ColorRGBA_Type();

//     virtual void pre();
  virtual void r(unsigned char);
  virtual void g(unsigned char);
  virtual void b(unsigned char);
  virtual void a(unsigned char);

  virtual RPG_Graphics_ColorRGBA post_RPG_Graphics_ColorRGBA_Type();

 private:
  RPG_Graphics_ColorRGBA myCurrentColorRGBA;
};

//class RPG_Graphics_ColorUnion_Type
//  : public RPG_Graphics_ColorUnion_Type_pskel
//{
//  public:
//    RPG_Graphics_ColorUnion_Type();

////   virtual void pre();
//    virtual void _characters(const ::xml_schema::ro_string&);
//    virtual RPG_Graphics_ColorUnion post_RPG_Graphics_ColorUnion_Type();

//  private:
//    RPG_Graphics_ColorUnion myCurrentType;
//};

#endif
