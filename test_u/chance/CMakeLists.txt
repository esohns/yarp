########### next target ###############
set(chance_dice_SRCS chance_dice.cpp)

if (CMAKE_COMPILER_IS_GNUCC)
    set_source_files_properties(${chance_dice_SRCS} PROPERTIES COMPILE_FLAGS "-g -march=i686 -Wall")
endif()

add_executable(chance_dice ${chance_dice_SRCS})

target_link_libraries(chance_dice pthread ACE xerces-c RPG_Dice)

install(TARGETS chance_dice RUNTIME DESTINATION bin)

########### next target ###############
set(chance_rangeToRoll_SRCS chance_rangeToRoll.cpp)

if (CMAKE_COMPILER_IS_GNUCC)
    set_source_files_properties(${chance_rangeToRoll_SRCS} PROPERTIES COMPILE_FLAGS "-g -march=i686 -Wall")
endif()

add_executable(chance_rangeToRoll ${chance_rangeToRoll_SRCS})

target_link_libraries(chance_rangeToRoll pthread ACE xerces-c RPG_Dice)

install(TARGETS chance_rangeToRoll RUNTIME DESTINATION bin)
