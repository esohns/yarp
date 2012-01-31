@rem #//%%%FILE%%%////////////////////////////////////////////////////////////////////
@rem #// File Name: scanner.bat
@rem #//
@rem #// History:
@rem #//   Date   |Name | Description of modification
@rem #// ---------|-----|-------------------------------------------------------------
@rem #// 20/02/06 | soh | Creation.
@rem #//%%%FILE%%%////////////////////////////////////////////////////////////////////

@rem # generate a scanner for the IRC protocol
"E:\GnuWin32\bin\flex.exe" .\IRCbisect.l
"E:\GnuWin32\bin\flex.exe" .\IRCscanner.l

copy /Y rpg_net_protocol_IRCbisect.cpp .\..
copy /Y rpg_net_protocol_IRCbisect.h .\..
copy /Y rpg_net_protocol_IRCscanner.cpp .\..
copy /Y rpg_net_protocol_IRCscanner.h .\..
