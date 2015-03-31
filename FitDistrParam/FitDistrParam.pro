INCLUDEPATH +=../FitGen
QMAKE_CXXFLAGS+= -std=c++11
LIBS += -L$$PWD/../FitGen -lFitGen
TARGET = fitdistrparam_app
TEMPLATE = app
SOURCES += fitdistrparam.cpp
