TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += src/main.cpp \
    src/fluid-sim/fluidsimulator.cpp \
    src/fluid-sim/fluidsystem.cpp

HEADERS += \
    src/fluid-sim/fluidsimulator.h \
    src/fluid-sim/fluidsystem.h \
    src/fluid-sim/math.h

LIBS += -lGL -lGLU -lglut

INCLUDEPATH += /usr/include/eigen3
