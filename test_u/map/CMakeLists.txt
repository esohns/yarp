cmake_minimum_required(VERSION 2.8)

if (UNIX)
 include(FindPkgConfig)
 pkg_check_modules(PKG_CONFIG_MODULE_SDL REQUIRED sdl)
 pkg_check_modules(PKG_CONFIG_MODULE_SDL_TTF REQUIRED SDL_ttf)
 pkg_check_modules(PKG_CONFIG_MODULE_LIBPNG REQUIRED libpng)
elseif (WIN32)
endif ()

if (UNIX)
 include_directories(${PKG_CONFIG_MODULE_SDL_INCLUDE_DIRS}
                     ${PKG_CONFIG_MODULE_SDL_TTF_INCLUDE_DIRS}
                     ${PKG_CONFIG_MODULE_LIBPNG_INCLUDE_DIRS})
elseif (WIN32)
 include_directories($ENV{LIB_ROOT}/SDL/include
                     $ENV{LIB_ROOT}/SDL_ttf/include
                     $ENV{LIB_ROOT}/gtk2/include)
endif ()

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../chance/dice")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../common")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../map")

#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

########### next target ###############
set(map_parser_SRCS map_parser.cpp)

if (CMAKE_COMPILER_IS_GNUCC)
    set_source_files_properties(${map_parser_SRCS} PROPERTIES COMPILE_FLAGS "-g -march=i686 -Wall")
endif()

add_executable(map_parser ${map_parser_SRCS})

add_dependencies(map_parser RPG_Dice RPG_Common RPG_Map)

target_link_libraries(map_parser pthread ACE xerces-c RPG_Dice RPG_Common RPG_Map)

########### next target ###############
set(map_generator_SRCS map_generator.cpp)

if (CMAKE_COMPILER_IS_GNUCC)
    set_source_files_properties(${map_generator_SRCS} PROPERTIES COMPILE_FLAGS "-g -march=i686 -Wall")
endif()

add_executable(map_generator ${map_generator_SRCS})

add_dependencies(map_generator RPG_Dice RPG_Common RPG_Map RPG_Engine)

target_link_libraries(map_generator
                      ${PKG_CONFIG_MODULE_SDL_LIBRARIES}
                      ${PKG_CONFIG_MODULE_SDL_TTF_LIBRARIES}
                      ${PKG_CONFIG_MODULE_LIBPNG_LIBRARIES})
target_link_libraries(map_generator pthread ACE xerces-c
                                    RPG_Chance RPG_Dice RPG_Common RPG_Map RPG_Character RPG_Magic RPG_Item
                                    RPG_Monster RPG_Player RPG_Combat RPG_Graphics RPG_Engine)

########### next target ###############
set(path_finder_SRCS path_finder.cpp)

if (CMAKE_COMPILER_IS_GNUCC)
    set_source_files_properties(${path_finder_SRCS} PROPERTIES COMPILE_FLAGS "-g -march=i686 -Wall")
endif()

add_executable(path_finder ${path_finder_SRCS})

add_dependencies(path_finder RPG_Dice RPG_Common RPG_Map)

target_link_libraries(path_finder pthread ACE xerces-c RPG_Dice RPG_Common RPG_Map)

########### install files ###############
install(TARGETS map_parser
        RUNTIME DESTINATION bin)

install(TARGETS map_generator
        RUNTIME DESTINATION bin)

install(TARGETS path_finder
        RUNTIME DESTINATION bin)
