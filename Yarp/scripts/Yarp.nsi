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
File "E:\ACE_wrappers\lib\ACE.dll"

File "E:\Gtk\bin\freetype6.dll"
File "E:\Gtk\bin\intl.dll"
File "E:\Gtk\bin\libatk-1.0-0.dll"
File "E:\Gtk\bin\libcairo-2.dll"
File "E:\Gtk\bin\libexpat-1.dll"
File "E:\Gtk\bin\libfontconfig-1.dll"
File "E:\Gtk\bin\libgdk_pixbuf-2.0-0.dll"
File "E:\Gtk\bin\libgdk-win32-2.0-0.dll"
File "E:\Gtk\bin\libgio-2.0-0.dll"
File "E:\Gtk\bin\libglib-2.0-0.dll"
File "E:\Gtk\bin\libgmodule-2.0-0.dll"
File "E:\Gtk\bin\libgobject-2.0-0.dll"
File "E:\Gtk\bin\libgthread-2.0-0.dll"
File "E:\Gtk\bin\libgtk-win32-2.0-0.dll"
File "E:\Gtk\bin\libpango-1.0-0.dll"
File "E:\Gtk\bin\libpangocairo-1.0-0.dll"
File "E:\Gtk\bin\libpangoft2-1.0-0.dll"
File "E:\Gtk\bin\libpangowin32-1.0-0.dll"
File "E:\Gtk\bin\libpng14-14.dll"
File "E:\Gtk\bin\zlib1.dll"

File "E:\SDL-1.2.14\lib\libfreetype-6.dll"
File "E:\SDL-1.2.14\lib\libogg-0.dll"
File "E:\SDL-1.2.14\lib\libvorbis-0.dll"
File "E:\SDL-1.2.14\lib\libvorbisfile-3.dll"
File "E:\SDL-1.2.14\lib\mikmod.dll"
File "E:\SDL-1.2.14\lib\SDL.dll"
File "E:\SDL-1.2.14\lib\SDL_mixer.dll"
File "E:\SDL-1.2.14\lib\SDL_ttf.dll"

File "E:\libglade-dev_2.6.4-1_win32\bin\libglade-2.0-0.dll"

File "E:\lpng157\projects\vstudio\Release\libpng15.dll"

File "D:\Archive\Develop\libiconv-2.dll"
File "D:\Archive\Develop\libxml2-2.dll"
    
File "E:\Yarp\Yarp\Release\dice.dll"
File "E:\Yarp\Yarp\Release\chance.dll"
File "E:\Yarp\Yarp\Release\common.dll"
File "E:\Yarp\Yarp\Release\magic.dll"
File "E:\Yarp\Yarp\Release\character.dll"
File "E:\Yarp\Yarp\Release\item.dll"
File "E:\Yarp\Yarp\Release\player.dll"
File "E:\Yarp\Yarp\Release\monster.dll"
File "E:\Yarp\Yarp\Release\combat.dll"
File "E:\Yarp\Yarp\Release\map.dll"
File "E:\Yarp\Yarp\Release\engine.dll"
File "E:\Yarp\Yarp\Release\sound.dll"
File "E:\Yarp\Yarp\Release\graphics.dll"
File "E:\Yarp\Yarp\Release\stream.dll"
File "E:\Yarp\Yarp\Release\net.dll"
File "E:\Yarp\Yarp\Release\protocol.dll"
File "E:\Yarp\Yarp\Release\client.dll"
  
File "E:\Yarp\Yarp\Release\test_u_chance_dice.exe"
File "E:\Yarp\Yarp\Release\test_u_chance_rangeToRoll.exe"
File "E:\Yarp\Yarp\Release\test_u_character_generator.exe"
File "E:\Yarp\Yarp\Release\test_u_character_generator_gui.exe"
File "E:\Yarp\Yarp\Release\test_u_character_parser.exe"
File "E:\Yarp\Yarp\Release\test_u_client_gui.exe"
File "E:\Yarp\Yarp\Release\test_u_graphics_parser.exe"
File "E:\Yarp\Yarp\Release\test_u_graphics_SDL_gui.exe"
File "E:\Yarp\Yarp\Release\test_u_item_parser.exe"
File "E:\Yarp\Yarp\Release\test_u_magic_parser.exe"
File "E:\Yarp\Yarp\Release\test_u_map_generator.exe"
File "E:\Yarp\Yarp\Release\test_u_map_parser.exe"
File "E:\Yarp\Yarp\Release\test_u_map_path_finder.exe"
File "E:\Yarp\Yarp\Release\test_u_map_vision.exe"
File "E:\Yarp\Yarp\Release\test_u_map_vision_gui.exe"
File "E:\Yarp\Yarp\Release\test_u_monster_parser.exe"
File "E:\Yarp\Yarp\Release\test_u_net_client.exe"
File "E:\Yarp\Yarp\Release\test_u_net_protocol_client.exe"
File "E:\Yarp\Yarp\Release\test_u_net_protocol_client_gui.exe"
File "E:\Yarp\Yarp\Release\test_u_net_server.exe"
File "E:\Yarp\Yarp\Release\test_u_sound_parser.exe"

; Config
; set output path to the installation directory
SetOutPath $INSTDIR\config

; Config - XML
File "E:\Yarp\magic\rpg_magic.xml"
File "E:\Yarp\item\rpg_item.xml"
File "E:\Yarp\character\monster\rpg_monster.xml"
File "E:\Yarp\graphics\rpg_graphics.xml"
File "E:\Yarp\sound\rpg_sound.xml"

; Config - XSD schema
File "E:\Yarp\engine\rpg_engine.xsd"
File "E:\Yarp\character\player\rpg_player.xsd"
File "E:\Yarp\map\rpg_map.xsd"
File "E:\Yarp\chance\dice\rpg_dice.xsd"
File "E:\Yarp\common\rpg_common.xsd"
File "E:\Yarp\common\rpg_common_environment.xsd"
File "E:\Yarp\character\rpg_character.xsd"
File "E:\Yarp\magic\rpg_magic.xsd"

; Config - glade
File "E:\Yarp\client\rpg_client.glade"
File "E:\Yarp\test_u\character\player\character_generator_gui.glade"

; Config - ini
File "E:\Yarp\client\rpg_client.ini"

; Data
; set output path to the installation directory
SetOutPath $INSTDIR\data\graphics\creatures

File "E:\Yarp\graphics\data\creatures\goblin.png"
File "E:\Yarp\graphics\data\creatures\human.png"
File "E:\Yarp\graphics\data\creatures\priest.png"

; set output path to the installation directory
SetOutPath $INSTDIR\data\graphics\cursors

File "E:\Yarp\graphics\data\cursors\cursor_door_open.png"
File "E:\Yarp\graphics\data\cursors\cursor_goblet.png"
File "E:\Yarp\graphics\data\cursors\cursor_hourglass.png"
File "E:\Yarp\graphics\data\cursors\cursor_normal.png"
File "E:\Yarp\graphics\data\cursors\cursor_scroll_down.png"
File "E:\Yarp\graphics\data\cursors\cursor_scroll_downleft.png"
File "E:\Yarp\graphics\data\cursors\cursor_scroll_downright.png"
File "E:\Yarp\graphics\data\cursors\cursor_scroll_left.png"
File "E:\Yarp\graphics\data\cursors\cursor_scroll_right.png"
File "E:\Yarp\graphics\data\cursors\cursor_scroll_up.png"
File "E:\Yarp\graphics\data\cursors\cursor_scroll_upleft.png"
File "E:\Yarp\graphics\data\cursors\cursor_scroll_upright.png"
File "E:\Yarp\graphics\data\cursors\cursor_stairs.png"
File "E:\Yarp\graphics\data\cursors\cursor_target_green.png"
File "E:\Yarp\graphics\data\cursors\cursor_target_help.png"
File "E:\Yarp\graphics\data\cursors\cursor_target_invalid.png"
File "E:\Yarp\graphics\data\cursors\cursor_target_red.png"

; set output path to the installation directory
SetOutPath $INSTDIR\data\graphics\doors

File "E:\Yarp\graphics\data\doors\door_wood_broken.png"
File "E:\Yarp\graphics\data\doors\door_wood_closed_h.png"
File "E:\Yarp\graphics\data\doors\door_wood_closed_v.png"
File "E:\Yarp\graphics\data\doors\door_wood_open_h.png"
File "E:\Yarp\graphics\data\doors\door_wood_open_v.png"
File "E:\Yarp\graphics\data\doors\drawbridge_wood_closed_h.png"
File "E:\Yarp\graphics\data\doors\drawbridge_wood_closed_v.png"
File "E:\Yarp\graphics\data\doors\drawbridge_wood_open_h.png"
File "E:\Yarp\graphics\data\doors\drawbridge_wood_open_v.png"

; set output path to the installation directory
SetOutPath $INSTDIR\data\graphics\floors

File "E:\Yarp\graphics\data\floors\floor_air_0_0.png"
File "E:\Yarp\graphics\data\floors\floor_air_0_1.png"
File "E:\Yarp\graphics\data\floors\floor_air_0_2.png"
File "E:\Yarp\graphics\data\floors\floor_air_1_0.png"
File "E:\Yarp\graphics\data\floors\floor_air_1_1.png"
File "E:\Yarp\graphics\data\floors\floor_air_1_2.png"
File "E:\Yarp\graphics\data\floors\floor_air_2_0.png"
File "E:\Yarp\graphics\data\floors\floor_air_2_1.png"
File "E:\Yarp\graphics\data\floors\floor_air_2_2.png"

File "E:\Yarp\graphics\data\floors\floor_carpet_0_0.png"
File "E:\Yarp\graphics\data\floors\floor_carpet_0_1.png"
File "E:\Yarp\graphics\data\floors\floor_carpet_1_0.png"
File "E:\Yarp\graphics\data\floors\floor_carpet_1_1.png"
File "E:\Yarp\graphics\data\floors\floor_carpet_2_0.png"
File "E:\Yarp\graphics\data\floors\floor_carpet_2_1.png"

File "E:\Yarp\graphics\data\floors\floor_ceramic_0_0.png"
File "E:\Yarp\graphics\data\floors\floor_ceramic_0_1.png"
File "E:\Yarp\graphics\data\floors\floor_ceramic_0_2.png"
File "E:\Yarp\graphics\data\floors\floor_ceramic_1_0.png"
File "E:\Yarp\graphics\data\floors\floor_ceramic_1_1.png"
File "E:\Yarp\graphics\data\floors\floor_ceramic_1_2.png"
File "E:\Yarp\graphics\data\floors\floor_ceramic_2_0.png"
File "E:\Yarp\graphics\data\floors\floor_ceramic_2_1.png"
File "E:\Yarp\graphics\data\floors\floor_ceramic_2_2.png"

File "E:\Yarp\graphics\data\floors\floor_cobblestone_0_0.png"
File "E:\Yarp\graphics\data\floors\floor_cobblestone_0_1.png"
File "E:\Yarp\graphics\data\floors\floor_cobblestone_0_2.png"
File "E:\Yarp\graphics\data\floors\floor_cobblestone_1_0.png"
File "E:\Yarp\graphics\data\floors\floor_cobblestone_1_1.png"
File "E:\Yarp\graphics\data\floors\floor_cobblestone_1_2.png"
File "E:\Yarp\graphics\data\floors\floor_cobblestone_2_0.png"
File "E:\Yarp\graphics\data\floors\floor_cobblestone_2_1.png"
File "E:\Yarp\graphics\data\floors\floor_cobblestone_2_2.png"

File "E:\Yarp\graphics\data\floors\floor_cobblestone_edge1.png"
File "E:\Yarp\graphics\data\floors\floor_cobblestone_edge2.png"
File "E:\Yarp\graphics\data\floors\floor_cobblestone_edge3.png"
File "E:\Yarp\graphics\data\floors\floor_cobblestone_edge4.png"
File "E:\Yarp\graphics\data\floors\floor_cobblestone_edge5.png"
File "E:\Yarp\graphics\data\floors\floor_cobblestone_edge6.png"
File "E:\Yarp\graphics\data\floors\floor_cobblestone_edge7.png"
File "E:\Yarp\graphics\data\floors\floor_cobblestone_edge8.png"
File "E:\Yarp\graphics\data\floors\floor_cobblestone_edge9.png"
File "E:\Yarp\graphics\data\floors\floor_cobblestone_edge10.png"
File "E:\Yarp\graphics\data\floors\floor_cobblestone_edge11.png"
File "E:\Yarp\graphics\data\floors\floor_cobblestone_edge12.png"

File "E:\Yarp\graphics\data\floors\floor_dark_0_0.png"

File "E:\Yarp\graphics\data\floors\floor_highlight.png"

File "E:\Yarp\graphics\data\floors\floor_ice_0_0.png"
File "E:\Yarp\graphics\data\floors\floor_ice_0_1.png"
File "E:\Yarp\graphics\data\floors\floor_ice_0_2.png"
File "E:\Yarp\graphics\data\floors\floor_ice_1_0.png"
File "E:\Yarp\graphics\data\floors\floor_ice_1_1.png"
File "E:\Yarp\graphics\data\floors\floor_ice_1_2.png"
File "E:\Yarp\graphics\data\floors\floor_ice_2_0.png"
File "E:\Yarp\graphics\data\floors\floor_ice_2_1.png"
File "E:\Yarp\graphics\data\floors\floor_ice_2_2.png"

File "E:\Yarp\graphics\data\floors\floor_invisible.png"

File "E:\Yarp\graphics\data\floors\floor_lava_0_0.png"
File "E:\Yarp\graphics\data\floors\floor_lava_0_1.png"
File "E:\Yarp\graphics\data\floors\floor_lava_0_2.png"
File "E:\Yarp\graphics\data\floors\floor_lava_1_0.png"
File "E:\Yarp\graphics\data\floors\floor_lava_1_1.png"
File "E:\Yarp\graphics\data\floors\floor_lava_1_2.png"
File "E:\Yarp\graphics\data\floors\floor_lava_2_0.png"
File "E:\Yarp\graphics\data\floors\floor_lava_2_1.png"
File "E:\Yarp\graphics\data\floors\floor_lava_2_2.png"

File "E:\Yarp\graphics\data\floors\floor_marble_0_0.png"
File "E:\Yarp\graphics\data\floors\floor_marble_0_1.png"
File "E:\Yarp\graphics\data\floors\floor_marble_0_2.png"
File "E:\Yarp\graphics\data\floors\floor_marble_1_0.png"
File "E:\Yarp\graphics\data\floors\floor_marble_1_1.png"
File "E:\Yarp\graphics\data\floors\floor_marble_1_2.png"
File "E:\Yarp\graphics\data\floors\floor_marble_2_0.png"
File "E:\Yarp\graphics\data\floors\floor_marble_2_1.png"
File "E:\Yarp\graphics\data\floors\floor_marble_2_2.png"

File "E:\Yarp\graphics\data\floors\floor_moss_covered_0_0.png"
File "E:\Yarp\graphics\data\floors\floor_moss_covered_0_1.png"
File "E:\Yarp\graphics\data\floors\floor_moss_covered_0_2.png"
File "E:\Yarp\graphics\data\floors\floor_moss_covered_1_0.png"
File "E:\Yarp\graphics\data\floors\floor_moss_covered_1_1.png"
File "E:\Yarp\graphics\data\floors\floor_moss_covered_1_2.png"
File "E:\Yarp\graphics\data\floors\floor_moss_covered_2_0.png"
File "E:\Yarp\graphics\data\floors\floor_moss_covered_2_1.png"
File "E:\Yarp\graphics\data\floors\floor_moss_covered_2_2.png"

File "E:\Yarp\graphics\data\floors\floor_mural2_0_0.png"
File "E:\Yarp\graphics\data\floors\floor_mural2_0_1.png"
File "E:\Yarp\graphics\data\floors\floor_mural2_1_0.png"
File "E:\Yarp\graphics\data\floors\floor_mural2_1_1.png"
File "E:\Yarp\graphics\data\floors\floor_mural2_2_0.png"
File "E:\Yarp\graphics\data\floors\floor_mural2_2_1.png"

File "E:\Yarp\graphics\data\floors\floor_mural_0_0.png"
File "E:\Yarp\graphics\data\floors\floor_mural_0_1.png"
File "E:\Yarp\graphics\data\floors\floor_mural_1_0.png"
File "E:\Yarp\graphics\data\floors\floor_mural_1_1.png"
File "E:\Yarp\graphics\data\floors\floor_mural_2_0.png"
File "E:\Yarp\graphics\data\floors\floor_mural_2_1.png"

File "E:\Yarp\graphics\data\floors\floor_rough_0_0.png"
File "E:\Yarp\graphics\data\floors\floor_rough_0_1.png"
File "E:\Yarp\graphics\data\floors\floor_rough_0_2.png"
File "E:\Yarp\graphics\data\floors\floor_rough_1_0.png"
File "E:\Yarp\graphics\data\floors\floor_rough_1_1.png"
File "E:\Yarp\graphics\data\floors\floor_rough_1_2.png"
File "E:\Yarp\graphics\data\floors\floor_rough_2_0.png"
File "E:\Yarp\graphics\data\floors\floor_rough_2_1.png"
File "E:\Yarp\graphics\data\floors\floor_rough_2_2.png"

File "E:\Yarp\graphics\data\floors\floor_rough_lit_0_0.png"
File "E:\Yarp\graphics\data\floors\floor_rough_lit_0_1.png"
File "E:\Yarp\graphics\data\floors\floor_rough_lit_0_2.png"
File "E:\Yarp\graphics\data\floors\floor_rough_lit_1_0.png"
File "E:\Yarp\graphics\data\floors\floor_rough_lit_1_1.png"
File "E:\Yarp\graphics\data\floors\floor_rough_lit_1_2.png"
File "E:\Yarp\graphics\data\floors\floor_rough_lit_2_0.png"
File "E:\Yarp\graphics\data\floors\floor_rough_lit_2_1.png"
File "E:\Yarp\graphics\data\floors\floor_rough_lit_2_2.png"

File "E:\Yarp\graphics\data\floors\floor_water_0_0.png"
File "E:\Yarp\graphics\data\floors\floor_water_0_1.png"
File "E:\Yarp\graphics\data\floors\floor_water_0_2.png"
File "E:\Yarp\graphics\data\floors\floor_water_1_0.png"
File "E:\Yarp\graphics\data\floors\floor_water_1_1.png"
File "E:\Yarp\graphics\data\floors\floor_water_1_2.png"
File "E:\Yarp\graphics\data\floors\floor_water_2_0.png"
File "E:\Yarp\graphics\data\floors\floor_water_2_1.png"
File "E:\Yarp\graphics\data\floors\floor_water_2_2.png"

; set output path to the installation directory
SetOutPath $INSTDIR\data\graphics\fonts

File "E:\Yarp\graphics\data\fonts\font_main.ttf"

; set output path to the installation directory
SetOutPath $INSTDIR\data\graphics\images

File "E:\Yarp\graphics\data\images\image_icon.png"
File "E:\Yarp\graphics\data\images\image_main_logo.png"
File "E:\Yarp\graphics\data\images\interface_elements.png"
File "E:\Yarp\graphics\data\images\interface_minimap_bg.png"

; set output path to the installation directory
SetOutPath $INSTDIR\data\graphics\walls

File "E:\Yarp\graphics\data\walls\wall_brick_banner_f_e.png"
File "E:\Yarp\graphics\data\walls\wall_brick_banner_f_n.png"
File "E:\Yarp\graphics\data\walls\wall_brick_banner_f_s.png"
File "E:\Yarp\graphics\data\walls\wall_brick_banner_f_w.png"
File "E:\Yarp\graphics\data\walls\wall_brick_banner_h_e.png"
File "E:\Yarp\graphics\data\walls\wall_brick_banner_h_n.png"
File "E:\Yarp\graphics\data\walls\wall_brick_banner_h_s.png"
File "E:\Yarp\graphics\data\walls\wall_brick_banner_h_w.png"

File "E:\Yarp\graphics\data\walls\wall_brick_f_e.png"
File "E:\Yarp\graphics\data\walls\wall_brick_f_n.png"
File "E:\Yarp\graphics\data\walls\wall_brick_f_s.png"
File "E:\Yarp\graphics\data\walls\wall_brick_f_w.png"
File "E:\Yarp\graphics\data\walls\wall_brick_h_e.png"
File "E:\Yarp\graphics\data\walls\wall_brick_h_n.png"
File "E:\Yarp\graphics\data\walls\wall_brick_h_s.png"
File "E:\Yarp\graphics\data\walls\wall_brick_h_w.png"

File "E:\Yarp\graphics\data\walls\wall_brick_painting_f_e.png"
File "E:\Yarp\graphics\data\walls\wall_brick_painting_f_n.png"
File "E:\Yarp\graphics\data\walls\wall_brick_painting_f_s.png"
File "E:\Yarp\graphics\data\walls\wall_brick_painting_f_w.png"
File "E:\Yarp\graphics\data\walls\wall_brick_painting_h_e.png"
File "E:\Yarp\graphics\data\walls\wall_brick_painting_h_n.png"
File "E:\Yarp\graphics\data\walls\wall_brick_painting_h_s.png"
File "E:\Yarp\graphics\data\walls\wall_brick_painting_h_w.png"

File "E:\Yarp\graphics\data\walls\wall_brick_pillar_f_e.png"
File "E:\Yarp\graphics\data\walls\wall_brick_pillar_f_n.png"
File "E:\Yarp\graphics\data\walls\wall_brick_pillar_f_s.png"
File "E:\Yarp\graphics\data\walls\wall_brick_pillar_f_w.png"
File "E:\Yarp\graphics\data\walls\wall_brick_pillar_h_e.png"
File "E:\Yarp\graphics\data\walls\wall_brick_pillar_h_n.png"
File "E:\Yarp\graphics\data\walls\wall_brick_pillar_h_s.png"
File "E:\Yarp\graphics\data\walls\wall_brick_pillar_h_w.png"

File "E:\Yarp\graphics\data\walls\wall_brick_pocket_f_e.png"
File "E:\Yarp\graphics\data\walls\wall_brick_pocket_f_n.png"
File "E:\Yarp\graphics\data\walls\wall_brick_pocket_f_s.png"
File "E:\Yarp\graphics\data\walls\wall_brick_pocket_f_w.png"
File "E:\Yarp\graphics\data\walls\wall_brick_pocket_h_e.png"
File "E:\Yarp\graphics\data\walls\wall_brick_pocket_h_n.png"
File "E:\Yarp\graphics\data\walls\wall_brick_pocket_h_s.png"
File "E:\Yarp\graphics\data\walls\wall_brick_pocket_h_w.png"

File "E:\Yarp\graphics\data\walls\wall_dark_f_e.png"
File "E:\Yarp\graphics\data\walls\wall_dark_f_n.png"
File "E:\Yarp\graphics\data\walls\wall_dark_f_s.png"
File "E:\Yarp\graphics\data\walls\wall_dark_f_w.png"
File "E:\Yarp\graphics\data\walls\wall_dark_h_e.png"
File "E:\Yarp\graphics\data\walls\wall_dark_h_n.png"
File "E:\Yarp\graphics\data\walls\wall_dark_h_s.png"
File "E:\Yarp\graphics\data\walls\wall_dark_h_w.png"

File "E:\Yarp\graphics\data\walls\wall_light_f_e.png"
File "E:\Yarp\graphics\data\walls\wall_light_f_n.png"
File "E:\Yarp\graphics\data\walls\wall_light_f_s.png"
File "E:\Yarp\graphics\data\walls\wall_light_f_w.png"
File "E:\Yarp\graphics\data\walls\wall_light_h_e.png"
File "E:\Yarp\graphics\data\walls\wall_light_h_n.png"
File "E:\Yarp\graphics\data\walls\wall_light_h_s.png"
File "E:\Yarp\graphics\data\walls\wall_light_h_w.png"

File "E:\Yarp\graphics\data\walls\wall_marble_f_e.png"
File "E:\Yarp\graphics\data\walls\wall_marble_f_n.png"
File "E:\Yarp\graphics\data\walls\wall_marble_f_s.png"
File "E:\Yarp\graphics\data\walls\wall_marble_f_w.png"
File "E:\Yarp\graphics\data\walls\wall_marble_h_e.png"
File "E:\Yarp\graphics\data\walls\wall_marble_h_n.png"
File "E:\Yarp\graphics\data\walls\wall_marble_h_s.png"
File "E:\Yarp\graphics\data\walls\wall_marble_h_w.png"

File "E:\Yarp\graphics\data\walls\wall_rough_f_e.png"
File "E:\Yarp\graphics\data\walls\wall_rough_f_n.png"
File "E:\Yarp\graphics\data\walls\wall_rough_f_s.png"
File "E:\Yarp\graphics\data\walls\wall_rough_f_w.png"
File "E:\Yarp\graphics\data\walls\wall_rough_h_e.png"
File "E:\Yarp\graphics\data\walls\wall_rough_h_n.png"
File "E:\Yarp\graphics\data\walls\wall_rough_h_s.png"
File "E:\Yarp\graphics\data\walls\wall_rough_h_w.png"

File "E:\Yarp\graphics\data\walls\wall_stucco_f_e.png"
File "E:\Yarp\graphics\data\walls\wall_stucco_f_n.png"
File "E:\Yarp\graphics\data\walls\wall_stucco_f_s.png"
File "E:\Yarp\graphics\data\walls\wall_stucco_f_w.png"
File "E:\Yarp\graphics\data\walls\wall_stucco_h_e.png"
File "E:\Yarp\graphics\data\walls\wall_stucco_h_n.png"
File "E:\Yarp\graphics\data\walls\wall_stucco_h_s.png"
File "E:\Yarp\graphics\data\walls\wall_stucco_h_w.png"

File "E:\Yarp\graphics\data\walls\wall_vine_covered_f_e.png"
File "E:\Yarp\graphics\data\walls\wall_vine_covered_f_n.png"
File "E:\Yarp\graphics\data\walls\wall_vine_covered_f_s.png"
File "E:\Yarp\graphics\data\walls\wall_vine_covered_f_w.png"
File "E:\Yarp\graphics\data\walls\wall_vine_covered_h_e.png"
File "E:\Yarp\graphics\data\walls\wall_vine_covered_h_n.png"
File "E:\Yarp\graphics\data\walls\wall_vine_covered_h_s.png"
File "E:\Yarp\graphics\data\walls\wall_vine_covered_h_w.png"

; set output path to the installation directory
SetOutPath $INSTDIR\data\sound\ambient

File "E:\Yarp\sound\data\ambient\music_main_title.ogg"

; set output path to the installation directory
SetOutPath $INSTDIR\data\sound\effect

File "E:\Yarp\sound\data\effect\sound_bell.ogg"
File "E:\Yarp\sound\data\effect\sound_condition_ill.ogg"
File "E:\Yarp\sound\data\effect\sound_condition_weak.ogg"
File "E:\Yarp\sound\data\effect\sound_counting.ogg"
File "E:\Yarp\sound\data\effect\sound_door_close.ogg"
File "E:\Yarp\sound\data\effect\sound_door_open.ogg"
File "E:\Yarp\sound\data\effect\sound_fountain.ogg"
File "E:\Yarp\sound\data\effect\sound_sword_hit.ogg"
File "E:\Yarp\sound\data\effect\sound_sword_miss.ogg"
File "E:\Yarp\sound\data\effect\sound_walk.ogg"
File "E:\Yarp\sound\data\effect\sound_xp_levelup.ogg"

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
CreateShortCut "$SMPROGRAMS\Yarp\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
CreateShortCut "$SMPROGRAMS\Yarp\Yarp (MakeNSISW).lnk" "$INSTDIR\Yarp.nsi" "" "$INSTDIR\Yarp.nsi" 0
  
SectionEnd

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
