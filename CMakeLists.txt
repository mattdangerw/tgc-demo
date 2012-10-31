cmake_minimum_required(VERSION 2.7)

project(tgc-demo)

find_package(OpenGL REQUIRED)

add_definitions(-DGLEW_STATIC)

add_subdirectory(libs)

include_directories(libs)
include_directories(libs/glew/include)
include_directories(libs/glfw/include)

add_executable(tgc-demo
  src/character.cpp
  src/character.h
  src/circles.cpp
  src/circles.h
  src/clouds.cpp
  src/clouds.h
  src/error.h
  src/error.cpp
  src/game.cpp
  src/game.h
  src/game_entity.h
  src/ground.cpp
  src/ground.h
  src/jumping_crowd.cpp
  src/jumping_crowd.h
  src/main.cpp
  src/particle.cpp
  src/particle.h
  src/particle_system.cpp
  src/particle_system.h
  src/path_shape.cpp
  src/path_shape.h
  src/point_mass.cpp
  src/point_mass.h
  src/quad.cpp
  src/quad.h
  src/random.h
  src/renderer.cpp
  src/renderer.h
  src/shader_program.cpp
  src/shader_program.h
  src/thought_bubble.cpp
  src/thought_bubble.h
  src/transform2D.h
  src/tree.cpp
  src/tree.h
  src/triggerables.cpp
  src/triggerables.h
  )

target_link_libraries(tgc-demo glew glfw ${OPENGL_LIBRARIES})