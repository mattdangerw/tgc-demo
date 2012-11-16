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
  src/game_state/character.cpp
  src/game_state/character.h
  src/game_state/clouds.cpp
  src/game_state/clouds.h
  src/game_state/game_entity.h
  src/game_state/ground.cpp
  src/game_state/ground.h
  src/game_state/ideas.cpp
  src/game_state/ideas.h
  src/game_state/jumping_crowd.cpp
  src/game_state/jumping_crowd.h
  src/game_state/point_mass.cpp
  src/game_state/point_mass.h
  src/game_state/thought_bubble.cpp
  src/game_state/thought_bubble.h
  src/game_state/tree.cpp
  src/game_state/tree.h
  src/game_state/triggerables.cpp
  src/game_state/triggerables.h
  src/render/animator.h
  src/render/animator.cpp
  src/render/circles.cpp
  src/render/circles.h
  src/render/particle_system.cpp
  src/render/particle_system.h
  src/render/path_shape.cpp
  src/render/path_shape.h
  src/render/quad.cpp
  src/render/quad.h
  src/render/renderer.cpp
  src/render/renderer.h
  src/render/shader_program.cpp
  src/render/shader_program.h
  src/util/error.h
  src/util/error.cpp
  src/util/random.h
  src/util/transform2D.h
  )

target_link_libraries(tgc-demo glew glfw ${OPENGL_LIBRARIES})