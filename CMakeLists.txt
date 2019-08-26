cmake_minimum_required(VERSION 3.14)
project(Zomboid3D C)

set(CMAKE_C_STANDARD 99)

add_executable(Zomboid3D main.c graphics/graphics.c graphics/graphics.h graphics/win.h graphics/win.c)

target_link_libraries(Zomboid3D X11 GL)