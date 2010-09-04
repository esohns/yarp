METASOURCES = AUTO

ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = sound_parser

sound_parser_SOURCES = sound_parser.cpp
sound_parser_LDFLAGS = -L/usr/local/lib
sound_parser_LDADD = -lpthread -lACE -lSDL -lSDL_mixer -lRPG_Dice -lRPG_Common \
	-lRPG_Sound
