cmake_minimum_required(VERSION 3.16)
project(mxx LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_executable(mxx 
    parser.cpp
    expr.cpp
    ast.cpp
    icode.cpp
    icode_imp.cpp                 
    main.cpp
    validator.cpp
    icode_visit.cpp
)
target_include_directories(mxx
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include/mxvm
        ${CMAKE_CURRENT_SOURCE_DIR}/src/scanner/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src/mxx/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../vm/include
)
target_link_libraries(mxx PUBLIC mxvm)
include(GNUInstallDirs)
set_target_properties(mxx PROPERTIES
    INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}"
)
install(TARGETS mxx
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
