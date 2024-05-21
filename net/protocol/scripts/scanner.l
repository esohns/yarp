%top{
#undef YYTOKENTYPE
#include "rpg_net_protocol_parser.h"

typedef void* yyscan_t;
#define YY_TYPEDEF_YY_SCANNER_T

#if !defined (YY_DECL)
#define YY_DECL                                                                   \
yy::RPG_Net_Protocol_Parser::token_type                                           \
RPG_Net_Protocol_Scanner_lex (yy::RPG_Net_Protocol_Parser::semantic_type* yylval, \
                              yy::RPG_Net_Protocol_Parser::location_type* yylloc, \
                              RPG_Net_Protocol_IParser* iparser_p,                \
                              yyscan_t yyscanner)
// ... and declare it for the parser's sake
YY_DECL;
#endif

// *NOTE*: needed for %bison-bridge
/*#define YYLTYPE yy::location
#define YYSTYPE yy::RPG_Net_Protocol_Parser::semantic_type*/

void RPG_Net_Protocol_Scanner_set_column (int, yyscan_t);

//#define FLEXINT_H
}

%{
#include "stdafx.h"

#define yyin yyg->yyin_r
#define yyout yyg->yyout_r
#define yyextra yyg->yyextra_r
#define yyleng yyg->yyleng_r
#define yytext yyg->yytext_r
#define yylineno (YY_CURRENT_BUFFER_LVALUE->yy_bs_lineno)
#define yycolumn (YY_CURRENT_BUFFER_LVALUE->yy_bs_column)
#define yy_flex_debug yyg->yy_flex_debug_r

#include <stdlib.h>

#include <string>
#include <sstream>

//#include "ace/OS_Memory.h"
#include "ace/Log_Msg.h"

#include "rpg_net_protocol_iparser.h"

/* By default yylex returns int, we use token_type.
   Unfortunately yyterminate by default returns 0, which is
   not of token_type. */
#define yyterminate() return yy::RPG_Net_Protocol_Parser::token::RPG_NET_PROTOCOL_SCANNER_EOF
%}

%option pointer 8bit reentrant never-interactive nounistd nodefault
/* %option c++ yyclass="RPG_Net_Protocol_IRCScanner" stack */
/* %option bison-bridge bison-locations */
%option warn batch nomain yywrap nounput extra-type="RPG_Net_Protocol_IParser*"
%option debug perf-report verbose yylineno noline
%option full backup perf-report verbose
%option outfile="rpg_net_protocol_scanner.cpp" header-file="rpg_net_protocol_scanner.h" prefix="RPG_Net_Protocol_Scanner_"

DIGIT          [0-9]
ALNUM          [a-zA-Z0-9]
SPACE          [ ]
COMMA          [,]
SEMICOLON      [;]
CRLF           \x0D\x0A
OCTET          [\x00-\xFF]
OCTET_NO_CRLF  [\x00-\xFF]{-}[\x0D\x0A]

%s             command
%s             position_x
%s             position_y
%s             path
%s             path_position_x
%s             path_position_y
%s             path_direction
%s             entity_id
%s             xml
%s             clientCommand
%s             previous_x
%s             previous_y
%s             cursor
%s             sound
%s             message
%s             source_x
%s             source_y
%s             positions
%s             positions_position_x
%s             positions_position_y
%s             radius

%{
#define YY_USER_ACTION yylloc->columns(yyleng);
%}

%%
%{
#if defined (_DEBUG)
  //yyset_debug (iparser_p->debug () ? 1 : 0, yyscanner);
#endif // _DEBUG
  std::stringstream converter;
%}

<INITIAL>{
{OCTET}{4}/{SPACE}         { iparser_p->reset ();
                             iparser_p->scannedBytes (yyleng);
                             ACE_UINT32 length_i = *reinterpret_cast<ACE_UINT32*> (yytext);
                             if (ACE_BYTE_ORDER == ACE_LITTLE_ENDIAN)
                               length_i = ACE_SWAP_LONG (length_i);
                             yylval->ival = static_cast<int> (length_i);
                             return yy::RPG_Net_Protocol_Parser::token::LENGTH; }
{SPACE}                    { iparser_p->scannedBytes (yyleng);
                             BEGIN(command);
                             yylval->ival = 1; }
} // end <INITIAL>
<command>{
{DIGIT}+                   { iparser_p->scannedBytes (yyleng);
                             converter.str (ACE_TEXT_ALWAYS_CHAR (""));
                             converter.clear ();
                             converter << yytext;
                             converter >> yylval->ival;
                             return yy::RPG_Net_Protocol_Parser::token::COMMAND; }
{SPACE}                    { iparser_p->scannedBytes (yyleng);
                             BEGIN(position_x);
                             yylval->ival = 1; }
} // end <command>
<position_x>{
{DIGIT}+                   { iparser_p->scannedBytes (yyleng);
                             converter.str (ACE_TEXT_ALWAYS_CHAR (""));
                             converter.clear ();
                             converter << yytext;
                             converter >> yylval->ival;
                             return yy::RPG_Net_Protocol_Parser::token::POSITION_X; }
{COMMA}                    { iparser_p->scannedBytes (yyleng);
                             BEGIN(position_y);
                             yylval->ival = 1; }
} // end <position_x>
<position_y>{
{DIGIT}+                   { iparser_p->scannedBytes (yyleng);
                             converter.str (ACE_TEXT_ALWAYS_CHAR (""));
                             converter.clear ();
                             converter << yytext;
                             converter >> yylval->ival;
                             return yy::RPG_Net_Protocol_Parser::token::POSITION_Y; }
{SPACE}                    { iparser_p->scannedBytes (yyleng);
                             BEGIN(path);
                             yylval->ival = 1; }
} // end <position_y>
<path>{
{DIGIT}{1}                 { yyless (0);
                             BEGIN(path_position_x); }
{SEMICOLON}{SPACE}         { iparser_p->scannedBytes (yyleng);
                             BEGIN(entity_id);
                             yylval->ival = 1; }
} // end <path>
<path_position_x>{
{DIGIT}+                   { iparser_p->scannedBytes (yyleng);
                             converter.str (ACE_TEXT_ALWAYS_CHAR (""));
                             converter.clear ();
                             converter << yytext;
                             converter >> yylval->ival;
                             return yy::RPG_Net_Protocol_Parser::token::PATH_NEXT_XY; }
{COMMA}                    { iparser_p->scannedBytes (yyleng);
                             yylval->ival = 1;
                             BEGIN(path_position_y); }
} // end <path_position_x>
<path_position_y>{
{DIGIT}+                   { iparser_p->scannedBytes (yyleng);
                             converter.str (ACE_TEXT_ALWAYS_CHAR (""));
                             converter.clear ();
                             converter << yytext;
                             converter >> yylval->ival;
                             return yy::RPG_Net_Protocol_Parser::token::PATH_NEXT_XY; }
{COMMA}                    { iparser_p->scannedBytes (yyleng);
                             yylval->ival = 1;
                             BEGIN(path_direction); }
} // end <path_position_y>
<path_direction>{
{DIGIT}+                   { iparser_p->scannedBytes (yyleng);
                             converter.str (ACE_TEXT_ALWAYS_CHAR (""));
                             converter.clear ();
                             converter << yytext;
                             converter >> yylval->ival;
                             return yy::RPG_Net_Protocol_Parser::token::PATH_NEXT_DIRECTION; }
{SPACE}                    { iparser_p->scannedBytes (yyleng);
                             BEGIN(path);
                             yylval->ival = 1; }
} // end <path_direction>
<entity_id>{
{DIGIT}+                   { iparser_p->scannedBytes (yyleng);
                             converter.str (ACE_TEXT_ALWAYS_CHAR (""));
                             converter.clear ();
                             converter << yytext;
                             converter >> yylval->ival;
                             return yy::RPG_Net_Protocol_Parser::token::ENTITY_ID; }
{SPACE}                    { iparser_p->scannedBytes (yyleng);
                             BEGIN(xml);
                             yylval->ival = 1; }
} // end <entity_id>
<xml>{
{OCTET_NO_CRLF}+           { iparser_p->scannedBytes (yyleng);
                             ACE_NEW_NORETURN (yylval->sval,
                                               std::string (yytext));
                             ACE_ASSERT (yylval->sval);
                             return yy::RPG_Net_Protocol_Parser::token::XML; }
{CRLF}                     { iparser_p->scannedBytes (yyleng);
                             yylval->ival = 2;
                             if (iparser_p->scannedBytes () < iparser_p->length ())
                               BEGIN(command);
                             else
                               BEGIN(INITIAL);
                             return yy::RPG_Net_Protocol_Parser::token::END_OF_COMMAND;
                           }
} // end <xml>
<clientCommand>{
{DIGIT}+                   { iparser_p->scannedBytes (yyleng);
                             converter.str (ACE_TEXT_ALWAYS_CHAR (""));
                             converter.clear ();
                             converter << yytext;
                             converter >> yylval->ival;
                             return yy::RPG_Net_Protocol_Parser::token::CLIENT_COMMAND; }
{SPACE}                    { iparser_p->scannedBytes (yyleng);
                             BEGIN(previous_x);
                             yylval->ival = 1; }
} // end <clientCommand>
<previous_x>{
{DIGIT}+                   { iparser_p->scannedBytes (yyleng);
                             converter.str (ACE_TEXT_ALWAYS_CHAR (""));
                             converter.clear ();
                             converter << yytext;
                             converter >> yylval->ival;
                             return yy::RPG_Net_Protocol_Parser::token::PREVIOUS_X; }
{COMMA}                    { iparser_p->scannedBytes (yyleng);
                             BEGIN(previous_y);
                             yylval->ival = 1; }
} // end <previous_x>
<previous_y>{
{DIGIT}+                   { iparser_p->scannedBytes (yyleng);
                             converter.str (ACE_TEXT_ALWAYS_CHAR (""));
                             converter.clear ();
                             converter << yytext;
                             converter >> yylval->ival;
                             return yy::RPG_Net_Protocol_Parser::token::PREVIOUS_Y; }
{SPACE}                    { iparser_p->scannedBytes (yyleng);
                             BEGIN(cursor);
                             yylval->ival = 1; }
} // end <previous_y>
<cursor>{
{DIGIT}+                   { iparser_p->scannedBytes (yyleng);
                             converter.str (ACE_TEXT_ALWAYS_CHAR (""));
                             converter.clear ();
                             converter << yytext;
                             converter >> yylval->ival;
                             return yy::RPG_Net_Protocol_Parser::token::CURSOR; }
{SPACE}                    { iparser_p->scannedBytes (yyleng);
                             BEGIN(sound);
                             yylval->ival = 1; }
} // end <cursor>
<sound>{
{DIGIT}+                   { iparser_p->scannedBytes (yyleng);
                             converter.str (ACE_TEXT_ALWAYS_CHAR (""));
                             converter.clear ();
                             converter << yytext;
                             converter >> yylval->ival;
                             return yy::RPG_Net_Protocol_Parser::token::SOUND; }
{SPACE}                    { iparser_p->scannedBytes (yyleng);
                             BEGIN(message);
                             yylval->ival = 1; }
} // end <sound>
<message>{
{OCTET_NO_CRLF}+           { iparser_p->scannedBytes (yyleng);
                             ACE_NEW_NORETURN (yylval->sval,
                                               std::string (yytext));
                             ACE_ASSERT (yylval->sval);
                             return yy::RPG_Net_Protocol_Parser::token::MESSAGE; }
{CRLF}                     { iparser_p->scannedBytes (yyleng);
                             BEGIN(source_x);
                             yylval->ival = 2; }
} // end <message>
<source_x>{
{DIGIT}+                   { iparser_p->scannedBytes (yyleng);
                             converter.str (ACE_TEXT_ALWAYS_CHAR (""));
                             converter.clear ();
                             converter << yytext;
                             converter >> yylval->ival;
                             return yy::RPG_Net_Protocol_Parser::token::SOURCE_X; }
{COMMA}                    { iparser_p->scannedBytes (yyleng);
                             BEGIN(source_y);
                             yylval->ival = 1; }
} // end <source_x>
<source_y>{
{DIGIT}+                   { iparser_p->scannedBytes (yyleng);
                             converter.str (ACE_TEXT_ALWAYS_CHAR (""));
                             converter.clear ();
                             converter << yytext;
                             converter >> yylval->ival;
                             return yy::RPG_Net_Protocol_Parser::token::SOURCE_Y; }
{SPACE}                    { iparser_p->scannedBytes (yyleng);
                             BEGIN(positions);
                             yylval->ival = 1; }
} // end <source_y>
<positions>{
{DIGIT}{1}                 { yyless (0);
                             BEGIN(positions_position_x); }
{SEMICOLON}{SPACE}         { iparser_p->scannedBytes (yyleng);
                             BEGIN(radius);
                             yylval->ival = 1; }
} // end <positions>
<positions_position_x>{
{DIGIT}+                   { iparser_p->scannedBytes (yyleng);
                             converter.str (ACE_TEXT_ALWAYS_CHAR (""));
                             converter.clear ();
                             converter << yytext;
                             converter >> yylval->ival;
                             return yy::RPG_Net_Protocol_Parser::token::POSITIONS_NEXT_XY; }
{COMMA}                    { iparser_p->scannedBytes (yyleng);
                             yylval->ival = 1;
                             BEGIN(positions_position_y); }
} // end <positions_position_x>
<positions_position_y>{
{DIGIT}+                   { iparser_p->scannedBytes (yyleng);
                             converter.str (ACE_TEXT_ALWAYS_CHAR (""));
                             converter.clear ();
                             converter << yytext;
                             converter >> yylval->ival;
                             return yy::RPG_Net_Protocol_Parser::token::POSITIONS_NEXT_XY; }
{SPACE}                    { iparser_p->scannedBytes (yyleng);
                             yylval->ival = 1;
                             BEGIN(positions); }
} // end <positions_position_y>
<radius>{
{DIGIT}+                   { iparser_p->scannedBytes (yyleng);
                             converter.str (ACE_TEXT_ALWAYS_CHAR (""));
                             converter.clear ();
                             converter << yytext;
                             converter >> yylval->ival;
                             return yy::RPG_Net_Protocol_Parser::token::RADIUS; }
{SPACE}                    { iparser_p->scannedBytes (yyleng);
                             yylval->ival = 1;
                             if (iparser_p->scannedBytes () < iparser_p->length ())
                               BEGIN(command);
                             else
                               BEGIN(INITIAL);
                             return yy::RPG_Net_Protocol_Parser::token::END_OF_COMMAND;
                           }
} // end <radius>
<<EOF>>                    { yyterminate(); }
<*>(?s:.)                  { iparser_p->scannedBytes (yyleng);

                             // debug info
                             std::string error_message = ACE_TEXT_ALWAYS_CHAR ("invalid character: \"");
                             error_message += yytext[0];
                             error_message += ACE_TEXT_ALWAYS_CHAR ("\", aborting");
                             iparser_p->error (*yylloc, error_message);
                             yyterminate(); }
%%

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
int
RPG_Net_Protocol_Scanner_wrap (yyscan_t yyscanner)
{
  RPG_TRACE (ACE_TEXT("::RPG_Net_Protocol_Scanner_wrap"));

  // sanity check
  RPG_Net_Protocol_IParser* iparser_p =
    RPG_Net_Protocol_Scanner_get_extra (yyscanner);
  ACE_ASSERT (iparser_p);

  // *NOTE*: there SHOULD be more data in a
  // continuation, so:
  // 1. gobble/save the rest
  // 2. switch buffers
  // 3. unput the rest
  // 4. continue matching

  // step1
  std::string the_rest;
  // the_rest += yytext[0];
  // for (char c = yyinput(yyscanner);
  //      c != EOF;
  //      c = yyinput(yyscanner))
  //   the_rest += c;

  // ACE_DEBUG((LM_DEBUG,
  //            ACE_TEXT("the rest: \"%s\"\n"),
  //            the_rest.c_str()));

  // step2
  if (!iparser_p->switchBuffer ())
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to RPG_Net_Protocol_IParser::switchBuffer() [data was: \"%s\"], aborting\n"),
                ACE_TEXT (the_rest.c_str ())));
    return 1;
  } // end IF

  // step3
  // for (std::string::reverse_iterator iterator = the_rest.rbegin();
  //      iterator != the_rest.rend();
  //      iterator++)
  //   unput(*iterator);

  // step4
 // yymore();

  return 0;
}
#ifdef __cplusplus
}
#endif /* __cplusplus */
