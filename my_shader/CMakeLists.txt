cmake_minimum_required(VERSION 3.2)
project(my_shader)

set(CMAKE_VERBOSE_MAKEFILE OFF)

include_directories(
    ${CMAKE_CURRENT_LIST_DIR}/include
    ${CMAKE_CURRENT_LIST_DIR}/src/shader
)

aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/src DIR_SRCS_1)
aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/src/shader DIR_SRCS_2)

# glew的库目录路径
link_directories(/opt/homebrew/Cellar//glew/2.2.0_1/lib)
# glfw的库目录路径
link_directories(/opt/homebrew/Cellar/glfw/3.3.7/lib)

add_executable(${PROJECT_NAME} main.cpp ${DIR_SRCS_1} ${DIR_SRCS_2})

# 指定glew库
target_link_libraries(${PROJECT_NAME} GLEW)
# 指定glfw库
target_link_libraries(${PROJECT_NAME} GLFW)

set(CMAKE_CXX_FLAGS "-g -std=c++11 -Wformat")
