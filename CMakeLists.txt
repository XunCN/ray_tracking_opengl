cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")

project(ray_tracking)

set(app_name "ray_tracking")

include_directories("vendor/glfw/include/")

link_directories("vendor/glfw/lib/")
link_libraries("opengl32" "glfw3_mt.lib")

file(GLOB_RECURSE SRC_LIST ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
add_executable(${app_name} ${SRC_LIST})

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/)

