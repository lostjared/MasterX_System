cmake_minimum_required(VERSION 3.10)
project(MasterX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O2 -Wall")
find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(ZLIB REQUIRED)
find_package(PNG REQUIRED)
find_package(OpenGL REQUIRED COMPONENTS GLES2)
include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_TTF_INCLUDE_DIRS} /usr/include /usr/include/SDL2 /usr/local/include /usr/local/include/SDL2 ${ZLIB_INCLUDE_DIRS} ${PNG_INCLUDE_DIRS})
link_directories(/usr/lib /usr/local/lib)
add_executable(MasterX src/main.cpp src/window.cpp src/gl_context.cpp src/splash.cpp src/dimension.cpp src/terminal.cpp src/mx_editor.cpp src/mx_controls.cpp src/mx_window.cpp src/mx_system_bar.cpp src/mx_event.cpp src/loadpng.cpp src/masterpiece.cpp src/cfg.cpp src/matrix.cpp src/mx_menu.cpp src/asteroid_window.cpp src/tetris_window.cpp src/messagebox.cpp src/pac_window.cpp src/pong_window.cpp) 
target_link_libraries(MasterX ${SDL2_LIBRARIES} SDL2_ttf ${PNG_LIBRARIES} ${ZLIB_LIBRARIES} OpenGL::GLES2)

# Install the executable to /usr/local/bin (default CMAKE_INSTALL_PREFIX).
install(TARGETS MasterX
	RUNTIME DESTINATION bin
)

# Install runtime assets to /usr/local/share/applications/MasterX/assets.
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/assets/
	DESTINATION share/applications/MasterX/assets
)

# Install desktop launcher.
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/MasterX_System.desktop
	DESTINATION share/applications
)
