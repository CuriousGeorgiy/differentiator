cmake_minimum_required(VERSION 3.17)
project(differentiator)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_definitions(-D_CRT_SECURE_NO_WARNINGS)

add_executable(differentiator)
target_sources(differentiator PUBLIC  Main.cpp BinaryTree.cpp File.cpp Differentiator.cpp)
target_include_directories(differentiator PUBLIC ${PROJECT_SOURCE_DIR})
