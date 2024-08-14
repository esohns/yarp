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

#ifndef RPG_NET_PROTOCOL_PARSER_DRIVER_H
#define RPG_NET_PROTOCOL_PARSER_DRIVER_H

#include <string>

#include "ace/Global_Macros.h"

#include "location.hh"

#include "common_parser_base.h"
#include "common_parser_common.h"

#include "rpg_net_protocol_iparser.h"
#include "rpg_net_protocol_parser.h"
#include "rpg_net_protocol_scanner.h"

class RPG_Net_Protocol_ParserDriver
 : public Common_ParserBase_T<struct Common_FlexBisonParserConfiguration,
                              yy::RPG_Net_Protocol_Parser,
                              RPG_Net_Protocol_IParser,
                              RPG_Net_Protocol_IParser>
{
  typedef Common_ParserBase_T<struct Common_FlexBisonParserConfiguration,
                              yy::RPG_Net_Protocol_Parser,
                              RPG_Net_Protocol_IParser,
                              RPG_Net_Protocol_IParser> inherited;

 public:
  RPG_Net_Protocol_ParserDriver ();
  inline virtual ~RPG_Net_Protocol_ParserDriver () {}

  // convenient types
  typedef Common_ParserBase_T<struct Common_FlexBisonParserConfiguration,
                              yy::RPG_Net_Protocol_Parser,
                              RPG_Net_Protocol_IParser,
                              RPG_Net_Protocol_IParser> PARSER_BASE_T;

  // implement (part of) BitTorrent_IParser
  using PARSER_BASE_T::initialize;
  using PARSER_BASE_T::buffer;
#if defined (_DEBUG)
  using PARSER_BASE_T::debug;
#endif // _DEBUG
  using PARSER_BASE_T::isBlocking;
  using PARSER_BASE_T::offset;
  using PARSER_BASE_T::parse;
  using PARSER_BASE_T::switchBuffer;
  using PARSER_BASE_T::waitBuffer;

  // implement (part of) RPG_Net_Protocol_IParser
  virtual void error (const yy::location&, // location
                      const std::string&); // message
//  virtual void error (const std::string&); // message
  inline virtual bool hasFinished () const { return scannedBytes_ >= length_; }

  inline virtual struct RPG_Net_Protocol_Command& current () { return currentRecord_; }
  inline virtual void record (struct RPG_Net_Protocol_Command*& record_in) { record (*record_in); }

  inline virtual void scannedBytes (ACE_UINT32 value_in) { scannedBytes_ += value_in; }
  inline virtual ACE_UINT32 scannedBytes () { return scannedBytes_; }
  inline virtual void length (ACE_UINT32 value_in) { length_ = value_in; }
  inline virtual ACE_UINT32 length () { return length_; }

  ////////////////////////////////////////
  // callbacks
  // *IMPORTANT NOTE*: fire-and-forget API
  virtual void record (struct RPG_Net_Protocol_Command&) = 0; // data record

  inline virtual void dump_state () const { ACE_ASSERT (false); ACE_NOTSUP; ACE_NOTREACHED (return;) }

  struct RPG_Net_Protocol_Command currentRecord_;
  ACE_UINT32                      length_;
  ACE_UINT32                      scannedBytes_;

 private:
  ACE_UNIMPLEMENTED_FUNC (RPG_Net_Protocol_ParserDriver (const RPG_Net_Protocol_ParserDriver&))
  ACE_UNIMPLEMENTED_FUNC (RPG_Net_Protocol_ParserDriver& operator= (const RPG_Net_Protocol_ParserDriver&))

  // implement Common_ILexScanner_T
  inline virtual const Common_FlexScannerState& getR () const { static Common_FlexScannerState dummy; ACE_ASSERT (false); ACE_NOTSUP_RETURN (dummy); ACE_NOTREACHED (return dummy;) }
  inline virtual const RPG_Net_Protocol_IParser* const getP () const { return this; }
  inline virtual void setP (RPG_Net_Protocol_IParser*) { ACE_ASSERT (false); ACE_NOTSUP; ACE_NOTREACHED (return;) }
  inline virtual void setDebug (yyscan_t state_in, bool toggle_in) { RPG_Net_Protocol_Scanner_set_debug ((toggle_in ? 1 : 0), state_in); }
  inline virtual void reset () { scannedBytes_ = 0; }
  virtual bool initialize (yyscan_t&, RPG_Net_Protocol_IParser*);
  inline virtual void finalize (yyscan_t& state_inout) { int result = RPG_Net_Protocol_Scanner_lex_destroy (state_inout); ACE_UNUSED_ARG (result); state_inout = NULL; }
  virtual struct yy_buffer_state* create (yyscan_t, // state handle
                                          char*,    // buffer handle
                                          size_t);  // buffer size
  inline virtual void destroy (yyscan_t state_in, struct yy_buffer_state*& buffer_inout) { RPG_Net_Protocol_Scanner__delete_buffer (buffer_inout, state_in); buffer_inout = NULL; }
  inline virtual bool lex (yyscan_t state_in) { ACE_ASSERT (false); ACE_NOTSUP_RETURN (false); ACE_NOTREACHED (return false;) }
};

#endif
