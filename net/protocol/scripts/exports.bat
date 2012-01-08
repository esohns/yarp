@rem //%%%FILE%%%////////////////////////////////////////////////////////////////////
@rem // File Name: exports.bat
@rem //
@rem #// History:
@rem #//   Date   |Name | Description of modification
@rem #// ---------|-----|-------------------------------------------------------------
@rem #// 20/02/06 | soh | Creation.
@rem #//%%%FILE%%%////////////////////////////////////////////////////////////////////

@rem generate exports file
"E:\ACE_wrappers\bin\generate_export_file.pl" -n RPG_Protocol > .\..\rpg_net_protocol_exports.h
