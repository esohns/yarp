
########### next target ###############
set (EXECUTABLE chance_dice)
set (${EXECUTABLE}_SRCS
     chance_dice.cpp
     stdafx.h)

add_executable (${EXECUTABLE}
                ${${EXECUTABLE}_SRCS})
set_property (TARGET ${EXECUTABLE} PROPERTY FOLDER "test_u")

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/log)

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../chance/dice
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../common)

if (UNIX)
 target_link_libraries (${EXECUTABLE}
                        xerces-c
                        ${ACE_LIBRARY}
                        RPG_Dice)
elseif (WIN32)
# set_target_properties (${EXECUTABLE} PROPERTIES LINK_FLAGS /FORCE:MULTIPLE)
 target_link_libraries (${EXECUTABLE}
                        $ENV{LIB_ROOT}/xerces-c/build/msvc/src/Debug/xerces-c_3D.lib
                        $ENV{LIB_ROOT}/ACE_TAO/ACE/lib/ACEd.lib
                        RPG_Dice)
endif ()

# add PCH
target_precompile_headers (${EXECUTABLE}
                           PRIVATE stdafx.h)

install (TARGETS ${EXECUTABLE} RUNTIME DESTINATION bin)

########### next target ###############
set (EXECUTABLE_2 chance_rangeToRoll)
set (${EXECUTABLE_2}_SRCS
     chance_rangeToRoll.cpp
     stdafx.h)

add_executable (${EXECUTABLE_2}
                ${${EXECUTABLE_2}_SRCS})
set_property (TARGET ${EXECUTABLE_2} PROPERTY FOLDER "test_u")

if (WIN32)
 set_target_properties (${EXECUTABLE} PROPERTIES LINK_FLAGS /FORCE:MULTIPLE)
endif (WIN32)

if (UNIX)
 target_link_libraries (${EXECUTABLE_2}
                        xerces-c
                        ${ACE_LIBRARY}
                        Common CommonLog
                        RPG_Dice
                        Common)
elseif (WIN32)
 target_link_libraries (${EXECUTABLE_2}
                        Userenv.lib
                        $ENV{LIB_ROOT}/xerces-c/build/msvc/src/Debug/xerces-c_3D.lib
                        $ENV{LIB_ROOT}/ACE_TAO/ACE/lib/ACEd.lib
                        Common CommonError CommonLog
                        RPG_Dice)
endif ()

# add PCH
target_precompile_headers (${EXECUTABLE_2}
                           REUSE_FROM ${EXECUTABLE})

install (TARGETS ${EXECUTABLE_2} RUNTIME DESTINATION bin)

##########################################

if (UNIX)
 file (TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${EXECUTABLE}" COMMAND)
 create_target_launcher (${EXECUTABLE}
                         COMMAND ${COMMAND}
                         ARGS "-t"
                         FORWARD_ARGS
                         WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                         ENVIRONMENT foo=bar)
 file (TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${EXECUTABLE_2}" COMMAND)
 create_target_launcher (${EXECUTABLE_2}
                         COMMAND ${COMMAND}
                         ARGS "-t"
                         FORWARD_ARGS
                         WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                         ENVIRONMENT foo=bar)
elseif (WIN32)
 get_filename_component (ACE_LIB_PATH ${ACE_LIBRARY}
                         DIRECTORY)
 file (TO_CMAKE_PATH "$ENV{LIB_ROOT}" LIB_ROOT)
 file (TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${EXECUTABLE}.exe" COMMAND)
 string (REPLACE "\\" "\\\\" COMMAND ${COMMAND})
 create_target_launcher (${EXECUTABLE}
                         COMMAND ${COMMAND}
                         ARGS "-p -r 5-12 -t"
                         FORWARD_ARGS
                         RUNTIME_LIBRARY_DIRS ${LIB_ROOT}/xerces-c/build/msvc/src/Debug
                                              ${LIB_ROOT}/ACE_TAO/ACE/lib
                         WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                         ENVIRONMENT foo=bar)
 file (TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${EXECUTABLE_2}.exe" COMMAND)
 string (REPLACE "\\" "\\\\" COMMAND ${COMMAND})
 create_target_launcher (${EXECUTABLE_2}
                         COMMAND ${COMMAND}
                         ARGS "-p -r 5-12 -t"
                         FORWARD_ARGS
                         RUNTIME_LIBRARY_DIRS ${LIB_ROOT}/xerces-c/build/msvc/src/Debug
                                              ${LIB_ROOT}/ACE_TAO/ACE/lib
                         WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                         ENVIRONMENT foo=bar)
endif ()
