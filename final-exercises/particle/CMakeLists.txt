project(particle)
cmake_minimum_required(VERSION 2.8)

find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
pkg_search_module(ASSIMP REQUIRED assimp)
find_package(glm REQUIRED)
find_package(OpenGL REQUIRED)

set(GL_LIBRARY
    GL X11
    pthread
    Xrandr
    Xi
    m
    dl
    )
file(GLOB MODEL_LIST
    models/*
    )

include_directories(
  ${GLFW_INCLUDE_DIRS}
  ${OPENGL_INCLUDE_DIRS}
  ${MODEL_LIST}
  )

aux_source_directory(. SRC_LIST)

file(GLOB RES_LIST
    include/*
    shaders/*
    )

add_executable(
    ${PROJECT_NAME}
    ${SRC_LIST}
    ${glad_c}
    ${RES_LIST}
    )

target_link_libraries(
    ${PROJECT_NAME}
    ${GLFW_LIBRARIES}
    ${ASSIMP_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${GL_LIBRARY}
    libSOIL.a
    )
