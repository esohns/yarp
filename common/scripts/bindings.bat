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
%PerlEXE% %PerlScript% -n RPG_Common > .\..\rpg_common_exports.h
if %ERRORLEVEL% NEQ 0 (
 echo failed to generate exports header^, exiting
 set RC=%ERRORLEVEL%
 goto Failed
)

@rem C++ "glue code"
set XML2CppCodeEXE=%cd%\..\..\prj\msvc\Debug\XML2CppCode.exe
if NOT exist "%XML2CppCodeEXE%" (
 echo invalid file ^(was: "%XML2CppCodeEXE%"^)^, exiting
 goto Failed
)
%XML2CppCodeEXE% -d RPG_Common_Export -e -f .\..\rpg_common.xsd -i -o .\.. -s -u -x RPG_Common
%XML2CppCodeEXE% -d RPG_Common_Export -e -f .\..\rpg_common_environment.xsd -i -o .\.. -s -u -x RPG_Common
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
@rem generate "XMLSchema" namespace include file (rpg_common.xsd/rpg_common_environment.xsd)
"%XsdEXE%" cxx-parser --char-type char --xml-parser xerces --force-overwrite --output-dir .\.. --hxx-suffix .h --show-anonymous --show-sloc --generate-xml-schema --skel-file-suffix "" ..\rpg_XMLSchema_XML_types.xsd
if %ERRORLEVEL% NEQ 0 (
 echo failed to generate XML namespace code ^(parser^)^, exiting
 set RC=%ERRORLEVEL%
 goto Failed
)
@rem generate include/implementation (rpg_common.xsd)
"%XsdEXE%" cxx-parser --type-map .\..\rpg_common.map --output-dir .\.. --export-symbol "RPG_Common_Export" --hxx-prologue "#include \"rpg_common_exports.h\"" --cxx-prologue-file .\..\stdafx.cpp --options-file .\..\..\scripts\xsdcxx_parser_options .\..\rpg_common.xsd
if %ERRORLEVEL% NEQ 0 (
 echo failed to generate XML parser code^, exiting
 set RC=%ERRORLEVEL%
 goto Failed
)
@rem generate include/implementation (rpg_common_environment.xsd)
"%XsdEXE%" cxx-parser --type-map .\..\rpg_common_environment.map --output-dir .\.. --export-symbol "RPG_Common_Export" --hxx-prologue "#include \"rpg_common_exports.h\"" --cxx-prologue-file .\..\stdafx.cpp --options-file .\..\..\scripts\xsdcxx_parser_options .\..\rpg_common_environment.xsd
if %ERRORLEVEL% NEQ 0 (
 echo failed to generate XML parser code^, exiting
 set RC=%ERRORLEVEL%
 goto Failed
)
@rem generate "XMLSchema" namespace include file (tree)
@rem "%XsdEXE%" cxx-tree --char-type char --output-dir .\.. --generate-serialization --generate-insertion ACE_OutputCDR --generate-extraction ACE_InputCDR --generate-xml-schema --hxx-suffix .h --show-anonymous --show-sloc ..\rpg_XMLSchema_XML_tree.xsd
"%XsdEXE%" cxx-tree --char-type char --output-dir .\.. --generate-serialization --generate-insertion ACE_OutputCDR --generate-extraction ACE_InputCDR --generate-xml-schema --hxx-suffix .h --show-anonymous --show-sloc ..\rpg_XMLSchema_XML_tree.xsd
if %ERRORLEVEL% NEQ 0 (
 echo failed to generate XML namespace code ^(tree^)^, exiting
 set RC=%ERRORLEVEL%
 goto Failed
)
@rem generate tree include/implementation (rpg_common.xsd)
@rem "%XsdEXE%" cxx-tree --generate-serialization --generate-ostream --generate-comparison --generate-insertion ACE_OutputCDR --generate-extraction ACE_InputCDR --type-regex "/(.+) RPG_(.+)_Type/RPG_\u$2_XMLTree_Type/" --char-type char --output-dir .\.. --namespace-map urn:rpg= --export-xml-schema --extern-xml-schema rpg_XMLSchema.h --hxx-suffix _XML_tree.h --cxx-suffix _XML_tree.cpp --show-anonymous --show-sloc --export-symbol "RPG_Common_Export" --hxx-prologue "#include \"rpg_common_exports.h\"" --cxx-prologue-file .\..\stdafx.cpp ..\rpg_common.xsd
"%XsdEXE%" cxx-tree --output-dir .\.. --export-symbol "RPG_Common_Export" --hxx-prologue "#include \"rpg_common_exports.h\"" --cxx-prologue-file .\..\stdafx.cpp --options-file .\..\..\scripts\xsdcxx_tree_options ..\rpg_common.xsd
if %ERRORLEVEL% NEQ 0 (
 echo failed to generate XML tree code^, exiting
 set RC=%ERRORLEVEL%
 goto Failed
)
@rem generate tree include/implementation (rpg_common_environment.xsd)
@rem "%XsdEXE%" cxx-tree --generate-serialization --generate-ostream --generate-comparison --generate-insertion ACE_OutputCDR --generate-extraction ACE_InputCDR --type-regex "/(.+) RPG_(.+)_Type/RPG_\u$2_XMLTree_Type/" --char-type char --output-dir .\.. --namespace-map urn:rpg= --export-xml-schema --extern-xml-schema rpg_XMLSchema.h --hxx-suffix _XML_tree.h --cxx-suffix _XML_tree.cpp --show-anonymous --show-sloc --export-symbol "RPG_Common_Export" --hxx-prologue "#include \"rpg_common_exports.h\"" --cxx-prologue-file .\..\stdafx.cpp ..\rpg_common_environment.xsd
"%XsdEXE%" cxx-tree --output-dir .\.. --export-symbol "RPG_Common_Export" --hxx-prologue "#include \"rpg_common_exports.h\"" --cxx-prologue-file .\..\stdafx.cpp --options-file .\..\..\scripts\xsdcxx_tree_options ..\rpg_common_environment.xsd
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
