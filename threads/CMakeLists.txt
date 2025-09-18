cmake_minimum_required(VERSION 3.10)
project(MeshNavigation)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW REQUIRED glfw3)

include_directories(${OPENGL_INCLUDE_DIRS} ${GLAD_INCLUDE_DIRS} ${GLFW_INCLUDE_DIRS} /usr/include)

add_executable(MeshNavigation main.cpp)
target_link_libraries(MeshNavigation ${GLAD_LIBRARIES} ${OPENGL_LIBRARIES} ${GLFW_LIBRARIES} dl pthread X11 Xrandr Xi Xxf86vm Xcursor)
