cmake_minimum_required(VERSION 3.2)
#项目名称
project(cpp-tools-box)
MESSAGE(STATUS "cpp-tools-box")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

#设置使用是否使用动态库
set(BUILD_USE_STATIC   ON CACHE BOOL "Build Use static library")
# 设定INSTALL文件夹

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

message(STATUS "CMAKE_C_COMPILER:" ${CMAKE_C_COMPILER})
message(STATUS "CMAKE_CXX_COMPILER:" ${CMAKE_CXX_COMPILER})

# # 第三方所有的头文件路径
# include_directories(/opt/host/include)
# # 第三方所有的库文件路径
# link_directories(/opt/host/lib) 

include_directories(${PROJECT_SOURCE_DIR})
# include_directories(src/terminal)
#设置二进制文件路径
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
#  # 设置库文件的输出目录
# set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)	   
#添加编译选项
add_definitions(-O2 -g -Wpsabi -fno-aggressive-loop-optimizations)
add_definitions(-nostartfiles)
add_definitions(-std=c++17)
#子目录编译
add_subdirectory(gwcpp/base)   
add_subdirectory(gwcpp/event)   
add_subdirectory(gwcpp/device)   
add_subdirectory(gwcpp/net)    
# add_subdirectory(${PROJECT_SOURCE_DIR}/src/db)

#应用程序编译
# add_subdirectory(${PROJECT_SOURCE_DIR}/src/app) 
#头文件安装
