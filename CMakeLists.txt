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
  src/game_state/clouds.cpp
  src/game_state/clouds.h
  src/game_state/entity.h
  src/game_state/entity.cpp
  src/game_state/state.h
  src/game_state/state.cpp
  src/game_state/updater.h
  src/game_state/updater.cpp
  src/game_state/ground.h
  src/game_state/ground.cpp
  src/game_state/background.h
  src/game_state/background.cpp
  src/game_state/character.h
  src/game_state/character.cpp
  src/game_state/scroller.h
  src/game_state/scroller.cpp
  src/game_state/event_manager.h
  src/game_state/event_manager.cpp
  src/game_state/birds.h
  src/game_state/birds.cpp
  src/render/animator.h
  src/render/animator.cpp
  src/render/circles.cpp
  src/render/circles.h
  src/render/particle_system.cpp
  src/render/particle_system.h
  src/render/path_shape.cpp
  src/render/path_shape.h
  src/render/shape_group.cpp
  src/render/shape_group.h
  src/render/quad.cpp
  src/render/quad.h
  src/render/scene_node.cpp
  src/render/scene_node.h
  src/render/renderer.cpp
  src/render/renderer.h
  src/render/shader_program.cpp
  src/render/shader_program.h
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