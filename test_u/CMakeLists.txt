#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

add_subdirectory(chance)
add_subdirectory(character)
add_subdirectory(combat)
add_subdirectory(graphics)
add_subdirectory(client)
add_subdirectory(item)
add_subdirectory(magic)
add_subdirectory(map)
add_subdirectory(monster)
add_subdirectory(net)
add_subdirectory(sound)

########### install files ###############

#original Makefile.am contents follow:

#METASOURCES = AUTO
#
#ACLOCAL_AMFLAGS = -I m4
#
#SUBDIRS = chance character combat graphics client item magic map monster net sound
