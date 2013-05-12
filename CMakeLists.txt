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
  src/game.h
  src/game.cpp
  src/game/clouds.cpp
  src/game/clouds.h
  src/game/state.h
  src/game/state.cpp
  src/game/ground.h
  src/game/ground.cpp
  src/game/background.h
  src/game/background.cpp
  src/game/character.h
  src/game/character.cpp
  src/game/scroller.h
  src/game/scroller.cpp
  src/game/event_manager.h
  src/game/event_manager.cpp
  src/game/birds.h
  src/game/birds.cpp
  src/engine/animator.h
  src/engine/animator.cpp
  src/engine/circles.cpp
  src/engine/circles.h
  src/engine/entity.h
  src/engine/entity.cpp
  src/engine/particle_system.cpp
  src/engine/particle_system.h
  src/engine/path_shape.cpp
  src/engine/path_shape.h
  src/engine/shape_group.cpp
  src/engine/shape_group.h
  src/engine/quad.cpp
  src/engine/quad.h
  src/engine/scene_node.cpp
  src/engine/scene_node.h
  src/engine/engine.cpp
  src/engine/engine.h
  src/engine/shader_program.cpp
  src/engine/shader_program.h
  src/engine/updater.h
  src/engine/updater.cpp
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

target_link_libraries(tgc-demo glew glfw ${OPENGL_LIBRARIES})