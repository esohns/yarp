
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/log)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../chance/dice
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../common
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../item
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../character
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../magic)

#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

############ next target ########################
set (EXECUTABLE item_parser)
set (${EXECUTABLE}_SRCS
     item_parser.cpp)

set_source_files_properties (${${EXECUTABLE}_SRCS} PROPERTIES COMPILE_DEFINITIONS DATADIR="/usr/local/share/RPG/config")

add_executable (${EXECUTABLE} ${${EXECUTABLE}_SRCS})

target_link_libraries (${EXECUTABLE}
                       pthread xerces-c
                       ACE 
                       RPG_Dice RPG_Chance RPG_Common RPG_Character RPG_Magic RPG_Item)

install (TARGETS ${EXECUTABLE} RUNTIME DESTINATION bin)
