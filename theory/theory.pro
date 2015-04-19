include("../qt_build.pri")
include("../lighting.pri")
QMAKE_CXXFLAGS+= -std=c++11
INCLUDEPATH += ..
LIBS += -L$$PWD/../LongScintillator -lscintillator
LIBS += -L$$PWD/../Model_routines -lmodel_routines
QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TARGET = theory_app
TEMPLATE = app
SOURCES += theorycalc.cpp
