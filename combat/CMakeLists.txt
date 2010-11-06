#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

########### next target ###############

set(RPG_Combat_LIB_SRCS
    rpg_XMLSchema_XML_types.h
    rpg_combat_XML_parser.cpp
    rpg_combat_XML_parser.h
    rpg_combat_XML_types.cpp
    rpg_combat_XML_types.h
    rpg_combat_attackform.h
    rpg_combat_attacksituation.h
    rpg_combat_check.h
    rpg_combat_checktypeunion.h
    rpg_combat_common.h
    rpg_combat_common_tools.cpp
    rpg_combat_common_tools.h
    rpg_combat_damage.h
    rpg_combat_damagecountermeasure.h
    rpg_combat_damageduration.h
    rpg_combat_damagereductiontype.h
    rpg_combat_defensesituation.h
    rpg_combat_incl.h
    rpg_combat_otherdamage.h
    rpg_combat_otherdamagetype.h
    rpg_combat_rangedattackproperties.h
    rpg_combat_rangedeffectunion.h)

add_library(RPG_Combat SHARED ${RPG_Combat_LIB_SRCS})

set_target_properties(RPG_Combat PROPERTIES VERSION 1.0.0 SOVERSION 1)
install(TARGETS RPG_Combat LIBRARY DESTINATION lib)

########### install files ###############

install(FILES  rpg_combat.xsd DESTINATION share/RPG/config)

install(FILES  rpg_combat_XML_parser.h rpg_combat_XML_types.h  	rpg_combat_attackform.h rpg_combat_attacksituation.h rpg_combat_check.h  	rpg_combat_checktypeunion.h rpg_combat_common.h rpg_combat_common_tools.h rpg_combat_damage.h  	rpg_combat_damagecountermeasure.h rpg_combat_damageduration.h rpg_combat_damageelement.h  	rpg_combat_damagereductiontype.h rpg_combat_damagetypeunion.h rpg_combat_defensesituation.h  	rpg_combat_incl.h rpg_combat_otherdamage.h rpg_combat_otherdamagetype.h  	rpg_combat_rangedattackproperties.h rpg_combat_rangedeffectunion.h rpg_combat_specialattack.h  	rpg_combat_specialdamagetype.h DESTINATION include)

#original Makefile.am contents follow:

#METASOURCES = AUTO
#
#ACLOCAL_AMFLAGS = -I m4
#
#noinst_HEADERS = rpg_XMLSchema_XML_types.h
#
#include_HEADERS = rpg_combat_XML_parser.h rpg_combat_XML_types.h \
#	rpg_combat_attackform.h rpg_combat_attacksituation.h rpg_combat_check.h \
#	rpg_combat_checktypeunion.h rpg_combat_common.h rpg_combat_common_tools.h rpg_combat_damage.h \
#	rpg_combat_damagecountermeasure.h rpg_combat_damageduration.h rpg_combat_damageelement.h \
#	rpg_combat_damagereductiontype.h rpg_combat_damagetypeunion.h rpg_combat_defensesituation.h \
#	rpg_combat_incl.h rpg_combat_otherdamage.h rpg_combat_otherdamagetype.h \
#	rpg_combat_rangedattackproperties.h rpg_combat_rangedeffectunion.h rpg_combat_specialattack.h \
#	rpg_combat_specialdamagetype.h
#
#lib_LTLIBRARIES = libRPG_Combat.la
#
#libRPG_Combat_la_SOURCES = rpg_XMLSchema_XML_types.h rpg_combat_XML_parser.cpp \
#	rpg_combat_XML_parser.h rpg_combat_XML_types.cpp rpg_combat_XML_types.h rpg_combat_attackform.h \
#	rpg_combat_attacksituation.h rpg_combat_check.h rpg_combat_checktypeunion.h rpg_combat_common.h \
#	rpg_combat_common_tools.cpp rpg_combat_common_tools.h rpg_combat_damage.h \
#	rpg_combat_damagecountermeasure.h rpg_combat_damageduration.h rpg_combat_damagereductiontype.h \
#	rpg_combat_defensesituation.h rpg_combat_incl.h rpg_combat_otherdamage.h rpg_combat_otherdamagetype.h \
#	rpg_combat_rangedattackproperties.h rpg_combat_rangedeffectunion.h
#
#pkgdatadir = $(datadir)/@META_PACKAGE@
#configdir = $(pkgdatadir)/config
#config_DATA = rpg_combat.xsd
#
#EXTRA_DIST = $(config_DATA)
