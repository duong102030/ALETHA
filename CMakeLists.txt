cmake_minimum_required(VERSION 3.20.0)

list(APPEND ZEPHYR_EXTRA_MODULES "${CMAKE_CURRENT_SOURCE_DIR}/drivers")

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(test_nau7802)

target_sources(app PRIVATE 
                src/main.c
                src/loadcell.c)

zephyr_library_include_directories(.)

include_directories(${CMAKE_SOURCE_DIR}/src)
