cmake_minimum_required(VERSION 3.16)
project(vm LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_executable(mxvmc main.cpp)
target_include_directories(mxvmc
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include/mxvm
        ${CMAKE_CURRENT_SOURCE_DIR}/src/vm/include
)
target_link_libraries(mxvmc PUBLIC mxvm)
include(GNUInstallDirs)
set_target_properties(mxvmc PROPERTIES
    INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}"
)
install(TARGETS mxvmc
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)