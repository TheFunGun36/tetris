cmake_minimum_required(VERSION 3.23)
project(tetris)

set(CMAKE_CXX_STANDARD 20)

add_executable(tetris main.cpp Game.cpp Game.h Map.cpp Map.h Position.cpp Position.h Shape.cpp Shape.h)
