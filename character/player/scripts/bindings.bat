@rem //%%%FILE%%%////////////////////////////////////////////////////////////////////
@rem // File Name: bindings.bat
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

@rem generate exports file
set PerlEXE=C:\Perl64\bin\perl.exe
if NOT exist "%PerlEXE%" (
 echo invalid file ^(was: "%PerlEXE%"^)^, exiting
 goto Failed
)
set PerlScript=%LIB_ROOT%\ACE_TAO\ACE\bin\generate_export_file.pl
if NOT exist "%PerlScript%" (
 echo invalid file ^(was: "%PerlScript%"^)^, exiting
 goto Failed
)
%PerlEXE% %PerlScript% -n RPG_Player > .\..\rpg_player_exports.h
if %ERRORLEVEL% NEQ 0 (
 echo failed to generate exports header^, exiting
 set RC=%ERRORLEVEL%
 goto Failed
)

@rem C++ "glue code"
set XML2CppCodeEXE=%cd%\..\..\..\tools\XML2CppCode\cmake\Debug\XML2CppCode.exe
if NOT exist "%XML2CppCodeEXE%" (
 echo invalid file ^(was: "%XML2CppCodeEXE%"^)^, exiting
 goto Failed
)
%XML2CppCodeEXE% -e -f .\..\rpg_player.xsd -i -o .\.. -s -u -x RPG_Player
if %ERRORLEVEL% NEQ 0 (
 echo failed to generate C++ glue code^, exiting
 set RC=%ERRORLEVEL%
 goto Failed
)

@rem XML Parser
@rem set XsdEXE=C:\Program Files\CodeSynthesis XSD 3.3\bin\xsd.exe
set XsdEXE=%LIB_ROOT%\xsd\bin\xsd.exe
if NOT exist "%XsdEXE%" (
 echo invalid file ^(was: "%XsdEXE%"^)^, exiting
 goto Failed
)

@rem XML Parser/Map
@rem "%XsdEXE%" cxx-parser --export-symbol "RPG_Player_Export" --hxx-prologue "#include \"rpg_player_exports.h\"" --cxx-prologue-file .\..\stdafx.cpp --type-map .\..\rpg_player.map --options-file .\..\..\..\scripts\xsdcxx_parser_options --output-dir .\.. --export-symbol "RPG_Player_Export" --hxx-prologue "#include \"rpg_player_exports.h\"" --cxx-prologue-file .\..\stdafx.cpp .\..\rpg_player.xsd
"%XsdEXE%" cxx-parser --cxx-prologue-file .\..\stdafx.cpp --type-map .\..\rpg_player.map --options-file .\..\..\..\scripts\xsdcxx_parser_options --output-dir .\.. .\..\rpg_player.xsd
if %ERRORLEVEL% NEQ 0 (
 echo failed to generate XML parser code^, exiting
 set RC=%ERRORLEVEL%
 goto Failed
)
@rem *NOTE*: xsd improperly rearranges the included headers from the map file
@rem --> move a repaired version back into the project directory
@rem *IMPORTANT NOTE*: needs to be updated after every change
@rem copy /Y rpg_player_XML_types.h .\.. >NUL 2>&1
@rem if %ERRORLEVEL% NEQ 0 (
@rem  echo failed to copy header file^, exiting
@rem  set RC=%ERRORLEVEL%
@rem  goto Failed
@rem ) else (
@rem  echo copied header file^, continuing
@rem )

@rem XML Parser/Tree
@rem generate "XMLSchema" namespace include file (tree)
@rem "%XsdEXE%" cxx-tree --char-type char --output-dir .\.. --generate-serialization --generate-insertion ACE_OutputCDR --generate-extraction ACE_InputCDR --generate-xml-schema --hxx-suffix .h --show-anonymous --show-sloc ..\rpg_XMLSchema_XML_tree.xsd
@rem generate tree include/implementation
@rem "%XsdEXE%" cxx-tree --export-symbol "RPG_Player_Export" --hxx-prologue "#include \"rpg_player_exports.h\"" --cxx-prologue-file .\..\stdafx.cpp --options-file .\..\..\..\scripts\xsdcxx_tree_options --output-dir .\.. .\..\rpg_player.xsd
"%XsdEXE%" cxx-tree --cxx-prologue-file .\..\stdafx.cpp --options-file .\..\..\..\scripts\xsdcxx_tree_options --output-dir .\.. .\..\rpg_player.xsd
if %ERRORLEVEL% NEQ 0 (
 echo failed to generate XML tree code^, exiting
 set RC=%ERRORLEVEL%
 goto Failed
)

goto Clean_Up

:Failed
echo processing bindings...FAILED

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
