@rem #//%%%FILE%%%////////////////////////////////////////////////////////////////////
@rem #// File Name: parser.bat
@rem #//
@rem #// History:
@rem #//   Date   |Name | Description of modification
@rem #// ---------|-----|-------------------------------------------------------------
@rem #// 20/02/06 | soh | Creation.
@rem #//%%%FILE%%%////////////////////////////////////////////////////////////////////

@rem generate a parser for map (text) files
"E:\cygwin\bin\bison.exe" --verbose --graph=parser_graph.txt --xml=parser_graph.xml ./map_parser.y --report=all --report-file=parser_report.txt --warnings=all

@rem move generated files into the project directory
move /Y rpg_map_parser.h .\..
move /Y rpg_map_parser.cpp .\..
move /Y position.hh .\..
move /Y stack.hh .\..
move /Y location.hh .\..
