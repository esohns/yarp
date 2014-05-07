%require          "2.4.1"
%debug
%language         "c++"
%locations
%name-prefix      "RPG_Net_Protocol_IRCScanner_"
%no-lines
%skeleton         "lalr1.cc"
/* %skeleton         "glr.c" */
%token-table
%defines          "rpg_net_protocol_IRCparser.h"
%output           "rpg_net_protocol_IRCparser.cpp"
/* %define           api.pure */
/* %define           api.push_pull */
/* %define           parse.lac full */
%define namespace "yy"
%error-verbose
%define parser_class_name "RPG_Net_Protocol_IRCParser"

%code requires {
class RPG_Net_Protocol_IRCParserDriver;
class RPG_Net_Protocol_IRCScanner;

typedef void* yyscan_t;
}

// calling conventions / parameter passing
%parse-param { RPG_Net_Protocol_IRCParserDriver* driver }
%parse-param { unsigned int* messageCount }
%parse-param { yyscan_t yyscanner }
%lex-param { RPG_Net_Protocol_IRCParserDriver* driver }
%lex-param { unsigned int* messageCount }
%lex-param { yyscan_t yyscanner }

%initial-action
{
  // Initialize the initial location
  //@$.begin.filename = @$.end.filename = &driver->file;

  // initialize the token value container
  // $$.ival = 0;
  $$.sval = NULL;
};

// symbols
%union
{
  int          ival;
  std::string* sval;
};

%code {
#include "rpg_net_protocol_IRCparser_driver.h"
#include "rpg_net_protocol_IRCmessage.h"

#include "rpg_common_macros.h"

#include <ace/Log_Msg.h>

#include <string>
}

%token <ival> SPACE       "space"
%token <sval> ORIGIN      "origin"
%token <sval> USER        "user"
%token <sval> HOST        "host"
%token <sval> CMD_STRING  "cmd_string"
%token <ival> CMD_NUMERIC "cmd_numeric"
%token <sval> PARAM       "param"
%token        END 0       "end_of_message"
/* %type  <sval> message prefix ext_prefix command params trailing */

%printer    { debug_stream() << *$$; } <sval>
%destructor { delete $$; $$ = NULL; } <sval>
%printer    { debug_stream() << $$; } <ival>
%destructor { $$ = 0; } <ival>
/*%destructor { ACE_DEBUG((LM_DEBUG,
                         ACE_TEXT("discarding tagless symbol...\n"))); } <>*/

%%
%start message;
%nonassoc ':' '!' '@';

message:      prefix body                                     /* default */
              | body                                          /* default */
              | "end_of_message"                              /* default */
prefix:       ':' "origin" ext_prefix                         { driver->myCurrentMessage->prefix.origin = *$2;
/*                                                                ACE_DEBUG((LM_DEBUG,
                                                                           ACE_TEXT("set origin: \"%s\"\n"),
                                                                                                                                             driver->myCurrentMessage->prefix.origin.c_str())); */
                                                              };
ext_prefix:   '!' "user" ext_prefix                           { driver->myCurrentMessage->prefix.user = *$2;
/*                                                                ACE_DEBUG((LM_DEBUG,
                                                                           ACE_TEXT("set user: \"%s\"\n"),
                                                                                                                                                      driver->myCurrentMessage->prefix.user.c_str())); */
                                                              };
              | '@' "host" ext_prefix                         { driver->myCurrentMessage->prefix.host = *$2;
/*                                                                ACE_DEBUG((LM_DEBUG,
                                                                           ACE_TEXT("set host: \"%s\"\n"),
                                                                           driver.myCurrentMessage->prefix.host.c_str())); */
                                                              };
              | "space"                                       /* default */
body:         command params "end_of_message"                 /* default */
command:      "cmd_string"                                    { ACE_ASSERT(driver->myCurrentMessage->command.string == NULL);
                                                                ACE_NEW_NORETURN(driver->myCurrentMessage->command.string,
                                                                                 std::string(*$1));
                                                                ACE_ASSERT(driver->myCurrentMessage->command.string);
                                                                driver->myCurrentMessage->command.discriminator = RPG_Net_Protocol_IRCMessage::Command::STRING;
/*                                                                ACE_DEBUG((LM_DEBUG,
                                                                             ACE_TEXT("set command: \"%s\"\n"),
                                                                             driver->myCurrentMessage->command.string->c_str())); */
                                                              };
              | "cmd_numeric"                                 { driver->myCurrentMessage->command.numeric = static_cast<RPG_Net_Protocol_IRCNumeric_t>($1);
                                                                driver->myCurrentMessage->command.discriminator = RPG_Net_Protocol_IRCMessage::Command::NUMERIC;
/*                                                                ACE_DEBUG((LM_DEBUG,
                                                                             ACE_TEXT("set command (numeric): %d\n"),
                                                                             $1)); */
                                                              };
params:       "space" params                                  /* default */
              | ':' trailing                                  /* default */
              | "param" params                                { driver->myCurrentMessage->params.push_front(*$1);
/*                                                                ACE_DEBUG((LM_DEBUG,
                                                                             ACE_TEXT("set param: \"%s\"\n"),
                                                                             driver->myCurrentMessage->params.front().c_str())); */
                                                              };
              |                                               /* empty */
trailing:     "param"                                         { driver->myCurrentMessage->params.push_front(*$1);
/*                                                                ACE_DEBUG((LM_DEBUG,
                                                                             ACE_TEXT("set final param: \"%s\"\n"),
                                                                             driver->myCurrentMessage->params.front().c_str())); */
                                                              };
              |                                               /* empty */
%%

void
yy::RPG_Net_Protocol_IRCParser::error(const location_type& location_in,
                                      const std::string& message_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_IRCParser::error"));

  driver->error(location_in, message_in);
}

void
yy::RPG_Net_Protocol_IRCParser::set(yyscan_t context_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_IRCParser::set"));

  yyscanner = context_in;
}
