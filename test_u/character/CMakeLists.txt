add_subdirectory(monster)

include(FindPkgConfig)
pkg_check_modules(PKG_CONFIG_MODULE_SDL REQUIRED sdl)
pkg_check_modules(PKG_CONFIG_MODULE_SDL_TTF REQUIRED SDL_ttf)
pkg_check_modules(PKG_CONFIG_MODULE_LIBPNG REQUIRED libpng)
pkg_check_modules(PKG_CONFIG_MODULE_LIBGLADE REQUIRED libglade-2.0)
pkg_check_modules(PKG_CONFIG_MODULE_LIBGNOMEUI REQUIRED libgnomeui-2.0>=1.110.0)

include_directories(${PKG_CONFIG_MODULE_SDL_INCLUDE_DIRS} ${PKG_CONFIG_MODULE_SDL_TTF_INCLUDE_DIRS} ${PKG_CONFIG_MODULE_LIBPNG_INCLUDE_DIRS})

########### next target ###############
set(character_generator_SRCS character_generator.cpp)

if (CMAKE_COMPILER_IS_GNUCC)
    set_source_files_properties(${character_generator_SRCS} PROPERTIES COMPILE_FLAGS "-g -march=i686 -Wall")
endif()

set_source_files_properties(${character_generator_SRCS} PROPERTIES COMPILE_DEFINITIONS DATADIR="/usr/local/share/RPG/config")

add_executable(character_generator ${character_generator_SRCS})

target_link_libraries(character_generator ${PKG_CONFIG_MODULE_SDL_LIBRARIES} ${PKG_CONFIG_MODULE_SDL_TTF_LIBRARIES} ${PKG_CONFIG_MODULE_LIBPNG_LIBRARIES})

target_link_libraries(character_generator ACE pthread xerces-c RPG_Dice RPG_Chance RPG_Common RPG_Magic RPG_Item RPG_Character RPG_Character_Monster RPG_Combat RPG_Map RPG_Graphics RPG_Engine)

########### install files ###############
install(TARGETS character_generator RUNTIME DESTINATION bin)

include_directories(${PKG_CONFIG_MODULE_LIBGLADE_INCLUDE_DIRS} ${PKG_CONFIG_MODULE_LIBGNOMEUI_INCLUDE_DIRS})

########### next target ###############
set(character_generator_gui_SRCS
    character_generator_gui.cpp
    character_generator_gui_callbacks.cpp
    character_generator_gui_callbacks.h
    character_generator_gui_common.h)

if (CMAKE_COMPILER_IS_GNUCC)
    set_source_files_properties(${character_generator_gui_SRCS} PROPERTIES COMPILE_FLAGS "-g -march=i686 -Wall")
endif()

set_source_files_properties(${character_generator_gui_SRCS} PROPERTIES COMPILE_DEFINITIONS DATADIR="/usr/local/share/RPG")

add_executable(character_generator_gui ${character_generator_gui_SRCS})

target_link_libraries(character_generator_gui ${PKG_CONFIG_MODULE_SDL_LIBRARIES} ${PKG_CONFIG_MODULE_SDL_TTF_LIBRARIES} ${PKG_CONFIG_MODULE_LIBPNG_LIBRARIES} ${PKG_CONFIG_MODULE_LIBGLADE_LIBRARIES} ${PKG_CONFIG_MODULE_LIBGNOMEUI_LIBRARIES})

target_link_libraries(character_generator_gui ACE pthread xerces-c RPG_Dice RPG_Chance RPG_Common RPG_Magic RPG_Item RPG_Character RPG_Character_Monster RPG_Combat RPG_Map RPG_Graphics RPG_Engine RPG_Client_GUI)

########### install files ###############
install(TARGETS character_generator_gui RUNTIME DESTINATION bin)

########### next target ###############
set(character_parser_SRCS character_parser.cpp)

if (CMAKE_COMPILER_IS_GNUCC)
    set_source_files_properties(${character_parser_SRCS} PROPERTIES COMPILE_FLAGS "-g -march=i686 -Wall")
endif()

set_source_files_properties(${character_parser_SRCS} PROPERTIES COMPILE_DEFINITIONS DATADIR="/usr/local/share/RPG/config")

# add_definitions(-DDATADIR="/usr/local/share/RPG/config")

add_executable(character_parser ${character_parser_SRCS})

target_link_libraries(character_parser ${PKG_CONFIG_MODULE_SDL_LIBRARIES} ${PKG_CONFIG_MODULE_SDL_TTF_LIBRARIES} ${PKG_CONFIG_MODULE_LIBPNG_LIBRARIES})

target_link_libraries(character_parser ACE pthread xerces-c RPG_Dice RPG_Chance RPG_Common RPG_Magic RPG_Item RPG_Character RPG_Character_Monster RPG_Combat RPG_Map RPG_Graphics RPG_Engine)

########### install files ###############
install(TARGETS character_parser RUNTIME DESTINATION bin)
install(FILES  character_generator_gui.glade DESTINATION share/RPG/config)
