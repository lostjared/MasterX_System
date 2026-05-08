cmake_minimum_required(VERSION 3.16)
project(mxvm LANGUAGES C CXX VERSION 1.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-std=c++20" COMPILER_SUPPORTS_CXX20)
if(NOT COMPILER_SUPPORTS_CXX20)
    message(FATAL_ERROR "Compiler does not support C++20")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
    set(DEBUG_MODE "Release Mode")
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g ")
    set(DEBUG_MODE "Debug Mode")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g ")
    set(DEBUG_MODE "Debug Mode")
endif()

option(WITH_SDL "Build with SDL module support" OFF)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -fPIC")

include(CheckIncludeFileCXX)

check_include_file_cxx("unistd.h" HAVE_UNISTD_H)
check_include_file_cxx("fcntl.h" HAVE_FCNTL_H)
check_include_file_cxx("sys/types.h" HAVE_SYS_TYPES_H)
check_include_file_cxx("sys/stat.h" HAVE_SYS_STAT_H)
check_include_file_cxx("errno.h" HAVE_ERRNO_H)
check_include_file_cxx("cstdint" HAVE_STDINT_H)
check_include_file_cxx("cstring" HAVE_STRING_H)
check_include_file_cxx("cmath" HAVE_MATH_H)
check_include_file_cxx("stdio.h" HAVE_STDIO_H)
check_include_file_cxx("stdlib.h" HAVE_STDLIB_H)
check_include_file_cxx("stdint.h" HAVE_STDINT__H)

if(NOT HAVE_UNISTD_H)
    message(FATAL_ERROR "Missing required header: unistd.h")
endif()
if(NOT HAVE_FCNTL_H)
    message(FATAL_ERROR "Missing required header: fcntl.h")
endif()
if(NOT HAVE_SYS_TYPES_H)
    message(FATAL_ERROR "Missing required header: sys/types.h")
endif()
if(NOT HAVE_SYS_STAT_H)
    message(FATAL_ERROR "Missing required header: sys/stat.h")
endif()
if(NOT HAVE_ERRNO_H)
    message(FATAL_ERROR "Missing required header: errno.h")
endif()
if(NOT HAVE_STDINT_H)
    message(FATAL_ERROR "Missing required header: cstdint")
endif()
if(NOT HAVE_STDINT__H)
    message(FATAL_ERROR "Missing required header: stdint.h")
endif()
if(NOT HAVE_STRING_H)
    message(FATAL_ERROR "Missing required header: cstring")
endif()
if(NOT HAVE_MATH_H)
    message(FATAL_ERROR "Missing required header: cmath")
endif()
if(NOT HAVE_STDIO_H)
    message(FATAL_ERROR "Missing required header: stdio.h")
endif()
if(NOT HAVE_STDLIB_H)
    message(FATAL_ERROR "Missing required header: stdlib.h")
endif()

add_library(mxvm SHARED
    src/mxvm.cpp
    src/parser.cpp
    src/instruct.cpp
    src/icode.cpp
    src/icode_gen.cpp
    src/icode_gen_x64.cpp
    src/icode_exec.cpp
    src/icode_opt.cpp
    src/ast.cpp
    src/valid.cpp
    src/function.cpp
    src/html_gen.cpp
)

include(CheckLibraryExists)
check_library_exists(dl dlopen "" HAVE_LIBDL)
check_library_exists(pthread pthread_create "" HAVE_LIBPTHREAD)

if(WIN32)
    find_package(dlfcn-win32 REQUIRED)
    target_link_libraries(mxvm PUBLIC scanner dlfcn-win32::dl)
else()
    if(NOT HAVE_LIBDL)
        message(FATAL_ERROR "Missing required library: dl")
    endif()
    target_link_libraries(mxvm PUBLIC scanner)
endif()

if(NOT HAVE_LIBPTHREAD)
    message(FATAL_ERROR "Missing required library: pthread")
endif()
if(NOT CMAKE_CROSSCOMPILING)
    find_program(CMAKE_C_COMPILER NAMES gcc clang REQUIRED)
    find_program(CMAKE_CXX_COMPILER NAMES g++ clang++ REQUIRED)
    if(NOT CMAKE_C_COMPILER)
        message(FATAL_ERROR "C compiler not found (gcc or clang required)")
    endif()
    if(NOT CMAKE_CXX_COMPILER)
        message(FATAL_ERROR "C++ compiler not found (g++ or clang++ required)")
    endif()
endif()
find_program(AR_EXECUTABLE ar)
if(NOT AR_EXECUTABLE AND NOT CMAKE_CROSSCOMPILING)
    message(FATAL_ERROR "ar tool not found")
endif()
add_subdirectory(src/scanner)

if(WITH_SDL)
    add_subdirectory(modules/sdl)
endif()
target_include_directories(mxvm PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/mxvm
    ${CMAKE_CURRENT_SOURCE_DIR}/src/scanner/include
)

add_subdirectory(modules/io)
add_subdirectory(modules/std)
add_subdirectory(modules/string)
add_compile_definitions()
add_subdirectory(src/vm)
add_subdirectory(src/html_gen)
add_subdirectory(src/frontend)
#add_subdirectory(tests)
include(GNUInstallDirs)
install(TARGETS mxvm
    EXPORT mxvmTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

message(STATUS "CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
message(STATUS "Mode: ${DEBUG_MODE}")
message(STATUS "Set CMAKE_BUILD_TYPE to Debug/Release")
if(WITH_SDL)
    message(STATUS "Building with SDL/SDL_ttf Support")
else()
    message(STATUS "Building without SDL/SDL_ttf Support use -DWITH_SDL=ON")
endif()

