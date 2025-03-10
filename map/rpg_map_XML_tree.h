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

#ifndef CXX__MAP_ETC_RPG_MAP_XML_TREE_H
#define CXX__MAP_ETC_RPG_MAP_XML_TREE_H

#ifndef XSD_CXX11
#define XSD_CXX11
#endif

#ifndef XSD_USE_CHAR
#define XSD_USE_CHAR
#endif

#ifndef XSD_CXX_TREE_USE_CHAR
#define XSD_CXX_TREE_USE_CHAR
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

#include "rpg_XMLSchema_XML_tree.h"

// Forward declarations.
//
class RPG_Map_DoorState_XMLTree_Type;
class RPG_Map_Position_XMLTree_Type;
class RPG_Map_Direction_XMLTree_Type;
class RPG_Map_Door_XMLTree_Type;
class RPG_Map_Level_XMLTree_Type;
class RPG_Map_Door_State_XMLTree_Type;
class RPG_Map_State_XMLTree_Type;

#include <memory>    // ::std::unique_ptr
#include <limits>    // std::numeric_limits
#include <algorithm> // std::binary_search
#include <utility>   // std::move

#include <xsd/cxx/xml/char-utf8.hxx>

#include <xsd/cxx/tree/exceptions.hxx>
#include <xsd/cxx/tree/elements.hxx>
#include <xsd/cxx/tree/containers.hxx>
#include <xsd/cxx/tree/list.hxx>

#include <xsd/cxx/xml/dom/parsing-header.hxx>

#include "rpg_common_environment_XML_tree.h"

class RPG_Map_DoorState_XMLTree_Type: public ::xml_schema::string
{
  public:
  enum value
  {
    DOORSTATE_OPEN,
    DOORSTATE_CLOSED,
    DOORSTATE_LOCKED,
    DOORSTATE_BROKEN
  };

  RPG_Map_DoorState_XMLTree_Type (value v);

  RPG_Map_DoorState_XMLTree_Type (const char* v);

  RPG_Map_DoorState_XMLTree_Type (const ::std::string& v);

  RPG_Map_DoorState_XMLTree_Type (const ::xml_schema::string& v);

  RPG_Map_DoorState_XMLTree_Type (const ::xercesc::DOMElement& e,
                                  ::xml_schema::flags f = 0,
                                  ::xml_schema::container* c = 0);

  RPG_Map_DoorState_XMLTree_Type (const ::xercesc::DOMAttr& a,
                                  ::xml_schema::flags f = 0,
                                  ::xml_schema::container* c = 0);

  RPG_Map_DoorState_XMLTree_Type (const ::std::string& s,
                                  const ::xercesc::DOMElement* e,
                                  ::xml_schema::flags f = 0,
                                  ::xml_schema::container* c = 0);

  RPG_Map_DoorState_XMLTree_Type (const RPG_Map_DoorState_XMLTree_Type& x,
                                  ::xml_schema::flags f = 0,
                                  ::xml_schema::container* c = 0);

#ifdef XSD_CXX11
  RPG_Map_DoorState_XMLTree_Type&
  operator= (const RPG_Map_DoorState_XMLTree_Type&) = default;
#endif

  virtual RPG_Map_DoorState_XMLTree_Type*
  _clone (::xml_schema::flags f = 0,
          ::xml_schema::container* c = 0) const;

  RPG_Map_DoorState_XMLTree_Type&
  operator= (value v);

  virtual
  operator value () const
  {
    return _xsd_RPG_Map_DoorState_XMLTree_Type_convert ();
  }

  protected:
  value
  _xsd_RPG_Map_DoorState_XMLTree_Type_convert () const;

  public:
  static const char* const _xsd_RPG_Map_DoorState_XMLTree_Type_literals_[4];
  static const value _xsd_RPG_Map_DoorState_XMLTree_Type_indexes_[4];
};

class RPG_Map_Position_XMLTree_Type: public ::xml_schema::type
{
  public:
  // x
  //
  typedef ::xml_schema::unsigned_int x_type;
  typedef ::xsd::cxx::tree::traits< x_type, char > x_traits;

  const x_type&
  x () const;

  x_type&
  x ();

  void
  x (const x_type& x);

  // y
  //
  typedef ::xml_schema::unsigned_int y_type;
  typedef ::xsd::cxx::tree::traits< y_type, char > y_traits;

  const y_type&
  y () const;

  y_type&
  y ();

  void
  y (const y_type& x);

  // Constructors.
  //
  RPG_Map_Position_XMLTree_Type (const x_type&,
                                 const y_type&);

  RPG_Map_Position_XMLTree_Type (const ::xercesc::DOMElement& e,
                                 ::xml_schema::flags f = 0,
                                 ::xml_schema::container* c = 0);

  RPG_Map_Position_XMLTree_Type (const RPG_Map_Position_XMLTree_Type& x,
                                 ::xml_schema::flags f = 0,
                                 ::xml_schema::container* c = 0);

  virtual RPG_Map_Position_XMLTree_Type*
  _clone (::xml_schema::flags f = 0,
          ::xml_schema::container* c = 0) const;

  RPG_Map_Position_XMLTree_Type&
  operator= (const RPG_Map_Position_XMLTree_Type& x);

  virtual 
  ~RPG_Map_Position_XMLTree_Type ();

  // Implementation.
  //
  protected:
  void
  parse (::xsd::cxx::xml::dom::parser< char >&,
         ::xml_schema::flags);

  protected:
  ::xsd::cxx::tree::one< x_type > x_;
  ::xsd::cxx::tree::one< y_type > y_;
};

bool
operator== (const RPG_Map_Position_XMLTree_Type&, const RPG_Map_Position_XMLTree_Type&);

bool
operator!= (const RPG_Map_Position_XMLTree_Type&, const RPG_Map_Position_XMLTree_Type&);


class RPG_Map_Direction_XMLTree_Type: public ::xml_schema::string
{
  public:
  enum value
  {
    DIRECTION_UP,
    DIRECTION_RIGHT,
    DIRECTION_DOWN,
    DIRECTION_LEFT
  };

  RPG_Map_Direction_XMLTree_Type (value v);

  RPG_Map_Direction_XMLTree_Type (const char* v);

  RPG_Map_Direction_XMLTree_Type (const ::std::string& v);

  RPG_Map_Direction_XMLTree_Type (const ::xml_schema::string& v);

  RPG_Map_Direction_XMLTree_Type (const ::xercesc::DOMElement& e,
                                  ::xml_schema::flags f = 0,
                                  ::xml_schema::container* c = 0);

  RPG_Map_Direction_XMLTree_Type (const ::xercesc::DOMAttr& a,
                                  ::xml_schema::flags f = 0,
                                  ::xml_schema::container* c = 0);

  RPG_Map_Direction_XMLTree_Type (const ::std::string& s,
                                  const ::xercesc::DOMElement* e,
                                  ::xml_schema::flags f = 0,
                                  ::xml_schema::container* c = 0);

  RPG_Map_Direction_XMLTree_Type (const RPG_Map_Direction_XMLTree_Type& x,
                                  ::xml_schema::flags f = 0,
                                  ::xml_schema::container* c = 0);

#ifdef XSD_CXX11
  RPG_Map_Direction_XMLTree_Type&
  operator= (const RPG_Map_Direction_XMLTree_Type&) = default;
#endif

  virtual RPG_Map_Direction_XMLTree_Type*
  _clone (::xml_schema::flags f = 0,
          ::xml_schema::container* c = 0) const;

  RPG_Map_Direction_XMLTree_Type&
  operator= (value v);

  virtual
  operator value () const
  {
    return _xsd_RPG_Map_Direction_XMLTree_Type_convert ();
  }

  protected:
  value
  _xsd_RPG_Map_Direction_XMLTree_Type_convert () const;

  public:
  static const char* const _xsd_RPG_Map_Direction_XMLTree_Type_literals_[4];
  static const value _xsd_RPG_Map_Direction_XMLTree_Type_indexes_[4];
};

class RPG_Map_Door_XMLTree_Type: public ::xml_schema::type
{
  public:
  // position
  //
  typedef ::RPG_Map_Position_XMLTree_Type position_type;
  typedef ::xsd::cxx::tree::traits< position_type, char > position_traits;

  const position_type&
  position () const;

  position_type&
  position ();

  void
  position (const position_type& x);

  void
  position (::std::unique_ptr< position_type > p);

  // outside
  //
  typedef ::RPG_Map_Direction_XMLTree_Type outside_type;
  typedef ::xsd::cxx::tree::traits< outside_type, char > outside_traits;

  const outside_type&
  outside () const;

  outside_type&
  outside ();

  void
  outside (const outside_type& x);

  void
  outside (::std::unique_ptr< outside_type > p);

  // Constructors.
  //
  RPG_Map_Door_XMLTree_Type (const position_type&,
                             const outside_type&);

  RPG_Map_Door_XMLTree_Type (::std::unique_ptr< position_type >,
                             const outside_type&);

  RPG_Map_Door_XMLTree_Type (const ::xercesc::DOMElement& e,
                             ::xml_schema::flags f = 0,
                             ::xml_schema::container* c = 0);

  RPG_Map_Door_XMLTree_Type (const RPG_Map_Door_XMLTree_Type& x,
                             ::xml_schema::flags f = 0,
                             ::xml_schema::container* c = 0);

  virtual RPG_Map_Door_XMLTree_Type*
  _clone (::xml_schema::flags f = 0,
          ::xml_schema::container* c = 0) const;

  RPG_Map_Door_XMLTree_Type&
  operator= (const RPG_Map_Door_XMLTree_Type& x);

  virtual 
  ~RPG_Map_Door_XMLTree_Type ();

  // Implementation.
  //
  protected:
  void
  parse (::xsd::cxx::xml::dom::parser< char >&,
         ::xml_schema::flags);

  protected:
  ::xsd::cxx::tree::one< position_type > position_;
  ::xsd::cxx::tree::one< outside_type > outside_;
};

bool
operator== (const RPG_Map_Door_XMLTree_Type&, const RPG_Map_Door_XMLTree_Type&);

bool
operator!= (const RPG_Map_Door_XMLTree_Type&, const RPG_Map_Door_XMLTree_Type&);


class RPG_Map_Level_XMLTree_Type: public ::xml_schema::type
{
  public:
  // name
  //
  typedef ::xml_schema::string name_type;
  typedef ::xsd::cxx::tree::traits< name_type, char > name_traits;

  const name_type&
  name () const;

  name_type&
  name ();

  void
  name (const name_type& x);

  void
  name (::std::unique_ptr< name_type > p);

  // environment
  //
  typedef ::RPG_Common_Environment_XMLTree_Type environment_type;
  typedef ::xsd::cxx::tree::traits< environment_type, char > environment_traits;

  const environment_type&
  environment () const;

  environment_type&
  environment ();

  void
  environment (const environment_type& x);

  void
  environment (::std::unique_ptr< environment_type > p);

  // map
  //
  typedef ::xml_schema::string map_type;
  typedef ::xsd::cxx::tree::traits< map_type, char > map_traits;

  const map_type&
  map () const;

  map_type&
  map ();

  void
  map (const map_type& x);

  void
  map (::std::unique_ptr< map_type > p);

  // door
  //
  typedef ::RPG_Map_Door_XMLTree_Type door_type;
  typedef ::xsd::cxx::tree::sequence< door_type > door_sequence;
  typedef door_sequence::iterator door_iterator;
  typedef door_sequence::const_iterator door_const_iterator;
  typedef ::xsd::cxx::tree::traits< door_type, char > door_traits;

  const door_sequence&
  door () const;

  door_sequence&
  door ();

  void
  door (const door_sequence& s);

  // Constructors.
  //
  RPG_Map_Level_XMLTree_Type (const name_type&,
                              const environment_type&,
                              const map_type&);

  RPG_Map_Level_XMLTree_Type (const name_type&,
                              ::std::unique_ptr< environment_type >,
                              const map_type&);

  RPG_Map_Level_XMLTree_Type (const ::xercesc::DOMElement& e,
                              ::xml_schema::flags f = 0,
                              ::xml_schema::container* c = 0);

  RPG_Map_Level_XMLTree_Type (const RPG_Map_Level_XMLTree_Type& x,
                              ::xml_schema::flags f = 0,
                              ::xml_schema::container* c = 0);

  virtual RPG_Map_Level_XMLTree_Type*
  _clone (::xml_schema::flags f = 0,
          ::xml_schema::container* c = 0) const;

  RPG_Map_Level_XMLTree_Type&
  operator= (const RPG_Map_Level_XMLTree_Type& x);

  virtual 
  ~RPG_Map_Level_XMLTree_Type ();

  // Implementation.
  //
  protected:
  void
  parse (::xsd::cxx::xml::dom::parser< char >&,
         ::xml_schema::flags);

  protected:
  ::xsd::cxx::tree::one< name_type > name_;
  ::xsd::cxx::tree::one< environment_type > environment_;
  ::xsd::cxx::tree::one< map_type > map_;
  door_sequence door_;
};

bool
operator== (const RPG_Map_Level_XMLTree_Type&, const RPG_Map_Level_XMLTree_Type&);

bool
operator!= (const RPG_Map_Level_XMLTree_Type&, const RPG_Map_Level_XMLTree_Type&);


class RPG_Map_Door_State_XMLTree_Type: public ::xml_schema::type
{
  public:
  // position
  //
  typedef ::RPG_Map_Position_XMLTree_Type position_type;
  typedef ::xsd::cxx::tree::traits< position_type, char > position_traits;

  const position_type&
  position () const;

  position_type&
  position ();

  void
  position (const position_type& x);

  void
  position (::std::unique_ptr< position_type > p);

  // state
  //
  typedef ::RPG_Map_DoorState_XMLTree_Type state_type;
  typedef ::xsd::cxx::tree::traits< state_type, char > state_traits;

  const state_type&
  state () const;

  state_type&
  state ();

  void
  state (const state_type& x);

  void
  state (::std::unique_ptr< state_type > p);

  // Constructors.
  //
  RPG_Map_Door_State_XMLTree_Type (const position_type&,
                                   const state_type&);

  RPG_Map_Door_State_XMLTree_Type (::std::unique_ptr< position_type >,
                                   const state_type&);

  RPG_Map_Door_State_XMLTree_Type (const ::xercesc::DOMElement& e,
                                   ::xml_schema::flags f = 0,
                                   ::xml_schema::container* c = 0);

  RPG_Map_Door_State_XMLTree_Type (const RPG_Map_Door_State_XMLTree_Type& x,
                                   ::xml_schema::flags f = 0,
                                   ::xml_schema::container* c = 0);

  virtual RPG_Map_Door_State_XMLTree_Type*
  _clone (::xml_schema::flags f = 0,
          ::xml_schema::container* c = 0) const;

  RPG_Map_Door_State_XMLTree_Type&
  operator= (const RPG_Map_Door_State_XMLTree_Type& x);

  virtual 
  ~RPG_Map_Door_State_XMLTree_Type ();

  // Implementation.
  //
  protected:
  void
  parse (::xsd::cxx::xml::dom::parser< char >&,
         ::xml_schema::flags);

  protected:
  ::xsd::cxx::tree::one< position_type > position_;
  ::xsd::cxx::tree::one< state_type > state_;
};

bool
operator== (const RPG_Map_Door_State_XMLTree_Type&, const RPG_Map_Door_State_XMLTree_Type&);

bool
operator!= (const RPG_Map_Door_State_XMLTree_Type&, const RPG_Map_Door_State_XMLTree_Type&);


class RPG_Map_State_XMLTree_Type: public ::xml_schema::type
{
  public:
  // file
  //
  typedef ::xml_schema::string file_type;
  typedef ::xsd::cxx::tree::traits< file_type, char > file_traits;

  const file_type&
  file () const;

  file_type&
  file ();

  void
  file (const file_type& x);

  void
  file (::std::unique_ptr< file_type > p);

  // door
  //
  typedef ::RPG_Map_Door_State_XMLTree_Type door_type;
  typedef ::xsd::cxx::tree::sequence< door_type > door_sequence;
  typedef door_sequence::iterator door_iterator;
  typedef door_sequence::const_iterator door_const_iterator;
  typedef ::xsd::cxx::tree::traits< door_type, char > door_traits;

  const door_sequence&
  door () const;

  door_sequence&
  door ();

  void
  door (const door_sequence& s);

  // Constructors.
  //
  RPG_Map_State_XMLTree_Type (const file_type&);

  RPG_Map_State_XMLTree_Type (const ::xercesc::DOMElement& e,
                              ::xml_schema::flags f = 0,
                              ::xml_schema::container* c = 0);

  RPG_Map_State_XMLTree_Type (const RPG_Map_State_XMLTree_Type& x,
                              ::xml_schema::flags f = 0,
                              ::xml_schema::container* c = 0);

  virtual RPG_Map_State_XMLTree_Type*
  _clone (::xml_schema::flags f = 0,
          ::xml_schema::container* c = 0) const;

  RPG_Map_State_XMLTree_Type&
  operator= (const RPG_Map_State_XMLTree_Type& x);

  virtual 
  ~RPG_Map_State_XMLTree_Type ();

  // Implementation.
  //
  protected:
  void
  parse (::xsd::cxx::xml::dom::parser< char >&,
         ::xml_schema::flags);

  protected:
  ::xsd::cxx::tree::one< file_type > file_;
  door_sequence door_;
};

bool
operator== (const RPG_Map_State_XMLTree_Type&, const RPG_Map_State_XMLTree_Type&);

bool
operator!= (const RPG_Map_State_XMLTree_Type&, const RPG_Map_State_XMLTree_Type&);


#include <iosfwd>

::std::ostream&
operator<< (::std::ostream&, RPG_Map_DoorState_XMLTree_Type::value);

::std::ostream&
operator<< (::std::ostream&, const RPG_Map_DoorState_XMLTree_Type&);

::std::ostream&
operator<< (::std::ostream&, const RPG_Map_Position_XMLTree_Type&);

::std::ostream&
operator<< (::std::ostream&, RPG_Map_Direction_XMLTree_Type::value);

::std::ostream&
operator<< (::std::ostream&, const RPG_Map_Direction_XMLTree_Type&);

::std::ostream&
operator<< (::std::ostream&, const RPG_Map_Door_XMLTree_Type&);

::std::ostream&
operator<< (::std::ostream&, const RPG_Map_Level_XMLTree_Type&);

::std::ostream&
operator<< (::std::ostream&, const RPG_Map_Door_State_XMLTree_Type&);

::std::ostream&
operator<< (::std::ostream&, const RPG_Map_State_XMLTree_Type&);

#include <iosfwd>

#include <xercesc/sax/InputSource.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMErrorHandler.hpp>

// Parse a URI or a local file.
//

::std::unique_ptr< ::RPG_Map_Level_XMLTree_Type >
map_t (const ::std::string& uri,
       ::xml_schema::flags f = 0,
       const ::xml_schema::properties& p = ::xml_schema::properties ());

::std::unique_ptr< ::RPG_Map_Level_XMLTree_Type >
map_t (const ::std::string& uri,
       ::xml_schema::error_handler& eh,
       ::xml_schema::flags f = 0,
       const ::xml_schema::properties& p = ::xml_schema::properties ());

::std::unique_ptr< ::RPG_Map_Level_XMLTree_Type >
map_t (const ::std::string& uri,
       ::xercesc::DOMErrorHandler& eh,
       ::xml_schema::flags f = 0,
       const ::xml_schema::properties& p = ::xml_schema::properties ());

// Parse std::istream.
//

::std::unique_ptr< ::RPG_Map_Level_XMLTree_Type >
map_t (::std::istream& is,
       ::xml_schema::flags f = 0,
       const ::xml_schema::properties& p = ::xml_schema::properties ());

::std::unique_ptr< ::RPG_Map_Level_XMLTree_Type >
map_t (::std::istream& is,
       ::xml_schema::error_handler& eh,
       ::xml_schema::flags f = 0,
       const ::xml_schema::properties& p = ::xml_schema::properties ());

::std::unique_ptr< ::RPG_Map_Level_XMLTree_Type >
map_t (::std::istream& is,
       ::xercesc::DOMErrorHandler& eh,
       ::xml_schema::flags f = 0,
       const ::xml_schema::properties& p = ::xml_schema::properties ());

::std::unique_ptr< ::RPG_Map_Level_XMLTree_Type >
map_t (::std::istream& is,
       const ::std::string& id,
       ::xml_schema::flags f = 0,
       const ::xml_schema::properties& p = ::xml_schema::properties ());

::std::unique_ptr< ::RPG_Map_Level_XMLTree_Type >
map_t (::std::istream& is,
       const ::std::string& id,
       ::xml_schema::error_handler& eh,
       ::xml_schema::flags f = 0,
       const ::xml_schema::properties& p = ::xml_schema::properties ());

::std::unique_ptr< ::RPG_Map_Level_XMLTree_Type >
map_t (::std::istream& is,
       const ::std::string& id,
       ::xercesc::DOMErrorHandler& eh,
       ::xml_schema::flags f = 0,
       const ::xml_schema::properties& p = ::xml_schema::properties ());

// Parse xercesc::InputSource.
//

::std::unique_ptr< ::RPG_Map_Level_XMLTree_Type >
map_t (::xercesc::InputSource& is,
       ::xml_schema::flags f = 0,
       const ::xml_schema::properties& p = ::xml_schema::properties ());

::std::unique_ptr< ::RPG_Map_Level_XMLTree_Type >
map_t (::xercesc::InputSource& is,
       ::xml_schema::error_handler& eh,
       ::xml_schema::flags f = 0,
       const ::xml_schema::properties& p = ::xml_schema::properties ());

::std::unique_ptr< ::RPG_Map_Level_XMLTree_Type >
map_t (::xercesc::InputSource& is,
       ::xercesc::DOMErrorHandler& eh,
       ::xml_schema::flags f = 0,
       const ::xml_schema::properties& p = ::xml_schema::properties ());

// Parse xercesc::DOMDocument.
//

::std::unique_ptr< ::RPG_Map_Level_XMLTree_Type >
map_t (const ::xercesc::DOMDocument& d,
       ::xml_schema::flags f = 0,
       const ::xml_schema::properties& p = ::xml_schema::properties ());

::std::unique_ptr< ::RPG_Map_Level_XMLTree_Type >
map_t (::xml_schema::dom::unique_ptr< ::xercesc::DOMDocument > d,
       ::xml_schema::flags f = 0,
       const ::xml_schema::properties& p = ::xml_schema::properties ());

#include <iosfwd>

#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMErrorHandler.hpp>
#include <xercesc/framework/XMLFormatter.hpp>

#include <xsd/cxx/xml/dom/auto-ptr.hxx>

void
operator<< (::xercesc::DOMElement&, const RPG_Map_DoorState_XMLTree_Type&);

void
operator<< (::xercesc::DOMAttr&, const RPG_Map_DoorState_XMLTree_Type&);

void
operator<< (::xml_schema::list_stream&,
            const RPG_Map_DoorState_XMLTree_Type&);

void
operator<< (::xercesc::DOMElement&, const RPG_Map_Position_XMLTree_Type&);

void
operator<< (::xercesc::DOMElement&, const RPG_Map_Direction_XMLTree_Type&);

void
operator<< (::xercesc::DOMAttr&, const RPG_Map_Direction_XMLTree_Type&);

void
operator<< (::xml_schema::list_stream&,
            const RPG_Map_Direction_XMLTree_Type&);

void
operator<< (::xercesc::DOMElement&, const RPG_Map_Door_XMLTree_Type&);

void
operator<< (::xercesc::DOMElement&, const RPG_Map_Level_XMLTree_Type&);

void
operator<< (::xercesc::DOMElement&, const RPG_Map_Door_State_XMLTree_Type&);

void
operator<< (::xercesc::DOMElement&, const RPG_Map_State_XMLTree_Type&);

// Serialize to std::ostream.
//

void
map_t (::std::ostream& os,
       const ::RPG_Map_Level_XMLTree_Type& x, 
       const ::xml_schema::namespace_infomap& m = ::xml_schema::namespace_infomap (),
       const ::std::string& e = "UTF-8",
       ::xml_schema::flags f = 0);

void
map_t (::std::ostream& os,
       const ::RPG_Map_Level_XMLTree_Type& x, 
       ::xml_schema::error_handler& eh,
       const ::xml_schema::namespace_infomap& m = ::xml_schema::namespace_infomap (),
       const ::std::string& e = "UTF-8",
       ::xml_schema::flags f = 0);

void
map_t (::std::ostream& os,
       const ::RPG_Map_Level_XMLTree_Type& x, 
       ::xercesc::DOMErrorHandler& eh,
       const ::xml_schema::namespace_infomap& m = ::xml_schema::namespace_infomap (),
       const ::std::string& e = "UTF-8",
       ::xml_schema::flags f = 0);

// Serialize to xercesc::XMLFormatTarget.
//

void
map_t (::xercesc::XMLFormatTarget& ft,
       const ::RPG_Map_Level_XMLTree_Type& x, 
       const ::xml_schema::namespace_infomap& m = ::xml_schema::namespace_infomap (),
       const ::std::string& e = "UTF-8",
       ::xml_schema::flags f = 0);

void
map_t (::xercesc::XMLFormatTarget& ft,
       const ::RPG_Map_Level_XMLTree_Type& x, 
       ::xml_schema::error_handler& eh,
       const ::xml_schema::namespace_infomap& m = ::xml_schema::namespace_infomap (),
       const ::std::string& e = "UTF-8",
       ::xml_schema::flags f = 0);

void
map_t (::xercesc::XMLFormatTarget& ft,
       const ::RPG_Map_Level_XMLTree_Type& x, 
       ::xercesc::DOMErrorHandler& eh,
       const ::xml_schema::namespace_infomap& m = ::xml_schema::namespace_infomap (),
       const ::std::string& e = "UTF-8",
       ::xml_schema::flags f = 0);

// Serialize to an existing xercesc::DOMDocument.
//

void
map_t (::xercesc::DOMDocument& d,
       const ::RPG_Map_Level_XMLTree_Type& x,
       ::xml_schema::flags f = 0);

// Serialize to a new xercesc::DOMDocument.
//

::xml_schema::dom::unique_ptr< ::xercesc::DOMDocument >
map_t (const ::RPG_Map_Level_XMLTree_Type& x, 
       const ::xml_schema::namespace_infomap& m = ::xml_schema::namespace_infomap (),
       ::xml_schema::flags f = 0);

#include <xsd/cxx/post.hxx>

// Begin epilogue.
//
//
// End epilogue.

#endif // CXX__MAP_ETC_RPG_MAP_XML_TREE_H
