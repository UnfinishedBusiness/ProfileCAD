cmake_minimum_required(VERSION 2.8)
# Project Name
PROJECT(ProfileCAD)

add_definitions("-std=c++11 -g")
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_definitions("-w -I/opt/X11/include")
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/../cmake")

set(CMAKE_BUILD_TYPE Debug)
#########################################################
# FIND X11
#########################################################
FIND_PACKAGE ( X11 REQUIRED )
IF ( X11_FOUND )
      INCLUDE_DIRECTORIES ( ${X11_INCLUDE_DIR} )
      LINK_LIBRARIES ( ${X11_LIBRARIES} )
      MESSAGE ( STATUS " X11_INCLUDE_DIR: " ${X11_INCLUDE_DIR} )
      MESSAGE ( STATUS " X11_LIBRARIES: " ${X11_LIBRARIES} )
ENDIF ( X11_FOUND )
#########################################################
# FIND GLUT
#########################################################
find_package(GLUT REQUIRED)
include_directories(${GLUT_INCLUDE_DIRS})
link_directories(${GLUT_LIBRARY_DIRS})
add_definitions(${GLUT_DEFINITIONS})
if(NOT GLUT_FOUND)
    message(ERROR " GLUT not found!")
endif(NOT GLUT_FOUND)
#########################################################
# FIND OPENGL
#########################################################
find_package(OpenGL REQUIRED)
include_directories(${OpenGL_INCLUDE_DIRS})
link_directories(${OpenGL_LIBRARY_DIRS})
add_definitions(${OpenGL_DEFINITIONS})
if(NOT OPENGL_FOUND)
    message(ERROR " OPENGL not found!")
endif(NOT OPENGL_FOUND)
#########################################################
# FIND LUA
#########################################################
find_package(Lua REQUIRED)
include_directories(${Lua_INCLUDE_DIRS})
link_directories(${Lua_LIBRARY_DIRS}
)
add_definitions(${Lua_DEFINITIONS})
if(NOT LUA_FOUND)
    message(ERROR " Lua not found!")
endif(NOT LUA_FOUND)
#########################################################
# FIND PNG
#########################################################
find_package(PNG REQUIRED)
include_directories(${PNG_INCLUDE_DIRS})
link_directories(${PNG_LIBRARY_DIRS})
add_definitions(${PNG_DEFINITIONS})
if(NOT PNG_FOUND)
    message(ERROR " PNG not found!")
endif(NOT PNG_FOUND)
#########################################################
# FIND GTK
#########################################################
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK2 REQUIRED gtk+-2.0)
include_directories(${GTK2_INCLUDE_DIRS})
link_directories(${GTK2_LIBRARY_DIRS})
# Add other flags to the compiler
add_definitions(${GTK2_CFLAGS_OTHER})
#########################################################
# Include Files
#########################################################
add_executable(ProfileCAD MACOSX_BUNDLE main Scene Keyboard Machine Menu Window Mouse CAD UI Geometry CLI FileIO Debug Lua)
include_directories(../inc)
########################################################
# Linking & stuff
#########################################################

  # create the program "ProfileCAD"
  target_link_libraries(ProfileCAD ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${LUA_LIBRARY} ${PNG_LIBRARY} ${GTK2_LIBRARIES} ${X11_LIBRARIES} -pthread )
