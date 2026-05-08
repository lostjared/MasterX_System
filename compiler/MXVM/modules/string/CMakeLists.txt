cmake_minimum_required(VERSION 3.10)
project(mxvm_string)
set(SOURCES string.cpp)
add_library(mxvm_string SHARED ${SOURCES})
target_include_directories(mxvm_string
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include/mxvm
)
target_link_libraries(mxvm_string
    PUBLIC
        mxvm
)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
add_library(mxvm_string_static STATIC cstring.c)

include(CheckFunctionExists)

check_function_exists(strlen HAVE_STRLEN)
if(NOT HAVE_STRLEN)
    message(FATAL_ERROR "Required function 'strlen' not found.")
endif()

check_function_exists(strcmp HAVE_STRCMP)
if(NOT HAVE_STRCMP)
    message(FATAL_ERROR "Required function 'strcmp' not found.")
endif()

check_function_exists(strncpy HAVE_STRNCPY)
if(NOT HAVE_STRNCPY)
    message(FATAL_ERROR "Required function 'strncpy' not found.")
endif()

check_function_exists(strncat HAVE_STRNCAT)
if(NOT HAVE_STRNCAT)
    message(FATAL_ERROR "Required function 'strncat' not found.")
endif()

check_function_exists(snprintf HAVE_SNPRINTF)
if(NOT HAVE_SNPRINTF)
    message(FATAL_ERROR "Required function 'snprintf' not found.")
endif()


set_target_properties(mxvm_string PROPERTIES
    INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib/modules/string"
)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/string.mxvm ${CMAKE_CURRENT_BINARY_DIR}/string.mxvm COPYONLY)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/string.mxvm
    DESTINATION include/mxvm/modules/string
    RENAME string.mxvm
)

install(TARGETS mxvm_string mxvm_string_static
    LIBRARY DESTINATION lib/modules/string
    ARCHIVE DESTINATION lib/modules/string
)

if(WIN32)
    install(TARGETS mxvm_string
        RUNTIME DESTINATION lib/modules/string
    )
endif()
