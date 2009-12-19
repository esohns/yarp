INCLUDES = 
METASOURCES = AUTO
bin_PROGRAMS = character_generator monster_parser
character_generator_SOURCES = character_generator.cpp
monster_parser_SOURCES = monster_parser.cpp
character_generator_LDFLAGS = -L/usr/local/lib
character_generator_LDADD = -lACE -lpthread -lxerces-c -lRPG_Dice -lRPG_Chance \
	-lRPG_Item -lRPG_Character
monster_parser_LDADD = -lACE -lpthread -lxerces-c -lRPG_Dice -lRPG_Chance \
	-lRPG_Item -lRPG_Character -lRPG_Monster -lRPG_Combat
monster_parser_LDFLAGS = -L/usr/local/lib
