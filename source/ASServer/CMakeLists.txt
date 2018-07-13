cmake_minimum_required(VERSION 3.0.2)

project(ASServer)

FILE(GLOB_RECURSE SRC_SVWr "*.cpp" "*.h")

if (UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif(UNIX)

add_executable(ASServer ${SRC_Srv})

if(MSVC) 
   set_target_properties (ASServer PROPERTIES LINK_FLAGS "/NODEFAULTLIB:LIBCMT")
endif(MSVC)

target_link_libraries(ASServer ASCommon ASBase)