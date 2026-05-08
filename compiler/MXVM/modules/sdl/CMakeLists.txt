cmake_minimum_required(VERSION 3.10)
project(mxvm_sdl)

if(APPLE)
    list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew" "/usr/local")
endif()

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED IMPORTED_TARGET sdl2)
pkg_check_modules(SDL2_TTF REQUIRED IMPORTED_TARGET SDL2_ttf)

add_library(mxvm_sdl_static STATIC sdl.c)
add_library(mxvm_sdl SHARED sdl.cpp)

target_include_directories(mxvm_sdl_static PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/mxvm
)

target_include_directories(mxvm_sdl PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/mxvm
)

target_link_libraries(mxvm_sdl_static PRIVATE 
    PkgConfig::SDL2 
    PkgConfig::SDL2_TTF 
    mxvm
)

target_link_libraries(mxvm_sdl PRIVATE 
    mxvm_sdl_static
    PkgConfig::SDL2 
    PkgConfig::SDL2_TTF 
    mxvm
)

set_target_properties(mxvm_sdl PROPERTIES
    INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib/modules/sdl"
)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/sdl.mxvm ${CMAKE_CURRENT_BINARY_DIR}/sdl.mxvm COPYONLY)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/sdl.mxvm
    DESTINATION include/mxvm/modules/sdl
    RENAME sdl.mxvm
)

install(TARGETS mxvm_sdl mxvm_sdl_static
    LIBRARY DESTINATION lib/modules/sdl
    ARCHIVE DESTINATION lib/modules/sdl
    RUNTIME DESTINATION lib/modules/sdl
)
