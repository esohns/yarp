@rem //%%%FILE%%%////////////////////////////////////////////////////////////////////
@rem // File Name: exports.bat
@rem //
@rem #// History:
@rem #//   Date   |Name | Description of modification
@rem #// ---------|-----|-------------------------------------------------------------
@rem #// 20/02/06 | soh | Creation.
@rem #//%%%FILE%%%////////////////////////////////////////////////////////////////////

@rem generate exports files
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
%PerlEXE% %PerlScript% -n RPG_Protocol > .\..\rpg_net_protocol_exports.h
if %ERRORLEVEL% NEQ 0 (
 echo failed to generate exports header^, exiting
 set RC=%ERRORLEVEL%
 goto Failed
)
