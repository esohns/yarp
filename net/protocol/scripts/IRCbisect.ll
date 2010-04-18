%option pointer full nounistd 8bit stack
/* %option c++ outfile="rpg_net_protocol_IRCbisect.cpp" */
%option warn nodefault noyywrap nounput
%option debug perf-report verbose
/* %option   outfile="RPG_Net_Protocol_IRCScanner.cpp" header-file="RPG_Net_Protocol_IRCScanner.h" */
/* %option bison-bridge bison-locations trace noline reentrant */
%option prefix="IRCBisect" yylineno

%{
int num_messages = 0;
%}

%%
.*\r\n  ++num_messages;
.       /* Nothing */
%%
