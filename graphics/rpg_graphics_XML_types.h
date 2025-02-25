// Copyright (c) 2005-2023 Code Synthesis.
//
// This program was generated by CodeSynthesis XSD, an XML Schema to
// C++ data binding compiler.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License version 2 as
// published by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
//
// In addition, as a special exception, Code Synthesis gives permission
// to link this program with the Xerces-C++ library (or with modified
// versions of Xerces-C++ that use the same license as Xerces-C++), and
// distribute linked combinations including the two. You must obey the GNU
// General Public License version 2 in all respects for all of the code
// used other than Xerces-C++. If you modify this copy of the program, you
// may extend this exception to your version of the program, but you are
// not obligated to do so. If you do not wish to do so, delete this
// exception statement from your version.
//
// Furthermore, Code Synthesis makes a special exception for the Free/Libre
// and Open Source Software (FLOSS) which is described in the accompanying
// FLOSSE file.
//

#ifndef CXX__GRAPHICS_ETC_RPG_GRAPHICS_XML_TYPES_H
#define CXX__GRAPHICS_ETC_RPG_GRAPHICS_XML_TYPES_H

#ifndef XSD_CXX11
#define XSD_CXX11
#endif

// Begin prologue.
//
//
// End prologue.

#include <xsd/cxx/config.hxx>

#if (LIBXSD_VERSION != 400002000000000L)
#error XSD runtime version mismatch
#endif

#include <xsd/cxx/pre.hxx>

// Forward declarations
//
class RPG_Graphics_Category_Type_pskel;
class RPG_Graphics_Orientation_Type_pskel;
class RPG_Graphics_TileType_Type_pskel;
class RPG_Graphics_FloorStyle_Type_pskel;
class RPG_Graphics_StairsStyle_Type_pskel;
class RPG_Graphics_WallStyle_Type_pskel;
class RPG_Graphics_EdgeStyle_Type_pskel;
class RPG_Graphics_DoorStyle_Type_pskel;
class RPG_Graphics_StyleUnion_Type_pskel;
class RPG_Graphics_Style_Type_pskel;
class RPG_Graphics_Cursor_Type_pskel;
class RPG_Graphics_Font_Type_pskel;
class RPG_Graphics_Image_Type_pskel;
class RPG_Graphics_Sprite_Type_pskel;
class RPG_Graphics_TileGraphic_Type_pskel;
class RPG_Graphics_TileSetGraphic_Type_pskel;
class RPG_Graphics_GraphicTypeUnion_Type_pskel;
class RPG_Graphics_WindowType_Type_pskel;
class RPG_Graphics_InterfaceElementType_Type_pskel;
class RPG_Graphics_HotspotType_Type_pskel;
class RPG_Graphics_ElementTypeUnion_Type_pskel;
class RPG_Graphics_Element_Type_pskel;
class RPG_Graphics_Tile_Type_pskel;
class RPG_Graphics_TileSetType_Type_pskel;
class RPG_Graphics_TileSet_Type_pskel;
class RPG_Graphics_Graphic_Type_pskel;
class RPG_Graphics_Dictionary_Type_pskel;
class RPG_Graphics_ColorName_Type_pskel;
class RPG_Graphics_ColorRGBA_Type_pskel;

#ifndef XSD_USE_CHAR
#define XSD_USE_CHAR
#endif

#ifndef XSD_CXX_PARSER_USE_CHAR
#define XSD_CXX_PARSER_USE_CHAR
#endif

#include "rpg_XMLSchema_XML_types.h"

#include "rpg_graphics_incl.h"

class RPG_Graphics_Category_Type_pskel: public virtual ::xml_schema::string_pskel
{
  public:
  // Parser callbacks. Override them in your implementation.
  //
  // virtual void
  // pre ();

  virtual RPG_Graphics_Category
  post_RPG_Graphics_Category_Type () = 0;
};

class RPG_Graphics_Orientation_Type_pskel: public virtual ::xml_schema::string_pskel
{
  public:
  // Parser callbacks. Override them in your implementation.
  //
  // virtual void
  // pre ();

  virtual RPG_Graphics_Orientation
  post_RPG_Graphics_Orientation_Type () = 0;
};

class RPG_Graphics_TileType_Type_pskel: public virtual ::xml_schema::string_pskel
{
  public:
  // Parser callbacks. Override them in your implementation.
  //
  // virtual void
  // pre ();

  virtual RPG_Graphics_TileType
  post_RPG_Graphics_TileType_Type () = 0;
};

class RPG_Graphics_FloorStyle_Type_pskel: public virtual ::xml_schema::string_pskel
{
  public:
  // Parser callbacks. Override them in your implementation.
  //
  // virtual void
  // pre ();

  virtual RPG_Graphics_FloorStyle
  post_RPG_Graphics_FloorStyle_Type () = 0;
};

class RPG_Graphics_StairsStyle_Type_pskel: public virtual ::xml_schema::string_pskel
{
  public:
  // Parser callbacks. Override them in your implementation.
  //
  // virtual void
  // pre ();

  virtual RPG_Graphics_StairsStyle
  post_RPG_Graphics_StairsStyle_Type () = 0;
};

class RPG_Graphics_WallStyle_Type_pskel: public virtual ::xml_schema::string_pskel
{
  public:
  // Parser callbacks. Override them in your implementation.
  //
  // virtual void
  // pre ();

  virtual RPG_Graphics_WallStyle
  post_RPG_Graphics_WallStyle_Type () = 0;
};

class RPG_Graphics_EdgeStyle_Type_pskel: public virtual ::xml_schema::string_pskel
{
  public:
  // Parser callbacks. Override them in your implementation.
  //
  // virtual void
  // pre ();

  virtual RPG_Graphics_EdgeStyle
  post_RPG_Graphics_EdgeStyle_Type () = 0;
};

class RPG_Graphics_DoorStyle_Type_pskel: public virtual ::xml_schema::string_pskel
{
  public:
  // Parser callbacks. Override them in your implementation.
  //
  // virtual void
  // pre ();

  virtual RPG_Graphics_DoorStyle
  post_RPG_Graphics_DoorStyle_Type () = 0;
};

class RPG_Graphics_StyleUnion_Type_pskel: public ::xml_schema::simple_content
{
  public:
  // Parser callbacks. Override them in your implementation.
  //
  // virtual void
  // pre ();
  //
  // virtual void
  // _characters (const ::xml_schema::ro_string&);

  virtual RPG_Graphics_StyleUnion
  post_RPG_Graphics_StyleUnion_Type () = 0;
};

class RPG_Graphics_Style_Type_pskel: public ::xml_schema::complex_content
{
  public:
  // Parser callbacks. Override them in your implementation.
  //
  // virtual void
  // pre ();

  virtual void
  floor (const RPG_Graphics_FloorStyle&);

  virtual void
  edge (const RPG_Graphics_EdgeStyle&);

  virtual void
  wall (const RPG_Graphics_WallStyle&);

  virtual void
  half_height_walls (bool);

  virtual void
  door (const RPG_Graphics_DoorStyle&);

  virtual void
  post_RPG_Graphics_Style_Type ();

  // Parser construction API.
  //
  void
  floor_parser (::RPG_Graphics_FloorStyle_Type_pskel&);

  void
  edge_parser (::RPG_Graphics_EdgeStyle_Type_pskel&);

  void
  wall_parser (::RPG_Graphics_WallStyle_Type_pskel&);

  void
  half_height_walls_parser (::xml_schema::boolean_pskel&);

  void
  door_parser (::RPG_Graphics_DoorStyle_Type_pskel&);

  void
  parsers (::RPG_Graphics_FloorStyle_Type_pskel& /* floor */,
           ::RPG_Graphics_EdgeStyle_Type_pskel& /* edge */,
           ::RPG_Graphics_WallStyle_Type_pskel& /* wall */,
           ::xml_schema::boolean_pskel& /* half_height_walls */,
           ::RPG_Graphics_DoorStyle_Type_pskel& /* door */);

  // Constructor.
  //
  RPG_Graphics_Style_Type_pskel ();

  // Implementation.
  //
  protected:
  virtual bool
  _start_element_impl (const ::xml_schema::ro_string&,
                       const ::xml_schema::ro_string&,
                       const ::xml_schema::ro_string*);

  virtual bool
  _end_element_impl (const ::xml_schema::ro_string&,
                     const ::xml_schema::ro_string&);

  protected:
  ::RPG_Graphics_FloorStyle_Type_pskel* floor_parser_;
  ::RPG_Graphics_EdgeStyle_Type_pskel* edge_parser_;
  ::RPG_Graphics_WallStyle_Type_pskel* wall_parser_;
  ::xml_schema::boolean_pskel* half_height_walls_parser_;
  ::RPG_Graphics_DoorStyle_Type_pskel* door_parser_;
};

class RPG_Graphics_Cursor_Type_pskel: public virtual ::xml_schema::string_pskel
{
  public:
  // Parser callbacks. Override them in your implementation.
  //
  // virtual void
  // pre ();

  virtual RPG_Graphics_Cursor
  post_RPG_Graphics_Cursor_Type () = 0;
};

class RPG_Graphics_Font_Type_pskel: public virtual ::xml_schema::string_pskel
{
  public:
  // Parser callbacks. Override them in your implementation.
  //
  // virtual void
  // pre ();

  virtual RPG_Graphics_Font
  post_RPG_Graphics_Font_Type () = 0;
};

class RPG_Graphics_Image_Type_pskel: public virtual ::xml_schema::string_pskel
{
  public:
  // Parser callbacks. Override them in your implementation.
  //
  // virtual void
  // pre ();

  virtual RPG_Graphics_Image
  post_RPG_Graphics_Image_Type () = 0;
};

class RPG_Graphics_Sprite_Type_pskel: public virtual ::xml_schema::string_pskel
{
  public:
  // Parser callbacks. Override them in your implementation.
  //
  // virtual void
  // pre ();

  virtual RPG_Graphics_Sprite
  post_RPG_Graphics_Sprite_Type () = 0;
};

class RPG_Graphics_TileGraphic_Type_pskel: public virtual ::xml_schema::string_pskel
{
  public:
  // Parser callbacks. Override them in your implementation.
  //
  // virtual void
  // pre ();

  virtual RPG_Graphics_TileGraphic
  post_RPG_Graphics_TileGraphic_Type () = 0;
};

class RPG_Graphics_TileSetGraphic_Type_pskel: public virtual ::xml_schema::string_pskel
{
  public:
  // Parser callbacks. Override them in your implementation.
  //
  // virtual void
  // pre ();

  virtual RPG_Graphics_TileSetGraphic
  post_RPG_Graphics_TileSetGraphic_Type () = 0;
};

class RPG_Graphics_GraphicTypeUnion_Type_pskel: public ::xml_schema::simple_content
{
  public:
  // Parser callbacks. Override them in your implementation.
  //
  // virtual void
  // pre ();
  //
  // virtual void
  // _characters (const ::xml_schema::ro_string&);

  virtual RPG_Graphics_GraphicTypeUnion
  post_RPG_Graphics_GraphicTypeUnion_Type () = 0;
};

class RPG_Graphics_WindowType_Type_pskel: public virtual ::xml_schema::string_pskel
{
  public:
  // Parser callbacks. Override them in your implementation.
  //
  // virtual void
  // pre ();

  virtual RPG_Graphics_WindowType
  post_RPG_Graphics_WindowType_Type () = 0;
};

class RPG_Graphics_InterfaceElementType_Type_pskel: public virtual ::xml_schema::string_pskel
{
  public:
  // Parser callbacks. Override them in your implementation.
  //
  // virtual void
  // pre ();

  virtual RPG_Graphics_InterfaceElementType
  post_RPG_Graphics_InterfaceElementType_Type () = 0;
};

class RPG_Graphics_HotspotType_Type_pskel: public virtual ::xml_schema::string_pskel
{
  public:
  // Parser callbacks. Override them in your implementation.
  //
  // virtual void
  // pre ();

  virtual RPG_Graphics_HotspotType
  post_RPG_Graphics_HotspotType_Type () = 0;
};

class RPG_Graphics_ElementTypeUnion_Type_pskel: public ::xml_schema::simple_content
{
  public:
  // Parser callbacks. Override them in your implementation.
  //
  // virtual void
  // pre ();
  //
  // virtual void
  // _characters (const ::xml_schema::ro_string&);

  virtual RPG_Graphics_ElementTypeUnion
  post_RPG_Graphics_ElementTypeUnion_Type () = 0;
};

class RPG_Graphics_Element_Type_pskel: public ::xml_schema::complex_content
{
  public:
  // Parser callbacks. Override them in your implementation.
  //
  // virtual void
  // pre ();

  virtual void
  type (const RPG_Graphics_ElementTypeUnion&);

  virtual void
  window (const RPG_Graphics_WindowType&);

  virtual void
  offsetX (unsigned int);

  virtual void
  offsetY (unsigned int);

  virtual void
  width (unsigned int);

  virtual void
  height (unsigned int);

  virtual RPG_Graphics_Element
  post_RPG_Graphics_Element_Type () = 0;

  // Parser construction API.
  //
  void
  type_parser (::RPG_Graphics_ElementTypeUnion_Type_pskel&);

  void
  window_parser (::RPG_Graphics_WindowType_Type_pskel&);

  void
  offsetX_parser (::xml_schema::unsigned_int_pskel&);

  void
  offsetY_parser (::xml_schema::unsigned_int_pskel&);

  void
  width_parser (::xml_schema::unsigned_int_pskel&);

  void
  height_parser (::xml_schema::unsigned_int_pskel&);

  void
  parsers (::RPG_Graphics_ElementTypeUnion_Type_pskel& /* type */,
           ::RPG_Graphics_WindowType_Type_pskel& /* window */,
           ::xml_schema::unsigned_int_pskel& /* offsetX */,
           ::xml_schema::unsigned_int_pskel& /* offsetY */,
           ::xml_schema::unsigned_int_pskel& /* width */,
           ::xml_schema::unsigned_int_pskel& /* height */);

  // Constructor.
  //
  RPG_Graphics_Element_Type_pskel ();

  // Implementation.
  //
  protected:
  virtual bool
  _start_element_impl (const ::xml_schema::ro_string&,
                       const ::xml_schema::ro_string&,
                       const ::xml_schema::ro_string*);

  virtual bool
  _end_element_impl (const ::xml_schema::ro_string&,
                     const ::xml_schema::ro_string&);

  protected:
  ::RPG_Graphics_ElementTypeUnion_Type_pskel* type_parser_;
  ::RPG_Graphics_WindowType_Type_pskel* window_parser_;
  ::xml_schema::unsigned_int_pskel* offsetX_parser_;
  ::xml_schema::unsigned_int_pskel* offsetY_parser_;
  ::xml_schema::unsigned_int_pskel* width_parser_;
  ::xml_schema::unsigned_int_pskel* height_parser_;
};

class RPG_Graphics_Tile_Type_pskel: public ::xml_schema::complex_content
{
  public:
  // Parser callbacks. Override them in your implementation.
  //
  // virtual void
  // pre ();

  virtual void
  type (const RPG_Graphics_TileType&);

  virtual void
  reference (const RPG_Graphics_GraphicTypeUnion&);

  virtual void
  style (const RPG_Graphics_StyleUnion&);

  virtual void
  orientation (const RPG_Graphics_Orientation&);

  virtual void
  file (const ::std::string&);

  virtual void
  offsetX (int);

  virtual void
  offsetY (int);

  virtual void
  open (bool);

  virtual void
  broken (bool);

  virtual RPG_Graphics_Tile
  post_RPG_Graphics_Tile_Type () = 0;

  // Parser construction API.
  //
  void
  type_parser (::RPG_Graphics_TileType_Type_pskel&);

  void
  reference_parser (::RPG_Graphics_GraphicTypeUnion_Type_pskel&);

  void
  style_parser (::RPG_Graphics_StyleUnion_Type_pskel&);

  void
  orientation_parser (::RPG_Graphics_Orientation_Type_pskel&);

  void
  file_parser (::xml_schema::string_pskel&);

  void
  offsetX_parser (::xml_schema::int_pskel&);

  void
  offsetY_parser (::xml_schema::int_pskel&);

  void
  open_parser (::xml_schema::boolean_pskel&);

  void
  broken_parser (::xml_schema::boolean_pskel&);

  void
  parsers (::RPG_Graphics_TileType_Type_pskel& /* type */,
           ::RPG_Graphics_GraphicTypeUnion_Type_pskel& /* reference */,
           ::RPG_Graphics_StyleUnion_Type_pskel& /* style */,
           ::RPG_Graphics_Orientation_Type_pskel& /* orientation */,
           ::xml_schema::string_pskel& /* file */,
           ::xml_schema::int_pskel& /* offsetX */,
           ::xml_schema::int_pskel& /* offsetY */,
           ::xml_schema::boolean_pskel& /* open */,
           ::xml_schema::boolean_pskel& /* broken */);

  // Constructor.
  //
  RPG_Graphics_Tile_Type_pskel ();

  // Implementation.
  //
  protected:
  virtual bool
  _start_element_impl (const ::xml_schema::ro_string&,
                       const ::xml_schema::ro_string&,
                       const ::xml_schema::ro_string*);

  virtual bool
  _end_element_impl (const ::xml_schema::ro_string&,
                     const ::xml_schema::ro_string&);

  virtual bool
  _attribute_impl (const ::xml_schema::ro_string&,
                   const ::xml_schema::ro_string&,
                   const ::xml_schema::ro_string&);

  protected:
  ::RPG_Graphics_TileType_Type_pskel* type_parser_;
  ::RPG_Graphics_GraphicTypeUnion_Type_pskel* reference_parser_;
  ::RPG_Graphics_StyleUnion_Type_pskel* style_parser_;
  ::RPG_Graphics_Orientation_Type_pskel* orientation_parser_;
  ::xml_schema::string_pskel* file_parser_;
  ::xml_schema::int_pskel* offsetX_parser_;
  ::xml_schema::int_pskel* offsetY_parser_;
  ::xml_schema::boolean_pskel* open_parser_;
  ::xml_schema::boolean_pskel* broken_parser_;
};

class RPG_Graphics_TileSetType_Type_pskel: public virtual ::xml_schema::string_pskel
{
  public:
  // Parser callbacks. Override them in your implementation.
  //
  // virtual void
  // pre ();

  virtual RPG_Graphics_TileSetType
  post_RPG_Graphics_TileSetType_Type () = 0;
};

class RPG_Graphics_TileSet_Type_pskel: public ::xml_schema::complex_content
{
  public:
  // Parser callbacks. Override them in your implementation.
  //
  // virtual void
  // pre ();

  virtual void
  type (const RPG_Graphics_TileSetType&);

  virtual void
  style (const RPG_Graphics_StyleUnion&);

  virtual void
  tile (const RPG_Graphics_Tile&);

  virtual void
  half (bool);

  virtual RPG_Graphics_TileSet
  post_RPG_Graphics_TileSet_Type () = 0;

  // Parser construction API.
  //
  void
  type_parser (::RPG_Graphics_TileSetType_Type_pskel&);

  void
  style_parser (::RPG_Graphics_StyleUnion_Type_pskel&);

  void
  tile_parser (::RPG_Graphics_Tile_Type_pskel&);

  void
  half_parser (::xml_schema::boolean_pskel&);

  void
  parsers (::RPG_Graphics_TileSetType_Type_pskel& /* type */,
           ::RPG_Graphics_StyleUnion_Type_pskel& /* style */,
           ::RPG_Graphics_Tile_Type_pskel& /* tile */,
           ::xml_schema::boolean_pskel& /* half */);

  // Constructor.
  //
  RPG_Graphics_TileSet_Type_pskel ();

  // Implementation.
  //
  protected:
  virtual bool
  _start_element_impl (const ::xml_schema::ro_string&,
                       const ::xml_schema::ro_string&,
                       const ::xml_schema::ro_string*);

  virtual bool
  _end_element_impl (const ::xml_schema::ro_string&,
                     const ::xml_schema::ro_string&);

  virtual bool
  _attribute_impl (const ::xml_schema::ro_string&,
                   const ::xml_schema::ro_string&,
                   const ::xml_schema::ro_string&);

  protected:
  ::RPG_Graphics_TileSetType_Type_pskel* type_parser_;
  ::RPG_Graphics_StyleUnion_Type_pskel* style_parser_;
  ::RPG_Graphics_Tile_Type_pskel* tile_parser_;
  ::xml_schema::boolean_pskel* half_parser_;
};

class RPG_Graphics_Graphic_Type_pskel: public ::xml_schema::complex_content
{
  public:
  // Parser callbacks. Override them in your implementation.
  //
  // virtual void
  // pre ();

  virtual void
  category (const RPG_Graphics_Category&);

  virtual void
  type (const RPG_Graphics_GraphicTypeUnion&);

  virtual void
  tile (const RPG_Graphics_Tile&);

  virtual void
  tileset (const RPG_Graphics_TileSet&);

  virtual void
  element (const RPG_Graphics_Element&);

  virtual void
  file (const ::std::string&);

  virtual void
  size (unsigned int);

  virtual RPG_Graphics_Graphic
  post_RPG_Graphics_Graphic_Type () = 0;

  // Parser construction API.
  //
  void
  category_parser (::RPG_Graphics_Category_Type_pskel&);

  void
  type_parser (::RPG_Graphics_GraphicTypeUnion_Type_pskel&);

  void
  tile_parser (::RPG_Graphics_Tile_Type_pskel&);

  void
  tileset_parser (::RPG_Graphics_TileSet_Type_pskel&);

  void
  element_parser (::RPG_Graphics_Element_Type_pskel&);

  void
  file_parser (::xml_schema::string_pskel&);

  void
  size_parser (::xml_schema::unsigned_int_pskel&);

  void
  parsers (::RPG_Graphics_Category_Type_pskel& /* category */,
           ::RPG_Graphics_GraphicTypeUnion_Type_pskel& /* type */,
           ::RPG_Graphics_Tile_Type_pskel& /* tile */,
           ::RPG_Graphics_TileSet_Type_pskel& /* tileset */,
           ::RPG_Graphics_Element_Type_pskel& /* element */,
           ::xml_schema::string_pskel& /* file */,
           ::xml_schema::unsigned_int_pskel& /* size */);

  // Constructor.
  //
  RPG_Graphics_Graphic_Type_pskel ();

  // Implementation.
  //
  protected:
  virtual bool
  _start_element_impl (const ::xml_schema::ro_string&,
                       const ::xml_schema::ro_string&,
                       const ::xml_schema::ro_string*);

  virtual bool
  _end_element_impl (const ::xml_schema::ro_string&,
                     const ::xml_schema::ro_string&);

  virtual bool
  _attribute_impl (const ::xml_schema::ro_string&,
                   const ::xml_schema::ro_string&,
                   const ::xml_schema::ro_string&);

  protected:
  ::RPG_Graphics_Category_Type_pskel* category_parser_;
  ::RPG_Graphics_GraphicTypeUnion_Type_pskel* type_parser_;
  ::RPG_Graphics_Tile_Type_pskel* tile_parser_;
  ::RPG_Graphics_TileSet_Type_pskel* tileset_parser_;
  ::RPG_Graphics_Element_Type_pskel* element_parser_;
  ::xml_schema::string_pskel* file_parser_;
  ::xml_schema::unsigned_int_pskel* size_parser_;
};

class RPG_Graphics_Dictionary_Type_pskel: public ::xml_schema::complex_content
{
  public:
  // Parser callbacks. Override them in your implementation.
  //
  // virtual void
  // pre ();

  virtual void
  graphic (const RPG_Graphics_Graphic&);

  virtual void
  post_RPG_Graphics_Dictionary_Type ();

  // Parser construction API.
  //
  void
  graphic_parser (::RPG_Graphics_Graphic_Type_pskel&);

  void
  parsers (::RPG_Graphics_Graphic_Type_pskel& /* graphic */);

  // Constructor.
  //
  RPG_Graphics_Dictionary_Type_pskel ();

  // Implementation.
  //
  protected:
  virtual bool
  _start_element_impl (const ::xml_schema::ro_string&,
                       const ::xml_schema::ro_string&,
                       const ::xml_schema::ro_string*);

  virtual bool
  _end_element_impl (const ::xml_schema::ro_string&,
                     const ::xml_schema::ro_string&);

  protected:
  ::RPG_Graphics_Graphic_Type_pskel* graphic_parser_;
};

class RPG_Graphics_ColorName_Type_pskel: public virtual ::xml_schema::string_pskel
{
  public:
  // Parser callbacks. Override them in your implementation.
  //
  // virtual void
  // pre ();

  virtual RPG_Graphics_ColorName
  post_RPG_Graphics_ColorName_Type () = 0;
};

class RPG_Graphics_ColorRGBA_Type_pskel: public ::xml_schema::complex_content
{
  public:
  // Parser callbacks. Override them in your implementation.
  //
  // virtual void
  // pre ();

  virtual void
  r (unsigned char);

  virtual void
  g (unsigned char);

  virtual void
  b (unsigned char);

  virtual void
  a (unsigned char);

  virtual RPG_Graphics_ColorRGBA
  post_RPG_Graphics_ColorRGBA_Type () = 0;

  // Parser construction API.
  //
  void
  r_parser (::xml_schema::unsigned_byte_pskel&);

  void
  g_parser (::xml_schema::unsigned_byte_pskel&);

  void
  b_parser (::xml_schema::unsigned_byte_pskel&);

  void
  a_parser (::xml_schema::unsigned_byte_pskel&);

  void
  parsers (::xml_schema::unsigned_byte_pskel& /* r */,
           ::xml_schema::unsigned_byte_pskel& /* g */,
           ::xml_schema::unsigned_byte_pskel& /* b */,
           ::xml_schema::unsigned_byte_pskel& /* a */);

  // Constructor.
  //
  RPG_Graphics_ColorRGBA_Type_pskel ();

  // Implementation.
  //
  protected:
  virtual bool
  _start_element_impl (const ::xml_schema::ro_string&,
                       const ::xml_schema::ro_string&,
                       const ::xml_schema::ro_string*);

  virtual bool
  _end_element_impl (const ::xml_schema::ro_string&,
                     const ::xml_schema::ro_string&);

  protected:
  ::xml_schema::unsigned_byte_pskel* r_parser_;
  ::xml_schema::unsigned_byte_pskel* g_parser_;
  ::xml_schema::unsigned_byte_pskel* b_parser_;
  ::xml_schema::unsigned_byte_pskel* a_parser_;
};

#include <xsd/cxx/post.hxx>

// Begin epilogue.
//
//
// End epilogue.

#endif // CXX__GRAPHICS_ETC_RPG_GRAPHICS_XML_TYPES_H
