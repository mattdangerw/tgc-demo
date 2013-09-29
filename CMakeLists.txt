cmake_minimum_required(VERSION 2.7)

project(tgc-demo)

find_package(OpenGL REQUIRED)

add_definitions(-DGLEW_STATIC)

add_subdirectory(libs)

include_directories(src)
include_directories(libs)
include_directories(libs/glew/include)
include_directories(libs/glfw/include)
include_directories(libs/freetype/include)

add_executable(tgc-demo
  src/main.cpp
  src/game.h
  src/game.cpp
  src/world/world.h
  src/world/world.cpp
  src/world/clouds.cpp
  src/world/clouds.h
  src/world/ground.h
  src/world/ground.cpp
  src/world/background.h
  src/world/background.cpp
  src/world/character.h
  src/world/character.cpp
  src/world/scroller.h
  src/world/scroller.cpp
  src/world/event_manager.h
  src/world/event_manager.cpp
  src/world/birds.h
  src/world/birds.cpp
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

target_link_libraries(tgc-demo glew glfw freetype ${OPENGL_LIBRARIES})
