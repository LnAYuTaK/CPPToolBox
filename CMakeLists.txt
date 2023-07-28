cmake_minimum_required(VERSION 3.2)
#项目名称
project(ZD903Q)
MESSAGE(STATUS "ZD903Q")
#设置交叉编译器
#交叉编译的系统名称
set(CMAKE_SYSTEM_NAME Linux)
# 交叉编译的CPU架构
set(CMAKE_SYSTEM_PROCESSOR aarch64)
#指明C和C++编译器
set(CMAKE_C_COMPILER aarch64-linux-gcc)
set(CMAKE_CXX_COMPILER aarch64-linux-g++)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
#设置使用是否使用动态库
set(BUILD_USE_STATIC   ON CACHE BOOL "Build Use static library")

message(STATUS "CMAKE_C_COMPILER:" ${CMAKE_C_COMPILER})
message(STATUS "CMAKE_CXX_COMPILER:" ${CMAKE_CXX_COMPILER})

# 第三方所有的头文件路径
include_directories(/opt/host/include)
# 第三方所有的库文件路径
link_directories(/opt/host/lib) 

include_directories(src/base)
include_directories(src/device)
include_directories(src/app)
include_directories(src/db)
include_directories(src/net)
include_directories(src/event)
include_directories(src/terminal)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)	   # 设置库文件的输出目录

#添加编译选项
add_definitions(-O2 -g -Wpsabi -fno-aggressive-loop-optimizations)
add_definitions(-nostartfiles)
add_definitions(-std=c++17)

#子目录编译
add_subdirectory(${PROJECT_SOURCE_DIR}/src/base)   
add_subdirectory(${PROJECT_SOURCE_DIR}/src/event)   
add_subdirectory(${PROJECT_SOURCE_DIR}/src/device)   
add_subdirectory(${PROJECT_SOURCE_DIR}/src/net)    
add_subdirectory(${PROJECT_SOURCE_DIR}/src/db)
add_subdirectory(${PROJECT_SOURCE_DIR}/src/terminal)
#应用程序编译
add_subdirectory(${PROJECT_SOURCE_DIR}/src/app) 

