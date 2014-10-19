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
set PerlEXE=C:\Perl\bin\perl.exe
if NOT exist "%PerlEXE%" (
 echo invalid file ^(was: "%PerlEXE%"^)^, exiting
 goto Failed
)
set PerlScript=D:\projects\ACE_wrappers\bin\generate_export_file.pl
if NOT exist "%PerlScript%" (
 echo invalid file ^(was: "%PerlScript%"^)^, exiting
 goto Failed
)
%PerlEXE% %PerlScript% -n RPG_Monster > .\..\rpg_monster_exports.h
if %ERRORLEVEL% NEQ 0 (
 echo failed to generate exports header^, exiting
 set RC=%ERRORLEVEL%
 goto Failed
)

@rem C++ "glue code"
set XML2CppCodeEXE=%cd%\..\..\..\prj\msvc\Debug\XML2CppCode.exe
if NOT exist "%XML2CppCodeEXE%" (
 echo invalid file ^(was: "%XML2CppCodeEXE%"^)^, exiting
 goto Failed
)
%XML2CppCodeEXE% -d RPG_Monster_Export -e -f .\..\rpg_monster.xsd -i -o .\.. -s -u -x RPG_Monster
if %ERRORLEVEL% NEQ 0 (
 echo failed to generate C++ glue code^, exiting
 set RC=%ERRORLEVEL%
 goto Failed
)

@rem XML Parser
@rem set XsdEXE=C:\Program Files\CodeSynthesis XSD 3.3\bin\xsd.exe
set XsdEXE=C:\Program Files (x86)\CodeSynthesis XSD 3.3\bin\xsd.exe
if NOT exist "%XsdEXE%" (
 echo invalid file ^(was: "%XsdEXE%"^)^, exiting
 goto Failed
)
@rem generate "XMLSchema" namespace include file
@rem "%XsdEXE%" cxx-parser --char-type char --output-dir .\.. --xml-parser xerces --force-overwrite --generate-xml-schema --skel-file-suffix "" --hxx-suffix .h --show-anonymous --show-sloc ..\rpg_XMLSchema_XML_types.xsd
@rem generate include/implementation (rpg_monster.xsd)
"%XsdEXE%" cxx-parser --export-symbol "RPG_Monster_Export" --hxx-prologue "#include \"rpg_monster_exports.h\"" --cxx-prologue-file .\..\stdafx.cpp --type-map .\..\rpg_monster.map --options-file .\..\..\..\scripts\xsdcxx_parser_options --output-dir .\.. .\..\rpg_monster.xsd
if %ERRORLEVEL% NEQ 0 (
 echo failed to generate XML parser code^, exiting
 set RC=%ERRORLEVEL%
 goto Failed
)
@rem *NOTE*: xsd improperly rearranges the included headers from the map file
@rem --> move a repaired version back into the project directory
@rem *IMPORTANT NOTE*: needs to be updated after every change
@rem copy /Y rpg_monster_XML_types.h .\.. >NUL 2>&1
@rem if %ERRORLEVEL% NEQ 0 (
@rem  echo failed to copy header file^, exiting
@rem  set RC=%ERRORLEVEL%
@rem  goto Failed
@rem ) else (
@rem  echo copied header file^, continuing
@rem )

@rem generate tree include/implementation (rpg_monster.xsd)
@rem "%XsdEXE%" cxx-tree --generate-serialization --generate-ostream --generate-comparison --generate-insertion ACE_OutputCDR --generate-extraction ACE_InputCDR --type-regex "/(.+) RPG_(.+)_Type/RPG_\u$2_XMLTree_Type/" --char-type char --output-dir .\.. --namespace-map urn:rpg= --extern-xml-schema rpg_XMLSchema.h --hxx-suffix _XML_tree.h --cxx-suffix _XML_tree.cpp --show-anonymous --show-sloc --export-symbol "RPG_Graphics_Export" --hxx-prologue "#include \"rpg_graphics_exports.h\"" --cxx-prologue-file .\..\stdafx.cpp ..\rpg_graphics.xsd
"%XsdEXE%" cxx-tree --output-dir .\.. --export-symbol "RPG_Monster_Export" --hxx-prologue "#include \"rpg_monster_exports.h\"" --cxx-prologue-file .\..\stdafx.cpp --options-file .\..\..\..\scripts\xsdcxx_tree_options ..\rpg_monster.xsd
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
