TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += src/main.cpp \
    src/fluid-sim/fluidsimulator.cpp \
    src/fluid-sim/fluidsystem.cpp \
    src/graphics/shader.cpp \
    src/graphics/interface.cpp \
    src/graphics/texture.cpp \
    src/graphics/resourcemanager.cpp

HEADERS += \
    src/fluid-sim/fluidsimulator.h \
    src/fluid-sim/fluidsystem.h \
    src/fluid-sim/math.h \
    src/graphics/canvas.h \
    src/graphics/interface.h \
    src/graphics/texture.h \
    src/graphics/resourcemanager.h

DISTFILES += \
    src/graphics/canvas.frag \
    src/graphics/canvas.vert

shaders.path = $${OUT_PWD}/shaders
shaders.files += \
    src/graphics/canvas.frag \
    src/graphics/canvas.vert
INSTALLS += shaders

LIBS += -lSOIL -lGL -lGLEW -lglfw

INCLUDEPATH += /usr/include/eigen3 \
    /usr/include/SOIL
