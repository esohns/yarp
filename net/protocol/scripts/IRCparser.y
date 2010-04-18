%require "2.4.1"
%error-verbose
%define parser_class_name "RPG_Net_Protocol_IRCParser"
/* %define namespace "" */
/* %name-prefix "IRCParse" */

%code requires {
class RPG_Net_Protocol_IRCParserDriver;
typedef void* yyscan_t;
#include <string>
}

// The parsing context.
%parse-param { RPG_Net_Protocol_IRCParserDriver& driver }
%parse-param { yyscan_t& context }
%lex-param   { RPG_Net_Protocol_IRCParserDriver& driver }
%lex-param   { yyscan_t& context }

%initial-action
{
  // Initialize the initial location.
  //@$.begin.filename = @$.end.filename = &driver.file;
};

// Symbols.
%union
{
  int          ival;
  std::string* sval;
};

%code {
#include "rpg_net_protocol_IRCparser_driver.h"
#include <ace/Log_Msg.h>
}

%token <ival> SPACE           "space"
%token <sval> SERVERNAME_NICK "servername_nick"
%token <sval> USER            "user"
%token <sval> HOST            "host"
%token <sval> CMD_STRING      "cmd_string"
%token <ival> CMD_NUMERIC     "cmd_numeric"
%token <sval> PARAM           "param"
%token        END 0           "end of message"
/* %type  <sval> prefix extended_prefix command params params_body */

%printer    { debug_stream() << *$$; } <sval>
%destructor { delete $$; } <sval>
%printer    { debug_stream() << $$; } <ival>

%%
%start message;
message:          prefix command params "end of message"        {};
prefix:           ':' "servername_nick" extended_prefix "space" { driver.myCurrentMessage.nick = $2; };
extended_prefix:  /* empty */
                  | '!' "user"                                  { driver.myCurrentMessage.user = $2; };
                  | '@' "host"                                  { driver.myCurrentMessage.host = $2; };
command:          "cmd_string"                                  { driver.myCurrentMessage.command.string = $1;
                                                                  driver.myCurrentMessage.command.discriminator = RPG_Net_Protocol_IRCMessageCommand::STRING; };
                  | "cmd_numeric"                               { driver.myCurrentMessage.command.numeric = RPG_Net_Protocol_IRC_Codes::RFC1459Numeric($1);
                                                                  driver.myCurrentMessage.command.discriminator = RPG_Net_Protocol_IRCMessageCommand::NUMERIC; };
params:           /* empty */
                  | "space" params_body                         { if (driver.myCurrentMessage.params == NULL)
                                                                    ACE_NEW_NORETURN(driver.myCurrentMessage.params,
                                                                                     std::vector<std::string>());
                                                                  ACE_ASSERT(driver.myCurrentMessage.params);
                                                                };
params_body:      /* empty */
                  | ':' "param"                                 { ACE_ASSERT(driver.myCurrentMessage.params);
                                                                  driver.myCurrentMessage.params->push_back(*$2); delete $2; };
                  | "param" params                              { ACE_ASSERT(driver.myCurrentMessage.params);
                                                                  driver.myCurrentMessage.params->push_back(*$1); delete $1; };
%%

void
yy::RPG_Net_Protocol_IRCParser::error(const location_type& location_in,
                                      const std::string& message_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_IRCParser::error"));

  driver.error(location_in, message_in);
}
