@rem #//%%%FILE%%%////////////////////////////////////////////////////////////////////
@rem #// File Name: scanner.bat
@rem #//
@rem #// History:
@rem #//   Date   |Name | Description of modification
@rem #// ---------|-----|-------------------------------------------------------------
@rem #// 20/02/06 | soh | Creation.
@rem #//%%%FILE%%%////////////////////////////////////////////////////////////////////

@rem # generate a scanner for the IRC protocol
"E:\cygwin\bin\flex.exe" ./IRCbisect.l
"E:\cygwin\bin\flex.exe" ./IRCscanner.l

move /Y rpg_net_protocol_IRCbisect.cpp .\..
move /Y rpg_net_protocol_IRCbisect.h .\..
move /Y rpg_net_protocol_IRCscanner.cpp .\..
move /Y rpg_net_protocol_IRCscanner.h .\..
