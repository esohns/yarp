// A Bison parser, made by GNU Bison 3.8.2.

// Skeleton implementation for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2015, 2018-2021 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// As a special exception, you may create a larger work that contains
// part or all of the Bison parser skeleton and distribute that work
// under terms of your choice, so long as that work isn't itself a
// parser generator using the skeleton or a modified version thereof
// as a parser skeleton.  Alternatively, if you modify or redistribute
// the parser skeleton itself, you may (at your option) remove this
// special exception, which will cause the skeleton and the resulting
// Bison output files to be licensed under the GNU General Public
// License without this special exception.

// This special exception was added by the Free Software Foundation in
// version 2.2 of Bison.

// DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
// especially those whose name start with YY_ or yy_.  They are
// private implementation details that can be changed or removed.

// "%code top" blocks.

#include "stdafx.h"


// Take the name prefix into account.
#define yylex   RPG_Net_Protocol_Scanner_lex



#include "rpg_net_protocol_parser.h"


// Unqualified %code blocks.

#include <string>

#include "ace/Basic_Types.h"
#include "ace/Log_Msg.h"

#include "rpg_common_macros.h"

#include "rpg_net_protocol_iparser.h"
#include "rpg_net_protocol_scanner.h"



#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> // FIXME: INFRINGES ON USER NAME SPACE.
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif


// Whether we are compiled with exception support.
#ifndef YY_EXCEPTIONS
# if defined __GNUC__ && !defined __EXCEPTIONS
#  define YY_EXCEPTIONS 0
# else
#  define YY_EXCEPTIONS 1
# endif
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K].location)
/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

# ifndef YYLLOC_DEFAULT
#  define YYLLOC_DEFAULT(Current, Rhs, N)                               \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).begin  = YYRHSLOC (Rhs, 1).begin;                   \
          (Current).end    = YYRHSLOC (Rhs, N).end;                     \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).begin = (Current).end = YYRHSLOC (Rhs, 0).end;      \
        }                                                               \
    while (false)
# endif


// Enable debugging if requested.
#if RPG_NET_PROTOCOL_SCANNER_DEBUG

// A pseudo ostream that takes yydebug_ into account.
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Symbol)         \
  do {                                          \
    if (yydebug_)                               \
    {                                           \
      *yycdebug_ << Title << ' ';               \
      yy_print_ (*yycdebug_, Symbol);           \
      *yycdebug_ << '\n';                       \
    }                                           \
  } while (false)

# define YY_REDUCE_PRINT(Rule)          \
  do {                                  \
    if (yydebug_)                       \
      yy_reduce_print_ (Rule);          \
  } while (false)

# define YY_STACK_PRINT()               \
  do {                                  \
    if (yydebug_)                       \
      yy_stack_print_ ();                \
  } while (false)

#else // !RPG_NET_PROTOCOL_SCANNER_DEBUG

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Symbol)  YY_USE (Symbol)
# define YY_REDUCE_PRINT(Rule)           static_cast<void> (0)
# define YY_STACK_PRINT()                static_cast<void> (0)

#endif // !RPG_NET_PROTOCOL_SCANNER_DEBUG

#define yyerrok         (yyerrstatus_ = 0)
#define yyclearin       (yyla.clear ())

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)

namespace yy {

  /// Build a parser object.
  RPG_Net_Protocol_Parser::RPG_Net_Protocol_Parser (RPG_Net_Protocol_IParser* driver_yyarg, yyscan_t yyscanner_yyarg)
#if RPG_NET_PROTOCOL_SCANNER_DEBUG
    : yydebug_ (false),
      yycdebug_ (&std::cerr),
#else
    :
#endif
      driver (driver_yyarg),
      yyscanner (yyscanner_yyarg)
  {}

  RPG_Net_Protocol_Parser::~RPG_Net_Protocol_Parser ()
  {}

  RPG_Net_Protocol_Parser::syntax_error::~syntax_error () YY_NOEXCEPT YY_NOTHROW
  {}

  /*---------.
  | symbol.  |
  `---------*/

  // basic_symbol.
  template <typename Base>
  RPG_Net_Protocol_Parser::basic_symbol<Base>::basic_symbol (const basic_symbol& that)
    : Base (that)
    , value (that.value)
    , location (that.location)
  {}


  /// Constructor for valueless symbols.
  template <typename Base>
  RPG_Net_Protocol_Parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, YY_MOVE_REF (location_type) l)
    : Base (t)
    , value ()
    , location (l)
  {}

  template <typename Base>
  RPG_Net_Protocol_Parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, YY_RVREF (value_type) v, YY_RVREF (location_type) l)
    : Base (t)
    , value (YY_MOVE (v))
    , location (YY_MOVE (l))
  {}


  template <typename Base>
  RPG_Net_Protocol_Parser::symbol_kind_type
  RPG_Net_Protocol_Parser::basic_symbol<Base>::type_get () const YY_NOEXCEPT
  {
    return this->kind ();
  }


  template <typename Base>
  bool
  RPG_Net_Protocol_Parser::basic_symbol<Base>::empty () const YY_NOEXCEPT
  {
    return this->kind () == symbol_kind::S_YYEMPTY;
  }

  template <typename Base>
  void
  RPG_Net_Protocol_Parser::basic_symbol<Base>::move (basic_symbol& s)
  {
    super_type::move (s);
    value = YY_MOVE (s.value);
    location = YY_MOVE (s.location);
  }

  // by_kind.
  RPG_Net_Protocol_Parser::by_kind::by_kind () YY_NOEXCEPT
    : kind_ (symbol_kind::S_YYEMPTY)
  {}

#if 201103L <= YY_CPLUSPLUS
  RPG_Net_Protocol_Parser::by_kind::by_kind (by_kind&& that) YY_NOEXCEPT
    : kind_ (that.kind_)
  {
    that.clear ();
  }
#endif

  RPG_Net_Protocol_Parser::by_kind::by_kind (const by_kind& that) YY_NOEXCEPT
    : kind_ (that.kind_)
  {}

  RPG_Net_Protocol_Parser::by_kind::by_kind (token_kind_type t) YY_NOEXCEPT
    : kind_ (yytranslate_ (t))
  {}



  void
  RPG_Net_Protocol_Parser::by_kind::clear () YY_NOEXCEPT
  {
    kind_ = symbol_kind::S_YYEMPTY;
  }

  void
  RPG_Net_Protocol_Parser::by_kind::move (by_kind& that)
  {
    kind_ = that.kind_;
    that.clear ();
  }

  RPG_Net_Protocol_Parser::symbol_kind_type
  RPG_Net_Protocol_Parser::by_kind::kind () const YY_NOEXCEPT
  {
    return kind_;
  }


  RPG_Net_Protocol_Parser::symbol_kind_type
  RPG_Net_Protocol_Parser::by_kind::type_get () const YY_NOEXCEPT
  {
    return this->kind ();
  }



  // by_state.
  RPG_Net_Protocol_Parser::by_state::by_state () YY_NOEXCEPT
    : state (empty_state)
  {}

  RPG_Net_Protocol_Parser::by_state::by_state (const by_state& that) YY_NOEXCEPT
    : state (that.state)
  {}

  void
  RPG_Net_Protocol_Parser::by_state::clear () YY_NOEXCEPT
  {
    state = empty_state;
  }

  void
  RPG_Net_Protocol_Parser::by_state::move (by_state& that)
  {
    state = that.state;
    that.clear ();
  }

  RPG_Net_Protocol_Parser::by_state::by_state (state_type s) YY_NOEXCEPT
    : state (s)
  {}

  RPG_Net_Protocol_Parser::symbol_kind_type
  RPG_Net_Protocol_Parser::by_state::kind () const YY_NOEXCEPT
  {
    if (state == empty_state)
      return symbol_kind::S_YYEMPTY;
    else
      return YY_CAST (symbol_kind_type, yystos_[+state]);
  }

  RPG_Net_Protocol_Parser::stack_symbol_type::stack_symbol_type ()
  {}

  RPG_Net_Protocol_Parser::stack_symbol_type::stack_symbol_type (YY_RVREF (stack_symbol_type) that)
    : super_type (YY_MOVE (that.state), YY_MOVE (that.value), YY_MOVE (that.location))
  {
#if 201103L <= YY_CPLUSPLUS
    // that is emptied.
    that.state = empty_state;
#endif
  }

  RPG_Net_Protocol_Parser::stack_symbol_type::stack_symbol_type (state_type s, YY_MOVE_REF (symbol_type) that)
    : super_type (s, YY_MOVE (that.value), YY_MOVE (that.location))
  {
    // that is emptied.
    that.kind_ = symbol_kind::S_YYEMPTY;
  }

#if YY_CPLUSPLUS < 201103L
  RPG_Net_Protocol_Parser::stack_symbol_type&
  RPG_Net_Protocol_Parser::stack_symbol_type::operator= (const stack_symbol_type& that)
  {
    state = that.state;
    value = that.value;
    location = that.location;
    return *this;
  }

  RPG_Net_Protocol_Parser::stack_symbol_type&
  RPG_Net_Protocol_Parser::stack_symbol_type::operator= (stack_symbol_type& that)
  {
    state = that.state;
    value = that.value;
    location = that.location;
    // that is emptied.
    that.state = empty_state;
    return *this;
  }
#endif

  template <typename Base>
  void
  RPG_Net_Protocol_Parser::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);

    // User destructor.
    switch (yysym.kind ())
    {
      case symbol_kind::S_LENGTH: // "length"
                    { (yysym.value.ival) = 0; }
        break;

      case symbol_kind::S_TYPE: // "type"
                    { (yysym.value.ival) = 0; }
        break;

      case symbol_kind::S_XML: // "xml"
                    { delete (yysym.value.sval); (yysym.value.sval) = NULL; }
        break;

      case symbol_kind::S_COMMAND: // "command"
                    { (yysym.value.ival) = 0; }
        break;

      case symbol_kind::S_POSITION_X: // "position_x"
                    { (yysym.value.ival) = 0; }
        break;

      case symbol_kind::S_POSITION_Y: // "position_y"
                    { (yysym.value.ival) = 0; }
        break;

      case symbol_kind::S_PATH_NEXT_XY: // "path_next_xy"
                    { (yysym.value.ival) = 0; }
        break;

      case symbol_kind::S_PATH_NEXT_DIRECTION: // "path_next_direction"
                    { (yysym.value.ival) = 0; }
        break;

      case symbol_kind::S_ENTITY_ID: // "entity_id"
                    { (yysym.value.ival) = 0; }
        break;

      case symbol_kind::S_PREVIOUS_X: // "previous_x"
                    { (yysym.value.ival) = 0; }
        break;

      case symbol_kind::S_PREVIOUS_Y: // "previous_y"
                    { (yysym.value.ival) = 0; }
        break;

      case symbol_kind::S_WINDOW: // "window"
                    { (yysym.value.ival) = 0; }
        break;

      case symbol_kind::S_CURSOR: // "cursor"
                    { (yysym.value.ival) = 0; }
        break;

      case symbol_kind::S_SOUND: // "sound"
                    { (yysym.value.ival) = 0; }
        break;

      case symbol_kind::S_MESSAGE: // "message"
                    { delete (yysym.value.sval); (yysym.value.sval) = NULL; }
        break;

      case symbol_kind::S_SOURCE_X: // "source_x"
                    { (yysym.value.ival) = 0; }
        break;

      case symbol_kind::S_SOURCE_Y: // "source_y"
                    { (yysym.value.ival) = 0; }
        break;

      case symbol_kind::S_POSITIONS_NEXT_XY: // "positions_next_xy"
                    { (yysym.value.ival) = 0; }
        break;

      case symbol_kind::S_RADIUS: // "radius"
                    { (yysym.value.ival) = 0; }
        break;

      case symbol_kind::S_END_OF_COMMAND: // "end_of_command"
                    { (yysym.value.ival) = 0; }
        break;

      case symbol_kind::S_END_OF_FRAME: // "end_of_frame"
                    { (yysym.value.ival) = 0; }
        break;

      case symbol_kind::S_frame: // frame
                    { (yysym.value.ival) = 0; }
        break;

      default:
        break;
    }
  }

#if RPG_NET_PROTOCOL_SCANNER_DEBUG
  template <typename Base>
  void
  RPG_Net_Protocol_Parser::yy_print_ (std::ostream& yyo, const basic_symbol<Base>& yysym) const
  {
    std::ostream& yyoutput = yyo;
    YY_USE (yyoutput);
    if (yysym.empty ())
      yyo << "empty symbol";
    else
      {
        symbol_kind_type yykind = yysym.kind ();
        yyo << (yykind < YYNTOKENS ? "token" : "nterm")
            << ' ' << yysym.name () << " ("
            << yysym.location << ": ";
        switch (yykind)
    {
      case symbol_kind::S_LENGTH: // "length"
                    { debug_stream() << (yysym.value.ival); }
        break;

      case symbol_kind::S_TYPE: // "type"
                    { debug_stream() << (yysym.value.ival); }
        break;

      case symbol_kind::S_XML: // "xml"
                    { debug_stream() << *(yysym.value.sval); }
        break;

      case symbol_kind::S_COMMAND: // "command"
                    { debug_stream() << (yysym.value.ival); }
        break;

      case symbol_kind::S_POSITION_X: // "position_x"
                    { debug_stream() << (yysym.value.ival); }
        break;

      case symbol_kind::S_POSITION_Y: // "position_y"
                    { debug_stream() << (yysym.value.ival); }
        break;

      case symbol_kind::S_PATH_NEXT_XY: // "path_next_xy"
                    { debug_stream() << (yysym.value.ival); }
        break;

      case symbol_kind::S_PATH_NEXT_DIRECTION: // "path_next_direction"
                    { debug_stream() << (yysym.value.ival); }
        break;

      case symbol_kind::S_ENTITY_ID: // "entity_id"
                    { debug_stream() << (yysym.value.ival); }
        break;

      case symbol_kind::S_PREVIOUS_X: // "previous_x"
                    { debug_stream() << (yysym.value.ival); }
        break;

      case symbol_kind::S_PREVIOUS_Y: // "previous_y"
                    { debug_stream() << (yysym.value.ival); }
        break;

      case symbol_kind::S_WINDOW: // "window"
                    { debug_stream() << (yysym.value.ival); }
        break;

      case symbol_kind::S_CURSOR: // "cursor"
                    { debug_stream() << (yysym.value.ival); }
        break;

      case symbol_kind::S_SOUND: // "sound"
                    { debug_stream() << (yysym.value.ival); }
        break;

      case symbol_kind::S_MESSAGE: // "message"
                    { debug_stream() << *(yysym.value.sval); }
        break;

      case symbol_kind::S_SOURCE_X: // "source_x"
                    { debug_stream() << (yysym.value.ival); }
        break;

      case symbol_kind::S_SOURCE_Y: // "source_y"
                    { debug_stream() << (yysym.value.ival); }
        break;

      case symbol_kind::S_POSITIONS_NEXT_XY: // "positions_next_xy"
                    { debug_stream() << (yysym.value.ival); }
        break;

      case symbol_kind::S_RADIUS: // "radius"
                    { debug_stream() << (yysym.value.ival); }
        break;

      case symbol_kind::S_END_OF_COMMAND: // "end_of_command"
                    { debug_stream() << (yysym.value.ival); }
        break;

      case symbol_kind::S_END_OF_FRAME: // "end_of_frame"
                    { debug_stream() << (yysym.value.ival); }
        break;

      case symbol_kind::S_frame: // frame
                    { debug_stream() << (yysym.value.ival); }
        break;

      default:
        break;
    }
        yyo << ')';
      }
  }
#endif

  void
  RPG_Net_Protocol_Parser::yypush_ (const char* m, YY_MOVE_REF (stack_symbol_type) sym)
  {
    if (m)
      YY_SYMBOL_PRINT (m, sym);
    yystack_.push (YY_MOVE (sym));
  }

  void
  RPG_Net_Protocol_Parser::yypush_ (const char* m, state_type s, YY_MOVE_REF (symbol_type) sym)
  {
#if 201103L <= YY_CPLUSPLUS
    yypush_ (m, stack_symbol_type (s, std::move (sym)));
#else
    stack_symbol_type ss (s, sym);
    yypush_ (m, ss);
#endif
  }

  void
  RPG_Net_Protocol_Parser::yypop_ (int n) YY_NOEXCEPT
  {
    yystack_.pop (n);
  }

#if RPG_NET_PROTOCOL_SCANNER_DEBUG
  std::ostream&
  RPG_Net_Protocol_Parser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  RPG_Net_Protocol_Parser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  RPG_Net_Protocol_Parser::debug_level_type
  RPG_Net_Protocol_Parser::debug_level () const
  {
    return yydebug_;
  }

  void
  RPG_Net_Protocol_Parser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif // RPG_NET_PROTOCOL_SCANNER_DEBUG

  RPG_Net_Protocol_Parser::state_type
  RPG_Net_Protocol_Parser::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - YYNTOKENS] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - YYNTOKENS];
  }

  bool
  RPG_Net_Protocol_Parser::yy_pact_value_is_default_ (int yyvalue) YY_NOEXCEPT
  {
    return yyvalue == yypact_ninf_;
  }

  bool
  RPG_Net_Protocol_Parser::yy_table_value_is_error_ (int yyvalue) YY_NOEXCEPT
  {
    return yyvalue == yytable_ninf_;
  }

  int
  RPG_Net_Protocol_Parser::operator() ()
  {
    return parse ();
  }

  int
  RPG_Net_Protocol_Parser::parse ()
  {
    int yyn;
    /// Length of the RHS of the rule being reduced.
    int yylen = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// The lookahead symbol.
    symbol_type yyla;

    /// The locations where the error started and ended.
    stack_symbol_type yyerror_range[3];

    /// The return value of parse ().
    int yyresult;

#if YY_EXCEPTIONS
    try
#endif // YY_EXCEPTIONS
      {
    YYCDEBUG << "Starting parse\n";


    // User initialization code.
{
  // Initialize the initial location
  //@$.begin.filename = @$.end.filename = &driver->file;

  // initialize the token value container
  // $$.ival = 0;
  yyla.value.sval = NULL;
}



    /* Initialize the stack.  The initial state will be set in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystack_.clear ();
    yypush_ (YY_NULLPTR, 0, YY_MOVE (yyla));

  /*-----------------------------------------------.
  | yynewstate -- push a new symbol on the stack.  |
  `-----------------------------------------------*/
  yynewstate:
    YYCDEBUG << "Entering state " << int (yystack_[0].state) << '\n';
    YY_STACK_PRINT ();

    // Accept?
    if (yystack_[0].state == yyfinal_)
      YYACCEPT;

    goto yybackup;


  /*-----------.
  | yybackup.  |
  `-----------*/
  yybackup:
    // Try to take a decision without lookahead.
    yyn = yypact_[+yystack_[0].state];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    // Read a lookahead token.
    if (yyla.empty ())
      {
        YYCDEBUG << "Reading a token\n";
#if YY_EXCEPTIONS
        try
#endif // YY_EXCEPTIONS
          {
            yyla.kind_ = yytranslate_ (yylex (&yyla.value, &yyla.location, driver, yyscanner));
          }
#if YY_EXCEPTIONS
        catch (const syntax_error& yyexc)
          {
            YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
            error (yyexc);
            goto yyerrlab1;
          }
#endif // YY_EXCEPTIONS
      }
    YY_SYMBOL_PRINT ("Next token is", yyla);

    if (yyla.kind () == symbol_kind::S_YYerror)
    {
      // The scanner already issued an error message, process directly
      // to error recovery.  But do not keep the error token as
      // lookahead, it is too special and may lead us to an endless
      // loop in error recovery. */
      yyla.kind_ = symbol_kind::S_YYUNDEF;
      goto yyerrlab1;
    }

    /* If the proper action on seeing token YYLA.TYPE is to reduce or
       to detect an error, take that action.  */
    yyn += yyla.kind ();
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.kind ())
      {
        goto yydefault;
      }

    // Reduce or error.
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
        if (yy_table_value_is_error_ (yyn))
          goto yyerrlab;
        yyn = -yyn;
        goto yyreduce;
      }

    // Count tokens shifted since error; after three, turn off error status.
    if (yyerrstatus_)
      --yyerrstatus_;

    // Shift the lookahead token.
    yypush_ ("Shifting", state_type (yyn), YY_MOVE (yyla));
    goto yynewstate;


  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[+yystack_[0].state];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;


  /*-----------------------------.
  | yyreduce -- do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    {
      stack_symbol_type yylhs;
      yylhs.state = yy_lr_goto_state_ (yystack_[yylen].state, yyr1_[yyn]);
      /* If YYLEN is nonzero, implement the default value of the
         action: '$$ = $1'.  Otherwise, use the top of the stack.

         Otherwise, the following line sets YYLHS.VALUE to garbage.
         This behavior is undocumented and Bison users should not rely
         upon it.  */
      if (yylen)
        yylhs.value = yystack_[yylen - 1].value;
      else
        yylhs.value = yystack_[0].value;

      // Default location.
      {
        stack_type::slice range (yystack_, yylen);
        YYLLOC_DEFAULT (yylhs.location, range, yylen);
        yyerror_range[1].location = yylhs.location;
      }

      // Perform the reduction.
      YY_REDUCE_PRINT (yyn);
#if YY_EXCEPTIONS
      try
#endif // YY_EXCEPTIONS
        {
          switch (yyn)
            {
  case 2: // $@1: %empty
                                                     { driver->length (static_cast<ACE_UINT32> ((yystack_[0].value.ival)));
                                                     }
    break;

  case 3: // frame: "length" $@1 commands "end_of_frame"
                                                     { (yylhs.value.ival) = driver->length ();
                                                       ACE_UNUSED_ARG ((yystack_[0].value.ival));
                                                       YYACCEPT; }
    break;

  case 6: // path_elem: "path_next_xy" "path_next_xy" "path_next_direction"
                                                                {
                                                       driver->current ().path.push_back (std::make_pair (std::make_pair (static_cast<unsigned int> ((yystack_[2].value.ival)), static_cast<unsigned int> ((yystack_[1].value.ival))), static_cast<enum RPG_Map_Direction> ((yystack_[0].value.ival))));
                                                     }
    break;

  case 9: // positions_elem: "positions_next_xy" "positions_next_xy"
                                                        {
                                                       driver->current ().positions.insert (std::make_pair (static_cast<unsigned int> ((yystack_[1].value.ival)), static_cast<unsigned int> ((yystack_[0].value.ival))));
                                                     }
    break;

  case 10: // xml: "xml"
                                                     { driver->current ().xml = *(yystack_[0].value.sval); }
    break;

  case 12: // message: "message"
                                                     { driver->current ().message = *(yystack_[0].value.sval); }
    break;

  case 14: // $@2: %empty
                                                     { driver->current ().type = static_cast<enum RPG_Net_Protocol_MessageType> ((yystack_[0].value.ival));
                                                     }
    break;

  case 15: // $@3: %empty
                                                     { driver->current ().command = (yystack_[0].value.ival);
                                                     }
    break;

  case 16: // $@4: %empty
                                                     { driver->current ().position.first = static_cast<unsigned int> ((yystack_[1].value.ival));
                                                       driver->current ().position.second = static_cast<unsigned int> ((yystack_[0].value.ival));
                                                     }
    break;

  case 17: // $@5: %empty
                                                     { driver->current ().entity_id = static_cast<RPG_Engine_EntityID_t> ((yystack_[0].value.ival));
                                                     }
    break;

  case 18: // $@6: %empty
                                                     { driver->current ().previous.first = static_cast<unsigned int> ((yystack_[1].value.ival));
                                                       driver->current ().previous.second = static_cast<unsigned int> ((yystack_[0].value.ival));
                                                     }
    break;

  case 19: // $@7: %empty
                                                     { driver->current ().window = static_cast<enum RPG_Graphics_WindowType> ((yystack_[0].value.ival));
                                                     }
    break;

  case 20: // $@8: %empty
                                                     { driver->current ().cursor = static_cast<enum RPG_Graphics_Cursor> ((yystack_[0].value.ival));
                                                     }
    break;

  case 21: // $@9: %empty
                                                     { driver->current ().sound = static_cast<enum RPG_Sound_Event> ((yystack_[0].value.ival));
                                                     }
    break;

  case 22: // $@10: %empty
                                                     { driver->current ().source.first = static_cast<unsigned int> ((yystack_[1].value.ival));
                                                       driver->current ().source.second = static_cast<unsigned int> ((yystack_[0].value.ival));
                                                     }
    break;

  case 23: // $@11: %empty
                                                     { driver->current ().radius = (yystack_[0].value.ival);
                                                     }
    break;

  case 24: // command: "type" $@2 xml "command" $@3 "position_x" "position_y" $@4 path "entity_id" $@5 "previous_x" "previous_y" $@6 "window" $@7 "cursor" $@8 "sound" $@9 message "source_x" "source_y" $@10 positions "radius" $@11 "end_of_command"
                                                     { ACE_UNUSED_ARG ((yystack_[0].value.ival));
                                                       struct RPG_Net_Protocol_Command* current_p = &driver->current ();
                                                       driver->record (current_p);
                                                       if (driver->scannedBytes () == driver->length ())
                                                         YYACCEPT; // *NOTE*: "end_of_frame" is currently never reached
                                                     }
    break;



            default:
              break;
            }
        }
#if YY_EXCEPTIONS
      catch (const syntax_error& yyexc)
        {
          YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
          error (yyexc);
          YYERROR;
        }
#endif // YY_EXCEPTIONS
      YY_SYMBOL_PRINT ("-> $$ =", yylhs);
      yypop_ (yylen);
      yylen = 0;

      // Shift the result of the reduction.
      yypush_ (YY_NULLPTR, YY_MOVE (yylhs));
    }
    goto yynewstate;


  /*--------------------------------------.
  | yyerrlab -- here on detecting error.  |
  `--------------------------------------*/
  yyerrlab:
    // If not already recovering from an error, report this error.
    if (!yyerrstatus_)
      {
        ++yynerrs_;
        context yyctx (*this, yyla);
        std::string msg = yysyntax_error_ (yyctx);
        error (yyla.location, YY_MOVE (msg));
      }


    yyerror_range[1].location = yyla.location;
    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */

        // Return failure if at end of input.
        if (yyla.kind () == symbol_kind::S_YYEOF)
          YYABORT;
        else if (!yyla.empty ())
          {
            yy_destroy_ ("Error: discarding", yyla);
            yyla.clear ();
          }
      }

    // Else will try to reuse lookahead token after shifting the error token.
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:
    /* Pacify compilers when the user code never invokes YYERROR and
       the label yyerrorlab therefore never appears in user code.  */
    if (false)
      YYERROR;

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    YY_STACK_PRINT ();
    goto yyerrlab1;


  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;   // Each real token shifted decrements this.
    // Pop stack until we find a state that shifts the error token.
    for (;;)
      {
        yyn = yypact_[+yystack_[0].state];
        if (!yy_pact_value_is_default_ (yyn))
          {
            yyn += symbol_kind::S_YYerror;
            if (0 <= yyn && yyn <= yylast_
                && yycheck_[yyn] == symbol_kind::S_YYerror)
              {
                yyn = yytable_[yyn];
                if (0 < yyn)
                  break;
              }
          }

        // Pop the current state because it cannot handle the error token.
        if (yystack_.size () == 1)
          YYABORT;

        yyerror_range[1].location = yystack_[0].location;
        yy_destroy_ ("Error: popping", yystack_[0]);
        yypop_ ();
        YY_STACK_PRINT ();
      }
    {
      stack_symbol_type error_token;

      yyerror_range[2].location = yyla.location;
      YYLLOC_DEFAULT (error_token.location, yyerror_range, 2);

      // Shift the error token.
      error_token.state = state_type (yyn);
      yypush_ ("Shifting", YY_MOVE (error_token));
    }
    goto yynewstate;


  /*-------------------------------------.
  | yyacceptlab -- YYACCEPT comes here.  |
  `-------------------------------------*/
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;


  /*-----------------------------------.
  | yyabortlab -- YYABORT comes here.  |
  `-----------------------------------*/
  yyabortlab:
    yyresult = 1;
    goto yyreturn;


  /*-----------------------------------------------------.
  | yyreturn -- parsing is finished, return the result.  |
  `-----------------------------------------------------*/
  yyreturn:
    if (!yyla.empty ())
      yy_destroy_ ("Cleanup: discarding lookahead", yyla);

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    YY_STACK_PRINT ();
    while (1 < yystack_.size ())
      {
        yy_destroy_ ("Cleanup: popping", yystack_[0]);
        yypop_ ();
      }

    return yyresult;
  }
#if YY_EXCEPTIONS
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack\n";
        // Do not try to display the values of the reclaimed symbols,
        // as their printers might throw an exception.
        if (!yyla.empty ())
          yy_destroy_ (YY_NULLPTR, yyla);

        while (1 < yystack_.size ())
          {
            yy_destroy_ (YY_NULLPTR, yystack_[0]);
            yypop_ ();
          }
        throw;
      }
#endif // YY_EXCEPTIONS
  }

  void
  RPG_Net_Protocol_Parser::error (const syntax_error& yyexc)
  {
    error (yyexc.location, yyexc.what ());
  }

  /* Return YYSTR after stripping away unnecessary quotes and
     backslashes, so that it's suitable for yyerror.  The heuristic is
     that double-quoting is unnecessary unless the string contains an
     apostrophe, a comma, or backslash (other than backslash-backslash).
     YYSTR is taken from yytname.  */
  std::string
  RPG_Net_Protocol_Parser::yytnamerr_ (const char *yystr)
  {
    if (*yystr == '"')
      {
        std::string yyr;
        char const *yyp = yystr;

        for (;;)
          switch (*++yyp)
            {
            case '\'':
            case ',':
              goto do_not_strip_quotes;

            case '\\':
              if (*++yyp != '\\')
                goto do_not_strip_quotes;
              else
                goto append;

            append:
            default:
              yyr += *yyp;
              break;

            case '"':
              return yyr;
            }
      do_not_strip_quotes: ;
      }

    return yystr;
  }

  std::string
  RPG_Net_Protocol_Parser::symbol_name (symbol_kind_type yysymbol)
  {
    return yytnamerr_ (yytname_[yysymbol]);
  }



  // RPG_Net_Protocol_Parser::context.
  RPG_Net_Protocol_Parser::context::context (const RPG_Net_Protocol_Parser& yyparser, const symbol_type& yyla)
    : yyparser_ (yyparser)
    , yyla_ (yyla)
  {}

  int
  RPG_Net_Protocol_Parser::context::expected_tokens (symbol_kind_type yyarg[], int yyargn) const
  {
    // Actual number of expected tokens
    int yycount = 0;

    const int yyn = yypact_[+yyparser_.yystack_[0].state];
    if (!yy_pact_value_is_default_ (yyn))
      {
        /* Start YYX at -YYN if negative to avoid negative indexes in
           YYCHECK.  In other words, skip the first -YYN actions for
           this state because they are default actions.  */
        const int yyxbegin = yyn < 0 ? -yyn : 0;
        // Stay within bounds of both yycheck and yytname.
        const int yychecklim = yylast_ - yyn + 1;
        const int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
        for (int yyx = yyxbegin; yyx < yyxend; ++yyx)
          if (yycheck_[yyx + yyn] == yyx && yyx != symbol_kind::S_YYerror
              && !yy_table_value_is_error_ (yytable_[yyx + yyn]))
            {
              if (!yyarg)
                ++yycount;
              else if (yycount == yyargn)
                return 0;
              else
                yyarg[yycount++] = YY_CAST (symbol_kind_type, yyx);
            }
      }

    if (yyarg && yycount == 0 && 0 < yyargn)
      yyarg[0] = symbol_kind::S_YYEMPTY;
    return yycount;
  }






  int
  RPG_Net_Protocol_Parser::yy_syntax_error_arguments_ (const context& yyctx,
                                                 symbol_kind_type yyarg[], int yyargn) const
  {
    /* There are many possibilities here to consider:
       - If this state is a consistent state with a default action, then
         the only way this function was invoked is if the default action
         is an error action.  In that case, don't check for expected
         tokens because there are none.
       - The only way there can be no lookahead present (in yyla) is
         if this state is a consistent state with a default action.
         Thus, detecting the absence of a lookahead is sufficient to
         determine that there is no unexpected or expected token to
         report.  In that case, just report a simple "syntax error".
       - Don't assume there isn't a lookahead just because this state is
         a consistent state with a default action.  There might have
         been a previous inconsistent state, consistent state with a
         non-default action, or user semantic action that manipulated
         yyla.  (However, yyla is currently not documented for users.)
       - Of course, the expected token list depends on states to have
         correct lookahead information, and it depends on the parser not
         to perform extra reductions after fetching a lookahead from the
         scanner and before detecting a syntax error.  Thus, state merging
         (from LALR or IELR) and default reductions corrupt the expected
         token list.  However, the list is correct for canonical LR with
         one exception: it will still contain any token that will not be
         accepted due to an error action in a later state.
    */

    if (!yyctx.lookahead ().empty ())
      {
        if (yyarg)
          yyarg[0] = yyctx.token ();
        int yyn = yyctx.expected_tokens (yyarg ? yyarg + 1 : yyarg, yyargn - 1);
        return yyn + 1;
      }
    return 0;
  }

  // Generate an error message.
  std::string
  RPG_Net_Protocol_Parser::yysyntax_error_ (const context& yyctx) const
  {
    // Its maximum.
    enum { YYARGS_MAX = 5 };
    // Arguments of yyformat.
    symbol_kind_type yyarg[YYARGS_MAX];
    int yycount = yy_syntax_error_arguments_ (yyctx, yyarg, YYARGS_MAX);

    char const* yyformat = YY_NULLPTR;
    switch (yycount)
      {
#define YYCASE_(N, S)                         \
        case N:                               \
          yyformat = S;                       \
        break
      default: // Avoid compiler warnings.
        YYCASE_ (0, YY_("syntax error"));
        YYCASE_ (1, YY_("syntax error, unexpected %s"));
        YYCASE_ (2, YY_("syntax error, unexpected %s, expecting %s"));
        YYCASE_ (3, YY_("syntax error, unexpected %s, expecting %s or %s"));
        YYCASE_ (4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
        YYCASE_ (5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
      }

    std::string yyres;
    // Argument number.
    std::ptrdiff_t yyi = 0;
    for (char const* yyp = yyformat; *yyp; ++yyp)
      if (yyp[0] == '%' && yyp[1] == 's' && yyi < yycount)
        {
          yyres += symbol_name (yyarg[yyi++]);
          ++yyp;
        }
      else
        yyres += *yyp;
    return yyres;
  }


  const signed char RPG_Net_Protocol_Parser::yypact_ninf_ = -17;

  const signed char RPG_Net_Protocol_Parser::yytable_ninf_ = -1;

  const signed char
  RPG_Net_Protocol_Parser::yypact_[] =
  {
      -1,   -17,     6,   -17,   -17,    -4,   -17,   -17,   -17,     2,
     -17,     3,   -17,     1,     4,   -17,   -17,    -8,     5,   -17,
     -17,     0,     8,   -17,    -2,   -17,     7,   -17,     9,   -17,
      -3,   -17,    10,   -17,    11,    12,   -17,   -17,   -16,    -5,
     -17,   -17,   -17,    -6,   -17
  };

  const signed char
  RPG_Net_Protocol_Parser::yydefact_[] =
  {
       0,     2,     0,    26,     1,     0,    14,     3,    25,    11,
      10,     0,    15,     0,     0,    16,     5,     0,     0,    17,
       4,     0,     0,     6,     0,    18,     0,    19,     0,    20,
       0,    21,    13,    12,     0,     0,    22,     8,     0,     0,
      23,     7,     9,     0,    24
  };

  const signed char
  RPG_Net_Protocol_Parser::yypgoto_[] =
  {
     -17,   -17,   -17,   -17,   -17,   -17,   -17,   -17,   -17,   -17,
     -17,   -17,   -17,   -17,   -17,   -17,   -17,   -17,   -17,   -17,
     -17
  };

  const signed char
  RPG_Net_Protocol_Parser::yydefgoto_[] =
  {
       0,     2,     3,    17,    20,    38,    41,    11,    34,     8,
       9,    13,    16,    22,    26,    28,    30,    32,    37,    43,
       5
  };

  const signed char
  RPG_Net_Protocol_Parser::yytable_[] =
  {
       6,    18,     1,    19,    39,    40,     4,    10,    14,    12,
      23,    25,    15,    31,    21,    42,    44,     0,     0,     7,
      24,    27,     0,     0,    29,     0,     0,    33,     0,    35,
       0,    36
  };

  const signed char
  RPG_Net_Protocol_Parser::yycheck_[] =
  {
       4,     9,     3,    11,    20,    21,     0,     5,     7,     6,
      10,    13,     8,    16,     9,    20,    22,    -1,    -1,    23,
      12,    14,    -1,    -1,    15,    -1,    -1,    17,    -1,    18,
      -1,    19
  };

  const signed char
  RPG_Net_Protocol_Parser::yystos_[] =
  {
       0,     3,    25,    26,     0,    44,     4,    23,    33,    34,
       5,    31,     6,    35,     7,     8,    36,    27,     9,    11,
      28,     9,    37,    10,    12,    13,    38,    14,    39,    15,
      40,    16,    41,    17,    32,    18,    19,    42,    29,    20,
      21,    30,    20,    43,    22
  };

  const signed char
  RPG_Net_Protocol_Parser::yyr1_[] =
  {
       0,    24,    26,    25,    27,    27,    28,    29,    29,    30,
      31,    31,    32,    32,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    33,    44,    44
  };

  const signed char
  RPG_Net_Protocol_Parser::yyr2_[] =
  {
       0,     2,     0,     4,     2,     0,     3,     2,     0,     2,
       1,     0,     1,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    28,     2,     0
  };


#if RPG_NET_PROTOCOL_SCANNER_DEBUG || 1
  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a YYNTOKENS, nonterminals.
  const char*
  const RPG_Net_Protocol_Parser::yytname_[] =
  {
  "\"end of file\"", "error", "\"invalid token\"", "\"length\"",
  "\"type\"", "\"xml\"", "\"command\"", "\"position_x\"", "\"position_y\"",
  "\"path_next_xy\"", "\"path_next_direction\"", "\"entity_id\"",
  "\"previous_x\"", "\"previous_y\"", "\"window\"", "\"cursor\"",
  "\"sound\"", "\"message\"", "\"source_x\"", "\"source_y\"",
  "\"positions_next_xy\"", "\"radius\"", "\"end_of_command\"",
  "\"end_of_frame\"", "$accept", "frame", "$@1", "path", "path_elem",
  "positions", "positions_elem", "xml", "message", "command", "$@2", "$@3",
  "$@4", "$@5", "$@6", "$@7", "$@8", "$@9", "$@10", "$@11", "commands", YY_NULLPTR
  };
#endif


#if RPG_NET_PROTOCOL_SCANNER_DEBUG
  const unsigned char
  RPG_Net_Protocol_Parser::yyrline_[] =
  {
       0,   103,   103,   103,   108,   109,   110,   113,   114,   115,
     118,   119,   120,   121,   122,   124,   126,   129,   131,   134,
     136,   138,   140,   143,   122,   151,   152
  };

  void
  RPG_Net_Protocol_Parser::yy_stack_print_ () const
  {
    *yycdebug_ << "Stack now";
    for (stack_type::const_iterator
           i = yystack_.begin (),
           i_end = yystack_.end ();
         i != i_end; ++i)
      *yycdebug_ << ' ' << int (i->state);
    *yycdebug_ << '\n';
  }

  void
  RPG_Net_Protocol_Parser::yy_reduce_print_ (int yyrule) const
  {
    int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    // Print the symbols being reduced, and their result.
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
               << " (line " << yylno << "):\n";
    // The symbols being reduced.
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
                       yystack_[(yynrhs) - (yyi + 1)]);
  }
#endif // RPG_NET_PROTOCOL_SCANNER_DEBUG

  RPG_Net_Protocol_Parser::symbol_kind_type
  RPG_Net_Protocol_Parser::yytranslate_ (int t) YY_NOEXCEPT
  {
    // YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to
    // TOKEN-NUM as returned by yylex.
    static
    const signed char
    translate_table[] =
    {
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23
    };
    // Last valid token kind.
    const int code_max = 278;

    if (t <= 0)
      return symbol_kind::S_YYEOF;
    else if (t <= code_max)
      return static_cast <symbol_kind_type> (translate_table[t]);
    else
      return symbol_kind::S_YYUNDEF;
  }

} // yy



void
yy::RPG_Net_Protocol_Parser::error (const location_type& location_in,
                                    const std::string& message_in)
{
  RPG_TRACE (ACE_TEXT ("RPG_Net_Protocol_Parser::error"));

  driver->error (location_in, message_in);
}
