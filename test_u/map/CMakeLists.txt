########### next target ###############
set(map_parser_SRCS map_parser.cpp)

if (CMAKE_COMPILER_IS_GNUCC)
    set_source_files_properties(${map_parser_SRCS} PROPERTIES COMPILE_FLAGS "-g -march=i686 -Wall")
endif()

add_executable(map_parser ${map_parser_SRCS})

target_link_libraries(map_parser pthread ACE xerces-c RPG_Dice RPG_Common RPG_Map)

########### next target ###############
set(map_generator_SRCS map_generator.cpp)

if (CMAKE_COMPILER_IS_GNUCC)
    set_source_files_properties(${map_generator_SRCS} PROPERTIES COMPILE_FLAGS "-g -march=i686 -Wall")
endif()

add_executable(map_generator ${map_generator_SRCS})

target_link_libraries(map_generator pthread ACE xerces-c RPG_Dice RPG_Common RPG_Map)

########### next target ###############
set(path_finder_SRCS path_finder.cpp)

if (CMAKE_COMPILER_IS_GNUCC)
    set_source_files_properties(${path_finder_SRCS} PROPERTIES COMPILE_FLAGS "-g -march=i686 -Wall")
endif()

add_executable(path_finder ${path_finder_SRCS})

add_dependencies(path_finder RPG_Dice RPG_Common RPG_Map)

target_link_libraries(path_finder pthread ACE xerces-c RPG_Dice RPG_Common RPG_Map)

########### install files ###############
install(TARGETS map_parser
        RUNTIME DESTINATION bin)

install(TARGETS map_generator
        RUNTIME DESTINATION bin)

install(TARGETS path_finder
        RUNTIME DESTINATION bin)
