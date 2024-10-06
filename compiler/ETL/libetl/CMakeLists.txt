cmake_minimum_required(VERSION 3.10)

# Set the project name
project(libetl)

# Set the ETL_PATH (you may want to modify this or pass it in via a command-line option)
set(ETL_PATH /usr/local)

# Find the SDL2 package, requires that SDL2 is installed on your system
find_package(SDL2 REQUIRED)

# Set the output directories for the libraries
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Add compilation options
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2")

# Include directories for SDL2
include_directories(${SDL2_INCLUDE_DIRS})

# Add source files
add_library(etl STATIC libetl.c io.c)
add_library(etl_sdl STATIC sdl.c)

# Specify install paths
install(FILES etl.e DESTINATION ${ETL_PATH}/include/libetl/)
install(FILES sdl.e DESTINATION ${ETL_PATH}/include/libetl/)
install(FILES io.e DESTINATION $(ETL_PATH)/incldue/libetl/)
install(TARGETS etl etl_sdl DESTINATION ${ETL_PATH}/lib)

# Clean up command (optional, typically not necessary in CMake)
add_custom_target(cleanup
    COMMAND ${CMAKE_COMMAND} -E remove *.o
)

