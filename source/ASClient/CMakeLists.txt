cmake_minimum_required(VERSION 3.0.2)

include(../cmake/qt.cmake)
include(../cmake/mswin.cmake)

project(ASClient)

FILE(GLOB_RECURSE SRC_View "*.cpp" "*.h" "*.ui" "*.qrc" ".rc")

if (UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif(UNIX)

add_executable(ASClient ${SRC_Client})

if(MSVC) 
   set_target_properties (ASClient PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /NODEFAULTLIB:LIBCMT")
   target_link_libraries (ASClient debug qtmaind)
   target_link_libraries (ASClient optimized qtmain)
endif(MSVC)

target_link_libraries(ASClient ASBase ASCommon)

# copy mswin dependent
if(WIN32)
   mswin_getDependDir(DEPEND_WIN_DIR)
   
   add_custom_command(TARGET ASClient PRE_BUILD
       COMMAND ${CMAKE_COMMAND} -E copy_directory ${DEPEND_WIN_DIR}/plugins ${CMAKE_CURRENT_BINARY_DIR}/plugins)

endif(WIN32)

qt5_use_modules(ASClient Core Widgets Gui)