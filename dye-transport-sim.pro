TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += src/main.cpp \
    src/fluid-sim/fluidsystem.cpp \
    src/graphics/shader.cpp \
    src/graphics/canvas.cpp \
    src/graphics/interface.cpp \
    src/graphics/resourcemanager.cpp \
    src/graphics/fluidtexture.cpp

HEADERS += \
    src/fluid-sim/fluidsystem.h \
    src/fluid-sim/math.h \
    src/graphics/shader.h \
    src/graphics/canvas.h \
    src/graphics/interface.h \
    src/graphics/resourcemanager.h \
    src/graphics/fluidtexture.h

DISTFILES += \
    src/graphics/canvas.frag \
    src/graphics/canvas.vert

shaders.path = $${OUT_PWD}/shaders
shaders.files += \
    src/graphics/canvas.frag \
    src/graphics/canvas.vert
INSTALLS += shaders

LIBS += -lGL -lGLEW -lglfw

INCLUDEPATH += /usr/include/eigen3 \
    /usr/include/SOIL
