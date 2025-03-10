@rem #//%%%FILE%%%////////////////////////////////////////////////////////////////////
@rem #// File Name: parser.bat
@rem #//
@rem #// History:
@rem #//   Date   |Name | Description of modification
@rem #// ---------|-----|-------------------------------------------------------------
@rem #// 20/02/06 | soh | Creation.
@rem #//%%%FILE%%%////////////////////////////////////////////////////////////////////
@echo off
set RC=0
setlocal enabledelayedexpansion
pushd . >NUL 2>&1

@rem generate a parser for map (text) files
set BisonEXE=C:\cygwin\bin\bison.exe
if NOT exist "%BisonEXE%" (
 echo invalid file ^(was: "%BisonEXE%"^)^, exiting
 goto Failed
)
%BisonEXE% --verbose --graph=parser_graph.txt --xml=parser_graph.xml ./map_parser.y --report=all --report-file=parser_report.txt --warnings=all
if %ERRORLEVEL% NEQ 0 (
 echo failed to generate parser^, exiting
 set RC=%ERRORLEVEL%
 goto Failed
)

@rem move generated files into the project directory
@move /Y rpg_map_parser.h .\..
@move /Y rpg_map_parser.cpp .\..
@rem @move /Y position.hh .\..
@rem @move /Y stack.hh .\..
@rem @move /Y location.hh .\..
if %ERRORLEVEL% NEQ 0 (
 echo failed to move parser file^(s^)^, exiting
 set RC=%ERRORLEVEL%
 goto Failed
)

goto Clean_Up

:Failed
echo processing parser...FAILED

:Clean_Up
popd
::endlocal & set RC=%ERRORLEVEL%
endlocal & set RC=%RC%
goto Error_Level

:Exit_Code
:: echo %ERRORLEVEL% %1 *WORKAROUND*
exit /b %1

:Error_Level
call :Exit_Code %RC%
