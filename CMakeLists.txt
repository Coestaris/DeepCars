cmake_minimum_required(VERSION 3.14)
project(DeepCars C)

set(CMAKE_C_STANDARD 99)

FILE(GLOB oilSources oil/*.c)
FILE(GLOB oilHeaders oil/*.h)

add_executable(DeepCars main.c lib/graphics/rendering/graphics.c lib/graphics/rendering/graphics.h lib/graphics/win.h lib/graphics/win.c lib/updater.c lib/updater.h lib/shaders/shader.c lib/shaders/shader.h lib/shaders/shm.c lib/shaders/shm.h lib/resources/texture.c lib/resources/texture.h lib/structs.c lib/structs.h lib/graphics/gmath.c lib/graphics/gmath.h lib/graphics/camera.c lib/graphics/camera.h lib/resources/model.c lib/resources/model.h lib/graphics/material.c lib/graphics/material.h lib/object.c lib/object.h app/loader.c app/loader.h app/win_defaults.h lib/scene.c lib/scene.h app/objects/obj_dummy.c app/objects/obj_dummy.h app/objects/obj_camera_control.c app/objects/obj_camera_control.h lib/object_include.h lib/scm.c lib/scm.h lib/graphics/light.c lib/graphics/light.h lib/resources/pack.c lib/resources/pack.h lib/graphics/rendering/render_chain.c lib/graphics/rendering/render_chain.h lib/graphics/rendering/render_stage.c lib/graphics/rendering/render_stage.h app/renderer.c app/renderer.h lib/resources/rmanager.c lib/resources/rmanager.h app/objects/scene.c)

target_sources(DeepCars PUBLIC ${oilSources})
target_sources(DeepCars PUBLIC ${oilHeaders})

target_link_libraries(DeepCars X11 GL z m)

# For gcov
#target_compile_options(DeepCars PRIVATE "-fprofile-arcs")
#target_compile_options(DeepCars PRIVATE "-ftest-coverage")
#target_compile_options(DeepCars PRIVATE "--coverage")
#target_link_libraries(DeepCars gcov)


#add_custom_command(TARGET DeepCars POST_BUILD
#        COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/resources/packer.py
#        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/resources/
#        COMMENT "Packing resources..."
#)