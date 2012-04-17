; Yarp.nsi

;--------------------------------

; The name of the installer
Name "Yarp"

; The file to write
OutFile "Yarp.exe"

; The default installation directory
;InstallDir $PROGRAMFILES\Yarp
InstallDir $DESKTOP\Yarp

; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM "Software\Yarp" "Install_Dir"

; Request application privileges for Windows Vista
RequestExecutionLevel user

;--------------------------------

; Pages

Page components
Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

;--------------------------------

; the stuff to install
Section "Yarp (required)"

  SectionIn RO
  
  ; set output path to the installation directory
  SetOutPath $INSTDIR
  
  ; put files there
  File "ACE.dll"
  File "chance.dll"
  File "character.dll"
  File "client.dll"
  File "combat.dll"
  File "common.dll"
  File "dice.dll"
  File "engine.dll"
  File "freetype6.dll"
  File "graphics.dll"
  File "intl.dll"
  File "item.dll"
  File "libatk-1.0-0.dll"
  File "libcairo-2.dll"
  File "libexpat-1.dll"
  File "libfontconfig-1.dll"
  File "libfreetype-6.dll"
  File "libgdk_pixbuf-2.0-0.dll"
  File "libgdk-win32-2.0-0.dll"
  File "libgio-2.0-0.dll"
  File "libglade-2.0-0.dll"
  File "libglib-2.0-0.dll"
  File "libgmodule-2.0-0.dll"
  File "libgobject-2.0-0.dll"
  File "libgthread-2.0-0.dll"
  File "libgtk-win32-2.0-0.dll"
  File "libiconv-2.dll"
  File "libogg-0.dll"
  File "libpango-1.0-0.dll"
  File "libpangocairo-1.0-0.dll"
  File "libpangoft2-1.0-0.dll"
  File "libpangowin32-1.0-0.dll"
  File "libpng14-14.dll"
  File "libpng15.dll"
  File "libvorbis-0.dll"
  File "libvorbisfile-3.dll"
  File "libxml2-2.dll"
  File "magic.dll"
  File "map.dll"
  File "mikmod.dll"
  File "monster.dll"
  File "net.dll"
  File "player.dll"
  File "protocol.dll"
  File "SDL.dll"
  File "SDL_mixer.dll"
  File "SDL_ttf.dll"
  File "sound.dll"
  File "stream.dll"
  File "zlib1.dll"
  
  File "test_u_chance_dice.exe"
  File "test_u_chance_rangeToRoll.exe"
  File "test_u_character_generator.exe"
  File "test_u_character_generator_gui.exe"
  File "test_u_character_parser.exe"
  File "test_u_client_gui.exe"
  File "test_u_graphics_parser.exe"
  File "test_u_graphics_SDL_gui.exe"
  File "test_u_item_parser.exe"
  File "test_u_magic_parser.exe"
  File "test_u_map_generator.exe"
  File "test_u_map_parser.exe"
  File "test_u_map_path_finder.exe"
  File "test_u_map_vision.exe"
  File "test_u_map_vision_gui.exe"
  File "test_u_monster_parser.exe"
  File "test_u_net_client.exe"
  File "test_u_net_protocol_client.exe"
  File "test_u_net_protocol_client_gui.exe"
  File "test_u_net_server.exe"
  File "test_u_sound_parser.exe"
  
  ; Write the installation path into the registry
  WriteRegStr HKLM SOFTWARE\Yarp "Install_Dir" "$INSTDIR"
  
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Yarp" "DisplayName" "Yarp"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Yarp" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Yarp" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Yarp" "NoRepair" 1
  WriteUninstaller "uninstall.exe"
  
SectionEnd

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"

  CreateDirectory "$SMPROGRAMS\Yarp"
  CreateShortCut "$SMPROGRAMS\Example2\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\Example2\Example2 (MakeNSISW).lnk" "$INSTDIR\example2.nsi" "" "$INSTDIR\example2.nsi" 0
  
SectionEnd

;--------------------------------

; Uninstaller

Section "Uninstall"
  
  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Example2"
  DeleteRegKey HKLM SOFTWARE\NSIS_Example2

  ; Remove files and uninstaller
  Delete $INSTDIR\example2.nsi
  Delete $INSTDIR\uninstall.exe

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\Example2\*.*"

  ; Remove directories used
  RMDir "$SMPROGRAMS\Example2"
  RMDir "$INSTDIR"

SectionEnd
