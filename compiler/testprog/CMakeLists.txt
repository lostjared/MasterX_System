cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(my_project LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find the cxx_scan library
find_package(scan_cxx REQUIRED PATHS /usr/lib/cmake /usr/local/lib/cmake)

# Add your executable
add_executable(my_app ${CMAKE_SOURCE_DIR}/main.cpp)

target_include_directories(my_app PRIVATE ${scan_cxx_INCLUDE_DIRS})

# Link the cxx_scan library
target_link_libraries(my_app PRIVATE scan_cxx::cxx_scan)

# Install the executable
install(TARGETS my_app DESTINATION bin)
