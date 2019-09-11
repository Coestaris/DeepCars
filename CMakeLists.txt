cmake_minimum_required(VERSION 3.14)
project(Zomboid3D C)

set(CMAKE_C_STANDARD 99)

FILE(GLOB oilSources oil/*.c)
FILE(GLOB oilHeaders oil/*.h)

add_executable(Zomboid3D main.c graphics/graphics.c graphics/graphics.h graphics/win.h graphics/win.c updater.c updater.h shaders/shader.c shaders/shader.h shaders/shaderMgr.c shaders/shaderMgr.h textures/texture.c textures/texture.h structs.c structs.h graphics/gmath.c graphics/gmath.h graphics/camera.c graphics/camera.h graphics/model.c graphics/model.h)

target_sources(Zomboid3D PUBLIC ${oilSources})
target_sources(Zomboid3D PUBLIC ${oilHeaders})

target_link_libraries(Zomboid3D X11 GL z m)