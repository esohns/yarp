include (${CMAKE_CURRENT_SOURCE_DIR}/../../Common/scripts/Threads.cmake)

include (${CMAKE_CURRENT_SOURCE_DIR}/../../Common/scripts/ACE.cmake)

if (UNIX)
 include (FindPkgConfig)
 pkg_check_modules (PKG_CONFIG_MODULE_SDL REQUIRED sdl)
 pkg_check_modules (PKG_CONFIG_MODULE_LIBPNG REQUIRED libpng)
 pkg_check_modules (PKG_CONFIG_MODULE_LIBGLADE REQUIRED libglade-2.0)
 pkg_check_modules (PKG_CONFIG_MODULE_LIBGNOMEUI REQUIRED libgnomeui-2.0>=1.110.0)
 pkg_check_modules (PKG_CONFIG_MODULE_GMODULE REQUIRED gmodule-export-2.0)
elseif (WIN32)
endif ()

########### next target ###############
set (LIBRARY RPG_Client)
set (${LIBRARY}_SRCS
     rpg_client_callbacks.cpp
     rpg_client_callbacks.h
     rpg_client_common.h
     rpg_client_common_tools.h
     rpg_client_common_tools.cpp
     rpg_client_defines.h
     rpg_client_engine.h
     rpg_client_engine.cpp
     rpg_client_entity_manager.h
     rpg_client_entity_manager.cpp
   #  rpg_client_exports.h
     rpg_client_iwindow.h
     rpg_client_iwindow_level.h
     rpg_client_ui_tools.h
     rpg_client_ui_tools.cpp
     rpg_client_window_level.cpp
     rpg_client_window_level.h
     rpg_client_window_main.cpp
     rpg_client_window_main.h
     rpg_client_window_minimap.h
     rpg_client_window_minimap.cpp)
#     rpg_client_window_message.h
#     rpg_client_window_message.cpp

#set_source_files_properties(${RPG_Client_LIB_SRCS} PROPERTIES COMPILE_DEFINITIONS "RPG_CLIENT_BUILD_DLL;RPG_ENGINE_HAS_DLL=1;RPG_GRAPHICS_HAS_DLL=1;RPG_SOUND_HAS_DLL=1")

add_library (${LIBRARY} STATIC ${${LIBRARY}_SRCS})

if (UNIX)
 target_include_directories (${LIBRARY} PUBLIC
                             $<BUILD_INTERFACE:${PKG_CONFIG_MODULE_SDL_INCLUDE_DIRS}>)
 target_include_directories (${LIBRARY} PUBLIC
                             $<BUILD_INTERFACE:${PKG_CONFIG_MODULE_LIBGNOMEUI_INCLUDE_DIRS}>)
 target_include_directories (${LIBRARY} PUBLIC
                             $<BUILD_INTERFACE:${PKG_CONFIG_MODULE_LIBGLADE_INCLUDE_DIRS}>)
elseif (WIN32)
 target_include_directories (${LIBRARY} PUBLIC
                             $<BUILD_INTERFACE:$ENV{LIB_ROOT}/SDL/include>)
 target_include_directories (${LIBRARY} PUBLIC
                             $<BUILD_INTERFACE:$ENV{LIB_ROOT}/gtk2/include>)
endif ()

if (UNIX)
 target_include_directories (${LIBRARY} PUBLIC
#                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../modules/ACE>)
#                             $<BUILD_INTERFACE:$ENV{LIB_ROOT}/ACE_TAO/ACE>)
                             $<BUILD_INTERFACE:/usr/local/src/ACE_wrappers>)
elseif (WIN32)
 if (DEFINED ENV{ACE_ROOT})
  target_include_directories (${LIBRARY} PUBLIC
                              $<BUILD_INTERFACE:$ENV{ACE_ROOT}>)
 else ()
  target_include_directories (${LIBRARY} PUBLIC
                              $<BUILD_INTERFACE:$ENV{LIB_ROOT}/ACE_TAO/ACE>)
 endif (DEFINED ENV{ACE_ROOT})
endif ()

target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../chance>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../chance/dice>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../common>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../character>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../magic>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../item>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../combat>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../character/player>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../character/monster>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../map>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../engine>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../graphics>)

add_dependencies (${LIBRARY} RPG_Chance RPG_Dice RPG_Common
                             RPG_Character RPG_Magic RPG_Item
                             RPG_Combat RPG_Player RPG_Monster
                             RPG_Map RPG_Engine RPG_Graphics)

set_target_properties (${LIBRARY} PROPERTIES VERSION 1.0.0 SOVERSION 1)

########### install files ###############
install (FILES
         rpg_client.glade
         rpg_client.ini
         DESTINATION share/RPG/config)
install (FILES
         rpg_client_callbacks.h
         rpg_client_common.h
         rpg_client_common_tools.h
         rpg_client_defines.h
         rpg_client_engine.h
         rpg_client_entity_manager.h
       #  rpg_client_exports.h
         rpg_client_iwindow.h
         rpg_client_iwindow_level.h
         rpg_client_ui_tools.h
         rpg_client_window_level.h
         rpg_client_window_main.h
         rpg_client_window_minimap.h
      # rpg_client_window_message.h
         DESTINATION include)
install (TARGETS RPG_Client LIBRARY
         DESTINATION lib)
