// Copyright (C) 2005-2008 Code Synthesis Tools CC
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
// In addition, as a special exception, Code Synthesis Tools CC gives
// permission to link this program with the Xerces-C++ library (or with
// modified versions of Xerces-C++ that use the same license as Xerces-C++),
// and distribute linked combinations including the two. You must obey
// the GNU General Public License version 2 in all respects for all of
// the code used other than Xerces-C++. If you modify this copy of the
// program, you may extend this exception to your version of the program,
// but you are not obligated to do so. If you do not wish to do so, delete
// this exception statement from your version.
//
// Furthermore, Code Synthesis Tools CC makes a special exception for
// the Free/Libre and Open Source Software (FLOSS) which is described
// in the accompanying FLOSSE file.
//

#ifndef CXX_____RPG_COMMON_XML_TYPES_H
#define CXX_____RPG_COMMON_XML_TYPES_H

// Begin prologue.
//
//
// End prologue.

#include <xsd/cxx/config.hxx>

#if (XSD_INT_VERSION != 3020000L)
#error XSD runtime version mismatch
#endif

#include <xsd/cxx/pre.hxx>

// Forward declarations
//
class RPG_Common_Attribute_Type_pskel;
class RPG_Common_SavingThrow_Type_pskel;
class RPG_Common_SavingThrowCheck_Type_pskel;

#ifndef XSD_USE_CHAR
#define XSD_USE_CHAR
#endif

#ifndef XSD_CXX_PARSER_USE_CHAR
#define XSD_CXX_PARSER_USE_CHAR
#endif

#include "rpg_XMLSchema_XML_types.h"

#include "rpg_common_incl.h"

class RPG_Common_Attribute_Type_pskel: public virtual ::xml_schema::string_pskel
{
  public:
  // Parser callbacks. Override them in your implementation.
  //
  // virtual void
  // pre ();

  virtual RPG_Common_Attribute
  post_RPG_Common_Attribute_Type () = 0;
};

class RPG_Common_SavingThrow_Type_pskel: public virtual ::xml_schema::string_pskel
{
  public:
  // Parser callbacks. Override them in your implementation.
  //
  // virtual void
  // pre ();

  virtual RPG_Common_SavingThrow
  post_RPG_Common_SavingThrow_Type () = 0;
};

class RPG_Common_SavingThrowCheck_Type_pskel: public ::xml_schema::complex_content
{
  public:
  // Parser callbacks. Override them in your implementation.
  //
  // virtual void
  // pre ();

  virtual void
  type (const RPG_Common_SavingThrow&);

  virtual void
  attribute (const RPG_Common_Attribute&);

  virtual void
  difficultyClass (unsigned char);

  virtual RPG_Common_SavingThrowCheck
  post_RPG_Common_SavingThrowCheck_Type () = 0;

  // Parser construction API.
  //
  void
  type_parser (::RPG_Common_SavingThrow_Type_pskel&);

  void
  attribute_parser (::RPG_Common_Attribute_Type_pskel&);

  void
  difficultyClass_parser (::xml_schema::unsigned_byte_pskel&);

  void
  parsers (::RPG_Common_SavingThrow_Type_pskel& /* type */,
           ::RPG_Common_Attribute_Type_pskel& /* attribute */,
           ::xml_schema::unsigned_byte_pskel& /* difficultyClass */);

  // Constructor.
  //
  RPG_Common_SavingThrowCheck_Type_pskel ();

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
  ::RPG_Common_SavingThrow_Type_pskel* type_parser_;
  ::RPG_Common_Attribute_Type_pskel* attribute_parser_;
  ::xml_schema::unsigned_byte_pskel* difficultyClass_parser_;
};

#include <xsd/cxx/post.hxx>

// Begin epilogue.
//
//
// End epilogue.

#endif // CXX_____RPG_COMMON_XML_TYPES_H