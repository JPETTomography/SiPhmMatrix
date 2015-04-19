include("../qt_build.pri")
QMAKE_CXXFLAGS+= -std=c++11
INCLUDEPATH += ..
LIBS += -L$$PWD/../LongScintillator -lscintillator
LIBS += -L$$PWD/../Model_routines -lmodel_routines
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
TARGET = ModelScin_coord_2_app
TEMPLATE = app
SOURCES += model_scin_coords_2.cpp
