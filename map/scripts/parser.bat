@rem #//%%%FILE%%%////////////////////////////////////////////////////////////////////
@rem #// File Name: parser.bat
@rem #//
@rem #// History:
@rem #//   Date   |Name | Description of modification
@rem #// ---------|-----|-------------------------------------------------------------
@rem #// 20/02/06 | soh | Creation.
@rem #//%%%FILE%%%////////////////////////////////////////////////////////////////////

@rem generate a parser for map (text) files
"E:\GnuWin32\bin\bison.exe" --language=C++ --skeleton="lalr1.cc" --debug --locations --no-lines --token-table --verbose --defines=.\rpg_map_parser.h --output=.\rpg_map_parser.cpp --graph=parser_graph.txt --xml=parser_graph.xml .\map_parser.y --report=all --report-file=parser_report.txt --warnings=all

@rem move generated files into the project directory
copy /Y rpg_map_parser.h .\..
copy /Y rpg_map_parser.cpp .\..
copy /Y position.hh .\..
copy /Y stack.hh .\..
copy /Y location.hh .\..