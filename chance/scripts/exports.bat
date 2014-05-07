@rem //%%%FILE%%%////////////////////////////////////////////////////////////////////
@rem // File Name: exports.bat
@rem //
@rem #// History:
@rem #//   Date   |Name | Description of modification
@rem #// ---------|-----|-------------------------------------------------------------
@rem #// 20/02/06 | soh | Creation.
@rem #//%%%FILE%%%////////////////////////////////////////////////////////////////////

@rem generate exports file
C:\Perl\bin\perl.exe  C:\Temp\ACE_wrappers\bin\generate_export_file.pl -n RPG_Chance > .\..\rpg_chance_exports.h
