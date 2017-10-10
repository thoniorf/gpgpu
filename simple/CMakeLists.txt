cmake_minimum_required(VERSION 2.8)

project(simple)

set(GLAD "./glad.c")
find_package(OpenGL REQUIRED)
set(GL_LIBRARY GL X11 pthread Xrandr Xi m dl)

add_executable(simple simple.cpp ${GLAD})

target_link_libraries(simple glfw3 ${GL_LIBRARY})
