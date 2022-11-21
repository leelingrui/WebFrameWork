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
SET(CMAKE_ASM_MASM_SOURCE_FILE_EXTENSIONS asm)
ENABLE_LANGUAGE(ASM_MASM)
SET(CMAKE_ASM_MASM_FLAGS "-g")
file(GLOB_RECURSE SRC_LIST ${PROJECT_SOURCE_DIR}/WebFrameWork/src/ "${PROJECT_SOURCE_DIR}/WebFrameWork/src/*.cpp")
#message(STATUS "源文件目录 ${PROJECT_SOURCE_DIR}/WebFrameWork/src")
message(STATUS "源文件 ${SRC_LIST}")
add_executable(TESTPROJ ${SRC_LIST} "test.asm" "Test.cpp")
