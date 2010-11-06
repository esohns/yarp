
########### next target ###############
set(chance_dice_SRCS chance_dice.cpp)

add_executable(chance_dice ${chance_dice_SRCS})

target_link_libraries(chance_dice pthread ACE xerces-c RPG_Dice)

install(TARGETS chance_dice RUNTIME DESTINATION bin)

########### next target ###############
set(chance_rangeToRoll_SRCS chance_rangeToRoll.cpp)

add_executable(chance_rangeToRoll ${chance_rangeToRoll_SRCS})

target_link_libraries(chance_rangeToRoll pthread ACE xerces-c RPG_Dice)

install(TARGETS chance_rangeToRoll RUNTIME DESTINATION bin)

########### install files ###############

#original Makefile.am contents follow:

#METASOURCES = AUTO
#
#ACLOCAL_AMFLAGS = -I m4
#
#bin_PROGRAMS = chance_dice chance_rangeToRoll
#
#chance_dice_SOURCES = chance_dice.cpp
#chance_dice_CPPFLAGS = $(pkg_libs_CFLAGS)
#chance_dice_LDFLAGS = $(pkg_libs_LIBS)
#chance_dice_LDADD = -lpthread -lxerces-c -lRPG_Dice
#
#chance_rangeToRoll_SOURCES = chance_rangeToRoll.cpp
#chance_rangeToRoll_LDADD = -lACE -lpthread -lxerces-c -lRPG_Dice
