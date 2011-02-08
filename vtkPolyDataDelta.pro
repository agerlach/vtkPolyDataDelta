#-------------------------------------------------
#
# Project created by QtCreator 2011-02-08T08:41:18
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = vtkPolyDataDelta
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp

win32|win64{
     debug {
         INCLUDEPATH += E:/VTK/VTK-5.6.0/VTK_x64_shared/VTK_debug/include/vtk-5.6
         LIBS +=-LE:/VTK/VTK-5.6.0/VTK_x64_shared/VTK_debug/lib/vtk-5.6
     }

    release {
         INCLUDEPATH += E:/VTK/VTK-5.6.0/VTK_x64_shared/VTK_release/include/vtk-5.6
         LIBS +=-LE:/VTK/VTK-5.6.0/VTK_x64_shared/VTK_release/lib/vtk-5.6
    }
}

LIBS += -lvtkParallel \
    -lvtkInfovis \
    -lvtkCommon \
    -lvtksys \
    -lQVTK \
    -lvtkQtChart \
    -lvtkViews \
    -lvtkWidgets \
    -lvtkInfovis \
    -lvtkRendering \
    -lvtkGraphics \
    -lvtkImaging \
    -lvtkIO \
    -lvtkFiltering \
    -lvtklibxml2 \
    -lvtkDICOMParser \
    -lvtkpng \
    -lvtkpng \
    -lvtktiff \
    -lvtkzlib \
    -lvtkjpeg \
    -lvtkalglib \
    -lvtkexpat \
    -lvtkverdict \
    -lvtkmetaio \
    -lvtkNetCDF \
   # -lvtksqlite \
    -lvtkexoIIc \
    -lvtkftgl \
    -lvtkfreetype \
    -lvtkHybrid \

HEADERS +=
