cmake_minimum_required(VERSION 3.14)
project(Zomboid3D C)

set(CMAKE_C_STANDARD 99)

add_executable(Zomboid3D main.c graphics/graphics.c graphics/graphics.h graphics/win.h graphics/win.c updater.c updater.h shaders/shader.c shaders/shader.h shaders/shaderMgr.c shaders/shaderMgr.h)

target_link_libraries(Zomboid3D X11 GL)