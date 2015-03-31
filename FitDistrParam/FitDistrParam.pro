INCLUDEPATH +=../FitGen
QMAKE_CXXFLAGS+= -std=c++11
LIBS += -L$$PWD/../FitGen -lFitGen
QT-=core gui
TARGET = fitdistrparam_app
TEMPLATE = app
SOURCES += fitdistrparam.cpp
