project(duk_log)

set(DUK_LOG_HEADERS
        include/duk_log/logger.h
        include/duk_log/sink.h
        include/duk_log/sink_std_console.h
        )
set(DUK_LOG_SOURCES
        src/duk_log/logger.cpp
        src/duk_log/sink.cpp
        src/duk_log/sink_std_console.cpp
        )

add_library(duk_log STATIC ${DUK_LOG_HEADERS} ${DUK_LOG_SOURCES})

target_include_directories(duk_log
        PUBLIC
        include

        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        )

target_link_libraries(duk_log
        PUBLIC
        duk_macros
        duk_events
        duk_task
        duk_tools
        )

set_target_properties(duk_log PROPERTIES CXX_STANDARD 20)