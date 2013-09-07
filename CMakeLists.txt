cmake_minimum_required(VERSION 2.7)

project(tgc-demo)

find_package(OpenGL REQUIRED)

add_definitions(-DGLEW_STATIC)

add_subdirectory(libs)

include_directories(src)
include_directories(libs)
include_directories(libs/glew/include)
include_directories(libs/glfw/include)

add_executable(tgc-demo
  src/main.cpp
  src/game.cpp
  src/world/world.cpp
  src/world/clouds.cpp
  src/world/ground.cpp
  src/world/background.cpp
  src/world/character.cpp
  src/world/scroller.cpp
  src/world/event_manager.cpp
  src/world/birds.cpp
  src/engine/engine.cpp
  src/engine/animator.cpp
  src/engine/circles.cpp
  src/engine/particle_system.cpp
  src/engine/shape.cpp
  src/engine/shape_group.cpp
  src/engine/fill.cpp
  src/engine/entity.cpp
  src/engine/shader_program.cpp
  src/util/settings.cpp
  src/util/error.cpp
  src/util/read_file.cpp
  src/util/json.c
  )

target_link_libraries(tgc-demo glew glfw ${OPENGL_LIBRARIES})
