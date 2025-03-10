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

set perl_exe=C:\Strawberry\perl\bin\perl.exe
:: sanity check(s)
if NOT exist %perl_exe% (
 echo Perl runtime not found^, exiting
 set RC=1
 goto Clean_Up
)
set Yarp_dir=%cd%

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

echo generating XSD schema links...
del /F %Yarp_dir%\character\etc\rpg_common.xsd
mklink "%Yarp_dir%\character\etc\rpg_common.xsd" "%Yarp_dir%\common\etc\rpg_common.xsd"
del /F %Yarp_dir%\character\etc\rpg_dice.xsd
mklink "%Yarp_dir%\character\etc\rpg_dice.xsd" "%Yarp_dir%\chance\dice\etc\rpg_dice.xsd"

del /F %Yarp_dir%\character\player\etc\rpg_dice.xsd
mklink "%Yarp_dir%\character\player\etc\rpg_dice.xsd" "%Yarp_dir%\chance\dice\etc\rpg_dice.xsd"
del /F %Yarp_dir%\character\player\etc\rpg_common.xsd
mklink "%Yarp_dir%\character\player\etc\rpg_common.xsd" "%Yarp_dir%\common\etc\rpg_common.xsd"
del /F %Yarp_dir%\character\player\etc\rpg_magic.xsd
mklink "%Yarp_dir%\character\player\etc\rpg_magic.xsd" "%Yarp_dir%\magic\etc\rpg_magic.xsd"
del /F %Yarp_dir%\character\player\etc\rpg_common_environment.xsd
mklink "%Yarp_dir%\character\player\etc\rpg_common_environment.xsd" "%Yarp_dir%\common\etc\rpg_common_environment.xsd"
del /F %Yarp_dir%\character\player\etc\rpg_character.xsd
mklink "%Yarp_dir%\character\player\etc\rpg_character.xsd" "%Yarp_dir%\character\etc\rpg_character.xsd"
del /F %Yarp_dir%\character\player\etc\rpg_item.xsd
mklink "%Yarp_dir%\character\player\etc\rpg_item.xsd" "%Yarp_dir%\item\etc\rpg_item.xsd"

del /F %Yarp_dir%\character\monster\etc\rpg_dice.xsd
mklink "%Yarp_dir%\character\monster\etc\rpg_dice.xsd" "%Yarp_dir%\chance\dice\etc\rpg_dice.xsd"
del /F %Yarp_dir%\character\monster\etc\rpg_common.xsd
mklink "%Yarp_dir%\character\monster\etc\rpg_common.xsd" "%Yarp_dir%\common\etc\rpg_common.xsd"
del /F %Yarp_dir%\character\monster\etc\rpg_magic.xsd
mklink "%Yarp_dir%\character\monster\etc\rpg_magic.xsd" "%Yarp_dir%\magic\etc\rpg_magic.xsd"
del /F %Yarp_dir%\character\monster\etc\rpg_common_environment.xsd
mklink "%Yarp_dir%\character\monster\etc\rpg_common_environment.xsd" "%Yarp_dir%\common\etc\rpg_common_environment.xsd"
del /F %Yarp_dir%\character\monster\etc\rpg_character.xsd
mklink "%Yarp_dir%\character\monster\etc\rpg_character.xsd" "%Yarp_dir%\character\etc\rpg_character.xsd"
del /F %Yarp_dir%\character\monster\etc\rpg_item.xsd
mklink "%Yarp_dir%\character\monster\etc\rpg_item.xsd" "%Yarp_dir%\item\etc\rpg_item.xsd"
del /F %Yarp_dir%\character\monster\etc\rpg_combat.xsd
mklink "%Yarp_dir%\character\monster\etc\rpg_combat.xsd" "%Yarp_dir%\combat\etc\rpg_combat.xsd"
del /F %Yarp_dir%\character\monster\etc\rpg_player.xsd
mklink "%Yarp_dir%\character\monster\etc\rpg_player.xsd" "%Yarp_dir%\character\player\etc\rpg_player.xsd"

del /F %Yarp_dir%\combat\etc\rpg_dice.xsd
mklink "%Yarp_dir%\combat\etc\rpg_dice.xsd" "%Yarp_dir%\chance\dice\etc\rpg_dice.xsd"
del /F %Yarp_dir%\combat\etc\rpg_common.xsd
mklink "%Yarp_dir%\combat\etc\rpg_common.xsd" "%Yarp_dir%\common\etc\rpg_common.xsd"
del /F %Yarp_dir%\combat\etc\rpg_magic.xsd
mklink "%Yarp_dir%\combat\etc\rpg_magic.xsd" "%Yarp_dir%\magic\etc\rpg_magic.xsd"
del /F %Yarp_dir%\combat\etc\rpg_common_environment.xsd
mklink "%Yarp_dir%\combat\etc\rpg_common_environment.xsd" "%Yarp_dir%\common\etc\rpg_common_environment.xsd"
del /F %Yarp_dir%\combat\etc\rpg_character.xsd
mklink "%Yarp_dir%\combat\etc\rpg_character.xsd" "%Yarp_dir%\character\etc\rpg_character.xsd"

del /F %Yarp_dir%\common\etc\rpg_dice.xsd
mklink "%Yarp_dir%\common\etc\rpg_dice.xsd" "%Yarp_dir%\chance\dice\etc\rpg_dice.xsd"

del /F %Yarp_dir%\engine\etc\rpg_dice.xsd
mklink "%Yarp_dir%\engine\etc\rpg_dice.xsd" "%Yarp_dir%\chance\dice\etc\rpg_dice.xsd"
del /F %Yarp_dir%\engine\etc\rpg_common.xsd
mklink "%Yarp_dir%\engine\etc\rpg_common.xsd" "%Yarp_dir%\common\etc\rpg_common.xsd"
del /F %Yarp_dir%\engine\etc\rpg_magic.xsd
mklink "%Yarp_dir%\engine\etc\rpg_magic.xsd" "%Yarp_dir%\magic\etc\rpg_magic.xsd"
del /F %Yarp_dir%\engine\etc\rpg_common_environment.xsd
mklink "%Yarp_dir%\engine\etc\rpg_common_environment.xsd" "%Yarp_dir%\common\etc\rpg_common_environment.xsd"
del /F %Yarp_dir%\engine\etc\rpg_character.xsd
mklink "%Yarp_dir%\engine\etc\rpg_character.xsd" "%Yarp_dir%\character\etc\rpg_character.xsd"
del /F %Yarp_dir%\engine\etc\rpg_item.xsd
mklink "%Yarp_dir%\engine\etc\rpg_item.xsd" "%Yarp_dir%\item\etc\rpg_item.xsd"
del /F %Yarp_dir%\engine\etc\rpg_combat.xsd
mklink "%Yarp_dir%\engine\etc\rpg_combat.xsd" "%Yarp_dir%\combat\etc\rpg_combat.xsd"
del /F %Yarp_dir%\engine\etc\rpg_player.xsd
mklink "%Yarp_dir%\engine\etc\rpg_player.xsd" "%Yarp_dir%\character\player\etc\rpg_player.xsd"
del /F %Yarp_dir%\engine\etc\rpg_monster.xsd
mklink "%Yarp_dir%\engine\etc\rpg_monster.xsd" "%Yarp_dir%\character\monster\etc\rpg_monster.xsd"
del /F %Yarp_dir%\engine\etc\rpg_map.xsd
mklink "%Yarp_dir%\engine\etc\rpg_map.xsd" "%Yarp_dir%\map\etc\rpg_map.xsd"

del /F %Yarp_dir%\item\etc\rpg_dice.xsd
mklink "%Yarp_dir%\item\etc\rpg_dice.xsd" "%Yarp_dir%\chance\dice\etc\rpg_dice.xsd"
del /F %Yarp_dir%\item\etc\rpg_common.xsd
mklink "%Yarp_dir%\item\etc\rpg_common.xsd" "%Yarp_dir%\common\etc\rpg_common.xsd"
del /F %Yarp_dir%\item\etc\rpg_magic.xsd
mklink "%Yarp_dir%\item\etc\rpg_magic.xsd" "%Yarp_dir%\magic\etc\rpg_magic.xsd"
del /F %Yarp_dir%\item\etc\rpg_common_environment.xsd
mklink "%Yarp_dir%\item\etc\rpg_common_environment.xsd" "%Yarp_dir%\common\etc\rpg_common_environment.xsd"
del /F %Yarp_dir%\item\etc\rpg_character.xsd
mklink "%Yarp_dir%\item\etc\rpg_character.xsd" "%Yarp_dir%\character\etc\rpg_character.xsd"

del /F %Yarp_dir%\magic\etc\rpg_dice.xsd
mklink "%Yarp_dir%\magic\etc\rpg_dice.xsd" "%Yarp_dir%\chance\dice\etc\rpg_dice.xsd"
del /F %Yarp_dir%\magic\etc\rpg_common.xsd
mklink "%Yarp_dir%\magic\etc\rpg_common.xsd" "%Yarp_dir%\common\etc\rpg_common.xsd"
del /F %Yarp_dir%\magic\etc\rpg_common_environment.xsd
mklink "%Yarp_dir%\magic\etc\rpg_common_environment.xsd" "%Yarp_dir%\common\etc\rpg_common_environment.xsd"
del /F %Yarp_dir%\magic\etc\rpg_character.xsd
mklink "%Yarp_dir%\magic\etc\rpg_character.xsd" "%Yarp_dir%\character\etc\rpg_character.xsd"

del /F %Yarp_dir%\map\etc\rpg_common_environment.xsd
mklink "%Yarp_dir%\map\etc\rpg_common_environment.xsd" "%Yarp_dir%\common\etc\rpg_common_environment.xsd"

del /F %Yarp_dir%\client\etc\rpg_dice.xsd
mklink "%Yarp_dir%\client\etc\rpg_dice.xsd" "%Yarp_dir%\chance\dice\etc\rpg_dice.xsd"
del /F %Yarp_dir%\client\etc\rpg_common.xsd
mklink "%Yarp_dir%\client\etc\rpg_common.xsd" "%Yarp_dir%\common\etc\rpg_common.xsd"
del /F %Yarp_dir%\client\etc\rpg_magic.xsd
mklink "%Yarp_dir%\client\etc\rpg_magic.xsd" "%Yarp_dir%\magic\etc\rpg_magic.xsd"
del /F %Yarp_dir%\client\etc\rpg_common_environment.xsd
mklink "%Yarp_dir%\client\etc\rpg_common_environment.xsd" "%Yarp_dir%\common\etc\rpg_common_environment.xsd"
del /F %Yarp_dir%\client\etc\rpg_character.xsd
mklink "%Yarp_dir%\client\etc\rpg_character.xsd" "%Yarp_dir%\character\etc\rpg_character.xsd"
del /F %Yarp_dir%\client\etc\rpg_item.xsd
mklink "%Yarp_dir%\client\etc\rpg_item.xsd" "%Yarp_dir%\item\etc\rpg_item.xsd"
del /F %Yarp_dir%\client\etc\rpg_combat.xsd
mklink "%Yarp_dir%\client\etc\rpg_combat.xsd" "%Yarp_dir%\combat\etc\rpg_combat.xsd"
del /F %Yarp_dir%\client\etc\rpg_player.xsd
mklink "%Yarp_dir%\client\etc\rpg_player.xsd" "%Yarp_dir%\character\player\etc\rpg_player.xsd"
del /F %Yarp_dir%\client\etc\rpg_monster.xsd
mklink "%Yarp_dir%\client\etc\rpg_monster.xsd" "%Yarp_dir%\character\monster\etc\rpg_monster.xsd"
del /F %Yarp_dir%\client\etc\rpg_map.xsd
mklink "%Yarp_dir%\client\etc\rpg_map.xsd" "%Yarp_dir%\map\etc\rpg_map.xsd"
del /F %Yarp_dir%\client\etc\rpg_graphics.xsd
mklink "%Yarp_dir%\client\etc\rpg_graphics.xsd" "%Yarp_dir%\graphics\etc\rpg_graphics.xsd"
del /F %Yarp_dir%\client\etc\rpg_engine.xsd
mklink "%Yarp_dir%\client\etc\rpg_engine.xsd" "%Yarp_dir%\engine\etc\rpg_engine.xsd"
echo generating XSD schema links...DONE

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
