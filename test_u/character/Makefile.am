METASOURCES = AUTO

bin_PROGRAMS = character_generator character_parser

character_generator_SOURCES = character_generator.cpp
character_generator_LDFLAGS = -L/usr/local/lib
character_generator_LDADD = -lACE -lpthread -lxerces-c -lRPG_Dice -lRPG_Chance \
	-lRPG_Common -lRPG_Magic -lRPG_Item -lRPG_Character -lRPG_Combat -lRPG_Monster

character_parser_SOURCES = character_parser.cpp
character_parser_LDFLAGS = -L/usr/local/lib
character_parser_LDADD = -lACE -lpthread -lxerces-c -lRPG_Dice -lRPG_Chance \
	-lRPG_Common -lRPG_Magic -lRPG_Item -lRPG_Character -lRPG_Combat -lRPG_Monster
