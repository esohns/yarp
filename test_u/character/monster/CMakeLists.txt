########### next target ###############
set(monster_parser_SRCS monster_parser.cpp)

if (CMAKE_COMPILER_IS_GNUCC)
    set_source_files_properties(${monster_parser_SRCS} PROPERTIES COMPILE_FLAGS "-g -march=i686 -Wall")
endif()

set_source_files_properties(${monster_parser_SRCS} PROPERTIES COMPILE_DEFINITIONS DATADIR="/usr/local/share/RPG/config")

add_executable(monster_parser ${monster_parser_SRCS})

target_link_libraries(monster_parser ACE pthread xerces-c RPG_Dice RPG_Chance RPG_Common RPG_Magic RPG_Item RPG_Character RPG_Character_Monster RPG_Combat)

########### install files ###############
install(TARGETS monster_parser RUNTIME DESTINATION bin)

