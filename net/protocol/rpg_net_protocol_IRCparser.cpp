
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison LALR(1) parsers in C++
   
      Copyright (C) 2002, 2003, 2004, 2005, 2006, 2007, 2008 Free Software
   Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */


/* First part of user declarations.  */




#include "rpg_net_protocol_IRCparser.h"

/* User implementation prologue.  */


/* Unqualified %code blocks.  */


#include "rpg_net_protocol_IRCparser_driver.h"
#include "rpg_net_protocol_IRCmessage.h"
#include <ace/OS_Memory.h>
#include <ace/Log_Msg.h>
#include <string>




#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* FIXME: INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#define YYUSE(e) ((void) (e))

/* Enable debugging if requested.  */
#if YYDEBUG

/* A pseudo ostream that takes yydebug_ into account.  */
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)	\
do {							\
  if (yydebug_)						\
    {							\
      *yycdebug_ << Title << ' ';			\
      yy_symbol_print_ ((Type), (Value), (Location));	\
      *yycdebug_ << std::endl;				\
    }							\
} while (false)

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug_)				\
    yy_reduce_print_ (Rule);		\
} while (false)

# define YY_STACK_PRINT()		\
do {					\
  if (yydebug_)				\
    yystack_print_ ();			\
} while (false)

#else /* !YYDEBUG */

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_REDUCE_PRINT(Rule)
# define YY_STACK_PRINT()

#endif /* !YYDEBUG */

#define yyerrok		(yyerrstatus_ = 0)
#define yyclearin	(yychar = yyempty_)

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)



namespace yy {

#if YYERROR_VERBOSE

  /* Return YYSTR after stripping away unnecessary quotes and
     backslashes, so that it's suitable for yyerror.  The heuristic is
     that double-quoting is unnecessary unless the string contains an
     apostrophe, a comma, or backslash (other than backslash-backslash).
     YYSTR is taken from yytname.  */
  std::string
  RPG_Net_Protocol_IRCParser::yytnamerr_ (const char *yystr)
  {
    if (*yystr == '"')
      {
        std::string yyr = "";
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
              /* Fall through.  */
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

#endif

  /// Build a parser object.
  RPG_Net_Protocol_IRCParser::RPG_Net_Protocol_IRCParser (RPG_Net_Protocol_IRCParserDriver& driver_yyarg, unsigned long& messageCount_yyarg, std::string& memory_yyarg, yyscan_t& context_yyarg)
    :
#if YYDEBUG
      yydebug_ (false),
      yycdebug_ (&std::cerr),
#endif
      driver (driver_yyarg),
      messageCount (messageCount_yyarg),
      memory (memory_yyarg),
      context (context_yyarg)
  {
  }

  RPG_Net_Protocol_IRCParser::~RPG_Net_Protocol_IRCParser ()
  {
  }

#if YYDEBUG
  /*--------------------------------.
  | Print this symbol on YYOUTPUT.  |
  `--------------------------------*/

  inline void
  RPG_Net_Protocol_IRCParser::yy_symbol_value_print_ (int yytype,
			   const semantic_type* yyvaluep, const location_type* yylocationp)
  {
    YYUSE (yylocationp);
    YYUSE (yyvaluep);
    switch (yytype)
      {
        case 3: /* "\"space\"" */

	{ debug_stream() << (yyvaluep->ival); };

	break;
      case 4: /* "\"origin\"" */

	{ debug_stream() << *(yyvaluep->sval); };

	break;
      case 5: /* "\"user\"" */

	{ debug_stream() << *(yyvaluep->sval); };

	break;
      case 6: /* "\"host\"" */

	{ debug_stream() << *(yyvaluep->sval); };

	break;
      case 7: /* "\"cmd_string\"" */

	{ debug_stream() << *(yyvaluep->sval); };

	break;
      case 8: /* "\"cmd_numeric\"" */

	{ debug_stream() << (yyvaluep->ival); };

	break;
      case 9: /* "\"param\"" */

	{ debug_stream() << *(yyvaluep->sval); };

	break;
       default:
	  break;
      }
  }


  void
  RPG_Net_Protocol_IRCParser::yy_symbol_print_ (int yytype,
			   const semantic_type* yyvaluep, const location_type* yylocationp)
  {
    *yycdebug_ << (yytype < yyntokens_ ? "token" : "nterm")
	       << ' ' << yytname_[yytype] << " ("
	       << *yylocationp << ": ";
    yy_symbol_value_print_ (yytype, yyvaluep, yylocationp);
    *yycdebug_ << ')';
  }
#endif

  void
  RPG_Net_Protocol_IRCParser::yydestruct_ (const char* yymsg,
			   int yytype, semantic_type* yyvaluep, location_type* yylocationp)
  {
    YYUSE (yylocationp);
    YYUSE (yymsg);
    YYUSE (yyvaluep);

    YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

    switch (yytype)
      {
        case 3: /* "\"space\"" */

	{ (yyvaluep->ival) = 0; };

	break;
      case 4: /* "\"origin\"" */

	{ delete (yyvaluep->sval); (yyvaluep->sval) = NULL; };

	break;
      case 5: /* "\"user\"" */

	{ delete (yyvaluep->sval); (yyvaluep->sval) = NULL; };

	break;
      case 6: /* "\"host\"" */

	{ delete (yyvaluep->sval); (yyvaluep->sval) = NULL; };

	break;
      case 7: /* "\"cmd_string\"" */

	{ delete (yyvaluep->sval); (yyvaluep->sval) = NULL; };

	break;
      case 8: /* "\"cmd_numeric\"" */

	{ (yyvaluep->ival) = 0; };

	break;
      case 9: /* "\"param\"" */

	{ delete (yyvaluep->sval); (yyvaluep->sval) = NULL; };

	break;

	default:
	  break;
      }
  }

  void
  RPG_Net_Protocol_IRCParser::yypop_ (unsigned int n)
  {
    yystate_stack_.pop (n);
    yysemantic_stack_.pop (n);
    yylocation_stack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  RPG_Net_Protocol_IRCParser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  RPG_Net_Protocol_IRCParser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  RPG_Net_Protocol_IRCParser::debug_level_type
  RPG_Net_Protocol_IRCParser::debug_level () const
  {
    return yydebug_;
  }

  void
  RPG_Net_Protocol_IRCParser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif

  int
  RPG_Net_Protocol_IRCParser::parse ()
  {
    /// Lookahead and lookahead in internal form.
    int yychar = yyempty_;
    int yytoken = 0;

    /* State.  */
    int yyn;
    int yylen = 0;
    int yystate = 0;

    /* Error handling.  */
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// Semantic value of the lookahead.
    semantic_type yylval;
    /// Location of the lookahead.
    location_type yylloc;
    /// The locations where the error started and ended.
    location_type yyerror_range[2];

    /// $$.
    semantic_type yyval;
    /// @$.
    location_type yyloc;

    int yyresult;

    YYCDEBUG << "Starting parse" << std::endl;


    /* User initialization code.  */
    
{
  // initialize the initial location
/*   @$.begin.filename = @$.end.filename = &driver.file; */

  // initialize the token value container
/*   $$.ival = 0; */
  yylval.sval = NULL;
}


    /* Initialize the stacks.  The initial state will be pushed in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystate_stack_ = state_stack_type (0);
    yysemantic_stack_ = semantic_stack_type (0);
    yylocation_stack_ = location_stack_type (0);
    yysemantic_stack_.push (yylval);
    yylocation_stack_.push (yylloc);

    /* New state.  */
  yynewstate:
    yystate_stack_.push (yystate);
    YYCDEBUG << "Entering state " << yystate << std::endl;

    /* Accept?  */
    if (yystate == yyfinal_)
      goto yyacceptlab;

    goto yybackup;

    /* Backup.  */
  yybackup:

    /* Try to take a decision without lookahead.  */
    yyn = yypact_[yystate];
    if (yyn == yypact_ninf_)
      goto yydefault;

    /* Read a lookahead token.  */
    if (yychar == yyempty_)
      {
	YYCDEBUG << "Reading a token: ";
	yychar = yylex (&yylval, &yylloc, driver, messageCount, memory, context);
      }


    /* Convert token to internal form.  */
    if (yychar <= yyeof_)
      {
	yychar = yytoken = yyeof_;
	YYCDEBUG << "Now at end of input." << std::endl;
      }
    else
      {
	yytoken = yytranslate_ (yychar);
	YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
      }

    /* If the proper action on seeing token YYTOKEN is to reduce or to
       detect an error, take that action.  */
    yyn += yytoken;
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yytoken)
      goto yydefault;

    /* Reduce or error.  */
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
	if (yyn == 0 || yyn == yytable_ninf_)
	goto yyerrlab;
	yyn = -yyn;
	goto yyreduce;
      }

    /* Shift the lookahead token.  */
    YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

    /* Discard the token being shifted.  */
    yychar = yyempty_;

    yysemantic_stack_.push (yylval);
    yylocation_stack_.push (yylloc);

    /* Count tokens shifted since error; after three, turn off error
       status.  */
    if (yyerrstatus_)
      --yyerrstatus_;

    yystate = yyn;
    goto yynewstate;

  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[yystate];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;

  /*-----------------------------.
  | yyreduce -- Do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    /* If YYLEN is nonzero, implement the default value of the action:
       `$$ = $1'.  Otherwise, use the top of the stack.

       Otherwise, the following line sets YYVAL to garbage.
       This behavior is undocumented and Bison
       users should not rely upon it.  */
    if (yylen)
      yyval = yysemantic_stack_[yylen - 1];
    else
      yyval = yysemantic_stack_[0];

    {
      slice<location_type, location_stack_type> slice (yylocation_stack_, yylen);
      YYLLOC_DEFAULT (yyloc, slice, yylen);
    }
    YY_REDUCE_PRINT (yyn);
    switch (yyn)
      {
	  case 5:

    { driver.myCurrentMessage->prefix.origin = *(yysemantic_stack_[(3) - (2)].sval);
/*                                                                ACE_DEBUG((LM_DEBUG,
                                                                           ACE_TEXT("set origin: \"%s\"\n"),
                                                                  driver.myCurrentMessage->prefix.origin.c_str())); */
                                                              }
    break;

  case 6:

    { driver.myCurrentMessage->prefix.user = *(yysemantic_stack_[(3) - (2)].sval);
/*                                                                ACE_DEBUG((LM_DEBUG,
                                                                           ACE_TEXT("set user: \"%s\"\n"),
                                                                           driver.myCurrentMessage->prefix.user.c_str())); */
                                                              }
    break;

  case 7:

    { driver.myCurrentMessage->prefix.host = *(yysemantic_stack_[(3) - (2)].sval);
/*                                                                ACE_DEBUG((LM_DEBUG,
                                                                           ACE_TEXT("set host: \"%s\"\n"),
                                                                           driver.myCurrentMessage->prefix.host.c_str())); */
                                                              }
    break;

  case 10:

    { ACE_ASSERT(driver.myCurrentMessage->command.string == NULL);
                                                                ACE_NEW_NORETURN(driver.myCurrentMessage->command.string,
                                                                                 std::string(*(yysemantic_stack_[(1) - (1)].sval)));
                                                                ACE_ASSERT(driver.myCurrentMessage->command.string);
                                                                driver.myCurrentMessage->command.discriminator = RPG_Net_Protocol_IRCMessage::Command::STRING;
/*                                                                ACE_DEBUG((LM_DEBUG,
                                                                           ACE_TEXT("set command: \"%s\"\n"),
                                                                           driver.myCurrentMessage->command.string->c_str())); */
                                                              }
    break;

  case 11:

    { driver.myCurrentMessage->command.numeric = ACE_static_cast(RPG_Net_Protocol_IRCNumeric_t, (yysemantic_stack_[(1) - (1)].ival));
                                                                driver.myCurrentMessage->command.discriminator = RPG_Net_Protocol_IRCMessage::Command::NUMERIC;
/*                                                                ACE_DEBUG((LM_DEBUG,
                                                                           ACE_TEXT("set command (numeric): %d\n"),
                                                                           $1)); */
                                                              }
    break;

  case 13:

    { driver.myCurrentMessage->params.push_front(*(yysemantic_stack_[(2) - (2)].sval));
/*                                                                ACE_DEBUG((LM_DEBUG,
                                                                           ACE_TEXT("set final param: \"%s\"\n"),
                                                                           driver.myCurrentMessage->params.front().c_str())); */
                                                              }
    break;

  case 14:

    { driver.myCurrentMessage->params.push_front(*(yysemantic_stack_[(2) - (1)].sval));
/*                                                                ACE_DEBUG((LM_DEBUG,
                                                                           ACE_TEXT("set param: \"%s\"\n"),
                                                                           driver.myCurrentMessage->params.front().c_str())); */
                                                              }
    break;



	default:
          break;
      }
    YY_SYMBOL_PRINT ("-> $$ =", yyr1_[yyn], &yyval, &yyloc);

    yypop_ (yylen);
    yylen = 0;
    YY_STACK_PRINT ();

    yysemantic_stack_.push (yyval);
    yylocation_stack_.push (yyloc);

    /* Shift the result of the reduction.  */
    yyn = yyr1_[yyn];
    yystate = yypgoto_[yyn - yyntokens_] + yystate_stack_[0];
    if (0 <= yystate && yystate <= yylast_
	&& yycheck_[yystate] == yystate_stack_[0])
      yystate = yytable_[yystate];
    else
      yystate = yydefgoto_[yyn - yyntokens_];
    goto yynewstate;

  /*------------------------------------.
  | yyerrlab -- here on detecting error |
  `------------------------------------*/
  yyerrlab:
    /* If not already recovering from an error, report this error.  */
    if (!yyerrstatus_)
      {
	++yynerrs_;
	error (yylloc, yysyntax_error_ (yystate, yytoken));
      }

    yyerror_range[0] = yylloc;
    if (yyerrstatus_ == 3)
      {
	/* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

	if (yychar <= yyeof_)
	  {
	  /* Return failure if at end of input.  */
	  if (yychar == yyeof_)
	    YYABORT;
	  }
	else
	  {
	    yydestruct_ ("Error: discarding", yytoken, &yylval, &yylloc);
	    yychar = yyempty_;
	  }
      }

    /* Else will try to reuse lookahead token after shifting the error
       token.  */
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:

    /* Pacify compilers like GCC when the user code never invokes
       YYERROR and the label yyerrorlab therefore never appears in user
       code.  */
    if (false)
      goto yyerrorlab;

    yyerror_range[0] = yylocation_stack_[yylen - 1];
    /* Do not reclaim the symbols of the rule which action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    yystate = yystate_stack_[0];
    goto yyerrlab1;

  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;	/* Each real token shifted decrements this.  */

    for (;;)
      {
	yyn = yypact_[yystate];
	if (yyn != yypact_ninf_)
	{
	  yyn += yyterror_;
	  if (0 <= yyn && yyn <= yylast_ && yycheck_[yyn] == yyterror_)
	    {
	      yyn = yytable_[yyn];
	      if (0 < yyn)
		break;
	    }
	}

	/* Pop the current state because it cannot handle the error token.  */
	if (yystate_stack_.height () == 1)
	YYABORT;

	yyerror_range[0] = yylocation_stack_[0];
	yydestruct_ ("Error: popping",
		     yystos_[yystate],
		     &yysemantic_stack_[0], &yylocation_stack_[0]);
	yypop_ ();
	yystate = yystate_stack_[0];
	YY_STACK_PRINT ();
      }

    yyerror_range[1] = yylloc;
    // Using YYLLOC is tempting, but would change the location of
    // the lookahead.  YYLOC is available though.
    YYLLOC_DEFAULT (yyloc, (yyerror_range - 1), 2);
    yysemantic_stack_.push (yylval);
    yylocation_stack_.push (yyloc);

    /* Shift the error token.  */
    YY_SYMBOL_PRINT ("Shifting", yystos_[yyn],
		     &yysemantic_stack_[0], &yylocation_stack_[0]);

    yystate = yyn;
    goto yynewstate;

    /* Accept.  */
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;

    /* Abort.  */
  yyabortlab:
    yyresult = 1;
    goto yyreturn;

  yyreturn:
    if (yychar != yyempty_)
      yydestruct_ ("Cleanup: discarding lookahead", yytoken, &yylval, &yylloc);

    /* Do not reclaim the symbols of the rule which action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    while (yystate_stack_.height () != 1)
      {
	yydestruct_ ("Cleanup: popping",
		   yystos_[yystate_stack_[0]],
		   &yysemantic_stack_[0],
		   &yylocation_stack_[0]);
	yypop_ ();
      }

    return yyresult;
  }

  // Generate an error message.
  std::string
  RPG_Net_Protocol_IRCParser::yysyntax_error_ (int yystate, int tok)
  {
    std::string res;
    YYUSE (yystate);
#if YYERROR_VERBOSE
    int yyn = yypact_[yystate];
    if (yypact_ninf_ < yyn && yyn <= yylast_)
      {
	/* Start YYX at -YYN if negative to avoid negative indexes in
	   YYCHECK.  */
	int yyxbegin = yyn < 0 ? -yyn : 0;

	/* Stay within bounds of both yycheck and yytname.  */
	int yychecklim = yylast_ - yyn + 1;
	int yyxend = yychecklim < yyntokens_ ? yychecklim : yyntokens_;
	int count = 0;
	for (int x = yyxbegin; x < yyxend; ++x)
	  if (yycheck_[x + yyn] == x && x != yyterror_)
	    ++count;

	// FIXME: This method of building the message is not compatible
	// with internationalization.  It should work like yacc.c does it.
	// That is, first build a string that looks like this:
	// "syntax error, unexpected %s or %s or %s"
	// Then, invoke YY_ on this string.
	// Finally, use the string as a format to output
	// yytname_[tok], etc.
	// Until this gets fixed, this message appears in English only.
	res = "syntax error, unexpected ";
	res += yytnamerr_ (yytname_[tok]);
	if (count < 5)
	  {
	    count = 0;
	    for (int x = yyxbegin; x < yyxend; ++x)
	      if (yycheck_[x + yyn] == x && x != yyterror_)
		{
		  res += (!count++) ? ", expecting " : " or ";
		  res += yytnamerr_ (yytname_[x]);
		}
	  }
      }
    else
#endif
      res = YY_("syntax error");
    return res;
  }


  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
  const signed char RPG_Net_Protocol_IRCParser::yypact_ninf_ = -20;
  const signed char
  RPG_Net_Protocol_IRCParser::yypact_[] =
  {
         0,   -20,   -20,   -20,    -1,    11,    -6,   -20,     6,     1,
     -20,   -20,     6,     6,    10,    14,   -20,    15,    16,   -20,
     -20,   -20,   -20,   -20,     1,     1,   -20,   -20
  };

  /* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
     doesn't specify something else to do.  Zero means the default is an
     error.  */
  const unsigned char
  RPG_Net_Protocol_IRCParser::yydefact_[] =
  {
         0,     4,    10,    11,     0,     0,     0,     3,    15,     0,
       1,     2,    15,    15,     0,     0,     8,     0,     0,     5,
      12,    14,    13,     9,     0,     0,     6,     7
  };

  /* YYPGOTO[NTERM-NUM].  */
  const signed char
  RPG_Net_Protocol_IRCParser::yypgoto_[] =
  {
       -20,   -20,   -20,   -19,    17,   -20,     5
  };

  /* YYDEFGOTO[NTERM-NUM].  */
  const signed char
  RPG_Net_Protocol_IRCParser::yydefgoto_[] =
  {
        -1,     5,     6,    19,     7,     8,    15
  };

  /* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule which
     number is the opposite.  If zero, do what YYDEFACT says.  */
  const signed char RPG_Net_Protocol_IRCParser::yytable_ninf_ = -1;
  const unsigned char
  RPG_Net_Protocol_IRCParser::yytable_[] =
  {
         1,     2,     3,     9,    16,    26,    27,     2,     3,    12,
       4,    10,    17,    18,    23,    13,    14,    20,    21,    22,
      24,     0,    25,    11
  };

  /* YYCHECK.  */
  const signed char
  RPG_Net_Protocol_IRCParser::yycheck_[] =
  {
         0,     7,     8,     4,     3,    24,    25,     7,     8,     3,
      10,     0,    11,    12,     0,     9,    10,    12,    13,     9,
       5,    -1,     6,     6
  };

  /* STOS_[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
  const unsigned char
  RPG_Net_Protocol_IRCParser::yystos_[] =
  {
         0,     0,     7,     8,    10,    14,    15,    17,    18,     4,
       0,    17,     3,     9,    10,    19,     3,    11,    12,    16,
      19,    19,     9,     0,     5,     6,    16,    16
  };

#if YYDEBUG
  /* TOKEN_NUMBER_[YYLEX-NUM] -- Internal symbol number corresponding
     to YYLEX-NUM.  */
  const unsigned short int
  RPG_Net_Protocol_IRCParser::yytoken_number_[] =
  {
         0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
      58,    33,    64
  };
#endif

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
  const unsigned char
  RPG_Net_Protocol_IRCParser::yyr1_[] =
  {
         0,    13,    14,    14,    14,    15,    16,    16,    16,    17,
      18,    18,    19,    19,    19,    19
  };

  /* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
  const unsigned char
  RPG_Net_Protocol_IRCParser::yyr2_[] =
  {
         0,     2,     2,     1,     1,     3,     3,     3,     1,     3,
       1,     1,     2,     2,     2,     0
  };

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
  /* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
     First, the terminals, then, starting at \a yyntokens_, nonterminals.  */
  const char*
  const RPG_Net_Protocol_IRCParser::yytname_[] =
  {
    "\"end_of_message\"", "error", "$undefined", "\"space\"", "\"origin\"",
  "\"user\"", "\"host\"", "\"cmd_string\"", "\"cmd_numeric\"", "\"param\"",
  "':'", "'!'", "'@'", "$accept", "message", "prefix", "ext_prefix",
  "body", "command", "params", 0
  };
#endif

#if YYDEBUG
  /* YYRHS -- A `-1'-separated list of the rules' RHS.  */
  const RPG_Net_Protocol_IRCParser::rhs_number_type
  RPG_Net_Protocol_IRCParser::yyrhs_[] =
  {
        14,     0,    -1,    15,    17,    -1,    17,    -1,     0,    -1,
      10,     4,    16,    -1,    11,     5,    16,    -1,    12,     6,
      16,    -1,     3,    -1,    18,    19,     0,    -1,     7,    -1,
       8,    -1,     3,    19,    -1,    10,     9,    -1,     9,    19,
      -1,    -1
  };

  /* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
     YYRHS.  */
  const unsigned char
  RPG_Net_Protocol_IRCParser::yyprhs_[] =
  {
         0,     0,     3,     6,     8,    10,    14,    18,    22,    24,
      28,    30,    32,    35,    38,    41
  };

  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
  const unsigned char
  RPG_Net_Protocol_IRCParser::yyrline_[] =
  {
         0,    70,    70,    71,    72,    73,    78,    83,    88,    89,
      90,    99,   105,   106,   111,   116
  };

  // Print the state stack on the debug stream.
  void
  RPG_Net_Protocol_IRCParser::yystack_print_ ()
  {
    *yycdebug_ << "Stack now";
    for (state_stack_type::const_iterator i = yystate_stack_.begin ();
	 i != yystate_stack_.end (); ++i)
      *yycdebug_ << ' ' << *i;
    *yycdebug_ << std::endl;
  }

  // Report on the debug stream that the rule \a yyrule is going to be reduced.
  void
  RPG_Net_Protocol_IRCParser::yy_reduce_print_ (int yyrule)
  {
    unsigned int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    /* Print the symbols being reduced, and their result.  */
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
	       << " (line " << yylno << "):" << std::endl;
    /* The symbols being reduced.  */
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
		       yyrhs_[yyprhs_[yyrule] + yyi],
		       &(yysemantic_stack_[(yynrhs) - (yyi + 1)]),
		       &(yylocation_stack_[(yynrhs) - (yyi + 1)]));
  }
#endif // YYDEBUG

  /* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
  RPG_Net_Protocol_IRCParser::token_number_type
  RPG_Net_Protocol_IRCParser::yytranslate_ (int t)
  {
    static
    const token_number_type
    translate_table[] =
    {
           0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    11,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    10,     2,
       2,     2,     2,     2,    12,     2,     2,     2,     2,     2,
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
       5,     6,     7,     8,     9
    };
    if ((unsigned int) t <= yyuser_token_number_max_)
      return translate_table[t];
    else
      return yyundef_token_;
  }

  const int RPG_Net_Protocol_IRCParser::yyeof_ = 0;
  const int RPG_Net_Protocol_IRCParser::yylast_ = 23;
  const int RPG_Net_Protocol_IRCParser::yynnts_ = 7;
  const int RPG_Net_Protocol_IRCParser::yyempty_ = -2;
  const int RPG_Net_Protocol_IRCParser::yyfinal_ = 10;
  const int RPG_Net_Protocol_IRCParser::yyterror_ = 1;
  const int RPG_Net_Protocol_IRCParser::yyerrcode_ = 256;
  const int RPG_Net_Protocol_IRCParser::yyntokens_ = 13;

  const unsigned int RPG_Net_Protocol_IRCParser::yyuser_token_number_max_ = 264;
  const RPG_Net_Protocol_IRCParser::token_number_type RPG_Net_Protocol_IRCParser::yyundef_token_ = 2;



} // yy





void
yy::RPG_Net_Protocol_IRCParser::error(const location_type& location_in,
                                      const std::string& message_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_IRCParser::error"));

  driver.error(location_in, message_in);
}

