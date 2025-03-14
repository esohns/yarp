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

// Begin prologue.
//
#include "stdafx.h"
//
// End prologue.

#include <xsd/cxx/pre.hxx>

#include "rpg_dice_XML_types.h"

// RPG_Dice_Roll_Type_pskel
//

void RPG_Dice_Roll_Type_pskel::
numDice_parser (::xml_schema::unsigned_int_pskel& p)
{
  this->numDice_parser_ = &p;
}

void RPG_Dice_Roll_Type_pskel::
typeDice_parser (::RPG_Dice_DieType_Type_pskel& p)
{
  this->typeDice_parser_ = &p;
}

void RPG_Dice_Roll_Type_pskel::
modifier_parser (::xml_schema::int_pskel& p)
{
  this->modifier_parser_ = &p;
}

void RPG_Dice_Roll_Type_pskel::
parsers (::xml_schema::unsigned_int_pskel& numDice,
         ::RPG_Dice_DieType_Type_pskel& typeDice,
         ::xml_schema::int_pskel& modifier)
{
  this->numDice_parser_ = &numDice;
  this->typeDice_parser_ = &typeDice;
  this->modifier_parser_ = &modifier;
}

RPG_Dice_Roll_Type_pskel::
RPG_Dice_Roll_Type_pskel ()
: numDice_parser_ (0),
  typeDice_parser_ (0),
  modifier_parser_ (0)
{
}

// RPG_Dice_ValueRange_Type_pskel
//

void RPG_Dice_ValueRange_Type_pskel::
begin_parser (::xml_schema::int_pskel& p)
{
  this->begin_parser_ = &p;
}

void RPG_Dice_ValueRange_Type_pskel::
end_parser (::xml_schema::int_pskel& p)
{
  this->end_parser_ = &p;
}

void RPG_Dice_ValueRange_Type_pskel::
parsers (::xml_schema::int_pskel& begin,
         ::xml_schema::int_pskel& end)
{
  this->begin_parser_ = &begin;
  this->end_parser_ = &end;
}

RPG_Dice_ValueRange_Type_pskel::
RPG_Dice_ValueRange_Type_pskel ()
: begin_parser_ (0),
  end_parser_ (0)
{
}

// RPG_Dice_Roll_Type_pskel
//

void RPG_Dice_Roll_Type_pskel::
numDice (unsigned int)
{
}

void RPG_Dice_Roll_Type_pskel::
typeDice (const RPG_Dice_DieType&)
{
}

void RPG_Dice_Roll_Type_pskel::
modifier (int)
{
}

bool RPG_Dice_Roll_Type_pskel::
_start_element_impl (const ::xml_schema::ro_string& ns,
                     const ::xml_schema::ro_string& n,
                     const ::xml_schema::ro_string* t)
{
  XSD_UNUSED (t);

  if (this->::xml_schema::complex_content::_start_element_impl (ns, n, t))
    return true;

  if (n == "numDice" && ns == "urn:rpg")
  {
    this->::xml_schema::complex_content::context_.top ().parser_ = this->numDice_parser_;

    if (this->numDice_parser_)
      this->numDice_parser_->pre ();

    return true;
  }

  if (n == "typeDice" && ns == "urn:rpg")
  {
    this->::xml_schema::complex_content::context_.top ().parser_ = this->typeDice_parser_;

    if (this->typeDice_parser_)
      this->typeDice_parser_->pre ();

    return true;
  }

  if (n == "modifier" && ns == "urn:rpg")
  {
    this->::xml_schema::complex_content::context_.top ().parser_ = this->modifier_parser_;

    if (this->modifier_parser_)
      this->modifier_parser_->pre ();

    return true;
  }

  return false;
}

bool RPG_Dice_Roll_Type_pskel::
_end_element_impl (const ::xml_schema::ro_string& ns,
                   const ::xml_schema::ro_string& n)
{
  if (this->::xml_schema::complex_content::_end_element_impl (ns, n))
    return true;

  if (n == "numDice" && ns == "urn:rpg")
  {
    if (this->numDice_parser_)
      this->numDice (this->numDice_parser_->post_unsigned_int ());

    return true;
  }

  if (n == "typeDice" && ns == "urn:rpg")
  {
    if (this->typeDice_parser_)
      this->typeDice (this->typeDice_parser_->post_RPG_Dice_DieType_Type ());

    return true;
  }

  if (n == "modifier" && ns == "urn:rpg")
  {
    if (this->modifier_parser_)
      this->modifier (this->modifier_parser_->post_int ());

    return true;
  }

  return false;
}

// RPG_Dice_ValueRange_Type_pskel
//

void RPG_Dice_ValueRange_Type_pskel::
begin (int)
{
}

void RPG_Dice_ValueRange_Type_pskel::
end (int)
{
}

bool RPG_Dice_ValueRange_Type_pskel::
_start_element_impl (const ::xml_schema::ro_string& ns,
                     const ::xml_schema::ro_string& n,
                     const ::xml_schema::ro_string* t)
{
  XSD_UNUSED (t);

  if (this->::xml_schema::complex_content::_start_element_impl (ns, n, t))
    return true;

  if (n == "begin" && ns == "urn:rpg")
  {
    this->::xml_schema::complex_content::context_.top ().parser_ = this->begin_parser_;

    if (this->begin_parser_)
      this->begin_parser_->pre ();

    return true;
  }

  if (n == "end" && ns == "urn:rpg")
  {
    this->::xml_schema::complex_content::context_.top ().parser_ = this->end_parser_;

    if (this->end_parser_)
      this->end_parser_->pre ();

    return true;
  }

  return false;
}

bool RPG_Dice_ValueRange_Type_pskel::
_end_element_impl (const ::xml_schema::ro_string& ns,
                   const ::xml_schema::ro_string& n)
{
  if (this->::xml_schema::complex_content::_end_element_impl (ns, n))
    return true;

  if (n == "begin" && ns == "urn:rpg")
  {
    if (this->begin_parser_)
      this->begin (this->begin_parser_->post_int ());

    return true;
  }

  if (n == "end" && ns == "urn:rpg")
  {
    if (this->end_parser_)
      this->end (this->end_parser_->post_int ());

    return true;
  }

  return false;
}

#include <xsd/cxx/post.hxx>

// Begin epilogue.
//
//
// End epilogue.

