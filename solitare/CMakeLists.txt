cmake_minimum_required(VERSION 3.20)
project(hsnr_wsy_solitare)

set(CMAKE_CXX_STANDARD 20)

include_directories(.)

add_executable(hsnr_wsy_solitare
        solitare.cpp)
