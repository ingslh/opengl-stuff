cmake_minimum_required(VERSION 3.2)
project(render_grap)

set(CMAKE_VERBOSE_MAKEFILE OFF)

include_directories(
    ${CMAKE_CURRENT_LIST_DIR}/../includes
)

AUX_SOURCE_DIRECTORY(src DIR_SRCS)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall  -O3 -march=native -std=c++11 -static-libgcc")
list(APPEND CMAKE_CXX_FLAGS "-std=c++11")


if(WIN32)
	link_directories(${CMAKE_SOURCE_DIR}/../lib)
else()	
	# glew的库目录路径
	link_directories(/opt/homebrew/Cellar//glew/2.2.0_1/lib)
	# glfw的库目录路径
	link_directories(/opt/homebrew/Cellar/glfw/3.3.7/lib)
endif(WIN32)

#macro(makeLink src dest target)
#  add_custom_command(TARGET ${target} POST_BUILD COMMAND ${CMAKE_COMMAND} -E create_symlink ${src} ${dest}  DEPENDS  ${dest} COMMENT "mklink ${src} -> ${dest}")
#endmacro()

add_executable(${PROJECT_NAME} ${DIR_SRCS})

# 指定glew库
#target_link_libraries(${PROJECT_NAME} GLEW)
# 指定glfw库
#target_link_libraries(${PROJECT_NAME} GLFW)
if(WIN32)
	target_link_libraries(${PROJECT_NAME} glfw3)
else()
	target_link_libraries(${PROJECT_NAME} GLFW)
endif(WIN32)

if(MSVC)
    configure_file(${CMAKE_SOURCE_DIR}/configuration/visualstudio.vcxproj.user.in ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.vcxproj.user @ONLY)
endif(MSVC)