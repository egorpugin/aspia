cmake_minimum_required(VERSION 3.0.2)

project(ASVideoCodec)

FILE(GLOB_RECURSE SRC_SVWr "*.cpp" "*.h")

if (UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif(UNIX)

add_executable(ASVideoCodec ${SRC_SVWr})

if(MSVC) 
   set_target_properties (ASVideoCodec PROPERTIES LINK_FLAGS "/NODEFAULTLIB:LIBCMT")
endif(MSVC)

target_link_libraries(ASVideoCodec ASCommon ASBase)