########### next target ###############
set(monster_parser_SRCS monster_parser.cpp)

if (CMAKE_COMPILER_IS_GNUCC)
    set_source_files_properties(${monster_parser_SRCS} PROPERTIES COMPILE_FLAGS "-g -march=i686 -Wall")
endif()

add_executable(monster_parser ${monster_parser_SRCS})

target_link_libraries(monster_parser ACE pthread xerces-c RPG_Dice RPG_Chance RPG_Common RPG_Magic RPG_Item RPG_Character RPG_Combat RPG_Monster)

install(TARGETS monster_parser RUNTIME DESTINATION bin)

########### install files ###############

#original Makefile.am contents follow:

#METASOURCES = AUTO
#
#ACLOCAL_AMFLAGS = -I m4
#
#bin_PROGRAMS = monster_parser
#
#monster_parser_SOURCES = monster_parser.cpp
#monster_parser_CPPFLAGS = -DDATADIR="\"$(datadir)/@META_PACKAGE@/config\""
#monster_parser_LDADD = -lACE -lpthread -lxerces-c -lRPG_Dice -lRPG_Chance \
#	-lRPG_Common -lRPG_Magic -lRPG_Item -lRPG_Character -lRPG_Combat -lRPG_Monster
