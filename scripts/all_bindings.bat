@rem //%%%FILE%%%////////////////////////////////////////////////////////////////////
@rem // File Name: all_bindings.bat
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

cd %cd%\..
set WD=%cd%
for %%A in (chance\dice character character\player character\monster client combat common engine graphics item magic map sound) do (
 if NOT exist "%%A\scripts\bindings.bat" (
  echo invalid directory ^(was: %%A^)^, exiting
  goto Failed
 )
 cd "%%A\scripts" 
 call bindings.bat
 if %ERRORLEVEL% NEQ 0 (
  echo failed to run bindings.bat in %%A^, exiting
  set RC=%ERRORLEVEL%
  goto Failed
 )
 cd "%WD%" 
 echo generated %%A...DONE
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

