cmake_minimum_required(VERSION 2.6)

PROJECT(vtkPolyDataDelta)
ENABLE_TESTING()

FIND_PACKAGE(VTK REQUIRED)
INCLUDE(${VTK_USE_FILE})

# Mesh difference with normals
ADD_EXECUTABLE(vtkPolyDataDeltaExample vtkPolyDataDeltaExample.cxx vtkPolyDataDelta.cxx)
TARGET_LINK_LIBRARIES(vtkPolyDataDeltaExample vtkHybrid )

ADD_EXECUTABLE(vtkPolyDataDeltaDemo vtkPolyDataDeltaDemo.cxx vtkPolyDataDelta.cxx)
TARGET_LINK_LIBRARIES(vtkPolyDataDeltaDemo vtkHybrid )

# Mesh difference without normals
ADD_EXECUTABLE(vtkMeshPointDifferenceExample vtkMeshPointDifferenceExample.cxx vtkMeshPointDifference.cxx)
TARGET_LINK_LIBRARIES(vtkMeshPointDifferenceExample vtkHybrid )

ADD_EXECUTABLE(vtkMeshPointDifferenceDemo vtkMeshPointDifferenceDemo.cxx vtkMeshPointDifference.cxx)
TARGET_LINK_LIBRARIES(vtkMeshPointDifferenceDemo vtkHybrid )
