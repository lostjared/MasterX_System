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
option(WITH_STATIC_SDL "build with static SDL" OFF)
# Add the core ETL library
add_library(etl STATIC ${CMAKE_SOURCE_DIR}/libetl/libetl.c ${CMAKE_SOURCE_DIR}/libetl/io.c)
target_include_directories(etl PUBLIC ${CMAKE_SOURCE_DIR}/libetl)

# Add the executable


# SDL configuration if WITH_SDL is enabled
if(WITH_SDL)
    find_package(SDL2 REQUIRED)
    find_package(SDL2_ttf REQUIRED)
    add_compile_definitions(WITH_SDL)    

    add_library(etl_sdl STATIC ${CMAKE_SOURCE_DIR}/libetl/sdl.c)
    target_include_directories(etl_sdl PUBLIC ${SDL2_INCLUDE_DIRS} /usr/local/include/SDL2 /usr/include/SDL2)
    target_link_libraries(etl_sdl PRIVATE ${SDL2_LIBRARIES})
    install(TARGETS etl_sdl DESTINATION lib)


    add_library(sdl_rt SHARED ${CMAKE_SOURCE_DIR}/libetl/sdl.c ${CMAKE_SOURCE_DIR}/libetl/shared/sdl/sdl.cpp)
    target_include_directories(sdl_rt PRIVATE ${scan_cxx_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS} /usr/local/include/SDL2 /usr/local/include /usr/include/SDL2)
    target_compile_options(sdl_rt PRIVATE -O2 -fPIC)
    target_link_libraries(sdl_rt PRIVATE ${SDL2_LIBRARIES} SDL2_ttf::SDL2_ttf)
    install(TARGETS sdl_rt DESTINATION /usr/local/lib)

    # Conditionally install SDL-related files
    install(FILES ${CMAKE_SOURCE_DIR}/libetl/sdl.e DESTINATION include/libetl)
    
    if(WITH_STATIC_SDL)
        add_compile_definitions(WITH_STATIC_SDL)    
        add_executable(ETL_C 
        ${CMAKE_SOURCE_DIR}/main.cpp 
        ${CMAKE_SOURCE_DIR}/parser.cpp 
        ${CMAKE_SOURCE_DIR}/test.cpp 
        ${CMAKE_SOURCE_DIR}/ir.cpp
        ${CMAKE_SOURCE_DIR}/debug.cpp
        ${CMAKE_SOURCE_DIR}/interp.cpp
        ${CMAKE_SOURCE_DIR}/exec.cpp
        ${CMAKE_SOURCE_DIR}/embed_func.cpp
        ${CMAKE_SOURCE_DIR}/libetl/sdl.c 
        ${CMAKE_SOURCE_DIR}/libetl/shared/sdl/sdl.cpp
        ${CMAKE_SOURCE_DIR}/libetl/io.c 
        ${CMAKE_SOURCE_DIR}/libetl/shared/io/io.cpp
        )
        target_include_directories(ETL_C PRIVATE ${scan_cxx_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS} /usr/local/include/SDL2 /usr/local/include /usr/include/SDL2)
        target_link_libraries(ETL_C PRIVATE dl scan_cxx::cxx_scan ${SDL2_LIBRARIES} SDL2_ttf::SDL2_ttf )
        target_compile_options(ETL_C PRIVATE -O2)
    else()
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
        target_include_directories(ETL_C PRIVATE ${scan_cxx_INCLUDE_DIRS})
        target_link_libraries(ETL_C PRIVATE dl scan_cxx::cxx_scan)  
        target_compile_options(ETL_C PRIVATE -O2)
    endif()
else()
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
target_include_directories(ETL_C PRIVATE ${scan_cxx_INCLUDE_DIRS})
target_link_libraries(ETL_C PRIVATE dl scan_cxx::cxx_scan)  
target_compile_options(ETL_C PRIVATE -O2)
endif()

add_library(io_rt SHARED ${CMAKE_SOURCE_DIR}/libetl/io.c ${CMAKE_SOURCE_DIR}/libetl/shared/io/io.cpp)
target_include_directories(io_rt PRIVATE ${scan_cxx_INCLUDE_DIRS})
target_compile_options(io_rt PRIVATE -O2 -fPIC)
install(TARGETS io_rt DESTINATION /usr/local/lib)

# Installation rules
install(TARGETS ETL_C DESTINATION bin)
install(TARGETS etl DESTINATION lib)
install(FILES ${CMAKE_SOURCE_DIR}/libetl/etl.e ${CMAKE_SOURCE_DIR}/libetl/io.e DESTINATION include/libetl)
install(PROGRAMS ${CMAKE_SOURCE_DIR}/etl DESTINATION bin)
install(PROGRAMS ${CMAKE_SOURCE_DIR}/etl-i DESTINATION bin)
install(FILES ${CMAKE_SOURCE_DIR}/etl-lib.txt DESTINATION "/usr/local/lib")
# Conditionally install SDL library if WITH_SDL is set
