cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(ETL_C LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(scan_cxx REQUIRED PATHS /usr/lib/cmake /usr/local/lib/cmake)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    list(APPEND COMPILER_FLAGS "-Wall" "-pedantic" "-Ofast")
else()
    message(STATUS "Debug Mode")
    list(APPEND COMPILER_FLAGS "-Wall" "-pedantic")
endif()

add_library(etl STATIC ${CMAKE_SOURCE_DIR}/libetl/libetl.c)
target_include_directories(etl PUBLIC ${CMAKE_SOURCE_DIR}/libetl)

add_executable(ETL_C 
    ${CMAKE_SOURCE_DIR}/main.cpp 
    ${CMAKE_SOURCE_DIR}/parser.cpp 
    ${CMAKE_SOURCE_DIR}/test.cpp 
    ${CMAKE_SOURCE_DIR}/ir.cpp
    ${CMAKE_SOURCE_DIR}/debug.cpp
)

target_include_directories(ETL_C PRIVATE ${scan_cxx_INCLUDE_DIRS})
target_link_libraries(ETL_C PRIVATE scan_cxx::cxx_scan)

install(TARGETS ETL_C DESTINATION bin)
install(TARGETS etl DESTINATION lib)
install(FILES ${CMAKE_SOURCE_DIR}/libetl/etl.e DESTINATION include/libetl)
install(PROGRAMS ${CMAKE_SOURCE_DIR}/etl DESTINATION bin)