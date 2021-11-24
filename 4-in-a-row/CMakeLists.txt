cmake_minimum_required(VERSION 3.20)
project(hsnr_wsy_4inarow)

set(CMAKE_CXX_STANDARD 20)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

include_directories(.)

add_executable(hsnr_wsy_4inarow
        game.h
        game.cpp
        main.cpp
        )
