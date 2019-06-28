
########### next target ###############
set (EXECUTABLE chance_dice)
set (${EXECUTABLE}_SRCS
     chance_dice.cpp)

add_executable (${EXECUTABLE}
                ${${EXECUTABLE}_SRCS})
set_property (TARGET ${EXECUTABLE} PROPERTY FOLDER "test_u")

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/log)

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../chance/dice
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../common)

target_link_libraries (${EXECUTABLE}
                       pthread xerces-c ACE RPG_Dice)

install (TARGETS ${EXECUTABLE} RUNTIME DESTINATION bin)

########### next target ###############
set (EXECUTABLE chance_rangeToRoll)
set (${EXECUTABLE}_SRCS
     chance_rangeToRoll.cpp)

add_executable (${EXECUTABLE}
                ${${EXECUTABLE}_SRCS})
set_property (TARGET ${EXECUTABLE} PROPERTY FOLDER "test_u")

target_link_libraries (${EXECUTABLE}
                       pthread xerces-c ACE RPG_Dice)

install (TARGETS ${EXECUTABLE} RUNTIME DESTINATION bin)
