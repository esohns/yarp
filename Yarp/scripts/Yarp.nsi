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

; put files there (3rd party)
;File "C:\Temp\ACE_wrappers\lib\ACE.dll"
File "C:\Temp\ACE_wrappers\lib\ACEd.dll"

File "C:\Temp\Gtk\bin\freetype6.dll"
File "C:\Temp\Gtk\bin\intl.dll"
File "C:\Temp\Gtk\bin\libatk-1.0-0.dll"
File "C:\Temp\Gtk\bin\libcairo-2.dll"
File "C:\Temp\Gtk\bin\libexpat-1.dll"
File "C:\Temp\Gtk\bin\libfontconfig-1.dll"
File "C:\Temp\Gtk\bin\libgdk_pixbuf-2.0-0.dll"
File "C:\Temp\Gtk\bin\libgdk-win32-2.0-0.dll"
File "C:\Temp\Gtk\bin\libgio-2.0-0.dll"
File "C:\Temp\Gtk\bin\libglib-2.0-0.dll"
File "C:\Temp\Gtk\bin\libgmodule-2.0-0.dll"
File "C:\Temp\Gtk\bin\libgobject-2.0-0.dll"
File "C:\Temp\Gtk\bin\libgthread-2.0-0.dll"
File "C:\Temp\Gtk\bin\libgtk-win32-2.0-0.dll"
File "C:\Temp\Gtk\bin\libpango-1.0-0.dll"
File "C:\Temp\Gtk\bin\libpangocairo-1.0-0.dll"
File "C:\Temp\Gtk\bin\libpangoft2-1.0-0.dll"
File "C:\Temp\Gtk\bin\libpangowin32-1.0-0.dll"
File "C:\Temp\Gtk\bin\libpng14-14.dll"
File "C:\Temp\Gtk\bin\zlib1.dll"

File "C:\Temp\SDL\lib\x86\libfreetype-6.dll"
File "C:\Temp\SDL\lib\x86\libogg-0.dll"
File "C:\Temp\SDL\lib\x86\libvorbis-0.dll"
File "C:\Temp\SDL\lib\x86\libvorbisfile-3.dll"
File "C:\Temp\SDL\lib\x86\libmikmod-2.dll"
File "C:\Temp\SDL\lib\x86\SDL.dll"
File "C:\Temp\SDL\lib\x86\SDL_mixer.dll"
File "C:\Temp\SDL\lib\x86\SDL_ttf.dll"

File "C:\Temp\libglade\bin\libglade-2.0-0.dll"

File "C:\Temp\lpng157\projects\vstudio\Release\libpng15.dll"

File "D:\software\Develop\libiconv-2.dll"
File "D:\software\Develop\libxml2-2.dll"

File "C:\Temp\yarp\Yarp\Debug\dice.dll"
File "C:\Temp\yarp\Yarp\Debug\chance.dll"
File "C:\Temp\yarp\Yarp\Debug\common.dll"
File "C:\Temp\yarp\Yarp\Debug\magic.dll"
File "C:\Temp\yarp\Yarp\Debug\character.dll"
File "C:\Temp\yarp\Yarp\Debug\item.dll"
File "C:\Temp\yarp\Yarp\Debug\player.dll"
File "C:\Temp\yarp\Yarp\Debug\monster.dll"
File "C:\Temp\yarp\Yarp\Debug\combat.dll"
File "C:\Temp\yarp\Yarp\Debug\map.dll"
File "C:\Temp\yarp\Yarp\Debug\engine.dll"
File "C:\Temp\yarp\Yarp\Debug\sound.dll"
File "C:\Temp\yarp\Yarp\Debug\graphics.dll"
File "C:\Temp\yarp\Yarp\Debug\stream.dll"
File "C:\Temp\yarp\Yarp\Debug\net.dll"
File "C:\Temp\yarp\Yarp\Debug\protocol.dll"
File "C:\Temp\yarp\Yarp\Debug\client.dll"

File "C:\Temp\yarp\Yarp\Debug\test_u_chance_dice.exe"
File "C:\Temp\yarp\Yarp\Debug\test_u_chance_rangeToRoll.exe"
File "C:\Temp\yarp\Yarp\Debug\test_u_character_generator.exe"
File "C:\Temp\yarp\Yarp\Debug\test_u_character_generator_gui.exe"
File "C:\Temp\yarp\Yarp\Debug\test_u_character_parser.exe"
File "C:\Temp\yarp\Yarp\Debug\test_u_client_gui.exe"
File "C:\Temp\yarp\Yarp\Debug\test_u_combat_simulator.exe"
File "C:\Temp\yarp\Yarp\Debug\test_u_graphics_parser.exe"
File "C:\Temp\yarp\Yarp\Debug\test_u_graphics_SDL_gui.exe"
File "C:\Temp\yarp\Yarp\Debug\test_u_item_parser.exe"
File "C:\Temp\yarp\Yarp\Debug\test_u_magic_parser.exe"
File "C:\Temp\yarp\Yarp\Debug\test_u_map_generator.exe"
File "C:\Temp\yarp\Yarp\Debug\test_u_map_parser.exe"
File "C:\Temp\yarp\Yarp\Debug\test_u_map_path_finder.exe"
File "C:\Temp\yarp\Yarp\Debug\test_u_map_path_finder_gui.exe"
File "C:\Temp\yarp\Yarp\Debug\test_u_map_vision.exe"
File "C:\Temp\yarp\Yarp\Debug\test_u_map_vision_gui.exe"
File "C:\Temp\yarp\Yarp\Debug\test_u_monster_parser.exe"
File "C:\Temp\yarp\Yarp\Debug\test_u_net_client.exe"
File "C:\Temp\yarp\Yarp\Debug\test_u_net_protocol_client.exe"
File "C:\Temp\yarp\Yarp\Debug\test_u_net_protocol_client_gui.exe"
File "C:\Temp\yarp\Yarp\Debug\test_u_net_server.exe"
File "C:\Temp\yarp\Yarp\Debug\test_u_sound_parser.exe"

; Config
; set output path to the installation directory
SetOutPath $INSTDIR\config

; Config - XML
File "C:\Temp\yarp\magic\rpg_magic.xml"
File "C:\Temp\yarp\item\rpg_item.xml"
File "C:\Temp\yarp\character\monster\rpg_monster.xml"
File "C:\Temp\yarp\graphics\rpg_graphics.xml"
File "C:\Temp\yarp\sound\rpg_sound.xml"

; Config - XSD schema
File "C:\Temp\yarp\engine\rpg_engine.xsd"
File "C:\Temp\yarp\character\player\rpg_player.xsd"
File "C:\Temp\yarp\map\rpg_map.xsd"
File "C:\Temp\yarp\chance\dice\rpg_dice.xsd"
File "C:\Temp\yarp\common\rpg_common.xsd"
File "C:\Temp\yarp\common\rpg_common_environment.xsd"
File "C:\Temp\yarp\character\rpg_character.xsd"
File "C:\Temp\yarp\magic\rpg_magic.xsd"

; Config - glade
File "C:\Temp\yarp\client\rpg_client.glade"
File "C:\Temp\yarp\test_u\character\player\character_generator_gui.glade"
File "C:\Temp\yarp\test_u\net\protocol\IRC_client_channel_tab.glade"
File "C:\Temp\yarp\test_u\net\protocol\IRC_client_main.glade"
File "C:\Temp\yarp\test_u\net\protocol\IRC_client_server_page.glade"

; Config - ini
File "C:\Temp\yarp\client\rpg_client.ini"
File "C:\Temp\yarp\test_u\net\protocol\IRC_client.ini"
File "C:\Temp\yarp\test_u\net\protocol\servers.ini"

; Data
; set output path to the installation directory
SetOutPath $INSTDIR\data\graphics\creatures

File "C:\Temp\yarp\graphics\data\creatures\goblin.png"
File "C:\Temp\yarp\graphics\data\creatures\human.png"
File "C:\Temp\yarp\graphics\data\creatures\priest.png"

; set output path to the installation directory
SetOutPath $INSTDIR\data\graphics\cursors

File "C:\Temp\yarp\graphics\data\cursors\cursor_door_open.png"
File "C:\Temp\yarp\graphics\data\cursors\cursor_goblet.png"
File "C:\Temp\yarp\graphics\data\cursors\cursor_hourglass.png"
File "C:\Temp\yarp\graphics\data\cursors\cursor_normal.png"
File "C:\Temp\yarp\graphics\data\cursors\cursor_scroll_down.png"
File "C:\Temp\yarp\graphics\data\cursors\cursor_scroll_downleft.png"
File "C:\Temp\yarp\graphics\data\cursors\cursor_scroll_downright.png"
File "C:\Temp\yarp\graphics\data\cursors\cursor_scroll_left.png"
File "C:\Temp\yarp\graphics\data\cursors\cursor_scroll_right.png"
File "C:\Temp\yarp\graphics\data\cursors\cursor_scroll_up.png"
File "C:\Temp\yarp\graphics\data\cursors\cursor_scroll_upleft.png"
File "C:\Temp\yarp\graphics\data\cursors\cursor_scroll_upright.png"
File "C:\Temp\yarp\graphics\data\cursors\cursor_stairs.png"
File "C:\Temp\yarp\graphics\data\cursors\cursor_target_green.png"
File "C:\Temp\yarp\graphics\data\cursors\cursor_target_help.png"
File "C:\Temp\yarp\graphics\data\cursors\cursor_target_invalid.png"
File "C:\Temp\yarp\graphics\data\cursors\cursor_target_red.png"

; set output path to the installation directory
SetOutPath $INSTDIR\data\graphics\doors

File "C:\Temp\yarp\graphics\data\doors\door_wood_broken.png"
File "C:\Temp\yarp\graphics\data\doors\door_wood_closed_h.png"
File "C:\Temp\yarp\graphics\data\doors\door_wood_closed_v.png"
File "C:\Temp\yarp\graphics\data\doors\door_wood_open_h.png"
File "C:\Temp\yarp\graphics\data\doors\door_wood_open_v.png"
File "C:\Temp\yarp\graphics\data\doors\drawbridge_wood_closed_h.png"
File "C:\Temp\yarp\graphics\data\doors\drawbridge_wood_closed_v.png"
File "C:\Temp\yarp\graphics\data\doors\drawbridge_wood_open_h.png"
File "C:\Temp\yarp\graphics\data\doors\drawbridge_wood_open_v.png"

; set output path to the installation directory
SetOutPath $INSTDIR\data\graphics\floors

File "C:\Temp\yarp\graphics\data\floors\floor_air_0_0.png"
File "C:\Temp\yarp\graphics\data\floors\floor_air_0_1.png"
File "C:\Temp\yarp\graphics\data\floors\floor_air_0_2.png"
File "C:\Temp\yarp\graphics\data\floors\floor_air_1_0.png"
File "C:\Temp\yarp\graphics\data\floors\floor_air_1_1.png"
File "C:\Temp\yarp\graphics\data\floors\floor_air_1_2.png"
File "C:\Temp\yarp\graphics\data\floors\floor_air_2_0.png"
File "C:\Temp\yarp\graphics\data\floors\floor_air_2_1.png"
File "C:\Temp\yarp\graphics\data\floors\floor_air_2_2.png"

File "C:\Temp\yarp\graphics\data\floors\floor_carpet_0_0.png"
File "C:\Temp\yarp\graphics\data\floors\floor_carpet_0_1.png"
File "C:\Temp\yarp\graphics\data\floors\floor_carpet_1_0.png"
File "C:\Temp\yarp\graphics\data\floors\floor_carpet_1_1.png"
File "C:\Temp\yarp\graphics\data\floors\floor_carpet_2_0.png"
File "C:\Temp\yarp\graphics\data\floors\floor_carpet_2_1.png"

File "C:\Temp\yarp\graphics\data\floors\floor_ceramic_0_0.png"
File "C:\Temp\yarp\graphics\data\floors\floor_ceramic_0_1.png"
File "C:\Temp\yarp\graphics\data\floors\floor_ceramic_0_2.png"
File "C:\Temp\yarp\graphics\data\floors\floor_ceramic_1_0.png"
File "C:\Temp\yarp\graphics\data\floors\floor_ceramic_1_1.png"
File "C:\Temp\yarp\graphics\data\floors\floor_ceramic_1_2.png"
File "C:\Temp\yarp\graphics\data\floors\floor_ceramic_2_0.png"
File "C:\Temp\yarp\graphics\data\floors\floor_ceramic_2_1.png"
File "C:\Temp\yarp\graphics\data\floors\floor_ceramic_2_2.png"

File "C:\Temp\yarp\graphics\data\floors\floor_cobblestone_0_0.png"
File "C:\Temp\yarp\graphics\data\floors\floor_cobblestone_0_1.png"
File "C:\Temp\yarp\graphics\data\floors\floor_cobblestone_0_2.png"
File "C:\Temp\yarp\graphics\data\floors\floor_cobblestone_1_0.png"
File "C:\Temp\yarp\graphics\data\floors\floor_cobblestone_1_1.png"
File "C:\Temp\yarp\graphics\data\floors\floor_cobblestone_1_2.png"
File "C:\Temp\yarp\graphics\data\floors\floor_cobblestone_2_0.png"
File "C:\Temp\yarp\graphics\data\floors\floor_cobblestone_2_1.png"
File "C:\Temp\yarp\graphics\data\floors\floor_cobblestone_2_2.png"

File "C:\Temp\yarp\graphics\data\floors\floor_cobblestone_edge1.png"
File "C:\Temp\yarp\graphics\data\floors\floor_cobblestone_edge2.png"
File "C:\Temp\yarp\graphics\data\floors\floor_cobblestone_edge3.png"
File "C:\Temp\yarp\graphics\data\floors\floor_cobblestone_edge4.png"
File "C:\Temp\yarp\graphics\data\floors\floor_cobblestone_edge5.png"
File "C:\Temp\yarp\graphics\data\floors\floor_cobblestone_edge6.png"
File "C:\Temp\yarp\graphics\data\floors\floor_cobblestone_edge7.png"
File "C:\Temp\yarp\graphics\data\floors\floor_cobblestone_edge8.png"
File "C:\Temp\yarp\graphics\data\floors\floor_cobblestone_edge9.png"
File "C:\Temp\yarp\graphics\data\floors\floor_cobblestone_edge10.png"
File "C:\Temp\yarp\graphics\data\floors\floor_cobblestone_edge11.png"
File "C:\Temp\yarp\graphics\data\floors\floor_cobblestone_edge12.png"

File "C:\Temp\yarp\graphics\data\floors\floor_dark_0_0.png"

File "C:\Temp\yarp\graphics\data\floors\floor_highlight.png"

File "C:\Temp\yarp\graphics\data\floors\floor_ice_0_0.png"
File "C:\Temp\yarp\graphics\data\floors\floor_ice_0_1.png"
File "C:\Temp\yarp\graphics\data\floors\floor_ice_0_2.png"
File "C:\Temp\yarp\graphics\data\floors\floor_ice_1_0.png"
File "C:\Temp\yarp\graphics\data\floors\floor_ice_1_1.png"
File "C:\Temp\yarp\graphics\data\floors\floor_ice_1_2.png"
File "C:\Temp\yarp\graphics\data\floors\floor_ice_2_0.png"
File "C:\Temp\yarp\graphics\data\floors\floor_ice_2_1.png"
File "C:\Temp\yarp\graphics\data\floors\floor_ice_2_2.png"

File "C:\Temp\yarp\graphics\data\floors\floor_invisible.png"

File "C:\Temp\yarp\graphics\data\floors\floor_lava_0_0.png"
File "C:\Temp\yarp\graphics\data\floors\floor_lava_0_1.png"
File "C:\Temp\yarp\graphics\data\floors\floor_lava_0_2.png"
File "C:\Temp\yarp\graphics\data\floors\floor_lava_1_0.png"
File "C:\Temp\yarp\graphics\data\floors\floor_lava_1_1.png"
File "C:\Temp\yarp\graphics\data\floors\floor_lava_1_2.png"
File "C:\Temp\yarp\graphics\data\floors\floor_lava_2_0.png"
File "C:\Temp\yarp\graphics\data\floors\floor_lava_2_1.png"
File "C:\Temp\yarp\graphics\data\floors\floor_lava_2_2.png"

File "C:\Temp\yarp\graphics\data\floors\floor_marble_0_0.png"
File "C:\Temp\yarp\graphics\data\floors\floor_marble_0_1.png"
File "C:\Temp\yarp\graphics\data\floors\floor_marble_0_2.png"
File "C:\Temp\yarp\graphics\data\floors\floor_marble_1_0.png"
File "C:\Temp\yarp\graphics\data\floors\floor_marble_1_1.png"
File "C:\Temp\yarp\graphics\data\floors\floor_marble_1_2.png"
File "C:\Temp\yarp\graphics\data\floors\floor_marble_2_0.png"
File "C:\Temp\yarp\graphics\data\floors\floor_marble_2_1.png"
File "C:\Temp\yarp\graphics\data\floors\floor_marble_2_2.png"

File "C:\Temp\yarp\graphics\data\floors\floor_moss_covered_0_0.png"
File "C:\Temp\yarp\graphics\data\floors\floor_moss_covered_0_1.png"
File "C:\Temp\yarp\graphics\data\floors\floor_moss_covered_0_2.png"
File "C:\Temp\yarp\graphics\data\floors\floor_moss_covered_1_0.png"
File "C:\Temp\yarp\graphics\data\floors\floor_moss_covered_1_1.png"
File "C:\Temp\yarp\graphics\data\floors\floor_moss_covered_1_2.png"
File "C:\Temp\yarp\graphics\data\floors\floor_moss_covered_2_0.png"
File "C:\Temp\yarp\graphics\data\floors\floor_moss_covered_2_1.png"
File "C:\Temp\yarp\graphics\data\floors\floor_moss_covered_2_2.png"

File "C:\Temp\yarp\graphics\data\floors\floor_mural2_0_0.png"
File "C:\Temp\yarp\graphics\data\floors\floor_mural2_0_1.png"
File "C:\Temp\yarp\graphics\data\floors\floor_mural2_1_0.png"
File "C:\Temp\yarp\graphics\data\floors\floor_mural2_1_1.png"
File "C:\Temp\yarp\graphics\data\floors\floor_mural2_2_0.png"
File "C:\Temp\yarp\graphics\data\floors\floor_mural2_2_1.png"

File "C:\Temp\yarp\graphics\data\floors\floor_mural_0_0.png"
File "C:\Temp\yarp\graphics\data\floors\floor_mural_0_1.png"
File "C:\Temp\yarp\graphics\data\floors\floor_mural_1_0.png"
File "C:\Temp\yarp\graphics\data\floors\floor_mural_1_1.png"
File "C:\Temp\yarp\graphics\data\floors\floor_mural_2_0.png"
File "C:\Temp\yarp\graphics\data\floors\floor_mural_2_1.png"

File "C:\Temp\yarp\graphics\data\floors\floor_rough_0_0.png"
File "C:\Temp\yarp\graphics\data\floors\floor_rough_0_1.png"
File "C:\Temp\yarp\graphics\data\floors\floor_rough_0_2.png"
File "C:\Temp\yarp\graphics\data\floors\floor_rough_1_0.png"
File "C:\Temp\yarp\graphics\data\floors\floor_rough_1_1.png"
File "C:\Temp\yarp\graphics\data\floors\floor_rough_1_2.png"
File "C:\Temp\yarp\graphics\data\floors\floor_rough_2_0.png"
File "C:\Temp\yarp\graphics\data\floors\floor_rough_2_1.png"
File "C:\Temp\yarp\graphics\data\floors\floor_rough_2_2.png"

File "C:\Temp\yarp\graphics\data\floors\floor_rough_lit_0_0.png"
File "C:\Temp\yarp\graphics\data\floors\floor_rough_lit_0_1.png"
File "C:\Temp\yarp\graphics\data\floors\floor_rough_lit_0_2.png"
File "C:\Temp\yarp\graphics\data\floors\floor_rough_lit_1_0.png"
File "C:\Temp\yarp\graphics\data\floors\floor_rough_lit_1_1.png"
File "C:\Temp\yarp\graphics\data\floors\floor_rough_lit_1_2.png"
File "C:\Temp\yarp\graphics\data\floors\floor_rough_lit_2_0.png"
File "C:\Temp\yarp\graphics\data\floors\floor_rough_lit_2_1.png"
File "C:\Temp\yarp\graphics\data\floors\floor_rough_lit_2_2.png"

File "C:\Temp\yarp\graphics\data\floors\floor_water_0_0.png"
File "C:\Temp\yarp\graphics\data\floors\floor_water_0_1.png"
File "C:\Temp\yarp\graphics\data\floors\floor_water_0_2.png"
File "C:\Temp\yarp\graphics\data\floors\floor_water_1_0.png"
File "C:\Temp\yarp\graphics\data\floors\floor_water_1_1.png"
File "C:\Temp\yarp\graphics\data\floors\floor_water_1_2.png"
File "C:\Temp\yarp\graphics\data\floors\floor_water_2_0.png"
File "C:\Temp\yarp\graphics\data\floors\floor_water_2_1.png"
File "C:\Temp\yarp\graphics\data\floors\floor_water_2_2.png"

; set output path to the installation directory
SetOutPath $INSTDIR\data\graphics\fonts

File "C:\Temp\yarp\graphics\data\fonts\font_main.ttf"

; set output path to the installation directory
SetOutPath $INSTDIR\data\graphics\images

File "C:\Temp\yarp\graphics\data\images\image_icon.png"
File "C:\Temp\yarp\graphics\data\images\image_main_logo.png"
File "C:\Temp\yarp\graphics\data\images\interface_elements.png"
File "C:\Temp\yarp\graphics\data\images\interface_minimap_bg.png"

; set output path to the installation directory
SetOutPath $INSTDIR\data\graphics\walls

File "C:\Temp\yarp\graphics\data\walls\wall_brick_banner_f_e.png"
File "C:\Temp\yarp\graphics\data\walls\wall_brick_banner_f_n.png"
File "C:\Temp\yarp\graphics\data\walls\wall_brick_banner_f_s.png"
File "C:\Temp\yarp\graphics\data\walls\wall_brick_banner_f_w.png"
File "C:\Temp\yarp\graphics\data\walls\wall_brick_banner_h_e.png"
File "C:\Temp\yarp\graphics\data\walls\wall_brick_banner_h_n.png"
File "C:\Temp\yarp\graphics\data\walls\wall_brick_banner_h_s.png"
File "C:\Temp\yarp\graphics\data\walls\wall_brick_banner_h_w.png"

File "C:\Temp\yarp\graphics\data\walls\wall_brick_f_e.png"
File "C:\Temp\yarp\graphics\data\walls\wall_brick_f_n.png"
File "C:\Temp\yarp\graphics\data\walls\wall_brick_f_s.png"
File "C:\Temp\yarp\graphics\data\walls\wall_brick_f_w.png"
File "C:\Temp\yarp\graphics\data\walls\wall_brick_h_e.png"
File "C:\Temp\yarp\graphics\data\walls\wall_brick_h_n.png"
File "C:\Temp\yarp\graphics\data\walls\wall_brick_h_s.png"
File "C:\Temp\yarp\graphics\data\walls\wall_brick_h_w.png"

File "C:\Temp\yarp\graphics\data\walls\wall_brick_painting_f_e.png"
File "C:\Temp\yarp\graphics\data\walls\wall_brick_painting_f_n.png"
File "C:\Temp\yarp\graphics\data\walls\wall_brick_painting_f_s.png"
File "C:\Temp\yarp\graphics\data\walls\wall_brick_painting_f_w.png"
File "C:\Temp\yarp\graphics\data\walls\wall_brick_painting_h_e.png"
File "C:\Temp\yarp\graphics\data\walls\wall_brick_painting_h_n.png"
File "C:\Temp\yarp\graphics\data\walls\wall_brick_painting_h_s.png"
File "C:\Temp\yarp\graphics\data\walls\wall_brick_painting_h_w.png"

File "C:\Temp\yarp\graphics\data\walls\wall_brick_pillar_f_e.png"
File "C:\Temp\yarp\graphics\data\walls\wall_brick_pillar_f_n.png"
File "C:\Temp\yarp\graphics\data\walls\wall_brick_pillar_f_s.png"
File "C:\Temp\yarp\graphics\data\walls\wall_brick_pillar_f_w.png"
File "C:\Temp\yarp\graphics\data\walls\wall_brick_pillar_h_e.png"
File "C:\Temp\yarp\graphics\data\walls\wall_brick_pillar_h_n.png"
File "C:\Temp\yarp\graphics\data\walls\wall_brick_pillar_h_s.png"
File "C:\Temp\yarp\graphics\data\walls\wall_brick_pillar_h_w.png"

File "C:\Temp\yarp\graphics\data\walls\wall_brick_pocket_f_e.png"
File "C:\Temp\yarp\graphics\data\walls\wall_brick_pocket_f_n.png"
File "C:\Temp\yarp\graphics\data\walls\wall_brick_pocket_f_s.png"
File "C:\Temp\yarp\graphics\data\walls\wall_brick_pocket_f_w.png"
File "C:\Temp\yarp\graphics\data\walls\wall_brick_pocket_h_e.png"
File "C:\Temp\yarp\graphics\data\walls\wall_brick_pocket_h_n.png"
File "C:\Temp\yarp\graphics\data\walls\wall_brick_pocket_h_s.png"
File "C:\Temp\yarp\graphics\data\walls\wall_brick_pocket_h_w.png"

File "C:\Temp\yarp\graphics\data\walls\wall_dark_f_e.png"
File "C:\Temp\yarp\graphics\data\walls\wall_dark_f_n.png"
File "C:\Temp\yarp\graphics\data\walls\wall_dark_f_s.png"
File "C:\Temp\yarp\graphics\data\walls\wall_dark_f_w.png"
File "C:\Temp\yarp\graphics\data\walls\wall_dark_h_e.png"
File "C:\Temp\yarp\graphics\data\walls\wall_dark_h_n.png"
File "C:\Temp\yarp\graphics\data\walls\wall_dark_h_s.png"
File "C:\Temp\yarp\graphics\data\walls\wall_dark_h_w.png"

File "C:\Temp\yarp\graphics\data\walls\wall_light_f_e.png"
File "C:\Temp\yarp\graphics\data\walls\wall_light_f_n.png"
File "C:\Temp\yarp\graphics\data\walls\wall_light_f_s.png"
File "C:\Temp\yarp\graphics\data\walls\wall_light_f_w.png"
File "C:\Temp\yarp\graphics\data\walls\wall_light_h_e.png"
File "C:\Temp\yarp\graphics\data\walls\wall_light_h_n.png"
File "C:\Temp\yarp\graphics\data\walls\wall_light_h_s.png"
File "C:\Temp\yarp\graphics\data\walls\wall_light_h_w.png"

File "C:\Temp\yarp\graphics\data\walls\wall_marble_f_e.png"
File "C:\Temp\yarp\graphics\data\walls\wall_marble_f_n.png"
File "C:\Temp\yarp\graphics\data\walls\wall_marble_f_s.png"
File "C:\Temp\yarp\graphics\data\walls\wall_marble_f_w.png"
File "C:\Temp\yarp\graphics\data\walls\wall_marble_h_e.png"
File "C:\Temp\yarp\graphics\data\walls\wall_marble_h_n.png"
File "C:\Temp\yarp\graphics\data\walls\wall_marble_h_s.png"
File "C:\Temp\yarp\graphics\data\walls\wall_marble_h_w.png"

File "C:\Temp\yarp\graphics\data\walls\wall_rough_f_e.png"
File "C:\Temp\yarp\graphics\data\walls\wall_rough_f_n.png"
File "C:\Temp\yarp\graphics\data\walls\wall_rough_f_s.png"
File "C:\Temp\yarp\graphics\data\walls\wall_rough_f_w.png"
File "C:\Temp\yarp\graphics\data\walls\wall_rough_h_e.png"
File "C:\Temp\yarp\graphics\data\walls\wall_rough_h_n.png"
File "C:\Temp\yarp\graphics\data\walls\wall_rough_h_s.png"
File "C:\Temp\yarp\graphics\data\walls\wall_rough_h_w.png"

File "C:\Temp\yarp\graphics\data\walls\wall_stucco_f_e.png"
File "C:\Temp\yarp\graphics\data\walls\wall_stucco_f_n.png"
File "C:\Temp\yarp\graphics\data\walls\wall_stucco_f_s.png"
File "C:\Temp\yarp\graphics\data\walls\wall_stucco_f_w.png"
File "C:\Temp\yarp\graphics\data\walls\wall_stucco_h_e.png"
File "C:\Temp\yarp\graphics\data\walls\wall_stucco_h_n.png"
File "C:\Temp\yarp\graphics\data\walls\wall_stucco_h_s.png"
File "C:\Temp\yarp\graphics\data\walls\wall_stucco_h_w.png"

File "C:\Temp\yarp\graphics\data\walls\wall_vine_covered_f_e.png"
File "C:\Temp\yarp\graphics\data\walls\wall_vine_covered_f_n.png"
File "C:\Temp\yarp\graphics\data\walls\wall_vine_covered_f_s.png"
File "C:\Temp\yarp\graphics\data\walls\wall_vine_covered_f_w.png"
File "C:\Temp\yarp\graphics\data\walls\wall_vine_covered_h_e.png"
File "C:\Temp\yarp\graphics\data\walls\wall_vine_covered_h_n.png"
File "C:\Temp\yarp\graphics\data\walls\wall_vine_covered_h_s.png"
File "C:\Temp\yarp\graphics\data\walls\wall_vine_covered_h_w.png"

; set output path to the installation directory
SetOutPath $INSTDIR\data\sound\ambient

File "C:\Temp\yarp\sound\data\ambient\music_main_title.ogg"

; set output path to the installation directory
SetOutPath $INSTDIR\data\sound\effect

File "C:\Temp\yarp\sound\data\effect\sound_bell.ogg"
File "C:\Temp\yarp\sound\data\effect\sound_condition_ill.ogg"
File "C:\Temp\yarp\sound\data\effect\sound_condition_weak.ogg"
File "C:\Temp\yarp\sound\data\effect\sound_counting.ogg"
File "C:\Temp\yarp\sound\data\effect\sound_door_close.ogg"
File "C:\Temp\yarp\sound\data\effect\sound_door_open.ogg"
File "C:\Temp\yarp\sound\data\effect\sound_fountain.ogg"
File "C:\Temp\yarp\sound\data\effect\sound_sword_hit.ogg"
File "C:\Temp\yarp\sound\data\effect\sound_sword_miss.ogg"
File "C:\Temp\yarp\sound\data\effect\sound_walk.ogg"
File "C:\Temp\yarp\sound\data\effect\sound_xp_levelup.ogg"

; Write the installation path into the registry
WriteRegStr HKLM SOFTWARE\Yarp "Install_Dir" "$INSTDIR"

; Write the uninstall keys for Windows
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Yarp" "DisplayName" "Yarp"
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Yarp" "UninstallString" '"$INSTDIR\uninstall.exe"'
WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Yarp" "NoModify" 1
WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Yarp" "NoRepair" 1
WriteUninstaller "uninstall.exe"
  
SectionEnd

;--------------------------------

; Optional section (can be disabled by the user)
;Section "Start Menu Shortcuts"

;CreateDirectory "$SMPROGRAMS\Yarp"
;CreateShortCut "$SMPROGRAMS\Yarp\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
;CreateShortCut "$SMPROGRAMS\Yarp\Yarp (MakeNSISW).lnk" "$INSTDIR\Yarp.nsi" "" "$INSTDIR\Yarp.nsi" 0
  
;SectionEnd

;--------------------------------

; Uninstaller

Section "Uninstall"
  
; Remove registry keys
DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Yarp"
DeleteRegKey HKLM SOFTWARE\Yarp

; Remove files and uninstaller
Delete $INSTDIR\Yarp.nsi
Delete $INSTDIR\uninstall.exe

; Remove shortcuts, if any
Delete "$SMPROGRAMS\Yarp\*.*"

; Remove directories used
RMDir "$SMPROGRAMS\Yarp"
RMDir "$INSTDIR"

SectionEnd
