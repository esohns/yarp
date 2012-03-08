cmake_minimum_required(VERSION 2.8)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/dice
                    ${CMAKE_CURRENT_SOURCE_DIR}/../common)

#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

add_subdirectory(dice)

########### next target ###############
set(RPG_Chance_LIB_SRCS
    rpg_chance_common_tools.cpp
    rpg_chance_common_tools.h)

set_source_files_properties(${RPG_Chance_LIB_SRCS} PROPERTIES COMPILE_DEFINITIONS RPG_CHANCE_BUILD_DLL)
if (CMAKE_COMPILER_IS_GNUCC)
    set_source_files_properties(${RPG_Chance_LIB_SRCS} PROPERTIES COMPILE_FLAGS "-g -march=i686 -Wall")
endif()

add_library(RPG_Chance SHARED ${RPG_Chance_LIB_SRCS})

add_dependencies(RPG_Chance RPG_Dice)

set_target_properties(RPG_Chance PROPERTIES VERSION 1.0.0 SOVERSION 1)

########### install files ###############
install(FILES
        rpg_chance_common_tools.h
	rpg_chance_exports.h
        DESTINATION include)
install(TARGETS RPG_Chance LIBRARY DESTINATION lib)
