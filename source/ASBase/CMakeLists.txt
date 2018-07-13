cmake_minimum_required(VERSION 3.0.2)

project(ASBase)

FILE(GLOB_RECURSE SRC_Base "*.cpp" "*.h")

if (UNIX)
    add_compile_options(-fPIC)
endif(UNIX)

add_library(ASBase STATIC ${SRC_Base})

