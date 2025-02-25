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

#include "rpg_sound_XML_types.h"

// RPG_Sound_Type_pskel
//

void RPG_Sound_Type_pskel::
category_parser (::RPG_Sound_Category_Type_pskel& p)
{
  this->category_parser_ = &p;
}

void RPG_Sound_Type_pskel::
sound_event_parser (::RPG_Sound_Event_Type_pskel& p)
{
  this->sound_event_parser_ = &p;
}

void RPG_Sound_Type_pskel::
volume_parser (::xml_schema::unsigned_byte_pskel& p)
{
  this->volume_parser_ = &p;
}

void RPG_Sound_Type_pskel::
file_parser (::xml_schema::string_pskel& p)
{
  this->file_parser_ = &p;
}

void RPG_Sound_Type_pskel::
interval_parser (::xml_schema::unsigned_byte_pskel& p)
{
  this->interval_parser_ = &p;
}

void RPG_Sound_Type_pskel::
parsers (::RPG_Sound_Category_Type_pskel& category,
         ::RPG_Sound_Event_Type_pskel& sound_event,
         ::xml_schema::unsigned_byte_pskel& volume,
         ::xml_schema::string_pskel& file,
         ::xml_schema::unsigned_byte_pskel& interval)
{
  this->category_parser_ = &category;
  this->sound_event_parser_ = &sound_event;
  this->volume_parser_ = &volume;
  this->file_parser_ = &file;
  this->interval_parser_ = &interval;
}

RPG_Sound_Type_pskel::
RPG_Sound_Type_pskel ()
: category_parser_ (0),
  sound_event_parser_ (0),
  volume_parser_ (0),
  file_parser_ (0),
  interval_parser_ (0)
{
}

// RPG_Sound_Dictionary_Type_pskel
//

void RPG_Sound_Dictionary_Type_pskel::
sound_parser (::RPG_Sound_Type_pskel& p)
{
  this->sound_parser_ = &p;
}

void RPG_Sound_Dictionary_Type_pskel::
parsers (::RPG_Sound_Type_pskel& sound)
{
  this->sound_parser_ = &sound;
}

RPG_Sound_Dictionary_Type_pskel::
RPG_Sound_Dictionary_Type_pskel ()
: sound_parser_ (0)
{
}

// RPG_Sound_Type_pskel
//

void RPG_Sound_Type_pskel::
category (const RPG_Sound_Category&)
{
}

void RPG_Sound_Type_pskel::
sound_event (const RPG_Sound_Event&)
{
}

void RPG_Sound_Type_pskel::
volume (unsigned char)
{
}

void RPG_Sound_Type_pskel::
file (const ::std::string&)
{
}

void RPG_Sound_Type_pskel::
interval (unsigned char)
{
}

bool RPG_Sound_Type_pskel::
_start_element_impl (const ::xml_schema::ro_string& ns,
                     const ::xml_schema::ro_string& n,
                     const ::xml_schema::ro_string* t)
{
  XSD_UNUSED (t);

  if (this->::xml_schema::complex_content::_start_element_impl (ns, n, t))
    return true;

  if (n == "category" && ns == "urn:rpg")
  {
    this->::xml_schema::complex_content::context_.top ().parser_ = this->category_parser_;

    if (this->category_parser_)
      this->category_parser_->pre ();

    return true;
  }

  if (n == "sound_event" && ns == "urn:rpg")
  {
    this->::xml_schema::complex_content::context_.top ().parser_ = this->sound_event_parser_;

    if (this->sound_event_parser_)
      this->sound_event_parser_->pre ();

    return true;
  }

  if (n == "volume" && ns == "urn:rpg")
  {
    this->::xml_schema::complex_content::context_.top ().parser_ = this->volume_parser_;

    if (this->volume_parser_)
      this->volume_parser_->pre ();

    return true;
  }

  if (n == "file" && ns == "urn:rpg")
  {
    this->::xml_schema::complex_content::context_.top ().parser_ = this->file_parser_;

    if (this->file_parser_)
      this->file_parser_->pre ();

    return true;
  }

  return false;
}

bool RPG_Sound_Type_pskel::
_end_element_impl (const ::xml_schema::ro_string& ns,
                   const ::xml_schema::ro_string& n)
{
  if (this->::xml_schema::complex_content::_end_element_impl (ns, n))
    return true;

  if (n == "category" && ns == "urn:rpg")
  {
    if (this->category_parser_)
      this->category (this->category_parser_->post_RPG_Sound_Category_Type ());

    return true;
  }

  if (n == "sound_event" && ns == "urn:rpg")
  {
    if (this->sound_event_parser_)
      this->sound_event (this->sound_event_parser_->post_RPG_Sound_Event_Type ());

    return true;
  }

  if (n == "volume" && ns == "urn:rpg")
  {
    if (this->volume_parser_)
      this->volume (this->volume_parser_->post_unsigned_byte ());

    return true;
  }

  if (n == "file" && ns == "urn:rpg")
  {
    if (this->file_parser_)
      this->file (this->file_parser_->post_string ());

    return true;
  }

  return false;
}

bool RPG_Sound_Type_pskel::
_attribute_impl (const ::xml_schema::ro_string& ns,
                 const ::xml_schema::ro_string& n,
                 const ::xml_schema::ro_string& v)
{
  if (this->::xml_schema::complex_content::_attribute_impl (ns, n, v))
    return true;

  if (n == "interval" && ns.empty ())
  {
    if (this->interval_parser_)
    {
      this->interval_parser_->pre ();
      this->interval_parser_->_pre_impl ();
      this->interval_parser_->_characters (v);
      this->interval_parser_->_post_impl ();
      this->interval (this->interval_parser_->post_unsigned_byte ());
    }

    return true;
  }

  return false;
}

// RPG_Sound_Dictionary_Type_pskel
//

void RPG_Sound_Dictionary_Type_pskel::
sound (const RPG_Sound&)
{
}

void RPG_Sound_Dictionary_Type_pskel::
post_RPG_Sound_Dictionary_Type ()
{
}

bool RPG_Sound_Dictionary_Type_pskel::
_start_element_impl (const ::xml_schema::ro_string& ns,
                     const ::xml_schema::ro_string& n,
                     const ::xml_schema::ro_string* t)
{
  XSD_UNUSED (t);

  if (this->::xml_schema::complex_content::_start_element_impl (ns, n, t))
    return true;

  if (n == "sound" && ns == "urn:rpg")
  {
    this->::xml_schema::complex_content::context_.top ().parser_ = this->sound_parser_;

    if (this->sound_parser_)
      this->sound_parser_->pre ();

    return true;
  }

  return false;
}

bool RPG_Sound_Dictionary_Type_pskel::
_end_element_impl (const ::xml_schema::ro_string& ns,
                   const ::xml_schema::ro_string& n)
{
  if (this->::xml_schema::complex_content::_end_element_impl (ns, n))
    return true;

  if (n == "sound" && ns == "urn:rpg")
  {
    if (this->sound_parser_)
      this->sound (this->sound_parser_->post_RPG_Sound_Type ());

    return true;
  }

  return false;
}

#include <xsd/cxx/post.hxx>

// Begin epilogue.
//
//
// End epilogue.

