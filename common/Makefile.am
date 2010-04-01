METASOURCES = AUTO

ACLOCAL_AMFLAGS = `-I m4'

INCLUDES = -I$(top_srcdir)/.

include_HEADERS = rpg_common.h rpg_common_XML_parser.h rpg_common_XML_types.h \
	rpg_common_actiontype.h rpg_common_amount.h rpg_common_areaofeffect.h rpg_common_attribute.h \
	rpg_common_basechecktypeunion.h rpg_common_camp.h rpg_common_checktype.h rpg_common_condition.h \
	rpg_common_countermeasure.h rpg_common_creaturemetatype.h rpg_common_creaturesubtype.h \
	rpg_common_creaturetype.h rpg_common_defines.h rpg_common_duration.h rpg_common_effecttype.h \
	rpg_common_file_tools.h rpg_common_incl.h rpg_common_istatistic.h \
	rpg_common_physicaldamagetype.h rpg_common_savereductiontype.h rpg_common_savingthrow.h \
	rpg_common_savingthrowcheck.h rpg_common_size.h rpg_common_skill.h rpg_common_subclass.h \
	rpg_common_tools.h rpg_common_usage.h

lib_LTLIBRARIES = libRPG_Common.la
libRPG_Common_la_SOURCES = rpg_XMLSchema_XML_types.h rpg_common.h \
	rpg_common_XML_parser.cpp rpg_common_XML_parser.h rpg_common_XML_types.cpp rpg_common_XML_types.h \
	rpg_common_actiontype.h rpg_common_amount.h rpg_common_areaofeffect.h rpg_common_attribute.h \
	rpg_common_basechecktypeunion.h rpg_common_camp.h rpg_common_checktype.h rpg_common_condition.h \
	rpg_common_countermeasure.h rpg_common_creaturemetatype.h rpg_common_creaturesubtype.h \
	rpg_common_creaturetype.h rpg_common_defines.h rpg_common_duration.h rpg_common_effecttype.h \
	rpg_common_file_tools.cpp rpg_common_file_tools.h rpg_common_incl.h rpg_common_istatistic.h \
	rpg_common_physicaldamagetype.h rpg_common_savereductiontype.h rpg_common_savingthrow.h \
	rpg_common_savingthrowcheck.h rpg_common_size.h rpg_common_skill.h rpg_common_subclass.h \
	rpg_common_tools.cpp rpg_common_tools.h rpg_common_usage.h
libRPG_Common_la_LDFLAGS = -L/usr/local/lib

bindata_DATA = rpg_common.xsd
bindatadir = $(bindir)
