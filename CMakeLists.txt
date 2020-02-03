cmake_minimum_required(VERSION 3.14)
project(DeepCars C)

set(CMAKE_C_STANDARD 99)

FILE(GLOB oilSources oil/*.c)
FILE(GLOB oilHeaders oil/*.h)

add_executable(DeepCars main.c lib/graphics/rendering/graphics.c lib/graphics/rendering/graphics.h lib/graphics/win.h lib/graphics/win.c lib/updater.c lib/updater.h lib/resources/shader.c lib/resources/shader.h lib/resources/shm.c lib/resources/shm.h lib/resources/texture.c lib/resources/texture.h lib/structs.c lib/structs.h lib/graphics/gmath.c lib/graphics/gmath.h lib/graphics/camera.c lib/graphics/camera.h lib/resources/model.c lib/resources/model.h lib/graphics/material.c lib/graphics/material.h lib/object.c lib/object.h app/loader.c app/loader.h app/win_defaults.h lib/scene.c lib/scene.h app/objects/obj_dummy.c app/objects/obj_dummy.h app/objects/obj_camera_control.c app/objects/obj_camera_control.h lib/object_include.h lib/scm.c lib/scm.h lib/graphics/light.c lib/graphics/light.h lib/resources/pack.c lib/resources/pack.h lib/graphics/rendering/render_chain.c lib/graphics/rendering/render_chain.h lib/graphics/rendering/render_stage.c lib/graphics/rendering/render_stage.h app/rendering/renderer.c app/rendering/renderer.h lib/resources/rmanager.c lib/resources/rmanager.h app/objects/scene.c sig_handlers.c sig_handlers.h app/rendering/ssao.c app/rendering/ssao.h app/rendering/shader_setup.c app/rendering/shader_setup.h lib/resources/font.c lib/resources/font.h app/objects/obj_info_draw.c app/objects/obj_info_draw.h app/rendering/text_rendering.c app/rendering/text_rendering.h app/rendering/vfx.c app/rendering/vfx.h app/objects/menu/obj_menu_drawer.c app/objects/menu/obj_menu_drawer.h app/objects/menu/obj_menu_camera_mover.c app/objects/menu/obj_menu_camera_mover.h app/objects/obj_default_bind_handler.c app/objects/obj_default_bind_handler.h app/objects/menu/obj_menu_phys.c app/objects/menu/obj_menu_phys.h mllib/genome.c mllib/genome.h mllib/node_genome.c mllib/node_genome.h mllib/connection_genome.c mllib/connection_genome.h oil/font.c oil/font.h)

target_sources(DeepCars PUBLIC ${oilSources})
target_sources(DeepCars PUBLIC ${oilHeaders})

# For OIL
include_directories(/usr/include/freetype2)
target_link_libraries(DeepCars X11 freetype)


target_link_libraries(DeepCars X11 GL z m)
target_link_libraries(DeepCars X11 chipmunk pthread)

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