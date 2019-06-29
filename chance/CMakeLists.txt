include (${CMAKE_CURRENT_SOURCE_DIR}/../../Common/scripts/Threads.cmake)

include (${CMAKE_CURRENT_SOURCE_DIR}/../../Common/scripts/ACE.cmake)

########### next target ###############
set (LIBRARY RPG_Chance)
set (${LIBRARY}_SRCS
     rpg_chance_common_tools.cpp
     rpg_chance_common_tools.h)

#set_source_files_properties (${${LIBRARY}_SRCS} PROPERTIES COMPILE_DEFINITIONS RPG_CHANCE_BUILD_DLL)

add_library (${LIBRARY} STATIC ${${LIBRARY}_SRCS})

if (UNIX)
elseif (WIN32)
 target_include_directories (${LIBRARY} PUBLIC
                             $<BUILD_INTERFACE:$ENV{LIB_ROOT}/xsd/include>)
endif ()

if (UNIX)
 target_include_directories (${LIBRARY} PUBLIC
#                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../modules/ACE>)
#                             $<BUILD_INTERFACE:$ENV{LIB_ROOT}/ACE_TAO/ACE>)
                             $<BUILD_INTERFACE:/usr/local/src/ACE_wrappers>)
elseif (WIN32)
 if (DEFINED ENV{ACE_ROOT})
  target_include_directories (${LIBRARY} PUBLIC
                              $<BUILD_INTERFACE:$ENV{ACE_ROOT}>)
 else ()
  target_include_directories (${LIBRARY} PUBLIC
                              $<BUILD_INTERFACE:$ENV{LIB_ROOT}/ACE_TAO/ACE>)
 endif (DEFINED ENV{ACE_ROOT})
endif ()

target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/dice>)
target_include_directories (${LIBRARY} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../common>)

set_target_properties (${LIBRARY} PROPERTIES VERSION 1.0.0 SOVERSION 1)

add_dependencies (${LIBRARY} RPG_Dice)

########### install files ###############
install (FILES
         rpg_chance_common_tools.h
       # rpg_chance_exports.h
         DESTINATION include)
install (TARGETS ${LIBRARY} LIBRARY DESTINATION lib)

add_subdirectory (dice)
