@rem //%%%FILE%%%////////////////////////////////////////////////////////////////////
@rem // File Name: exports.bat
@rem //
@rem #// History:
@rem #//   Date   |Name | Description of modification
@rem #// ---------|-----|-------------------------------------------------------------
@rem #// 20/02/06 | soh | Creation.
@rem #//%%%FILE%%%////////////////////////////////////////////////////////////////////

@rem generate exports file(s)
C:\Perl\bin\perl.exe C:\Temp\ACE_wrappers\bin\generate_export_file.pl -n RPG_Net > .\..\rpg_net_exports.h
C:\Perl\bin\perl.exe C:\Temp\ACE_wrappers\bin\generate_export_file.pl -n RPG_Net_Client > .\..\client\rpg_net_client_exports.h
C:\Perl\bin\perl.exe C:\Temp\ACE_wrappers\bin\generate_export_file.pl -n RPG_Net_Server > .\..\server\rpg_net_server_exports.h
