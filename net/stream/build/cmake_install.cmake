# Install script for directory: /mnt/win_c/Projects/RPG/stream

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "/usr/local")
ENDIF(NOT DEFINED CMAKE_INSTALL_PREFIX)
STRING(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
IF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  IF(BUILD_TYPE)
    STRING(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  ELSE(BUILD_TYPE)
    SET(CMAKE_INSTALL_CONFIG_NAME "Debug")
  ENDIF(BUILD_TYPE)
  MESSAGE(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
ENDIF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)

# Set the component getting installed.
IF(NOT CMAKE_INSTALL_COMPONENT)
  IF(COMPONENT)
    MESSAGE(STATUS "Install component: \"${COMPONENT}\"")
    SET(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  ELSE(COMPONENT)
    SET(CMAKE_INSTALL_COMPONENT)
  ENDIF(COMPONENT)
ENDIF(NOT CMAKE_INSTALL_COMPONENT)

# Install shared libraries without execute permission?
IF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  SET(CMAKE_INSTALL_SO_NO_EXE "0")
ENDIF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FOREACH(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libRPG_Stream.so.1.0.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libRPG_Stream.so.1"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libRPG_Stream.so"
      )
    IF(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      FILE(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    ENDIF()
  ENDFOREACH()
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES
    "/mnt/win_c/Projects/RPG/stream/build/libRPG_Stream.so.1.0.0"
    "/mnt/win_c/Projects/RPG/stream/build/libRPG_Stream.so.1"
    "/mnt/win_c/Projects/RPG/stream/build/libRPG_Stream.so"
    )
  FOREACH(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libRPG_Stream.so.1.0.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libRPG_Stream.so.1"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libRPG_Stream.so"
      )
    IF(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      IF(CMAKE_INSTALL_DO_STRIP)
        EXECUTE_PROCESS(COMMAND "/usr/bin/strip" "${file}")
      ENDIF(CMAKE_INSTALL_DO_STRIP)
    ENDIF()
  ENDFOREACH()
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES
    "/mnt/win_c/Projects/RPG/stream/rpg_stream.h"
    "/mnt/win_c/Projects/RPG/stream/rpg_stream_allocatorheap.h"
    "/mnt/win_c/Projects/RPG/stream/rpg_stream_base.h"
    "/mnt/win_c/Projects/RPG/stream/rpg_stream_base.i"
    "/mnt/win_c/Projects/RPG/stream/rpg_stream_counter.h"
    "/mnt/win_c/Projects/RPG/stream/rpg_stream_data_message_base.h"
    "/mnt/win_c/Projects/RPG/stream/rpg_stream_data_message_base.i"
    "/mnt/win_c/Projects/RPG/stream/rpg_stream_datablockallocatorheap.h"
    "/mnt/win_c/Projects/RPG/stream/rpg_stream_defines.h"
    "/mnt/win_c/Projects/RPG/stream/rpg_stream_headmoduletask.h"
    "/mnt/win_c/Projects/RPG/stream/rpg_stream_headmoduletask_base.h"
    "/mnt/win_c/Projects/RPG/stream/rpg_stream_headmoduletask_base.i"
    "/mnt/win_c/Projects/RPG/stream/rpg_stream_iallocator.h"
    "/mnt/win_c/Projects/RPG/stream/rpg_stream_imessagequeue.h"
    "/mnt/win_c/Projects/RPG/stream/rpg_stream_irefcount.h"
    "/mnt/win_c/Projects/RPG/stream/rpg_stream_istreamcontrol.h"
    "/mnt/win_c/Projects/RPG/stream/rpg_stream_itask.h"
    "/mnt/win_c/Projects/RPG/stream/rpg_stream_itask_base.h"
    "/mnt/win_c/Projects/RPG/stream/rpg_stream_message_base.h"
    "/mnt/win_c/Projects/RPG/stream/rpg_stream_messageallocatorheap.h"
    "/mnt/win_c/Projects/RPG/stream/rpg_stream_messageallocatorheap_base.h"
    "/mnt/win_c/Projects/RPG/stream/rpg_stream_messageallocatorheap_base.i"
    "/mnt/win_c/Projects/RPG/stream/rpg_stream_messagequeue.h"
    "/mnt/win_c/Projects/RPG/stream/rpg_stream_messagequeue_base.h"
    "/mnt/win_c/Projects/RPG/stream/rpg_stream_module.h"
    "/mnt/win_c/Projects/RPG/stream/rpg_stream_session_config.h"
    "/mnt/win_c/Projects/RPG/stream/rpg_stream_session_config_base.h"
    "/mnt/win_c/Projects/RPG/stream/rpg_stream_session_config_base.i"
    "/mnt/win_c/Projects/RPG/stream/rpg_stream_session_message.h"
    "/mnt/win_c/Projects/RPG/stream/rpg_stream_session_message_base.h"
    "/mnt/win_c/Projects/RPG/stream/rpg_stream_session_message_base.i"
    "/mnt/win_c/Projects/RPG/stream/rpg_stream_statemachine_control.h"
    "/mnt/win_c/Projects/RPG/stream/rpg_stream_streammodule.h"
    "/mnt/win_c/Projects/RPG/stream/rpg_stream_streammodule.i"
    "/mnt/win_c/Projects/RPG/stream/rpg_stream_streammodule_base.h"
    "/mnt/win_c/Projects/RPG/stream/rpg_stream_streammodule_base.i"
    "/mnt/win_c/Projects/RPG/stream/rpg_stream_task.h"
    "/mnt/win_c/Projects/RPG/stream/rpg_stream_task_asynch.h"
    "/mnt/win_c/Projects/RPG/stream/rpg_stream_task_base.h"
    "/mnt/win_c/Projects/RPG/stream/rpg_stream_task_base.i"
    "/mnt/win_c/Projects/RPG/stream/rpg_stream_task_base_asynch.h"
    "/mnt/win_c/Projects/RPG/stream/rpg_stream_task_base_asynch.i"
    "/mnt/win_c/Projects/RPG/stream/rpg_stream_task_base_synch.h"
    "/mnt/win_c/Projects/RPG/stream/rpg_stream_task_base_synch.i"
    "/mnt/win_c/Projects/RPG/stream/rpg_stream_task_synch.h"
    "/mnt/win_c/Projects/RPG/stream/rpg_stream_tools.h"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(CMAKE_INSTALL_COMPONENT)
  SET(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
ELSE(CMAKE_INSTALL_COMPONENT)
  SET(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
ENDIF(CMAKE_INSTALL_COMPONENT)

FILE(WRITE "/mnt/win_c/Projects/RPG/stream/build/${CMAKE_INSTALL_MANIFEST}" "")
FOREACH(file ${CMAKE_INSTALL_MANIFEST_FILES})
  FILE(APPEND "/mnt/win_c/Projects/RPG/stream/build/${CMAKE_INSTALL_MANIFEST}" "${file}\n")
ENDFOREACH(file)
