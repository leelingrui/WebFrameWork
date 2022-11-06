# CMakeList.txt: 顶层 CMake 项目文件，在此处执行全局配置
# 并包含子项目。
#

cmake_minimum_required (VERSION 3.8)

project ("WebFrameWork")
# 包含子项目。
include_directories(${PROJECT_SOURCE_DIR}/WebFrameWork/include/)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if(WIN32)
	add_definitions(-DWIN32)
endif()
set(SRC_LIST)

file(GLOB_RECURSE SRC_LIST ${PROJECT_SOURCE_DIR}/WebFrameWork/src/ "${PROJECT_SOURCE_DIR}/WebFrameWork/src/*.cpp")
#message(STATUS "源文件目录 ${PROJECT_SOURCE_DIR}/WebFrameWork/src")
message(STATUS "源文件 ${SRC_LIST}")
add_executable(TESTPROJ "Test.cpp" ${SRC_LIST})
