cmake_minimum_required(VERSION 3.0.2)

PROJECT("AspiaZ")
set(CMAKE_CXX_STANDARD 11)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Builds/$<CONFIG>/${CMAKE_VS_PLATFORM_NAME})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Builds/$<CONFIG>/${CMAKE_VS_PLATFORM_NAME})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Builds/$<CONFIG>/${CMAKE_VS_PLATFORM_NAME})

if(MSVC)   
   link_directories(";$(OutDir)")
endif(MSVC)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(ASBase)
add_subdirectory(ASCommon)
add_subdirectory(ASServer)
add_subdirectory(ASVideoCodec)

# gui
add_subdirectory(ASClient)

