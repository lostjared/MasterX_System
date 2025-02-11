cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(cxx_scan LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

message(STATUS "Build configuration is: ${CMAKE_BUILD_TYPE}")
message(STATUS "is the C++ compiler loaded? ${CMAKE_CXX_COMPILER_LOADED}")

if(CMAKE_CXX_COMPILER_LOADED)
    message(STATUS "The C++ Compiler ID is: ${CMAKE_CXX_COMPILER_ID}")
    message(STATUS "The C++ Compiler Version: ${CMAKE_CXX_COMPILER_VERSION}")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    list(APPEND COMPILER_FLAGS "-Wall" "-pedantic" "-Ofast")
else()
    message(STATUS "Debug Mode")
    list(APPEND COMPILER_FLAGS "-Wall" "-pedantic")
endif()

list(APPEND source_files ${CMAKE_SOURCE_DIR}/scanner.cpp ${CMAKE_SOURCE_DIR}/types.cpp ${CMAKE_SOURCE_DIR}/string_buffer.cpp)
list(APPEND header ${CMAKE_SOURCE_DIR}/types.hpp ${CMAKE_SOURCE_DIR}/string_buffer.hpp ${CMAKE_SOURCE_DIR}/scanner.hpp ${CMAKE_SOURCE_DIR}/argz.hpp)
add_library(cxx_scan ${header} ${source_files})

target_compile_options(cxx_scan PRIVATE ${COMPILER_FLAGS})

# Install header files to cxx_scan/include
install(FILES ${header} DESTINATION include/cxx_scan)

# Install the library to cxx_scan/lib
install(TARGETS cxx_scan
        EXPORT scan_cxxTargets
        LIBRARY DESTINATION lib/cxx_scan
        ARCHIVE DESTINATION lib/cxx_scan
        RUNTIME DESTINATION bin)

# Create and install the export set
install(EXPORT scan_cxxTargets
        FILE scan_cxxTargets.cmake
        NAMESPACE scan_cxx::
        DESTINATION lib/cmake)

# Set variables for include and library destinations
set(PACKAGE_INCLUDE_DESTINATION "${CMAKE_INSTALL_PREFIX}/include/cxx_scan")
set(PACKAGE_LIBRARY_DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cxx_scan")

# Configure the package config file
include(CMakePackageConfigHelpers)
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/scan_cxxConfig.cmake"
    INSTALL_DESTINATION lib/cmake
    PATH_VARS PACKAGE_INCLUDE_DESTINATION PACKAGE_LIBRARY_DESTINATION
)

# Install the package config file
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/scan_cxxConfig.cmake"
    DESTINATION lib/cmake
)

add_executable(scanner_html ${CMAKE_SOURCE_DIR}/html.cpp ${CMAKE_SOURCE_DIR}/main.cpp)
target_compile_options(scanner_html PRIVATE ${COMPILER_FLAGS})
target_link_libraries(scanner_html cxx_scan)

# Install the executable to cxx_scan/bin
install(TARGETS scanner_html DESTINATION bin)


# Add uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    @ONLY
)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
)
