cmake_minimum_required(VERSION 3.2)
#项目名称
project(cpp-tools-box C CXX)
MESSAGE(STATUS "cpp-tools-box")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

message(STATUS "CMAKE_C_COMPILER:" ${CMAKE_C_COMPILER})
message(STATUS "CMAKE_CXX_COMPILER:" ${CMAKE_CXX_COMPILER})

include_directories(${PROJECT_SOURCE_DIR})

add_definitions(-O2 -g -Wpsabi -fno-aggressive-loop-optimizations)
add_definitions(-nostartfiles)
add_definitions(-std=c++17)
#子目录编译
add_subdirectory(gwcpp/base)   
add_subdirectory(gwcpp/event)   
add_subdirectory(gwcpp/device)   
add_subdirectory(gwcpp/net)    
