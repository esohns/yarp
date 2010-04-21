%require "2.4.1"
%error-verbose
%define parser_class_name "RPG_Net_Protocol_IRCParser"
/* %define api.pure */
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
#include "rpg_net_protocol_common.h"
#include <ace/Log_Msg.h>
}

%token        SPACE       "space"
%token <sval> ORIGIN      "origin"
%token <sval> USER        "user"
%token <sval> HOST        "host"
%token <sval> CMD_STRING  "cmd_string"
%token <ival> CMD_NUMERIC "cmd_numeric"
%token <sval> PARAM       "param"
%token        END 0       "end of message"
/* %type  <sval> message ext_prefix ext_prefix_2 body params */

%printer    { debug_stream() << *$$; } <sval>
%destructor { delete $$; } <sval>
%printer    { debug_stream() << $$; } <ival>

%%
%start message;
message:      ':' "origin" ext_prefix                         { driver.myCurrentMessage->prefix.origin = $2; };
              | body
              | "end of message"
ext_prefix:   '!' "user" ext_prefix_2                         { driver.myCurrentMessage->prefix.user = $2; };
              | "space" body                                  { };
ext_prefix_2: | '@' "host" body                               { driver.myCurrentMessage->prefix.host = $2; };
              | "space" body                                  { };
body:         "cmd_string" "space" params "end of message"    { driver.myCurrentMessage->command.string = $1;
                                                                driver.myCurrentMessage->command.discriminator = RPG_Net_Protocol_IRCMessage::Command::STRING; };
              | "cmd_numeric" "space" params "end of message" { driver.myCurrentMessage->command.numeric = RPG_Net_Protocol_IRC_Codes::RFC1459Numeric($1);
                                                                driver.myCurrentMessage->command.discriminator = RPG_Net_Protocol_IRCMessage::Command::NUMERIC; };
params:       ':' "param"                                     { if (driver.myCurrentMessage->params == NULL)
                                                                  ACE_NEW_NORETURN(driver.myCurrentMessage->params,
                                                                                   std::vector<std::string>());
                                                                ACE_ASSERT(driver.myCurrentMessage->params);
                                                                driver.myCurrentMessage->params->push_back(*$2); };
              | "param" "space" params                        { if (driver.myCurrentMessage->params == NULL)
                                                                  ACE_NEW_NORETURN(driver.myCurrentMessage->params,
                                                                                   std::vector<std::string>());
                                                                ACE_ASSERT(driver.myCurrentMessage->params);
                                                                driver.myCurrentMessage->params->push_back(*$1); };
%%

void
yy::RPG_Net_Protocol_IRCParser::error(const location_type& location_in,
                                      const std::string& message_in)
{
  ACE_TRACE(ACE_TEXT("RPG_Net_Protocol_IRCParser::error"));

  driver.error(location_in, message_in);
}
