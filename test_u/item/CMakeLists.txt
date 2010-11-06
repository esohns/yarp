########### next target ###############
set(item_parser_SRCS item_parser.cpp)

add_executable(item_parser ${item_parser_SRCS})

target_link_libraries(item_parser pthread ACE xerces-c RPG_Dice RPG_Common RPG_Magic RPG_Item)

install(TARGETS item_parser RUNTIME DESTINATION bin)

########### install files ###############

#original Makefile.am contents follow:

#METASOURCES = AUTO
#
#ACLOCAL_AMFLAGS = -I m4
#
#bin_PROGRAMS = item_parser
#
#item_parser_SOURCES = item_parser.cpp
#item_parser_CPPFLAGS = -DDATADIR="\"$(datadir)/@META_PACKAGE@/config\""
#item_parser_LDADD = -lpthread -lACE -lxerces-c -lRPG_Dice -lRPG_Common \
#	-lRPG_Magic -lRPG_Item
