
cmake_minimum_required(VERSION 2.8)

PROJECT(ProfileCAD)

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

add_executable(ProfileCAD MACOSX_BUNDLE ProfileCAD)

if(VTK_LIBRARIES)
  target_link_libraries(ProfileCAD ${VTK_LIBRARIES})
else()
  target_link_libraries(ProfileCAD vtkHybrid vtkWidgets)
endif()
