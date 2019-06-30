
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

set_target_properties (${EXECUTABLE} PROPERTIES LINK_FLAGS /FORCE:MULTIPLE)

target_link_libraries (${EXECUTABLE}
                       $ENV{LIB_ROOT}/xerces-c/build/src/Debug/xerces-c_3D.lib
                       $ENV{LIB_ROOT}/ACE_TAO/ACE/lib/ACEd.lib
                       RPG_Dice)

install (TARGETS ${EXECUTABLE} RUNTIME DESTINATION bin)

########### next target ###############
set (EXECUTABLE chance_rangeToRoll)
set (${EXECUTABLE}_SRCS
     chance_rangeToRoll.cpp)

add_executable (${EXECUTABLE}
                ${${EXECUTABLE}_SRCS})
set_property (TARGET ${EXECUTABLE} PROPERTY FOLDER "test_u")

set_target_properties (${EXECUTABLE} PROPERTIES LINK_FLAGS /FORCE:MULTIPLE)

target_link_libraries (${EXECUTABLE}
                       Userenv.lib
                       $ENV{LIB_ROOT}/xerces-c/build/src/Debug/xerces-c_3D.lib
                       $ENV{LIB_ROOT}/ACE_TAO/ACE/lib/ACEd.lib
                       Common CommonError CommonLog
                       RPG_Dice)

install (TARGETS ${EXECUTABLE} RUNTIME DESTINATION bin)
