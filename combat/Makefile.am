METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4

noinst_HEADERS = rpg_XMLSchema_XML_types.h

include_HEADERS = rpg_combat_XML_parser.h rpg_combat_XML_types.h \
	rpg_combat_attackform.h rpg_combat_attacksituation.h rpg_combat_check.h \
	rpg_combat_checktypeunion.h rpg_combat_common.h rpg_combat_common_tools.h rpg_combat_damage.h \
	rpg_combat_damagecountermeasure.h rpg_combat_damageduration.h rpg_combat_damageelement.h \
	rpg_combat_damagereductiontype.h rpg_combat_damagetypeunion.h rpg_combat_defensesituation.h \
	rpg_combat_incl.h rpg_combat_otherdamage.h rpg_combat_otherdamagetype.h \
	rpg_combat_rangedattackproperties.h rpg_combat_rangedeffectunion.h rpg_combat_specialattack.h \
	rpg_combat_specialdamagetype.h

lib_LTLIBRARIES = libRPG_Combat.la

libRPG_Combat_la_SOURCES = rpg_XMLSchema_XML_types.h rpg_combat_XML_parser.cpp \
	rpg_combat_XML_parser.h rpg_combat_XML_types.cpp rpg_combat_XML_types.h rpg_combat_attackform.h \
	rpg_combat_attacksituation.h rpg_combat_check.h rpg_combat_checktypeunion.h rpg_combat_common.h \
	rpg_combat_common_tools.cpp rpg_combat_common_tools.h rpg_combat_damage.h \
	rpg_combat_damagecountermeasure.h rpg_combat_damageduration.h rpg_combat_damagereductiontype.h \
	rpg_combat_defensesituation.h rpg_combat_incl.h rpg_combat_otherdamage.h rpg_combat_otherdamagetype.h \
	rpg_combat_rangedattackproperties.h rpg_combat_rangedeffectunion.h
libRPG_Combat_la_LDFLAGS = -L/usr/local/lib

bindata_DATA = rpg_combat.xsd
bindatadir = $(bindir)
