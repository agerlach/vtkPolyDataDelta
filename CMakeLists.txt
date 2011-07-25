cmake_minimum_required(VERSION 2.6)

PROJECT(vtkPolyDataDelta)
ENABLE_TESTING()

FIND_PACKAGE(VTK REQUIRED)
INCLUDE(${VTK_USE_FILE})

ADD_EXECUTABLE(vtkPolyDataDeltaExample vtkPolyDataDeltaExample.cxx vtkPolyDataDelta.cxx)
TARGET_LINK_LIBRARIES(vtkPolyDataDeltaExample vtkHybrid )

ADD_EXECUTABLE(vtkPolyDataDeltaDemo vtkPolyDataDeltaDemo.cxx vtkPolyDataDelta.cxx)
TARGET_LINK_LIBRARIES(vtkPolyDataDeltaDemo vtkHybrid )
