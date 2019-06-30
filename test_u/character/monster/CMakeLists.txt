include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../chance/dice
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../../common
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../../item
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../../character
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../../magic
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../../combat
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../../character/monster)

#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

########### next target ###############
set (EXECUTABLE monster_parser)
set (${EXECUTABLE}_SRCS
     monster_parser.cpp)

set_source_files_properties(${${EXECUTABLE}_SRCS} PROPERTIES COMPILE_DEFINITIONS DATADIR="/usr/local/share/RPG/config")

add_executable(${EXECUTABLE} ${${EXECUTABLE}_SRCS})

target_link_libraries(${EXECUTABLE} ACE pthread xerces-c RPG_Dice RPG_Chance RPG_Common RPG_Magic
                                    RPG_Item RPG_Character RPG_Monster RPG_Player RPG_Combat)

set_property (TARGET ${EXECUTABLE} PROPERTY FOLDER "test_u")

########### install files ###############
install(TARGETS ${EXECUTABLE} RUNTIME DESTINATION bin)
