@rem //%%%FILE%%%////////////////////////////////////////////////////////////////////
@rem // File Name: generate_all_bindings.bat
@rem //
@rem #// History:
@rem #//   Date   |Name | Description of modification
@rem #// ---------|-----|-------------------------------------------------------------
@rem #// 20/02/06 | soh | Creation.
@rem #//%%%FILE%%%////////////////////////////////////////////////////////////////////

@echo off
set RC=0
setlocal enabledelayedexpansion
pushd . >NUL 2>&1

set perl_exe=C:\Perl64\bin\perl.exe
:: sanity check(s)
if NOT exist %perl_exe% (
 echo Perl runtime not found^, exiting
 set RC=1
 goto Clean_Up
)
set Yarp_dir=.

:: ..........................................

echo generating DLL exports headers...
@rem call %Yarp_dir%\chance\scripts\exports.bat
@rem call %Yarp_dir%\net\scripts\exports.bat
@rem call %Yarp_dir%\net\protocol\scripts\exports.bat
@rem call %Yarp_dir%\net\stream\scripts\exports.bat

if %ERRORLEVEL% NEQ 0 (
 echo failed generating DLL exports headers
 goto Clean_Up
)
echo generating DLL exports headers...DONE

:: ..........................................

echo generating XML data bindings...
call %Yarp_dir%\chance\dice\scripts\bindings.bat
call %Yarp_dir%\character\scripts\bindings.bat
call %Yarp_dir%\character\player\scripts\bindings.bat
call %Yarp_dir%\character\monster\scripts\bindings.bat
call %Yarp_dir%\combat\scripts\bindings.bat
call %Yarp_dir%\common\scripts\bindings.bat
call %Yarp_dir%\engine\scripts\bindings.bat
call %Yarp_dir%\graphics\scripts\bindings.bat
call %Yarp_dir%\item\scripts\bindings.bat
call %Yarp_dir%\magic\scripts\bindings.bat
call %Yarp_dir%\map\scripts\bindings.bat
call %Yarp_dir%\sound\scripts\bindings.bat
call %Yarp_dir%\client\scripts\bindings.bat

if %ERRORLEVEL% NEQ 0 (
 echo failed generating XML data bindings
 goto Clean_Up
)
echo generating XML data bindings...DONE

:: ..........................................

echo generating lex/yacc parsers...
call %Yarp_dir%\map\scripts\scanner.bat
call %Yarp_dir%\map\scripts\parser.bat
call %Yarp_dir%\net\protocol\scripts\scanner.bat
call %Yarp_dir%\net\protocol\scripts\parser.bat

if %ERRORLEVEL% NEQ 0 (
 echo failed generating lex/yacc parsers
 goto Clean_Up
)
echo generating lex/yacc parsers...DONE

:Clean_Up
popd
if %ERRORLEVEL% NEQ 0 (
 set RC=%ERRORLEVEL%
)
::endlocal & set RC=%ERRORLEVEL%
endlocal & set RC=%RC%
goto Error_Level

:Exit_Code
:: echo %ERRORLEVEL% %1 *WORKAROUND*
exit /b %1

:Error_Level
call :Exit_Code %RC%
