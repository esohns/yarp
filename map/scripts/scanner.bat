@rem #//%%%FILE%%%////////////////////////////////////////////////////////////////////
@rem #// File Name: scanner.bat
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

@rem # generate a scanner for map (text) files
set FlexEXE=C:\cygwin\bin\flex.exe
if NOT exist "%FlexEXE%" (
 echo invalid file ^(was: "%FlexEXE%"^)^, exiting
 goto Failed
)
%FlexEXE% ./map_scanner.l
if %ERRORLEVEL% NEQ 0 (
 echo failed to generate scanner^, exiting
 set RC=%ERRORLEVEL%
 goto Failed
)

@move /Y rpg_map_scanner.cpp .\..
@move /Y rpg_map_scanner.h .\..
if %ERRORLEVEL% NEQ 0 (
 echo failed to move scanner file^(s^)^, exiting
 set RC=%ERRORLEVEL%
 goto Failed
)

goto Clean_Up

:Failed
echo processing scanner...FAILED

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
