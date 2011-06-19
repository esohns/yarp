########### next target ###############
set(magic_parser_SRCS magic_parser.cpp)

if (CMAKE_COMPILER_IS_GNUCC)
    set_source_files_properties(${magic_parser_SRCS} PROPERTIES COMPILE_FLAGS "-g -march=i686 -Wall")
endif()

set_source_files_properties(${magic_parser_SRCS} PROPERTIES COMPILE_DEFINITIONS DATADIR="/usr/local/share/RPG/config")

add_executable(magic_parser ${magic_parser_SRCS})

target_link_libraries(magic_parser pthread ACE xerces-c RPG_Dice RPG_Common RPG_Item RPG_Character RPG_Magic)

install(TARGETS magic_parser RUNTIME DESTINATION bin)

########### install files ###############

#original Makefile.am contents follow:

#METASOURCES = AUTO
#
#ACLOCAL_AMFLAGS = -I m4
#
#bin_PROGRAMS = magic_parser
#
#magic_parser_SOURCES = magic_parser.cpp
#magic_parser_CPPFLAGS = -DDATADIR="\"$(datadir)/@META_PACKAGE@/config\""
#magic_parser_LDADD = -lpthread -lACE -lxerces-c -lRPG_Dice -lRPG_Common \
#	-lRPG_Magic
