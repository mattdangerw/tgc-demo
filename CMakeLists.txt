cmake_minimum_required(VERSION 2.7)

project(tgc-demo)

find_package(OpenGL REQUIRED)

add_definitions(-DGLEW_STATIC)
set(GLFW_INSTALL OFF)

add_subdirectory(libs)

include_directories(src)
include_directories(libs)
include_directories(libs/glew/include)
include_directories(libs/glfw/include)
include_directories(libs/freetype/include)

add_library(engine
  src/engine/engine.h
  src/engine/engine.cpp
  src/engine/animator.h
  src/engine/animator.cpp
  src/engine/circles.cpp
  src/engine/circles.h
  src/engine/text.cpp
  src/engine/text.h
  src/engine/particle_system.cpp
  src/engine/particle_system.h
  src/engine/shape.cpp
  src/engine/shape.h
  src/engine/shape_group.cpp
  src/engine/shape_group.h
  src/engine/quad.h
  src/engine/fill.cpp
  src/engine/fill.h
  src/engine/entity.cpp
  src/engine/entity.h
  src/engine/shader_program.cpp
  src/engine/shader_program.h
  src/util/settings.h
  src/util/settings.cpp
  src/util/error.h
  src/util/error.cpp
  src/util/random.h
  src/util/transform2D.h
  src/util/read_file.h
  src/util/read_file.cpp
  src/util/json.h
  src/util/json.c
  )

add_executable(render-test
  src/main.cpp
  src/game.h
  src/game.cpp
  src/render-test/world.h
  src/render-test/world.cpp
  src/render-test/clouds.cpp
  src/render-test/clouds.h
  src/render-test/ground.h
  src/render-test/ground.cpp
  src/render-test/background.h
  src/render-test/background.cpp
  src/render-test/character.h
  src/render-test/character.cpp
  src/render-test/scroller.h
  src/render-test/scroller.cpp
  src/render-test/event_manager.h
  src/render-test/event_manager.cpp
  src/render-test/birds.h
  src/render-test/birds.cpp
  )

target_link_libraries(render-test engine glew glfw freetype ${OPENGL_LIBRARIES} ${GLFW_LIBRARIES})
