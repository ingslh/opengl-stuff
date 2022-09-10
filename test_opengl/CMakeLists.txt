cmake_minimum_required(VERSION 3.2)
project(test_opengl)

set(CMAKE_VERBOSE_MAKEFILE OFF)

# glfw和glew的头文件目录，这是我的路径，你可以换成你自己的
INCLUDE_DIRECTORIES(
  include
)

AUX_SOURCE_DIRECTORY(src DIR_SRCS)

set(CMAKE_CXX_FLAGS "-g -std=c++11 -Wformat")

# glew的库目录路径
link_directories(/opt/homebrew/Cellar//glew/2.2.0_1/lib)
# glfw的库目录路径
link_directories(/opt/homebrew/Cellar/glfw/3.3.7/lib)
add_executable(test_opengl ${DIR_SRCS} ${HDRS})

# 指定glew库
target_link_libraries(test_opengl GLEW)
# 指定glfw库
target_link_libraries(test_opengl GLFW)