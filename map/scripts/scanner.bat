@rem #//%%%FILE%%%////////////////////////////////////////////////////////////////////
@rem #// File Name: scanner.bat
@rem #//
@rem #// History:
@rem #//   Date   |Name | Description of modification
@rem #// ---------|-----|-------------------------------------------------------------
@rem #// 20/02/06 | soh | Creation.
@rem #//%%%FILE%%%////////////////////////////////////////////////////////////////////

@rem # generate a scanner for map (text) files
"E:\GnuWin32\bin\flex.exe" -b -d -p -B -L -o.\lex.yy.cc .\map_scanner.l

copy /Y lex.yy.cc .\..
