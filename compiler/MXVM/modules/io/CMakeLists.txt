cmake_minimum_required(VERSION 3.10)
project(mxvm_io)
set(SOURCES io.cpp)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
add_library(mxvm_io SHARED ${SOURCES})
target_include_directories(mxvm_io
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include/mxvm
)
target_link_libraries(mxvm_io
    PUBLIC
        mxvm
)
add_library(mxvm_io_static STATIC io.c)
include(CheckFunctionExists)

check_function_exists(fopen HAVE_FOPEN)
if(NOT HAVE_FOPEN)
    message(FATAL_ERROR "Required function 'fopen' not found.")
endif()

check_function_exists(fclose HAVE_FCLOSE)
if(NOT HAVE_FCLOSE)
    message(FATAL_ERROR "Required function 'fclose' not found.")
endif()

check_function_exists(fread HAVE_FREAD)
if(NOT HAVE_FREAD)
    message(FATAL_ERROR "Required function 'fread' not found.")
endif()

check_function_exists(fwrite HAVE_FWRITE)
if(NOT HAVE_FWRITE)
    message(FATAL_ERROR "Required function 'fwrite' not found.")
endif()

check_function_exists(fseek HAVE_FSEEK)
if(NOT HAVE_FSEEK)
    message(FATAL_ERROR "Required function 'fseek' not found.")
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/io.mxvm ${CMAKE_CURRENT_BINARY_DIR}/io.mxvm COPYONLY)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/io.mxvm
    DESTINATION include/mxvm/modules/io
    RENAME io.mxvm
)

set_target_properties(mxvm_io PROPERTIES
    INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib/modules/io"
)

install(TARGETS mxvm_io mxvm_io_static
    LIBRARY DESTINATION lib/modules/io
    ARCHIVE DESTINATION lib/modules/io
)
if(WIN32)
    install(TARGETS mxvm_io
        RUNTIME DESTINATION lib/modules/io
    )
endif()
