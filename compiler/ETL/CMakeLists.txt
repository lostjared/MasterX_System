cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(ETL_C LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find the scan_cxx package
find_package(scan_cxx REQUIRED PATHS /usr/lib/cmake /usr/local/lib/cmake)

# Compiler flags based on build type
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    list(APPEND COMPILER_FLAGS "-Wall" "-pedantic" "-Ofast")
else()
    message(STATUS "Debug Mode")
    list(APPEND COMPILER_FLAGS "-Wall" "-pedantic")
endif()

# Option to enable SDL
option(WITH_SDL "Build with SDL support" OFF)

# Add the core ETL library
add_library(etl STATIC ${CMAKE_SOURCE_DIR}/libetl/libetl.c ${CMAKE_SOURCE_DIR}/libetl/io.c)
target_include_directories(etl PUBLIC ${CMAKE_SOURCE_DIR}/libetl)

# Add the executable
add_executable(ETL_C 
    ${CMAKE_SOURCE_DIR}/main.cpp 
    ${CMAKE_SOURCE_DIR}/parser.cpp 
    ${CMAKE_SOURCE_DIR}/test.cpp 
    ${CMAKE_SOURCE_DIR}/ir.cpp
    ${CMAKE_SOURCE_DIR}/debug.cpp
    ${CMAKE_SOURCE_DIR}/interp.cpp
    ${CMAKE_SOURCE_DIR}/exec.cpp
    ${CMAKE_SOURCE_DIR}/embed_func.cpp
)

# Link scan_cxx library to ETL_C
target_include_directories(ETL_C PRIVATE ${scan_cxx_INCLUDE_DIRS})
target_link_libraries(ETL_C PRIVATE dl scan_cxx::cxx_scan)

# SDL configuration if WITH_SDL is enabled
if(WITH_SDL)
    find_package(SDL2 REQUIRED)
    
    # Create a separate SDL library
    add_library(etl_sdl STATIC ${CMAKE_SOURCE_DIR}/libetl/sdl.c)
    target_include_directories(etl_sdl PUBLIC ${SDL2_INCLUDE_DIRS} /usr/local/include/SDL2 /usr/include/SDL2)
    target_link_libraries(etl_sdl PRIVATE ${SDL2_LIBRARIES})
    # Conditionally install SDL-related files
    install(FILES ${CMAKE_SOURCE_DIR}/libetl/sdl.e DESTINATION include/libetl)
endif()

# Installation rules
install(TARGETS ETL_C DESTINATION bin)
install(TARGETS etl DESTINATION lib)
install(FILES ${CMAKE_SOURCE_DIR}/libetl/etl.e ${CMAKE_SOURCE_DIR}/libetl/io.e DESTINATION include/libetl)
install(PROGRAMS ${CMAKE_SOURCE_DIR}/etl DESTINATION bin)
install(PROGRAMS ${CMAKE_SOURCE_DIR}/etl-i DESTINATION bin)

# Conditionally install SDL library if WITH_SDL is set
if(WITH_SDL)
    install(TARGETS etl_sdl DESTINATION lib)
endif()
