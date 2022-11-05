cmake_minimum_required(VERSION 3.2)

if(APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.9" CACHE STRING "Minimum OS X deployment version")
endif()

project(render_grap)

set(CMAKE_CXX_STANDARD 17)

set(RENDER_GRAP
	src/earcut.hpp
	src/json_data_manager.cpp
	src/json_data_manager.h
	src/main.cpp
	src/fillinfo.cpp
	src/fillinfo.h
	src/groupcontents.h
	src/groupcontents.cpp
	src/jsonreader.h
	src/jsonreader.cpp
	src/layersinfo.h
	src/layersinfo.cpp
	src/pathinfo.h
	src/pathinfo.cpp
	src/base_render_data.hpp
	src/color_render_data.cpp
	src/color_render_data.hpp
	src/bezier_generator.hpp
	src/earcut.hpp
	src/polygon.h
	src/polygon.cpp
	src/render_content.hpp
	src/render_content.cpp
	src/render_data_factory.h
	src/render_data_factory.cpp
	src/shapegroup.h
	src/shapegroup.cpp
	src/transform_render_data.hpp
	src/transform_render_data.cpp
	src/transform.h
	src/transform.cpp
	src/vertices_render_data.hpp
	src/vertices_render_data.cpp
)

set(SHADER_FILES
	shader_bezier.frag
	shader_bezier.geom
	shader_bezier.vert
)

source_group(shader_files FILES ${SHADER_FILES})
source_group(render_grap FILES ${RENDER_GRAP})

set(CMAKE_INCLUDE_CURRENT_DIR ON) 

if(WIN32)
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
  set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF /SUBSYSTEM:WINDOWS /entry:mainCRTStartup")
endif()

if(UNIX)
  set(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)
  set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
  if(NOT APPLE)
    set(CMAKE_INSTALL_RPATH "$ORIGIN")
  else()
    #cmake_policy(SET CMP0068 OLD)
    set(CMAKE_MACOSX_RPATH ON)
    set(CMAKE_INSTALL_RPATH "@executable_path")
    #set(CMAKE_INSTALL_NAME_DIR "@loader_path/../lib")
  endif()
endif()

if(CMAKE_SIZEOF_VOID_P MATCHES 8)
	set(PROJECT_ARCH "x64")
elseif(CMAKE_SIZEOF_VOID_P MATCHES 4)
	set(PROJECT_ARCH "Win32")
endif()

if(APPLE)
  set(CONFIG_TYPE $(CONFIGURATION))
else()
  set(CONFIG_TYPE ${CMAKE_CFG_INTDIR})
endif()
set(TARGET_TRIPLE_CONFIG ${CMAKE_SYSTEM_NAME}-${PROJECT_ARCH}-${CONFIG_TYPE})	

set(INCLUDE_DIRECTORIES 
	${CMAKE_CURRENT_LIST_DIR}/../includes 
	src
)

set(LIBS_DIRECTORIES
	${CMAKE_SOURCE_DIR}/../lib
)

add_library(GLAD ${CMAKE_SOURCE_DIR}/../includes/glad/glad.c)

if(WIN32)
	link_directories(${LIBS_DIRECTORIES})
else()	
	link_directories(/opt/homebrew/Cellar//glew/2.2.0_1/lib)
	link_directories(/opt/homebrew/Cellar/glfw/3.3.7/lib)
endif()

add_executable(${PROJECT_NAME} ${SHADER_FILES} ${RENDER_GRAP})

#set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS  "/MANIFESTUAC:\"level='requireAdministrator' uiAccess='false'\"")

target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIRECTORIES})
target_compile_definitions(${PROJECT_NAME} PRIVATE GLEW_STATIC)

if(WIN32)
  target_compile_definitions(${PROJECT_NAME} PUBLIC _CRT_SECURE_NO_DEPRECATE _CRT_SECURE_NO_WARNINGS)
else()
  target_compile_options(${PROJECT_NAME} PUBLIC -std=c++17 -msse4)
  target_compile_definitions(${PROJECT_NAME} PUBLIC _FILE_OFFSET_BITS=64)
  target_compile_definitions(${PROJECT_NAME} PRIVATE USING_OPENGL_RENDER=1)
endif()

if(WIN32)
	target_link_libraries(${PROJECT_NAME} glfw3 GLAD)
else()
	target_link_libraries(${PROJECT_NAME} GLFW GLAD)
endif()



include_directories(
    ${CMAKE_CURRENT_LIST_DIR}/../includes
)

#AUX_SOURCE_DIRECTORY(src DIR_SRCS)

#list(APPEND CMAKE_CXX_FLAGS "-std=c++17")

#add_executable(${PROJECT_NAME} ${DIR_SRCS})


if(MSVC)    
  add_definitions(/W2 /GR)
else()
  add_definitions(-fexceptions -frtti)
endif()