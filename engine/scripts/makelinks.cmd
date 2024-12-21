@rem //%%%FILE%%%////////////////////////////////////////////////////////////////////
@rem // File Name: makelinks.cmd
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

@rem create symbolic links on ntfs drives
set MkLinkEXE=mklink.exe
@rem if NOT exist "%MkLinkEXE%" (
@rem  echo invalid file ^(was: "%MkLinkEXE%"^)^, exiting
@rem  goto Failed
@rem )

cd %PRJ_ROOT%\yarp\engine\etc

mklink rpg_dice.xsd ..\..\chance\dice\etc\rpg_dice.xsd
if %ERRORLEVEL% NEQ 0 (
 echo failed to generate symbolic link, exiting
 set RC=%ERRORLEVEL%
 goto Failed
)
mklink rpg_common.xsd ..\..\common\etc\rpg_common.xsd
if %ERRORLEVEL% NEQ 0 (
 echo failed to generate symbolic link, exiting
 set RC=%ERRORLEVEL%
 goto Failed
)
mklink rpg_common_environment.xsd ..\..\common\etc\rpg_common_environment.xsd
if %ERRORLEVEL% NEQ 0 (
 echo failed to generate symbolic link, exiting
 set RC=%ERRORLEVEL%
 goto Failed
)
mklink rpg_character.xsd ..\..\character\etc\rpg_character.xsd
if %ERRORLEVEL% NEQ 0 (
 echo failed to generate symbolic link, exiting
 set RC=%ERRORLEVEL%
 goto Failed
)
mklink rpg_magic.xsd ..\..\magic\etc\rpg_magic.xsd
if %ERRORLEVEL% NEQ 0 (
 echo failed to generate symbolic link, exiting
 set RC=%ERRORLEVEL%
 goto Failed
)
mklink rpg_item.xsd ..\..\item\etc\rpg_item.xsd
if %ERRORLEVEL% NEQ 0 (
 echo failed to generate symbolic link, exiting
 set RC=%ERRORLEVEL%
 goto Failed
)
mklink rpg_player.xsd ..\..\character\player\etc\rpg_player.xsd
if %ERRORLEVEL% NEQ 0 (
 echo failed to generate symbolic link, exiting
 set RC=%ERRORLEVEL%
 goto Failed
)
mklink rpg_combat.xsd ..\..\combat\etc\rpg_combat.xsd
if %ERRORLEVEL% NEQ 0 (
 echo failed to generate symbolic link, exiting
 set RC=%ERRORLEVEL%
 goto Failed
)
mklink rpg_monster.xsd ..\..\character\monster\etc\rpg_monster.xsd
if %ERRORLEVEL% NEQ 0 (
 echo failed to generate symbolic link, exiting
 set RC=%ERRORLEVEL%
 goto Failed
)
mklink rpg_map.xsd ..\..\map\etc\rpg_map.xsd
if %ERRORLEVEL% NEQ 0 (
 echo failed to generate symbolic link, exiting
 set RC=%ERRORLEVEL%
 goto Failed
)

goto Clean_Up

:Failed
echo processing links...FAILED

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
