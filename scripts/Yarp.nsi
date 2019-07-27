; Yarp.nsi
;--------------------------------

; Includes
;!include "FileFunc.nsh"
;--------------------------------

!define PROGRAM "Yarp"
!define CONFIGURATION_SUBDIR "config"
!define DATA_SUBDIR "data"

; Languages
LoadLanguageFile "${NSISDIR}\Contrib\Language files\English.nlf"

; The name of the installer
Name ${PROGRAM}

; The file to write
!searchparse /file "..\configure.ac" `AC_SUBST([YARP_API_VERSION], [` VER_MAJOR `.` VER_MINOR `])`
VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductName" ${PROGRAM}
VIAddVersionKey /LANG=${LANG_ENGLISH} "Comments" "a RPG framework"
;VIAddVersionKey /LANG=${LANG_ENGLISH} "CompanyName" ""
!define /date NOW "%Y"
VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalCopyright" "© ${NOW}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileDescription" "${PROGRAM} installer"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileVersion" "${VER_MAJOR}.${VER_MINOR}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductVersion" "${VER_MAJOR}.${VER_MINOR}"
;VIAddVersionKey /LANG=${LANG_ENGLISH} "InternalName" ""
;VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalTrademarks" ""
;VIAddVersionKey /LANG=${LANG_ENGLISH} "OriginalFilename" ""
;VIAddVersionKey /LANG=${LANG_ENGLISH} "PrivateBuild" ""
;VIAddVersionKey /LANG=${LANG_ENGLISH} "SpecialBuild" ""
VIProductVersion "${VER_MAJOR}.${VER_MINOR}.0.0"

OutFile "${PROGRAM}-${VER_MAJOR}.${VER_MINOR}.exe"

; The default installation directory
;InstallDir $DESKTOP\${PROGRAM}
InstallDir $PROGRAMFILES\${PROGRAM}

; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM "Software\${PROGRAM}" "Install_Dir"

; Request application privileges for Windows Vista
RequestExecutionLevel admin

; License
LicenseData "..\COPYING"

; Options
AutoCloseWindow true
Icon "..\..\graphics\data\images\image_icon.ico"
XPStyle on

;--------------------------------

; Pages

Page license
Page components
Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

;--------------------------------

; the stuff to install
Section "${PROGRAM} (required)"

SectionIn RO
  
; set output path to the installation directory
SetOutPath $INSTDIR

; put files there (3rd party)
!if ${release} == "Debug"
File "D:\software\Develop\msvcr100d.dll" ; *TODO*: remove this ASAP
File "D:\projects\ACE_wrappers\lib\ACEd.dll"
!else
File "D:\projects\ACE_wrappers\lib\ACE.dll"
!endif

File "D:\projects\SDL-1.2.15\VisualC\SDL\${release}\SDL.dll"
File "D:\projects\SDL_mixer-1.2.12\lib\x86\libogg-0.dll"
File "D:\projects\SDL_mixer-1.2.12\lib\x86\libvorbis-0.dll"
File "D:\projects\SDL_mixer-1.2.12\lib\x86\libvorbisfile-3.dll"
File "D:\projects\SDL_mixer-1.2.12\lib\x86\libmikmod-2.dll"
File "D:\projects\SDL_mixer-1.2.12\lib\x86\SDL_mixer.dll"
File "D:\projects\SDL_ttf-2.0.11\lib\x86\libfreetype-6.dll"
File "D:\projects\SDL_ttf-2.0.11\lib\x86\SDL_ttf.dll"

File "D:\projects\lpng167\projects\vstudio\${release}\libpng16.dll"

File "D:\projects\gtk\bin\freetype6.dll"
File "D:\projects\gtk\bin\intl.dll"
File "D:\projects\gtk\bin\libatk-1.0-0.dll"
File "D:\projects\gtk\bin\libcairo-2.dll"
File "D:\projects\gtk\bin\libexpat-1.dll"
File "D:\projects\gtk\bin\libfontconfig-1.dll"
File "D:\projects\gtk\bin\libgdk_pixbuf-2.0-0.dll"
File "D:\projects\gtk\bin\libgdk-win32-2.0-0.dll"
File "D:\projects\gtk\bin\libgio-2.0-0.dll"
File "D:\projects\gtk\bin\libglib-2.0-0.dll"
File "D:\projects\gtk\bin\libgmodule-2.0-0.dll"
File "D:\projects\gtk\bin\libgobject-2.0-0.dll"
File "D:\projects\gtk\bin\libgthread-2.0-0.dll"
File "D:\projects\gtk\bin\libgtk-win32-2.0-0.dll"
File "D:\projects\gtk\bin\libpango-1.0-0.dll"
File "D:\projects\gtk\bin\libpangocairo-1.0-0.dll"
File "D:\projects\gtk\bin\libpangoft2-1.0-0.dll"
File "D:\projects\gtk\bin\libpangowin32-1.0-0.dll"
File "D:\projects\gtk\bin\libpng14-14.dll"
File "D:\projects\gtk\bin\zlib1.dll"

File "D:\projects\libglade\bin\libglade-2.0-0.dll"

File "D:\software\Develop\libiconv-2.dll"
File "D:\software\Develop\libxml2-2.dll"

File "..\..\Yarp\${release}\rpg_dice.dll"
File "..\..\Yarp\${release}\rpg_chance.dll"
File "..\..\Yarp\${release}\rpg_common.dll"
File "..\..\Yarp\${release}\rpg_magic.dll"
File "..\..\Yarp\${release}\rpg_character.dll"
File "..\..\Yarp\${release}\rpg_item.dll"
File "..\..\Yarp\${release}\rpg_player.dll"
File "..\..\Yarp\${release}\rpg_monster.dll"
File "..\..\Yarp\${release}\rpg_combat.dll"
File "..\..\Yarp\${release}\rpg_map.dll"
File "..\..\Yarp\${release}\rpg_engine.dll"
File "..\..\Yarp\${release}\rpg_sound.dll"
File "..\..\Yarp\${release}\rpg_graphics.dll"
File "..\..\Yarp\${release}\rpg_stream.dll"
File "..\..\Yarp\${release}\rpg_net.dll"
File "..\..\Yarp\${release}\rpg_protocol.dll"
File "..\..\Yarp\${release}\rpg_net_client.dll"
File "..\..\Yarp\${release}\rpg_client.dll"

File "..\..\Yarp\${release}\test_u_chance_dice.exe"
File "..\..\Yarp\${release}\test_u_chance_rangeToRoll.exe"
File "..\..\Yarp\${release}\test_u_character_generator.exe"
File "..\..\Yarp\${release}\test_u_character_generator_gui.exe"
File "..\..\Yarp\${release}\test_u_character_parser.exe"
File "..\..\Yarp\${release}\test_u_client_gui.exe"
File "..\..\Yarp\${release}\test_u_combat_simulator.exe"
File "..\..\Yarp\${release}\test_u_graphics_parser.exe"
File "..\..\Yarp\${release}\test_u_graphics_SDL_gui.exe"
File "..\..\Yarp\${release}\test_u_item_parser.exe"
File "..\..\Yarp\${release}\test_u_magic_parser.exe"
File "..\..\Yarp\${release}\test_u_map_generator.exe"
File "..\..\Yarp\${release}\test_u_map_parser.exe"
File "..\..\Yarp\${release}\test_u_map_path_finder.exe"
File "..\..\Yarp\${release}\test_u_map_path_finder_gui.exe"
File "..\..\Yarp\${release}\test_u_map_vision.exe"
File "..\..\Yarp\${release}\test_u_map_vision_gui.exe"
File "..\..\Yarp\${release}\test_u_monster_parser.exe"
File "..\..\Yarp\${release}\test_u_net_client.exe"
File "..\..\Yarp\${release}\test_u_net_protocol_client.exe"
File "..\..\Yarp\${release}\test_u_net_protocol_client_gui.exe"
File "..\..\Yarp\${release}\test_u_net_server.exe"
File "..\..\Yarp\${release}\test_u_sound_parser.exe"

; Config
; set output path to the installation directory
SetOutPath $INSTDIR\${CONFIGURATION_SUBDIR}

; Config - XML
File "..\..\magic\rpg_magic.xml"
File "..\..\item\rpg_item.xml"
File "..\..\character\monster\rpg_monster.xml"
File "..\..\graphics\rpg_graphics.xml"
File "..\..\sound\rpg_sound.xml"

; Config - XSD schema
File "..\..\engine\rpg_engine.xsd"
File "..\..\character\player\rpg_player.xsd"
File "..\..\map\rpg_map.xsd"
File "..\..\chance\dice\rpg_dice.xsd"
File "..\..\common\rpg_common.xsd"
File "..\..\common\rpg_common_environment.xsd"
File "..\..\character\rpg_character.xsd"
File "..\..\magic\rpg_magic.xsd"
File "..\..\item\rpg_item.xsd"
File "..\..\graphics\rpg_graphics.xsd"
File "..\..\character\monster\rpg_monster.xsd"
File "..\..\combat\rpg_combat.xsd"
File "..\..\sound\rpg_sound.xsd"

; Config - glade
File "..\..\client\rpg_client.glade"
File "..\..\test_u\character\player\character_generator_gui.glade"
File "..\..\test_u\net\protocol\IRC_client_channel_tab.glade"
File "..\..\test_u\net\protocol\IRC_client_main.glade"
File "..\..\test_u\net\protocol\IRC_client_server_page.glade"

; Config - ini
; set output path to the installation directory
SetOutPath $APPDATA\${PROGRAM}
File "..\..\client\rpg_client.ini"
File "..\..\test_u\net\protocol\IRC_client.ini"
File "..\..\test_u\net\protocol\servers.ini"

; Data
; set output path to the installation directory
SetOutPath $INSTDIR\${DATA_SUBDIR}\graphics\creatures

File "..\..\graphics\data\creatures\goblin.png"
File "..\..\graphics\data\creatures\human.png"
File "..\..\graphics\data\creatures\priest.png"

; set output path to the installation directory
SetOutPath $INSTDIR\${DATA_SUBDIR}\graphics\cursors

File "..\..\graphics\data\cursors\cursor_door_open.png"
File "..\..\graphics\data\cursors\cursor_goblet.png"
File "..\..\graphics\data\cursors\cursor_hourglass.png"
File "..\..\graphics\data\cursors\cursor_normal.png"
File "..\..\graphics\data\cursors\cursor_scroll_down.png"
File "..\..\graphics\data\cursors\cursor_scroll_downleft.png"
File "..\..\graphics\data\cursors\cursor_scroll_downright.png"
File "..\..\graphics\data\cursors\cursor_scroll_left.png"
File "..\..\graphics\data\cursors\cursor_scroll_right.png"
File "..\..\graphics\data\cursors\cursor_scroll_up.png"
File "..\..\graphics\data\cursors\cursor_scroll_upleft.png"
File "..\..\graphics\data\cursors\cursor_scroll_upright.png"
File "..\..\graphics\data\cursors\cursor_stairs.png"
File "..\..\graphics\data\cursors\cursor_target_green.png"
File "..\..\graphics\data\cursors\cursor_target_help.png"
File "..\..\graphics\data\cursors\cursor_target_invalid.png"
File "..\..\graphics\data\cursors\cursor_target_red.png"

; set output path to the installation directory
SetOutPath $INSTDIR\${DATA_SUBDIR}\graphics\doors

File "..\..\graphics\data\doors\door_wood_broken.png"
File "..\..\graphics\data\doors\door_wood_closed_h.png"
File "..\..\graphics\data\doors\door_wood_closed_v.png"
File "..\..\graphics\data\doors\door_wood_open_h.png"
File "..\..\graphics\data\doors\door_wood_open_v.png"
File "..\..\graphics\data\doors\drawbridge_wood_closed_h.png"
File "..\..\graphics\data\doors\drawbridge_wood_closed_v.png"
File "..\..\graphics\data\doors\drawbridge_wood_open_h.png"
File "..\..\graphics\data\doors\drawbridge_wood_open_v.png"

; set output path to the installation directory
SetOutPath $INSTDIR\${DATA_SUBDIR}\graphics\floors

File "..\..\graphics\data\floors\floor_air_0_0.png"
File "..\..\graphics\data\floors\floor_air_0_1.png"
File "..\..\graphics\data\floors\floor_air_0_2.png"
File "..\..\graphics\data\floors\floor_air_1_0.png"
File "..\..\graphics\data\floors\floor_air_1_1.png"
File "..\..\graphics\data\floors\floor_air_1_2.png"
File "..\..\graphics\data\floors\floor_air_2_0.png"
File "..\..\graphics\data\floors\floor_air_2_1.png"
File "..\..\graphics\data\floors\floor_air_2_2.png"

File "..\..\graphics\data\floors\floor_carpet_0_0.png"
File "..\..\graphics\data\floors\floor_carpet_0_1.png"
File "..\..\graphics\data\floors\floor_carpet_1_0.png"
File "..\..\graphics\data\floors\floor_carpet_1_1.png"
File "..\..\graphics\data\floors\floor_carpet_2_0.png"
File "..\..\graphics\data\floors\floor_carpet_2_1.png"

File "..\..\graphics\data\floors\floor_ceramic_0_0.png"
File "..\..\graphics\data\floors\floor_ceramic_0_1.png"
File "..\..\graphics\data\floors\floor_ceramic_0_2.png"
File "..\..\graphics\data\floors\floor_ceramic_1_0.png"
File "..\..\graphics\data\floors\floor_ceramic_1_1.png"
File "..\..\graphics\data\floors\floor_ceramic_1_2.png"
File "..\..\graphics\data\floors\floor_ceramic_2_0.png"
File "..\..\graphics\data\floors\floor_ceramic_2_1.png"
File "..\..\graphics\data\floors\floor_ceramic_2_2.png"

File "..\..\graphics\data\floors\floor_cobblestone_0_0.png"
File "..\..\graphics\data\floors\floor_cobblestone_0_1.png"
File "..\..\graphics\data\floors\floor_cobblestone_0_2.png"
File "..\..\graphics\data\floors\floor_cobblestone_1_0.png"
File "..\..\graphics\data\floors\floor_cobblestone_1_1.png"
File "..\..\graphics\data\floors\floor_cobblestone_1_2.png"
File "..\..\graphics\data\floors\floor_cobblestone_2_0.png"
File "..\..\graphics\data\floors\floor_cobblestone_2_1.png"
File "..\..\graphics\data\floors\floor_cobblestone_2_2.png"

File "..\..\graphics\data\floors\floor_cobblestone_edge1.png"
File "..\..\graphics\data\floors\floor_cobblestone_edge2.png"
File "..\..\graphics\data\floors\floor_cobblestone_edge3.png"
File "..\..\graphics\data\floors\floor_cobblestone_edge4.png"
File "..\..\graphics\data\floors\floor_cobblestone_edge5.png"
File "..\..\graphics\data\floors\floor_cobblestone_edge6.png"
File "..\..\graphics\data\floors\floor_cobblestone_edge7.png"
File "..\..\graphics\data\floors\floor_cobblestone_edge8.png"
File "..\..\graphics\data\floors\floor_cobblestone_edge9.png"
File "..\..\graphics\data\floors\floor_cobblestone_edge10.png"
File "..\..\graphics\data\floors\floor_cobblestone_edge11.png"
File "..\..\graphics\data\floors\floor_cobblestone_edge12.png"

File "..\..\graphics\data\floors\floor_dark_0_0.png"

File "..\..\graphics\data\floors\floor_highlight.png"

File "..\..\graphics\data\floors\floor_ice_0_0.png"
File "..\..\graphics\data\floors\floor_ice_0_1.png"
File "..\..\graphics\data\floors\floor_ice_0_2.png"
File "..\..\graphics\data\floors\floor_ice_1_0.png"
File "..\..\graphics\data\floors\floor_ice_1_1.png"
File "..\..\graphics\data\floors\floor_ice_1_2.png"
File "..\..\graphics\data\floors\floor_ice_2_0.png"
File "..\..\graphics\data\floors\floor_ice_2_1.png"
File "..\..\graphics\data\floors\floor_ice_2_2.png"

File "..\..\graphics\data\floors\floor_invisible.png"

File "..\..\graphics\data\floors\floor_lava_0_0.png"
File "..\..\graphics\data\floors\floor_lava_0_1.png"
File "..\..\graphics\data\floors\floor_lava_0_2.png"
File "..\..\graphics\data\floors\floor_lava_1_0.png"
File "..\..\graphics\data\floors\floor_lava_1_1.png"
File "..\..\graphics\data\floors\floor_lava_1_2.png"
File "..\..\graphics\data\floors\floor_lava_2_0.png"
File "..\..\graphics\data\floors\floor_lava_2_1.png"
File "..\..\graphics\data\floors\floor_lava_2_2.png"

File "..\..\graphics\data\floors\floor_marble_0_0.png"
File "..\..\graphics\data\floors\floor_marble_0_1.png"
File "..\..\graphics\data\floors\floor_marble_0_2.png"
File "..\..\graphics\data\floors\floor_marble_1_0.png"
File "..\..\graphics\data\floors\floor_marble_1_1.png"
File "..\..\graphics\data\floors\floor_marble_1_2.png"
File "..\..\graphics\data\floors\floor_marble_2_0.png"
File "..\..\graphics\data\floors\floor_marble_2_1.png"
File "..\..\graphics\data\floors\floor_marble_2_2.png"

File "..\..\graphics\data\floors\floor_moss_covered_0_0.png"
File "..\..\graphics\data\floors\floor_moss_covered_0_1.png"
File "..\..\graphics\data\floors\floor_moss_covered_0_2.png"
File "..\..\graphics\data\floors\floor_moss_covered_1_0.png"
File "..\..\graphics\data\floors\floor_moss_covered_1_1.png"
File "..\..\graphics\data\floors\floor_moss_covered_1_2.png"
File "..\..\graphics\data\floors\floor_moss_covered_2_0.png"
File "..\..\graphics\data\floors\floor_moss_covered_2_1.png"
File "..\..\graphics\data\floors\floor_moss_covered_2_2.png"

File "..\..\graphics\data\floors\floor_mural2_0_0.png"
File "..\..\graphics\data\floors\floor_mural2_0_1.png"
File "..\..\graphics\data\floors\floor_mural2_1_0.png"
File "..\..\graphics\data\floors\floor_mural2_1_1.png"
File "..\..\graphics\data\floors\floor_mural2_2_0.png"
File "..\..\graphics\data\floors\floor_mural2_2_1.png"

File "..\..\graphics\data\floors\floor_mural_0_0.png"
File "..\..\graphics\data\floors\floor_mural_0_1.png"
File "..\..\graphics\data\floors\floor_mural_1_0.png"
File "..\..\graphics\data\floors\floor_mural_1_1.png"
File "..\..\graphics\data\floors\floor_mural_2_0.png"
File "..\..\graphics\data\floors\floor_mural_2_1.png"

File "..\..\graphics\data\floors\floor_rough_0_0.png"
File "..\..\graphics\data\floors\floor_rough_0_1.png"
File "..\..\graphics\data\floors\floor_rough_0_2.png"
File "..\..\graphics\data\floors\floor_rough_1_0.png"
File "..\..\graphics\data\floors\floor_rough_1_1.png"
File "..\..\graphics\data\floors\floor_rough_1_2.png"
File "..\..\graphics\data\floors\floor_rough_2_0.png"
File "..\..\graphics\data\floors\floor_rough_2_1.png"
File "..\..\graphics\data\floors\floor_rough_2_2.png"

File "..\..\graphics\data\floors\floor_rough_lit_0_0.png"
File "..\..\graphics\data\floors\floor_rough_lit_0_1.png"
File "..\..\graphics\data\floors\floor_rough_lit_0_2.png"
File "..\..\graphics\data\floors\floor_rough_lit_1_0.png"
File "..\..\graphics\data\floors\floor_rough_lit_1_1.png"
File "..\..\graphics\data\floors\floor_rough_lit_1_2.png"
File "..\..\graphics\data\floors\floor_rough_lit_2_0.png"
File "..\..\graphics\data\floors\floor_rough_lit_2_1.png"
File "..\..\graphics\data\floors\floor_rough_lit_2_2.png"

File "..\..\graphics\data\floors\floor_water_0_0.png"
File "..\..\graphics\data\floors\floor_water_0_1.png"
File "..\..\graphics\data\floors\floor_water_0_2.png"
File "..\..\graphics\data\floors\floor_water_1_0.png"
File "..\..\graphics\data\floors\floor_water_1_1.png"
File "..\..\graphics\data\floors\floor_water_1_2.png"
File "..\..\graphics\data\floors\floor_water_2_0.png"
File "..\..\graphics\data\floors\floor_water_2_1.png"
File "..\..\graphics\data\floors\floor_water_2_2.png"

; set output path to the installation directory
SetOutPath $INSTDIR\${DATA_SUBDIR}\graphics\fonts

File "..\..\graphics\data\fonts\font_main.ttf"

; set output path to the installation directory
SetOutPath $INSTDIR\${DATA_SUBDIR}\graphics\images

File "..\..\graphics\data\images\image_icon.png"
File "..\..\graphics\data\images\image_main_logo.png"
File "..\..\graphics\data\images\interface_elements.png"
File "..\..\graphics\data\images\interface_minimap_bg.png"

; set output path to the installation directory
SetOutPath $INSTDIR\${DATA_SUBDIR}\graphics\walls

File "..\..\graphics\data\walls\wall_brick_banner_f_e.png"
File "..\..\graphics\data\walls\wall_brick_banner_f_n.png"
File "..\..\graphics\data\walls\wall_brick_banner_f_s.png"
File "..\..\graphics\data\walls\wall_brick_banner_f_w.png"
File "..\..\graphics\data\walls\wall_brick_banner_h_e.png"
File "..\..\graphics\data\walls\wall_brick_banner_h_n.png"
File "..\..\graphics\data\walls\wall_brick_banner_h_s.png"
File "..\..\graphics\data\walls\wall_brick_banner_h_w.png"

File "..\..\graphics\data\walls\wall_brick_f_e.png"
File "..\..\graphics\data\walls\wall_brick_f_n.png"
File "..\..\graphics\data\walls\wall_brick_f_s.png"
File "..\..\graphics\data\walls\wall_brick_f_w.png"
File "..\..\graphics\data\walls\wall_brick_h_e.png"
File "..\..\graphics\data\walls\wall_brick_h_n.png"
File "..\..\graphics\data\walls\wall_brick_h_s.png"
File "..\..\graphics\data\walls\wall_brick_h_w.png"

File "..\..\graphics\data\walls\wall_brick_painting_f_e.png"
File "..\..\graphics\data\walls\wall_brick_painting_f_n.png"
File "..\..\graphics\data\walls\wall_brick_painting_f_s.png"
File "..\..\graphics\data\walls\wall_brick_painting_f_w.png"
File "..\..\graphics\data\walls\wall_brick_painting_h_e.png"
File "..\..\graphics\data\walls\wall_brick_painting_h_n.png"
File "..\..\graphics\data\walls\wall_brick_painting_h_s.png"
File "..\..\graphics\data\walls\wall_brick_painting_h_w.png"

File "..\..\graphics\data\walls\wall_brick_pillar_f_e.png"
File "..\..\graphics\data\walls\wall_brick_pillar_f_n.png"
File "..\..\graphics\data\walls\wall_brick_pillar_f_s.png"
File "..\..\graphics\data\walls\wall_brick_pillar_f_w.png"
File "..\..\graphics\data\walls\wall_brick_pillar_h_e.png"
File "..\..\graphics\data\walls\wall_brick_pillar_h_n.png"
File "..\..\graphics\data\walls\wall_brick_pillar_h_s.png"
File "..\..\graphics\data\walls\wall_brick_pillar_h_w.png"

File "..\..\graphics\data\walls\wall_brick_pocket_f_e.png"
File "..\..\graphics\data\walls\wall_brick_pocket_f_n.png"
File "..\..\graphics\data\walls\wall_brick_pocket_f_s.png"
File "..\..\graphics\data\walls\wall_brick_pocket_f_w.png"
File "..\..\graphics\data\walls\wall_brick_pocket_h_e.png"
File "..\..\graphics\data\walls\wall_brick_pocket_h_n.png"
File "..\..\graphics\data\walls\wall_brick_pocket_h_s.png"
File "..\..\graphics\data\walls\wall_brick_pocket_h_w.png"

File "..\..\graphics\data\walls\wall_dark_f_e.png"
File "..\..\graphics\data\walls\wall_dark_f_n.png"
File "..\..\graphics\data\walls\wall_dark_f_s.png"
File "..\..\graphics\data\walls\wall_dark_f_w.png"
File "..\..\graphics\data\walls\wall_dark_h_e.png"
File "..\..\graphics\data\walls\wall_dark_h_n.png"
File "..\..\graphics\data\walls\wall_dark_h_s.png"
File "..\..\graphics\data\walls\wall_dark_h_w.png"

File "..\..\graphics\data\walls\wall_light_f_e.png"
File "..\..\graphics\data\walls\wall_light_f_n.png"
File "..\..\graphics\data\walls\wall_light_f_s.png"
File "..\..\graphics\data\walls\wall_light_f_w.png"
File "..\..\graphics\data\walls\wall_light_h_e.png"
File "..\..\graphics\data\walls\wall_light_h_n.png"
File "..\..\graphics\data\walls\wall_light_h_s.png"
File "..\..\graphics\data\walls\wall_light_h_w.png"

File "..\..\graphics\data\walls\wall_marble_f_e.png"
File "..\..\graphics\data\walls\wall_marble_f_n.png"
File "..\..\graphics\data\walls\wall_marble_f_s.png"
File "..\..\graphics\data\walls\wall_marble_f_w.png"
File "..\..\graphics\data\walls\wall_marble_h_e.png"
File "..\..\graphics\data\walls\wall_marble_h_n.png"
File "..\..\graphics\data\walls\wall_marble_h_s.png"
File "..\..\graphics\data\walls\wall_marble_h_w.png"

File "..\..\graphics\data\walls\wall_rough_f_e.png"
File "..\..\graphics\data\walls\wall_rough_f_n.png"
File "..\..\graphics\data\walls\wall_rough_f_s.png"
File "..\..\graphics\data\walls\wall_rough_f_w.png"
File "..\..\graphics\data\walls\wall_rough_h_e.png"
File "..\..\graphics\data\walls\wall_rough_h_n.png"
File "..\..\graphics\data\walls\wall_rough_h_s.png"
File "..\..\graphics\data\walls\wall_rough_h_w.png"

File "..\..\graphics\data\walls\wall_stucco_f_e.png"
File "..\..\graphics\data\walls\wall_stucco_f_n.png"
File "..\..\graphics\data\walls\wall_stucco_f_s.png"
File "..\..\graphics\data\walls\wall_stucco_f_w.png"
File "..\..\graphics\data\walls\wall_stucco_h_e.png"
File "..\..\graphics\data\walls\wall_stucco_h_n.png"
File "..\..\graphics\data\walls\wall_stucco_h_s.png"
File "..\..\graphics\data\walls\wall_stucco_h_w.png"

File "..\..\graphics\data\walls\wall_vine_covered_f_e.png"
File "..\..\graphics\data\walls\wall_vine_covered_f_n.png"
File "..\..\graphics\data\walls\wall_vine_covered_f_s.png"
File "..\..\graphics\data\walls\wall_vine_covered_f_w.png"
File "..\..\graphics\data\walls\wall_vine_covered_h_e.png"
File "..\..\graphics\data\walls\wall_vine_covered_h_n.png"
File "..\..\graphics\data\walls\wall_vine_covered_h_s.png"
File "..\..\graphics\data\walls\wall_vine_covered_h_w.png"

; set output path to the installation directory
SetOutPath $INSTDIR\${DATA_SUBDIR}\sound\ambient

File "..\..\sound\data\ambient\music_main_title.ogg"

; set output path to the installation directory
SetOutPath $INSTDIR\${DATA_SUBDIR}\sound\effect

File "..\..\sound\data\effect\sound_bell.ogg"
File "..\..\sound\data\effect\sound_condition_ill.ogg"
File "..\..\sound\data\effect\sound_condition_weak.ogg"
File "..\..\sound\data\effect\sound_counting.ogg"
File "..\..\sound\data\effect\sound_door_close.ogg"
File "..\..\sound\data\effect\sound_door_open.ogg"
File "..\..\sound\data\effect\sound_fountain.ogg"
File "..\..\sound\data\effect\sound_sword_hit.ogg"
File "..\..\sound\data\effect\sound_sword_miss.ogg"
File "..\..\sound\data\effect\sound_walk.ogg"
File "..\..\sound\data\effect\sound_xp_levelup.ogg"

; set output path to the installation directory
SetOutPath $INSTDIR\${DATA_SUBDIR}\maps
File "..\..\engine\data\default_level.level"

; set output path to the application data directory
SetOutPath $APPDATA\${PROGRAM}\profiles
File "..\..\engine\data\default_player.profile"

; Write the installation path into the registry
WriteRegStr HKLM SOFTWARE\${PROGRAM} "Install_Dir" "$INSTDIR"

; Write the uninstall keys for Windows
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PROGRAM}" "DisplayName" "${PROGRAM}"
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PROGRAM}" "UninstallString" '"$INSTDIR\uninstall.exe"'
WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PROGRAM}" "NoModify" 1
WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PROGRAM}" "NoRepair" 1
WriteUninstaller "uninstall.exe"

SectionEnd

;--------------------------------

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"

CreateDirectory "$SMPROGRAMS\${PROGRAM}"
CreateShortCut "$SMPROGRAMS\${PROGRAM}\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
CreateShortCut "$SMPROGRAMS\${PROGRAM}\${PROGRAM}.lnk" "$INSTDIR\test_u_client_gui.exe" "" "$INSTDIR\test_u_client_gui.exe" 0

SectionEnd

;--------------------------------

; Uninstaller

Section "Uninstall"

; Auto-Close
SetAutoClose true

; Remove registry keys
DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PROGRAM}"
DeleteRegKey HKLM SOFTWARE\${PROGRAM}

; Remove files AND uninstaller (yes this works !!!)
;Delete "$INSTDIR\*.*"
; Ask before removing player profiles ? *TODO*
RMDir /r "$APPDATA\${PROGRAM}"

; Remove shortcuts, if any
Delete "$SMPROGRAMS\${PROGRAM}\*.*"

; Remove directories used
RMDir /r "$INSTDIR"
RMDir "$SMPROGRAMS\${PROGRAM}"

SectionEnd
