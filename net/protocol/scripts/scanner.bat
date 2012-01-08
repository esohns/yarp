@rem #//%%%FILE%%%////////////////////////////////////////////////////////////////////
@rem #// File Name: scanner.bat
@rem #//
@rem #// History:
@rem #//   Date   |Name | Description of modification
@rem #// ---------|-----|-------------------------------------------------------------
@rem #// 20/02/06 | soh | Creation.
@rem #//%%%FILE%%%////////////////////////////////////////////////////////////////////

@rem # generate a scanner for the IRC protocol
"E:\GnuWin32\bin\flex.exe" -b -d -p -B -L -o.\IRCbisect_lex.yy.cc .\IRCbisect.l
"E:\GnuWin32\bin\flex.exe" -b -d -p -B -L -o.\IRCscanner_lex.yy.cc .\IRCscanner.l

copy /Y IRCbisect_lex.yy.cc .\..
copy /Y IRCscanner_lex.yy.cc .\..
