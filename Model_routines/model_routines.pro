include("../qt_build.pri")
include("../lighting.pri")
QMAKE_CXXFLAGS+= -std=c++11
INCLUDEPATH += ..
#root
LIBS += -L$$ROOTDEPENDPATH -lCore
LIBS += -L$$ROOTDEPENDPATH -lGui
LIBS += -L$$ROOTDEPENDPATH -lGpad
LIBS += -L$$ROOTDEPENDPATH -lGraf
LIBS += -L$$ROOTDEPENDPATH -lHist
INCLUDEPATH += $$ROOTINCLUDEPATH
DEPENDPATH += $$ROOTDEPENDPATH
QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TARGET = model_routines
TEMPLATE = lib
SOURCES += displaygraph.cpp funcfromfile.cpp ModelObjects.cpp
HEADERS += displaygraph.h funcfromfile.h ModelObjects.h display_root_object.h
