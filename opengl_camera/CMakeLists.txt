cmake_minimum_required(VERSION 3.2)
project(opengl_camera)

set(CMAKE_VERBOSE_MAKEFILE OFF)

include_directories(
    ${CMAKE_CURRENT_LIST_DIR}/../includes
)

AUX_SOURCE_DIRECTORY(src DIR_SRCS)

set(CMAKE_CXX_FLAGS "-g -std=c++11 -Wformat")

# glew的库目录路径
link_directories(/opt/homebrew/Cellar//glew/2.2.0_1/lib)
# glfw的库目录路径
link_directories(/opt/homebrew/Cellar/glfw/3.3.7/lib)

add_executable(${PROJECT_NAME} ${DIR_SRCS})

# 指定glew库
target_link_libraries(${PROJECT_NAME} GLEW)
# 指定glfw库
target_link_libraries(${PROJECT_NAME} GLFW)