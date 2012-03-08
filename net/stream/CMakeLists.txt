cmake_minimum_required(VERSION 2.8)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../chance/dice
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../common)

#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)

########### next target ###############
set(RPG_Stream_LIB_SRCS
    rpg_stream.cpp
    rpg_stream.h
    rpg_stream_allocatorheap.cpp
    rpg_stream_allocatorheap.h
    rpg_stream_cachedallocatorheap.cpp
    rpg_stream_cachedallocatorheap.h
    rpg_stream_base.cpp
    rpg_stream_base.h
    rpg_stream_base.i
    rpg_stream_counter.cpp
    rpg_stream_counter.h
    rpg_stream_data_message_base.cpp
    rpg_stream_data_message_base.h
    rpg_stream_data_message_base.i
    rpg_stream_datablockallocatorheap.cpp
    rpg_stream_datablockallocatorheap.h
    rpg_stream_cacheddatablockallocatorheap.cpp
    rpg_stream_cacheddatablockallocatorheap.h
    rpg_stream_defines.h
    rpg_stream_exports.h
    rpg_stream_headmoduletask.cpp
    rpg_stream_headmoduletask.h
    rpg_stream_headmoduletask_base.cpp
    rpg_stream_headmoduletask_base.h
    rpg_stream_headmoduletask_base.i
    rpg_stream_iallocator.h
    rpg_stream_imessagequeue.h
    rpg_stream_irefcount.h
    rpg_stream_istreamcontrol.h
    rpg_stream_itask.h
    rpg_stream_itask_base.h
    rpg_stream_message_base.cpp
    rpg_stream_message_base.h
    rpg_stream_messageallocatorheap.cpp
    rpg_stream_messageallocatorheap.h
    rpg_stream_cachedmessageallocatorheap.cpp
    rpg_stream_cachedmessageallocatorheap.h
    rpg_stream_messageallocatorheap_base.cpp
    rpg_stream_messageallocatorheap_base.h
    rpg_stream_messageallocatorheap_base.i
    rpg_stream_cachedmessageallocatorheap_base.cpp
    rpg_stream_cachedmessageallocatorheap_base.h
    rpg_stream_cachedmessageallocatorheap_base.i
    rpg_stream_messagequeue.cpp
    rpg_stream_messagequeue.h
    rpg_stream_messagequeue_base.cpp
    rpg_stream_messagequeue_base.h
    rpg_stream_module.cpp
    rpg_stream_module.h
    rpg_stream_session_config.cpp
    rpg_stream_session_config.h
    rpg_stream_session_config_base.cpp
    rpg_stream_session_config_base.h
    rpg_stream_session_config_base.i
    rpg_stream_session_message.cpp
    rpg_stream_session_message.h
    rpg_stream_session_message_base.cpp
    rpg_stream_session_message_base.h
    rpg_stream_session_message_base.i
    rpg_stream_statemachine_control.cpp
    rpg_stream_statemachine_control.h
    rpg_stream_streammodule.cpp
    rpg_stream_streammodule.h
    rpg_stream_streammodule.i
    rpg_stream_streammodule_base.cpp
    rpg_stream_streammodule_base.h
    rpg_stream_streammodule_base.i
    rpg_stream_task.cpp
    rpg_stream_task.h
    rpg_stream_task_asynch.cpp
    rpg_stream_task_asynch.h
    rpg_stream_task_base.cpp
    rpg_stream_task_base.h
    rpg_stream_task_base.i
    rpg_stream_task_base_asynch.cpp
    rpg_stream_task_base_asynch.h
    rpg_stream_task_base_asynch.i
    rpg_stream_task_base_synch.cpp
    rpg_stream_task_base_synch.h
    rpg_stream_task_base_synch.i
    rpg_stream_task_synch.cpp
    rpg_stream_task_synch.h
    rpg_stream_tools.cpp
    rpg_stream_tools.h)

if (CMAKE_COMPILER_IS_GNUCC)
    set_source_files_properties(${RPG_Stream_LIB_SRCS} PROPERTIES COMPILE_FLAGS "-g -march=i686 -Wall")
endif()
set_source_files_properties(${RPG_Stream_LIB_SRCS} PROPERTIES COMPILE_DEFINITIONS RPG_STREAM_BUILD_DLL)

add_library(RPG_Stream SHARED ${RPG_Stream_LIB_SRCS})

set_target_properties(RPG_Stream PROPERTIES VERSION 1.0.0 SOVERSION 1)

########### install files ###############
install(FILES
        rpg_stream.h
        rpg_stream_allocatorheap.h
        rpg_stream_cachedallocatorheap.h
        rpg_stream_base.h
        rpg_stream_base.i
        rpg_stream_counter.h
        rpg_stream_data_message_base.h
        rpg_stream_data_message_base.i
        rpg_stream_datablockallocatorheap.h
        rpg_stream_cacheddatablockallocatorheap.h
        rpg_stream_defines.h
        rpg_stream_exports.h
        rpg_stream_headmoduletask.h
        rpg_stream_headmoduletask_base.h
        rpg_stream_headmoduletask_base.i
        rpg_stream_iallocator.h
        rpg_stream_imessagequeue.h
        rpg_stream_irefcount.h
        rpg_stream_istreamcontrol.h
        rpg_stream_itask.h
        rpg_stream_itask_base.h
        rpg_stream_message_base.h
        rpg_stream_messageallocatorheap.h
        rpg_stream_cachedmessageallocatorheap.h
        rpg_stream_messageallocatorheap_base.h
        rpg_stream_messageallocatorheap_base.i
        rpg_stream_cachedmessageallocatorheap_base.h
        rpg_stream_cachedmessageallocatorheap_base.i
        rpg_stream_messagequeue.h
        rpg_stream_messagequeue_base.h
        rpg_stream_module.h
        rpg_stream_session_config.h
        rpg_stream_session_config_base.h
        rpg_stream_session_config_base.i
        rpg_stream_session_message.h
        rpg_stream_session_message_base.h
        rpg_stream_session_message_base.i
        rpg_stream_statemachine_control.h
        rpg_stream_streammodule.h
        rpg_stream_streammodule.i
        rpg_stream_streammodule_base.h
        rpg_stream_streammodule_base.i
        rpg_stream_task.h
        rpg_stream_task_asynch.h
        rpg_stream_task_base.h
        rpg_stream_task_base.i
        rpg_stream_task_base_asynch.h
        rpg_stream_task_base_asynch.i
        rpg_stream_task_base_synch.h
        rpg_stream_task_base_synch.i
        rpg_stream_task_synch.h
        rpg_stream_tools.h
        DESTINATION include)
install(TARGETS RPG_Stream LIBRARY DESTINATION lib)
